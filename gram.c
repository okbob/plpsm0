
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

static Plpsm_stmt *make_stmt_sql(const char *prefix, int location);

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
#line 166 "gram.c"

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
     CASE = 273,
     CLOSE = 274,
     CONDITION = 275,
     CONTINUE = 276,
     CURSOR = 277,
     DECLARE = 278,
     DEFAULT = 279,
     DO = 280,
     ELSE = 281,
     ELSEIF = 282,
     END = 283,
     EXECUTE = 284,
     EXIT = 285,
     FETCH = 286,
     FOR = 287,
     FOUND = 288,
     FROM = 289,
     HANDLER = 290,
     IF = 291,
     IMMEDIATE = 292,
     INTO = 293,
     ITERATE = 294,
     LEAVE = 295,
     LOOP = 296,
     NO = 297,
     NOT = 298,
     OPEN = 299,
     PREPARE = 300,
     PRINT = 301,
     REPEAT = 302,
     RETURN = 303,
     SCROLL = 304,
     SELECT = 305,
     SET = 306,
     SQLEXCEPTION = 307,
     SQLSTATE = 308,
     SQLCODE = 309,
     SQLWARNING = 310,
     THEN = 311,
     UNDO = 312,
     UNTIL = 313,
     USING = 314,
     VALUE = 315,
     WHEN = 316,
     WHILE = 317,
     INSERT = 318,
     UPDATE = 319,
     DELETE = 320
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 89 "gram.y"

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
#line 284 "gram.c"
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
#line 309 "gram.c"

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
#define YYFINAL  64
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   622

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  56
/* YYNRULES -- Number of rules.  */
#define YYNRULES  110
/* YYNRULES -- Number of states.  */
#define YYNSTATES  197

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   320

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      69,    70,     2,     2,    68,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    67,    66,
       2,    71,     2,     2,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    49,    51,    53,    55,    57,    64,
      71,    80,    85,    88,    89,    91,    92,    94,    98,   101,
     106,   111,   118,   119,   121,   125,   130,   131,   135,   138,
     140,   142,   144,   146,   148,   149,   157,   158,   164,   168,
     175,   176,   179,   181,   186,   187,   191,   194,   203,   205,
     209,   213,   215,   217,   221,   223,   225,   227,   230,   232,
     240,   249,   258,   261,   264,   269,   272,   277,   284,   289,
     293,   296,   301,   306,   309,   318,   319,   321,   323,   325,
     329,   334,   335,   336,   338,   342,   343,   344,   345,   346,
     347
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      73,     0,    -1,    76,    74,    -1,    66,    -1,    -1,    76,
      -1,    75,    66,    76,    -1,    77,    -1,    78,    -1,    96,
      -1,   102,    -1,   104,    -1,   105,    -1,   106,    -1,   107,
      -1,   108,    -1,   103,    -1,   109,    -1,   110,    -1,   111,
      -1,   112,    -1,   113,    -1,   114,    -1,   115,    -1,    89,
      -1,    91,    -1,   117,    -1,   118,    -1,    79,    17,    75,
      66,    28,    80,    -1,    79,    17,    81,    66,    28,    80,
      -1,    79,    17,    81,    66,    75,    66,    28,    80,    -1,
      79,    17,    28,    80,    -1,    14,    67,    -1,    -1,    14,
      -1,    -1,    82,    -1,    81,    66,    82,    -1,    23,    83,
      -1,    23,    83,    20,    85,    -1,    23,    83,    22,    32,
      -1,    23,    83,    35,    32,    84,    76,    -1,    -1,    86,
      -1,    84,    68,    86,    -1,    32,    53,    88,    87,    -1,
      -1,    53,    88,    87,    -1,    43,    33,    -1,    55,    -1,
      52,    -1,    14,    -1,     5,    -1,    60,    -1,    -1,    36,
     124,    75,    66,    90,    28,    36,    -1,    -1,    27,   124,
      75,    66,    90,    -1,    26,    75,    66,    -1,    18,    92,
      93,    95,    28,    18,    -1,    -1,    93,    94,    -1,    94,
      -1,    61,   124,    75,    66,    -1,    -1,    26,    75,    66,
      -1,    51,    97,    -1,    51,    69,   100,    70,    71,    69,
     121,    70,    -1,    98,    -1,    97,    68,    98,    -1,    99,
      71,   122,    -1,   101,    -1,   101,    -1,   100,    68,   101,
      -1,    14,    -1,    15,    -1,    10,    -1,    46,   121,    -1,
      48,    -1,    79,    41,    75,    66,    28,    41,    80,    -1,
      79,    62,   123,    75,    66,    28,    62,    80,    -1,    79,
      47,    75,    66,    58,   125,    47,    80,    -1,    39,    14,
      -1,    40,    14,    -1,    45,    14,    34,   120,    -1,    29,
      14,    -1,    29,    14,    38,   100,    -1,    29,    14,    38,
     100,    59,   100,    -1,    29,    14,    59,   100,    -1,    29,
      37,   126,    -1,    44,   101,    -1,    44,   101,    59,   100,
      -1,    31,   101,    38,   100,    -1,    19,   101,    -1,    79,
      32,   116,    75,    66,    28,    32,    80,    -1,    -1,    63,
      -1,    64,    -1,    65,    -1,    50,   119,   100,    -1,    50,
     119,   100,    34,    -1,    -1,    -1,   127,    -1,   121,    68,
     127,    -1,    -1,    -1,    -1,    -1,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   196,   196,   205,   209,   214,   218,   229,   246,   247,
     248,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   281,   289,
     297,   308,   318,   323,   329,   334,   340,   345,   381,   389,
     398,   414,   427,   433,   437,   444,   448,   454,   459,   465,
     472,   479,   485,   505,   508,   517,   528,   531,   541,   553,
     564,   585,   590,   596,   607,   610,   626,   630,   640,   645,
     653,   662,   671,   675,   682,   686,   690,   706,   721,   744,
     760,   777,   794,   809,   824,   841,   847,   854,   862,   878,
     894,   900,   915,   931,   946,   962,  1030,  1031,  1032,  1036,
    1046,  1060,  1083,  1089,  1093,  1100,  1115,  1129,  1143,  1157,
    1174
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT", "FCONST", "SCONST", "BCONST",
  "XCONST", "Op", "ICONST", "PARAM", "TYPECAST", "DOT_DOT", "COLON_EQUALS",
  "WORD", "CWORD", "AS", "BEGIN", "CASE", "CLOSE", "CONDITION", "CONTINUE",
  "CURSOR", "DECLARE", "DEFAULT", "DO", "ELSE", "ELSEIF", "END", "EXECUTE",
  "EXIT", "FETCH", "FOR", "FOUND", "FROM", "HANDLER", "IF", "IMMEDIATE",
  "INTO", "ITERATE", "LEAVE", "LOOP", "NO", "NOT", "OPEN", "PREPARE",
  "PRINT", "REPEAT", "RETURN", "SCROLL", "SELECT", "SET", "SQLEXCEPTION",
  "SQLSTATE", "SQLCODE", "SQLWARNING", "THEN", "UNDO", "UNTIL", "USING",
  "VALUE", "WHEN", "WHILE", "INSERT", "UPDATE", "DELETE", "';'", "':'",
  "','", "'('", "')'", "'='", "$accept", "function", "opt_semi",
  "statements", "dstmt", "stmt", "stmt_compound", "opt_label",
  "opt_end_label", "declarations", "declaration", "declare_prefetch",
  "condition_list", "opt_sqlstate", "condition", "sqlstate", "opt_value",
  "stmt_if", "stmt_else", "stmt_case", "opt_expr_until_when",
  "case_when_list", "case_when", "opt_case_else", "stmt_set",
  "assign_list", "assign_item", "target", "qual_identif_list",
  "qual_identif", "stmt_print", "stmt_return", "stmt_loop", "stmt_while",
  "stmt_repeat_until", "stmt_iterate", "stmt_leave", "stmt_prepare",
  "stmt_execute", "stmt_execute_immediate", "stmt_open", "stmt_fetch",
  "stmt_close", "stmt_for", "for_prefetch", "stmt_sql", "stmt_select_into",
  "expr_list_into", "expr_until_semi", "expr_list",
  "expr_until_semi_or_coma", "expr_until_do", "expr_until_then",
  "expr_until_end", "expr_until_semi_into_using",
  "expr_until_semi_or_coma_or_parent", 0
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
     315,   316,   317,   318,   319,   320,    59,    58,    44,    40,
      41,    61
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    72,    73,    74,    74,    75,    75,    76,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    78,    78,
      78,    78,    79,    79,    80,    80,    81,    81,    82,    82,
      82,    82,    83,    84,    84,    85,    85,    86,    86,    86,
      86,    86,    87,    88,    88,    89,    90,    90,    90,    91,
      92,    93,    93,    94,    95,    95,    96,    96,    97,    97,
      98,    99,   100,   100,   101,   101,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   110,   110,   110,   111,
     112,   112,   113,   114,   115,   116,   117,   117,   117,   118,
     118,   119,   120,   121,   121,   122,   123,   124,   125,   126,
     127
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     0,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     6,     6,
       8,     4,     2,     0,     1,     0,     1,     3,     2,     4,
       4,     6,     0,     1,     3,     4,     0,     3,     2,     1,
       1,     1,     1,     1,     0,     7,     0,     5,     3,     6,
       0,     2,     1,     4,     0,     3,     2,     8,     1,     3,
       3,     1,     1,     3,     1,     1,     1,     2,     1,     7,
       8,     8,     2,     2,     4,     2,     4,     6,     4,     3,
       2,     4,     4,     2,     8,     0,     1,     1,     1,     3,
       4,     0,     0,     1,     3,     0,     0,     0,     0,     0,
       0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      33,     0,    60,     0,     0,     0,   107,     0,     0,     0,
       0,   110,    78,   101,     0,    96,    97,    98,     0,     4,
       7,     8,     0,    24,    25,     9,    10,    16,    11,    12,
      13,    14,    15,    17,    18,    19,    20,    21,    22,    23,
      26,    27,    32,     0,    76,    74,    75,    93,    85,   109,
       0,    33,    82,    83,    90,     0,    77,   103,     0,     0,
      66,    68,     0,    71,     1,     3,     2,    33,    95,    33,
      33,   106,   107,    64,    62,     0,     0,    89,     0,     0,
       5,     0,   102,   110,    99,    72,     0,     0,   105,    42,
      35,     0,     0,    36,    33,     0,     0,    33,    33,    33,
      61,     0,    86,    88,    92,    33,    91,    84,   104,   100,
       0,     0,    69,    70,    38,    34,    31,    33,    33,     0,
      33,    33,     0,     0,     0,     0,     0,    33,   107,     6,
       0,    73,     0,    46,     0,     0,    35,    35,     0,    37,
      33,     0,   108,    33,    33,    33,    59,    87,     0,    33,
       0,   110,     0,    39,    40,     0,    28,    29,    33,     0,
      35,     0,     0,    33,     0,    55,     0,    54,    51,     0,
      50,    54,    49,    33,    43,    35,    35,    79,    35,    35,
      33,    67,    53,     0,    48,     0,     0,    41,    30,    94,
      81,    80,    57,    52,    45,    47,    44
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    18,    66,    79,    80,    20,    21,    22,   116,    92,
      93,   114,   173,   153,   174,   194,   183,    23,   130,    24,
      43,    73,    74,   101,    25,    60,    61,    62,    84,    85,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    94,    40,    41,    58,   107,    56,
     113,    97,    51,   161,    77,    57
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -122
static const yytype_int16 yypact[] =
{
     557,   -47,  -122,    57,     5,    57,  -122,     7,    10,    57,
      16,  -122,  -122,  -122,    -1,  -122,  -122,  -122,    37,   -26,
    -122,  -122,    -6,  -122,  -122,  -122,  -122,  -122,  -122,  -122,
    -122,  -122,  -122,  -122,  -122,  -122,  -122,  -122,  -122,  -122,
    -122,  -122,  -122,   -18,  -122,  -122,  -122,  -122,    -7,  -122,
       8,   557,  -122,  -122,   -12,    26,     9,  -122,    57,    57,
      14,  -122,    12,  -122,  -122,  -122,  -122,   177,  -122,   557,
     557,  -122,  -122,   -16,  -122,    57,    57,  -122,    57,    20,
    -122,    57,  -122,  -122,   -17,  -122,   -32,    57,  -122,  -122,
      73,    24,    33,  -122,   557,    34,    38,   557,   557,   557,
    -122,    64,   -41,    28,    28,    83,    28,  -122,  -122,  -122,
      57,    32,  -122,  -122,   104,  -122,  -122,   253,   215,    40,
     291,   329,    41,    47,    49,    98,    57,   557,  -122,  -122,
      90,  -122,    56,   100,   103,   105,    73,    73,    70,  -122,
     367,    89,  -122,   405,   138,   443,  -122,    28,    72,   557,
     106,  -122,    88,  -122,  -122,    36,  -122,  -122,   481,   117,
      73,   107,    91,   519,    84,  -122,    -5,    95,  -122,   118,
    -122,    95,  -122,    30,  -122,    73,    73,  -122,    73,    73,
      83,  -122,  -122,   154,  -122,   154,    36,  -122,  -122,  -122,
    -122,  -122,  -122,  -122,  -122,  -122,  -122
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -122,  -122,  -122,   -65,     0,  -122,  -122,  -122,  -121,  -122,
      42,  -122,  -122,  -122,   -25,   -23,    -3,  -122,   -15,  -122,
    -122,  -122,    97,  -122,  -122,  -122,    85,  -122,   -53,    -2,
    -122,  -122,  -122,  -122,  -122,  -122,  -122,  -122,  -122,  -122,
    -122,  -122,  -122,  -122,  -122,  -122,  -122,  -122,  -122,    25,
    -122,  -122,   -64,  -122,  -122,    92
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -66
static const yytype_int16 yytable[] =
{
      19,    47,    91,    50,    95,    96,    86,    54,    98,    44,
      99,    67,    63,    45,    46,   156,   157,   109,   126,    48,
      42,    52,   102,   103,    53,   104,    68,   110,   106,   119,
      55,    75,   122,   123,   124,    69,   110,    64,   111,   177,
      65,    70,    49,    72,     1,    72,    78,    81,     2,     3,
     168,   110,    76,   138,   188,   189,    71,   190,   191,     4,
      82,     5,   148,    83,   149,   181,     6,    44,    59,     7,
       8,    45,    46,   147,     9,    10,    11,    83,    12,   169,
      13,    14,    87,    88,   164,    63,   105,   115,   170,   171,
     117,   172,   125,    15,    16,    17,   110,     1,   186,   118,
     120,     2,     3,   132,   121,   129,   140,   143,   131,   127,
     128,   -56,     4,   144,     5,   145,   146,   129,   150,     6,
     129,   129,     7,     8,   133,   151,   134,     9,    10,    11,
     160,    12,   152,    13,    14,   154,   158,   155,   163,   135,
     129,   167,   165,   129,   129,   129,    15,    16,    17,   176,
     180,   184,     1,   179,   178,   182,     2,     3,   129,   193,
     139,   196,   195,   129,   -63,   192,   -63,     4,   185,     5,
     100,     0,   112,   187,     6,   108,   166,     7,     8,     0,
     129,     0,     9,    10,    11,     0,    12,     0,    13,    14,
       0,     1,     0,     0,     0,     2,     3,     0,     0,   -63,
      89,    15,    16,    17,     0,    90,     4,     0,     5,     0,
       0,     0,     0,     6,     0,     0,     7,     8,     0,     0,
       0,     9,    10,    11,     0,    12,     0,    13,    14,     1,
       0,     0,     0,     2,     3,     0,     0,     0,    89,     0,
      15,    16,    17,   137,     4,     0,     5,     0,     0,     0,
       0,     6,     0,     0,     7,     8,     0,     0,     0,     9,
      10,    11,     0,    12,     0,    13,    14,     1,     0,     0,
       0,     2,     3,     0,     0,     0,     0,     0,    15,    16,
      17,   136,     4,     0,     5,     0,     0,     0,     0,     6,
       0,     0,     7,     8,     0,     0,     0,     9,    10,    11,
       0,    12,     0,    13,    14,     1,     0,     0,     0,     2,
       3,     0,     0,     0,     0,     0,    15,    16,    17,   141,
       4,     0,     5,     0,     0,     0,     0,     6,     0,     0,
       7,     8,     0,     0,     0,     9,    10,    11,     0,    12,
       0,    13,    14,     1,     0,     0,     0,     2,     3,     0,
       0,     0,     0,     0,    15,    16,    17,     0,     4,     0,
       5,     0,     0,     0,     0,     6,     0,     0,     7,     8,
       0,     0,     0,     9,    10,    11,     0,    12,     0,    13,
      14,     1,     0,     0,     0,     2,     3,   142,     0,     0,
       0,     0,    15,    16,    17,   159,     4,     0,     5,     0,
       0,     0,     0,     6,     0,     0,     7,     8,     0,     0,
       0,     9,    10,    11,     0,    12,     0,    13,    14,     1,
       0,     0,     0,     2,     3,     0,     0,     0,     0,     0,
      15,    16,    17,   162,     4,     0,     5,     0,     0,     0,
       0,     6,     0,     0,     7,     8,     0,     0,     0,     9,
      10,    11,     0,    12,     0,    13,    14,     1,     0,     0,
       0,     2,     3,     0,     0,     0,     0,     0,    15,    16,
      17,   -65,     4,     0,     5,     0,     0,     0,     0,     6,
       0,     0,     7,     8,     0,     0,     0,     9,    10,    11,
       0,    12,     0,    13,    14,     1,     0,     0,     0,     2,
       3,     0,     0,     0,     0,     0,    15,    16,    17,   175,
       4,     0,     5,     0,     0,     0,     0,     6,     0,     0,
       7,     8,     0,     0,     0,     9,    10,    11,     0,    12,
       0,    13,    14,     1,     0,     0,     0,     2,     3,     0,
       0,     0,     0,     0,    15,    16,    17,   -58,     4,     0,
       5,     0,     0,     0,     0,     6,     0,     0,     7,     8,
       0,     0,     0,     9,    10,    11,     0,    12,     0,    13,
      14,     1,     0,     0,     0,     2,     3,     0,     0,     0,
       0,     0,    15,    16,    17,     0,     4,     0,     5,     0,
       0,     0,     0,     6,     0,     0,     7,     8,     0,     0,
       0,     9,    10,    11,     0,    12,     0,    13,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      15,    16,    17
};

static const yytype_int16 yycheck[] =
{
       0,     3,    67,     5,    69,    70,    59,     9,    72,    10,
      26,    17,    14,    14,    15,   136,   137,    34,    59,    14,
      67,    14,    75,    76,    14,    78,    32,    68,    81,    94,
      14,    38,    97,    98,    99,    41,    68,     0,    70,   160,
      66,    47,    37,    61,    14,    61,    38,    59,    18,    19,
      14,    68,    59,   118,   175,   176,    62,   178,   179,    29,
      34,    31,   127,    68,   128,    70,    36,    10,    69,    39,
      40,    14,    15,   126,    44,    45,    46,    68,    48,    43,
      50,    51,    68,    71,   149,    87,    66,    14,    52,    53,
      66,    55,    28,    63,    64,    65,    68,    14,    68,    66,
      66,    18,    19,    71,    66,   105,    66,    66,   110,    26,
      27,    28,    29,    66,    31,    66,    18,   117,    28,    36,
     120,   121,    39,    40,    20,    69,    22,    44,    45,    46,
      41,    48,    32,    50,    51,    32,    66,    32,    66,    35,
     140,    53,    36,   143,   144,   145,    63,    64,    65,    32,
      66,    33,    14,    62,    47,    60,    18,    19,   158,     5,
     118,   186,   185,   163,    26,   180,    28,    29,   171,    31,
      73,    -1,    87,   173,    36,    83,   151,    39,    40,    -1,
     180,    -1,    44,    45,    46,    -1,    48,    -1,    50,    51,
      -1,    14,    -1,    -1,    -1,    18,    19,    -1,    -1,    61,
      23,    63,    64,    65,    -1,    28,    29,    -1,    31,    -1,
      -1,    -1,    -1,    36,    -1,    -1,    39,    40,    -1,    -1,
      -1,    44,    45,    46,    -1,    48,    -1,    50,    51,    14,
      -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    23,    -1,
      63,    64,    65,    28,    29,    -1,    31,    -1,    -1,    -1,
      -1,    36,    -1,    -1,    39,    40,    -1,    -1,    -1,    44,
      45,    46,    -1,    48,    -1,    50,    51,    14,    -1,    -1,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    63,    64,
      65,    28,    29,    -1,    31,    -1,    -1,    -1,    -1,    36,
      -1,    -1,    39,    40,    -1,    -1,    -1,    44,    45,    46,
      -1,    48,    -1,    50,    51,    14,    -1,    -1,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    63,    64,    65,    28,
      29,    -1,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,
      39,    40,    -1,    -1,    -1,    44,    45,    46,    -1,    48,
      -1,    50,    51,    14,    -1,    -1,    -1,    18,    19,    -1,
      -1,    -1,    -1,    -1,    63,    64,    65,    -1,    29,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    39,    40,
      -1,    -1,    -1,    44,    45,    46,    -1,    48,    -1,    50,
      51,    14,    -1,    -1,    -1,    18,    19,    58,    -1,    -1,
      -1,    -1,    63,    64,    65,    28,    29,    -1,    31,    -1,
      -1,    -1,    -1,    36,    -1,    -1,    39,    40,    -1,    -1,
      -1,    44,    45,    46,    -1,    48,    -1,    50,    51,    14,
      -1,    -1,    -1,    18,    19,    -1,    -1,    -1,    -1,    -1,
      63,    64,    65,    28,    29,    -1,    31,    -1,    -1,    -1,
      -1,    36,    -1,    -1,    39,    40,    -1,    -1,    -1,    44,
      45,    46,    -1,    48,    -1,    50,    51,    14,    -1,    -1,
      -1,    18,    19,    -1,    -1,    -1,    -1,    -1,    63,    64,
      65,    28,    29,    -1,    31,    -1,    -1,    -1,    -1,    36,
      -1,    -1,    39,    40,    -1,    -1,    -1,    44,    45,    46,
      -1,    48,    -1,    50,    51,    14,    -1,    -1,    -1,    18,
      19,    -1,    -1,    -1,    -1,    -1,    63,    64,    65,    28,
      29,    -1,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,
      39,    40,    -1,    -1,    -1,    44,    45,    46,    -1,    48,
      -1,    50,    51,    14,    -1,    -1,    -1,    18,    19,    -1,
      -1,    -1,    -1,    -1,    63,    64,    65,    28,    29,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    39,    40,
      -1,    -1,    -1,    44,    45,    46,    -1,    48,    -1,    50,
      51,    14,    -1,    -1,    -1,    18,    19,    -1,    -1,    -1,
      -1,    -1,    63,    64,    65,    -1,    29,    -1,    31,    -1,
      -1,    -1,    -1,    36,    -1,    -1,    39,    40,    -1,    -1,
      -1,    44,    45,    46,    -1,    48,    -1,    50,    51,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      63,    64,    65
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    14,    18,    19,    29,    31,    36,    39,    40,    44,
      45,    46,    48,    50,    51,    63,    64,    65,    73,    76,
      77,    78,    79,    89,    91,    96,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     117,   118,    67,    92,    10,    14,    15,   101,    14,    37,
     101,   124,    14,    14,   101,    14,   121,   127,   119,    69,
      97,    98,    99,   101,     0,    66,    74,    17,    32,    41,
      47,    62,    61,    93,    94,    38,    59,   126,    38,    75,
      76,    59,    34,    68,   100,   101,   100,    68,    71,    23,
      28,    75,    81,    82,   116,    75,    75,   123,   124,    26,
      94,    95,   100,   100,   100,    66,   100,   120,   127,    34,
      68,    70,    98,   122,    83,    14,    80,    66,    66,    75,
      66,    66,    75,    75,    75,    28,    59,    26,    27,    76,
      90,   101,    71,    20,    22,    35,    28,    28,    75,    82,
      66,    28,    58,    66,    66,    66,    18,   100,    75,   124,
      28,    69,    32,    85,    32,    32,    80,    80,    66,    28,
      41,   125,    28,    66,    75,    36,   121,    53,    14,    43,
      52,    53,    55,    84,    86,    28,    32,    80,    47,    62,
      66,    70,    60,    88,    33,    88,    68,    76,    80,    80,
      80,    80,    90,     5,    87,    87,    86
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
#line 197 "gram.y"
    {
					plpsm_parser_tree = (yyvsp[(1) - (2)].stmt);
					if (plpsm_debug_parser)
						elog_stmt(NOTICE, (yyvsp[(1) - (2)].stmt));
				;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 206 "gram.y"
    {
				;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 209 "gram.y"
    {
				;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 215 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 219 "gram.y"
    {
					(yyvsp[(1) - (3)].stmt)->last->next = (yyvsp[(3) - (3)].stmt);
					(yyvsp[(1) - (3)].stmt)->last = (yyvsp[(3) - (3)].stmt)->last;
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 230 "gram.y"
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
#line 246 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 247 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 248 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 249 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 250 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 251 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 252 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 253 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 254 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 255 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 256 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 257 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 258 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 259 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 260 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 261 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 262 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 263 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 264 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 265 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 282 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yylsp[(2) - (6)]));
					new->name = (yyvsp[(1) - (6)].str);
					new->inner_left = (yyvsp[(3) - (6)].stmt);
					check_labels((yyvsp[(1) - (6)].str), (yyvsp[(6) - (6)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 290 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yylsp[(2) - (6)]));
					new->name = (yyvsp[(1) - (6)].str);
					new->inner_left = (yyvsp[(3) - (6)].stmt);
					check_labels((yyvsp[(1) - (6)].str), (yyvsp[(6) - (6)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 298 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					/* join declarations and statements */
					(yyvsp[(3) - (8)].stmt)->last->next = (yyvsp[(5) - (8)].stmt);
					(yyvsp[(3) - (8)].stmt)->last = (yyvsp[(5) - (8)].stmt)->last;
					new->inner_left = (yyvsp[(3) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 309 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yylsp[(2) - (4)]));
					new->name = (yyvsp[(1) - (4)].str);
					check_labels((yyvsp[(1) - (4)].str), (yyvsp[(4) - (4)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 319 "gram.y"
    {
					(yyval.str) = (yyvsp[(1) - (2)].word).ident;
				;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 323 "gram.y"
    {
					(yyval.str) = NULL;
				;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 330 "gram.y"
    {
					(yyval.str) = (yyvsp[(1) - (1)].word).ident;
				;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 334 "gram.y"
    {
					(yyval.str) = NULL;
				;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 341 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
					(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 346 "gram.y"
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

  case 38:

/* Line 1455 of yacc.c  */
#line 382 "gram.y"
    {
					DEBUG_INIT;
					if ((yyvsp[(2) - (2)].stmt)->typ != PLPSM_STMT_DECLARE_VARIABLE)
						yyerror("syntax error");
					(yyval.stmt) = (yyvsp[(2) - (2)].stmt);
					DEBUG_SET((yyvsp[(2) - (2)].stmt), (yylsp[(1) - (2)]));
				;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 390 "gram.y"
    {
					DEBUG_INIT;
					if ((yyvsp[(2) - (4)].stmt)->typ != PLPSM_STMT_DECLARE_CONDITION)
						yyerror("syntax error");
					(yyvsp[(2) - (4)].stmt)->data = (yyvsp[(4) - (4)].node);
					(yyval.stmt) = (yyvsp[(2) - (4)].stmt);
					DEBUG_SET((yyvsp[(2) - (4)].stmt), (yylsp[(1) - (4)]));
				;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 399 "gram.y"
    {
					int	tok;
					DEBUG_INIT;
					if ((yyvsp[(2) - (4)].stmt)->typ != PLPSM_STMT_DECLARE_CURSOR)
						yyerror("syntax error");
					if ((tok = yylex()) != WORD)
					{
						plpsm_push_back_token(tok);
						(yyvsp[(2) - (4)].stmt)->query = read_until(';', 0, 0, ";", false, false, NULL, -1);
					}
					else
						(yyvsp[(2) - (4)].stmt)->name = yylval.word.ident;
					(yyval.stmt) = (yyvsp[(2) - (4)].stmt);
					DEBUG_SET((yyvsp[(2) - (4)].stmt), (yylsp[(1) - (4)]));
				;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 415 "gram.y"
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

  case 42:

/* Line 1455 of yacc.c  */
#line 427 "gram.y"
    {
					(yyval.stmt) = declare_prefetch();
				;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 434 "gram.y"
    {
					(yyval.list) = list_make1((yyvsp[(1) - (1)].node));
				;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 438 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].node));
				;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 445 "gram.y"
    {
					(yyval.node) = (yyvsp[(4) - (4)].node);
				;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 448 "gram.y"
    {
					(yyval.node) = NULL;
				;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 455 "gram.y"
    {
					(yyval.node) = (Node *) list_make2(
						makeString(pstrdup("IN")), (yyvsp[(3) - (3)].node));
				;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 460 "gram.y"
    {
					(yyval.node) = (Node *) list_make2(
						makeString(pstrdup("IN")),
						makeInteger(MAKE_SQLSTATE('0','2','0','0','0')));
				;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 466 "gram.y"
    {
					(yyval.node) = (Node *) list_make3(
						makeString(pstrdup("IN")),
						makeInteger(MAKE_SQLSTATE('0','1','0','0','0')),
						makeInteger(MAKE_SQLSTATE('0','2','0','0','0')));
				;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 473 "gram.y"
    {
					(yyval.node) = (Node *) list_make3(
						makeString(pstrdup("NOT_IN")),
						makeInteger(MAKE_SQLSTATE('0','1','0','0','0')),
						makeInteger(MAKE_SQLSTATE('0','2','0','0','0')));
				;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 480 "gram.y"
    {
					(yyval.node) = (Node *) makeString((yyvsp[(1) - (1)].word).ident);
				;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 486 "gram.y"
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

  case 53:

/* Line 1455 of yacc.c  */
#line 506 "gram.y"
    {
				;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 508 "gram.y"
    {
				;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 518 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_IF, (yylsp[(1) - (7)]));
					new->expr = (yyvsp[(2) - (7)].str);
					new->inner_left = (yyvsp[(3) - (7)].stmt);
					new->inner_right = (yyvsp[(5) - (7)].stmt);
					(yyval.stmt) = new;
				;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 528 "gram.y"
    {
					(yyval.stmt) = NULL;
				;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 532 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_IF, (yylsp[(1) - (5)]));
					DEBUG_INIT;
					new->expr = (yyvsp[(2) - (5)].str);
					new->inner_left = (yyvsp[(3) - (5)].stmt);
					new->inner_right = (yyvsp[(5) - (5)].stmt);
					(yyval.stmt) = new;
					DEBUG_SET(new, (yylsp[(1) - (5)]));
				;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 542 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (3)].stmt);
				;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 554 "gram.y"
    {
						Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CASE, (yylsp[(1) - (6)]));
						new->expr = (yyvsp[(2) - (6)].str);
						new->inner_left = (yyvsp[(3) - (6)].stmt);
						new->inner_right = (yyvsp[(4) - (6)].stmt);
						(yyval.stmt) = new;
					;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 564 "gram.y"
    {
					char *expr = NULL;
					int	tok = yylex();
					StringInfoData	ds;

					if (tok != WHEN)
					{
						plpsm_push_back_token(tok);
						initStringInfo(&ds);
						expr = read_until(WHEN, -1, -1, "WHEN", true, false, NULL, -1);
						appendStringInfo(&ds, "SELECT (%s)", expr);
						check_sql_expr(ds.data);
						pfree(ds.data);
						(yyval.str) = expr;
					}
					plpsm_push_back_token(WHEN);
					(yyval.str) = expr;
				;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 586 "gram.y"
    {
					(yyvsp[(1) - (2)].stmt)->last->next = (yyvsp[(2) - (2)].stmt);
					(yyvsp[(1) - (2)].stmt)->last = (yyvsp[(2) - (2)].stmt);
				;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 591 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 597 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CASE, (yylsp[(1) - (4)]));
					new->expr = (yyvsp[(2) - (4)].str);
					new->inner_left = (yyvsp[(3) - (4)].stmt);
					new->last = new;
					(yyval.stmt) = new;
				;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 607 "gram.y"
    {
					(yyval.stmt) = NULL;
				;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 611 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (3)].stmt);
				;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 627 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (2)].stmt);
				;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 631 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SET, (yylsp[(1) - (8)]));
					new->compound_target = (yyvsp[(3) - (8)].list);
					new->expr_list = (yyvsp[(7) - (8)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 641 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
					(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 646 "gram.y"
    {
					/* expected just very short list */
					(yyvsp[(1) - (3)].stmt)->last->next = (yyvsp[(3) - (3)].stmt);
					(yyvsp[(1) - (3)].stmt)->last = (yyvsp[(3) - (3)].stmt);
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 654 "gram.y"
    {
					(yyvsp[(1) - (3)].stmt)->typ = PLPSM_STMT_SET;
					(yyvsp[(1) - (3)].stmt)->expr = (yyvsp[(3) - (3)].str);
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 663 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_UNKNOWN, (yylsp[(1) - (1)]));
					new->target = (yyvsp[(1) - (1)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 672 "gram.y"
    {
					(yyval.list) = list_make1((yyvsp[(1) - (1)].list));
				;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 676 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].list));
				;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 683 "gram.y"
    {
					(yyval.list) = list_make1(makeString((yyvsp[(1) - (1)].word).ident));
				;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 687 "gram.y"
    {
					(yyval.list) = (yyvsp[(1) - (1)].cword).idents;
				;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 691 "gram.y"
    {
					/* ToDo: Plpsm_object should be a param type too */
					char buf[32];
					snprintf(buf, sizeof(buf), "$%d", (yyvsp[(1) - (1)].ival));
					(yyval.list) = list_make1(makeString(pstrdup(buf)));
				;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 707 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PRINT, (yylsp[(1) - (2)]));
					new->compound_target = (yyvsp[(2) - (2)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 722 "gram.y"
    {
					int tok = yylex();
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RETURN, (yylsp[(1) - (1)]));
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

  case 79:

/* Line 1455 of yacc.c  */
#line 745 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_LOOP, (yyvsp[(1) - (7)].str) ? (yylsp[(1) - (7)]) : (yylsp[(2) - (7)]));
					new->name = (yyvsp[(1) - (7)].str);
					new->inner_left = (yyvsp[(3) - (7)].stmt);
					check_labels((yyvsp[(1) - (7)].str), (yyvsp[(7) - (7)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 761 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_WHILE, (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					new->expr = (yyvsp[(3) - (8)].str);
					new->inner_left = (yyvsp[(4) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 778 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_REPEAT_UNTIL, (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					new->expr = (yyvsp[(6) - (8)].str);
					new->inner_left = (yyvsp[(3) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 795 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_ITERATE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 810 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_LEAVE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 825 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PREPARE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->expr = (yyvsp[(4) - (4)].str);
					(yyval.stmt) = new;
				;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 842 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 848 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->compound_target = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 855 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (6)]));
					new->name = (yyvsp[(2) - (6)].word).ident;
					new->compound_target = (yyvsp[(4) - (6)].list);
					new->var_list = (yyvsp[(6) - (6)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 863 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->var_list = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 879 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE_IMMEDIATE, (yylsp[(1) - (3)]));
					new->expr = (yyvsp[(3) - (3)].str);
					(yyval.stmt) = new;
				;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 895 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_OPEN, (yylsp[(1) - (2)]));
					new->target = (yyvsp[(2) - (2)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 901 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_OPEN, (yylsp[(1) - (4)]));
					new->target = (yyvsp[(2) - (4)].list);
					new->var_list = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 916 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_FETCH, (yylsp[(1) - (4)]));
					new->target = (yyvsp[(2) - (4)].list);
					new->compound_target = (yyvsp[(4) - (4)].list);
					new->data = (yyvsp[(2) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 932 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CLOSE, (yylsp[(1) - (2)]));
					new->target = (yyvsp[(2) - (2)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 947 "gram.y"
    {
					Plpsm_stmt *new = (yyvsp[(3) - (8)].stmt);
					new->location = (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]);
					new->name = (yyvsp[(1) - (8)].str);
					new->inner_left = (yyvsp[(4) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 962 "gram.y"
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
					new->query = read_until(DO, -1, -1, "DO", false, false, NULL, startloc);
					(yyval.stmt) = new;
				;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 1030 "gram.y"
    { (yyval.stmt) = make_stmt_sql("INSERT", (yylsp[(1) - (1)])); ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 1031 "gram.y"
    { (yyval.stmt) = make_stmt_sql("UPDATE", (yylsp[(1) - (1)])); ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 1032 "gram.y"
    { (yyval.stmt) = make_stmt_sql("DELETE", (yylsp[(1) - (1)])); ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 1037 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SELECT_INTO, (yylsp[(1) - (3)]));
					new->expr_list = (yyvsp[(2) - (3)].list);
					new->compound_target = (yyvsp[(3) - (3)].list);
					if (list_length((yyvsp[(2) - (3)].list)) != list_length((yyvsp[(3) - (3)].list)))
						elog(ERROR, "number of target variables is different than number of attributies");

					(yyval.stmt) = new;
				;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 1047 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SELECT_INTO, (yylsp[(1) - (4)]));
					new->expr_list = (yyvsp[(2) - (4)].list);
					new->compound_target = (yyvsp[(3) - (4)].list);
					if (list_length((yyvsp[(2) - (4)].list)) != list_length((yyvsp[(3) - (4)].list)))
						elog(ERROR, "number of target variables is different than number of attributies");

					new->from_clause = read_until(';', 0, 0, ";", false, false, NULL, (yylsp[(4) - (4)]));
					(yyval.stmt) = new;
				;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 1060 "gram.y"
    {
					int endtok;
					List	*expr_list = NIL;

					do
					{
						char *expr;
						StringInfoData	ds;
						expr = read_until(',', INTO, 0, ", or INTO", true, false, &endtok, -1);
						initStringInfo(&ds);
						appendStringInfo(&ds, "SELECT (%s)", expr);
						check_sql_expr(ds.data);
						pfree(ds.data);
						if (endtok == ',')
							yylex();

						expr_list = lappend(expr_list, makeString(expr));
					} while (endtok != INTO);
					(yyval.list) = expr_list;
				;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 1083 "gram.y"
    {
					(yyval.str) = read_expr_until_semi();
				;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 1090 "gram.y"
    {
					(yyval.list) = list_make1(makeString((yyvsp[(1) - (1)].str)));
				;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 1094 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), makeString((yyvsp[(3) - (3)].str)));
				;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 1100 "gram.y"
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

  case 106:

/* Line 1455 of yacc.c  */
#line 1115 "gram.y"
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

  case 107:

/* Line 1455 of yacc.c  */
#line 1129 "gram.y"
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

  case 108:

/* Line 1455 of yacc.c  */
#line 1143 "gram.y"
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

  case 109:

/* Line 1455 of yacc.c  */
#line 1157 "gram.y"
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

  case 110:

/* Line 1455 of yacc.c  */
#line 1174 "gram.y"
    {
					StringInfoData		ds;
					char *expr;
					int	endtok;

					expr = read_until(';',',', ')', "; or , or )", true, false, &endtok, -1);
					initStringInfo(&ds);
					appendStringInfo(&ds, "SELECT (%s)", expr);
					check_sql_expr(ds.data);
					pfree(ds.data);
					if (endtok != ';' && endtok != ',')
						plpsm_push_back_token(endtok);
					(yyval.str) = expr;
				;}
    break;



/* Line 1455 of yacc.c  */
#line 3130 "gram.c"
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
#line 1190 "gram.y"


static Plpsm_stmt *
make_stmt_sql(const char *prefix, int location)
{
	StringInfoData ds;
	char *expr;
	Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SQL, location);

	initStringInfo(&ds);
	expr = read_until(';', 0, 0, ";", false, false, NULL, -1);
	appendStringInfo(&ds, "%s %s", prefix, expr);
	pfree(expr);
	new->query = ds.data;
	return new;
}

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

			/* 
			 * ToDo: better to use a special function than read_until,
			 * because it raise a error to late. Datatype must not contains
			 * a keywords, special chars etc
			 */
			datatype = read_until(';', DEFAULT, 0, "; or \"DEFAULT\"", false, true, &endtok, startlocation);
			parse_datatype(datatype, &type_id, &typmod);
			get_typlenbyval(type_id, &typlen, &typbyval);

			result->datum.typoid = type_id;
			result->datum.typmod = typmod;
			result->datum.typname = datatype;
			result->datum.typlen = typlen;
			result->datum.typbyval = typbyval;

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
			result->target = varnames;
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
			else if (is_unreserved_keyword(tok))
			{
				varnames = list_make1(makeString(yylval.word.ident));
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
		case SQLCODE:
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
		case PLPSM_STMT_CASE:
			return "case statement";
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
	appendStringInfo(ds, "%s| ExprList: %s\n", ident, nodeToString(stmt->expr_list));
	
	switch (stmt->typ)
	{
		case PLPSM_STMT_FOR:
			{
				appendStringInfo(ds, "%s| Data: loopvar:%s, cursor:%s\n", ident,
										stmt->stmtfor.loopvar_name,
										stmt->stmtfor.cursor_name);
			}
			break;
		default:
			/* do nothing */ ;
	}
	
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

