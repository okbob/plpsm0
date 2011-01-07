%{
/*-----------------------------------------------------------------------------------------------
 *
 * a PSM language parser
 *
 *
 *
 *-----------------------------------------------------------------------------------------------
 */

#include "psm.h"

#include "nodes/nodes.h"
#include "nodes/value.h"
#include "parser/scanner.h"
#include "parser/parser.h"
#include "parser/parse_type.h"
#include "utils/lsyscache.h"

/* Location tracking support --- simpler than bison's default */
#define YYLLOC_DEFAULT(Current, Rhs, N) \
	do { \
		if (N) \
			(Current) = (Rhs)[1]; \
		else \
			(Current) = (Rhs)[0]; \
	} while (0)

#define YYMALLOC palloc
#define YYFREE   pfree

union YYSTYPE;

bool	plpsm_debug_parser = false;


static char * read_until(int until1, int until2, int until3, const char *expected, 
							bool is_expr, bool is_datatype, 
							int *endtoken,
							int startlocation);
static void check_sql_expr(const char *stmt);
static void parse_datatype(const char *string, Oid *type_id, int32 *typmod);

static char *read_expr_until_semi(void);
static Plpsm_stmt *declare_prefetch(void);
static void check_labels(const char *label1, const char *label2);
static bool is_unreserved_keyword(int tok);

static const char *parser_stmt_name(Plpsm_stmt_type typ);
static void stmt_out(StringInfo ds, Plpsm_stmt *stmt, int nested_level);
static void elog_stmt(int level, Plpsm_stmt *stmt);

#define ENABLE_DEBUG_ATTR

#ifdef ENABLE_DEBUG_ATTR
#define DEBUG_INIT		StringInfoData debug_string;
#define DEBUG_SET(S,b) \
	do { \
		if (!(S)->debug) \
		{ \
			initStringInfo(&debug_string); \
			plpsm_push_back_token(yylex()); \
			plpsm_append_source_text(&debug_string, b, yylloc); \
			(S)->debug = debug_string.data; \
		} \
	} while (0)
#define DEBUG_SET_PART(S,b,c) \
	do { \
		initStringInfo(&debug_string); \
		debug_token = yylex(); \
		plpsm_append_source_text(&debug_string, b, c); \
		plpsm_push_back_token(debug_token); \
		(S)->debug = debug_string.data; \
	} while (0)
#else
#define DEBUG_INT
#define DEBUG_SET(S,b) 
#define DEBUG_SET_PART(S,b,c)
#endif

%}

%expect 0
%name-prefix="plpsm_yy"
%locations

%union {
		core_YYSTYPE		core_yystype;
		/* these fields must match core_YYSTYPE */
		int						ival;
		char				*str;
		const char			*keyword;

		PLword			word;
		PLcword			cword;
		void			*ptr;
		List				*list;
		Plpsm_stmt			*stmt;
		Node		*node;
}

%type <stmt>	function stmt dstmt statements stmt_compound
%type <stmt>	stmt_repeat_until stmt_loop stmt_while
%type <stmt>	stmt_iterate stmt_leave
%type <stmt>	assign_item assign_list stmt_set target
%type <stmt>	stmt_print stmt_return
%type <stmt>	declaration declarations declare_prefetch
%type <stmt>	stmt_if stmt_else
%type <list>	qual_identif_list qual_identif
%type <str>	expr_until_semi_or_coma expr_until_semi expr_until_do expr_until_end
%type <str>	expr_until_semi_into_using expr_until_then
%type <str>	opt_label opt_end_label
%type <node>	condition sqlstate opt_sqlstate
%type <list>	condition_list expr_list
%type <stmt>	stmt_prepare param_types_opt stmt_execute stmt_execute_immediate
%type <stmt>	stmt_open stmt_fetch stmt_close stmt_for for_prefetch
%type <str>	cursor_def

/*
 * Basic non-keyword token types.  These are hard-wired into the core lexer.
 * They must be listed first so that their numeric codes do not depend on
 * the set of keywords.  Keep this list in sync with backend/parser/gram.y!
 *
 * Some of these are not directly referenced in this file, but they must be
 * here anyway.
 */
%token <str>	IDENT FCONST SCONST BCONST XCONST Op
%token <ival>	ICONST PARAM
%token			TYPECAST DOT_DOT COLON_EQUALS

/*
 * Other tokens recognized by plpsm's lexer
 */
%token <word>		WORD
%token <cword>		CWORD

%token <keyword>	AS
%token <keyword>	BEGIN
%token <keyword>	CLOSE
%token <keyword>	CONDITION
%token <keyword>	CONTINUE
%token <keyword>	CURSOR
%token <keyword>	DECLARE
%token <keyword>	DEFAULT
%token <keyword>	DO
%token <keyword>	ELSE
%token <keyword>	ELSEIF
%token <keyword>	END
%token <keyword>	EXECUTE
%token <keyword>	EXIT
%token <keyword>	FETCH
%token <keyword>	FOR
%token <keyword>	FOUND
%token <keyword>	FROM
%token <keyword>	HANDLER
%token <keyword>	IF
%token <keyword>	IMMEDIATE
%token <keyword>	INTO
%token <keyword>	ITERATE
%token <keyword>	LEAVE
%token <keyword>	LOOP
%token <keyword>	NO
%token <keyword>	NOT
%token <keyword>	OPEN
%token <keyword>	PREPARE
%token <keyword>	PRINT
%token <keyword>	REPEAT
%token <keyword>	RETURN
%token <keyword>	SCROLL
%token <keyword>	SET
%token <keyword>	SQLEXCEPTION
%token <keyword>	SQLSTATE
%token <keyword>	SQLWARNING
%token <keyword>	THEN
%token <keyword>	UNDO
%token <keyword>	UNTIL
%token <keyword>	USING
%token <keyword>	VALUE
%token <keyword>	WHILE

%%

function:
			dstmt opt_semi
				{
					plpsm_parser_tree = $1;
					if (plpsm_debug_parser)
						elog_stmt(NOTICE, $1);
				}
		;

opt_semi:
			';'
				{
				}
			|
				{
				}
		;

statements:
			dstmt
				{
					$$ = $1;
				}
			| statements  ';' dstmt
				{
					$1->last->next = $3;
					$1->last = $3->last;
					$$ = $1;
				}
		;

/*
 * statement with possible debug info
 */
dstmt:			stmt
				{
					int	location = $1->location;
					DEBUG_INIT;
					/* recheck "last" ptr */
					if (!$1->last)
						$1->last = $1;
#ifdef ENABLE_DEBUG_ATTR
					while ($1)
					{
						DEBUG_SET($1, location);
						$1 = $1->next;
					}
#endif
				}

stmt:
			stmt_compound				{ $$ = $1; }
			| stmt_set				{ $$ = $1; }
			| stmt_print				{ $$ = $1; }
			| stmt_loop				{ $$ = $1; }
			| stmt_while				{ $$ = $1; }
			| stmt_repeat_until			{ $$ = $1; }
			| stmt_iterate				{ $$ = $1; }
			| stmt_leave				{ $$ = $1; }
			| stmt_return				{ $$ = $1; }
			| stmt_prepare				{ $$ = $1; }
			| stmt_execute				{ $$ = $1; }
			| stmt_execute_immediate		{ $$ = $1; }
			| stmt_open				{ $$ = $1; }
			| stmt_fetch				{ $$ = $1; }
			| stmt_close				{ $$ = $1; }
			| stmt_for				{ $$ = $1; }
			| stmt_if				{ $$ = $1; }
		;

/*----
 * compound statement
 * 
 * [ label: ]
 * BEGIN 
 *   [ variable's or condition's declarations; ]
 *   [ cursor's declarations; ]
 *   [ handler's declarations; ]
 *   [ statements; ]
 * END [ label ]
 *
 */
stmt_compound:
			opt_label BEGIN statements ';' END opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, @2);
					new->name = $1;
					new->inner_left = $3;
					check_labels($1, $6);
					$$ = new;
				}
			| opt_label BEGIN declarations ';' END opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, @2);
					new->name = $1;
					new->inner_left = $3;
					check_labels($1, $6);
					$$ = new;
				}
			| opt_label BEGIN declarations ';' statements ';' END opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, @2);
					new->name = $1;
					/* join declarations and statements */
					$3->last->next = $5;
					$3->last = $5->last;
					new->inner_left = $3;
					check_labels($1, $8);
					$$ = new;
				}
			| opt_label BEGIN END opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, @2);
					new->name = $1;
					check_labels($1, $4);
					$$ = new;
				}
		;

opt_label:
			WORD ':'
				{
					$$ = $1.ident;
				}
			|
				{
					$$ = NULL;
				}
		;

opt_end_label:
			WORD
				{
					$$ = $1.ident;
				}
			|
				{
					$$ = NULL;
				}
		;

declarations:
			declaration 
				{
					$$ = $1;
					$1->last = $1;
				}
			| declarations ';' declaration
				{
					/*
					 * we have to check order of declarations,
					 * first variable or condition, next cursor's
					 * declarations and last handler's declarations.
					 */
					Plpsm_stmt *lstmt = $1->last;
					
					if ($3->typ == PLPSM_STMT_DECLARE_VARIABLE)
					{
						if (lstmt->typ == PLPSM_STMT_DECLARE_CURSOR || lstmt->typ == PLPSM_STMT_DECLARE_HANDLER)
							yyerror("syntax error, variable declaration after cursor or handler");
					}
					else if ($3->typ == PLPSM_STMT_DECLARE_CONDITION)
					{
						if (lstmt->typ == PLPSM_STMT_DECLARE_CURSOR || lstmt->typ == PLPSM_STMT_DECLARE_HANDLER)
							yyerror("syntax error, condition declaration after cursor or handler");
					}
					else if ($3->typ == PLPSM_STMT_DECLARE_CURSOR)
					{
						if (lstmt->typ == PLPSM_STMT_DECLARE_HANDLER)
							yyerror("syntax error, cursor declaration after handler");
					}
					else if ($3->typ != PLPSM_STMT_DECLARE_HANDLER)
					{
						elog(ERROR, "internal error, unexpected value");
					}

					$1->last->next = $3;
					$1->last = $3;
					$$ = $1;
				}
		;

declaration:
			DECLARE declare_prefetch
				{
					DEBUG_INIT;
					if ($2->typ != PLPSM_STMT_DECLARE_VARIABLE)
						yyerror("syntax error");
					$$ = $2;
					DEBUG_SET($2, @1);
				}
			| DECLARE declare_prefetch CONDITION opt_sqlstate
				{
					DEBUG_INIT;
					if ($2->typ != PLPSM_STMT_DECLARE_CONDITION)
						yyerror("syntax error");
					$2->data = $4;
					$$ = $2;
					DEBUG_SET($2, @1);
				}
			| DECLARE declare_prefetch CURSOR FOR cursor_def
				{
					DEBUG_INIT;
					if ($2->typ != PLPSM_STMT_DECLARE_CURSOR)
						yyerror("syntax error");
					$2->query = $5;
					$$ = $2;
					DEBUG_SET($2, @1);
				}
			| DECLARE declare_prefetch HANDLER FOR condition_list dstmt
				{
					DEBUG_INIT;
					if ($2->typ != PLPSM_STMT_DECLARE_HANDLER)
						yyerror("syntax error");
					$2->inner_left = $6;
					$2->data = $5;
					$$ = $2;
					DEBUG_SET($2, @1);
				}
		;

declare_prefetch:
				{
					$$ = declare_prefetch();
				}
		;

condition_list:
			condition
				{
					$$ = list_make1($1);
				}
			| condition_list ',' condition
				{
					$$ = lappend($1, $3);
				}
		;

opt_sqlstate:
			FOR SQLSTATE opt_value sqlstate
				{
					$$ = $4;
				}
			|	{
					$$ = NULL;
				}
		;

condition:
			SQLSTATE opt_value sqlstate
				{
					$$ = (Node *) list_make2(
						makeString(pstrdup("IN")), $3);
				}
			| NOT FOUND
				{
					$$ = (Node *) list_make2(
						makeString(pstrdup("IN")),
						makeInteger(MAKE_SQLSTATE('0','2','0','0','0')));
				}
			| SQLWARNING
				{
					$$ = (Node *) list_make3(
						makeString(pstrdup("IN")),
						makeInteger(MAKE_SQLSTATE('0','1','0','0','0')),
						makeInteger(MAKE_SQLSTATE('0','2','0','0','0')));
				}
			| SQLEXCEPTION
				{
					$$ = (Node *) list_make3(
						makeString(pstrdup("NOT_IN")),
						makeInteger(MAKE_SQLSTATE('0','1','0','0','0')),
						makeInteger(MAKE_SQLSTATE('0','2','0','0','0')));
				}
			| WORD
				{
					$$ = (Node *) makeString($1.ident);
				}
		;

sqlstate:		SCONST
				{
					char   *sqlstatestr;

					sqlstatestr = $1;

					if (strlen(sqlstatestr) != 5)
						yyerror("invalid SQLSTATE code");
					if (strspn(sqlstatestr, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5)
						yyerror("invalid SQLSTATE code");

					$$ = (Node *) makeInteger(MAKE_SQLSTATE(sqlstatestr[0],
												  sqlstatestr[1],
												  sqlstatestr[2],
												  sqlstatestr[3],
												  sqlstatestr[4]));
				}
		;

opt_value:
			VALUE
				{
				}
			|	{
				}
		;

/*----
 * condition statement IF
 *
 * IF ... THEN  ELSIF ... THEN ... ELSE ... END IF
 */
stmt_if:		IF expr_until_then statements ';' stmt_else END IF
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_IF, @1);
					new->expr = $2;
					new->inner_left = $3;
					new->inner_right = $5;
					$$ = new;
				}
		;

stmt_else:
				{
					$$ = NULL;
				}
			| ELSEIF expr_until_then statements ';' stmt_else
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_IF, @1);
					DEBUG_INIT;
					new->expr = $2;
					new->inner_left = $3;
					new->inner_right = $5;
					$$ = new;
					DEBUG_SET(new, @1);
				}
			| ELSE statements ';'
				{
					$$ = $2;
				}
		;

/*----
 * (multi) assign statement 
 * 
 * SET var = expr
 * SET var1 = expr1 [, var2 = expr2 [..] ]         -- MySQL multi assign statement
 * SET (var1 [,var2 [..]] ) = row_expr             -- ANSI SQL multi assign statement
 *
 * There are supported simple assign statement, multi assign statement by ANSI and
 * multi assign statement from MySQL
 */
stmt_set:		SET assign_list
				{
					$$ = $2;
				}
		;

assign_list:
			assign_item
				{
					$$ = $1;
					$1->last = $1;
				}
			| assign_list ',' assign_item
				{
					/* expected just very short list */
					$1->last->next = $3;
					$1->last = $3;
					$$ = $1;
				}

assign_item:		target '=' expr_until_semi_or_coma
				{
					$1->typ = PLPSM_STMT_SET;
					$1->expr = $3;
					$$ = $1;
				}
		;

target:
			qual_identif
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_UNKNOWN, @1);
					new->target = $1;
					$$ = new;
				}
			| '(' qual_identif_list ')'
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_UNKNOWN, @1);
					new->compound_target = $2;
					$$ = new;
				}
		;

qual_identif_list:
			qual_identif
				{
					$$ = list_make1($1);
				}
			| qual_identif_list ',' qual_identif
				{
					$$ = lappend($1, $3);
				}
			;

qual_identif:
			WORD
				{
					$$ = list_make1(makeString($1.ident));
				}
			| CWORD
				{
					$$ = $1.idents;
				}
			| PARAM
				{
					/* ToDo: Plpsm_object should be a param type too */
					char buf[32];
					snprintf(buf, sizeof(buf), "$%d", $1);
					$$ = list_make1(makeString(pstrdup(buf)));
				}
		;

/*----
 * debug print
 *
 * PRINT expr
 *
 * note: this is only PostgreSQL feature
 */
stmt_print:		PRINT expr_until_semi
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PRINT, @1);
					new->expr = $2;
					$$ = new;
				}
		;

/*----
 * return statement - returns value from function
 *
 * RETURN [ expr ]
 *
 */
stmt_return:
			RETURN
				{
					int tok = yylex();
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RETURN, @1);
					if (tok == ';' || tok == 0)
					{
						plpsm_push_back_token(tok);
					}
					else
					{
						plpsm_push_back_token(tok);
						new->expr = read_expr_until_semi();
					}
					$$ = new;
				}
		;

/*----
 * simple loop - unfinited cycle
 *
 * [ label: ] LOOP statements; END LOOP [ label ]
 *
 */
stmt_loop:		opt_label LOOP statements ';' END LOOP opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_LOOP, $1 ? @1 : @2);
					new->name = $1;
					new->inner_left = $3;
					check_labels($1, $7);
					$$ = new;
				}
		;

/*----
 * while cycle
 *
 * [ label: ] WHILE expr DO statements; END WHILE [ label ]
 *
 */
stmt_while:		opt_label WHILE expr_until_do statements ';' END WHILE opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_WHILE, $1 ? @1 : @2);
					new->name = $1;
					new->expr = $3;
					new->inner_left = $4;
					check_labels($1, $8);
					$$ = new;
				}
		;

/*----
 * repeat-until cycle
 *
 * [ label: ] REPEAT statements; UNTIL expr END REPEAT [ label ]
 *
 */
stmt_repeat_until:	opt_label REPEAT statements ';' UNTIL expr_until_end REPEAT opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_REPEAT_UNTIL, $1 ? @1 : @2);
					new->name = $1;
					new->expr = $6;
					new->inner_left = $3;
					check_labels($1, $8);
					$$ = new;
				}
		;

/*----
 * start a new cycle
 *
 * ITERATE label
 *
 */
stmt_iterate:		ITERATE WORD
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_ITERATE, @1);
					new->name = $2.ident;
					$$ = new;
				}
		;

/*----
 * break a cycle
 *
 * LEAVE label
 *
 * note: LEAVE can be used inside compound statement too
 */
stmt_leave:		LEAVE WORD
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_LEAVE, @1);
					new->name = $2.ident;
					$$ = new;
				}
		;


/*----
 * create a prepare statement
 *
 * PREPARE name [ '(' type [, type [..]] ')' ] FROM expr
 *
 */
stmt_prepare:		PREPARE WORD param_types_opt FROM expr_until_semi
				{
					Plpsm_stmt *new = $3;
					new->location = @1;
					new->target = list_make1(makeString($2.ident));
					new->expr = $5;
					$$ = new;
				}
		;

param_types_opt:
				{
					int tok = yylex();
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PREPARE, -1);

					if (tok == '(')
					{
						int endtok = '(';
						List	*types = NIL;
						char *expr;

						/* read a list of data types */
						while (endtok != ')')
						{
							expr = read_until(',', ')', 0, ", or )", false, true, &endtok, -1);
							types = lappend(types, makeString(expr));
							if (endtok == ')')
								break;
							if (endtok == ',')
								/* consume coma */
								yylex();
							else
								yyerror("syntax error, expected \",\"");
						}
						new->data = types;
					}
					else
						plpsm_push_back_token(tok);
					$$ = new;
				}
		;

/*----
 * execute a prepared statement
 *
 * EXECUTE name [ INTO variable [, variable [..]] ] [ USING expr [, expr [..]] ]
 *
 * note: a clausule using is a PostgreSQL feature
 */
stmt_execute:
			EXECUTE WORD 
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, @1);
					new->name = $2.ident;
					$$ = new;
				}
			| EXECUTE WORD INTO target 
				{
					Plpsm_stmt *new = $4;
					new->typ = PLPSM_STMT_EXECUTE;
					new->location = @1;
					new->name = $2.ident;
					$$ = new;
				}
			| EXECUTE WORD INTO target USING expr_list
				{
					Plpsm_stmt *new = $4;
					new->typ = PLPSM_STMT_EXECUTE;
					new->location = @1;
					new->name = $2.ident;
					new->data = $6;
					$$ = new;
				}
			| EXECUTE WORD USING expr_list
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, @1);
					new->name = $2.ident;
					new->data = $4;
					$$ = new;
				}
		;

/*----
 * execute a prepared statement
 *
 * EXECUTE IMMEDIATE expr [ INTO variable [, variable [..]] ] [ USING expr [, expr [..]] ]
 *
 */
stmt_execute_immediate:
			EXECUTE IMMEDIATE expr_until_semi_into_using
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE_IMMEDIATE, @1);
					new->expr = $3;
					$$ = new;
				}
			| EXECUTE IMMEDIATE expr_until_semi_into_using INTO target
				{
					Plpsm_stmt *new = $5;
					new->typ = PLPSM_STMT_EXECUTE_IMMEDIATE;
					new->location = @1;
					new->expr = $3;
					$$ = new;
				}
			| EXECUTE IMMEDIATE expr_until_semi_into_using USING expr_list
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE_IMMEDIATE, @1);
					new->expr = $3;
					new->data = $5;
					$$ = new;
				}
			| EXECUTE IMMEDIATE expr_until_semi_into_using INTO target USING expr_list
				{
					Plpsm_stmt *new = $5;
					new->typ = PLPSM_STMT_EXECUTE_IMMEDIATE;
					new->location = @1;
					new->expr = $3;
					new->data = $7;
					$$ = new;
				}
		;

/*----
 * open a cursor, possible open a cursor based on prepared statement
 *
 * OPEN cursor_name [ USING expr [, expr [..] ]
 *
 * note: a using of USING clause is a DB2 feature - dynamic cursor support
 */
stmt_open:
			OPEN qual_identif
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_OPEN, @1);
					new->target = $2;
					$$ = new;
				}
			| OPEN qual_identif USING expr_list
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_OPEN, @1);
					new->target = $2;
					new->data = $4;
					$$ = new;
				}
		;

/*----
 * store data to variable(s)
 *
 * FETCH cursor_name INTO variable [, variable [..] ]
 *
 */
stmt_fetch:		FETCH qual_identif INTO qual_identif_list
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_FETCH, @1);
					new->compound_target = $4;
					new->data = $2;
					$$ = new;
				}
		;

/*----
 * close a cursor
 *
 * CLOSE cursor_name
 *
 */
stmt_close:		CLOSE qual_identif
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CLOSE, @1);
					new->target = $2;
					$$ = new;
				}
		;

/*----
 * iterate over query
 *
 * [ label: ] FOR [ namespace AS ] [ cursor_name CURSOR FOR ] Sqlquery DO statements; END FOR [ label ]
 *
 */
stmt_for:
			opt_label FOR for_prefetch statements ';' END FOR opt_end_label
				{
					Plpsm_stmt *new = $3;
					new->location = $1 ? @1 : @2;
					new->name = $1;
					new->inner_left = $4;
					check_labels($1, $8);
					$$ = new;
				}
		;

/* 
 * syntax of FOR statement uses a "postfix form", so it
 * isn't possible to implement with gramatic rules.
 */
for_prefetch:
				{
					char *namespace = NULL;
					char *cursor_name = NULL;
					int tok;
					int	startloc = -1;
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_FOR, -1);

					/* read a possible namespace */
					tok = yylex(); startloc = yylloc;
					if (tok  == WORD)
					{
						int tok1;

						namespace = yylval.word.ident;
						tok1 = yylex();
						if (tok1 == AS)
						{
							/* try to read cursor name */
							tok = yylex();
							startloc = yylloc;
							if (tok == WORD)
							{
								cursor_name = yylval.word.ident;
								tok1 = yylex();
								if (tok1 == CURSOR)
								{
									if (yylex() != FOR)
										yyerror("expected for");
									startloc = -1;
								}
								else
								{
									cursor_name = NULL;
									plpsm_push_back_token(tok1);
									plpsm_push_back_token(tok);
								}
							}
							else
								plpsm_push_back_token(tok);
						}
						else if (tok1 == CURSOR)
						{
							cursor_name = namespace;
							namespace = NULL;
							if (yylex() != FOR)
								yyerror("expected for");
							startloc = -1;
						}
						else
						{
							plpsm_push_back_token(tok1);
							plpsm_push_back_token(tok);
						}
					}
					else
						plpsm_push_back_token(tok);

					if (namespace != NULL || cursor_name != NULL)
						new->data = list_make2(makeString(namespace),
									    makeString(cursor_name));
					new->query = read_until(DO, -1, -1, "DO", false, false, NULL, startloc);
					$$ = new;
				}
		;

expr_until_semi:
				{
					$$ = read_expr_until_semi();
				}
		;

expr_list:
			expr_until_semi_or_coma
				{
					$$ = list_make1(makeString($1));
				}
			| expr_list ',' expr_until_semi_or_coma
				{
					$$ = lappend($1, makeString($3));
				}
			;

expr_until_semi_or_coma:
				{
					StringInfoData		ds;
					char *expr;
					int	tok;

					expr = read_until(';',',', 0, "; or ,", true, false, &tok, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					$$ = expr;
				}
		;

expr_until_do:
				{
					StringInfoData		ds;
					char *expr;

					expr = read_until(DO, -1, -1, "DO", true, false, NULL, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					$$ = expr;
				}
		;

expr_until_then:
				{
					StringInfoData		ds;
					char *expr;

					expr = read_until(THEN, -1, -1, "THEN", true, false, NULL, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					$$ = expr;
				}
		;

expr_until_end:
				{
					StringInfoData		ds;
					char *expr;

					expr = read_until(END, -1, -1, "END", true, false, NULL, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					$$ = expr;
				}
		;

expr_until_semi_into_using:
				{
					StringInfoData		ds;
					char *expr;
					int	endtok;

					expr = read_until(';',INTO, USING, "; or INTO or USING", true, false, &endtok, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					if (endtok != ';')
						plpsm_push_back_token(endtok);
					$$ = expr;
				}
		;

cursor_def:
				{
					$$ = read_until(';', 0, 0, ";", false, false, NULL, -1);
				}

%%

static char 
*read_expr_until_semi(void)
{
	StringInfoData		ds;
	char *expr;

	expr = read_until(';',0, 0, ";", true, false, NULL, -1);
	initStringInfo(&ds);
	appendStringInfo(&ds, "SELECT (%s)", expr);
	
	check_sql_expr(ds.data);
	pfree(ds.data);
	
	return expr;
}

/*
 * when optional_endtoken is true, then expression can be finished
 * without identification of any until_tokens.
 */
static char *
read_until(int until1,
			int until2,
			int until3,
			const char *expected,
			bool	is_expr,
			bool	is_datatype,
			int *endtoken,
			int startlocation)
{
	int				parenlevel = 0;
	int tok;
	StringInfoData		ds;

	initStringInfo(&ds);

	for (;;)
	{
		/* read a current location before you read a next tag */
		tok = yylex();
		if (startlocation < 0)
			startlocation = yylloc;

		/* 
		 * When until1 is semicolon, then endtag is optional.
		 * Any lists, or SQLs must not contains a zero tag and must not
		 * contains a semicolon - but, parenlevel must be zero for
		 * leaving cycle.
		 */
		if (tok == until1 || tok == until2 || tok == until3 || (until1 == ';' && tok == 0))
		{
			/* don't want to leave early - etc SET a = (10,20), b = .. */
			if (tok == ';' || tok == 0 || parenlevel == 0)
				break;
		}
		if (tok == '(' || tok == '[')
			parenlevel++;
		else if (tok == ')' || tok == ']')
		{
			parenlevel--;
			if (parenlevel < 0)
				yyerror("mismatched parentheses");
		}

		if (tok == 0)
		{
			if (parenlevel != 0)
				yyerror("mismatched parentheses");

			/*
			 * Probably there can be a missing token, if object is
			 * SQL expression, then try to verify expression first, there
			 * can be more early detected missing symbol.
			 */
			if (is_expr)
			{
				StringInfoData	bexpr;

				if (startlocation >= yylloc)
					yyerror("missing expression");

				initStringInfo(&bexpr);
				appendStringInfoString(&bexpr, "SELECT (");
				plpsm_append_source_text(&bexpr, startlocation, yylloc);
				check_sql_expr(bexpr.data);
				pfree(bexpr.data);
			}

			ereport(ERROR,
					(errcode(ERRCODE_SYNTAX_ERROR),
					 errmsg("missing \"%s\" at end of SQL expression",
										    expected)));
		}
	}

	if (parenlevel != 0)
		yyerror("mismatched parentheses");

	if (endtoken)
		*endtoken = tok;

	if (is_expr && startlocation >= yylloc)
		yyerror("missing expression");

	if (is_datatype && startlocation >= yylloc)
		yyerror("missing data type");

	/* semicolon is usually returned back */
	if (tok == ';' || tok == ',')
		plpsm_push_back_token(tok);

	plpsm_append_source_text(&ds, startlocation, yylloc);

	return ds.data;
}

static void
check_sql_expr(const char *stmt)
{
	MemoryContext oldCxt;

	oldCxt = MemoryContextSwitchTo(CurrentMemoryContext);
	(void) raw_parser(stmt);
	MemoryContextSwitchTo(oldCxt);
}

typedef enum
{
	Initial = 0,
	Unknown = 1,
	EXPECTED_VARIABLE = 2,
	COMPLETE_LIST_OF_VARIABLES = 3,
	EXPECTED_DATATYPE = 4,
	EXPECTED_EXPRESSION = 5,
	EXPECTED_CURSOR = 6
} DeclareParsingState;

/*
 * Because I would to see a parser by the most simply, then
 * a parsing of declaratin part is handy written. This section needs
 * a relative large prefetch lookup.
 */
static Plpsm_stmt *
declare_prefetch(void)
{
	DeclareParsingState state = Initial;
	Plpsm_stmt *result;
	int tok;
	List	*varnames = NIL;
	int	option;
	int		startlocation = -1;

	result = plpsm_new_stmt(PLPSM_STMT_UNKNOWN, -1);

	for (;;)
	{
		if (state == EXPECTED_DATATYPE)
		{
			int endtok;
			char *datatype;
			Oid	type_id;
			int32	typmod;
			int16		typlen;
			bool		typbyval;

			result->typ = PLPSM_STMT_DECLARE_VARIABLE;
			result->compound_target = varnames;

			datatype = read_until(';', DEFAULT, 0, "; or \"DEFAULT\"", false, true, &endtok, startlocation);
			parse_datatype(datatype, &type_id, &typmod);
			get_typlenbyval(type_id, &typlen, &typbyval);

			result->vartype.typoid = type_id;
			result->vartype.typmod = typmod;
			result->vartype.typename = datatype;
			result->vartype.typlen = typlen;
			result->vartype.typbyval = typbyval;

			if (endtok == ';')
				break;

			/* when DEFAULT value is specified, then read a expression until semicolon */
			result->expr = read_expr_until_semi();
			break;
		}

		if (state == EXPECTED_CURSOR)
		{
			int tok = yylex();

			Assert(tok == CURSOR);

			result->typ = PLPSM_STMT_DECLARE_CURSOR;
			result->target = linitial(varnames);
			result->option = option;
			plpsm_push_back_token(tok);
			break;
		}

		if ((tok = yylex()) == 0)
			yyerror("unpexpected end of function definition");

		if (state == Initial)
		{
			if (tok == WORD)
			{
				/* store identifier */
				varnames = list_make1(makeString(yylval.word.ident));
				state = Unknown;
				continue;
			}
			if (tok == CONTINUE || tok == EXIT || tok == UNDO)
			{
				int tok1;
				const char *varname = yylval.keyword;		/* store pointer for possible later usage */

				/* 
				 * recheck next symbol, when next token is HANDLER,
				 * then we found a handler's declaration, otherwise
				 * it's variable declaration.
				 */
				if ((tok1 = yylex()) == HANDLER)
				{
					plpsm_push_back_token(tok1);
					result->typ = PLPSM_STMT_DECLARE_HANDLER;
					switch (tok1)
					{
						case CONTINUE:
							result->option = PLPSM_HANDLER_CONTINUE;
							break;
						case EXIT:
							result->option = PLPSM_HANDLER_EXIT;
							break;
						case UNDO:
							result->option = PLPSM_HANDLER_UNDO;
							break;
					}
					break;
				}
				else
				{
					/* store identifier */
					plpsm_push_back_token(tok1);
					varnames = list_make1(makeString(pstrdup(varname)));
					state = Unknown;
					continue;
				}
			}
		}

		if (state == Unknown)
		{
			if (tok == ',')
			{
				/* store identificator */
				state = EXPECTED_VARIABLE;
				continue;
			}

			if (tok == CONDITION)
			{
				result->typ = PLPSM_STMT_DECLARE_CONDITION;
				result->target = linitial(varnames);
				plpsm_push_back_token(tok);
				break;
			}

			if (tok == CURSOR)
			{
				plpsm_push_back_token(tok);
				option = PLPSM_CURSOR_NOSCROLL;
				state = EXPECTED_CURSOR;
				continue;
			}
			else if (tok == SCROLL)
			{
				int tok1;

				startlocation = yylloc;
				tok1 = yylex();
				if (tok1 == CURSOR)
				{
					plpsm_push_back_token(tok1);
					option = PLPSM_CURSOR_SCROLL;
					state = EXPECTED_CURSOR;
					continue;
				}
				else
				{
					plpsm_push_back_token(tok1);
					plpsm_push_back_token(tok);
					state = EXPECTED_DATATYPE;
					continue;
				}
			}
			else if (tok == NO)
			{
				int tok1;

				startlocation = yylloc;
				tok1 = yylex();
				if (tok1 != SCROLL)
				{
					plpsm_push_back_token(tok1);
					plpsm_push_back_token(tok);
					state = EXPECTED_DATATYPE;
					continue;
				}
				else
				{
					int tok2 = yylex();

					if (tok2 != CURSOR)
					{
						plpsm_push_back_token(tok2);
						plpsm_push_back_token(tok1);
						plpsm_push_back_token(tok);
						state = EXPECTED_DATATYPE;
						continue;
					}
					else
					{
						plpsm_push_back_token(tok2);
						option = PLPSM_CURSOR_NOSCROLL;
						state = EXPECTED_CURSOR;
						continue;
					}
				}
			}

			if (tok == WORD || is_unreserved_keyword(tok))
			{
				plpsm_push_back_token(tok);
				state = EXPECTED_DATATYPE;
				continue;
			}
		}

		if (state == EXPECTED_VARIABLE)
		{
			if (tok != WORD && !is_unreserved_keyword(tok))
				yyerror("missing a variable identifier");
			varnames = lappend(varnames, makeString(yylval.word.ident));
			state = COMPLETE_LIST_OF_VARIABLES;
			continue;
		}

		if (state == COMPLETE_LIST_OF_VARIABLES)
		{
			if (tok == ',')
			{
				/* do nothing */
				state = EXPECTED_VARIABLE;
				continue;
			}
			else
			{
				plpsm_push_back_token(tok);
				state = EXPECTED_DATATYPE;
				continue;
			}
		}

		yyerror("syntax error");
	}

	return result;
}

static bool
is_unreserved_keyword(int tok)
{
	switch (tok)
	{
		case AS:
		case CONTINUE:
		case EXIT:
		case FOUND:
		case IMMEDIATE:
		case NO:
		case NOT:
		case SCROLL:
		case SQLEXCEPTION:
		case SQLSTATE:
		case SQLWARNING:
		case UNDO:
			return true;
		default:
			return false;
	}
}

static void
parse_datatype(const char *string, Oid *type_id, int32 *typmod)
{
	/* Let the main parser try to parse it under standard SQL rules */
	parseTypeString(string, type_id, typmod);
}

Plpsm_stmt *
plpsm_new_stmt(Plpsm_stmt_type typ, int location)
{
	Plpsm_stmt *n = palloc0(sizeof(Plpsm_stmt));
	n->typ = typ;
	n->location = location;
	return n;
}

/*
 * ensure so end label is same as begin label
 */
static void
check_labels(const char *label1, const char *label2)
{
	if (label2 == NULL || label1 == NULL)
		return;
	if (label2 != NULL && label1 == NULL)
		yyerror("syntax error, missing begin label");
	if (strcmp(label1, label2) != 0)
		yyerror("end label is defferent than begin label");
}

static const char *
parser_stmt_name(Plpsm_stmt_type typ)
{
	switch (typ)
	{
		case PLPSM_STMT_COMPOUND_STATEMENT:
			return "compound statement";
		case PLPSM_STMT_DECLARE_VARIABLE:
			return "declare variable";
		case PLPSM_STMT_DECLARE_CONDITION:
			return "declare condition";
		case PLPSM_STMT_DECLARE_CURSOR:
			return "declare cursor";
		case PLPSM_STMT_DECLARE_HANDLER:
			return "declare handler";
		case PLPSM_STMT_EXECUTE:
			return "execute statement";
		case PLPSM_STMT_EXECUTE_IMMEDIATE:
			return "execute immediate statement";
		case PLPSM_STMT_SET:
			return "assign statement";
		case PLPSM_STMT_PREPARE:
			return "prepare statement";
		case PLPSM_STMT_PRINT:
			return "print statement";
		case PLPSM_STMT_LOOP:
			return "loop statement";
		case PLPSM_STMT_WHILE:
			return "while statement";
		case PLPSM_STMT_REPEAT_UNTIL:
			return "repeat-until statement";
		case PLPSM_STMT_ITERATE:
			return "iterate statement";
		case PLPSM_STMT_LEAVE:
			return "leave statement";
		case PLPSM_STMT_RETURN:
			return "return statement";
		case PLPSM_STMT_UNKNOWN:
			return "unknown statement";
		case PLPSM_STMT_OPEN:
			return "open statement";
		case PLPSM_STMT_CLOSE:
			return "close statement";
		case PLPSM_STMT_FETCH:
			return "fetch statement";
		case PLPSM_STMT_FOR:
			return "for statement";
		case PLPSM_STMT_IF:
			return "if statement";
		default:
			return "unknown statment typid";
	}
}

/*
 * helper routines to debug output of parser stage
 */
static void
stmt_out(StringInfo ds, Plpsm_stmt *stmt, int nested_level)
{
	char *ident = pstrdup("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t");

	if (stmt == NULL)
		return;
	ident[nested_level] = '\0';

	appendStringInfo(ds, "%s+--------------------------------------------------\n", ident);
	appendStringInfo(ds, "%s| debug for <<%s>>\n", ident, parser_stmt_name(stmt->typ));
	appendStringInfo(ds, "%s+--------------------------------------------------\n", ident);
	appendStringInfo(ds, "%s| Name: %s\n", ident, stmt->name);
	appendStringInfo(ds, "%s| Target: %s\n", ident, nodeToString(stmt->target));
	appendStringInfo(ds, "%s| Compound target: %s\n", ident, nodeToString(stmt->compound_target));
	//appendStringInfo(ds, "%s| Data: %s\n", ident, nodeToString(stmt->data));
	appendStringInfo(ds, "%s| Option: %d\n", ident, stmt->option);
	appendStringInfo(ds, "%s| Query: %s\n", ident, stmt->query);
	appendStringInfo(ds, "%s| Expr: %s\n", ident, stmt->expr);
	appendStringInfo(ds, "%s| Debug: \"%s\"\n", ident, stmt->debug ? stmt->debug : "");
	appendStringInfo(ds, "%s| Inner left:\n", ident);
	stmt_out(ds, stmt->inner_left, nested_level + 1);
	appendStringInfo(ds, "%s| Inner right:\n", ident);
	stmt_out(ds, stmt->inner_right, nested_level + 1);
	if (stmt->next != NULL)
	{
		appendStringInfo(ds, "%s\n", ident);
		stmt_out(ds, stmt->next, nested_level);
	}
	else
		appendStringInfo(ds, "%s+-------------------------------\n", ident);

	pfree(ident);
}

static void 
elog_stmt(int level, Plpsm_stmt *stmt)
{
	StringInfoData ds;
	
	initStringInfo(&ds);
	stmt_out(&ds, stmt, 0);
	
	elog(level, "Parser stage result:\n%s", ds.data);
	pfree(ds.data);
}

