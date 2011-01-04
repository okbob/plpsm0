
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse         plpsm_yyparse
#define yylex           plpsm_yylex
#define yyerror         plpsm_yyerror
#define yylval          plpsm_yylval
#define yychar          plpsm_yychar
#define yydebug         plpsm_yydebug
#define yynerrs         plpsm_yynerrs
#define yylloc          plpsm_yylloc

/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "gram.y"

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

static char * read_until(int until1, int until2, int until3, const char *expected, 
							bool is_expr, bool is_datatype, 
							int *endtoken,
							int startlocation);
static void check_sql_expr(const char *stmt);
static void parse_datatype(const char *string, Oid *type_id, int32 *typmod);

static char *read_expr_until_semi(void);
static Plpsm_stmt *declare_prefetch(void);
static Plpsm_stmt *new_stmt(Plpsm_stmt_type typ, int location);
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



/* Line 189 of yacc.c  */
#line 161 "gram.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT = 258,
     FCONST = 259,
     SCONST = 260,
     BCONST = 261,
     XCONST = 262,
     Op = 263,
     ICONST = 264,
     PARAM = 265,
     TYPECAST = 266,
     DOT_DOT = 267,
     COLON_EQUALS = 268,
     WORD = 269,
     CWORD = 270,
     AS = 271,
     BEGIN = 272,
     CLOSE = 273,
     CONDITION = 274,
     CONTINUE = 275,
     CURSOR = 276,
     DECLARE = 277,
     DEFAULT = 278,
     DO = 279,
     ELSE = 280,
     ELSEIF = 281,
     END = 282,
     EXECUTE = 283,
     EXIT = 284,
     FETCH = 285,
     FOR = 286,
     FOUND = 287,
     FROM = 288,
     HANDLER = 289,
     IF = 290,
     IMMEDIATE = 291,
     INTO = 292,
     ITERATE = 293,
     LEAVE = 294,
     LOOP = 295,
     NO = 296,
     NOT = 297,
     OPEN = 298,
     PREPARE = 299,
     PRINT = 300,
     REPEAT = 301,
     RETURN = 302,
     SCROLL = 303,
     SET = 304,
     SQLEXCEPTION = 305,
     SQLSTATE = 306,
     SQLWARNING = 307,
     THEN = 308,
     UNDO = 309,
     UNTIL = 310,
     USING = 311,
     VALUE = 312,
     WHILE = 313
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 84 "gram.y"

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



/* Line 214 of yacc.c  */
#line 272 "gram.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 297 "gram.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  52
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   391

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  65
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  49
/* YYNRULES -- Number of rules.  */
#define YYNRULES  97
/* YYNRULES -- Number of states.  */
#define YYNSTATES  175

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   313

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      63,    64,     2,     2,    61,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    60,    59,
       2,    62,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    49,    51,    58,    65,    74,    79,
      82,    83,    85,    86,    88,    92,    95,   100,   106,   113,
     114,   116,   120,   125,   126,   130,   133,   135,   137,   139,
     141,   143,   144,   152,   153,   159,   163,   166,   168,   172,
     176,   178,   182,   184,   188,   190,   192,   195,   197,   205,
     214,   223,   226,   229,   235,   236,   239,   244,   251,   256,
     260,   266,   272,   280,   283,   288,   293,   296,   305,   306,
     307,   309,   313,   314,   315,   316,   317,   318
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      66,     0,    -1,    69,    67,    -1,    59,    -1,    -1,    69,
      -1,    68,    59,    69,    -1,    70,    -1,    71,    -1,    84,
      -1,    90,    -1,    92,    -1,    93,    -1,    94,    -1,    95,
      -1,    96,    -1,    91,    -1,    97,    -1,    99,    -1,   100,
      -1,   101,    -1,   102,    -1,   103,    -1,   104,    -1,    82,
      -1,    72,    17,    68,    59,    27,    73,    -1,    72,    17,
      74,    59,    27,    73,    -1,    72,    17,    74,    59,    68,
      59,    27,    73,    -1,    72,    17,    27,    73,    -1,    14,
      60,    -1,    -1,    14,    -1,    -1,    75,    -1,    74,    59,
      75,    -1,    22,    76,    -1,    22,    76,    19,    78,    -1,
      22,    76,    21,    31,   113,    -1,    22,    76,    34,    31,
      77,    69,    -1,    -1,    79,    -1,    77,    61,    79,    -1,
      31,    51,    81,    80,    -1,    -1,    51,    81,    80,    -1,
      42,    32,    -1,    52,    -1,    50,    -1,    14,    -1,     5,
      -1,    57,    -1,    -1,    35,   110,    68,    59,    83,    27,
      35,    -1,    -1,    26,   110,    68,    59,    83,    -1,    25,
      68,    59,    -1,    49,    85,    -1,    86,    -1,    85,    61,
      86,    -1,    87,    62,   108,    -1,    89,    -1,    63,    88,
      64,    -1,    89,    -1,    88,    61,    89,    -1,    14,    -1,
      15,    -1,    45,   106,    -1,    47,    -1,    72,    40,    68,
      59,    27,    40,    73,    -1,    72,    58,   109,    68,    59,
      27,    58,    73,    -1,    72,    46,    68,    59,    55,   111,
      46,    73,    -1,    38,    14,    -1,    39,    14,    -1,    44,
      14,    98,    33,   106,    -1,    -1,    28,    14,    -1,    28,
      14,    37,    87,    -1,    28,    14,    37,    87,    56,   107,
      -1,    28,    14,    56,   107,    -1,    28,    36,   112,    -1,
      28,    36,   112,    37,    87,    -1,    28,    36,   112,    56,
     107,    -1,    28,    36,   112,    37,    87,    56,   107,    -1,
      43,    89,    -1,    43,    89,    56,   107,    -1,    30,    89,
      37,    88,    -1,    18,    89,    -1,    72,    31,   105,    68,
      59,    27,    31,    73,    -1,    -1,    -1,   108,    -1,   107,
      61,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   181,   181,   189,   193,   198,   202,   213,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   246,   262,   270,   278,   289,   299,
     304,   310,   315,   321,   326,   362,   370,   379,   388,   401,
     407,   411,   418,   422,   428,   433,   439,   446,   453,   459,
     479,   482,   491,   502,   505,   515,   531,   538,   543,   551,
     560,   566,   575,   579,   586,   590,   603,   618,   641,   657,
     674,   691,   705,   720,   731,   770,   776,   784,   793,   809,
     815,   823,   830,   849,   855,   870,   885,   900,   916,   982,
     988,   992,   999,  1014,  1028,  1042,  1056,  1073
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT", "FCONST", "SCONST", "BCONST",
  "XCONST", "Op", "ICONST", "PARAM", "TYPECAST", "DOT_DOT", "COLON_EQUALS",
  "WORD", "CWORD", "AS", "BEGIN", "CLOSE", "CONDITION", "CONTINUE",
  "CURSOR", "DECLARE", "DEFAULT", "DO", "ELSE", "ELSEIF", "END", "EXECUTE",
  "EXIT", "FETCH", "FOR", "FOUND", "FROM", "HANDLER", "IF", "IMMEDIATE",
  "INTO", "ITERATE", "LEAVE", "LOOP", "NO", "NOT", "OPEN", "PREPARE",
  "PRINT", "REPEAT", "RETURN", "SCROLL", "SET", "SQLEXCEPTION", "SQLSTATE",
  "SQLWARNING", "THEN", "UNDO", "UNTIL", "USING", "VALUE", "WHILE", "';'",
  "':'", "','", "'='", "'('", "')'", "$accept", "function", "opt_semi",
  "statements", "dstmt", "stmt", "stmt_compound", "opt_label",
  "opt_end_label", "declarations", "declaration", "declare_prefetch",
  "condition_list", "opt_sqlstate", "condition", "sqlstate", "opt_value",
  "stmt_if", "stmt_else", "stmt_set", "assign_list", "assign_item",
  "target", "qual_identif_list", "qual_identif", "stmt_print",
  "stmt_return", "stmt_loop", "stmt_while", "stmt_repeat_until",
  "stmt_iterate", "stmt_leave", "stmt_prepare", "param_types_opt",
  "stmt_execute", "stmt_execute_immediate", "stmt_open", "stmt_fetch",
  "stmt_close", "stmt_for", "for_prefetch", "expr_until_semi", "expr_list",
  "expr_until_semi_or_coma", "expr_until_do", "expr_until_then",
  "expr_until_end", "expr_until_semi_into_using", "cursor_def", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,    59,
      58,    44,    61,    40,    41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    65,    66,    67,    67,    68,    68,    69,    70,    70,
      70,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    70,    71,    71,    71,    71,    72,
      72,    73,    73,    74,    74,    75,    75,    75,    75,    76,
      77,    77,    78,    78,    79,    79,    79,    79,    79,    80,
      81,    81,    82,    83,    83,    83,    84,    85,    85,    86,
      87,    87,    88,    88,    89,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,    99,    99,    99,   100,
     100,   100,   100,   101,   101,   102,   103,   104,   105,   106,
     107,   107,   108,   109,   110,   111,   112,   113
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     0,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     6,     8,     4,     2,
       0,     1,     0,     1,     3,     2,     4,     5,     6,     0,
       1,     3,     4,     0,     3,     2,     1,     1,     1,     1,
       1,     0,     7,     0,     5,     3,     2,     1,     3,     3,
       1,     3,     1,     3,     1,     1,     2,     1,     7,     8,
       8,     2,     2,     5,     0,     2,     4,     6,     4,     3,
       5,     5,     7,     2,     4,     4,     2,     8,     0,     0,
       1,     3,     0,     0,     0,     0,     0,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      30,     0,     0,     0,     0,    94,     0,     0,     0,     0,
      89,    67,     0,     0,     4,     7,     8,     0,    24,     9,
      10,    16,    11,    12,    13,    14,    15,    17,    18,    19,
      20,    21,    22,    23,    29,    64,    65,    86,    75,    96,
       0,    30,    71,    72,    83,    74,    66,     0,    56,    57,
       0,    60,     1,     3,     2,    30,    88,    30,    30,    93,
       0,    92,    79,     0,     0,     5,    92,     0,     0,    62,
       0,    92,    39,    32,     0,     0,    33,    30,     0,     0,
      30,    76,    78,    90,     0,    92,    85,    30,    84,    89,
       0,    61,    58,    59,    35,    31,    28,    30,    30,     0,
      30,    30,     0,    92,    92,    80,    81,    30,    94,     6,
       0,    73,    63,    43,     0,     0,    32,    32,     0,    34,
      30,     0,    95,    30,    77,    91,    92,     0,    30,     0,
       0,    36,    97,     0,    25,    26,    30,     0,    32,     0,
       0,    82,    30,     0,    52,    51,    37,    48,     0,    47,
      51,    46,    30,    40,    32,    32,    68,    32,    32,    30,
      50,     0,    45,     0,     0,    38,    27,    87,    70,    69,
      54,    49,    42,    44,    41
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    13,    54,    64,    65,    15,    16,    17,    96,    75,
      76,    94,   152,   131,   153,   172,   161,    18,   110,    19,
      48,    49,    50,    68,    51,    20,    21,    22,    23,    24,
      25,    26,    27,    67,    28,    29,    30,    31,    32,    33,
      77,    46,    82,    83,    80,    41,   139,    62,   146
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -65
static const yytype_int16 yypact[] =
{
     342,   -47,     5,    -4,     5,   -65,     3,     8,     5,    12,
     -65,   -65,    -6,    28,   -30,   -65,   -65,    30,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -25,   -65,
      -7,   342,   -65,   -65,   -19,   -65,   -65,     5,   -22,   -65,
     -21,   -65,   -65,   -65,   -65,   158,   -65,   342,   342,   -65,
      -6,   -65,   -23,     5,   -17,   -65,   -65,     7,   -46,   -65,
      -6,   -65,   -65,    37,   -15,    -3,   -65,   342,     1,    13,
     342,    15,    16,   -65,    -6,   -65,    17,   130,    16,   -65,
       5,   -65,   -65,   -65,     2,   -65,   -65,   209,   186,    14,
     232,    94,    21,   -65,   -65,    23,    16,   342,   -65,   -65,
      55,   -65,   -65,    52,    53,    61,    37,    37,    26,   -65,
     255,    46,   -65,   278,    16,   -65,   -65,    36,   342,    63,
      48,   -65,   -65,    54,   -65,   -65,   301,    71,    37,    57,
      49,    16,   324,    50,   -65,    56,   -65,   -65,    78,   -65,
      56,   -65,    20,   -65,    37,    37,   -65,    37,    37,   130,
     -65,   106,   -65,   106,    54,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -65,   -65,   -65,   -53,     0,   -65,   -65,   -65,   -64,   -65,
      18,   -65,   -65,   -65,   -50,   -48,   -33,   -65,   -41,   -65,
     -65,    51,   -49,    62,    -1,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,    38,   -60,   -55,   -65,    11,   -65,   -65,   -65
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -56
static const yytype_int16 yytable[] =
{
      14,    37,    74,    40,    78,    79,    88,    44,    35,    36,
      38,    81,    60,    34,    84,    90,    93,    42,    91,    35,
      36,   113,    43,   114,    99,   106,    45,   102,    52,    53,
      63,    61,    39,    85,     1,   105,   115,    66,     2,    70,
      89,    71,    87,   124,    97,   118,    69,    55,     3,   125,
       4,    95,   134,   135,   127,     5,    98,    47,     6,     7,
     100,    56,    69,     8,     9,    10,   141,    11,   147,    12,
      57,   103,   101,   120,   156,   143,    58,   104,    90,   126,
     123,   164,   129,   130,   132,   136,   138,   109,    59,   112,
     166,   167,   133,   168,   169,   142,   148,   109,   144,   145,
     109,   109,   155,   157,   149,   150,   151,   158,     1,   159,
     162,   171,     2,   160,   174,   173,   119,   163,   170,   128,
     109,    92,     3,   109,     4,    86,     0,   111,     0,     5,
       0,     0,     6,     7,     0,     0,   109,     8,     9,    10,
       0,    11,   109,    12,     1,     0,     0,     0,     2,   122,
       0,     0,   165,     0,     0,   107,   108,   -53,     3,   109,
       4,     0,     0,     0,     0,     5,     0,     0,     6,     7,
       0,     0,     1,     8,     9,    10,     2,    11,     0,    12,
      72,     0,     0,     0,     0,    73,     3,     0,     4,     0,
       0,     0,     0,     5,     0,     0,     6,     7,     0,     0,
       1,     8,     9,    10,     2,    11,     0,    12,    72,     0,
       0,     0,     0,   117,     3,     0,     4,     0,     0,     0,
       0,     5,     0,     1,     6,     7,     0,     2,     0,     8,
       9,    10,     0,    11,     0,    12,   116,     3,     0,     4,
       0,     0,     0,     0,     5,     0,     1,     6,     7,     0,
       2,     0,     8,     9,    10,     0,    11,     0,    12,   121,
       3,     0,     4,     0,     0,     0,     0,     5,     0,     1,
       6,     7,     0,     2,     0,     8,     9,    10,     0,    11,
       0,    12,   137,     3,     0,     4,     0,     0,     0,     0,
       5,     0,     1,     6,     7,     0,     2,     0,     8,     9,
      10,     0,    11,     0,    12,   140,     3,     0,     4,     0,
       0,     0,     0,     5,     0,     1,     6,     7,     0,     2,
       0,     8,     9,    10,     0,    11,     0,    12,   154,     3,
       0,     4,     0,     0,     0,     0,     5,     0,     1,     6,
       7,     0,     2,     0,     8,     9,    10,     0,    11,     0,
      12,   -55,     3,     0,     4,     0,     1,     0,     0,     5,
       2,     0,     6,     7,     0,     0,     0,     8,     9,    10,
       3,    11,     4,    12,     0,     0,     0,     5,     0,     0,
       6,     7,     0,     0,     0,     8,     9,    10,     0,    11,
       0,    12
};

static const yytype_int16 yycheck[] =
{
       0,     2,    55,     4,    57,    58,    66,     8,    14,    15,
      14,    60,    37,    60,    37,    61,    71,    14,    64,    14,
      15,    19,    14,    21,    77,    85,    14,    80,     0,    59,
      37,    56,    36,    56,    14,    84,    34,    56,    18,    61,
      33,    62,    59,   103,    59,    98,    47,    17,    28,   104,
      30,    14,   116,   117,   107,    35,    59,    63,    38,    39,
      59,    31,    63,    43,    44,    45,   126,    47,    14,    49,
      40,    56,    59,    59,   138,   128,    46,    61,    61,    56,
      59,    61,    27,    31,    31,    59,    40,    87,    58,    90,
     154,   155,    31,   157,   158,    59,    42,    97,    35,    51,
     100,   101,    31,    46,    50,    51,    52,    58,    14,    59,
      32,     5,    18,    57,   164,   163,    98,   150,   159,   108,
     120,    70,    28,   123,    30,    63,    -1,    89,    -1,    35,
      -1,    -1,    38,    39,    -1,    -1,   136,    43,    44,    45,
      -1,    47,   142,    49,    14,    -1,    -1,    -1,    18,    55,
      -1,    -1,   152,    -1,    -1,    25,    26,    27,    28,   159,
      30,    -1,    -1,    -1,    -1,    35,    -1,    -1,    38,    39,
      -1,    -1,    14,    43,    44,    45,    18,    47,    -1,    49,
      22,    -1,    -1,    -1,    -1,    27,    28,    -1,    30,    -1,
      -1,    -1,    -1,    35,    -1,    -1,    38,    39,    -1,    -1,
      14,    43,    44,    45,    18,    47,    -1,    49,    22,    -1,
      -1,    -1,    -1,    27,    28,    -1,    30,    -1,    -1,    -1,
      -1,    35,    -1,    14,    38,    39,    -1,    18,    -1,    43,
      44,    45,    -1,    47,    -1,    49,    27,    28,    -1,    30,
      -1,    -1,    -1,    -1,    35,    -1,    14,    38,    39,    -1,
      18,    -1,    43,    44,    45,    -1,    47,    -1,    49,    27,
      28,    -1,    30,    -1,    -1,    -1,    -1,    35,    -1,    14,
      38,    39,    -1,    18,    -1,    43,    44,    45,    -1,    47,
      -1,    49,    27,    28,    -1,    30,    -1,    -1,    -1,    -1,
      35,    -1,    14,    38,    39,    -1,    18,    -1,    43,    44,
      45,    -1,    47,    -1,    49,    27,    28,    -1,    30,    -1,
      -1,    -1,    -1,    35,    -1,    14,    38,    39,    -1,    18,
      -1,    43,    44,    45,    -1,    47,    -1,    49,    27,    28,
      -1,    30,    -1,    -1,    -1,    -1,    35,    -1,    14,    38,
      39,    -1,    18,    -1,    43,    44,    45,    -1,    47,    -1,
      49,    27,    28,    -1,    30,    -1,    14,    -1,    -1,    35,
      18,    -1,    38,    39,    -1,    -1,    -1,    43,    44,    45,
      28,    47,    30,    49,    -1,    -1,    -1,    35,    -1,    -1,
      38,    39,    -1,    -1,    -1,    43,    44,    45,    -1,    47,
      -1,    49
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    14,    18,    28,    30,    35,    38,    39,    43,    44,
      45,    47,    49,    66,    69,    70,    71,    72,    82,    84,
      90,    91,    92,    93,    94,    95,    96,    97,    99,   100,
     101,   102,   103,   104,    60,    14,    15,    89,    14,    36,
      89,   110,    14,    14,    89,    14,   106,    63,    85,    86,
      87,    89,     0,    59,    67,    17,    31,    40,    46,    58,
      37,    56,   112,    37,    68,    69,    56,    98,    88,    89,
      61,    62,    22,    27,    68,    74,    75,   105,    68,    68,
     109,    87,   107,   108,    37,    56,    88,    59,   107,    33,
      61,    64,    86,   108,    76,    14,    73,    59,    59,    68,
      59,    59,    68,    56,    61,    87,   107,    25,    26,    69,
      83,   106,    89,    19,    21,    34,    27,    27,    68,    75,
      59,    27,    55,    59,   107,   108,    56,    68,   110,    27,
      31,    78,    31,    31,    73,    73,    59,    27,    40,   111,
      27,   107,    59,    68,    35,    51,   113,    14,    42,    50,
      51,    52,    77,    79,    27,    31,    73,    46,    58,    59,
      57,    81,    32,    81,    61,    69,    73,    73,    73,    73,
      83,     5,    80,    80,    79
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 182 "gram.y"
    {
					plpsm_parser_result = (yyvsp[(1) - (2)].stmt);
					//elog_stmt(NOTICE, $1);
				;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 190 "gram.y"
    {
				;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 193 "gram.y"
    {
				;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 199 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 203 "gram.y"
    {
					(yyvsp[(1) - (3)].stmt)->last->next = (yyvsp[(3) - (3)].stmt);
					(yyvsp[(1) - (3)].stmt)->last = (yyvsp[(3) - (3)].stmt)->last;
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 214 "gram.y"
    {
					int	location = (yyvsp[(1) - (1)].stmt)->location;
					DEBUG_INIT;
					/* recheck "last" ptr */
					if (!(yyvsp[(1) - (1)].stmt)->last)
						(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
#ifdef ENABLE_DEBUG_ATTR
					while ((yyvsp[(1) - (1)].stmt))
					{
						DEBUG_SET((yyvsp[(1) - (1)].stmt), location);
						(yyvsp[(1) - (1)].stmt) = (yyvsp[(1) - (1)].stmt)->next;
					}
#endif
				;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 230 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 231 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 232 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 233 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 234 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 235 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 236 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 237 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 238 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 239 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 240 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 241 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 242 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 243 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 244 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 245 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 246 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 263 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yylsp[(2) - (6)]));
					new->name = (yyvsp[(1) - (6)].str);
					new->inner_left = (yyvsp[(3) - (6)].stmt);
					check_labels((yyvsp[(1) - (6)].str), (yyvsp[(6) - (6)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 271 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yylsp[(2) - (6)]));
					new->name = (yyvsp[(1) - (6)].str);
					new->inner_left = (yyvsp[(3) - (6)].stmt);
					check_labels((yyvsp[(1) - (6)].str), (yyvsp[(6) - (6)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 279 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					/* join declarations and statements */
					(yyvsp[(3) - (8)].stmt)->last->next = (yyvsp[(5) - (8)].stmt);
					(yyvsp[(3) - (8)].stmt)->last = (yyvsp[(5) - (8)].stmt)->last;
					new->inner_left = (yyvsp[(3) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 290 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yylsp[(2) - (4)]));
					new->name = (yyvsp[(1) - (4)].str);
					check_labels((yyvsp[(1) - (4)].str), (yyvsp[(4) - (4)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 300 "gram.y"
    {
					(yyval.str) = (yyvsp[(1) - (2)].word).ident;
				;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 304 "gram.y"
    {
					(yyval.str) = NULL;
				;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 311 "gram.y"
    {
					(yyval.str) = (yyvsp[(1) - (1)].word).ident;
				;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 315 "gram.y"
    {
					(yyval.str) = NULL;
				;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 322 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
					(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 327 "gram.y"
    {
					/*
					 * we have to check order of declarations,
					 * first variable or condition, next cursor's
					 * declarations and last handler's declarations.
					 */
					Plpsm_stmt *lstmt = (yyvsp[(1) - (3)].stmt)->last;
					
					if ((yyvsp[(3) - (3)].stmt)->typ == PLPSM_STMT_DECLARE_VARIABLE)
					{
						if (lstmt->typ == PLPSM_STMT_DECLARE_CURSOR || lstmt->typ == PLPSM_STMT_DECLARE_HANDLER)
							yyerror("syntax error, variable declaration after cursor or handler");
					}
					else if ((yyvsp[(3) - (3)].stmt)->typ == PLPSM_STMT_DECLARE_CONDITION)
					{
						if (lstmt->typ == PLPSM_STMT_DECLARE_CURSOR || lstmt->typ == PLPSM_STMT_DECLARE_HANDLER)
							yyerror("syntax error, condition declaration after cursor or handler");
					}
					else if ((yyvsp[(3) - (3)].stmt)->typ == PLPSM_STMT_DECLARE_CURSOR)
					{
						if (lstmt->typ == PLPSM_STMT_DECLARE_HANDLER)
							yyerror("syntax error, cursor declaration after handler");
					}
					else if ((yyvsp[(3) - (3)].stmt)->typ != PLPSM_STMT_DECLARE_HANDLER)
					{
						elog(ERROR, "internal error, unexpected value");
					}

					(yyvsp[(1) - (3)].stmt)->last->next = (yyvsp[(3) - (3)].stmt);
					(yyvsp[(1) - (3)].stmt)->last = (yyvsp[(3) - (3)].stmt);
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 363 "gram.y"
    {
					DEBUG_INIT;
					if ((yyvsp[(2) - (2)].stmt)->typ != PLPSM_STMT_DECLARE_VARIABLE)
						yyerror("syntax error");
					(yyval.stmt) = (yyvsp[(2) - (2)].stmt);
					DEBUG_SET((yyvsp[(2) - (2)].stmt), (yylsp[(1) - (2)]));
				;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 371 "gram.y"
    {
					DEBUG_INIT;
					if ((yyvsp[(2) - (4)].stmt)->typ != PLPSM_STMT_DECLARE_CONDITION)
						yyerror("syntax error");
					(yyvsp[(2) - (4)].stmt)->data = (yyvsp[(4) - (4)].node);
					(yyval.stmt) = (yyvsp[(2) - (4)].stmt);
					DEBUG_SET((yyvsp[(2) - (4)].stmt), (yylsp[(1) - (4)]));
				;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 380 "gram.y"
    {
					DEBUG_INIT;
					if ((yyvsp[(2) - (5)].stmt)->typ != PLPSM_STMT_DECLARE_CURSOR)
						yyerror("syntax error");
					(yyvsp[(2) - (5)].stmt)->query = (yyvsp[(5) - (5)].str);
					(yyval.stmt) = (yyvsp[(2) - (5)].stmt);
					DEBUG_SET((yyvsp[(2) - (5)].stmt), (yylsp[(1) - (5)]));
				;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 389 "gram.y"
    {
					DEBUG_INIT;
					if ((yyvsp[(2) - (6)].stmt)->typ != PLPSM_STMT_DECLARE_HANDLER)
						yyerror("syntax error");
					(yyvsp[(2) - (6)].stmt)->inner_left = (yyvsp[(6) - (6)].stmt);
					(yyvsp[(2) - (6)].stmt)->data = (yyvsp[(5) - (6)].list);
					(yyval.stmt) = (yyvsp[(2) - (6)].stmt);
					DEBUG_SET((yyvsp[(2) - (6)].stmt), (yylsp[(1) - (6)]));
				;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 401 "gram.y"
    {
					(yyval.stmt) = declare_prefetch();
				;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 408 "gram.y"
    {
					(yyval.list) = list_make1((yyvsp[(1) - (1)].node));
				;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 412 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].node));
				;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 419 "gram.y"
    {
					(yyval.node) = (yyvsp[(4) - (4)].node);
				;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 422 "gram.y"
    {
					(yyval.node) = NULL;
				;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 429 "gram.y"
    {
					(yyval.node) = (Node *) list_make2(
						makeString(pstrdup("IN")), (yyvsp[(3) - (3)].node));
				;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 434 "gram.y"
    {
					(yyval.node) = (Node *) list_make2(
						makeString(pstrdup("IN")),
						makeInteger(MAKE_SQLSTATE('0','2','0','0','0')));
				;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 440 "gram.y"
    {
					(yyval.node) = (Node *) list_make3(
						makeString(pstrdup("IN")),
						makeInteger(MAKE_SQLSTATE('0','1','0','0','0')),
						makeInteger(MAKE_SQLSTATE('0','2','0','0','0')));
				;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 447 "gram.y"
    {
					(yyval.node) = (Node *) list_make3(
						makeString(pstrdup("NOT_IN")),
						makeInteger(MAKE_SQLSTATE('0','1','0','0','0')),
						makeInteger(MAKE_SQLSTATE('0','2','0','0','0')));
				;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 454 "gram.y"
    {
					(yyval.node) = (Node *) makeString((yyvsp[(1) - (1)].word).ident);
				;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 460 "gram.y"
    {
					char   *sqlstatestr;

					sqlstatestr = (yyvsp[(1) - (1)].str);

					if (strlen(sqlstatestr) != 5)
						yyerror("invalid SQLSTATE code");
					if (strspn(sqlstatestr, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5)
						yyerror("invalid SQLSTATE code");

					(yyval.node) = (Node *) makeInteger(MAKE_SQLSTATE(sqlstatestr[0],
												  sqlstatestr[1],
												  sqlstatestr[2],
												  sqlstatestr[3],
												  sqlstatestr[4]));
				;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 480 "gram.y"
    {
				;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 482 "gram.y"
    {
				;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 492 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_IF, (yylsp[(1) - (7)]));
					new->expr = (yyvsp[(2) - (7)].str);
					new->inner_left = (yyvsp[(3) - (7)].stmt);
					new->inner_right = (yyvsp[(5) - (7)].stmt);
					(yyval.stmt) = new;
				;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 502 "gram.y"
    {
					(yyval.stmt) = NULL;
				;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 506 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_IF, (yylsp[(1) - (5)]));
					DEBUG_INIT;
					new->expr = (yyvsp[(2) - (5)].str);
					new->inner_left = (yyvsp[(3) - (5)].stmt);
					new->inner_right = (yyvsp[(5) - (5)].stmt);
					(yyval.stmt) = new;
					DEBUG_SET(new, (yylsp[(1) - (5)]));
				;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 516 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (3)].stmt);
				;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 532 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (2)].stmt);
				;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 539 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
					(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 544 "gram.y"
    {
					/* expected just very short list */
					(yyvsp[(1) - (3)].stmt)->last->next = (yyvsp[(3) - (3)].stmt);
					(yyvsp[(1) - (3)].stmt)->last = (yyvsp[(3) - (3)].stmt);
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 552 "gram.y"
    {
					(yyvsp[(1) - (3)].stmt)->typ = PLPSM_STMT_SET;
					(yyvsp[(1) - (3)].stmt)->expr = (yyvsp[(3) - (3)].str);
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 561 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_UNKNOWN, (yylsp[(1) - (1)]));
					new->target = (yyvsp[(1) - (1)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 567 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_UNKNOWN, (yylsp[(1) - (3)]));
					new->compound_target = (yyvsp[(2) - (3)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 576 "gram.y"
    {
					(yyval.list) = list_make1((yyvsp[(1) - (1)].list));
				;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 580 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].list));
				;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 587 "gram.y"
    {
					(yyval.list) = list_make1(makeString((yyvsp[(1) - (1)].word).ident));
				;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 591 "gram.y"
    {
					(yyval.list) = (yyvsp[(1) - (1)].cword).idents;
				;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 604 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_PRINT, (yylsp[(1) - (2)]));
					new->expr = (yyvsp[(2) - (2)].str);
					(yyval.stmt) = new;
				;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 619 "gram.y"
    {
					int tok = yylex();
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_RETURN, (yylsp[(1) - (1)]));
					if (tok == ';' || tok == 0)
					{
						plpsm_push_back_token(tok);
					}
					else
					{
						plpsm_push_back_token(tok);
						new->expr = read_expr_until_semi();
					}
					(yyval.stmt) = new;
				;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 642 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_LOOP, (yyvsp[(1) - (7)].str) ? (yylsp[(1) - (7)]) : (yylsp[(2) - (7)]));
					new->name = (yyvsp[(1) - (7)].str);
					new->inner_left = (yyvsp[(3) - (7)].stmt);
					check_labels((yyvsp[(1) - (7)].str), (yyvsp[(7) - (7)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 658 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_WHILE, (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					new->expr = (yyvsp[(3) - (8)].str);
					new->inner_left = (yyvsp[(4) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 675 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_REPEAT_UNTIL, (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					new->expr = (yyvsp[(6) - (8)].str);
					new->inner_left = (yyvsp[(3) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 692 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_ITERATE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 706 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_LEAVE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 721 "gram.y"
    {
					Plpsm_stmt *new = (yyvsp[(3) - (5)].stmt);
					new->location = (yylsp[(1) - (5)]);
					new->target = list_make1(makeString((yyvsp[(2) - (5)].word).ident));
					new->expr = (yyvsp[(5) - (5)].str);
					(yyval.stmt) = new;
				;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 731 "gram.y"
    {
					int tok = yylex();
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_PREPARE, -1);

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
					(yyval.stmt) = new;
				;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 771 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 777 "gram.y"
    {
					Plpsm_stmt *new = (yyvsp[(4) - (4)].stmt);
					new->typ = PLPSM_STMT_EXECUTE;
					new->location = (yylsp[(1) - (4)]);
					new->name = (yyvsp[(2) - (4)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 785 "gram.y"
    {
					Plpsm_stmt *new = (yyvsp[(4) - (6)].stmt);
					new->typ = PLPSM_STMT_EXECUTE;
					new->location = (yylsp[(1) - (6)]);
					new->name = (yyvsp[(2) - (6)].word).ident;
					new->data = (yyvsp[(6) - (6)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 794 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->data = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 810 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_EXECUTE_IMMEDIATE, (yylsp[(1) - (3)]));
					new->expr = (yyvsp[(3) - (3)].str);
					(yyval.stmt) = new;
				;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 816 "gram.y"
    {
					Plpsm_stmt *new = (yyvsp[(5) - (5)].stmt);
					new->typ = PLPSM_STMT_EXECUTE_IMMEDIATE;
					new->location = (yylsp[(1) - (5)]);
					new->expr = (yyvsp[(3) - (5)].str);
					(yyval.stmt) = new;
				;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 824 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_EXECUTE_IMMEDIATE, (yylsp[(1) - (5)]));
					new->expr = (yyvsp[(3) - (5)].str);
					new->data = (yyvsp[(5) - (5)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 831 "gram.y"
    {
					Plpsm_stmt *new = (yyvsp[(5) - (7)].stmt);
					new->typ = PLPSM_STMT_EXECUTE_IMMEDIATE;
					new->location = (yylsp[(1) - (7)]);
					new->expr = (yyvsp[(3) - (7)].str);
					new->data = (yyvsp[(7) - (7)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 850 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_OPEN, (yylsp[(1) - (2)]));
					new->target = (yyvsp[(2) - (2)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 856 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_OPEN, (yylsp[(1) - (4)]));
					new->target = (yyvsp[(2) - (4)].list);
					new->data = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 871 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_FETCH, (yylsp[(1) - (4)]));
					new->compound_target = (yyvsp[(4) - (4)].list);
					new->data = (yyvsp[(2) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 886 "gram.y"
    {
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_CLOSE, (yylsp[(1) - (2)]));
					new->target = (yyvsp[(2) - (2)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 901 "gram.y"
    {
					Plpsm_stmt *new = (yyvsp[(3) - (8)].stmt);
					new->location = (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]);
					new->name = (yyvsp[(1) - (8)].str);
					new->inner_left = (yyvsp[(4) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 916 "gram.y"
    {
					char *namespace = NULL;
					char *cursor_name = NULL;
					int tok;
					int	startloc = -1;
					Plpsm_stmt *new = new_stmt(PLPSM_STMT_FOR, -1);

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
					(yyval.stmt) = new;
				;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 982 "gram.y"
    {
					(yyval.str) = read_expr_until_semi();
				;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 989 "gram.y"
    {
					(yyval.list) = list_make1(makeString((yyvsp[(1) - (1)].str)));
				;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 993 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), makeString((yyvsp[(3) - (3)].str)));
				;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 999 "gram.y"
    {
					StringInfoData		ds;
					char *expr;
					int	tok;

					expr = read_until(';',',', 0, "; or ,", true, false, &tok, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					(yyval.str) = expr;
				;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 1014 "gram.y"
    {
					StringInfoData		ds;
					char *expr;

					expr = read_until(DO, -1, -1, "DO", true, false, NULL, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					(yyval.str) = expr;
				;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 1028 "gram.y"
    {
					StringInfoData		ds;
					char *expr;

					expr = read_until(THEN, -1, -1, "THEN", true, false, NULL, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					(yyval.str) = expr;
				;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 1042 "gram.y"
    {
					StringInfoData		ds;
					char *expr;

					expr = read_until(END, -1, -1, "END", true, false, NULL, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					(yyval.str) = expr;
				;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 1056 "gram.y"
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
					(yyval.str) = expr;
				;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 1073 "gram.y"
    {
					(yyval.str) = read_until(';', 0, 0, ";", false, false, NULL, -1);
				;}
    break;



/* Line 1455 of yacc.c  */
#line 2903 "gram.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1077 "gram.y"


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

	result = new_stmt(PLPSM_STMT_UNKNOWN, -1);

	for (;;)
	{
		if (state == EXPECTED_DATATYPE)
		{
			int endtok;
			char *datatype;
			Oid	type_id;
			int32	typmod;
			
			result->typ = PLPSM_STMT_DECLARE_VARIABLE;
			result->compound_target = varnames;

			datatype = read_until(';', DEFAULT, 0, "; or \"DEFAULT\"", false, true, &endtok, startlocation);
			parse_datatype(datatype, &type_id, &typmod);
			result->data = list_make3(makeString(datatype),
							    makeInteger(type_id),
							    makeInteger(typmod));

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

static Plpsm_stmt *
new_stmt(Plpsm_stmt_type typ, int location)
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
	appendStringInfo(ds, "%s| Data: %s\n", ident, nodeToString(stmt->data));
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

