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
#include "utils/memutils.h"

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

#define parser_errposition(pos)		plpsm_scanner_errposition(pos)

static Plpsm_ESQL *read_embeded_sql(int until1,
						int until2,
						int until3,
							const char *expected,
								Plpsm_esql_type expected_type,
											bool valid_sql,
												int *endtoken, int startlocation,
													Oid *typoid, int32 *typmod);

static void check_sql_expr(const char *stmt, int location, int leaderlen);
static void parse_datatype(const char *string, int location, Oid *typoid, int32 *typmod);

static Plpsm_ESQL *read_expr_until_semi(void);
static Plpsm_stmt *declare_prefetch(void);
static void check_labels(const char *label1, const char *label2);
static bool is_unreserved_keyword(int tok);

static const char *parser_stmt_name(Plpsm_stmt_type typ);
static void stmt_out(StringInfo ds, Plpsm_stmt *stmt, int nested_level);
static void elog_stmt(int level, Plpsm_stmt *stmt);

static void pqualid_out(StringInfo ds, Plpsm_positioned_qualid *qualid);
static void pqualid_list_out(StringInfo ds, List *list);

static Plpsm_stmt *make_stmt_sql(int location);

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
		bool			boolean;
		List				*list;
		Plpsm_stmt			*stmt;
		Plpsm_ESQL			*esql;
		Plpsm_positioned_qualid		*qualid;
		Plpsm_condition_value		*condition;
		Node		*node;
}

%type <stmt>	function stmt dstmt statements stmt_compound
%type <stmt>	stmt_repeat_until stmt_loop stmt_while
%type <stmt>	stmt_iterate stmt_leave
%type <stmt>	assign_item assign_list stmt_set target
%type <stmt>	stmt_print stmt_return
%type <stmt>	declaration declarations declare_prefetch
%type <stmt>	stmt_if stmt_else
%type <list>	qual_identif_list 
%type <qualid>	qual_identif
%type <esql>	expr_until_semi_or_coma expr_until_semi expr_until_do expr_until_end
%type <esql>	expr_until_semi_into_using expr_until_then
%type <str>	opt_label opt_end_label
%type <condition>	condition condition_list opt_sqlstate
%type <list>	expr_list expr_list_into
%type <stmt>	stmt_prepare stmt_execute stmt_execute_immediate
%type <stmt>	stmt_open stmt_fetch stmt_close stmt_for for_prefetch
%type <stmt>	stmt_case case_when_list case_when opt_case_else
%type <esql>	opt_expr_until_when expr_until_semi_or_coma_or_parent
%type <stmt>	stmt_sql stmt_select_into
%type <ival>	sqlstate
%type <boolean>		opt_atomic

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

%token <keyword>	ATOMIC
%token <keyword>	AS
%token <keyword>	BEGIN
%token <keyword>	CASE
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
%token <keyword>	SELECT
%token <keyword>	SET
%token <keyword>	SQLEXCEPTION
%token <keyword>	SQLSTATE
%token <keyword>	SQLCODE
%token <keyword>	SQLWARNING
%token <keyword>	THEN
%token <keyword>	UNDO
%token <keyword>	UNTIL
%token <keyword>	USING
%token <keyword>	VALUE
%token <keyword>	WHEN
%token <keyword>	WHILE

%token <keyword>	INSERT
%token <keyword>	UPDATE
%token <keyword>	DELETE

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
					if (!$1->last)
						$1->last = $1;
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
			| stmt_case				{ $$ = $1; }
			| stmt_sql				{ $$ = $1; }
			| stmt_select_into			{ $$ = $1; }
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
			opt_label BEGIN opt_atomic statements ';' END opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, $1 ? @1: @2);
					new->name = $1;
					new->option = $3;
					new->inner_left = $4;
					check_labels($1, $7);
					$$ = new;
				}
			| opt_label BEGIN opt_atomic declarations ';' END opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, $1 ? @1: @2);
					new->name = $1;
					new->option = $3;
					new->inner_left = $4;
					check_labels($1, $7);
					$$ = new;
				}
			| opt_label BEGIN opt_atomic declarations ';' statements ';' END opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, $1 ? @1: @2);
					new->name = $1;
					new->option = $3;
					/* join declarations and statements */
					$4->last->next = $6;
					$4->last = $6->last;
					new->inner_left = $4;
					check_labels($1, $9);
					$$ = new;
				}
			| opt_label BEGIN END opt_end_label
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, $1 ? @1: @2);
					new->name = $1;
					check_labels($1, $4);
					$$ = new;
				}
		;

opt_atomic:
				{
					$$ = false;
				}
			| ATOMIC
				{
					$$ = true;
				}
		;

/*
 * When missing a colon, then move a position back, because we want
 * to mark a WORD, not next token.
 */
opt_label:
			WORD
				{
					$$ = $1.ident;
					if (yylex() != ':')
						ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("syntax error, bizzare label \"%s\"", $1.ident),
									parser_errposition(@1)));
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
					if ($2->typ != PLPSM_STMT_DECLARE_VARIABLE)
						yyerror("syntax error");
					$2->lineno = plpsm_location_to_lineno(@1);
					$$ = $2;
				}
			| DECLARE declare_prefetch CONDITION opt_sqlstate
				{
					if ($2->typ != PLPSM_STMT_DECLARE_CONDITION)
						yyerror("syntax error");
					$2->data = $4;
					$2->lineno = plpsm_location_to_lineno(@1);
					$$ = $2;
				}
			| DECLARE declare_prefetch CURSOR FOR 
				{
					int	tok;
					if ($2->typ != PLPSM_STMT_DECLARE_CURSOR)
						yyerror("syntax error");
					if ((tok = yylex()) != WORD)
					{
						plpsm_push_back_token(tok);
						$2->esql = read_embeded_sql(';', 0, 0, ";", PLPSM_ESQL_QUERY,
													    true, NULL, -1,
															    NULL, NULL);
					}
					else
						$2->name = yylval.word.ident;
					$2->lineno = plpsm_location_to_lineno(@1);
					$$ = $2;
				}
			| DECLARE declare_prefetch HANDLER FOR condition_list dstmt
				{
					if ($2->typ != PLPSM_STMT_DECLARE_HANDLER)
						yyerror("syntax error");
					$2->inner_left = $6;
					$2->data = $5;
					$2->lineno = plpsm_location_to_lineno(@1);
					$$ = $2;
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
					$$ = $1;
				}
			| condition_list ',' condition
				{
					$$ = $1;
					$$->next = $3;
				}
		;

opt_sqlstate:
			FOR SQLSTATE opt_value sqlstate
				{
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->typ = PLPSM_SQLSTATE;
					new->sqlstate = $4;
					$$ = new;
				}
			|	{
					$$ = NULL;
				}
		;

condition:
			SQLSTATE opt_value sqlstate
				{
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = @1;
					new->typ = PLPSM_SQLSTATE;
					new->sqlstate = $3;
					$$ = new;
				}
			| NOT FOUND
				{
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = @1;
					new->typ = PLPSM_SQLSTATE;
					new->sqlstate = MAKE_SQLSTATE('0','2','0','0','0');
					$$ = new;
				}
			| SQLWARNING
				{
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = @1;
					new->typ = PLPSM_SQLWARNING;
					$$ = new;
				}
			| SQLEXCEPTION
				{
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = @1;
					new->typ = PLPSM_SQLEXCEPTION;
					$$ = new;
				}
			| WORD
				{
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = @1;
					new->typ = PLPSM_CONDITION_NAME;
					new->condition_name = $1.ident;
					$$ = new;
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

					$$ = MAKE_SQLSTATE(sqlstatestr[0],
										  sqlstatestr[1],
										  sqlstatestr[2],
										  sqlstatestr[3],
										  sqlstatestr[4]);
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
					new->esql = $2;
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
					new->esql = $2;
					new->inner_left = $3;
					new->inner_right = $5;
					$$ = new;
				}
			| ELSE statements ';'
				{
					$$ = $2;
				}
		;

/*----
 * condition statement CASE
 *
 * CASE expr WHEN x [, ...] THEN ... [ ELSE ... ] END CASE
 * CASE WHEN expr THEN .. ... [ ELSE ...] END CASE
 */
stmt_case: 		CASE opt_expr_until_when case_when_list opt_case_else END CASE
					{
						Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CASE, @1);
						new->esql = $2;
						new->inner_left = $3;
						new->inner_right = $4;
						$$ = new;
					}
				;

case_when_list:
			case_when_list case_when
				{
					$1->last->next = $2;
					$1->last = $2;
				}
			| case_when
				{
					$$ = $1;
				}
		;

case_when:		WHEN expr_until_then statements ';'
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CASE, @1);
					new->esql = $2;
					new->inner_left = $3;
					new->last = new;
					$$ = new;
				}
			;

opt_case_else:
				{
					$$ = NULL;
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
					$$->location = @1;
				}
			| SET '(' qual_identif_list ')' '=' '(' expr_list ')'
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SET, @1);
					new->compound_target = $3;
					new->esql_list = $7;
					$$ = new;
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
					$1->esql = $3;
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
					$$ = new_qualid(list_make1($1.ident), @1);
				}
			| CWORD
				{
					ListCell	*l;
					List	*idents = NIL;
					foreach (l, $1.idents)
					{
						idents = lappend(idents, strVal(lfirst(l)));
					}
					$$ = new_qualid(idents, @1);
				}
			| PARAM
				{
					/* ToDo: Plpsm_object should be a param type too */
					char buf[32];
					snprintf(buf, sizeof(buf), "$%d", $1);
					$$ = $$ = new_qualid(list_make1(pstrdup(buf)), @1);
				}
		;

/*----
 * debug print
 *
 * PRINT expr
 *
 * note: this is only PostgreSQL feature
 */
stmt_print:		PRINT expr_list
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PRINT, @1);
					new->esql_list = $2;
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
						new->esql = read_expr_until_semi();
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
					new->esql = $3;
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
					new->esql = $6;
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
 * PREPARE name FROM expr
 *
 */
stmt_prepare:		PREPARE WORD FROM expr_until_semi
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PREPARE, @1);
					new->name = $2.ident;
					new->esql = $4;
					$$ = new;
				}
		;

/*----
 * execute a prepared statement
 *
 * EXECUTE name [ INTO variable [, variable [..]] ] [ USING var [, var [..]] ]
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
			| EXECUTE WORD INTO qual_identif_list
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, @1);
					new->name = $2.ident;
					new->compound_target = $4;
					$$ = new;
				}
			| EXECUTE WORD INTO qual_identif_list USING qual_identif_list
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, @1);
					new->name = $2.ident;
					new->compound_target = $4;
					new->variables = $6;
					$$ = new;
				}
			| EXECUTE WORD USING qual_identif_list
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, @1);
					new->name = $2.ident;
					new->variables = $4;
					$$ = new;
				}
		;

/*----
 * execute a dynamic query
 *
 * EXECUTE IMMEDIATE expr
 *
 */
stmt_execute_immediate:
			EXECUTE IMMEDIATE expr_until_semi_into_using
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE_IMMEDIATE, @1);
					new->esql = $3;
					$$ = new;
				}
		;

/*----
 * open a cursor, possible open a cursor based on prepared statement
 *
 * OPEN cursor_name [ USING var [, var [..] ]
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
			| OPEN qual_identif USING qual_identif_list
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_OPEN, @1);
					new->target = $2;
					new->variables = $4;
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
					new->target = $2;
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
					new->lineno = plpsm_location_to_lineno(new->location);
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
					char *loopvar_name = NULL;
					char *cursor_name = NULL;
					int tok;
					int	startloc = -1;
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_FOR, -1);

					/* read a possible loopvar_name */
					tok = yylex(); startloc = yylloc;
					if (tok  == WORD)
					{
						int tok1;

						loopvar_name = yylval.word.ident;
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
							cursor_name = loopvar_name;
							loopvar_name = NULL;
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

					if (loopvar_name != NULL || cursor_name != NULL)
					{
						new->stmtfor.cursor_name = cursor_name;
						new->stmtfor.loopvar_name = loopvar_name;
					}
					new->esql = read_embeded_sql(DO, -1, -1, "DO", PLPSM_ESQL_QUERY, true, NULL, startloc, NULL, NULL);
					$$ = new;
				}
		;

stmt_sql:
			INSERT 				{ $$ = make_stmt_sql(@1); }
			| UPDATE 			{ $$ = make_stmt_sql(@1); }
			| DELETE			{ $$ = make_stmt_sql(@1); }
			;

stmt_select_into:
			SELECT expr_list_into qual_identif_list 
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SELECT_INTO, @1);
					new->esql_list = $2;
					new->compound_target = $3;
					if (list_length($2) != list_length($3))
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("number of target variables is different than number of attributies"),
								 parser_errposition(@1)));

					$$ = new;
				}
			| SELECT expr_list_into qual_identif_list FROM
				{
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SELECT_INTO, @1);
					new->esql_list = $2;
					new->compound_target = $3;
					if (list_length($2) != list_length($3))
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("number of target variables is different than number of attributies"),
								 parser_errposition(@1)));

					new->from_clause = read_embeded_sql(';', 0, 0, ";", PLPSM_ESQL_QUERY, false, 
														    NULL, @4, 
															    NULL, NULL);
					$$ = new;
				}
			;

expr_list_into:
				{
					int endtok;
					List	*esql_list = NIL;

					do
					{
						Plpsm_ESQL *esql = read_embeded_sql(',',INTO, 0, ", or INTO", 
												PLPSM_ESQL_EXPR, true, 
														    &endtok, -1, 
															    NULL, NULL);
						if (endtok == ',')
							yylex();

						esql_list = lappend(esql_list, esql);
					} while (endtok != INTO);
					$$ = esql_list;
				}
		;

expr_until_semi:
				{
					$$ = read_expr_until_semi();
				}
		;

expr_list:
			expr_until_semi_or_coma_or_parent
				{
					$$ = list_make1($1);
				}
			| expr_list ',' expr_until_semi_or_coma_or_parent
				{
					$$ = lappend($1, $3);
				}
			;

expr_until_semi_or_coma:
				{
					Plpsm_ESQL *esql = read_embeded_sql(';',',', 0, "; or ,", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					$$ = esql;
				}
		;

expr_until_do:
				{
					Plpsm_ESQL *esql = read_embeded_sql(DO, -1, -1, "DO", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					$$ = esql;
				}
		;

expr_until_then:
				{
					Plpsm_ESQL *esql = read_embeded_sql(THEN, -1, -1, "THEN", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					$$ = esql;
				}
		;

expr_until_end:
				{
					Plpsm_ESQL *esql = read_embeded_sql(END, -1, -1, "END", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					$$ = esql;
				}
		;

expr_until_semi_into_using:
				{
					int endtok;
					Plpsm_ESQL *esql = read_embeded_sql(';',INTO, USING, "; or INTO or USING", 
												PLPSM_ESQL_EXPR, true, 
														    &endtok, -1, 
															    NULL, NULL);
					if (endtok != ';')
						plpsm_push_back_token(endtok);
					$$ = esql;
				}
		;

expr_until_semi_or_coma_or_parent:
				{
					int endtok;
					Plpsm_ESQL *esql = read_embeded_sql(';',',', ')', "; or , or )", 
												PLPSM_ESQL_EXPR, true, 
														    &endtok, -1, 
															    NULL, NULL);

					if (endtok != ';' && endtok != ',')
						plpsm_push_back_token(endtok);
					$$ = esql;
				}
		;

opt_expr_until_when:
				{
					Plpsm_ESQL *esql = NULL;
					int	tok = yylex();

					if (tok != WHEN)
					{
						esql = read_embeded_sql(WHEN,-1, -1, "WHEN", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, yylloc, 
															    NULL, NULL);
					}
					plpsm_push_back_token(WHEN);
					$$ = esql;
				}
		;

%%

static Plpsm_stmt *
make_stmt_sql(int location)
{
	Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SQL, location);
	new->esql = read_embeded_sql(';', 0, 0, ";", PLPSM_ESQL_QUERY, true, NULL, location, NULL, NULL);
	return new;
}

static Plpsm_ESQL
*read_expr_until_semi(void)
{
	return read_embeded_sql(';', 0, 0, ";", PLPSM_ESQL_EXPR, true, NULL, -1, NULL, NULL);
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

Plpsm_positioned_qualid *
new_qualid(List *qualId, int location)
{
	Plpsm_positioned_qualid *new = palloc(sizeof(Plpsm_positioned_qualid));
	new->lineno = plpsm_location_to_lineno(location);
	new->location = location;
	new->qualId = qualId;
	return new;
}

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
			Plpsm_ESQL *datatype;
			Oid	typoid;
			int32	typmod;
			int16		typlen;
			bool		typbyval;

			result->typ = PLPSM_STMT_DECLARE_VARIABLE;
			result->compound_target = varnames;

			/* 
			 * ToDo: better to use a special function than read_until,
			 * because it raise a error to late. Datatype must not contains
			 * a keywords, special chars etc
			 */
			datatype = read_embeded_sql(';', DEFAULT, -1, "; or \"DEFAULT\"", PLPSM_ESQL_DATATYPE, false, &endtok, 
															    startlocation,
																&typoid,
																&typmod);
			get_typlenbyval(typoid, &typlen, &typbyval);

			result->datum.typoid = typoid;
			result->datum.typmod = typmod;
			result->datum.typname = datatype->sqlstr;
			result->datum.typlen = typlen;
			result->datum.typbyval = typbyval;

			if (endtok == ';')
				break;

			/* when DEFAULT value is specified, then read a expression until semicolon */
			result->esql = read_expr_until_semi();
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
				varnames = list_make1(new_qualid(list_make1(yylval.word.ident), yylloc));
				state = Unknown;
				continue;
			}
			else if (tok == CONTINUE || tok == EXIT || tok == UNDO)
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
					switch (tok)
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
					varnames = list_make1(new_qualid(list_make1(pstrdup(varname)), yylloc));
					state = Unknown;
					continue;
				}
			}
			else if (is_unreserved_keyword(tok))
			{
				varnames = list_make1(new_qualid(list_make1(yylval.word.ident), yylloc));
				state = Unknown;
				continue;
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
			varnames = lappend(varnames, new_qualid(list_make1(yylval.word.ident), yylloc));
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
		case ATOMIC:
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
		case SQLCODE:
		case UNDO:
			return true;
		default:
			return false;
	}
}

Plpsm_stmt *
plpsm_new_stmt(Plpsm_stmt_type typ, int location)
{
	Plpsm_stmt *n = palloc0(sizeof(Plpsm_stmt));
	n->typ = typ;
	if ((n->location = location) != -1)
		n->lineno = plpsm_location_to_lineno(location);
	return n;
}

/*
 * ensure so end label is same as begin label
 */
static void
check_labels(const char *label1, const char *label2)
{
	if (label2 != NULL && label1 == NULL)
		yyerror("syntax error, missing begin label");
	if (label2 == NULL || label1 == NULL)
		return;
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
		case PLPSM_STMT_CASE:
			return "case statement";
		default:
			return "unknown statment typid";
	}
}

/*----
 * Debug out routines 
 *
 */
static void
esql_out(StringInfo ds, Plpsm_ESQL *esql)
{
	if (esql == NULL)
		return;

	switch (esql->typ)
	{
		case PLPSM_ESQL_EXPR:
			appendStringInfo(ds, "EXPR: %s", esql->sqlstr);
			break;
		case PLPSM_ESQL_QUERY:
			appendStringInfo(ds, "QUERY: %s", esql->sqlstr);
			break;
		case PLPSM_ESQL_DATATYPE:
			appendStringInfo(ds, "DATATYPE: %s", esql->sqlstr);
			break;
	}
}

static void 
esql_list_out(StringInfo ds, List *list)
{
	ListCell	*l;
	bool	isFirst = true;

	foreach (l, list)
	{
		if (!isFirst)
			appendStringInfoChar(ds, ',');
		else
			isFirst = false;
		esql_out(ds, (Plpsm_ESQL *) lfirst(l));
	}
}

static void
pqualid_out(StringInfo ds, Plpsm_positioned_qualid *qualid)
{
	bool	isFirst = true;
	ListCell	*l;

	if (qualid == NULL)
		return;

	foreach(l, qualid->qualId)
	{
		if (!isFirst)
			appendStringInfoChar(ds, '.');
		else
			isFirst = false;
		appendStringInfoString(ds, (char *) lfirst(l));
	}
}

static void
pqualid_list_out(StringInfo ds, List *list)
{
	bool isFirst = true;
	ListCell	*l;

	foreach (l, list)
	{
		if (!isFirst)
			appendStringInfoChar(ds, ',');
		else
			isFirst = false;
		pqualid_out(ds, (Plpsm_positioned_qualid *) lfirst(l));
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
	appendStringInfo(ds, "%s| Target: ", ident);
	pqualid_out(ds, stmt->target);
	appendStringInfoChar(ds, '\n');
	appendStringInfo(ds, "%s| Compound target: ", ident);
	pqualid_list_out(ds,stmt->compound_target);
	appendStringInfoChar(ds,'\n');
	appendStringInfo(ds, "%s| Variables: ", ident);
	pqualid_list_out(ds, stmt->variables);
	appendStringInfoChar(ds, '\n');
	appendStringInfo(ds, "%s| Option: %d\n", ident, stmt->option);
	appendStringInfo(ds, "%s| ESQL: ", ident);
	esql_out(ds, stmt->esql);
	appendStringInfoChar(ds, '\n');
	appendStringInfo(ds, "%s| ESQL list: ", ident);
	esql_list_out(ds, stmt->esql_list);
	appendStringInfoChar(ds, '\n');

	switch (stmt->typ)
	{
		case PLPSM_STMT_FOR:
			{
				appendStringInfo(ds, "%s| Data: loopvar:%s, cursor:%s\n", ident,
										stmt->stmtfor.loopvar_name,
										stmt->stmtfor.cursor_name);
			}
			break;
		case PLPSM_STMT_DECLARE_HANDLER:
			{
				Plpsm_condition_value *condition = (Plpsm_condition_value *) stmt->data;

				appendStringInfo(ds, "%s| condtions:", ident);
				while (condition != NULL)
				{
					switch (condition->typ)
					{
						case PLPSM_SQLSTATE:
							appendStringInfo(ds, " %s", unpack_sql_state(condition->sqlstate));
							break;
						case PLPSM_SQLEXCEPTION:
							appendStringInfoString(ds, " SQLEXCEPTION");
							break;
						case PLPSM_SQLWARNING:
							appendStringInfoString(ds, " SQLWARNING");
							break;
						case PLPSM_CONDITION_NAME:
							appendStringInfo(ds, " %s", condition->condition_name);
							break;
					}
					condition = condition->next;
				}
				appendStringInfoChar(ds, '\n');
				break;
			}
		default:
			/* do nothing */ ;
	}

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

/*
 * when optional_endtoken is true, then expression can be finished
 * without identification of any until_tokens.
 */
static Plpsm_ESQL *
read_embeded_sql(int until1,
			int until2,
			int until3,
			const char *expected,
			Plpsm_esql_type expected_type,
			bool valid_sql,
			int *endtoken,
			int startlocation,
			Oid *typoid,
			int32 *typmod)
{
	int				parenlevel = 0;
	int tok;
	StringInfoData		ds;
	Plpsm_ESQL	*esql = palloc(sizeof(Plpsm_ESQL));

	esql->typ = expected_type;

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
			if (tok == ';' || tok == 0 || parenlevel == 0 || 
					(tok == ')' && parenlevel == 0) ||
					(tok == ',' && parenlevel == 0))
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

		/* special rules for plpgsql users. Datatype cannot to contain '=' */
		if (tok == '=' && expected_type == PLPSM_ESQL_DATATYPE)
		{
			plpsm_push_back_token(tok);
			break;
		}

		/* PSM keywords without a few exceptions are prohibited */
		switch (tok)
		{
			case BEGIN:
			case CLOSE:
			case CONDITION:
			case CURSOR:
			case DECLARE:
			case DEFAULT:
			case DO:
			case ELSEIF:
			case EXIT:
			case FETCH:
			case FOR:
			case HANDLER:
			case IF:
			case ITERATE:
			case LEAVE:
			case LOOP:
			case OPEN:
			case PREPARE:
			case PRINT:
			case REPEAT:
			case RETURN:
			case WHILE:
			case UNTIL:
				yyerror("using not allowed PLPSM keyword");
		}

		/* expression, or query, or dataype must not contains a semicolon ever */
		if (tok == ';')
		{
			plpsm_push_back_token(tok);
			break;
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
			if (expected_type == PLPSM_ESQL_EXPR)
			{
				StringInfoData	bexpr;

				if (startlocation >= yylloc)
					yyerror("missing expression");

				initStringInfo(&bexpr);
				appendStringInfoString(&bexpr, "SELECT (");
				plpsm_append_source_text(&bexpr, startlocation, yylloc);
				check_sql_expr(bexpr.data, startlocation, strlen("SELECT ("));
				pfree(bexpr.data);
			}

			ereport(ERROR,
					(errcode(ERRCODE_SYNTAX_ERROR),
					 errmsg("missing \"%s\" at end of SQL expression",
										    expected),
					 parser_errposition(yylloc)));
		}
	}

	if (parenlevel != 0)
		yyerror("mismatched parentheses");

	if (endtoken)
		*endtoken = tok;

	if (expected_type == PLPSM_ESQL_EXPR)
	{
		StringInfoData	bexpr;

		if (startlocation >= yylloc)
			yyerror("missing expression");
		
		plpsm_append_source_text(&ds, startlocation, yylloc);

		if (valid_sql)
		{
			initStringInfo(&bexpr);
			appendStringInfoString(&bexpr, "SELECT (");
			plpsm_append_source_text(&bexpr, startlocation, yylloc);
			appendStringInfoChar(&bexpr, ')');
			check_sql_expr(bexpr.data, startlocation, strlen("SELECT ("));
			pfree(bexpr.data);
		}
	}
	else if (expected_type == PLPSM_ESQL_DATATYPE)
	{
		if (startlocation >= yylloc)
			yyerror("missing data type");

		plpsm_append_source_text(&ds, startlocation, yylloc);
		parse_datatype(ds.data, startlocation, typoid, typmod);
	}
	else
	{
		plpsm_append_source_text(&ds, startlocation, yylloc);

		if (valid_sql)
			check_sql_expr(ds.data, startlocation, 0);
	}

	esql->location = startlocation;
	esql->lineno = plpsm_location_to_lineno(startlocation);
	esql->sqlstr = ds.data;

	/* try to truncate from right a returned string */
	while (isspace(ds.data[ds.len - 1]))
		ds.data[--ds.len] = '\0';

	/* semicolon is usually returned back */
	if (tok == ';' || tok == ',')
		plpsm_push_back_token(tok);

	return esql;
}

static void
check_sql_expr(const char *stmt, int location, int leaderlen)
{
	Plpsm_sql_error_callback_arg cbarg;
	ErrorContextCallback  syntax_errcontext;
	MemoryContext oldCxt;

	cbarg.location = location;
	cbarg.leaderlen = leaderlen;

	syntax_errcontext.callback = plpsm_sql_error_callback;
	syntax_errcontext.arg = &cbarg;
	syntax_errcontext.previous = error_context_stack;
	error_context_stack = &syntax_errcontext;

	oldCxt = MemoryContextSwitchTo(plpsm_compile_tmp_cxt);
	(void) raw_parser(stmt);
	MemoryContextSwitchTo(oldCxt);
	MemoryContextReset(plpsm_compile_tmp_cxt);

	/* Restore former ereport callback */
	error_context_stack = syntax_errcontext.previous;
}

void
plpsm_sql_error_callback(void *arg)
{
	Plpsm_sql_error_callback_arg *cbarg = (Plpsm_sql_error_callback_arg *) arg;
	int			errpos;

	/*
	 * First, set up internalerrposition to point to the start of the
	 * statement text within the function text.  Note this converts
	 * location (a byte offset) to a character number.
	 */
	parser_errposition(cbarg->location);

	/*
	 * If the core parser provided an error position, transpose it.
	 * Note we are dealing with 1-based character numbers at this point.
	 */
	errpos = geterrposition();

	if (errpos > cbarg->leaderlen)
	{
		int		myerrpos = getinternalerrposition();

		if (myerrpos > 0)		/* safety check */
			internalerrposition(myerrpos + errpos - cbarg->leaderlen - 1);
	}

	/* In any case, flush errposition --- we want internalerrpos only */
	errposition(0);
}

/*
 * Parse a SQL datatype name.
 *
 * The heavy lifting is done elsewhere.  Here we are only concerned
 * with setting up an errcontext link that will let us give an error
 * cursor pointing into the plpgsql function source, if necessary.
 * This is handled the same as in check_sql_expr(), and we likewise
 * expect that the given string is a copy from the source text.
 */
static void
parse_datatype(const char *string, int location, Oid *typoid, int32 *typmod)
{
	Plpsm_sql_error_callback_arg cbarg;
	ErrorContextCallback  syntax_errcontext;

	cbarg.location = location;
	cbarg.leaderlen = 0;

	syntax_errcontext.callback = plpsm_sql_error_callback;
	syntax_errcontext.arg = &cbarg;
	syntax_errcontext.previous = error_context_stack;
	error_context_stack = &syntax_errcontext;

	/* Let the main parser try to parse it under standard SQL rules */
	parseTypeString(string, typoid, typmod);

	/* Restore former ereport callback */
	error_context_stack = syntax_errcontext.previous;
}
