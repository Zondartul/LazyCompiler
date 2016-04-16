
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison GLR parsers in C
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.
   
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

/* "%code requires" blocks.  */

/* Line 2638 of glr.c  */
#line 4 "yaccin.y"

  #define YYLTYPE YYLTYPE
  
  #include "globals.h"
  #include "AST.h"
  #include "semantic.h"
	#include "vector2.h"
	int finalNode;
	int yyerror(const char *s);



/* Line 2638 of glr.c  */
#line 52 "yaccin.tab.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     RETURN = 258,
     CLASS = 259,
     ID = 260,
     TYPE = 261,
     END = 262,
     IF = 263,
     THEN = 264,
     ELSE = 265,
     ELSEIF = 266,
     WHILE = 267,
     INTEGER = 268,
     INTEGERX = 269,
     INTEGERB = 270,
     FLOATING = 271,
     CHARACTER = 272,
     STRING = 273
   };
#endif


#ifndef YYSTYPE
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{

  int first_line;
  int first_column;
  int last_line;
  int last_column;

} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

/* "%code provides" blocks.  */

/* Line 2638 of glr.c  */
#line 14 "yaccin.y"

char *posToString(YYLTYPE pos);




/* Line 2638 of glr.c  */
#line 110 "yaccin.tab.h"


extern YYSTYPE yylval;

extern YYLTYPE yylloc;


