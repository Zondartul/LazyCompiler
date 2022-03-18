%{
	
%}
%code requires {
  //#define YYLTYPE YYLTYPE
  #include "globals.h"
  #include "types/ast_node.h"
  
	//ast_node.h not fully included yet due to shenanigans
	typedef struct ast_node ast_node;
	
	ast_node *finalNode;
	int yyerror(const char *s);
}
%code provides{
char *posToString(YYLTYPE pos);
char *get_source_text2(YYLTYPE pos);
}	
%code {
	#include "globals.h"
  #include "AST.h"
  #include "semantic.h"
  #include "codegen.h"
	#include "vector2.h"
	#include "types/ast_node.h"
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
}
%token RETURN CLASS ID TYPE END IF ELSE ELSEIF WHILE FOR INTEGER INTEGERX INTEGERB FLOATING CHARACTER STRING EQUAL NOTEQUAL INC DEC
/*%glr-parser*/
%debug
%error-verbose
%left '='
%left '|' '&'
%left EQUAL NOTEQUAL '<' '>'
%left '+' '-'
%left '^' '/' '*'
%left '.' 
%precedence '[' //']'
%precedence '(' //')' 
%precedence PREDEREF PREREF '!'
%precedence PREINC PREDEC 
%precedence PRENEG
//%precedence POSTINC POSTDEC
%%

program :	decl_stmt_list	//{finalNode = make_node((struct ast_token){"program",0,0,@$},1,$1); }
							{	
								YYLTYPE pos = @$;
								ast_node *child1 = $1;
								finalNode = ast_node_new(
									ast_token_here("program",0,NULL,pos),
									vector2_ptr_ast_node_here_from_list
									(1, child1)
								);
								printParsed(finalNode);
							}		
		;

stmt	:	imp_stmt		//{$$ = (int)make_node((struct ast_token){"stmt",0,0,@$},1,$1);}
							{
								ast_node **res = &($$);
								*res = ast_node_new(
									ast_token_here("stmt",0,NULL,@$),
									vector2_ptr_ast_node_here_from_list
									(1, $1)
								);
								printParsed(*res);
							}
		|	decl_stmt		//{$$ = (int)make_node((struct ast_token){"stmt",1,0,@$},1,$1);}
							{
								$$ = ast_node_new(
									ast_token_here("stmt",1,NULL,@$),
									vector2_ptr_ast_node_here_from_list
									(1, $1)
								);
								printParsed($$);
							}
		;
		
stmt_list	: stmt_list_ne		//{$$ = (int)make_node((struct ast_token){"stmt_list",0,0,@$},1,$1); ast_unroll_lists($$);}
								{
									$$ = ast_node_new(
										ast_token_here("stmt_list",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
									ast_unroll_lists($$);
								}
			|					//{@$.null = 1;$$ = (int)make_node((struct ast_token){"stmt_list",1,"<empty>",@$},0);}
								{
									YYLTYPE *pos = &(@$);
									pos->null = 1;
									
									$$ = ast_node_new(
										ast_token_here("stmt_list",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed($$);
								} %empty
			;
		
stmt_list_ne	: stmt_list_ne stmt	//{$$ = (int)make_node((struct ast_token){"stmt_list_ne",0,0,@$},2,$1,$2);}
									{
										$$ = ast_node_new(
											ast_token_here("stmt_list_ne",0,NULL,@$),
											vector2_ptr_ast_node_here_from_list
											(2, $1, $2)
										);
										printParsed($$);
									}
				| stmt				//{$$ = (int)make_node((struct ast_token){"stmt_list_ne",1,0,@$},1,$1);}
									{
										$$ = ast_node_new(
											ast_token_here("stmt_list_ne",1,NULL,@$),
											vector2_ptr_ast_node_here_from_list
											(1, $1)
										);
										printParsed($$);
									}
				;
		
decl_stmt	:	class_def		//{$$ = (int)make_node((struct ast_token){"decl_stmt",0,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("decl_stmt",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
			|	func_def		//{$$ = (int)make_node((struct ast_token){"decl_stmt",1,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("decl_stmt",1,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
			|	var_decl ';'	//{$$ = (int)make_node((struct ast_token){"decl_stmt",2,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("decl_stmt",2,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
			;
			
decl_stmt_list	:	decl_stmt_list_ne	//{$$ = (int)make_node((struct ast_token){"decl_stmt_list",0,0,@$},1,$1); ast_unroll_lists($$);}
								{
									$$ = ast_node_new(
										ast_token_here("decl_stmt_list",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
									ast_unroll_lists($$);
								}
				|						//{@$.null = 1;$$ = (int)make_node((struct ast_token){"decl_stmt_list",1,"<empty>",@$},0);}
								{
									YYLTYPE *pos = &(@$);
									pos->null = 1;
									
									$$ = ast_node_new(
										ast_token_here("decl_stmt_list",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed($$);
								} %empty
				;

decl_stmt_list_ne	: decl_stmt_list_ne decl_stmt   //{$$ = (int)make_node((struct ast_token){"decl_stmt_list_ne",0,0,@$},2,$1,$2);}
								{
									$$ = ast_node_new(
										ast_token_here("decl_stmt_list_ne",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(2, $1, $2)
									);
									printParsed($$);
								}
					| decl_stmt						//{$$ = (int)make_node((struct ast_token){"decl_stmt_list_ne",1,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("decl_stmt_list_ne",1,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
					;
				
imp_stmt	:	if_block					//{$$ = (int)make_node((struct ast_token){"imp_stmt",0,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("imp_stmt",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
			|	while_loop					//{$$ = (int)make_node((struct ast_token){"imp_stmt",1,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("imp_stmt",1,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
			|	expr ';'					//{$$ = (int)make_node((struct ast_token){"imp_stmt",2,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("imp_stmt",2,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
			|	RETURN ';'					//{$$ = (int)make_node((struct ast_token){"imp_stmt",3,0,@$},0);}
								{
									$$ = ast_node_new(
										ast_token_here("imp_stmt",3,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed($$);
								}
			|	RETURN expr ';'				//{$$ = (int)make_node((struct ast_token){"imp_stmt",4,0,@$},1,$2);}
								{
									$$ = ast_node_new(
										ast_token_here("imp_stmt",4,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $2)
									);
									printParsed($$);
								}
			|	for_loop					//{$$ = (int)make_node((struct ast_token){"imp_stmt",5,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("imp_stmt",5,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
			;
			
class_def	:	CLASS ID decl_stmt_list END	//{printf("FUNC_DEF ID = [%s]\n",(char*)$2);$$ = (int)make_node((struct ast_token){"class_def",0,(char*)$2,@$},1,$3);}
								{
									const char *id_str = $2->token.value;
									printf("FUNC_DEF ID = [%s]\n",id_str);//(char*)$2);
									$$ = ast_node_new(
										ast_token_here("class_def",0,id_str, @$),
										vector2_ptr_ast_node_here_from_list
										(2, $2, $3)
									);
									printParsed($$);
								}

STAR		:   '*'					//{$$ = (int)make_node((struct ast_token){"stars",0,"*",@$},0);}
								{
									$$ = ast_node_new(
										ast_token_here("stars",0,"*",@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed($$);
								}
			;

ptr_stars	:	ptr_stars_ne		//{$$ = (int)make_node((struct ast_token){"ptr_stars",0,0,@$},1,$1); ast_unroll_lists($$);}
								{
									$$ = ast_node_new(
										ast_token_here("ptr_stars",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
									ast_unroll_lists($$);
								}
			|						//{@$.null = 1;$$ = (int)make_node((struct ast_token){"ptr_stars",1,"<empty>",@$},0);}
								{
									$$ = ast_node_new(
										ast_token_here("ptr_stars",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed($$);
								}	%empty
			;

ptr_stars_ne:	ptr_stars_ne STAR	//{$$ = (int)make_node((struct ast_token){"ptr_stars_ne",0,0,@$},2,$1,$2);}
								{
									$$ = ast_node_new(
										ast_token_here("ptr_stars_ne",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(2, $1, $2)
									);
									printParsed($$);
								}
			|	STAR				//{$$ = (int)make_node((struct ast_token){"ptr_stars_ne",1,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("ptr_stars_ne",1,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
			;

typename	:	TYPE	 ptr_stars	//{$$ = (int)make_node((struct ast_token){"typename",0,(char*)$1,@$},1,$2);}
								{
									const char *type_str = $1->token.value;
									$$ = ast_node_new(
										ast_token_here("typename",0,type_str, @$),
										vector2_ptr_ast_node_here_from_list
										(2, $1, $2)
									);
									printParsed($$);
								}
			|	CLASS ID ptr_stars	//{$$ = (int)make_node((struct ast_token){"typename",1,(char*)$2,@$},1,$3);}
								{
									const char *type_str = $2->token.value;
									$$ = ast_node_new(
										ast_token_here("typename",1,type_str, @$),
										vector2_ptr_ast_node_here_from_list
										(2, $2, $3)
									);
									printParsed($$);
								}
			;

func_def	:	typename ID '('	var_decl_list ')' stmt_list END	//{printf("FUNC_DEF ID = [%s]\n",(char*)$2);$$ = (int)make_node((struct ast_token){"func_def",0,(char*)$2,@$},3,$1,$4,$6);}
								{
									const char *id_str = $2->token.value;
									printf("FUNC_DEF ID = [%s]\n", id_str);
									$$ = ast_node_new(
										ast_token_here("func_def", 0, id_str, @$),
										vector2_ptr_ast_node_here_from_list
										(4, $1, $2, $4, $6)
									);
									printParsed($$);
								}

var_decl	:	typename ID						//{$$ = (int)make_node((struct ast_token){"var_decl",0,(char*)$2,@$},1,$1);}
								{
									const char *id_str = $2->token.value;
									$$ = ast_node_new(
										ast_token_here("var_decl",0,id_str, @$),
										vector2_ptr_ast_node_here_from_list
										(2, $1, $2)
									);
									printParsed($$);
								}
			|	typename ID '[' expr ']'		//{$$ = (int)make_node((struct ast_token){"var_decl",1,(char*)$2,@$},2,$1,$4);}
								{
									const char *id_str = $2->token.value;
									$$ = ast_node_new(
										ast_token_here("var_decl",1,id_str, @$),
										vector2_ptr_ast_node_here_from_list
										(3, $1, $2, $4)
									);
									printParsed($$);
								}
			|	typename ID '=' expr			//{$$ = (int)make_node((struct ast_token){"var_decl_assign",0,(char*)$2,@$},2,$1,$4);}//fix_decl_assign($$);}
								{
									const char *id_str = $2->token.value;
									$$ = ast_node_new(
										ast_token_here("var_decl_assign",0,id_str,@$),
										vector2_ptr_ast_node_here_from_list
										(3, $1, $2, $4)
									);
									printParsed($$);
								}
			;

var_decl_list	:	var_decl_list_ne			//{$$ = (int)make_node((struct ast_token){"var_decl_list",0,0,@$},1,$1); ast_unroll_lists($$);}
								{
									$$ = ast_node_new(
										ast_token_here("var_decl_list",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
									ast_unroll_lists($$);
								}
				|								//{$$ = (int)make_node((struct ast_token){"var_decl_list",1,"<empty>",@$},0);}
								{
									$$ = ast_node_new(
										ast_token_here("var_decl_list",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);
									printParsed($$);
								} %empty //inform Bison/YACC that this rule was intentionally left empty
				;
				
var_decl_list_ne	:	var_decl_list_ne ',' var_decl	//{$$ = (int)make_node((struct ast_token){"var_decl_list_ne",0,0,@$},2,$1,$3);}
								{
									$$ = ast_node_new(
										ast_token_here("var_decl_list_ne",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(2, $1, $3)
									);
									printParsed($$);
								}
					|	var_decl						//{$$ = (int)make_node((struct ast_token){"var_decl_list_ne",1,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("var_decl_list_ne",1,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
					;
				
if_block	:	if_then END										//{$$ = (int)make_node((struct ast_token){"if_block",0,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("if_block",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
			|	if_then ELSE stmt_list END						//{$$ = (int)make_node((struct ast_token){"if_block",1,0,@$},2,$1,$3);}
								{
									$$ = ast_node_new(
										ast_token_here("if_block",1,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(2, $1, $3)
									);
									printParsed($$);
								}
			;

if_then		:	IF '(' expr ')' stmt_list			//{$$ = (int)make_node((struct ast_token){"if_then",0,0,@$},2,$3,$5);}
								{
									$$ = ast_node_new(
										ast_token_here("if_then",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(2, $3, $5)
									);
									printParsed($$);
								}
			|	if_then ELSEIF '(' expr ')' stmt_list	//{$$ = (int)make_node((struct ast_token){"if_then",2,0,@$},3,$1,$4,$6);}
								{
									$$ = ast_node_new(
										ast_token_here("if_then",2,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(3, $1, $4, $6)
									);
									printParsed($$);
								}
			;
			
		/* 	if_block	:	if_start ELSE stmt_list END	{$$ = (int)make_node((struct ast_token){"if_block",0,0},2,$1,$3);}
		//	|	if_start ELSE if_block		{$$ = (int)make_node((struct ast_token){"if_block",1,0},2,$1,$2);}
			|	if_start END				{$$ = (int)make_node((struct ast_token){"if_block",2,0},1,$1);}
			;
			
if_start	:	IF '(' expr ')' THEN stmt_list 	{$$ = (int)make_node((struct ast_token){"if_start",0,0},2,$3,$6);}
			;
			 */
while_loop	:	WHILE '(' expr ')' stmt_list END	//{$$ = (int)make_node((struct ast_token){"while_loop",0,0,@$},2,$3,$5);}
								{
									$$ = ast_node_new(
										ast_token_here("while_loop",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(2, $3, $5)
									);
									printParsed($$);
								}
			;

for_loop	:	FOR '(' stmt expr ';' expr ')' stmt_list END	//{$$ = (int)make_node((struct ast_token){"for_loop",0,0,@$},4,$3,$4,$6,$8);}
								{
									$$ = ast_node_new(
										ast_token_here("for_loop",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(4, $3, $4, $6, $8)
									);
									printParsed($$);
								}
			;
			
expr_list	:	expr_list_ne	//{$$ = (int)make_node((struct ast_token){"expr_list",0,0,@$},1,$1);ast_unroll_lists($$);}
								{
									$$ = ast_node_new(
										ast_token_here("expr_list",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
									ast_unroll_lists($$);
								}
			|					//{@$.null = 1;$$ = (int)make_node((struct ast_token){"expr_list",1,"<empty>",@$},0);}
								{
									YYLTYPE *pos = &(@$);
									pos->null = 1;
									
									$$ = ast_node_new(
										ast_token_here("expr_list",1,"<empty>",nullPos()),//@$),
										vector2_ptr_ast_node_here_from_list
										(0)
									);	
									printParsed($$);
								} %empty
			;
			
expr_list_ne	: expr_list_ne ',' expr	//{$$ = (int)make_node((struct ast_token){"expr_list_ne",0,0,@$},2,$1,$3);}
								{
									$$ = ast_node_new(
										ast_token_here("expr_list_ne",0,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(2, $1, $3)
									);
									printParsed($$);
								}
				| expr					//{$$ = (int)make_node((struct ast_token){"expr_list_ne",1,0,@$},1,$1);}
								{
									$$ = ast_node_new(
										ast_token_here("expr_list_ne",1,NULL,@$),
										vector2_ptr_ast_node_here_from_list
										(1, $1)
									);
									printParsed($$);
								}
				;

expr	:	ID							{$$ = ast_node_new(ast_token_here("expr_id",	0,$1->token.value,@$), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_id",0,(char*)$1,@$},0);}
		|	INTEGER						{$$ = ast_node_new(ast_token_here("expr_const",	0,$1->token.value,@$), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed($$);}//$$ = (int)make_node((struct ast_token){"expr_const",0,(char*)$1,@$},0);}
		|	INTEGERX					{$$ = ast_node_new(ast_token_here("expr_const",	1,$1->token.value,@$), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_const",1,(char*)$1,@$},0);}
		|	INTEGERB					{$$ = ast_node_new(ast_token_here("expr_const",	2,$1->token.value,@$), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_const",2,(char*)$1,@$},0);}
		|	FLOATING					{$$ = ast_node_new(ast_token_here("expr_const",	3,$1->token.value,@$), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_const",3,(char*)$1,@$},0);}
		|	CHARACTER					{$$ = ast_node_new(ast_token_here("expr_const",	4,$1->token.value,@$), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_const",4,(char*)$1,@$},0);}
		|	STRING						{$$ = ast_node_new(ast_token_here("expr_const",	5,$1->token.value,@$), vector2_ptr_ast_node_here_from_list(1, yylval)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_const",5,(char*)$1,@$},0);}
		|	expr '[' expr ']'			{$$ = ast_node_new(ast_token_here("expr_index",	0,NULL,@$),	vector2_ptr_ast_node_here_from_list(2, $1,$3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_index",0,0,@$},2,$1,$3);}
		|	'(' expr ')'				{$$ = ast_node_new(ast_token_here("expr_subexpr",0,NULL,@$),vector2_ptr_ast_node_here_from_list(1, $2)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_subexpr",0,0,@$},1,$2);}
		|	expr '(' expr_list ')'		{$$ = ast_node_new(ast_token_here("expr_call",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_call",0,0,@$},2,$1,$3);}
		|	expr '.' expr				{$$ = ast_node_new(ast_token_here("expr_.",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_.",0,0,@$},2,$1,$3);}
		|	expr '^' expr				{$$ = ast_node_new(ast_token_here("expr_^",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_^",0,0,@$},2,$1,$3);}
		|	expr '/' expr				{$$ = ast_node_new(ast_token_here("expr_/",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_/",0,0,@$},2,$1,$3);}
		|	expr '*' expr				{$$ = ast_node_new(ast_token_here("expr_*",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_*",0,0,@$},2,$1,$3);}
		|	expr '%' expr				{$$ = ast_node_new(ast_token_here("expr_%",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_%",0,0,@$},2,$1,$3);}
		|	expr '-' expr				{$$ = ast_node_new(ast_token_here("expr_-",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_-",0,0,@$},2,$1,$3);}
		|	expr '+' expr				{$$ = ast_node_new(ast_token_here("expr_+",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_+",0,0,@$},2,$1,$3);}
		|	'!' expr					{$$ = ast_node_new(ast_token_here("expr_not",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(1, $2)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_not",0,0,@$},1,$2);}
		|	expr '&' expr				{$$ = ast_node_new(ast_token_here("expr_and",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_and",0,0,@$},2,$1,$3);}
		|	expr '|' expr				{$$ = ast_node_new(ast_token_here("expr_or",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_or",0,0,@$},2,$1,$3);}
		|	expr EQUAL expr				{$$ = ast_node_new(ast_token_here("expr_==",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_==",0,0,@$},2,$1,$3);}
		|	expr NOTEQUAL expr		  	{$$ = ast_node_new(ast_token_here("expr_!=",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_!=",0,0,@$},2,$1,$3);}
		|	expr '>' expr			 	{$$ = ast_node_new(ast_token_here("expr_>",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_>",0,0,@$},2,$1,$3);}
		|	expr '<' expr			 	{$$ = ast_node_new(ast_token_here("expr_<",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_<",0,0,@$},2,$1,$3);}
		|	expr '=' expr				{$$ = ast_node_new(ast_token_here("expr_=",		0,NULL,@$), vector2_ptr_ast_node_here_from_list(2, $1, $3)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_=",0,0,@$},2,$1,$3);}
		|	expr INC					{$$ = ast_node_new(ast_token_here("expr_++",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(1, $1)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_++",0,0,@$},1,$1);}
		|	INC	expr    %prec PREINC	{$$ = ast_node_new(ast_token_here("expr_++",	1,NULL,@$), vector2_ptr_ast_node_here_from_list(1, $1)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_++",1,0,@$},1,$2);}
		|	expr DEC					{$$ = ast_node_new(ast_token_here("expr_--",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(1, $1)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_--",0,0,@$},1,$1);}
		|	DEC	expr    %prec PREDEC	{$$ = ast_node_new(ast_token_here("expr_--",	1,NULL,@$), vector2_ptr_ast_node_here_from_list(1, $2)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_--",1,0,@$},1,$2);}
		|	'-' expr 	%prec PRENEG	{$$ = ast_node_new(ast_token_here("expr_neg",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(1, $2)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_neg",0,0,@$},1,$2);}
		|	'*' expr	%prec PREDEREF	{$$ = ast_node_new(ast_token_here("expr_deref",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(1, $2)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_deref",0,0,@$},1,$2);}
		|	'&' expr	%prec PREREF	{$$ = ast_node_new(ast_token_here("expr_ref",	0,NULL,@$), vector2_ptr_ast_node_here_from_list(1, $2)); printParsed($$);}//{$$ = (int)make_node((struct ast_token){"expr_ref",0,0,@$},1,$2);}
		;

%%
extern FILE *yyin;
FILE *yyin2;

/*

|	expr INC	%prec POSTINC	
|	INC	expr    %prec PREINC	
|	expr DEC	%prec POSTDEC	
|	DEC	expr    %prec PREDEC	
|	'-' expr 	%prec PRENEG	
|	'*' expr	%prec PREDEREF	
|	'&' expr	%prec PREREF	

*/

int main(int argc, char **argv); //moved to main.c

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


int yyerror(const char *s)
{	
	printf("\nyyerror()!\n");
	int x1 = yylloc.first_column;
	int y1 = yylloc.first_line;
	int x2 = yylloc.last_column;
	//int y2 = yylloc.last_line;
	const char *f = yylloc.filename;
	int length = x2-x1;
	if(length == 1){length = 0;}
	point_out_error(y1,x1,f,s,length);
	fflush(stdout);
	
	error("syntax error. yytext: [%s]\n",(char*)yylval);
	//exit(1);
	return 1;
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