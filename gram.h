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
     NO = 305,
     NOT = 306,
     OPEN = 307,
     PREPARE = 308,
     PRINT = 309,
     REPEAT = 310,
     RESIGNAL = 311,
     RETURN = 312,
     ROW_COUNT = 313,
     SCROLL = 314,
     SELECT = 315,
     SET = 316,
     SIGNAL = 317,
     SQLEXCEPTION = 318,
     SQLSTATE = 319,
     SQLCODE = 320,
     SQLWARNING = 321,
     STACKED = 322,
     THEN = 323,
     UNDO = 324,
     UNTIL = 325,
     USING = 326,
     VALUE = 327,
     WHEN = 328,
     WHILE = 329,
     INSERT = 330,
     UPDATE = 331,
     DELETE = 332,
     TRUNCATE = 333
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
#line 152 "gram.h"
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

