/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.4.3"

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

extern ParserState pstate;



/* Line 189 of yacc.c  */
#line 149 "gram.c"

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
     ATOMIC = 271,
     AS = 272,
     BEGIN = 273,
     CASE = 274,
     CLOSE = 275,
     CONDITION = 276,
     CONDITION_IDENTIFIER = 277,
     CONTINUE = 278,
     CURSOR = 279,
     CURRENT = 280,
     DECLARE = 281,
     DEFAULT = 282,
     DETAIL_TEXT = 283,
     DIAGNOSTICS = 284,
     DO = 285,
     ELSE = 286,
     ELSEIF = 287,
     END = 288,
     EXECUTE = 289,
     EXIT = 290,
     FETCH = 291,
     FOR = 292,
     FOUND = 293,
     FROM = 294,
     GET = 295,
     HANDLER = 296,
     HINT_TEXT = 297,
     IF = 298,
     IMMEDIATE = 299,
     INTO = 300,
     ITERATE = 301,
     LEAVE = 302,
     LOOP = 303,
     MESSAGE_TEXT = 304,
     NO = 305,
     NOT = 306,
     OPEN = 307,
     PREPARE = 308,
     PRINT = 309,
     REPEAT = 310,
     RESIGNAL = 311,
     RETURNED_SQLCODE = 312,
     RETURNED_SQLSTATE = 313,
     RETURN = 314,
     ROW_COUNT = 315,
     SCROLL = 316,
     SELECT = 317,
     SET = 318,
     SIGNAL = 319,
     SQLEXCEPTION = 320,
     SQLSTATE = 321,
     SQLCODE = 322,
     SQLWARNING = 323,
     STACKED = 324,
     THEN = 325,
     UNDO = 326,
     UNTIL = 327,
     USING = 328,
     VALUE = 329,
     WHEN = 330,
     WHILE = 331,
     WITH = 332,
     INSERT = 333,
     UPDATE = 334,
     DELETE = 335,
     TRUNCATE = 336
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 73 "gram.y"

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
		Plpsm_signal_info		*sinfo;
		Plpsm_gd_info			*ginfo;
		Node		*node;



/* Line 214 of yacc.c  */
#line 289 "gram.c"
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
#line 314 "gram.c"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
#define YYFINAL  79
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   878

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  88
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  68
/* YYNRULES -- Number of rules.  */
#define YYNRULES  151
/* YYNRULES -- Number of states.  */
#define YYNSTATES  259

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   336

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      84,    85,     2,     2,    83,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    82,
       2,    86,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    87,     2,     2,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    49,    51,    53,    55,    57,    59,
      61,    63,    71,    79,    89,    94,    95,    97,    99,   100,
     102,   103,   105,   109,   112,   117,   122,   129,   130,   132,
     136,   141,   142,   146,   149,   151,   153,   155,   157,   159,
     160,   168,   169,   175,   179,   186,   189,   191,   196,   197,
     201,   204,   213,   215,   219,   223,   225,   229,   231,   233,
     237,   239,   241,   243,   246,   248,   256,   265,   274,   277,
     280,   285,   287,   291,   295,   297,   299,   300,   302,   304,
     306,   308,   310,   312,   314,   319,   322,   329,   334,   336,
     341,   344,   351,   356,   360,   362,   366,   370,   374,   378,
     382,   386,   390,   395,   398,   403,   410,   415,   419,   422,
     427,   432,   435,   444,   445,   447,   449,   451,   453,   457,
     462,   463,   464,   466,   470,   471,   472,   473,   474,   475,
     476,   477
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      89,     0,    -1,    92,    90,    -1,    82,    -1,    -1,    92,
      -1,    91,    82,    92,    -1,    93,    -1,    94,    -1,   112,
      -1,   119,    -1,   121,    -1,   122,    -1,   123,    -1,   124,
      -1,   125,    -1,   120,    -1,   135,    -1,   136,    -1,   137,
      -1,   138,    -1,   139,    -1,   140,    -1,   141,    -1,   106,
      -1,   108,    -1,   143,    -1,   144,    -1,   131,    -1,   132,
      -1,   126,    -1,    96,    18,    95,    91,    82,    33,    97,
      -1,    96,    18,    95,    98,    82,    33,    97,    -1,    96,
      18,    95,    98,    82,    91,    82,    33,    97,    -1,    96,
      18,    33,    97,    -1,    -1,    16,    -1,    14,    -1,    -1,
      14,    -1,    -1,    99,    -1,    98,    82,    99,    -1,    26,
     100,    -1,    26,   100,    21,   102,    -1,    26,   100,    24,
      37,    -1,    26,   100,    41,    37,   101,    92,    -1,    -1,
     103,    -1,   101,    83,   103,    -1,    37,    66,   105,   104,
      -1,    -1,    66,   105,   104,    -1,    51,    38,    -1,    68,
      -1,    65,    -1,    14,    -1,     5,    -1,    74,    -1,    -1,
      43,   151,    91,    82,   107,    33,    43,    -1,    -1,    32,
     151,    91,    82,   107,    -1,    31,    91,    82,    -1,    19,
     155,   109,   111,    33,    19,    -1,   109,   110,    -1,   110,
      -1,    75,   151,    91,    82,    -1,    -1,    31,    91,    82,
      -1,    63,   113,    -1,    63,    84,   117,    85,    86,    84,
     147,    85,    -1,   114,    -1,   113,    83,   114,    -1,   115,
      86,   149,    -1,   116,    -1,   115,    87,   148,    -1,   118,
      -1,   118,    -1,   117,    83,   118,    -1,    14,    -1,    15,
      -1,    10,    -1,    54,   147,    -1,    59,    -1,    96,    48,
      91,    82,    33,    48,    97,    -1,    96,    76,   150,    91,
      82,    33,    76,    97,    -1,    96,    55,    91,    82,    72,
     152,    55,    97,    -1,    46,    14,    -1,    47,    14,    -1,
      40,   129,    29,   127,    -1,   128,    -1,   127,    83,   128,
      -1,   118,    86,   130,    -1,    25,    -1,    69,    -1,    -1,
      58,    -1,    57,    -1,    49,    -1,    28,    -1,    42,    -1,
      60,    -1,    22,    -1,    64,    66,   105,   104,    -1,    64,
      14,    -1,    64,    66,   105,   104,    63,   133,    -1,    64,
      14,    63,   133,    -1,    56,    -1,    56,    66,   105,   104,
      -1,    56,    14,    -1,    56,    66,   105,   104,    63,   133,
      -1,    56,    14,    63,   133,    -1,    56,    63,   133,    -1,
     134,    -1,   133,    83,   134,    -1,    28,    86,     5,    -1,
      42,    86,     5,    -1,    49,    86,     5,    -1,    28,    86,
     118,    -1,    42,    86,   118,    -1,    49,    86,   118,    -1,
      53,    14,    39,   146,    -1,    34,    14,    -1,    34,    14,
      45,   117,    -1,    34,    14,    45,   117,    73,   117,    -1,
      34,    14,    73,   117,    -1,    34,    44,   153,    -1,    52,
     118,    -1,    52,   118,    73,   117,    -1,    36,   118,    45,
     117,    -1,    20,   118,    -1,    96,    37,   142,    91,    82,
      33,    37,    97,    -1,    -1,    78,    -1,    79,    -1,    80,
      -1,    81,    -1,    62,   145,   117,    -1,    62,   145,   117,
      39,    -1,    -1,    -1,   154,    -1,   147,    83,   154,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   209,   209,   218,   222,   227,   231,   242,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   287,   296,   305,   317,   327,   330,   341,   351,   357,
     362,   368,   373,   409,   417,   426,   444,   457,   463,   467,
     475,   482,   488,   496,   504,   511,   518,   529,   551,   554,
     564,   575,   578,   586,   599,   610,   615,   622,   633,   636,
     652,   657,   667,   672,   680,   688,   693,   712,   721,   725,
     732,   736,   746,   763,   778,   828,   845,   863,   881,   897,
     912,   922,   927,   945,   954,   959,   966,   973,   974,   975,
     976,   977,   978,   979,   989,   995,  1001,  1008,  1023,  1029,
    1036,  1043,  1051,  1059,  1069,  1074,  1096,  1103,  1110,  1117,
    1124,  1131,  1146,  1163,  1169,  1176,  1184,  1200,  1216,  1222,
    1238,  1255,  1270,  1287,  1355,  1356,  1357,  1358,  1362,  1375,
    1394,  1414,  1420,  1424,  1431,  1441,  1451,  1461,  1471,  1481,
    1494,  1508
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT", "FCONST", "SCONST", "BCONST",
  "XCONST", "Op", "ICONST", "PARAM", "TYPECAST", "DOT_DOT", "COLON_EQUALS",
  "WORD", "CWORD", "ATOMIC", "AS", "BEGIN", "CASE", "CLOSE", "CONDITION",
  "CONDITION_IDENTIFIER", "CONTINUE", "CURSOR", "CURRENT", "DECLARE",
  "DEFAULT", "DETAIL_TEXT", "DIAGNOSTICS", "DO", "ELSE", "ELSEIF", "END",
  "EXECUTE", "EXIT", "FETCH", "FOR", "FOUND", "FROM", "GET", "HANDLER",
  "HINT_TEXT", "IF", "IMMEDIATE", "INTO", "ITERATE", "LEAVE", "LOOP",
  "MESSAGE_TEXT", "NO", "NOT", "OPEN", "PREPARE", "PRINT", "REPEAT",
  "RESIGNAL", "RETURNED_SQLCODE", "RETURNED_SQLSTATE", "RETURN",
  "ROW_COUNT", "SCROLL", "SELECT", "SET", "SIGNAL", "SQLEXCEPTION",
  "SQLSTATE", "SQLCODE", "SQLWARNING", "STACKED", "THEN", "UNDO", "UNTIL",
  "USING", "VALUE", "WHEN", "WHILE", "WITH", "INSERT", "UPDATE", "DELETE",
  "TRUNCATE", "';'", "','", "'('", "')'", "'='", "'['", "$accept",
  "function", "opt_semi", "statements", "dstmt", "stmt", "stmt_compound",
  "opt_atomic", "opt_label", "opt_end_label", "declarations",
  "declaration", "declare_prefetch", "condition_list", "opt_sqlstate",
  "condition", "sqlstate", "opt_value", "stmt_if", "stmt_else",
  "stmt_case", "case_when_list", "case_when", "opt_case_else", "stmt_set",
  "assign_list", "assign_item", "assign_var", "target",
  "qual_identif_list", "qual_identif", "stmt_print", "stmt_return",
  "stmt_loop", "stmt_while", "stmt_repeat_until", "stmt_iterate",
  "stmt_leave", "stmt_get_diag", "gd_info_list", "gd_info", "gd_area_opt",
  "gd_info_enum", "stmt_signal", "stmt_resignal", "signal_info",
  "signal_info_item", "stmt_prepare", "stmt_execute",
  "stmt_execute_immediate", "stmt_open", "stmt_fetch", "stmt_close",
  "stmt_for", "for_prefetch", "stmt_sql", "stmt_select_into",
  "expr_list_into", "expr_until_semi", "expr_list",
  "expr_until_right_bracket", "expr_until_semi_or_coma", "expr_until_do",
  "expr_until_then", "expr_until_end", "expr_until_semi_into_using",
  "expr_until_semi_or_coma_or_parent", "opt_expr_until_when", 0
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
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,    59,    44,    40,    41,    61,    91
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    88,    89,    90,    90,    91,    91,    92,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    93,    93,    93,    93,    93,    93,    93,    93,    93,
      93,    94,    94,    94,    94,    95,    95,    96,    96,    97,
      97,    98,    98,    99,    99,    99,    99,   100,   101,   101,
     102,   102,   103,   103,   103,   103,   103,   104,   105,   105,
     106,   107,   107,   107,   108,   109,   109,   110,   111,   111,
     112,   112,   113,   113,   114,   115,   115,   116,   117,   117,
     118,   118,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   127,   128,   129,   129,   129,   130,   130,   130,
     130,   130,   130,   130,   131,   131,   131,   131,   132,   132,
     132,   132,   132,   132,   133,   133,   134,   134,   134,   134,
     134,   134,   135,   136,   136,   136,   136,   137,   138,   138,
     139,   140,   141,   142,   143,   143,   143,   143,   144,   144,
     145,   146,   147,   147,   148,   149,   150,   151,   152,   153,
     154,   155
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     0,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     7,     7,     9,     4,     0,     1,     1,     0,     1,
       0,     1,     3,     2,     4,     4,     6,     0,     1,     3,
       4,     0,     3,     2,     1,     1,     1,     1,     1,     0,
       7,     0,     5,     3,     6,     2,     1,     4,     0,     3,
       2,     8,     1,     3,     3,     1,     3,     1,     1,     3,
       1,     1,     1,     2,     1,     7,     8,     8,     2,     2,
       4,     1,     3,     3,     1,     1,     0,     1,     1,     1,
       1,     1,     1,     1,     4,     2,     6,     4,     1,     4,
       2,     6,     4,     3,     1,     3,     3,     3,     3,     3,
       3,     3,     4,     2,     4,     6,     4,     3,     2,     4,
       4,     2,     8,     0,     1,     1,     1,     1,     3,     4,
       0,     0,     1,     3,     0,     0,     0,     0,     0,     0,
       0,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      38,    37,   151,     0,     0,     0,    96,   147,     0,     0,
       0,     0,   150,   108,    84,   140,     0,     0,   134,   135,
     136,   137,     0,     4,     7,     8,     0,    24,    25,     9,
      10,    16,    11,    12,    13,    14,    15,    30,    28,    29,
      17,    18,    19,    20,    21,    22,    23,    26,    27,     0,
      82,    80,    81,   131,   123,   149,     0,    94,    95,     0,
      38,    88,    89,   128,     0,    83,   142,   110,     0,    59,
       0,     0,    70,    72,     0,    75,    77,   105,    59,     1,
       3,     2,    35,   133,    38,    38,   146,   147,    68,    66,
       0,     0,   127,     0,     0,     0,     5,     0,   141,   150,
       0,     0,     0,     0,   113,   114,    58,     0,   138,    78,
       0,     0,   145,   144,     0,     0,    36,    40,    38,    38,
       0,     0,    38,    38,    38,    65,     0,   124,   126,   130,
       0,    90,    91,    38,   129,   122,   143,   112,     0,     0,
       0,     0,    57,   109,   139,     0,     0,    73,    74,    76,
     107,   104,    39,    34,    47,     0,     0,    41,     0,    38,
      38,     0,     0,     0,     0,     0,     0,     0,    38,   147,
       6,     0,   116,   119,   117,   120,   118,   121,   115,     0,
      79,     0,     0,    43,    38,    38,    38,     0,   148,    38,
      38,    38,    64,   125,   103,   100,   101,    99,    98,    97,
     102,    93,    92,     0,    38,     0,   111,   150,   106,    51,
       0,     0,    40,    40,     0,    42,     0,    40,     0,     0,
      38,     0,    60,     0,     0,    44,    45,     0,    31,    32,
      38,    40,    85,    40,    40,    38,    71,    59,    56,     0,
      55,    59,    54,    38,    48,    40,   132,    87,    86,    62,
       0,    53,     0,     0,    46,    33,    50,    52,    49
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    22,    81,    95,    96,    24,    25,   118,    26,   153,
     156,   157,   183,   243,   225,   244,   143,   107,    27,   171,
      28,    88,    89,   126,    29,    72,    73,    74,    75,   108,
     109,    30,    31,    32,    33,    34,    35,    36,    37,   131,
     132,    59,   201,    38,    39,   104,   105,    40,    41,    42,
      43,    44,    45,    46,   119,    47,    48,    70,   135,    65,
     149,   148,   122,    60,   218,    92,    66,    49
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -111
static const yytype_int16 yypact[] =
{
     797,  -111,  -111,    70,    12,    70,   -15,  -111,    15,    21,
      70,    34,  -111,     4,  -111,  -111,     2,    11,  -111,  -111,
    -111,  -111,    20,   -22,  -111,  -111,    13,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,    -6,
    -111,  -111,  -111,  -111,   -30,  -111,    42,  -111,  -111,    75,
     797,  -111,  -111,    32,    40,    23,  -111,    47,     9,    37,
      70,    70,    38,  -111,     8,  -111,  -111,    54,    37,  -111,
    -111,  -111,    16,  -111,   797,   797,  -111,  -111,   -18,  -111,
      70,    70,  -111,    70,    70,    41,  -111,    70,  -111,  -111,
       9,    39,    44,    49,    56,  -111,  -111,   139,   -20,  -111,
     -49,    70,  -111,  -111,     9,   139,  -111,   131,   338,   797,
      64,    65,   797,   797,   797,  -111,   115,   -31,    68,    68,
      67,    71,  -111,   185,    68,  -111,  -111,    56,    98,   114,
     117,     9,  -111,    93,  -111,    70,    69,  -111,  -111,  -111,
      56,    94,  -111,  -111,  -111,    76,    82,  -111,    86,   389,
     440,    87,    88,    89,   153,    70,    92,    70,   797,  -111,
    -111,   140,  -111,  -111,  -111,  -111,  -111,  -111,  -111,     9,
    -111,    90,     9,     6,   491,   287,   542,   127,  -111,   593,
     236,   644,  -111,    68,  -111,  -111,  -111,  -111,  -111,  -111,
    -111,  -111,  -111,    95,   797,   133,    56,  -111,    56,   141,
     142,   144,   131,   131,   103,  -111,   150,   131,   137,   112,
     695,   111,  -111,    -9,   129,  -111,  -111,    50,  -111,  -111,
     746,   131,  -111,   131,   131,   185,  -111,    37,  -111,   158,
    -111,    37,  -111,    19,  -111,   131,  -111,  -111,  -111,  -111,
     139,  -111,   139,    50,  -111,  -111,  -111,  -111,  -111
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -111,  -111,  -111,   -78,     0,  -111,  -111,  -111,  -111,   -51,
    -111,    17,  -111,  -111,  -111,   -56,  -110,   -74,  -111,   -37,
    -111,  -111,   113,  -111,  -111,  -111,    96,  -111,  -111,   -69,
      -2,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,
      33,  -111,  -111,  -111,  -111,   -91,    62,  -111,  -111,  -111,
    -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,  -111,    -1,
    -111,  -111,  -111,   -76,  -111,  -111,   109,  -111
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -70
static const yytype_int16 yytable[] =
{
      23,    53,   110,    56,   115,   151,   120,   121,    63,   137,
      57,   123,    50,   124,    76,    90,    51,    52,    67,   144,
      79,   127,   128,   150,   129,    77,    54,   209,   134,    61,
     210,    82,   116,     1,   145,    62,   146,   101,     2,     3,
     155,   158,   165,    91,   161,   162,   163,   211,    64,   117,
      83,   102,   145,     4,    58,     5,    55,    87,   103,     6,
      80,    84,     7,   145,   238,     8,     9,    68,    85,    87,
      69,    10,    11,    12,    99,    13,   236,    78,    14,    98,
      50,    15,    16,    17,    51,    52,    71,    93,   206,    86,
     203,   208,   130,   204,   112,   113,   193,    18,    19,    20,
      21,   239,   253,   172,    94,    97,    99,   214,    50,    76,
     100,   106,    51,    52,   194,   240,   241,   114,   242,   174,
     195,   111,   176,   133,    50,   138,   221,    50,    51,    52,
     139,    51,    52,   170,   196,   140,   173,   175,   177,   141,
     256,   197,   257,   180,   142,   152,   159,   160,   164,   198,
     199,   145,   200,   166,   167,   181,   179,   182,   184,   170,
     170,   228,   229,   250,   185,   130,   232,   252,   186,   189,
     190,   191,   192,   205,   207,   217,   222,   220,   224,   226,
     246,   227,   247,   248,   170,   230,   170,   231,   234,   170,
     170,   170,   233,   235,   255,   237,   251,   258,   249,     1,
     202,   125,   215,   178,     2,     3,   223,   147,   136,     0,
       0,     0,     0,     0,     0,     0,   168,   169,   -61,     4,
     170,     5,     0,     0,     0,     6,     0,     0,     7,     0,
     170,     8,     9,     0,     0,   170,     0,    10,    11,    12,
       0,    13,     0,   254,    14,     0,     0,    15,    16,    17,
       1,     0,     0,     0,     0,     2,     3,     0,     0,     0,
       0,     0,     0,    18,    19,    20,    21,   -67,     0,   -67,
       4,     0,     5,     0,     0,     0,     6,     0,     0,     7,
       0,     0,     8,     9,     0,     0,     0,     0,    10,    11,
      12,     0,    13,     0,     0,    14,     0,     0,    15,    16,
      17,     1,     0,     0,     0,     0,     2,     3,     0,     0,
       0,   -67,     0,   154,    18,    19,    20,    21,     0,     0,
     213,     4,     0,     5,     0,     0,     0,     6,     0,     0,
       7,     0,     0,     8,     9,     0,     0,     0,     0,    10,
      11,    12,     0,    13,     0,     0,    14,     0,     0,    15,
      16,    17,     1,     0,     0,     0,     0,     2,     3,     0,
       0,     0,     0,     0,   154,    18,    19,    20,    21,     0,
       0,     0,     4,     0,     5,     0,     0,     0,     6,     0,
       0,     7,     0,     0,     8,     9,     0,     0,     0,     0,
      10,    11,    12,     0,    13,     0,     0,    14,     0,     0,
      15,    16,    17,     1,     0,     0,     0,     0,     2,     3,
       0,     0,     0,     0,     0,     0,    18,    19,    20,    21,
       0,     0,   187,     4,     0,     5,     0,     0,     0,     6,
       0,     0,     7,     0,     0,     8,     9,     0,     0,     0,
       0,    10,    11,    12,     0,    13,     0,     0,    14,     0,
       0,    15,    16,    17,     1,     0,     0,     0,     0,     2,
       3,     0,     0,     0,     0,     0,     0,    18,    19,    20,
      21,     0,     0,     0,     4,     0,     5,     0,     0,     0,
       6,     0,     0,     7,     0,     0,     8,     9,     0,     0,
       0,     0,    10,    11,    12,     0,    13,     0,     0,    14,
       0,     0,    15,    16,    17,     1,     0,     0,     0,     0,
       2,     3,   188,     0,     0,     0,     0,     0,    18,    19,
      20,    21,     0,     0,   212,     4,     0,     5,     0,     0,
       0,     6,     0,     0,     7,     0,     0,     8,     9,     0,
       0,     0,     0,    10,    11,    12,     0,    13,     0,     0,
      14,     0,     0,    15,    16,    17,     1,     0,     0,     0,
       0,     2,     3,     0,     0,     0,     0,     0,     0,    18,
      19,    20,    21,     0,     0,   216,     4,     0,     5,     0,
       0,     0,     6,     0,     0,     7,     0,     0,     8,     9,
       0,     0,     0,     0,    10,    11,    12,     0,    13,     0,
       0,    14,     0,     0,    15,    16,    17,     1,     0,     0,
       0,     0,     2,     3,     0,     0,     0,     0,     0,     0,
      18,    19,    20,    21,     0,     0,   219,     4,     0,     5,
       0,     0,     0,     6,     0,     0,     7,     0,     0,     8,
       9,     0,     0,     0,     0,    10,    11,    12,     0,    13,
       0,     0,    14,     0,     0,    15,    16,    17,     1,     0,
       0,     0,     0,     2,     3,     0,     0,     0,     0,     0,
       0,    18,    19,    20,    21,     0,     0,   -69,     4,     0,
       5,     0,     0,     0,     6,     0,     0,     7,     0,     0,
       8,     9,     0,     0,     0,     0,    10,    11,    12,     0,
      13,     0,     0,    14,     0,     0,    15,    16,    17,     1,
       0,     0,     0,     0,     2,     3,     0,     0,     0,     0,
       0,     0,    18,    19,    20,    21,     0,     0,   -63,     4,
       0,     5,     0,     0,     0,     6,     0,     0,     7,     0,
       0,     8,     9,     0,     0,     0,     0,    10,    11,    12,
       0,    13,     0,     0,    14,     0,     0,    15,    16,    17,
       1,     0,     0,     0,     0,     2,     3,     0,     0,     0,
       0,     0,     0,    18,    19,    20,    21,     0,     0,   245,
       4,     0,     5,     0,     0,     0,     6,     0,     0,     7,
       0,     0,     8,     9,     0,     0,     0,     0,    10,    11,
      12,     0,    13,     0,     0,    14,     0,     0,    15,    16,
      17,     1,     0,     0,     0,     0,     2,     3,     0,     0,
       0,     0,     0,     0,    18,    19,    20,    21,     0,     0,
       0,     4,     0,     5,     0,     0,     0,     6,     0,     0,
       7,     0,     0,     8,     9,     0,     0,     0,     0,    10,
      11,    12,     0,    13,     0,     0,    14,     0,     0,    15,
      16,    17,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    18,    19,    20,    21
};

static const yytype_int16 yycheck[] =
{
       0,     3,    71,     5,    78,   115,    84,    85,    10,   100,
      25,    87,    10,    31,    16,    45,    14,    15,    14,    39,
       0,    90,    91,   114,    93,    14,    14,    21,    97,    14,
      24,    18,    16,    14,    83,    14,    85,    28,    19,    20,
     118,   119,    73,    73,   122,   123,   124,    41,    14,    33,
      37,    42,    83,    34,    69,    36,    44,    75,    49,    40,
      82,    48,    43,    83,    14,    46,    47,    63,    55,    75,
      66,    52,    53,    54,    83,    56,    85,    66,    59,    39,
      10,    62,    63,    64,    14,    15,    84,    45,   179,    76,
     168,   182,    94,   169,    86,    87,   165,    78,    79,    80,
      81,    51,    83,     5,    29,    73,    83,   185,    10,   111,
      63,    74,    14,    15,    22,    65,    66,    63,    68,     5,
      28,    83,     5,    82,    10,    86,   204,    10,    14,    15,
      86,    14,    15,   133,    42,    86,   138,   139,   140,    83,
     250,    49,   252,   145,     5,    14,    82,    82,    33,    57,
      58,    83,    60,    86,    83,    86,    63,    63,    82,   159,
     160,   212,   213,   237,    82,   167,   217,   241,    82,    82,
      82,    82,    19,    33,    84,    48,    43,    82,    37,    37,
     231,    37,   233,   234,   184,    82,   186,    37,    76,   189,
     190,   191,    55,    82,   245,    66,    38,   253,   235,    14,
     167,    88,   185,   141,    19,    20,   207,   111,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    31,    32,    33,    34,
     220,    36,    -1,    -1,    -1,    40,    -1,    -1,    43,    -1,
     230,    46,    47,    -1,    -1,   235,    -1,    52,    53,    54,
      -1,    56,    -1,   243,    59,    -1,    -1,    62,    63,    64,
      14,    -1,    -1,    -1,    -1,    19,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    80,    81,    31,    -1,    33,
      34,    -1,    36,    -1,    -1,    -1,    40,    -1,    -1,    43,
      -1,    -1,    46,    47,    -1,    -1,    -1,    -1,    52,    53,
      54,    -1,    56,    -1,    -1,    59,    -1,    -1,    62,    63,
      64,    14,    -1,    -1,    -1,    -1,    19,    20,    -1,    -1,
      -1,    75,    -1,    26,    78,    79,    80,    81,    -1,    -1,
      33,    34,    -1,    36,    -1,    -1,    -1,    40,    -1,    -1,
      43,    -1,    -1,    46,    47,    -1,    -1,    -1,    -1,    52,
      53,    54,    -1,    56,    -1,    -1,    59,    -1,    -1,    62,
      63,    64,    14,    -1,    -1,    -1,    -1,    19,    20,    -1,
      -1,    -1,    -1,    -1,    26,    78,    79,    80,    81,    -1,
      -1,    -1,    34,    -1,    36,    -1,    -1,    -1,    40,    -1,
      -1,    43,    -1,    -1,    46,    47,    -1,    -1,    -1,    -1,
      52,    53,    54,    -1,    56,    -1,    -1,    59,    -1,    -1,
      62,    63,    64,    14,    -1,    -1,    -1,    -1,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    80,    81,
      -1,    -1,    33,    34,    -1,    36,    -1,    -1,    -1,    40,
      -1,    -1,    43,    -1,    -1,    46,    47,    -1,    -1,    -1,
      -1,    52,    53,    54,    -1,    56,    -1,    -1,    59,    -1,
      -1,    62,    63,    64,    14,    -1,    -1,    -1,    -1,    19,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,    80,
      81,    -1,    -1,    -1,    34,    -1,    36,    -1,    -1,    -1,
      40,    -1,    -1,    43,    -1,    -1,    46,    47,    -1,    -1,
      -1,    -1,    52,    53,    54,    -1,    56,    -1,    -1,    59,
      -1,    -1,    62,    63,    64,    14,    -1,    -1,    -1,    -1,
      19,    20,    72,    -1,    -1,    -1,    -1,    -1,    78,    79,
      80,    81,    -1,    -1,    33,    34,    -1,    36,    -1,    -1,
      -1,    40,    -1,    -1,    43,    -1,    -1,    46,    47,    -1,
      -1,    -1,    -1,    52,    53,    54,    -1,    56,    -1,    -1,
      59,    -1,    -1,    62,    63,    64,    14,    -1,    -1,    -1,
      -1,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,    78,
      79,    80,    81,    -1,    -1,    33,    34,    -1,    36,    -1,
      -1,    -1,    40,    -1,    -1,    43,    -1,    -1,    46,    47,
      -1,    -1,    -1,    -1,    52,    53,    54,    -1,    56,    -1,
      -1,    59,    -1,    -1,    62,    63,    64,    14,    -1,    -1,
      -1,    -1,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      78,    79,    80,    81,    -1,    -1,    33,    34,    -1,    36,
      -1,    -1,    -1,    40,    -1,    -1,    43,    -1,    -1,    46,
      47,    -1,    -1,    -1,    -1,    52,    53,    54,    -1,    56,
      -1,    -1,    59,    -1,    -1,    62,    63,    64,    14,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    78,    79,    80,    81,    -1,    -1,    33,    34,    -1,
      36,    -1,    -1,    -1,    40,    -1,    -1,    43,    -1,    -1,
      46,    47,    -1,    -1,    -1,    -1,    52,    53,    54,    -1,
      56,    -1,    -1,    59,    -1,    -1,    62,    63,    64,    14,
      -1,    -1,    -1,    -1,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    78,    79,    80,    81,    -1,    -1,    33,    34,
      -1,    36,    -1,    -1,    -1,    40,    -1,    -1,    43,    -1,
      -1,    46,    47,    -1,    -1,    -1,    -1,    52,    53,    54,
      -1,    56,    -1,    -1,    59,    -1,    -1,    62,    63,    64,
      14,    -1,    -1,    -1,    -1,    19,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    79,    80,    81,    -1,    -1,    33,
      34,    -1,    36,    -1,    -1,    -1,    40,    -1,    -1,    43,
      -1,    -1,    46,    47,    -1,    -1,    -1,    -1,    52,    53,
      54,    -1,    56,    -1,    -1,    59,    -1,    -1,    62,    63,
      64,    14,    -1,    -1,    -1,    -1,    19,    20,    -1,    -1,
      -1,    -1,    -1,    -1,    78,    79,    80,    81,    -1,    -1,
      -1,    34,    -1,    36,    -1,    -1,    -1,    40,    -1,    -1,
      43,    -1,    -1,    46,    47,    -1,    -1,    -1,    -1,    52,
      53,    54,    -1,    56,    -1,    -1,    59,    -1,    -1,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    79,    80,    81
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    14,    19,    20,    34,    36,    40,    43,    46,    47,
      52,    53,    54,    56,    59,    62,    63,    64,    78,    79,
      80,    81,    89,    92,    93,    94,    96,   106,   108,   112,
     119,   120,   121,   122,   123,   124,   125,   126,   131,   132,
     135,   136,   137,   138,   139,   140,   141,   143,   144,   155,
      10,    14,    15,   118,    14,    44,   118,    25,    69,   129,
     151,    14,    14,   118,    14,   147,   154,    14,    63,    66,
     145,    84,   113,   114,   115,   116,   118,    14,    66,     0,
      82,    90,    18,    37,    48,    55,    76,    75,   109,   110,
      45,    73,   153,    45,    29,    91,    92,    73,    39,    83,
      63,    28,    42,    49,   133,   134,    74,   105,   117,   118,
     117,    83,    86,    87,    63,   105,    16,    33,    95,   142,
      91,    91,   150,   151,    31,   110,   111,   117,   117,   117,
     118,   127,   128,    82,   117,   146,   154,   133,    86,    86,
      86,    83,     5,   104,    39,    83,    85,   114,   149,   148,
     133,   104,    14,    97,    26,    91,    98,    99,    91,    82,
      82,    91,    91,    91,    33,    73,    86,    83,    31,    32,
      92,   107,     5,   118,     5,   118,     5,   118,   134,    63,
     118,    86,    63,   100,    82,    82,    82,    33,    72,    82,
      82,    82,    19,   117,    22,    28,    42,    49,    57,    58,
      60,   130,   128,    91,   151,    33,   133,    84,   133,    21,
      24,    41,    33,    33,    91,    99,    33,    48,   152,    33,
      82,    91,    43,   147,    37,   102,    37,    37,    97,    97,
      82,    37,    97,    55,    76,    82,    85,    66,    14,    51,
      65,    66,    68,   101,   103,    33,    97,    97,    97,   107,
     105,    38,   105,    83,    92,    97,   104,   104,   103
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
    YYLTYPE yyerror_range[3];

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

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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

/* Line 1464 of yacc.c  */
#line 210 "gram.y"
    {
					plpsm_parser_tree = (yyvsp[(1) - (2)].stmt);
					if (plpsm_debug_parser)
						elog_stmt(NOTICE, (yyvsp[(1) - (2)].stmt));
				;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 219 "gram.y"
    {
				;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 222 "gram.y"
    {
				;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 228 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 232 "gram.y"
    {
					(yyvsp[(1) - (3)].stmt)->last->next = (yyvsp[(3) - (3)].stmt);
					(yyvsp[(1) - (3)].stmt)->last = (yyvsp[(3) - (3)].stmt)->last;
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 243 "gram.y"
    {
					if (!(yyvsp[(1) - (1)].stmt)->last)
						(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 249 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 250 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 251 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 252 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 253 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 254 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 255 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 256 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 257 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 258 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 259 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 260 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 261 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 262 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 263 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 264 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 265 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 266 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 267 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 268 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 269 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 270 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 271 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 288 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yyvsp[(1) - (7)].str) ? (yylsp[(1) - (7)]): (yylsp[(2) - (7)]));
					new->name = (yyvsp[(1) - (7)].str);
					new->option = (yyvsp[(3) - (7)].boolean);
					new->inner_left = (yyvsp[(4) - (7)].stmt);
					check_labels((yyvsp[(1) - (7)].str), (yyvsp[(7) - (7)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 297 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yyvsp[(1) - (7)].str) ? (yylsp[(1) - (7)]): (yylsp[(2) - (7)]));
					new->name = (yyvsp[(1) - (7)].str);
					new->option = (yyvsp[(3) - (7)].boolean);
					new->inner_left = (yyvsp[(4) - (7)].stmt);
					check_labels((yyvsp[(1) - (7)].str), (yyvsp[(7) - (7)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 306 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yyvsp[(1) - (9)].str) ? (yylsp[(1) - (9)]): (yylsp[(2) - (9)]));
					new->name = (yyvsp[(1) - (9)].str);
					new->option = (yyvsp[(3) - (9)].boolean);
					/* join declarations and statements */
					(yyvsp[(4) - (9)].stmt)->last->next = (yyvsp[(6) - (9)].stmt);
					(yyvsp[(4) - (9)].stmt)->last = (yyvsp[(6) - (9)].stmt)->last;
					new->inner_left = (yyvsp[(4) - (9)].stmt);
					check_labels((yyvsp[(1) - (9)].str), (yyvsp[(9) - (9)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 318 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yyvsp[(1) - (4)].str) ? (yylsp[(1) - (4)]): (yylsp[(2) - (4)]));
					new->name = (yyvsp[(1) - (4)].str);
					check_labels((yyvsp[(1) - (4)].str), (yyvsp[(4) - (4)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 327 "gram.y"
    {
					(yyval.boolean) = false;
				;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 331 "gram.y"
    {
					(yyval.boolean) = true;
				;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 342 "gram.y"
    {
					(yyval.str) = (yyvsp[(1) - (1)].word).ident;
					if (yylex() != ':')
						ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
							 errmsg("syntax error, bizzare label \"%s\"", (yyvsp[(1) - (1)].word).ident),
								parser_errposition((yylsp[(1) - (1)]))));
				;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 351 "gram.y"
    {
					(yyval.str) = NULL;
				;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 358 "gram.y"
    {
					(yyval.str) = (yyvsp[(1) - (1)].word).ident;
				;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 362 "gram.y"
    {
					(yyval.str) = NULL;
				;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 369 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
					(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 374 "gram.y"
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

  case 43:

/* Line 1464 of yacc.c  */
#line 410 "gram.y"
    {
					if ((yyvsp[(2) - (2)].stmt)->typ != PLPSM_STMT_DECLARE_VARIABLE)
						yyerror("syntax error");
					(yyvsp[(2) - (2)].stmt)->lineno = plpsm_location_to_lineno((yylsp[(1) - (2)]));
					(yyvsp[(2) - (2)].stmt)->location = (yylsp[(1) - (2)]);
					(yyval.stmt) = (yyvsp[(2) - (2)].stmt);
				;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 418 "gram.y"
    {
					if ((yyvsp[(2) - (4)].stmt)->typ != PLPSM_STMT_DECLARE_CONDITION)
						yyerror("syntax error");
					(yyvsp[(2) - (4)].stmt)->data = (yyvsp[(4) - (4)].condition);
					(yyvsp[(2) - (4)].stmt)->lineno = plpsm_location_to_lineno((yylsp[(1) - (4)]));
					(yyvsp[(2) - (4)].stmt)->location = (yylsp[(1) - (4)]);
					(yyval.stmt) = (yyvsp[(2) - (4)].stmt);
				;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 427 "gram.y"
    {
					int	tok;
					if ((yyvsp[(2) - (4)].stmt)->typ != PLPSM_STMT_DECLARE_CURSOR)
						yyerror("syntax error");
					if ((tok = yylex()) != WORD)
					{
						plpsm_push_back_token(tok);
						(yyvsp[(2) - (4)].stmt)->esql = read_embeded_sql(';', 0, 0, ";", PLPSM_ESQL_QUERY,
													    true, NULL, -1,
															    NULL, NULL);
					}
					else
						(yyvsp[(2) - (4)].stmt)->name = yylval.word.ident;
					(yyvsp[(2) - (4)].stmt)->lineno = plpsm_location_to_lineno((yylsp[(1) - (4)]));
					(yyvsp[(2) - (4)].stmt)->location = (yylsp[(1) - (4)]);
					(yyval.stmt) = (yyvsp[(2) - (4)].stmt);
				;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 445 "gram.y"
    {
					if ((yyvsp[(2) - (6)].stmt)->typ != PLPSM_STMT_DECLARE_HANDLER)
						yyerror("syntax error");
					(yyvsp[(2) - (6)].stmt)->inner_left = (yyvsp[(6) - (6)].stmt);
					(yyvsp[(2) - (6)].stmt)->data = (yyvsp[(5) - (6)].condition);
					(yyvsp[(2) - (6)].stmt)->lineno = plpsm_location_to_lineno((yylsp[(1) - (6)]));
					(yyvsp[(2) - (6)].stmt)->location = (yylsp[(1) - (6)]);
					(yyval.stmt) = (yyvsp[(2) - (6)].stmt);
				;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 457 "gram.y"
    {
					(yyval.stmt) = declare_prefetch();
				;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 464 "gram.y"
    {
					(yyval.condition) = (yyvsp[(1) - (1)].condition);
				;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 468 "gram.y"
    {
					(yyval.condition) = (yyvsp[(1) - (3)].condition);
					(yyval.condition)->next = (yyvsp[(3) - (3)].condition);
				;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 476 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->typ = PLPSM_SQLSTATE;
					new->sqlstate = (yyvsp[(4) - (4)].ival);
					(yyval.condition) = new;
				;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 482 "gram.y"
    {
					(yyval.condition) = NULL;
				;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 489 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (3)]);
					new->typ = PLPSM_SQLSTATE;
					new->sqlstate = (yyvsp[(3) - (3)].ival);
					(yyval.condition) = new;
				;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 497 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (2)]);
					new->typ = PLPSM_SQLSTATE;
					new->sqlstate = MAKE_SQLSTATE('0','2','0','0','0');
					(yyval.condition) = new;
				;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 505 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (1)]);
					new->typ = PLPSM_SQLWARNING;
					(yyval.condition) = new;
				;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 512 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (1)]);
					new->typ = PLPSM_SQLEXCEPTION;
					(yyval.condition) = new;
				;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 519 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (1)]);
					new->typ = PLPSM_CONDITION_NAME;
					new->condition_name = (yyvsp[(1) - (1)].word).ident;
					(yyval.condition) = new;
				;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 530 "gram.y"
    {
					char   *sqlstatestr;

					sqlstatestr = (yyvsp[(1) - (1)].str);

					if (strlen(sqlstatestr) != 5)
						yyerror("invalid SQLSTATE code");
					if (strspn(sqlstatestr, "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ") != 5)
						yyerror("invalid SQLSTATE code");

					(yyval.ival) = MAKE_SQLSTATE(sqlstatestr[0],
										  sqlstatestr[1],
										  sqlstatestr[2],
										  sqlstatestr[3],
										  sqlstatestr[4]);
					if ((yyval.ival) == 0)
						yyerror("invalid SQLSTATE code");
				;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 552 "gram.y"
    {
				;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 554 "gram.y"
    {
				;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 565 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_IF, (yylsp[(1) - (7)]));
					new->esql = (yyvsp[(2) - (7)].esql);
					new->inner_left = (yyvsp[(3) - (7)].stmt);
					new->inner_right = (yyvsp[(5) - (7)].stmt);
					(yyval.stmt) = new;
				;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 575 "gram.y"
    {
					(yyval.stmt) = NULL;
				;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 579 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_IF, (yylsp[(1) - (5)]));
					new->esql = (yyvsp[(2) - (5)].esql);
					new->inner_left = (yyvsp[(3) - (5)].stmt);
					new->inner_right = (yyvsp[(5) - (5)].stmt);
					(yyval.stmt) = new;
				;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 587 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (3)].stmt);
				;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 600 "gram.y"
    {
						Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CASE, (yylsp[(1) - (6)]));
						new->esql = (yyvsp[(2) - (6)].esql);
						new->inner_left = (yyvsp[(3) - (6)].stmt);
						new->inner_right = (yyvsp[(4) - (6)].stmt);
						(yyval.stmt) = new;
					;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 611 "gram.y"
    {
					(yyvsp[(1) - (2)].stmt)->last->next = (yyvsp[(2) - (2)].stmt);
					(yyvsp[(1) - (2)].stmt)->last = (yyvsp[(2) - (2)].stmt);
				;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 616 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 623 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CASE, (yylsp[(1) - (4)]));
					new->esql = (yyvsp[(2) - (4)].esql);
					new->inner_left = (yyvsp[(3) - (4)].stmt);
					new->last = new;
					(yyval.stmt) = new;
				;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 633 "gram.y"
    {
					(yyval.stmt) = NULL;
				;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 637 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (3)].stmt);
				;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 653 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (2)].stmt);
					(yyval.stmt)->location = (yylsp[(1) - (2)]);
				;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 658 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SET, (yylsp[(1) - (8)]));
					new->compound_target = (yyvsp[(3) - (8)].list);
					new->esql_list = (yyvsp[(7) - (8)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 668 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
					(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 673 "gram.y"
    {
					/* expected just very short list */
					(yyvsp[(1) - (3)].stmt)->last->next = (yyvsp[(3) - (3)].stmt);
					(yyvsp[(1) - (3)].stmt)->last = (yyvsp[(3) - (3)].stmt);
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 681 "gram.y"
    {
					(yyvsp[(1) - (3)].stmt)->esql = (yyvsp[(3) - (3)].esql);
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 689 "gram.y"
    {
					(yyvsp[(1) - (1)].stmt)->typ = PLPSM_STMT_SET;
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 694 "gram.y"
    {
					int	i = 0;

					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
					if ((yyvsp[(1) - (3)].stmt)->subscripts == NULL)
						(yyvsp[(1) - (3)].stmt)->subscripts = palloc0(sizeof(Plpsm_ESQL *) * MAXDIM);

					while ((yyvsp[(1) - (3)].stmt)->subscripts[i] != NULL)
					{
						if (++i == MAXDIM)
							yyerror("too much subscripts");
					}

					(yyvsp[(1) - (3)].stmt)->subscripts[i] = (yyvsp[(3) - (3)].esql);
				;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 713 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_UNKNOWN, (yylsp[(1) - (1)]));
					new->target = (yyvsp[(1) - (1)].qualid);
					(yyval.stmt) = new;
				;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 722 "gram.y"
    {
					(yyval.list) = list_make1((yyvsp[(1) - (1)].qualid));
				;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 726 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].qualid));
				;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 733 "gram.y"
    {
					(yyval.qualid) = new_qualid(list_make1((yyvsp[(1) - (1)].word).ident), (yylsp[(1) - (1)]));
				;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 737 "gram.y"
    {
					ListCell	*l;
					List	*idents = NIL;
					foreach (l, (yyvsp[(1) - (1)].cword).idents)
					{
						idents = lappend(idents, strVal(lfirst(l)));
					}
					(yyval.qualid) = new_qualid(idents, (yylsp[(1) - (1)]));
				;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 747 "gram.y"
    {
					/* ToDo: Plpsm_object should be a param type too */
					char buf[32];
					snprintf(buf, sizeof(buf), "$%d", (yyvsp[(1) - (1)].ival));
					(yyval.qualid) = new_qualid(list_make1(pstrdup(buf)), (yylsp[(1) - (1)]));
				;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 764 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PRINT, (yylsp[(1) - (2)]));
					new->esql_list = (yyvsp[(2) - (2)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 779 "gram.y"
    {
					int tok = yylex();
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RETURN, (yylsp[(1) - (1)]));
					if (tok == ';' || tok == 0)
					{
						plpsm_push_back_token(tok);
					}
					else if (tok == SELECT || tok == WITH)
					{
						plpsm_push_back_token(tok);
						new->esql = read_embeded_sql(';', 0, 0,
											";",
											PLPSM_ESQL_QUERY,
											true,
											NULL,
											-1,
											NULL,
											NULL);
						new->option = PLPSM_RETURN_QUERY;
					}
					else if (tok == '(')
					{
						new->esql = read_embeded_sql(')', 0, 0,
											")",
											PLPSM_ESQL_EXPR,
											true,
											NULL,
											-1,
											NULL,
											NULL);
						new->option = PLPSM_RETURN_EXPR;
					}
					else
					{
						plpsm_push_back_token(tok);
						new->esql = read_expr_until_semi();
						new->option = PLPSM_RETURN_EXPR;
					}
					(yyval.stmt) = new;
				;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 829 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_LOOP, (yyvsp[(1) - (7)].str) ? (yylsp[(1) - (7)]) : (yylsp[(2) - (7)]));
					new->name = (yyvsp[(1) - (7)].str);
					new->inner_left = (yyvsp[(3) - (7)].stmt);
					check_labels((yyvsp[(1) - (7)].str), (yyvsp[(7) - (7)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 846 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_WHILE, (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					new->esql = (yyvsp[(3) - (8)].esql);
					new->inner_left = (yyvsp[(4) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 864 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_REPEAT_UNTIL, (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					new->esql = (yyvsp[(6) - (8)].esql);
					new->inner_left = (yyvsp[(3) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 882 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_ITERATE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 898 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_LEAVE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 913 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_GET_DIAGNOSTICS, (yylsp[(1) - (4)]));
					new->option = (yyvsp[(2) - (4)].ival);
					new->data = (yyvsp[(4) - (4)].ginfo);
					(yyval.stmt) = new;
				;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 923 "gram.y"
    {
					(yyvsp[(1) - (1)].ginfo)->next = NULL;
					(yyval.ginfo) = (yyvsp[(1) - (1)].ginfo);
				;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 928 "gram.y"
    {
					Plpsm_gd_info *iterator = (yyvsp[(1) - (3)].ginfo);
					while (iterator != NULL)
					{
						if (iterator->next == NULL)
						{
							iterator->next = (yyvsp[(3) - (3)].ginfo);
							(yyvsp[(3) - (3)].ginfo)->next = NULL;
							break;
						}
						iterator = iterator->next;
					}
					(yyval.ginfo) = (yyvsp[(1) - (3)].ginfo);
				;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 946 "gram.y"
    {
					Plpsm_gd_info *new = palloc(sizeof(Plpsm_gd_info));
					new->target = (yyvsp[(1) - (3)].qualid);
					new->typ = (yyvsp[(3) - (3)].ival);
					(yyval.ginfo) = new;
				;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 955 "gram.y"
    {
					pstate->has_get_diagnostics_stmt = true;
					(yyval.ival) = PLPSM_GDAREA_CURRENT;
				;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 960 "gram.y"
    {
					pstate->has_get_diagnostics_stmt = true;
					pstate->has_get_stacked_diagnostics_stmt = true;
					(yyval.ival) = PLPSM_GDAREA_STACKED;
				;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 966 "gram.y"
    {
					pstate->has_get_diagnostics_stmt = true;
					(yyval.ival) = PLPSM_GDAREA_CURRENT;
				;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 973 "gram.y"
    { (yyval.ival) = PLPSM_GDINFO_SQLSTATE; ;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 974 "gram.y"
    { (yyval.ival) = PLPSM_GDINFO_SQLCODE; ;}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 975 "gram.y"
    { (yyval.ival) = PLPSM_GDINFO_MESSAGE; ;}
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 976 "gram.y"
    { (yyval.ival) = PLPSM_GDINFO_DETAIL; ;}
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 977 "gram.y"
    { (yyval.ival) = PLPSM_GDINFO_HINT; ;}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 978 "gram.y"
    { (yyval.ival) = PLPSM_GDINFO_ROW_COUNT; ;}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 979 "gram.y"
    { (yyval.ival) = PLPSM_GDINFO_CONDITION_IDENTIFIER; ;}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 990 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SIGNAL, (yylsp[(1) - (4)]));
					new->option = (yyvsp[(4) - (4)].ival);
					(yyval.stmt) = new;
				;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 996 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SIGNAL, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 1002 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SIGNAL, (yylsp[(1) - (6)]));
					new->option = (yyvsp[(4) - (6)].ival);
					new->data = (yyvsp[(6) - (6)].sinfo);
					(yyval.stmt) = new;
				;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 1009 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SIGNAL, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->data = (yyvsp[(4) - (4)].sinfo);
					(yyval.stmt) = new;
				;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 1024 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (1)]));
					pstate->has_resignal_stmt = true;
					(yyval.stmt) = new;
				;}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 1030 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (4)]));
					new->option = (yyvsp[(4) - (4)].ival);
					pstate->has_resignal_stmt = true;
					(yyval.stmt) = new;
				;}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 1037 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					pstate->has_resignal_stmt = true;
					(yyval.stmt) = new;
				;}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 1044 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (6)]));
					new->option = (yyvsp[(4) - (6)].ival);
					new->data = (yyvsp[(6) - (6)].sinfo);
					pstate->has_resignal_stmt = true;
					(yyval.stmt) = new;
				;}
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 1052 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->data = (yyvsp[(4) - (4)].sinfo);
					pstate->has_resignal_stmt = true;
					(yyval.stmt) = new;
				;}
    break;

  case 113:

/* Line 1464 of yacc.c  */
#line 1060 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (3)]));
					new->data = (yyvsp[(3) - (3)].sinfo);
					pstate->has_resignal_stmt = true;
					(yyval.stmt) = new;
				;}
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 1070 "gram.y"
    {
					(yyvsp[(1) - (1)].sinfo)->next = NULL;
					(yyval.sinfo) = (yyvsp[(1) - (1)].sinfo);
				;}
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 1075 "gram.y"
    {
					Plpsm_signal_info *iterator = (yyvsp[(1) - (3)].sinfo);

					/* check unique type in list */
					while (iterator != NULL)
					{
						if (iterator->typ == (yyvsp[(3) - (3)].sinfo)->typ)
							yyerror("syntax error, signal info name isn't unique");
						if (iterator->next == NULL)
						{
							iterator->next = (yyvsp[(3) - (3)].sinfo);
							break;
						}

						iterator = iterator->next;
					}
					(yyval.sinfo) = (yyvsp[(1) - (3)].sinfo);
				;}
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 1097 "gram.y"
    {
					Plpsm_signal_info *new = palloc(sizeof(Plpsm_signal_info));
					new->typ = PLPSM_SINFO_DETAIL;
					new->value = (yyvsp[(3) - (3)].str);
					(yyval.sinfo) = new;
				;}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 1104 "gram.y"
    {
					Plpsm_signal_info *new = palloc(sizeof(Plpsm_signal_info));
					new->typ = PLPSM_SINFO_HINT;
					new->value = (yyvsp[(3) - (3)].str);
					(yyval.sinfo) = new;
				;}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 1111 "gram.y"
    {
					Plpsm_signal_info *new = palloc(sizeof(Plpsm_signal_info));
					new->typ = PLPSM_SINFO_MESSAGE;
					new->value = (yyvsp[(3) - (3)].str);
					(yyval.sinfo) = new;
				;}
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 1118 "gram.y"
    {
					Plpsm_signal_info *new = palloc(sizeof(Plpsm_signal_info));
					new->typ = PLPSM_SINFO_DETAIL;
					new->var = (yyvsp[(3) - (3)].qualid);
					(yyval.sinfo) = new;
				;}
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 1125 "gram.y"
    {
					Plpsm_signal_info *new = palloc(sizeof(Plpsm_signal_info));
					new->typ = PLPSM_SINFO_HINT;
					new->var = (yyvsp[(3) - (3)].qualid);
					(yyval.sinfo) = new;
				;}
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 1132 "gram.y"
    {
					Plpsm_signal_info *new = palloc(sizeof(Plpsm_signal_info));
					new->typ = PLPSM_SINFO_MESSAGE;
					new->var = (yyvsp[(3) - (3)].qualid);
					(yyval.sinfo) = new;
				;}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 1147 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PREPARE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->esql = (yyvsp[(4) - (4)].esql);
					(yyval.stmt) = new;
				;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 1164 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 1170 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->compound_target = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 1177 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (6)]));
					new->name = (yyvsp[(2) - (6)].word).ident;
					new->compound_target = (yyvsp[(4) - (6)].list);
					new->variables = (yyvsp[(6) - (6)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 1185 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->variables = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 1201 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE_IMMEDIATE, (yylsp[(1) - (3)]));
					new->esql = (yyvsp[(3) - (3)].esql);
					(yyval.stmt) = new;
				;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 1217 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_OPEN, (yylsp[(1) - (2)]));
					new->target = (yyvsp[(2) - (2)].qualid);
					(yyval.stmt) = new;
				;}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 1223 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_OPEN, (yylsp[(1) - (4)]));
					new->target = (yyvsp[(2) - (4)].qualid);
					new->variables = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 1239 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_FETCH, (yylsp[(1) - (4)]));
					new->target = (yyvsp[(2) - (4)].qualid);
					new->compound_target = (yyvsp[(4) - (4)].list);
					new->data = (yyvsp[(2) - (4)].qualid);
					(yyval.stmt) = new;
				;}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 1256 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CLOSE, (yylsp[(1) - (2)]));
					new->target = (yyvsp[(2) - (2)].qualid);
					(yyval.stmt) = new;
				;}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 1271 "gram.y"
    {
					Plpsm_stmt *new = (yyvsp[(3) - (8)].stmt);
					new->location = (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]);
					new->lineno = plpsm_location_to_lineno(new->location);
					new->name = (yyvsp[(1) - (8)].str);
					new->inner_left = (yyvsp[(4) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 1287 "gram.y"
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
					(yyval.stmt) = new;
				;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 1355 "gram.y"
    { (yyval.stmt) = make_stmt_sql((yylsp[(1) - (1)])); ;}
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 1356 "gram.y"
    { (yyval.stmt) = make_stmt_sql((yylsp[(1) - (1)])); ;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 1357 "gram.y"
    { (yyval.stmt) = make_stmt_sql((yylsp[(1) - (1)])); ;}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 1358 "gram.y"
    { (yyval.stmt) = make_stmt_sql((yylsp[(1) - (1)])); ;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 1363 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SELECT_INTO, (yylsp[(1) - (3)]));
					new->esql_list = (yyvsp[(2) - (3)].list);
					new->compound_target = (yyvsp[(3) - (3)].list);
					if (list_length((yyvsp[(2) - (3)].list)) != list_length((yyvsp[(3) - (3)].list)))
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("number of target variables is different than number of attributies"),
								 parser_errposition((yylsp[(1) - (3)]))));

					(yyval.stmt) = new;
				;}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 1376 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SELECT_INTO, (yylsp[(1) - (4)]));
					new->esql_list = (yyvsp[(2) - (4)].list);
					new->compound_target = (yyvsp[(3) - (4)].list);
					if (list_length((yyvsp[(2) - (4)].list)) != list_length((yyvsp[(3) - (4)].list)))
						ereport(ERROR,
								(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("number of target variables is different than number of attributies"),
								 parser_errposition((yylsp[(1) - (4)]))));

					new->from_clause = read_embeded_sql(';', 0, 0, ";", PLPSM_ESQL_QUERY, false, 
														    NULL, (yylsp[(4) - (4)]), 
															    NULL, NULL);
					(yyval.stmt) = new;
				;}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 1394 "gram.y"
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
					(yyval.list) = esql_list;
				;}
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 1414 "gram.y"
    {
					(yyval.esql) = read_expr_until_semi();
				;}
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 1421 "gram.y"
    {
					(yyval.list) = list_make1((yyvsp[(1) - (1)].esql));
				;}
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 1425 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].esql));
				;}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 1431 "gram.y"
    {
					Plpsm_ESQL *esql = read_embeded_sql(']', -1, -1, "]", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					(yyval.esql) = esql;
				;}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 1441 "gram.y"
    {
					Plpsm_ESQL *esql = read_embeded_sql(';',',', 0, "; or ,", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					(yyval.esql) = esql;
				;}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 1451 "gram.y"
    {
					Plpsm_ESQL *esql = read_embeded_sql(DO, -1, -1, "DO", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					(yyval.esql) = esql;
				;}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 1461 "gram.y"
    {
					Plpsm_ESQL *esql = read_embeded_sql(THEN, -1, -1, "THEN", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					(yyval.esql) = esql;
				;}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 1471 "gram.y"
    {
					Plpsm_ESQL *esql = read_embeded_sql(END, -1, -1, "END", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					(yyval.esql) = esql;
				;}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 1481 "gram.y"
    {
					int endtok;
					Plpsm_ESQL *esql = read_embeded_sql(';',INTO, USING, "; or INTO or USING", 
												PLPSM_ESQL_EXPR, true, 
														    &endtok, -1, 
															    NULL, NULL);
					if (endtok != ';')
						plpsm_push_back_token(endtok);
					(yyval.esql) = esql;
				;}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 1494 "gram.y"
    {
					int endtok;
					Plpsm_ESQL *esql = read_embeded_sql(';',',', ')', "; or , or )", 
												PLPSM_ESQL_EXPR, true, 
														    &endtok, -1, 
															    NULL, NULL);

					if (endtok != ';' && endtok != ',')
						plpsm_push_back_token(endtok);
					(yyval.esql) = esql;
				;}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 1508 "gram.y"
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
					(yyval.esql) = esql;
				;}
    break;



/* Line 1464 of yacc.c  */
#line 3723 "gram.c"
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

  yyerror_range[1] = yylloc;

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

  yyerror_range[1] = yylsp[1-yylen];
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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
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



/* Line 1684 of yacc.c  */
#line 1524 "gram.y"


static Plpsm_stmt *
make_stmt_sql(int location)
{
	Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SQL, location);
	new->esql = read_embeded_sql(';', 0, 0, ";", PLPSM_ESQL_QUERY, true, NULL, location, NULL, NULL);
	return new;
}

static Plpsm_ESQL *
read_expr_until_semi(void)
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
		case CONDITION_IDENTIFIER:
		case CONTINUE:
		case CURRENT:
		case DETAIL_TEXT:
		case DIAGNOSTICS:
		case EXIT:
		case FOUND:
		case HINT_TEXT:
		case IMMEDIATE:
		case MESSAGE_TEXT:
		case NO:
		case NOT:
		case RETURNED_SQLCODE:
		case RETURNED_SQLSTATE:
		case ROW_COUNT:
		case SCROLL:
		case SQLEXCEPTION:
		case SQLSTATE:
		case SQLWARNING:
		case SQLCODE:
		case STACKED:
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
		case PLPSM_STMT_SIGNAL:
			return "signal statement";
		case PLPSM_STMT_RESIGNAL:
			return "resignal statement";
		case PLPSM_STMT_GET_DIAGNOSTICS:
			return "get diagnostics statement";
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

	if (stmt->typ == PLPSM_STMT_SET && stmt->subscripts != NULL)
	{
		int	i = 0;
		while (i < MAXDIM && stmt->subscripts[i] != NULL)
		{
			appendStringInfoChar(ds, '[');
			esql_out(ds, stmt->subscripts[i]);
			appendStringInfoChar(ds, ']');
			i++;
		}
	}

	appendStringInfoChar(ds, '\n');
	appendStringInfo(ds, "%s| Compound target: ", ident);
	pqualid_list_out(ds,stmt->compound_target);
	appendStringInfoChar(ds,'\n');
	appendStringInfo(ds, "%s| Variables: ", ident);
	pqualid_list_out(ds, stmt->variables);
	appendStringInfoChar(ds, '\n');
	if (stmt->typ == PLPSM_STMT_SIGNAL)
		appendStringInfo(ds, "%s| Option: %s\n", ident, unpack_sql_state(stmt->option));
	else
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
		case PLPSM_STMT_SIGNAL:
		case PLPSM_STMT_RESIGNAL:
			{
				Plpsm_signal_info *sinfo = (Plpsm_signal_info *) stmt->data;
				if (sinfo != NULL)
				{
					appendStringInfo(ds, "%s| info:", ident);
					while (sinfo != NULL)
					{
						switch (sinfo->typ)
						{
							case PLPSM_SINFO_DETAIL:
								
								appendStringInfoString(ds, " DETAIL=");
								if (sinfo->var != NULL)
									pqualid_out(ds, sinfo->var);
								else
									appendStringInfoString(ds, sinfo->value);
								break;
							case PLPSM_SINFO_HINT:
								appendStringInfo(ds, " HINT=");
								if (sinfo->var != NULL)
									pqualid_out(ds, sinfo->var);
								else
									appendStringInfoString(ds, sinfo->value);
								break;
							case PLPSM_SINFO_MESSAGE:
								appendStringInfo(ds, " MESSAGE=");
								if (sinfo->var != NULL)
									pqualid_out(ds, sinfo->var);
								else
									appendStringInfoString(ds, sinfo->value);
								break;
						}
						sinfo = sinfo->next;
					}
					appendStringInfoChar(ds, '\n');
				}
				break;
			}
		case PLPSM_STMT_GET_DIAGNOSTICS:
			{
				Plpsm_gd_info *gdinfo = (Plpsm_gd_info *) stmt->data;
				if (gdinfo != NULL)
				{
					pqualid_out(ds, gdinfo->target);
					appendStringInfo(ds, "%s| info:", ident);
					while (gdinfo != NULL)
					{
						switch (gdinfo->typ)
						{
							case PLPSM_GDINFO_DETAIL:
								appendStringInfoString(ds, " = DETAIL_TEXT");
								break;
							case PLPSM_GDINFO_HINT:
								appendStringInfoString(ds, " = HINT_TEXT");
								break;
							case PLPSM_GDINFO_MESSAGE:
								appendStringInfoString(ds, " = MESSAGE_TEXT");
								break;
							case PLPSM_GDINFO_SQLSTATE:
								appendStringInfoString(ds, " = SQLSTATE");
								break;
							case PLPSM_GDINFO_SQLCODE:
								appendStringInfoString(ds, " = SQLCODE");
								break;
							case PLPSM_GDINFO_ROW_COUNT:
								appendStringInfoString(ds, " = ROW_COUNT");
								break;
							case PLPSM_GDINFO_CONDITION_IDENTIFIER:
								appendStringInfoString(ds, " = CONDITION_IDENTIFIER");
								break;
							default:
								/* be compiler quite */;
						}
						gdinfo = gdinfo->next;
					}
					appendStringInfoChar(ds, '\n');
				}
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
	bool	first_token = true;

	esql->typ = expected_type;

	initStringInfo(&ds);

	for (;;)
	{
		/* read a current location before you read a next tag */
		tok = yylex();

		if (first_token && tok == SELECT)
		{
			/* comma isn't separator in end tag in this case */
			until2 = -1;
			first_token = false;
			esql->typ = PLPSM_ESQL_QUERY;
		}
		else
			first_token = false;

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
					(tok == ']' && parenlevel == 0) ||
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
			case SIGNAL:
			case WHILE:
			case UNTIL:
			case GET:
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
			if (esql->typ == PLPSM_ESQL_EXPR)
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

	if (esql->typ == PLPSM_ESQL_EXPR)
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
	else if (esql->typ == PLPSM_ESQL_DATATYPE)
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

