/*-------------------------------------------------------------------------
 *
 * scanner.c
 *	  lexical scanning for PSM0
 *
 *
 * Portions Copyright (c) 1996-2010, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/pl/plpsm/src/pl_scanner.c
 *
 *-------------------------------------------------------------------------
 */
#include "psm.h"

#include "mb/pg_wchar.h"
#include "nodes/value.h"
#include "parser/scanner.h"

#include "gram.h"			/* must be after parser/scanner.h */

#define PG_KEYWORD(a,b,c) {a,b,c},

/*
 * A word about keywords:
 *
 * We keep reserved and unreserved keywords in separate arrays.  The
 * reserved keywords are passed to the core scanner, so they will be
 * recognized before (and instead of) any variable name.  Unreserved
 * words are checked for separately, after determining that the identifier
 * isn't a known variable name.  If plpsm_IdentifierLookup is DECLARE then
 * no variable names will be recognized, so the unreserved words always work.
 * (Note in particular that this helps us avoid reserving keywords that are
 * only needed in DECLARE sections.)
 *
 * In certain contexts it is desirable to prefer recognizing an unreserved
 * keyword over recognizing a variable name.  Those cases are handled in
 * gram.y using tok_is_keyword().
 *
 * For the most part, the reserved keywords are those that start a PL/pgSQL
 * statement (and so would conflict with an assignment to a variable of the
 * same name).	We also don't sweat it much about reserving keywords that
 * are reserved in the core grammar.  Try to avoid reserving other words.
 */

/*
 * Lists of keyword (name, token-value, category) entries.
 *
 * !!WARNING!!: These lists must be sorted by ASCII name, because binary
 *		 search is used to locate entries.
 *
 * Be careful not to put the same word in both lists.  Also be sure that
 * gram.y's unreserved_keyword production agrees with the second list.
 */

static const ScanKeyword reserved_keywords[] = {
	PG_KEYWORD("begin", BEGIN, RESERVED_KEYWORD)
	PG_KEYWORD("case", CASE, RESERVED_KEYWORD)
	PG_KEYWORD("close", CLOSE, RESERVED_KEYWORD)
	PG_KEYWORD("condition",CONDITION, RESERVED_KEYWORD)
	PG_KEYWORD("cursor",CURSOR, RESERVED_KEYWORD)
	PG_KEYWORD("declare", DECLARE, RESERVED_KEYWORD)
	PG_KEYWORD("default", DEFAULT, RESERVED_KEYWORD)
	PG_KEYWORD("delete", DELETE, RESERVED_KEYWORD)
	PG_KEYWORD("do", DO, RESERVED_KEYWORD)
	PG_KEYWORD("else", ELSE, RESERVED_KEYWORD)
	PG_KEYWORD("elseif", ELSEIF, RESERVED_KEYWORD)
	PG_KEYWORD("end", END, RESERVED_KEYWORD)
	PG_KEYWORD("execute", EXECUTE, RESERVED_KEYWORD)
	PG_KEYWORD("exit",EXIT, RESERVED_KEYWORD)
	PG_KEYWORD("fetch", FETCH, RESERVED_KEYWORD)
	PG_KEYWORD("for", FOR, RESERVED_KEYWORD)
	PG_KEYWORD("from", FROM, RESERVED_KEYWORD)
	PG_KEYWORD("handler", HANDLER, RESERVED_KEYWORD)
	PG_KEYWORD("if", IF, RESERVED_KEYWORD)
	PG_KEYWORD("insert", INSERT, RESERVED_KEYWORD)
	PG_KEYWORD("into", INTO, RESERVED_KEYWORD)
	PG_KEYWORD("iterate", ITERATE, RESERVED_KEYWORD)
	PG_KEYWORD("leave", LEAVE, RESERVED_KEYWORD)
	PG_KEYWORD("loop", LOOP, RESERVED_KEYWORD)
	PG_KEYWORD("open", OPEN, RESERVED_KEYWORD)
	PG_KEYWORD("prepare", PREPARE, RESERVED_KEYWORD)
	PG_KEYWORD("print", PRINT, RESERVED_KEYWORD)
	PG_KEYWORD("repeat", REPEAT, RESERVED_KEYWORD)
	PG_KEYWORD("return", RETURN, RESERVED_KEYWORD)
	PG_KEYWORD("select", SELECT, RESERVED_KEYWORD)
	PG_KEYWORD("set", SET, RESERVED_KEYWORD)
	PG_KEYWORD("then", THEN, RESERVED_KEYWORD)
	PG_KEYWORD("until", UNTIL, RESERVED_KEYWORD)
	PG_KEYWORD("update", UPDATE, RESERVED_KEYWORD)
	PG_KEYWORD("using", USING, RESERVED_KEYWORD)
	PG_KEYWORD("value", VALUE, RESERVED_KEYWORD)
	PG_KEYWORD("when", WHEN, RESERVED_KEYWORD)
	PG_KEYWORD("while", WHILE, RESERVED_KEYWORD)
};

static const int num_reserved_keywords = lengthof(reserved_keywords);

static const ScanKeyword unreserved_keywords[] = {
	PG_KEYWORD("as", AS, UNRESERVED_KEYWORD)
	PG_KEYWORD("continue", CONTINUE, UNRESERVED_KEYWORD)
	PG_KEYWORD("exit", EXIT, UNRESERVED_KEYWORD)
	PG_KEYWORD("found", FOUND, UNRESERVED_KEYWORD)
	PG_KEYWORD("immediate", IMMEDIATE, UNRESERVED_KEYWORD)
	PG_KEYWORD("no", NO, UNRESERVED_KEYWORD)
	PG_KEYWORD("not", NOT, UNRESERVED_KEYWORD)
	PG_KEYWORD("scroll", SCROLL, UNRESERVED_KEYWORD)
	PG_KEYWORD("sqlexception", SQLEXCEPTION, UNRESERVED_KEYWORD)
	PG_KEYWORD("sqlcode", SQLCODE, UNRESERVED_KEYWORD)
	PG_KEYWORD("sqlstate", SQLSTATE, UNRESERVED_KEYWORD)
	PG_KEYWORD("sqlwarning", SQLWARNING, UNRESERVED_KEYWORD)
	PG_KEYWORD("undo", UNDO, UNRESERVED_KEYWORD)
};

static const int num_unreserved_keywords = lengthof(unreserved_keywords);

/* Auxiliary data about a token (other than the token type) */
typedef struct
{
	YYSTYPE		lval;			/* semantic information */
	YYLTYPE		lloc;			/* offset in scanbuf */
	int			leng;			/* length in bytes */
} TokenAuxData;

/*
 * Scanner working state.  At some point we might wish to fold all this
 * into a YY_EXTRA struct.	For the moment, there is no need for plpsm's
 * lexer to be re-entrant, and the notational burden of passing a yyscanner
 * pointer around is great enough to not want to do it without need.
 */

/* The stuff the core lexer needs */
static core_yyscan_t yyscanner = NULL;
static core_yy_extra_type core_yy;

/* The original input string */
static const char *scanorig;

/* Current token's length */
static int	plpsm_yyleng;

/* Token pushback stack */
#define MAX_PUSHBACKS 4

static int	num_pushbacks;
static int	pushback_token[MAX_PUSHBACKS];
static TokenAuxData pushback_auxdata[MAX_PUSHBACKS];

static int internal_yylex(TokenAuxData *auxdata);
static void parse_word(char *word1, const char *yytxt, PLword *word);
static void push_back_token(int token, TokenAuxData *auxdata);


/*
 * This is the yylex routine called from the PL/PSM grammar.
 * It is a wrapper around the core lexer. It returns a
 * T_WORD or T_CWORD, or as an unreserved keyword if it
 * matches one of those.
 */
int
plpsm_yylex(void)
{
	int			tok1;
	TokenAuxData aux1;
	const ScanKeyword *kw;

	tok1 = internal_yylex(&aux1);
	if (tok1 == IDENT)
	{
		int			tok2;
		TokenAuxData aux2;

		tok2 = internal_yylex(&aux2);
		if (tok2 == '.')
		{
			int			tok3;
			TokenAuxData aux3;

			tok3 = internal_yylex(&aux3);
			if (tok3 == IDENT)
			{
				int			tok4;
				TokenAuxData aux4;

				tok4 = internal_yylex(&aux4);
				if (tok4 == '.')
				{
					int			tok5;
					TokenAuxData aux5;

					tok5 = internal_yylex(&aux5);
					if (tok5 == IDENT)
					{
						aux1.lval.cword.idents = list_make3(makeString(aux1.lval.str),
														makeString(aux3.lval.str),
														makeString(aux5.lval.str));
						tok1 = CWORD;
					}
					else
					{
						/* not A.B.C, so just process A.B */
						push_back_token(tok5, &aux5);
						push_back_token(tok4, &aux4);
						
						aux1.lval.cword.idents = list_make2(makeString(aux1.lval.str),
														makeString(aux3.lval.str));
						tok1 = CWORD;
					}
				}
				else
				{
					/* not A.B.C, so just process A.B */
					push_back_token(tok4, &aux4);
					aux1.lval.cword.idents = list_make2(makeString(aux1.lval.str),
													makeString(aux3.lval.str));
					tok1 = CWORD;
				}
			}
			else
			{
				/* not A.B, so just process A */
				push_back_token(tok3, &aux3);
				push_back_token(tok2, &aux2);

				parse_word(aux1.lval.str, core_yy.scanbuf + aux1.lloc,  &aux1.lval.word);
				if (!aux1.lval.word.quoted &&
						 (kw = ScanKeywordLookup(aux1.lval.word.ident,
												 unreserved_keywords,
												 num_unreserved_keywords)))
				{
					aux1.lval.keyword = kw->name;
					tok1 = kw->value;
				}
				else
					tok1 = WORD;
			}
		}
		else
		{
			/* not A.B, so just process A */
			push_back_token(tok2, &aux2);

			parse_word(aux1.lval.str, core_yy.scanbuf + aux1.lloc,  &aux1.lval.word);
			if (!aux1.lval.word.quoted &&
					 (kw = ScanKeywordLookup(aux1.lval.word.ident,
											 unreserved_keywords,
											 num_unreserved_keywords)))
			{
				aux1.lval.keyword = kw->name;
				tok1 = kw->value;
			}
			else
				tok1 = WORD;
		}
	}

	plpsm_yylval = aux1.lval;
	plpsm_yylloc = aux1.lloc;
	plpsm_yyleng = aux1.leng;
	return tok1;
}

static void
parse_word(char *word1, const char *yytxt, PLword *word)
{
	word->ident = word1;
	word->quoted = (yytxt[0] == '"');
}


/*
 * Internal yylex function.  This wraps the core lexer and adds one feature:
 * a token pushback stack.	We also make a couple of trivial single-token
 * translations from what the core lexer does to what we want, in particular
 * interfacing from the core_YYSTYPE to YYSTYPE union.
 */
static int
internal_yylex(TokenAuxData *auxdata)
{
	int			token;
	const char *yytext;

	if (num_pushbacks > 0)
	{
		num_pushbacks--;
		token = pushback_token[num_pushbacks];
		*auxdata = pushback_auxdata[num_pushbacks];
	}
	else
	{
		token = core_yylex(&auxdata->lval.core_yystype,
						   &auxdata->lloc,
						   yyscanner);

		/* remember the length of yytext before it gets changed */
		yytext = core_yy.scanbuf + auxdata->lloc;
		auxdata->leng = strlen(yytext);
	}

	return token;
}

void
plpsm_yyerror(const char *message)
{
	char	   *yytext = core_yy.scanbuf + plpsm_yylloc;

	if (*yytext == '\0')
	{
		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
		/* translator: %s is typically the translation of "syntax error" */
				 errmsg("%s at end of input", _(message))));
	}
	else
	{
		/*
		 * If we have done any lookahead then flex will have restored the
		 * character after the end-of-token.  Zap it again so that we report
		 * only the single token here.	This modifies scanbuf but we no longer
		 * care about that.
		 */
		yytext[plpsm_yyleng] = '\0';

		ereport(ERROR,
				(errcode(ERRCODE_SYNTAX_ERROR),
		/* translator: first %s is typically the translation of "syntax error" */
				 errmsg("%s at or near \"%s\"", _(message), yytext)));
	}
}


void
plpsm_scanner_init(const char *str)
{
	num_pushbacks = 0;
	plpsm_yylloc = 0;

	yyscanner = scanner_init(str, &core_yy,
						reserved_keywords, num_reserved_keywords);
	scanorig = str;
}

void
plpsm_scanner_finish(void)
{
	scanner_finish(yyscanner);
	yyscanner = NULL;
	scanorig = NULL;
}

/*
 * Push back a token to be re-read by next internal_yylex() call.
 */
static void
push_back_token(int token, TokenAuxData *auxdata)
{
	if (num_pushbacks >= MAX_PUSHBACKS)
		elog(ERROR, "too many tokens pushed back");
	pushback_token[num_pushbacks] = token;
	pushback_auxdata[num_pushbacks] = *auxdata;
	num_pushbacks++;
}

/*
 * Push back a single token to be re-read by next plpsm_yylex() call.
 *
 * NOTE: this does not cause yylval or yylloc to "back up".  Also, it
 * is not a good idea to push back a token code other than what you read.
 */
void
plpsm_push_back_token(int token)
{
	TokenAuxData auxdata;

	auxdata.lval = plpsm_yylval;
	auxdata.lloc = plpsm_yylloc;
	auxdata.leng = plpsm_yyleng;
	push_back_token(token, &auxdata);
}

void
plpsm_append_source_text(StringInfo buf,
						   int startlocation, int endlocation)
{
	Assert(startlocation <= endlocation);
	appendBinaryStringInfo(buf, scanorig + startlocation,
						   endlocation - startlocation);
}
