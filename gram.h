/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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
     NEW = 305,
     NO = 306,
     NOT = 307,
     OLD = 308,
     OPEN = 309,
     PREPARE = 310,
     PRINT = 311,
     REPEAT = 312,
     RESIGNAL = 313,
     RETURNED_SQLCODE = 314,
     RETURNED_SQLSTATE = 315,
     RETURN = 316,
     ROW_COUNT = 317,
     SCROLL = 318,
     SELECT = 319,
     SET = 320,
     SIGNAL = 321,
     SQLEXCEPTION = 322,
     SQLSTATE = 323,
     SQLCODE = 324,
     SQLWARNING = 325,
     STACKED = 326,
     THEN = 327,
     UNDO = 328,
     UNTIL = 329,
     USING = 330,
     VALUE = 331,
     WHEN = 332,
     WHILE = 333,
     WITH = 334,
     INSERT = 335,
     UPDATE = 336,
     DELETE = 337,
     TRUNCATE = 338
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 92 "gram.y"

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



/* Line 1685 of yacc.c  */
#line 157 "gram.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE plpsm_yylval;

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

extern YYLTYPE plpsm_yylloc;

