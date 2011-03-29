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
     CONTINUE = 277,
     CURSOR = 278,
     CURRENT = 279,
     DECLARE = 280,
     DEFAULT = 281,
     DETAIL_TEXT = 282,
     DIAGNOSTICS = 283,
     DO = 284,
     ELSE = 285,
     ELSEIF = 286,
     END = 287,
     EXECUTE = 288,
     EXIT = 289,
     FETCH = 290,
     FOR = 291,
     FOUND = 292,
     FROM = 293,
     GET = 294,
     HANDLER = 295,
     HINT_TEXT = 296,
     IF = 297,
     IMMEDIATE = 298,
     INTO = 299,
     ITERATE = 300,
     LEAVE = 301,
     LOOP = 302,
     MESSAGE_TEXT = 303,
     NO = 304,
     NOT = 305,
     OPEN = 306,
     PREPARE = 307,
     PRINT = 308,
     REPEAT = 309,
     RESIGNAL = 310,
     RETURN = 311,
     ROW_COUNT = 312,
     SCROLL = 313,
     SELECT = 314,
     SET = 315,
     SIGNAL = 316,
     SQLEXCEPTION = 317,
     SQLSTATE = 318,
     SQLCODE = 319,
     SQLWARNING = 320,
     STACKED = 321,
     THEN = 322,
     UNDO = 323,
     UNTIL = 324,
     USING = 325,
     VALUE = 326,
     WHEN = 327,
     WHILE = 328,
     INSERT = 329,
     UPDATE = 330,
     DELETE = 331,
     TRUNCATE = 332
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
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



/* Line 1685 of yacc.c  */
#line 151 "gram.h"
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

