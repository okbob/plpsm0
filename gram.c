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



/* Line 189 of yacc.c  */
#line 147 "gram.c"

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
     CONTINUE = 277,
     CURSOR = 278,
     DECLARE = 279,
     DEFAULT = 280,
     DETAIL = 281,
     DO = 282,
     ELSE = 283,
     ELSEIF = 284,
     END = 285,
     EXECUTE = 286,
     EXIT = 287,
     FETCH = 288,
     FOR = 289,
     FOUND = 290,
     FROM = 291,
     HANDLER = 292,
     HINT = 293,
     IF = 294,
     IMMEDIATE = 295,
     INTO = 296,
     ITERATE = 297,
     LEAVE = 298,
     LOOP = 299,
     MESSAGE_TEXT = 300,
     NO = 301,
     NOT = 302,
     OPEN = 303,
     PREPARE = 304,
     PRINT = 305,
     REPEAT = 306,
     RESIGNAL = 307,
     RETURN = 308,
     SCROLL = 309,
     SELECT = 310,
     SET = 311,
     SIGNAL = 312,
     SQLEXCEPTION = 313,
     SQLSTATE = 314,
     SQLCODE = 315,
     SQLWARNING = 316,
     THEN = 317,
     UNDO = 318,
     UNTIL = 319,
     USING = 320,
     VALUE = 321,
     WHEN = 322,
     WHILE = 323,
     INSERT = 324,
     UPDATE = 325,
     DELETE = 326,
     TRUNCATE = 327
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 71 "gram.y"

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
		Node		*node;



/* Line 214 of yacc.c  */
#line 277 "gram.c"
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
#line 302 "gram.c"

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
#define YYFINAL  73
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   775

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  78
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  61
/* YYNRULES -- Number of rules.  */
#define YYNRULES  130
/* YYNRULES -- Number of states.  */
#define YYNSTATES  233

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   327

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      75,    76,     2,     2,    74,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    73,
       2,    77,     2,     2,     2,     2,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71,    72
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     8,     9,    11,    15,    17,    19,
      21,    23,    25,    27,    29,    31,    33,    35,    37,    39,
      41,    43,    45,    47,    49,    51,    53,    55,    57,    59,
      61,    69,    77,    87,    92,    93,    95,    97,    98,   100,
     101,   103,   107,   110,   115,   120,   127,   128,   130,   134,
     139,   140,   144,   147,   149,   151,   153,   155,   157,   158,
     166,   167,   173,   177,   184,   187,   189,   194,   195,   199,
     202,   211,   213,   217,   221,   223,   225,   229,   231,   233,
     235,   238,   240,   248,   257,   266,   269,   272,   277,   280,
     287,   292,   294,   299,   302,   309,   314,   318,   320,   324,
     328,   332,   336,   341,   344,   349,   356,   361,   365,   368,
     373,   378,   381,   390,   391,   393,   395,   397,   399,   403,
     408,   409,   410,   412,   416,   417,   418,   419,   420,   421,
     422
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      79,     0,    -1,    82,    80,    -1,    73,    -1,    -1,    82,
      -1,    81,    73,    82,    -1,    83,    -1,    84,    -1,   102,
      -1,   108,    -1,   110,    -1,   111,    -1,   112,    -1,   113,
      -1,   114,    -1,   109,    -1,   119,    -1,   120,    -1,   121,
      -1,   122,    -1,   123,    -1,   124,    -1,   125,    -1,    96,
      -1,    98,    -1,   127,    -1,   128,    -1,   115,    -1,   116,
      -1,    86,    18,    85,    81,    73,    30,    87,    -1,    86,
      18,    85,    88,    73,    30,    87,    -1,    86,    18,    85,
      88,    73,    81,    73,    30,    87,    -1,    86,    18,    30,
      87,    -1,    -1,    16,    -1,    14,    -1,    -1,    14,    -1,
      -1,    89,    -1,    88,    73,    89,    -1,    24,    90,    -1,
      24,    90,    21,    92,    -1,    24,    90,    23,    34,    -1,
      24,    90,    37,    34,    91,    82,    -1,    -1,    93,    -1,
      91,    74,    93,    -1,    34,    59,    95,    94,    -1,    -1,
      59,    95,    94,    -1,    47,    35,    -1,    61,    -1,    58,
      -1,    14,    -1,     5,    -1,    66,    -1,    -1,    39,   134,
      81,    73,    97,    30,    39,    -1,    -1,    29,   134,    81,
      73,    97,    -1,    28,    81,    73,    -1,    19,   138,    99,
     101,    30,    19,    -1,    99,   100,    -1,   100,    -1,    67,
     134,    81,    73,    -1,    -1,    28,    81,    73,    -1,    56,
     103,    -1,    56,    75,   106,    76,    77,    75,   131,    76,
      -1,   104,    -1,   103,    74,   104,    -1,   105,    77,   132,
      -1,   107,    -1,   107,    -1,   106,    74,   107,    -1,    14,
      -1,    15,    -1,    10,    -1,    50,   131,    -1,    53,    -1,
      86,    44,    81,    73,    30,    44,    87,    -1,    86,    68,
     133,    81,    73,    30,    68,    87,    -1,    86,    51,    81,
      73,    64,   135,    51,    87,    -1,    42,    14,    -1,    43,
      14,    -1,    57,    59,    95,    94,    -1,    57,    14,    -1,
      57,    59,    95,    94,    56,   117,    -1,    57,    14,    56,
     117,    -1,    52,    -1,    52,    59,    95,    94,    -1,    52,
      14,    -1,    52,    59,    95,    94,    56,   117,    -1,    52,
      14,    56,   117,    -1,    52,    56,   117,    -1,   118,    -1,
     117,    74,   118,    -1,    26,    77,     5,    -1,    38,    77,
       5,    -1,    45,    77,     5,    -1,    49,    14,    36,   130,
      -1,    31,    14,    -1,    31,    14,    41,   106,    -1,    31,
      14,    41,   106,    65,   106,    -1,    31,    14,    65,   106,
      -1,    31,    40,   136,    -1,    48,   107,    -1,    48,   107,
      65,   106,    -1,    33,   107,    41,   106,    -1,    20,   107,
      -1,    86,    34,   126,    81,    73,    30,    34,    87,    -1,
      -1,    69,    -1,    70,    -1,    71,    -1,    72,    -1,    55,
     129,   106,    -1,    55,   129,   106,    36,    -1,    -1,    -1,
     137,    -1,   131,    74,   137,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   194,   194,   203,   207,   212,   216,   227,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     271,   280,   289,   301,   311,   314,   325,   335,   341,   346,
     352,   357,   393,   400,   408,   425,   437,   443,   447,   455,
     462,   468,   476,   484,   491,   498,   509,   531,   534,   544,
     555,   558,   566,   579,   590,   595,   602,   613,   616,   632,
     637,   647,   652,   660,   669,   678,   682,   689,   693,   703,
     720,   735,   759,   776,   794,   812,   828,   843,   849,   855,
     862,   877,   882,   888,   894,   901,   908,   917,   922,   944,
     951,   958,   973,   990,   996,  1003,  1011,  1027,  1043,  1049,
    1065,  1082,  1097,  1114,  1182,  1183,  1184,  1185,  1189,  1202,
    1221,  1241,  1247,  1251,  1258,  1268,  1278,  1288,  1298,  1311,
    1325
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
  "CONTINUE", "CURSOR", "DECLARE", "DEFAULT", "DETAIL", "DO", "ELSE",
  "ELSEIF", "END", "EXECUTE", "EXIT", "FETCH", "FOR", "FOUND", "FROM",
  "HANDLER", "HINT", "IF", "IMMEDIATE", "INTO", "ITERATE", "LEAVE", "LOOP",
  "MESSAGE_TEXT", "NO", "NOT", "OPEN", "PREPARE", "PRINT", "REPEAT",
  "RESIGNAL", "RETURN", "SCROLL", "SELECT", "SET", "SIGNAL",
  "SQLEXCEPTION", "SQLSTATE", "SQLCODE", "SQLWARNING", "THEN", "UNDO",
  "UNTIL", "USING", "VALUE", "WHEN", "WHILE", "INSERT", "UPDATE", "DELETE",
  "TRUNCATE", "';'", "','", "'('", "')'", "'='", "$accept", "function",
  "opt_semi", "statements", "dstmt", "stmt", "stmt_compound", "opt_atomic",
  "opt_label", "opt_end_label", "declarations", "declaration",
  "declare_prefetch", "condition_list", "opt_sqlstate", "condition",
  "sqlstate", "opt_value", "stmt_if", "stmt_else", "stmt_case",
  "case_when_list", "case_when", "opt_case_else", "stmt_set",
  "assign_list", "assign_item", "target", "qual_identif_list",
  "qual_identif", "stmt_print", "stmt_return", "stmt_loop", "stmt_while",
  "stmt_repeat_until", "stmt_iterate", "stmt_leave", "stmt_signal",
  "stmt_resignal", "signal_info", "signal_info_item", "stmt_prepare",
  "stmt_execute", "stmt_execute_immediate", "stmt_open", "stmt_fetch",
  "stmt_close", "stmt_for", "for_prefetch", "stmt_sql", "stmt_select_into",
  "expr_list_into", "expr_until_semi", "expr_list",
  "expr_until_semi_or_coma", "expr_until_do", "expr_until_then",
  "expr_until_end", "expr_until_semi_into_using",
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
     325,   326,   327,    59,    44,    40,    41,    61
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    78,    79,    80,    80,    81,    81,    82,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      84,    84,    84,    84,    85,    85,    86,    86,    87,    87,
      88,    88,    89,    89,    89,    89,    90,    91,    91,    92,
      92,    93,    93,    93,    93,    93,    94,    95,    95,    96,
      97,    97,    97,    98,    99,    99,   100,   101,   101,   102,
     102,   103,   103,   104,   105,   106,   106,   107,   107,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   115,   115,
     115,   116,   116,   116,   116,   116,   116,   117,   117,   118,
     118,   118,   119,   120,   120,   120,   120,   121,   122,   122,
     123,   124,   125,   126,   127,   127,   127,   127,   128,   128,
     129,   130,   131,   131,   132,   133,   134,   135,   136,   137,
     138
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     1,     0,     1,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       7,     7,     9,     4,     0,     1,     1,     0,     1,     0,
       1,     3,     2,     4,     4,     6,     0,     1,     3,     4,
       0,     3,     2,     1,     1,     1,     1,     1,     0,     7,
       0,     5,     3,     6,     2,     1,     4,     0,     3,     2,
       8,     1,     3,     3,     1,     1,     3,     1,     1,     1,
       2,     1,     7,     8,     8,     2,     2,     4,     2,     6,
       4,     1,     4,     2,     6,     4,     3,     1,     3,     3,
       3,     3,     4,     2,     4,     6,     4,     3,     2,     4,
       4,     2,     8,     0,     1,     1,     1,     1,     3,     4,
       0,     0,     1,     3,     0,     0,     0,     0,     0,     0,
       0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      37,    36,   130,     0,     0,     0,   126,     0,     0,     0,
       0,   129,    91,    81,   120,     0,     0,   114,   115,   116,
     117,     0,     4,     7,     8,     0,    24,    25,     9,    10,
      16,    11,    12,    13,    14,    15,    28,    29,    17,    18,
      19,    20,    21,    22,    23,    26,    27,     0,    79,    77,
      78,   111,   103,   128,     0,    37,    85,    86,   108,     0,
      80,   122,    93,     0,    58,     0,     0,    69,    71,     0,
      74,    88,    58,     1,     3,     2,    34,   113,    37,    37,
     125,   126,    67,    65,     0,     0,   107,     0,     0,     5,
       0,   121,   129,     0,     0,     0,     0,    96,    97,    57,
       0,   118,    75,     0,     0,   124,     0,     0,    35,    39,
      37,    37,     0,     0,    37,    37,    37,    64,     0,   104,
     106,   110,    37,   109,   102,   123,    95,     0,     0,     0,
       0,    56,    92,   119,     0,     0,    72,    73,    90,    87,
      38,    33,    46,     0,     0,    40,     0,    37,    37,     0,
       0,     0,     0,     0,    37,   126,     6,     0,    99,   100,
     101,    98,     0,    76,     0,     0,    42,    37,    37,    37,
       0,   127,    37,    37,    37,    63,   105,     0,    37,     0,
      94,   129,    89,    50,     0,     0,    39,    39,     0,    41,
       0,    39,     0,     0,    37,     0,    59,     0,     0,    43,
      44,     0,    30,    31,    37,    39,    82,    39,    39,    37,
      70,    58,    55,     0,    54,    58,    53,    37,    47,    39,
     112,    84,    83,    61,     0,    52,     0,     0,    45,    32,
      49,    51,    48
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    21,    75,    88,    89,    23,    24,   110,    25,   141,
     144,   145,   166,   217,   199,   218,   132,   100,    26,   157,
      27,    82,    83,   118,    28,    67,    68,    69,   101,   102,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    97,
      98,    38,    39,    40,    41,    42,    43,    44,   111,    45,
      46,    65,   124,    60,   137,   114,    55,   192,    86,    61,
      47
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -102
static const yytype_int16 yypact[] =
{
     703,  -102,  -102,    24,     4,    24,  -102,    18,    29,    24,
      37,  -102,     5,  -102,  -102,     1,     3,  -102,  -102,  -102,
    -102,    55,   -10,  -102,  -102,   -11,  -102,  -102,  -102,  -102,
    -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,
    -102,  -102,  -102,  -102,  -102,  -102,  -102,     8,  -102,  -102,
    -102,  -102,   -12,  -102,    30,   703,  -102,  -102,    12,    42,
       6,  -102,    27,    66,    34,    24,    24,    21,  -102,    33,
    -102,    45,    34,  -102,  -102,  -102,    38,  -102,   703,   703,
    -102,  -102,   -15,  -102,    24,    24,  -102,    24,    39,  -102,
      24,  -102,  -102,    66,    36,    40,    43,    41,  -102,  -102,
     109,   -16,  -102,   -46,    24,  -102,    66,   109,  -102,   110,
     305,   703,    53,    54,   703,   703,   703,  -102,    98,   -39,
      57,    57,   172,    57,  -102,  -102,    41,   124,   127,   129,
      66,  -102,    79,  -102,    24,    59,  -102,  -102,    41,    81,
    -102,  -102,  -102,    65,    68,  -102,    69,   349,   393,    70,
      72,    73,   120,    24,   703,  -102,  -102,   119,  -102,  -102,
    -102,  -102,    66,  -102,    75,    66,    84,   439,   261,   483,
     107,  -102,   527,   217,   571,  -102,    57,    80,   703,   113,
      41,  -102,    41,   121,   122,   123,   110,   110,    85,  -102,
     125,   110,   103,    92,   615,    88,  -102,    32,   104,  -102,
    -102,    35,  -102,  -102,   659,   110,  -102,   110,   110,   172,
    -102,    34,  -102,   130,  -102,    34,  -102,    17,  -102,   110,
    -102,  -102,  -102,  -102,   109,  -102,   109,    35,  -102,  -102,
    -102,  -102,  -102
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -102,  -102,  -102,   -69,     0,  -102,  -102,  -102,  -102,   -89,
    -102,    -6,  -102,  -102,  -102,   -61,  -101,   -71,  -102,   -45,
    -102,  -102,    86,  -102,  -102,  -102,    67,  -102,   -63,    -1,
    -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,   -81,
      46,  -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,
    -102,  -102,  -102,    -4,  -102,  -102,   -76,  -102,  -102,    78,
    -102
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -69
static const yytype_int16 yytable[] =
{
      22,   107,    51,   103,    54,   115,   139,    76,    58,   112,
     113,    48,   126,   116,    70,    49,    50,    71,    52,    62,
     133,   119,   120,    77,   121,   138,   153,   123,   134,    84,
     135,     1,    56,    78,    48,   134,     2,     3,    49,    50,
      79,   143,   146,    57,    53,   149,   150,   151,     4,   212,
       5,    59,    81,    85,   108,    73,     6,    80,   134,     7,
       8,    63,    72,    74,    64,     9,    10,    11,   109,    12,
      13,    87,    14,    15,    16,    81,    66,    90,    91,   178,
      92,   180,   213,    93,   182,   177,    17,    18,    19,    20,
     176,   227,    94,   214,   215,   104,   216,   202,   203,   188,
      99,   106,   206,    70,    95,   183,    92,   184,   210,   195,
     105,    96,   122,   127,   131,   130,   220,   128,   221,   222,
     129,   185,   156,   230,   140,   231,   147,   148,   152,   158,
     229,   134,   159,   163,   160,   162,   164,   165,   167,   175,
     224,   168,   169,   172,   226,   173,   174,   156,   156,   179,
     181,   191,   196,   194,   207,   198,   200,   201,   204,   205,
     208,   209,   189,   211,   223,   225,   232,   156,   117,   156,
     125,   136,   156,   156,   156,     0,   161,   197,     0,     0,
       0,     0,     0,     0,     0,     0,     1,     0,     0,     0,
       0,     2,     3,     0,   156,     0,     0,     0,     0,     0,
     154,   155,   -60,     4,   156,     5,     0,     0,     0,   156,
       0,     6,     0,     0,     7,     8,     0,   228,     0,     0,
       9,    10,    11,     0,    12,    13,     0,    14,    15,    16,
       0,     1,     0,     0,     0,     0,     2,     3,     0,     0,
       0,    17,    18,    19,    20,   -66,     0,   -66,     4,     0,
       5,     0,     0,     0,     0,     0,     6,     0,     0,     7,
       8,     0,     0,     0,     0,     9,    10,    11,     0,    12,
      13,     0,    14,    15,    16,     1,     0,     0,     0,     0,
       2,     3,     0,     0,   -66,   142,    17,    18,    19,    20,
       0,   187,     4,     0,     5,     0,     0,     0,     0,     0,
       6,     0,     0,     7,     8,     0,     0,     0,     0,     9,
      10,    11,     0,    12,    13,     0,    14,    15,    16,     1,
       0,     0,     0,     0,     2,     3,     0,     0,     0,   142,
      17,    18,    19,    20,     0,     0,     4,     0,     5,     0,
       0,     0,     0,     0,     6,     0,     0,     7,     8,     0,
       0,     0,     0,     9,    10,    11,     0,    12,    13,     0,
      14,    15,    16,     1,     0,     0,     0,     0,     2,     3,
       0,     0,     0,     0,    17,    18,    19,    20,     0,   170,
       4,     0,     5,     0,     0,     0,     0,     0,     6,     0,
       0,     7,     8,     0,     0,     0,     0,     9,    10,    11,
       0,    12,    13,     0,    14,    15,    16,     1,     0,     0,
       0,     0,     2,     3,     0,     0,     0,     0,    17,    18,
      19,    20,     0,     0,     4,     0,     5,     0,     0,     0,
       0,     0,     6,     0,     0,     7,     8,     0,     0,     0,
       0,     9,    10,    11,     0,    12,    13,     0,    14,    15,
      16,     0,     0,     1,     0,     0,     0,   171,     2,     3,
       0,     0,    17,    18,    19,    20,     0,     0,     0,   186,
       4,     0,     5,     0,     0,     0,     0,     0,     6,     0,
       0,     7,     8,     0,     0,     0,     0,     9,    10,    11,
       0,    12,    13,     0,    14,    15,    16,     1,     0,     0,
       0,     0,     2,     3,     0,     0,     0,     0,    17,    18,
      19,    20,     0,   190,     4,     0,     5,     0,     0,     0,
       0,     0,     6,     0,     0,     7,     8,     0,     0,     0,
       0,     9,    10,    11,     0,    12,    13,     0,    14,    15,
      16,     1,     0,     0,     0,     0,     2,     3,     0,     0,
       0,     0,    17,    18,    19,    20,     0,   193,     4,     0,
       5,     0,     0,     0,     0,     0,     6,     0,     0,     7,
       8,     0,     0,     0,     0,     9,    10,    11,     0,    12,
      13,     0,    14,    15,    16,     1,     0,     0,     0,     0,
       2,     3,     0,     0,     0,     0,    17,    18,    19,    20,
       0,   -68,     4,     0,     5,     0,     0,     0,     0,     0,
       6,     0,     0,     7,     8,     0,     0,     0,     0,     9,
      10,    11,     0,    12,    13,     0,    14,    15,    16,     1,
       0,     0,     0,     0,     2,     3,     0,     0,     0,     0,
      17,    18,    19,    20,     0,   -62,     4,     0,     5,     0,
       0,     0,     0,     0,     6,     0,     0,     7,     8,     0,
       0,     0,     0,     9,    10,    11,     0,    12,    13,     0,
      14,    15,    16,     1,     0,     0,     0,     0,     2,     3,
       0,     0,     0,     0,    17,    18,    19,    20,     0,   219,
       4,     0,     5,     0,     0,     0,     0,     0,     6,     0,
       0,     7,     8,     0,     0,     0,     0,     9,    10,    11,
       0,    12,    13,     0,    14,    15,    16,     1,     0,     0,
       0,     0,     2,     3,     0,     0,     0,     0,    17,    18,
      19,    20,     0,     0,     4,     0,     5,     0,     0,     0,
       0,     0,     6,     0,     0,     7,     8,     0,     0,     0,
       0,     9,    10,    11,     0,    12,    13,     0,    14,    15,
      16,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,    18,    19,    20
};

static const yytype_int16 yycheck[] =
{
       0,    72,     3,    66,     5,    81,   107,    18,     9,    78,
      79,    10,    93,    28,    15,    14,    15,    14,    14,    14,
      36,    84,    85,    34,    87,   106,    65,    90,    74,    41,
      76,    14,    14,    44,    10,    74,    19,    20,    14,    15,
      51,   110,   111,    14,    40,   114,   115,   116,    31,    14,
      33,    14,    67,    65,    16,     0,    39,    68,    74,    42,
      43,    56,    59,    73,    59,    48,    49,    50,    30,    52,
      53,    41,    55,    56,    57,    67,    75,    65,    36,   155,
      74,   162,    47,    56,   165,   154,    69,    70,    71,    72,
     153,    74,    26,    58,    59,    74,    61,   186,   187,   168,
      66,    56,   191,   104,    38,    21,    74,    23,    76,   178,
      77,    45,    73,    77,     5,    74,   205,    77,   207,   208,
      77,    37,   122,   224,    14,   226,    73,    73,    30,     5,
     219,    74,     5,   134,     5,    56,    77,    56,    73,    19,
     211,    73,    73,    73,   215,    73,    73,   147,   148,    30,
      75,    44,    39,    73,    51,    34,    34,    34,    73,    34,
      68,    73,   168,    59,   209,    35,   227,   167,    82,   169,
      92,   104,   172,   173,   174,    -1,   130,   181,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    14,    -1,    -1,    -1,
      -1,    19,    20,    -1,   194,    -1,    -1,    -1,    -1,    -1,
      28,    29,    30,    31,   204,    33,    -1,    -1,    -1,   209,
      -1,    39,    -1,    -1,    42,    43,    -1,   217,    -1,    -1,
      48,    49,    50,    -1,    52,    53,    -1,    55,    56,    57,
      -1,    14,    -1,    -1,    -1,    -1,    19,    20,    -1,    -1,
      -1,    69,    70,    71,    72,    28,    -1,    30,    31,    -1,
      33,    -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,    42,
      43,    -1,    -1,    -1,    -1,    48,    49,    50,    -1,    52,
      53,    -1,    55,    56,    57,    14,    -1,    -1,    -1,    -1,
      19,    20,    -1,    -1,    67,    24,    69,    70,    71,    72,
      -1,    30,    31,    -1,    33,    -1,    -1,    -1,    -1,    -1,
      39,    -1,    -1,    42,    43,    -1,    -1,    -1,    -1,    48,
      49,    50,    -1,    52,    53,    -1,    55,    56,    57,    14,
      -1,    -1,    -1,    -1,    19,    20,    -1,    -1,    -1,    24,
      69,    70,    71,    72,    -1,    -1,    31,    -1,    33,    -1,
      -1,    -1,    -1,    -1,    39,    -1,    -1,    42,    43,    -1,
      -1,    -1,    -1,    48,    49,    50,    -1,    52,    53,    -1,
      55,    56,    57,    14,    -1,    -1,    -1,    -1,    19,    20,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,    30,
      31,    -1,    33,    -1,    -1,    -1,    -1,    -1,    39,    -1,
      -1,    42,    43,    -1,    -1,    -1,    -1,    48,    49,    50,
      -1,    52,    53,    -1,    55,    56,    57,    14,    -1,    -1,
      -1,    -1,    19,    20,    -1,    -1,    -1,    -1,    69,    70,
      71,    72,    -1,    -1,    31,    -1,    33,    -1,    -1,    -1,
      -1,    -1,    39,    -1,    -1,    42,    43,    -1,    -1,    -1,
      -1,    48,    49,    50,    -1,    52,    53,    -1,    55,    56,
      57,    -1,    -1,    14,    -1,    -1,    -1,    64,    19,    20,
      -1,    -1,    69,    70,    71,    72,    -1,    -1,    -1,    30,
      31,    -1,    33,    -1,    -1,    -1,    -1,    -1,    39,    -1,
      -1,    42,    43,    -1,    -1,    -1,    -1,    48,    49,    50,
      -1,    52,    53,    -1,    55,    56,    57,    14,    -1,    -1,
      -1,    -1,    19,    20,    -1,    -1,    -1,    -1,    69,    70,
      71,    72,    -1,    30,    31,    -1,    33,    -1,    -1,    -1,
      -1,    -1,    39,    -1,    -1,    42,    43,    -1,    -1,    -1,
      -1,    48,    49,    50,    -1,    52,    53,    -1,    55,    56,
      57,    14,    -1,    -1,    -1,    -1,    19,    20,    -1,    -1,
      -1,    -1,    69,    70,    71,    72,    -1,    30,    31,    -1,
      33,    -1,    -1,    -1,    -1,    -1,    39,    -1,    -1,    42,
      43,    -1,    -1,    -1,    -1,    48,    49,    50,    -1,    52,
      53,    -1,    55,    56,    57,    14,    -1,    -1,    -1,    -1,
      19,    20,    -1,    -1,    -1,    -1,    69,    70,    71,    72,
      -1,    30,    31,    -1,    33,    -1,    -1,    -1,    -1,    -1,
      39,    -1,    -1,    42,    43,    -1,    -1,    -1,    -1,    48,
      49,    50,    -1,    52,    53,    -1,    55,    56,    57,    14,
      -1,    -1,    -1,    -1,    19,    20,    -1,    -1,    -1,    -1,
      69,    70,    71,    72,    -1,    30,    31,    -1,    33,    -1,
      -1,    -1,    -1,    -1,    39,    -1,    -1,    42,    43,    -1,
      -1,    -1,    -1,    48,    49,    50,    -1,    52,    53,    -1,
      55,    56,    57,    14,    -1,    -1,    -1,    -1,    19,    20,
      -1,    -1,    -1,    -1,    69,    70,    71,    72,    -1,    30,
      31,    -1,    33,    -1,    -1,    -1,    -1,    -1,    39,    -1,
      -1,    42,    43,    -1,    -1,    -1,    -1,    48,    49,    50,
      -1,    52,    53,    -1,    55,    56,    57,    14,    -1,    -1,
      -1,    -1,    19,    20,    -1,    -1,    -1,    -1,    69,    70,
      71,    72,    -1,    -1,    31,    -1,    33,    -1,    -1,    -1,
      -1,    -1,    39,    -1,    -1,    42,    43,    -1,    -1,    -1,
      -1,    48,    49,    50,    -1,    52,    53,    -1,    55,    56,
      57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    69,    70,    71,    72
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    14,    19,    20,    31,    33,    39,    42,    43,    48,
      49,    50,    52,    53,    55,    56,    57,    69,    70,    71,
      72,    79,    82,    83,    84,    86,    96,    98,   102,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   119,   120,
     121,   122,   123,   124,   125,   127,   128,   138,    10,    14,
      15,   107,    14,    40,   107,   134,    14,    14,   107,    14,
     131,   137,    14,    56,    59,   129,    75,   103,   104,   105,
     107,    14,    59,     0,    73,    80,    18,    34,    44,    51,
      68,    67,    99,   100,    41,    65,   136,    41,    81,    82,
      65,    36,    74,    56,    26,    38,    45,   117,   118,    66,
      95,   106,   107,   106,    74,    77,    56,    95,    16,    30,
      85,   126,    81,    81,   133,   134,    28,   100,   101,   106,
     106,   106,    73,   106,   130,   137,   117,    77,    77,    77,
      74,     5,    94,    36,    74,    76,   104,   132,   117,    94,
      14,    87,    24,    81,    88,    89,    81,    73,    73,    81,
      81,    81,    30,    65,    28,    29,    82,    97,     5,     5,
       5,   118,    56,   107,    77,    56,    90,    73,    73,    73,
      30,    64,    73,    73,    73,    19,   106,    81,   134,    30,
     117,    75,   117,    21,    23,    37,    30,    30,    81,    89,
      30,    44,   135,    30,    73,    81,    39,   131,    34,    92,
      34,    34,    87,    87,    73,    34,    87,    51,    68,    73,
      76,    59,    14,    47,    58,    59,    61,    91,    93,    30,
      87,    87,    87,    97,    95,    35,    95,    74,    82,    87,
      94,    94,    93
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
#line 195 "gram.y"
    {
					plpsm_parser_tree = (yyvsp[(1) - (2)].stmt);
					if (plpsm_debug_parser)
						elog_stmt(NOTICE, (yyvsp[(1) - (2)].stmt));
				;}
    break;

  case 3:

/* Line 1464 of yacc.c  */
#line 204 "gram.y"
    {
				;}
    break;

  case 4:

/* Line 1464 of yacc.c  */
#line 207 "gram.y"
    {
				;}
    break;

  case 5:

/* Line 1464 of yacc.c  */
#line 213 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 217 "gram.y"
    {
					(yyvsp[(1) - (3)].stmt)->last->next = (yyvsp[(3) - (3)].stmt);
					(yyvsp[(1) - (3)].stmt)->last = (yyvsp[(3) - (3)].stmt)->last;
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 228 "gram.y"
    {
					if (!(yyvsp[(1) - (1)].stmt)->last)
						(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 234 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 235 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 236 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 11:

/* Line 1464 of yacc.c  */
#line 237 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 12:

/* Line 1464 of yacc.c  */
#line 238 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 13:

/* Line 1464 of yacc.c  */
#line 239 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 14:

/* Line 1464 of yacc.c  */
#line 240 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 15:

/* Line 1464 of yacc.c  */
#line 241 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 16:

/* Line 1464 of yacc.c  */
#line 242 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 17:

/* Line 1464 of yacc.c  */
#line 243 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 18:

/* Line 1464 of yacc.c  */
#line 244 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 245 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 20:

/* Line 1464 of yacc.c  */
#line 246 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 21:

/* Line 1464 of yacc.c  */
#line 247 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 22:

/* Line 1464 of yacc.c  */
#line 248 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 23:

/* Line 1464 of yacc.c  */
#line 249 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 24:

/* Line 1464 of yacc.c  */
#line 250 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 25:

/* Line 1464 of yacc.c  */
#line 251 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 26:

/* Line 1464 of yacc.c  */
#line 252 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 27:

/* Line 1464 of yacc.c  */
#line 253 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 28:

/* Line 1464 of yacc.c  */
#line 254 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 255 "gram.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 272 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yyvsp[(1) - (7)].str) ? (yylsp[(1) - (7)]): (yylsp[(2) - (7)]));
					new->name = (yyvsp[(1) - (7)].str);
					new->option = (yyvsp[(3) - (7)].boolean);
					new->inner_left = (yyvsp[(4) - (7)].stmt);
					check_labels((yyvsp[(1) - (7)].str), (yyvsp[(7) - (7)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 281 "gram.y"
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
#line 290 "gram.y"
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

  case 33:

/* Line 1464 of yacc.c  */
#line 302 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_COMPOUND_STATEMENT, (yyvsp[(1) - (4)].str) ? (yylsp[(1) - (4)]): (yylsp[(2) - (4)]));
					new->name = (yyvsp[(1) - (4)].str);
					check_labels((yyvsp[(1) - (4)].str), (yyvsp[(4) - (4)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 311 "gram.y"
    {
					(yyval.boolean) = false;
				;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 315 "gram.y"
    {
					(yyval.boolean) = true;
				;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 326 "gram.y"
    {
					(yyval.str) = (yyvsp[(1) - (1)].word).ident;
					if (yylex() != ':')
						ereport(ERROR,
							(errcode(ERRCODE_SYNTAX_ERROR),
								 errmsg("syntax error, bizzare label \"%s\"", (yyvsp[(1) - (1)].word).ident),
									parser_errposition((yylsp[(1) - (1)]))));
				;}
    break;

  case 37:

/* Line 1464 of yacc.c  */
#line 335 "gram.y"
    {
					(yyval.str) = NULL;
				;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 342 "gram.y"
    {
					(yyval.str) = (yyvsp[(1) - (1)].word).ident;
				;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 346 "gram.y"
    {
					(yyval.str) = NULL;
				;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 353 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
					(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 358 "gram.y"
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

  case 42:

/* Line 1464 of yacc.c  */
#line 394 "gram.y"
    {
					if ((yyvsp[(2) - (2)].stmt)->typ != PLPSM_STMT_DECLARE_VARIABLE)
						yyerror("syntax error");
					(yyvsp[(2) - (2)].stmt)->lineno = plpsm_location_to_lineno((yylsp[(1) - (2)]));
					(yyval.stmt) = (yyvsp[(2) - (2)].stmt);
				;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 401 "gram.y"
    {
					if ((yyvsp[(2) - (4)].stmt)->typ != PLPSM_STMT_DECLARE_CONDITION)
						yyerror("syntax error");
					(yyvsp[(2) - (4)].stmt)->data = (yyvsp[(4) - (4)].condition);
					(yyvsp[(2) - (4)].stmt)->lineno = plpsm_location_to_lineno((yylsp[(1) - (4)]));
					(yyval.stmt) = (yyvsp[(2) - (4)].stmt);
				;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 409 "gram.y"
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
					(yyval.stmt) = (yyvsp[(2) - (4)].stmt);
				;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 426 "gram.y"
    {
					if ((yyvsp[(2) - (6)].stmt)->typ != PLPSM_STMT_DECLARE_HANDLER)
						yyerror("syntax error");
					(yyvsp[(2) - (6)].stmt)->inner_left = (yyvsp[(6) - (6)].stmt);
					(yyvsp[(2) - (6)].stmt)->data = (yyvsp[(5) - (6)].condition);
					(yyvsp[(2) - (6)].stmt)->lineno = plpsm_location_to_lineno((yylsp[(1) - (6)]));
					(yyval.stmt) = (yyvsp[(2) - (6)].stmt);
				;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 437 "gram.y"
    {
					(yyval.stmt) = declare_prefetch();
				;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 444 "gram.y"
    {
					(yyval.condition) = (yyvsp[(1) - (1)].condition);
				;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 448 "gram.y"
    {
					(yyval.condition) = (yyvsp[(1) - (3)].condition);
					(yyval.condition)->next = (yyvsp[(3) - (3)].condition);
				;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 456 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->typ = PLPSM_SQLSTATE;
					new->sqlstate = (yyvsp[(4) - (4)].ival);
					(yyval.condition) = new;
				;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 462 "gram.y"
    {
					(yyval.condition) = NULL;
				;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 469 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (3)]);
					new->typ = PLPSM_SQLSTATE;
					new->sqlstate = (yyvsp[(3) - (3)].ival);
					(yyval.condition) = new;
				;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 477 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (2)]);
					new->typ = PLPSM_SQLSTATE;
					new->sqlstate = MAKE_SQLSTATE('0','2','0','0','0');
					(yyval.condition) = new;
				;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 485 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (1)]);
					new->typ = PLPSM_SQLWARNING;
					(yyval.condition) = new;
				;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 492 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (1)]);
					new->typ = PLPSM_SQLEXCEPTION;
					(yyval.condition) = new;
				;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 499 "gram.y"
    {
					Plpsm_condition_value *new = palloc0(sizeof(Plpsm_condition_value));
					new->location = (yylsp[(1) - (1)]);
					new->typ = PLPSM_CONDITION_NAME;
					new->condition_name = (yyvsp[(1) - (1)].word).ident;
					(yyval.condition) = new;
				;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 510 "gram.y"
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

  case 57:

/* Line 1464 of yacc.c  */
#line 532 "gram.y"
    {
				;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 534 "gram.y"
    {
				;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 545 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_IF, (yylsp[(1) - (7)]));
					new->esql = (yyvsp[(2) - (7)].esql);
					new->inner_left = (yyvsp[(3) - (7)].stmt);
					new->inner_right = (yyvsp[(5) - (7)].stmt);
					(yyval.stmt) = new;
				;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 555 "gram.y"
    {
					(yyval.stmt) = NULL;
				;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 559 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_IF, (yylsp[(1) - (5)]));
					new->esql = (yyvsp[(2) - (5)].esql);
					new->inner_left = (yyvsp[(3) - (5)].stmt);
					new->inner_right = (yyvsp[(5) - (5)].stmt);
					(yyval.stmt) = new;
				;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 567 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (3)].stmt);
				;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 580 "gram.y"
    {
						Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CASE, (yylsp[(1) - (6)]));
						new->esql = (yyvsp[(2) - (6)].esql);
						new->inner_left = (yyvsp[(3) - (6)].stmt);
						new->inner_right = (yyvsp[(4) - (6)].stmt);
						(yyval.stmt) = new;
					;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 591 "gram.y"
    {
					(yyvsp[(1) - (2)].stmt)->last->next = (yyvsp[(2) - (2)].stmt);
					(yyvsp[(1) - (2)].stmt)->last = (yyvsp[(2) - (2)].stmt);
				;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 596 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 603 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CASE, (yylsp[(1) - (4)]));
					new->esql = (yyvsp[(2) - (4)].esql);
					new->inner_left = (yyvsp[(3) - (4)].stmt);
					new->last = new;
					(yyval.stmt) = new;
				;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 613 "gram.y"
    {
					(yyval.stmt) = NULL;
				;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 617 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (3)].stmt);
				;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 633 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(2) - (2)].stmt);
					(yyval.stmt)->location = (yylsp[(1) - (2)]);
				;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 638 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SET, (yylsp[(1) - (8)]));
					new->compound_target = (yyvsp[(3) - (8)].list);
					new->esql_list = (yyvsp[(7) - (8)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 648 "gram.y"
    {
					(yyval.stmt) = (yyvsp[(1) - (1)].stmt);
					(yyvsp[(1) - (1)].stmt)->last = (yyvsp[(1) - (1)].stmt);
				;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 653 "gram.y"
    {
					/* expected just very short list */
					(yyvsp[(1) - (3)].stmt)->last->next = (yyvsp[(3) - (3)].stmt);
					(yyvsp[(1) - (3)].stmt)->last = (yyvsp[(3) - (3)].stmt);
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 661 "gram.y"
    {
					(yyvsp[(1) - (3)].stmt)->typ = PLPSM_STMT_SET;
					(yyvsp[(1) - (3)].stmt)->esql = (yyvsp[(3) - (3)].esql);
					(yyval.stmt) = (yyvsp[(1) - (3)].stmt);
				;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 670 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_UNKNOWN, (yylsp[(1) - (1)]));
					new->target = (yyvsp[(1) - (1)].qualid);
					(yyval.stmt) = new;
				;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 679 "gram.y"
    {
					(yyval.list) = list_make1((yyvsp[(1) - (1)].qualid));
				;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 683 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].qualid));
				;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 690 "gram.y"
    {
					(yyval.qualid) = new_qualid(list_make1((yyvsp[(1) - (1)].word).ident), (yylsp[(1) - (1)]));
				;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 694 "gram.y"
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

  case 79:

/* Line 1464 of yacc.c  */
#line 704 "gram.y"
    {
					/* ToDo: Plpsm_object should be a param type too */
					char buf[32];
					snprintf(buf, sizeof(buf), "$%d", (yyvsp[(1) - (1)].ival));
					(yyval.qualid) = (yyval.qualid) = new_qualid(list_make1(pstrdup(buf)), (yylsp[(1) - (1)]));
				;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 721 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PRINT, (yylsp[(1) - (2)]));
					new->esql_list = (yyvsp[(2) - (2)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 736 "gram.y"
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
						new->esql = read_expr_until_semi();
					}
					(yyval.stmt) = new;
				;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 760 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_LOOP, (yyvsp[(1) - (7)].str) ? (yylsp[(1) - (7)]) : (yylsp[(2) - (7)]));
					new->name = (yyvsp[(1) - (7)].str);
					new->inner_left = (yyvsp[(3) - (7)].stmt);
					check_labels((yyvsp[(1) - (7)].str), (yyvsp[(7) - (7)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 777 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_WHILE, (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					new->esql = (yyvsp[(3) - (8)].esql);
					new->inner_left = (yyvsp[(4) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 795 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_REPEAT_UNTIL, (yyvsp[(1) - (8)].str) ? (yylsp[(1) - (8)]) : (yylsp[(2) - (8)]));
					new->name = (yyvsp[(1) - (8)].str);
					new->esql = (yyvsp[(6) - (8)].esql);
					new->inner_left = (yyvsp[(3) - (8)].stmt);
					check_labels((yyvsp[(1) - (8)].str), (yyvsp[(8) - (8)].str));
					(yyval.stmt) = new;
				;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 813 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_ITERATE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 829 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_LEAVE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 844 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SIGNAL, (yylsp[(1) - (4)]));
					new->option = (yyvsp[(4) - (4)].ival);
					(yyval.stmt) = new;
				;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 850 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SIGNAL, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 856 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SIGNAL, (yylsp[(1) - (6)]));
					new->option = (yyvsp[(4) - (6)].ival);
					new->data = (yyvsp[(6) - (6)].sinfo);
					(yyval.stmt) = new;
				;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 863 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_SIGNAL, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->data = (yyvsp[(4) - (4)].sinfo);
					(yyval.stmt) = new;
				;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 878 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (1)]));
					(yyval.stmt) = new;
				;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 883 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (4)]));
					new->option = (yyvsp[(4) - (4)].ival);
					(yyval.stmt) = new;
				;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 889 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 895 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (6)]));
					new->option = (yyvsp[(4) - (6)].ival);
					new->data = (yyvsp[(6) - (6)].sinfo);
					(yyval.stmt) = new;
				;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 902 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->data = (yyvsp[(4) - (4)].sinfo);
					(yyval.stmt) = new;
				;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 909 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_RESIGNAL, (yylsp[(1) - (3)]));
					new->data = (yyvsp[(3) - (3)].sinfo);
					(yyval.stmt) = new;
				;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 918 "gram.y"
    {
					(yyvsp[(1) - (1)].sinfo)->next = NULL;
					(yyval.sinfo) = (yyvsp[(1) - (1)].sinfo);
				;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 923 "gram.y"
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

  case 99:

/* Line 1464 of yacc.c  */
#line 945 "gram.y"
    {
					Plpsm_signal_info *new = palloc(sizeof(Plpsm_signal_info));
					new->typ = PLPSM_SINFO_DETAIL;
					new->value = (yyvsp[(3) - (3)].str);
					(yyval.sinfo) = new;
				;}
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 952 "gram.y"
    {
					Plpsm_signal_info *new = palloc(sizeof(Plpsm_signal_info));
					new->typ = PLPSM_SINFO_HINT;
					new->value = (yyvsp[(3) - (3)].str);
					(yyval.sinfo) = new;
				;}
    break;

  case 101:

/* Line 1464 of yacc.c  */
#line 959 "gram.y"
    {
					Plpsm_signal_info *new = palloc(sizeof(Plpsm_signal_info));
					new->typ = PLPSM_SINFO_MESSAGE;
					new->value = (yyvsp[(3) - (3)].str);
					(yyval.sinfo) = new;
				;}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 974 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_PREPARE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->esql = (yyvsp[(4) - (4)].esql);
					(yyval.stmt) = new;
				;}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 991 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (2)]));
					new->name = (yyvsp[(2) - (2)].word).ident;
					(yyval.stmt) = new;
				;}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 997 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->compound_target = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 1004 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (6)]));
					new->name = (yyvsp[(2) - (6)].word).ident;
					new->compound_target = (yyvsp[(4) - (6)].list);
					new->variables = (yyvsp[(6) - (6)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 1012 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE, (yylsp[(1) - (4)]));
					new->name = (yyvsp[(2) - (4)].word).ident;
					new->variables = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 1028 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_EXECUTE_IMMEDIATE, (yylsp[(1) - (3)]));
					new->esql = (yyvsp[(3) - (3)].esql);
					(yyval.stmt) = new;
				;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 1044 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_OPEN, (yylsp[(1) - (2)]));
					new->target = (yyvsp[(2) - (2)].qualid);
					(yyval.stmt) = new;
				;}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 1050 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_OPEN, (yylsp[(1) - (4)]));
					new->target = (yyvsp[(2) - (4)].qualid);
					new->variables = (yyvsp[(4) - (4)].list);
					(yyval.stmt) = new;
				;}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 1066 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_FETCH, (yylsp[(1) - (4)]));
					new->target = (yyvsp[(2) - (4)].qualid);
					new->compound_target = (yyvsp[(4) - (4)].list);
					new->data = (yyvsp[(2) - (4)].qualid);
					(yyval.stmt) = new;
				;}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 1083 "gram.y"
    {
					Plpsm_stmt *new = plpsm_new_stmt(PLPSM_STMT_CLOSE, (yylsp[(1) - (2)]));
					new->target = (yyvsp[(2) - (2)].qualid);
					(yyval.stmt) = new;
				;}
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 1098 "gram.y"
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

  case 113:

/* Line 1464 of yacc.c  */
#line 1114 "gram.y"
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

  case 114:

/* Line 1464 of yacc.c  */
#line 1182 "gram.y"
    { (yyval.stmt) = make_stmt_sql((yylsp[(1) - (1)])); ;}
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 1183 "gram.y"
    { (yyval.stmt) = make_stmt_sql((yylsp[(1) - (1)])); ;}
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 1184 "gram.y"
    { (yyval.stmt) = make_stmt_sql((yylsp[(1) - (1)])); ;}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 1185 "gram.y"
    { (yyval.stmt) = make_stmt_sql((yylsp[(1) - (1)])); ;}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 1190 "gram.y"
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

  case 119:

/* Line 1464 of yacc.c  */
#line 1203 "gram.y"
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

  case 120:

/* Line 1464 of yacc.c  */
#line 1221 "gram.y"
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

  case 121:

/* Line 1464 of yacc.c  */
#line 1241 "gram.y"
    {
					(yyval.esql) = read_expr_until_semi();
				;}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 1248 "gram.y"
    {
					(yyval.list) = list_make1((yyvsp[(1) - (1)].esql));
				;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 1252 "gram.y"
    {
					(yyval.list) = lappend((yyvsp[(1) - (3)].list), (yyvsp[(3) - (3)].esql));
				;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 1258 "gram.y"
    {
					Plpsm_ESQL *esql = read_embeded_sql(';',',', 0, "; or ,", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					(yyval.esql) = esql;
				;}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 1268 "gram.y"
    {
					Plpsm_ESQL *esql = read_embeded_sql(DO, -1, -1, "DO", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					(yyval.esql) = esql;
				;}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 1278 "gram.y"
    {
					Plpsm_ESQL *esql = read_embeded_sql(THEN, -1, -1, "THEN", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					(yyval.esql) = esql;
				;}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 1288 "gram.y"
    {
					Plpsm_ESQL *esql = read_embeded_sql(END, -1, -1, "END", 
												PLPSM_ESQL_EXPR, true, 
														    NULL, -1, 
															    NULL, NULL);
					(yyval.esql) = esql;
				;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 1298 "gram.y"
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

  case 129:

/* Line 1464 of yacc.c  */
#line 1311 "gram.y"
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

  case 130:

/* Line 1464 of yacc.c  */
#line 1325 "gram.y"
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
#line 3410 "gram.c"
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
#line 1341 "gram.y"


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
		case DETAIL:
		case EXIT:
		case FOUND:
		case HINT:
		case IMMEDIATE:
		case MESSAGE_TEXT:
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
		case PLPSM_STMT_SIGNAL:
			return "signal statement";
		case PLPSM_STMT_RESIGNAL:
			return "resignal statement";
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
								appendStringInfo(ds, " DETAIL=\"%s\"", sinfo->value);
								break;
							case PLPSM_SINFO_HINT:
								appendStringInfo(ds, " HINT=\"%s\"", sinfo->value);
								break;
							case PLPSM_SINFO_MESSAGE:
								appendStringInfo(ds, " MESSAGE=\"%s\"", sinfo->value);
								break;
						}
						sinfo = sinfo->next;
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
			case SIGNAL:
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

