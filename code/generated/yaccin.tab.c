/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "input/yaccin.y"

	

#line 75 "generated/yaccin.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yaccin.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_RETURN = 3,                     /* RETURN  */
  YYSYMBOL_CLASS = 4,                      /* CLASS  */
  YYSYMBOL_ID = 5,                         /* ID  */
  YYSYMBOL_TYPE = 6,                       /* TYPE  */
  YYSYMBOL_END = 7,                        /* END  */
  YYSYMBOL_IF = 8,                         /* IF  */
  YYSYMBOL_ELSE = 9,                       /* ELSE  */
  YYSYMBOL_ELSEIF = 10,                    /* ELSEIF  */
  YYSYMBOL_WHILE = 11,                     /* WHILE  */
  YYSYMBOL_FOR = 12,                       /* FOR  */
  YYSYMBOL_INTEGER = 13,                   /* INTEGER  */
  YYSYMBOL_INTEGERX = 14,                  /* INTEGERX  */
  YYSYMBOL_INTEGERB = 15,                  /* INTEGERB  */
  YYSYMBOL_FLOATING = 16,                  /* FLOATING  */
  YYSYMBOL_CHARACTER = 17,                 /* CHARACTER  */
  YYSYMBOL_STRING = 18,                    /* STRING  */
  YYSYMBOL_EQUAL = 19,                     /* EQUAL  */
  YYSYMBOL_NOTEQUAL = 20,                  /* NOTEQUAL  */
  YYSYMBOL_INC = 21,                       /* INC  */
  YYSYMBOL_DEC = 22,                       /* DEC  */
  YYSYMBOL_VARARGS = 23,                   /* VARARGS  */
  YYSYMBOL_ASM = 24,                       /* ASM  */
  YYSYMBOL_LEQUAL = 25,                    /* LEQUAL  */
  YYSYMBOL_GEQUAL = 26,                    /* GEQUAL  */
  YYSYMBOL_27_ = 27,                       /* '='  */
  YYSYMBOL_ASSIGN = 28,                    /* ASSIGN  */
  YYSYMBOL_LOGIC = 29,                     /* LOGIC  */
  YYSYMBOL_30_ = 30,                       /* '&'  */
  YYSYMBOL_COMPARE = 31,                   /* COMPARE  */
  YYSYMBOL_32_ = 32,                       /* '+'  */
  YYSYMBOL_33_ = 33,                       /* '-'  */
  YYSYMBOL_ADDSUB = 34,                    /* ADDSUB  */
  YYSYMBOL_35_ = 35,                       /* '/'  */
  YYSYMBOL_36_ = 36,                       /* '%'  */
  YYSYMBOL_37_ = 37,                       /* '*'  */
  YYSYMBOL_MULDIV = 38,                    /* MULDIV  */
  YYSYMBOL_EXP = 39,                       /* EXP  */
  YYSYMBOL_PREOP = 40,                     /* PREOP  */
  YYSYMBOL_NEG = 41,                       /* NEG  */
  YYSYMBOL_42_ = 42,                       /* '!'  */
  YYSYMBOL_LNEG = 43,                      /* LNEG  */
  YYSYMBOL_CAST = 44,                      /* CAST  */
  YYSYMBOL_DEREF = 45,                     /* DEREF  */
  YYSYMBOL_REF = 46,                       /* REF  */
  YYSYMBOL_47_ = 47,                       /* '('  */
  YYSYMBOL_48_ = 48,                       /* '['  */
  YYSYMBOL_49_ = 49,                       /* '.'  */
  YYSYMBOL_POSTOP = 50,                    /* POSTOP  */
  YYSYMBOL_CALL = 51,                      /* CALL  */
  YYSYMBOL_INDEX = 52,                     /* INDEX  */
  YYSYMBOL_DOT = 53,                       /* DOT  */
  YYSYMBOL_BRACELIST = 54,                 /* BRACELIST  */
  YYSYMBOL_SUBEXPR = 55,                   /* SUBEXPR  */
  YYSYMBOL_56_ = 56,                       /* ';'  */
  YYSYMBOL_57_ = 57,                       /* ')'  */
  YYSYMBOL_58_ = 58,                       /* ']'  */
  YYSYMBOL_59_ = 59,                       /* ','  */
  YYSYMBOL_60_ = 60,                       /* '{'  */
  YYSYMBOL_61_ = 61,                       /* '}'  */
  YYSYMBOL_62_ = 62,                       /* '^'  */
  YYSYMBOL_63_ = 63,                       /* '|'  */
  YYSYMBOL_64_ = 64,                       /* '>'  */
  YYSYMBOL_65_ = 65,                       /* '<'  */
  YYSYMBOL_PRENEG = 66,                    /* PRENEG  */
  YYSYMBOL_YYACCEPT = 67,                  /* $accept  */
  YYSYMBOL_program = 68,                   /* program  */
  YYSYMBOL_stmt = 69,                      /* stmt  */
  YYSYMBOL_stmt_list = 70,                 /* stmt_list  */
  YYSYMBOL_stmt_list_ne = 71,              /* stmt_list_ne  */
  YYSYMBOL_decl_stmt = 72,                 /* decl_stmt  */
  YYSYMBOL_decl_stmt_list = 73,            /* decl_stmt_list  */
  YYSYMBOL_decl_stmt_list_ne = 74,         /* decl_stmt_list_ne  */
  YYSYMBOL_imp_stmt = 75,                  /* imp_stmt  */
  YYSYMBOL_asm_stmt = 76,                  /* asm_stmt  */
  YYSYMBOL_class_def = 77,                 /* class_def  */
  YYSYMBOL_STAR = 78,                      /* STAR  */
  YYSYMBOL_ptr_stars = 79,                 /* ptr_stars  */
  YYSYMBOL_ptr_stars_ne = 80,              /* ptr_stars_ne  */
  YYSYMBOL_typename = 81,                  /* typename  */
  YYSYMBOL_func_def = 82,                  /* func_def  */
  YYSYMBOL_var_decl = 83,                  /* var_decl  */
  YYSYMBOL_var_decl_list = 84,             /* var_decl_list  */
  YYSYMBOL_var_decl_list_ne = 85,          /* var_decl_list_ne  */
  YYSYMBOL_if_block = 86,                  /* if_block  */
  YYSYMBOL_if_then = 87,                   /* if_then  */
  YYSYMBOL_while_loop = 88,                /* while_loop  */
  YYSYMBOL_for_loop = 89,                  /* for_loop  */
  YYSYMBOL_expr_list = 90,                 /* expr_list  */
  YYSYMBOL_expr_list_ne = 91,              /* expr_list_ne  */
  YYSYMBOL_expr = 92                       /* expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 19 "input/yaccin.y"

	#include "globals.h"
  #include "AST.h"
  #include "semantic.h"
  #include "codegen.h"
	#include "vector2.h"
	#include "types/ast_node.h"

	ast_node *finalNode;

	//		B = B C | C
	//beceomes
	//		B = C C C C C
	void ast_unroll_lists_helper(ast_node *N2){
		//ast_node *N2 = ast_get_node(nodeID2);
		if(!N2->children.size){return;}
		ast_node *N3 = ast_get_child(N2,0);
		if(N2->token.type != N3->token.type){return;}
		ast_unroll_lists_helper(ast_get_child(N2, 0));
		m(N2->children,erase,0);
		int i;
		for(i = 0; i < N3->children.size; i++){
			//m(N2->children,push_back,ast_get_child_id(N3,i));
			m(N2->children,push_back,ast_get_child(N3,i));
		}
	}
	//turns left-recursive lists into non-recursive ones
	//so	A = B | empty
	//		B = B C | C
	//becomes	A = C C C C C C | empty
	
	void ast_unroll_lists(ast_node *N1){
		//ast_node *N1 = ast_get_node(nodeID);
		printf("unroll [%s] ",N1->token.type);
		if(!N1->children.size){return;}
		ast_node *N2 = ast_get_child(N1, 0);
		if(!N2->children.size){return;}
		//ast_unroll_lists_helper(ast_get_child_id(N1, 0));
		ast_unroll_lists_helper(ast_get_child(N1,0));
		m(N1->children,erase,0);
		int i;
		for(i = 0; i < N2->children.size; i++){
			//m(N1->children,push_front,ast_get_child_id(N2,i));
			m(N1->children,push_front,ast_get_child(N2,i));
		}
		printf(" unroll done\n");
	}
	
	void fix_decl_assign(ast_node *N1){
		//return;
		//struct ast_node *N1 = ast_get_node(nodeID);
		//int node_expr = *(int*)vector_get_reference(&N1->children,1);
		ast_node *node_expr = m(N1->children,pop_back);
		//vector_pop_back(&N1->children);
		//int node_ID = make_node((struct ast_token){"expr_id",0,N1->token.value,nullPos()},0);
		ast_node *node_ID = ast_node_new(ast_token_here("expr_id",0,N1->token.value,nullPos()),v2pan_here());
		//int N3 = make_node((struct ast_token){"expr_=",0,0,nullPos()},2,node_ID,node_expr);
		ast_node *N3 = ast_node_new(ast_token_here("expr_=",0,0,nullPos()),
									vector2_ptr_ast_node_here_from_list(2, node_ID, node_expr));
		//vector_set(&N1->children,1,&N3);
		m(N1->children,push_back,N3);
	}	
	
	int checkValidLoc(YYLTYPE loc, const char *name, int num){
		/*
		int null;
		int first_line;
		int first_column;
		int last_line;
		int last_column;
		int start;
		int end;
		char *filename;
		*/
		int valid = 1;
		if(loc.null){printf("checkValidLoc: loc [%s %d] is null\n",name,num); valid = 0;}
		else{
			if(loc.first_line > loc.last_line){printf("checkValidLoc: loc [%s %d] has first_line %d > last_line %d\n",name,num,loc.first_line,loc.last_line); valid = 0;}
			if(loc.first_line == loc.last_line){
				if(loc.first_column > loc.last_column){
					printf("checkValidLoc: loc [%s %d] has first_col %d > last_col %d on same line\n",name,num,loc.first_column,loc.last_column);
					valid = 0;
				}
			}
			if(loc.start > loc.end){printf("checkValidLoc: loc [%s %d] has start %d > end %d\n",name,num,loc.start,loc.end);valid = 0;}
			if(!loc.filename){printf("checkValidLoc: loc [%s %d] has no filename\n",name,num); valid = 0;}
		}
		return valid;
	}
	
	void printParsed(ast_node *node){
		printf("\n\n|parsed node [%s]-----\n", node->token.type);
		int valid = checkValidLoc(node->token.pos,"node",0);
		printf("|node loc check: %d\n",valid);
		printf("|node has %d children\n", node->children.size);
		for(int I = 0; I < node->children.size; I++){
			ast_node *N = ast_get_child(node,I);
			printf("|-child %d/%d: [%s]\n", I, node->children.size,N->token.type);
			int valid2 = checkValidLoc(N->token.pos,"child",I);
			printf("|-child loc valid: %d\n",valid2);
		}
		if(!(node->token.pos.null) && (node->token.pos.filename)){
			printf("|node sourcecode: [%s]\n",get_source_text2(node->token.pos));
		}else{
			printf("|no source\n");
		}
		printf(    "|end node    [%s]-----\n\n",node->token.type);
	}
	/* //collects all the stars and appends them to the node
	ast_unroll_stars_helper(int nodeID){
		struct ast_node *N1 = ast_get_node(nodeID);
		if(N1->children.size){
			
		}
	}
	ast_unroll_stars(int nodeID){
		struct ast_node *N1 = ast_get_node(nodeID);
		printf("unroll [%s] ",N1->token.type);
		ast_unroll_stars_helper(ast_get_child_id(N1,0));
		printf(" unroll done\n");
	} */
	# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do{		\
	 printf("YYLOC_DEFAULT(%d)\n",N);		\
    if(N) /*if (YYID (N))*/							\
	{								\
								\
		(Current).null = 1;	\
		(Current).first_line   = 0;\
		(Current).first_column = 0;\
		(Current).last_line    = 0;\
		(Current).last_column  = 0;\
		(Current).start        = 0;\
		(Current).end          = 0;\
		(Current).filename = 0;\
		int i = 0;	\
		int hasFirst = 0;	\
		for(i = 1; i <= N; i++){	\
			printf("RHS(%d).null=%d\n",i,YYRHSLOC(Rhs,i).null);	\
			/*checkValidLoc(YYRHSLOC(Rhs,i),"Rhs",i);*/ \
			if(!YYRHSLOC(Rhs,i).null){	\
				(Current).null = 0;	\
				if(!hasFirst){	\
					hasFirst = 1;	\
					(Current).first_line = YYRHSLOC(Rhs,i).first_line;	\
					(Current).first_column = YYRHSLOC(Rhs,i).first_column;	\
					(Current).start = YYRHSLOC(Rhs,i).start;	\
					(Current).filename = YYRHSLOC(Rhs,i).filename;\
				}	\
				(Current).last_line = YYRHSLOC(Rhs,i).last_line;	\
				(Current).last_column = YYRHSLOC(Rhs,i).last_column;	\
				(Current).end = YYRHSLOC(Rhs,i).end;	\
			}	\
		}	\
		/*checkValidLoc((Current), "LHS",0);*/ \
		printf("LHS.null=%d\n",(Current).null);\
	}								\
      else	/* this is what happens when we have an empty production */ \
	{								\
	  (Current).null = 1;	\
	  (Current).first_line   = 0;\
	  (Current).first_column = 0;\
	  (Current).last_line    = 0;\
	  (Current).last_column  = 0;\
	  (Current).start        = 0;\
	  (Current).end          = 0;\
	}}								\
    while(0)/*while (YYID (0))*/
//	    YYRHSLOC (Rhs, 0).last_column;	
//
/*

	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).start        = YYRHSLOC (Rhs, 1).start;			\
  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).start = (Current).end = (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).end;				\
	  (Current).filename = YYRHSLOC (Rhs, 0).filename;	\
	  */
#line 201 "input/yaccin.y"

	
	extern FILE *yyin;
	FILE *yyin2;

	int main(int argc, char **argv); //moved to main.c
	/* /// moved to semantic/yylloc.c
	void printPos(YYLTYPE pos){
		printf("null: %d\n",pos.null);
		printf("first_line: %d\n",pos.first_line);
		printf("first_column: %d\n",pos.first_column);
		printf("last_line: %d\n", pos.last_line);
		printf("last_column: %d\n", pos.last_column);
		printf("start: %d\n", pos.start);
		printf("end: %d\n", pos.end);
		printf("filename: [%s]\n",pos.filename);
	}


	void printPosErr(YYLTYPE pos){
		fprintf(stderr,"null: %d\n",pos.null);
		fprintf(stderr,"first_line: %d\n",pos.first_line);
		fprintf(stderr,"first_column: %d\n",pos.first_column);
		fprintf(stderr,"last_line: %d\n", pos.last_line);
		fprintf(stderr,"last_column: %d\n", pos.last_column);
		fprintf(stderr,"start: %d\n", pos.start);
		fprintf(stderr,"end: %d\n", pos.end);
		fprintf(stderr,"filename: [%s]\n",pos.filename);
	}
	*/

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"

///  from https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
/// The variable yynerrs contains the number of syntax errors reported so far. Normally this variable is global; 
/// but if you request a pure parser (see A Pure (Reentrant) Parser) then it is a local variable which only the actions can access.
	
	extern YYLTYPE prev_yylloc[3];

	void /*int*/ yyerror(const char *s)
	{	
		YYLTYPE report_loc = prev_yylloc[2];
		if(report_loc.null){report_loc = prev_yylloc[1];}
		if(report_loc.null){report_loc = prev_yylloc[0];}
		if(report_loc.null){report_loc = yylloc;}
		printf("\nyyerror(%s)!\n", s);
		int x1 = report_loc.first_column;
		int y1 = report_loc.first_line;
		int x2 = report_loc.last_column;
		//int y2 = yylloc.last_line;
		const char *f = report_loc.filename;
		int length = x2-x1;
		if(length == 1){length = 0;}
		point_out_error(y1,x1,f,s,length);
		fflush(stdout);
		
		//error("syntax error. yytext: [%s]\n",(char*)yylval);
		//error("[PARSE] Syntax error: %s\n", s);
		fprintf(stderr, "%s[PARSE] %s\n%s", KRED, s, KNRM);
		exit(1); /// we have to exit to get out of an error loop for some reason
		//return 1;
	}

	char *posToString(YYLTYPE pos){
		int start = pos.first_column;
		int end = pos.last_column;
		int len = end-start;
		char *buff = malloc(sizeof(char)*len);
		fseek(yyin2, start, SEEK_SET);
		fgets(buff, len, yyin2);
		return buff;
	}

	ast_node *production(const char* name, int prod_id, const char* value, YYLTYPE pos, ast_node *ch1, ast_node *ch2, ast_node *ch3, ast_node *ch4){
		ast_node *res = ast_node_new(
				ast_token_here(name,prod_id,value,pos),
				ch1? ch2? ch3? ch4?
				 vector2_ptr_ast_node_here_from_list(4, ch1, ch2, ch3, ch4)
				:vector2_ptr_ast_node_here_from_list(3, ch1, ch2, ch3)
				:vector2_ptr_ast_node_here_from_list(2, ch1, ch2)
				:vector2_ptr_ast_node_here_from_list(1, ch1)		
				:vector2_ptr_ast_node_here_from_list(0)			
			);
		printParsed(res);
		return res;
	}

	ast_node *error_production(const char* msg, YYLTYPE pos){
		//yylloc = prev_yylloc[3]; 
		yyerror(msg); 
		return ast_node_new(ast_token_here("error",0,NULL,pos),vector2_ptr_ast_node_here_from_list(0));
	}
	#define STR_ERR_SEMICOLON "Syntax error: expected semicolon ';'"
	#define STR_ERR_END "Syntax error: expected 'end'"
	#define STR_ERR_END_OR_SEMICOLON "Syntax error: expected 'end' or semicolon ';'"

#line 482 "generated/yaccin.tab.c"

#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  18
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   993

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  67
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  26
/* YYNRULES -- Number of rules.  */
#define YYNRULES  89
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  170

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   300


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    42,     2,     2,     2,    36,    30,     2,
      47,    57,    37,    32,    59,    33,    49,    35,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    56,
      65,    27,    64,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    48,     2,    58,    62,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    60,    63,    61,     2,     2,     2,     2,
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
      25,    26,    28,    29,    31,    34,    38,    39,    40,    41,
      43,    44,    45,    46,    50,    51,    52,    53,    54,    55,
      66
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   401,   401,   415,   426,   429,   430,   433,   434,   438,
     439,   440,   441,   444,   445,   448,   449,   451,   452,   453,
     454,   455,   456,   457,   458,   461,   463,   465,   468,   469,
     472,   473,   476,   477,   480,   483,   484,   485,   486,   487,
     490,   491,   494,   495,   498,   499,   502,   503,   506,   509,
     512,   513,   516,   517,   520,   521,   522,   523,   524,   525,
     526,   527,   528,   529,   530,   531,   532,   533,   534,   535,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   549,   550,   551,   552,   553,   554,   555
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "RETURN", "CLASS",
  "ID", "TYPE", "END", "IF", "ELSE", "ELSEIF", "WHILE", "FOR", "INTEGER",
  "INTEGERX", "INTEGERB", "FLOATING", "CHARACTER", "STRING", "EQUAL",
  "NOTEQUAL", "INC", "DEC", "VARARGS", "ASM", "LEQUAL", "GEQUAL", "'='",
  "ASSIGN", "LOGIC", "'&'", "COMPARE", "'+'", "'-'", "ADDSUB", "'/'",
  "'%'", "'*'", "MULDIV", "EXP", "PREOP", "NEG", "'!'", "LNEG", "CAST",
  "DEREF", "REF", "'('", "'['", "'.'", "POSTOP", "CALL", "INDEX", "DOT",
  "BRACELIST", "SUBEXPR", "';'", "')'", "']'", "','", "'{'", "'}'", "'^'",
  "'|'", "'>'", "'<'", "PRENEG", "$accept", "program", "stmt", "stmt_list",
  "stmt_list_ne", "decl_stmt", "decl_stmt_list", "decl_stmt_list_ne",
  "imp_stmt", "asm_stmt", "class_def", "STAR", "ptr_stars", "ptr_stars_ne",
  "typename", "func_def", "var_decl", "var_decl_list", "var_decl_list_ne",
  "if_block", "if_then", "while_loop", "for_loop", "expr_list",
  "expr_list_ne", "expr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-112)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-30)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     169,  -112,    18,   -12,  -112,    26,  -112,  -112,   109,  -112,
      27,  -112,   -23,   340,  -112,  -112,  -112,   -12,  -112,  -112,
     -19,  -112,    28,  -112,  -112,   389,    54,   389,  -112,  -112,
    -112,  -112,  -112,  -112,  -112,  -112,   389,   389,   389,   389,
     389,   389,   318,   389,   878,    29,    32,  -112,   -16,     2,
       5,    14,   878,   408,   399,   399,   399,   878,   399,   399,
      17,   455,   -21,   389,   389,  -112,  -112,   389,   389,   389,
     389,   389,   389,   389,   389,   389,   389,   389,   389,   389,
     389,   389,   389,   -12,   -17,   243,     1,  -112,   389,  -112,
     389,  -112,  -112,    57,    57,    57,    57,   878,   897,   928,
     928,   399,   399,   399,    23,   502,    98,   399,   897,    57,
      57,   389,  -112,   353,    34,    38,    50,    51,  -112,    92,
     135,  -112,  -112,    44,  -112,    12,  -112,  -112,   549,  -112,
     878,   399,  -112,  -112,  -112,   596,   389,   389,   283,    84,
    -112,  -112,  -112,  -112,   243,    80,  -112,  -112,   643,   690,
     389,    72,   124,   389,   193,   243,   737,  -112,  -112,   784,
    -112,   126,   389,   193,  -112,   831,  -112,   243,   127,  -112
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    12,     0,    29,    39,     0,    16,     2,     0,     9,
       0,    10,     0,     0,    27,    31,    32,    28,     1,    15,
      35,    11,     0,    33,    30,     0,    41,     0,    26,    54,
      55,    56,    57,    58,    59,    60,     0,     0,     0,     0,
       0,     0,     0,    51,    37,     0,     0,    43,     0,    40,
       0,    50,    53,     0,    84,    86,    89,    87,    88,    73,
       0,     0,     0,     0,     0,    83,    85,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    51,     0,     0,     0,
       0,     0,     0,    29,    35,     0,     0,    38,     0,    36,
       0,    62,    63,    76,    77,    80,    81,    82,    74,    71,
      70,    67,    69,    68,     0,     0,    65,    66,    75,    78,
      79,    51,    12,     0,     0,     0,     0,     0,     8,     0,
       0,     4,     3,     0,    17,     0,    18,    22,     0,    42,
      52,    72,    64,    61,    20,     0,     0,     0,     0,     0,
      34,     7,    23,    44,     0,     0,    19,    21,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    25,    45,     0,
      46,     0,     0,     0,    48,     0,    47,     0,     0,    49
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -112,  -112,  -111,   -37,  -112,    95,   122,  -112,  -112,  -112,
    -112,   137,   152,  -112,   -22,  -112,   -20,  -112,  -112,  -112,
    -112,  -112,  -112,   -40,  -112,   -25
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     5,   118,   119,   120,   121,     7,     8,   122,   123,
       9,    15,    23,    17,    10,    11,    12,    48,    49,   124,
     125,   126,   127,    50,    51,   128
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      44,    52,    53,    62,    46,    45,    47,     3,    25,   141,
      25,    54,    55,    56,    57,    58,    59,    61,    52,   143,
      60,   144,   145,    13,     4,    14,    18,   150,    26,    27,
     111,    27,    20,    21,    83,    28,   104,    84,    93,    94,
      92,    85,    95,    96,    97,    98,    99,   100,   101,   102,
     103,    52,   105,   106,   107,   108,   109,   110,    45,    29,
       3,    86,    87,   130,    46,   131,   129,    30,    31,    32,
      33,    34,    35,    88,    90,    36,    37,     4,    65,    66,
     132,   136,    67,    68,    38,   137,    52,    39,   135,    71,
      72,    40,    73,    74,    75,     6,    41,   138,   139,   140,
     142,    42,   151,    19,    76,    77,    78,   152,     6,   -13,
       1,   148,   149,     2,    43,     3,   -13,   160,   161,    79,
      80,    81,    82,    67,    68,   156,   166,   153,   159,   157,
     168,   158,     4,   164,   169,    22,   112,   165,   113,     2,
      29,     3,    -5,   114,    -5,    -5,   115,   116,    30,    31,
      32,    33,    34,    35,    24,    16,    36,    37,     4,   117,
      79,    80,    81,    82,     0,    38,     0,     0,    39,   -14,
       1,     0,    40,     2,     0,     3,     0,    41,     0,     0,
       0,     0,    42,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     4,     0,   112,    43,   113,     2,    29,     3,
      -6,   114,    -6,    -6,   115,   116,    30,    31,    32,    33,
      34,    35,     0,     0,    36,    37,     4,   117,     0,     0,
       0,     0,     0,    38,     0,     0,    39,     0,     0,     0,
      40,     0,     0,     0,     0,    41,     0,     0,     0,     0,
      42,     0,     0,     0,   112,     0,   113,     2,    29,     3,
      -6,   114,     0,    43,   115,   116,    30,    31,    32,    33,
      34,    35,     0,     0,    36,    37,     4,   117,     0,     0,
       0,     0,     0,    38,     0,     0,    39,     0,     0,     0,
      40,     0,     0,     0,   112,    41,   113,     2,    29,     3,
      42,   114,     0,     0,   115,   116,    30,    31,    32,    33,
      34,    35,     0,    43,    36,    37,     4,   117,     0,     0,
       0,     0,     0,    38,     0,     0,    39,     0,     0,     0,
      40,     0,    45,    29,     3,    41,     0,     0,     0,     0,
      42,    30,    31,    32,    33,    34,    35,     0,     0,    36,
      37,     1,     0,    43,     2,   -29,     3,   -14,    38,     0,
       0,    39,     0,     0,     0,    40,     0,     0,    29,     0,
      41,     0,     0,     4,     0,    42,    30,    31,    32,    33,
      34,    35,     0,     0,    36,    37,     0,    14,    43,     0,
       0,     0,     0,    38,     0,     0,    39,     0,     0,     0,
      40,     0,     0,     0,    29,    41,     0,     0,     0,     0,
      42,     0,    30,    31,    32,    33,    34,    35,     0,   134,
      36,    37,     0,    43,     0,     0,     0,     0,     0,    38,
      65,    66,    39,     0,    67,    68,    40,    63,    64,    65,
      66,    41,     0,    67,    68,    69,    42,     0,    70,     0,
      71,    72,     0,    73,    74,    75,    76,    77,    78,    43,
       0,     0,     0,     0,     0,    76,    77,    78,     0,     0,
       0,    79,    80,    81,    82,     0,    89,     0,     0,     0,
      79,    80,    81,    82,    63,    64,    65,    66,     0,     0,
      67,    68,    69,     0,     0,    70,     0,    71,    72,     0,
      73,    74,    75,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    76,    77,    78,     0,     0,     0,     0,     0,
       0,     0,    91,     0,     0,     0,     0,    79,    80,    81,
      82,    63,    64,    65,    66,     0,     0,    67,    68,    69,
       0,     0,    70,     0,    71,    72,     0,    73,    74,    75,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    76,
      77,    78,     0,     0,     0,     0,     0,     0,     0,     0,
     133,     0,     0,     0,    79,    80,    81,    82,    63,    64,
      65,    66,     0,     0,    67,    68,    69,     0,     0,    70,
       0,    71,    72,     0,    73,    74,    75,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    76,    77,    78,     0,
       0,     0,     0,     0,     0,   146,     0,     0,     0,     0,
       0,    79,    80,    81,    82,    63,    64,    65,    66,     0,
       0,    67,    68,    69,     0,     0,    70,     0,    71,    72,
       0,    73,    74,    75,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    76,    77,    78,     0,     0,     0,     0,
       0,     0,   147,     0,     0,     0,     0,     0,    79,    80,
      81,    82,    63,    64,    65,    66,     0,     0,    67,    68,
      69,     0,     0,    70,     0,    71,    72,     0,    73,    74,
      75,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      76,    77,    78,     0,     0,     0,     0,     0,     0,     0,
     154,     0,     0,     0,     0,    79,    80,    81,    82,    63,
      64,    65,    66,     0,     0,    67,    68,    69,     0,     0,
      70,     0,    71,    72,     0,    73,    74,    75,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    76,    77,    78,
       0,     0,     0,     0,     0,     0,     0,   155,     0,     0,
       0,     0,    79,    80,    81,    82,    63,    64,    65,    66,
       0,     0,    67,    68,    69,     0,     0,    70,     0,    71,
      72,     0,    73,    74,    75,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    76,    77,    78,     0,     0,     0,
       0,     0,     0,   162,     0,     0,     0,     0,     0,    79,
      80,    81,    82,    63,    64,    65,    66,     0,     0,    67,
      68,    69,     0,     0,    70,     0,    71,    72,     0,    73,
      74,    75,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    76,    77,    78,     0,     0,     0,     0,     0,     0,
       0,   163,     0,     0,     0,     0,    79,    80,    81,    82,
      63,    64,    65,    66,     0,     0,    67,    68,    69,     0,
       0,    70,     0,    71,    72,     0,    73,    74,    75,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    76,    77,
      78,     0,     0,     0,     0,     0,     0,     0,   167,     0,
       0,     0,     0,    79,    80,    81,    82,    63,    64,    65,
      66,     0,     0,    67,    68,    69,     0,     0,    70,     0,
      71,    72,     0,    73,    74,    75,    63,    64,    65,    66,
       0,     0,    67,    68,     0,    76,    77,    78,     0,    71,
      72,     0,    73,    74,    75,     0,     0,     0,     0,     0,
      79,    80,    81,    82,    76,    77,    78,     0,     0,    65,
      66,     0,     0,    67,    68,     0,     0,     0,     0,    79,
      80,    81,    82,    73,    74,    75,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    76,    77,    78,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      79,    80,    81,    82
};

static const yytype_int16 yycheck[] =
{
      25,    26,    27,    43,    26,     4,    26,     6,    27,   120,
      27,    36,    37,    38,    39,    40,    41,    42,    43,     7,
      42,     9,    10,     5,    23,    37,     0,   138,    47,    48,
      47,    48,     5,    56,     5,     7,    76,     5,    63,    64,
      61,    57,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,     4,     5,
       6,    59,    57,    88,    86,    90,    86,    13,    14,    15,
      16,    17,    18,    59,    57,    21,    22,    23,    21,    22,
      57,    47,    25,    26,    30,    47,   111,    33,   113,    32,
      33,    37,    35,    36,    37,     0,    42,    47,    47,     7,
      56,    47,    18,     8,    47,    48,    49,   144,    13,     0,
       1,   136,   137,     4,    60,     6,     7,   154,   155,    62,
      63,    64,    65,    25,    26,   150,   163,    47,   153,    57,
     167,     7,    23,     7,     7,    13,     1,   162,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    17,     3,    21,    22,    23,    24,
      62,    63,    64,    65,    -1,    30,    -1,    -1,    33,     0,
       1,    -1,    37,     4,    -1,     6,    -1,    42,    -1,    -1,
      -1,    -1,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    23,    -1,     1,    60,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    -1,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    33,    -1,    -1,    -1,
      37,    -1,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      47,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
       7,     8,    -1,    60,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    -1,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    33,    -1,    -1,    -1,
      37,    -1,    -1,    -1,     1,    42,     3,     4,     5,     6,
      47,     8,    -1,    -1,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    60,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    33,    -1,    -1,    -1,
      37,    -1,     4,     5,     6,    42,    -1,    -1,    -1,    -1,
      47,    13,    14,    15,    16,    17,    18,    -1,    -1,    21,
      22,     1,    -1,    60,     4,     5,     6,     7,    30,    -1,
      -1,    33,    -1,    -1,    -1,    37,    -1,    -1,     5,    -1,
      42,    -1,    -1,    23,    -1,    47,    13,    14,    15,    16,
      17,    18,    -1,    -1,    21,    22,    -1,    37,    60,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    33,    -1,    -1,    -1,
      37,    -1,    -1,    -1,     5,    42,    -1,    -1,    -1,    -1,
      47,    -1,    13,    14,    15,    16,    17,    18,    -1,    56,
      21,    22,    -1,    60,    -1,    -1,    -1,    -1,    -1,    30,
      21,    22,    33,    -1,    25,    26,    37,    19,    20,    21,
      22,    42,    -1,    25,    26,    27,    47,    -1,    30,    -1,
      32,    33,    -1,    35,    36,    37,    47,    48,    49,    60,
      -1,    -1,    -1,    -1,    -1,    47,    48,    49,    -1,    -1,
      -1,    62,    63,    64,    65,    -1,    58,    -1,    -1,    -1,
      62,    63,    64,    65,    19,    20,    21,    22,    -1,    -1,
      25,    26,    27,    -1,    -1,    30,    -1,    32,    33,    -1,
      35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    49,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    57,    -1,    -1,    -1,    -1,    62,    63,    64,
      65,    19,    20,    21,    22,    -1,    -1,    25,    26,    27,
      -1,    -1,    30,    -1,    32,    33,    -1,    35,    36,    37,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      58,    -1,    -1,    -1,    62,    63,    64,    65,    19,    20,
      21,    22,    -1,    -1,    25,    26,    27,    -1,    -1,    30,
      -1,    32,    33,    -1,    35,    36,    37,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    -1,
      -1,    -1,    -1,    -1,    -1,    56,    -1,    -1,    -1,    -1,
      -1,    62,    63,    64,    65,    19,    20,    21,    22,    -1,
      -1,    25,    26,    27,    -1,    -1,    30,    -1,    32,    33,
      -1,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    48,    49,    -1,    -1,    -1,    -1,
      -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,    62,    63,
      64,    65,    19,    20,    21,    22,    -1,    -1,    25,    26,
      27,    -1,    -1,    30,    -1,    32,    33,    -1,    35,    36,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      57,    -1,    -1,    -1,    -1,    62,    63,    64,    65,    19,
      20,    21,    22,    -1,    -1,    25,    26,    27,    -1,    -1,
      30,    -1,    32,    33,    -1,    35,    36,    37,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,    -1,    -1,
      -1,    -1,    62,    63,    64,    65,    19,    20,    21,    22,
      -1,    -1,    25,    26,    27,    -1,    -1,    30,    -1,    32,
      33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    48,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,    62,
      63,    64,    65,    19,    20,    21,    22,    -1,    -1,    25,
      26,    27,    -1,    -1,    30,    -1,    32,    33,    -1,    35,
      36,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    48,    49,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    57,    -1,    -1,    -1,    -1,    62,    63,    64,    65,
      19,    20,    21,    22,    -1,    -1,    25,    26,    27,    -1,
      -1,    30,    -1,    32,    33,    -1,    35,    36,    37,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      49,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    57,    -1,
      -1,    -1,    -1,    62,    63,    64,    65,    19,    20,    21,
      22,    -1,    -1,    25,    26,    27,    -1,    -1,    30,    -1,
      32,    33,    -1,    35,    36,    37,    19,    20,    21,    22,
      -1,    -1,    25,    26,    -1,    47,    48,    49,    -1,    32,
      33,    -1,    35,    36,    37,    -1,    -1,    -1,    -1,    -1,
      62,    63,    64,    65,    47,    48,    49,    -1,    -1,    21,
      22,    -1,    -1,    25,    26,    -1,    -1,    -1,    -1,    62,
      63,    64,    65,    35,    36,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    49,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      62,    63,    64,    65
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     4,     6,    23,    68,    72,    73,    74,    77,
      81,    82,    83,     5,    37,    78,    79,    80,     0,    72,
       5,    56,    73,    79,    78,    27,    47,    48,     7,     5,
      13,    14,    15,    16,    17,    18,    21,    22,    30,    33,
      37,    42,    47,    60,    92,     4,    81,    83,    84,    85,
      90,    91,    92,    92,    92,    92,    92,    92,    92,    92,
      81,    92,    90,    19,    20,    21,    22,    25,    26,    27,
      30,    32,    33,    35,    36,    37,    47,    48,    49,    62,
      63,    64,    65,     5,     5,    57,    59,    57,    59,    58,
      57,    57,    61,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    90,    92,    92,    92,    92,    92,
      92,    47,     1,     3,     8,    11,    12,    24,    69,    70,
      71,    72,    75,    76,    86,    87,    88,    89,    92,    83,
      92,    92,    57,    58,    56,    92,    47,    47,    47,    47,
       7,    69,    56,     7,     9,    10,    56,    56,    92,    92,
      69,    18,    70,    47,    57,    57,    92,    57,     7,    92,
      70,    70,    56,    57,     7,    92,    70,    57,    70,     7
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    67,    68,    69,    69,    70,    70,    71,    71,    72,
      72,    72,    72,    73,    73,    74,    74,    75,    75,    75,
      75,    75,    75,    75,    75,    76,    77,    78,    79,    79,
      80,    80,    81,    81,    82,    83,    83,    83,    83,    83,
      84,    84,    85,    85,    86,    86,    87,    87,    88,    89,
      90,    90,    91,    91,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     0,     2,     1,     1,
       1,     2,     1,     1,     0,     2,     1,     1,     1,     2,
       2,     3,     1,     2,     1,     4,     4,     1,     1,     0,
       2,     1,     2,     3,     7,     2,     5,     4,     5,     1,
       1,     0,     3,     1,     2,     4,     5,     6,     6,     9,
       1,     0,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     3,     3,     4,     3,     3,     3,     3,     3,
       3,     3,     4,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     2,     2,     2,     2,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: decl_stmt_list  */
#line 402 "input/yaccin.y"
                                                        {	
								// YYLTYPE pos = @$;
								// ast_node *child1 = $1;
								// finalNode = ast_node_new(
								// 	ast_token_here("program",0,NULL,pos),
								// 	vector2_ptr_ast_node_here_from_list
								// 	(1, child1)
								// );
								// printParsed(finalNode);
								finalNode = production("program",0,NULL,(yyloc),yyvsp[0],0,0,0);
							}
#line 2110 "generated/yaccin.tab.c"
    break;

  case 3: /* stmt: imp_stmt  */
#line 416 "input/yaccin.y"
                                                        {
								// ast_node **res = &($$);
								// *res = ast_node_new(
								// 	ast_token_here("stmt",0,NULL,@$),
								// 	vector2_ptr_ast_node_here_from_list
								// 	(1, $1)
								// );
								// printParsed(*res);
								yyval = production("stmt",0,NULL,(yyloc),yyvsp[0],0,0,0);
							}
#line 2125 "generated/yaccin.tab.c"
    break;

  case 4: /* stmt: decl_stmt  */
#line 426 "input/yaccin.y"
                                                {/*printParsed($$);*/	yyval = production("stmt",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2131 "generated/yaccin.tab.c"
    break;

  case 5: /* stmt_list: stmt_list_ne  */
#line 429 "input/yaccin.y"
                                                                        {yyval = production("stmt_list",0,NULL,(yyloc),yyvsp[0],0,0,0); ast_unroll_lists(yyval);}
#line 2137 "generated/yaccin.tab.c"
    break;

  case 6: /* stmt_list: %empty  */
#line 430 "input/yaccin.y"
                                                                {(yyloc).null = 1;	yyval = production("stmt_list",1,"<empty>",nullPos(),0,0,0,0);}
#line 2143 "generated/yaccin.tab.c"
    break;

  case 7: /* stmt_list_ne: stmt_list_ne stmt  */
#line 433 "input/yaccin.y"
                                                                {yyval = production("stmt_list_ne",0,NULL,(yyloc),yyvsp[-1],yyvsp[0],0,0);}
#line 2149 "generated/yaccin.tab.c"
    break;

  case 8: /* stmt_list_ne: stmt  */
#line 434 "input/yaccin.y"
                                                                                        {yyval = production("stmt_list_ne",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2155 "generated/yaccin.tab.c"
    break;

  case 9: /* decl_stmt: class_def  */
#line 438 "input/yaccin.y"
                                                                                        {yyval = production("decl_stmt",0,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2161 "generated/yaccin.tab.c"
    break;

  case 10: /* decl_stmt: func_def  */
#line 439 "input/yaccin.y"
                                                                                        {yyval = production("decl_stmt",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2167 "generated/yaccin.tab.c"
    break;

  case 11: /* decl_stmt: var_decl ';'  */
#line 440 "input/yaccin.y"
                                                                                {yyval = production("decl_stmt",2,NULL,(yyloc),yyvsp[-1],0,0,0);}
#line 2173 "generated/yaccin.tab.c"
    break;

  case 12: /* decl_stmt: error  */
#line 441 "input/yaccin.y"
                                                                                        {yyval = error_production(STR_ERR_END_OR_SEMICOLON, (yyloc));}
#line 2179 "generated/yaccin.tab.c"
    break;

  case 13: /* decl_stmt_list: decl_stmt_list_ne  */
#line 444 "input/yaccin.y"
                                                                {yyval = production("decl_stmt_list",0,NULL,(yyloc),yyvsp[0],0,0,0);ast_unroll_lists(yyval);}
#line 2185 "generated/yaccin.tab.c"
    break;

  case 14: /* decl_stmt_list: %empty  */
#line 445 "input/yaccin.y"
                                                                {(yyloc).null =  1;	yyval = production("decl_stmt_list",1,"<empty>",nullPos(),0,0,0,0);}
#line 2191 "generated/yaccin.tab.c"
    break;

  case 15: /* decl_stmt_list_ne: decl_stmt_list_ne decl_stmt  */
#line 448 "input/yaccin.y"
                                                      {yyval = production("decl_stmt_list_ne",0,NULL,(yyloc),yyvsp[-1],yyvsp[0],0,0);}
#line 2197 "generated/yaccin.tab.c"
    break;

  case 16: /* decl_stmt_list_ne: decl_stmt  */
#line 449 "input/yaccin.y"
                                                                                        {yyval = production("decl_stmt_list_ne",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2203 "generated/yaccin.tab.c"
    break;

  case 17: /* imp_stmt: if_block  */
#line 451 "input/yaccin.y"
                                                                                {yyval = production("imp_stmt",0,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2209 "generated/yaccin.tab.c"
    break;

  case 18: /* imp_stmt: while_loop  */
#line 452 "input/yaccin.y"
                                                                                        {yyval = production("imp_stmt",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2215 "generated/yaccin.tab.c"
    break;

  case 19: /* imp_stmt: expr ';'  */
#line 453 "input/yaccin.y"
                                                                                        {yyval = production("imp_stmt",2,NULL,(yyloc),yyvsp[-1],0,0,0);}
#line 2221 "generated/yaccin.tab.c"
    break;

  case 20: /* imp_stmt: RETURN ';'  */
#line 454 "input/yaccin.y"
                                                                                        {yyval = production("imp_stmt",3,NULL,(yyloc),0,0,0,0);}
#line 2227 "generated/yaccin.tab.c"
    break;

  case 21: /* imp_stmt: RETURN expr ';'  */
#line 455 "input/yaccin.y"
                                                                                {yyval = production("imp_stmt",4,NULL,(yyloc),yyvsp[-1],0,0,0);}
#line 2233 "generated/yaccin.tab.c"
    break;

  case 22: /* imp_stmt: for_loop  */
#line 456 "input/yaccin.y"
                                                                                        {yyval = production("imp_stmt",5,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2239 "generated/yaccin.tab.c"
    break;

  case 23: /* imp_stmt: asm_stmt ';'  */
#line 457 "input/yaccin.y"
                                                                                {yyval = production("imp_stmt",6,NULL,(yyloc),yyvsp[-1],0,0,0);}
#line 2245 "generated/yaccin.tab.c"
    break;

  case 24: /* imp_stmt: error  */
#line 458 "input/yaccin.y"
                                                                                        {yyval = error_production(STR_ERR_END_OR_SEMICOLON, (yyloc));}
#line 2251 "generated/yaccin.tab.c"
    break;

  case 25: /* asm_stmt: ASM '(' STRING ')'  */
#line 461 "input/yaccin.y"
                                                                        {yyval = production("asm_stmt",0,yyvsp[-1]->token.value,(yyloc),0,0,0,0);}
#line 2257 "generated/yaccin.tab.c"
    break;

  case 26: /* class_def: CLASS ID decl_stmt_list END  */
#line 463 "input/yaccin.y"
                                                                {const char *id_str = yyvsp[-2]->token.value; yyval = production("class_def",0,id_str,(yyloc),yyvsp[-2],yyvsp[-1],0,0);}
#line 2263 "generated/yaccin.tab.c"
    break;

  case 27: /* STAR: '*'  */
#line 465 "input/yaccin.y"
                                                                                {yyval = production("stars",0,"*",(yyloc),0,0,0,0);}
#line 2269 "generated/yaccin.tab.c"
    break;

  case 28: /* ptr_stars: ptr_stars_ne  */
#line 468 "input/yaccin.y"
                                                                        {yyval = production("ptr_stars",0,NULL,(yyloc),yyvsp[0],0,0,0); ast_unroll_lists(yyval);}
#line 2275 "generated/yaccin.tab.c"
    break;

  case 29: /* ptr_stars: %empty  */
#line 469 "input/yaccin.y"
                                                                                                {yyval = production("ptr_stars",1,"<empty>",nullPos(),0,0,0,0);}
#line 2281 "generated/yaccin.tab.c"
    break;

  case 30: /* ptr_stars_ne: ptr_stars_ne STAR  */
#line 472 "input/yaccin.y"
                                                                {yyval = production("ptr_stars_ne",0,NULL,(yyloc),yyvsp[-1],yyvsp[0],0,0);}
#line 2287 "generated/yaccin.tab.c"
    break;

  case 31: /* ptr_stars_ne: STAR  */
#line 473 "input/yaccin.y"
                                                                                        {yyval = production("ptr_stars_ne",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2293 "generated/yaccin.tab.c"
    break;

  case 32: /* typename: TYPE ptr_stars  */
#line 476 "input/yaccin.y"
                                                                        {const char *type_str = yyvsp[-1]->token.value; yyval = production("typename",0,type_str,(yyloc),yyvsp[-1],yyvsp[0],0,0);}
#line 2299 "generated/yaccin.tab.c"
    break;

  case 33: /* typename: CLASS ID ptr_stars  */
#line 477 "input/yaccin.y"
                                                                                {const char *type_str = yyvsp[-1]->token.value; yyval = production("typename",1,type_str,(yyloc),yyvsp[-1],yyvsp[0],0,0);}
#line 2305 "generated/yaccin.tab.c"
    break;

  case 34: /* func_def: typename ID '(' var_decl_list ')' stmt_list END  */
#line 481 "input/yaccin.y"
                                                                                                {const char *id_str = yyvsp[-5]->token.value; yyval = production("func_def",0,id_str,(yyloc),yyvsp[-6],yyvsp[-5],yyvsp[-3],yyvsp[-1]);}
#line 2311 "generated/yaccin.tab.c"
    break;

  case 35: /* var_decl: typename ID  */
#line 483 "input/yaccin.y"
                                                                                {const char *id_str = yyvsp[0]->token.value; yyval = production("var_decl",0,id_str,(yyloc),yyvsp[-1],yyvsp[0],0,0);}
#line 2317 "generated/yaccin.tab.c"
    break;

  case 36: /* var_decl: typename ID '[' expr ']'  */
#line 484 "input/yaccin.y"
                                                                        {const char *id_str = yyvsp[-3]->token.value; yyval = production("var_decl",1,id_str,(yyloc),yyvsp[-4],yyvsp[-3],yyvsp[-1],0);}
#line 2323 "generated/yaccin.tab.c"
    break;

  case 37: /* var_decl: typename ID '=' expr  */
#line 485 "input/yaccin.y"
                                                                        {const char *id_str = yyvsp[-2]->token.value; yyval = production("var_decl_assign",0,id_str,(yyloc),yyvsp[-3],yyvsp[-2],yyvsp[0],0);}
#line 2329 "generated/yaccin.tab.c"
    break;

  case 38: /* var_decl: typename ID '(' expr_list ')'  */
#line 486 "input/yaccin.y"
                                                                {const char *id_str = yyvsp[-3]->token.value; yyval = production("var_decl_constructor",0,id_str,(yyloc),yyvsp[-4],yyvsp[-3],yyvsp[-1],0);}
#line 2335 "generated/yaccin.tab.c"
    break;

  case 39: /* var_decl: VARARGS  */
#line 487 "input/yaccin.y"
                                                                                        {yyval = production("var_decl_varargs",0,0,(yyloc),0,0,0,0);}
#line 2341 "generated/yaccin.tab.c"
    break;

  case 40: /* var_decl_list: var_decl_list_ne  */
#line 490 "input/yaccin.y"
                                                                                                {yyval = production("var_decl_list",0,NULL,(yyloc),yyvsp[0],0,0,0); ast_unroll_lists(yyval);}
#line 2347 "generated/yaccin.tab.c"
    break;

  case 41: /* var_decl_list: %empty  */
#line 491 "input/yaccin.y"
                                                                                                                                {yyval = production ("var_decl_list",1,"<empty>",nullPos(),0,0,0,0);}
#line 2353 "generated/yaccin.tab.c"
    break;

  case 42: /* var_decl_list_ne: var_decl_list_ne ',' var_decl  */
#line 494 "input/yaccin.y"
                                                                                {yyval = production("var_decl_list_ne",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2359 "generated/yaccin.tab.c"
    break;

  case 43: /* var_decl_list_ne: var_decl  */
#line 495 "input/yaccin.y"
                                                                                                                        {yyval = production("var_decl_list_ne",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2365 "generated/yaccin.tab.c"
    break;

  case 44: /* if_block: if_then END  */
#line 498 "input/yaccin.y"
                                                                                                                {yyval = production("if_block",0,NULL,(yyloc),yyvsp[-1],0,0,0);}
#line 2371 "generated/yaccin.tab.c"
    break;

  case 45: /* if_block: if_then ELSE stmt_list END  */
#line 499 "input/yaccin.y"
                                                                                                        {yyval = production("if_block",1,NULL,(yyloc),yyvsp[-3],yyvsp[-1],0,0);}
#line 2377 "generated/yaccin.tab.c"
    break;

  case 46: /* if_then: IF '(' expr ')' stmt_list  */
#line 502 "input/yaccin.y"
                                                                                                {yyval = production("if_then",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2383 "generated/yaccin.tab.c"
    break;

  case 47: /* if_then: if_then ELSEIF '(' expr ')' stmt_list  */
#line 503 "input/yaccin.y"
                                                                                        {yyval = production("if_then",2,NULL,(yyloc),yyvsp[-5],yyvsp[-2],yyvsp[0],0);}
#line 2389 "generated/yaccin.tab.c"
    break;

  case 48: /* while_loop: WHILE '(' expr ')' stmt_list END  */
#line 506 "input/yaccin.y"
                                                                                        {yyval = production("while_loop",0,NULL,(yyloc),yyvsp[-3],yyvsp[-1],0,0);}
#line 2395 "generated/yaccin.tab.c"
    break;

  case 49: /* for_loop: FOR '(' stmt expr ';' expr ')' stmt_list END  */
#line 509 "input/yaccin.y"
                                                                        {yyval = production("for_loop",0,NULL,(yyloc),yyvsp[-6],yyvsp[-5],yyvsp[-3],yyvsp[-1]);}
#line 2401 "generated/yaccin.tab.c"
    break;

  case 50: /* expr_list: expr_list_ne  */
#line 512 "input/yaccin.y"
                                                                        {yyval = production("expr_list",0,NULL,(yyloc),yyvsp[0],0,0,0);ast_unroll_lists(yyval);}
#line 2407 "generated/yaccin.tab.c"
    break;

  case 51: /* expr_list: %empty  */
#line 513 "input/yaccin.y"
                                                                {(yyloc).null = 1; 	yyval = production("expr_list",1,"<empty>",nullPos(),0,0,0,0);}
#line 2413 "generated/yaccin.tab.c"
    break;

  case 52: /* expr_list_ne: expr_list_ne ',' expr  */
#line 516 "input/yaccin.y"
                                                        {yyval = production("expr_list_ne",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2419 "generated/yaccin.tab.c"
    break;

  case 53: /* expr_list_ne: expr  */
#line 517 "input/yaccin.y"
                                                                                        {yyval = production("expr_list_ne",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2425 "generated/yaccin.tab.c"
    break;

  case 54: /* expr: ID  */
#line 520 "input/yaccin.y"
                                                                                        {yyval = production("expr_id",0,yyvsp[0]->token.value,(yyloc),yylval,0,0,0);}
#line 2431 "generated/yaccin.tab.c"
    break;

  case 55: /* expr: INTEGER  */
#line 521 "input/yaccin.y"
                                                                                        {yyval = production("expr_const",0,yyvsp[0]->token.value,(yyloc),yylval,0,0,0);}
#line 2437 "generated/yaccin.tab.c"
    break;

  case 56: /* expr: INTEGERX  */
#line 522 "input/yaccin.y"
                                                                                        {yyval = production("expr_const",1,yyvsp[0]->token.value,(yyloc),yylval,0,0,0);}
#line 2443 "generated/yaccin.tab.c"
    break;

  case 57: /* expr: INTEGERB  */
#line 523 "input/yaccin.y"
                                                                                        {yyval = production("expr_const",2,yyvsp[0]->token.value,(yyloc),yylval,0,0,0);}
#line 2449 "generated/yaccin.tab.c"
    break;

  case 58: /* expr: FLOATING  */
#line 524 "input/yaccin.y"
                                                                                        {yyval = production("expr_const",3,yyvsp[0]->token.value,(yyloc),yylval,0,0,0);}
#line 2455 "generated/yaccin.tab.c"
    break;

  case 59: /* expr: CHARACTER  */
#line 525 "input/yaccin.y"
                                                                                        {yyval = production("expr_const",4,yyvsp[0]->token.value,(yyloc),yylval,0,0,0);}
#line 2461 "generated/yaccin.tab.c"
    break;

  case 60: /* expr: STRING  */
#line 526 "input/yaccin.y"
                                                                                        {yyval = production("expr_const",5,yyvsp[0]->token.value,(yyloc),yylval,0,0,0);}
#line 2467 "generated/yaccin.tab.c"
    break;

  case 61: /* expr: expr '[' expr ']'  */
#line 527 "input/yaccin.y"
                                                                        {yyval = production("expr_index",0,NULL,(yyloc),yyvsp[-3],yyvsp[-1],0,0);}
#line 2473 "generated/yaccin.tab.c"
    break;

  case 62: /* expr: '(' expr ')'  */
#line 528 "input/yaccin.y"
                                                                {yyval = production("expr_subexpr",0,NULL,(yyloc),yyvsp[-1],0,0,0);}
#line 2479 "generated/yaccin.tab.c"
    break;

  case 63: /* expr: '{' expr_list '}'  */
#line 529 "input/yaccin.y"
                                                                {yyval = production("expr_braced_list",0,NULL,(yyloc),yyvsp[-1],0,0,0);}
#line 2485 "generated/yaccin.tab.c"
    break;

  case 64: /* expr: expr '(' expr_list ')'  */
#line 530 "input/yaccin.y"
                                                                {yyval = production("expr_call",0,NULL,(yyloc),yyvsp[-3],yyvsp[-1],0,0);}
#line 2491 "generated/yaccin.tab.c"
    break;

  case 65: /* expr: expr '.' expr  */
#line 531 "input/yaccin.y"
                                                                        {yyval = production("expr_.",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2497 "generated/yaccin.tab.c"
    break;

  case 66: /* expr: expr '^' expr  */
#line 532 "input/yaccin.y"
                                                                        {yyval = production("expr_^",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2503 "generated/yaccin.tab.c"
    break;

  case 67: /* expr: expr '/' expr  */
#line 533 "input/yaccin.y"
                                                                {yyval = production("expr_/",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2509 "generated/yaccin.tab.c"
    break;

  case 68: /* expr: expr '*' expr  */
#line 534 "input/yaccin.y"
                                                                {yyval = production("expr_*",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2515 "generated/yaccin.tab.c"
    break;

  case 69: /* expr: expr '%' expr  */
#line 535 "input/yaccin.y"
                                                                {yyval = production("expr_%",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2521 "generated/yaccin.tab.c"
    break;

  case 70: /* expr: expr '-' expr  */
#line 536 "input/yaccin.y"
                                                                {yyval = production("expr_-",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2527 "generated/yaccin.tab.c"
    break;

  case 71: /* expr: expr '+' expr  */
#line 537 "input/yaccin.y"
                                                                {yyval = production("expr_+",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2533 "generated/yaccin.tab.c"
    break;

  case 72: /* expr: '(' typename ')' expr  */
#line 538 "input/yaccin.y"
                                                                {yyval = production("expr_cast",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2539 "generated/yaccin.tab.c"
    break;

  case 73: /* expr: '!' expr  */
#line 539 "input/yaccin.y"
                                                                                {yyval = production("expr_not",0,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2545 "generated/yaccin.tab.c"
    break;

  case 74: /* expr: expr '&' expr  */
#line 540 "input/yaccin.y"
                                                                        {yyval = production("expr_and",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2551 "generated/yaccin.tab.c"
    break;

  case 75: /* expr: expr '|' expr  */
#line 541 "input/yaccin.y"
                                                                        {yyval = production("expr_or",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2557 "generated/yaccin.tab.c"
    break;

  case 76: /* expr: expr EQUAL expr  */
#line 542 "input/yaccin.y"
                                                                {yyval = production("expr_==",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2563 "generated/yaccin.tab.c"
    break;

  case 77: /* expr: expr NOTEQUAL expr  */
#line 543 "input/yaccin.y"
                                                                {yyval = production("expr_!=",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2569 "generated/yaccin.tab.c"
    break;

  case 78: /* expr: expr '>' expr  */
#line 544 "input/yaccin.y"
                                                                {yyval = production("expr_>",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2575 "generated/yaccin.tab.c"
    break;

  case 79: /* expr: expr '<' expr  */
#line 545 "input/yaccin.y"
                                                                {yyval = production("expr_<",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2581 "generated/yaccin.tab.c"
    break;

  case 80: /* expr: expr LEQUAL expr  */
#line 546 "input/yaccin.y"
                                                                {yyval = production("expr_<=",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2587 "generated/yaccin.tab.c"
    break;

  case 81: /* expr: expr GEQUAL expr  */
#line 547 "input/yaccin.y"
                                                                {yyval = production("expr_>=",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2593 "generated/yaccin.tab.c"
    break;

  case 82: /* expr: expr '=' expr  */
#line 548 "input/yaccin.y"
                                                                {yyval = production("expr_=",0,NULL,(yyloc),yyvsp[-2],yyvsp[0],0,0);}
#line 2599 "generated/yaccin.tab.c"
    break;

  case 83: /* expr: expr INC  */
#line 549 "input/yaccin.y"
                                                                        {yyval = production("expr_++",0,NULL,(yyloc),yyvsp[-1],0,0,0);}
#line 2605 "generated/yaccin.tab.c"
    break;

  case 84: /* expr: INC expr  */
#line 550 "input/yaccin.y"
                                                                                {yyval = production("expr_++",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2611 "generated/yaccin.tab.c"
    break;

  case 85: /* expr: expr DEC  */
#line 551 "input/yaccin.y"
                                                                        {yyval = production("expr_--",0,NULL,(yyloc),yyvsp[-1],0,0,0);}
#line 2617 "generated/yaccin.tab.c"
    break;

  case 86: /* expr: DEC expr  */
#line 552 "input/yaccin.y"
                                                                                {yyval = production("expr_--",1,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2623 "generated/yaccin.tab.c"
    break;

  case 87: /* expr: '-' expr  */
#line 553 "input/yaccin.y"
                                                                        {yyval = production("expr_neg",0,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2629 "generated/yaccin.tab.c"
    break;

  case 88: /* expr: '*' expr  */
#line 554 "input/yaccin.y"
                                                                                {yyval = production("expr_deref",0,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2635 "generated/yaccin.tab.c"
    break;

  case 89: /* expr: '&' expr  */
#line 555 "input/yaccin.y"
                                                                                {yyval = production("expr_ref",0,NULL,(yyloc),yyvsp[0],0,0,0);}
#line 2641 "generated/yaccin.tab.c"
    break;


#line 2645 "generated/yaccin.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 558 "input/yaccin.y"


