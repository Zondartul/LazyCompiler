/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_GENERATED_YACCIN_TAB_H_INCLUDED
# define YY_YY_GENERATED_YACCIN_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 4 "input/yaccin.y" /* yacc.c:1909  */

  //#define YYLTYPE YYLTYPE
  #include "globals.h"
  #include "types/ast_node.h"
  
	//ast_node.h not fully included yet due to shenanigans
	typedef struct ast_node ast_node;
	
	ast_node *finalNode;
	int yyerror(const char *s);

#line 56 "generated/yaccin.tab.h" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    RETURN = 258,
    CLASS = 259,
    ID = 260,
    TYPE = 261,
    END = 262,
    IF = 263,
    ELSE = 264,
    ELSEIF = 265,
    WHILE = 266,
    FOR = 267,
    INTEGER = 268,
    INTEGERX = 269,
    INTEGERB = 270,
    FLOATING = 271,
    CHARACTER = 272,
    STRING = 273,
    EQUAL = 274,
    NOTEQUAL = 275,
    INC = 276,
    DEC = 277,
    PREDEREF = 278,
    PREREF = 279,
    PREINC = 280,
    PREDEC = 281,
    PRENEG = 282
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);
/* "%code provides" blocks.  */
#line 15 "input/yaccin.y" /* yacc.c:1909  */

char *posToString(YYLTYPE pos);
char *get_source_text2(YYLTYPE pos);

#line 122 "generated/yaccin.tab.h" /* yacc.c:1909  */

#endif /* !YY_YY_GENERATED_YACCIN_TAB_H_INCLUDED  */
