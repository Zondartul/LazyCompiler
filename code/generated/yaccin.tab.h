/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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
#line 4 "input/yaccin.y"

  //#define YYLTYPE YYLTYPE
  #include "globals.h"
  #include "types/ast_node.h"
  
	//ast_node.h not fully included yet due to shenanigans
	typedef struct ast_node ast_node;
	
	extern ast_node *finalNode;
	void /*int*/ yyerror(const char *s);

#line 61 "generated/yaccin.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    RETURN = 258,                  /* RETURN  */
    CLASS = 259,                   /* CLASS  */
    ID = 260,                      /* ID  */
    TYPE = 261,                    /* TYPE  */
    END = 262,                     /* END  */
    IF = 263,                      /* IF  */
    ELSE = 264,                    /* ELSE  */
    ELSEIF = 265,                  /* ELSEIF  */
    WHILE = 266,                   /* WHILE  */
    FOR = 267,                     /* FOR  */
    INTEGER = 268,                 /* INTEGER  */
    INTEGERX = 269,                /* INTEGERX  */
    INTEGERB = 270,                /* INTEGERB  */
    FLOATING = 271,                /* FLOATING  */
    CHARACTER = 272,               /* CHARACTER  */
    STRING = 273,                  /* STRING  */
    EQUAL = 274,                   /* EQUAL  */
    NOTEQUAL = 275,                /* NOTEQUAL  */
    INC = 276,                     /* INC  */
    DEC = 277,                     /* DEC  */
    ASSIGN = 278,                  /* ASSIGN  */
    LOGIC = 279,                   /* LOGIC  */
    COMPARE = 280,                 /* COMPARE  */
    ADDSUB = 281,                  /* ADDSUB  */
    MULDIV = 282,                  /* MULDIV  */
    EXP = 283,                     /* EXP  */
    PREOP = 284,                   /* PREOP  */
    NEG = 285,                     /* NEG  */
    LNEG = 286,                    /* LNEG  */
    CAST = 287,                    /* CAST  */
    DEREF = 288,                   /* DEREF  */
    REF = 289,                     /* REF  */
    POSTOP = 290,                  /* POSTOP  */
    CALL = 291,                    /* CALL  */
    INDEX = 292,                   /* INDEX  */
    DOT = 293,                     /* DOT  */
    BRACELIST = 294,               /* BRACELIST  */
    SUBEXPR = 295,                 /* SUBEXPR  */
    PRENEG = 296                   /* PRENEG  */
  };
  typedef enum yytokentype yytoken_kind_t;
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
#line 15 "input/yaccin.y"

char *posToString(YYLTYPE pos);
char *get_source_text2(YYLTYPE pos);

#line 148 "generated/yaccin.tab.h"

#endif /* !YY_YY_GENERATED_YACCIN_TAB_H_INCLUDED  */
