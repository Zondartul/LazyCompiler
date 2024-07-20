%{
	
%}
%code requires {
  //#define YYLTYPE YYLTYPE
  #include "globals.h"
  #include "types/ast_node.h"
  
	//ast_node.h not fully included yet due to shenanigans
	typedef struct ast_node ast_node;
	
	extern ast_node *finalNode;
	void /*int*/ yyerror(const char *s);
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
}
%code {
	
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
		//exit(1);
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
}

/*
From https://www.gnu.org/software/bison/manual/bison.html#Precedence

The first precedence/associativity declaration in the file declares 
	the operators whose precedence is lowest, the next such declaration 
	declares the operators whose precedence is a little higher, and so on.

Finally, the resolution of conflicts works by comparing the precedence 
	of the rule being considered with that of the lookahead token. 
	If the token’s precedence is higher, the choice is to shift. 
	If the rule’s precedence is higher, the choice is to reduce. 
	If they have equal precedence, the choice is made based on the 
	associativity of that precedence level. The verbose output file 
	made by -v (see Invoking Bison) says how each conflict was resolved.

Not all rules and not all tokens have precedence. If either the rule 
	or the lookahead token has no precedence, then the default is to shift.
*/

%token RETURN CLASS ID TYPE END IF ELSE ELSEIF WHILE FOR INTEGER INTEGERX INTEGERB FLOATING CHARACTER STRING EQUAL NOTEQUAL INC DEC

/// precedence-coding tokens:
/// INDEX SUBEXPR BRACELIST CALL DOT EXPR MULDIV ADDSUB CAST LNEG LOGIC COMPARE ASSIGN POSTOP PREOP REF DEREF PRENEG 
/*%glr-parser*/
%debug
%define parse.error verbose
/// ps: 'left' and 'precedence' are both precedence
///     but 'precedence' does not set associativity

/// HOW TO SET UP PRECEDENCE AND ASSOCIATIVITY:
/// a method that seems to work, 20 July 2024:
/// 1. use both operator tokens (e.g. '=')
///    and production tokens (e.g. ASSIGN).
///		- explanation: when the bottom up parser encounters the next operator, and it's at the end of a production,
///		  there is a shift/reduce conflict. Shift means 'prefer the incoming operator, it has higher precedence'.
///		  Reduce means 'prefer the existing production, it has higher precedence'. The prec. of production and token are compared.
///		  associativity also does something, idk.
///		  operator token associativity marks the token itself, while production token creates a fake token for an expression,
///		  because the precedence of a production comes from the last token and that happens to be 'expr' which is used in multiple productions
///		  and so one value of precedence for 'expr' doesn't help us.
///	2. use %right and %left when possible - they set associativity and precedence simultaneously.
///	   if not possible, use %precedence to set only precedence without associativity.
///  3. Things that appear later in the %right %left list (on a lower line) are "more important".
///  4. Use the 'C operator precedence list' and write it down upside-down while following rule 1.
///  5. Check unit-tests and compare actual ASTs to check if production is correct.
///  6. suspect ambiguous tokens as it is unclear which line to put them on 
///		suspect tokens:
///		  INC / NEG are tokens in both POSTOP and PREOP (++x, x++, --x, x--)
/// 	  '*' used in both DEREF and MULDIV (1*2, *ptr)
///		  - used in both NEG and ADDSUB (-x, 2-1)
///		  ( used in CALL, CAST and SUBEXPR ( foo(), (int)x, 1*(2+3))
///		  & used in both LOGIC and DEREF ( isA & isB, &x)
///

%right '=' ASSIGN
%left LOGIC
%left EQUAL NOTEQUAL COMPARE
%left '+' ADDSUB // -
%left '/' '%' MULDIV EXP  // *
%right PREOP INC DEC '-' NEG '!' LNEG CAST '*' DEREF '&' REF
%left '(' '[' '.' POSTOP CALL INDEX DOT BRACELIST // INC DEC
%precedence SUBEXPR

/*
%precedence SUBEXPR
%left '(' '[' '.' POSTOP CALL INDEX DOT BRACELIST // INC DEC
%right PREOP INC DEC '-' NEG '!' LNEG CAST '*' DEREF '&' REF
%left '/' '%' MULDIV EXP  // *
%left '+' ADDSUB // -
%left EQUAL NOTEQUAL COMPARE
%left LOGIC
%right '=' ASSIGN
*/


/*
%left '='
%left '|' '&'
%left '<' '>' 
%left '+' '-'
%left '^' '/' '*'
*/
/* %precedence ASSIGN  // x =
%precedence LOGIC	// x & y
%precedence COMPARE // x > y, x == y
%precedence ADDSUB  // x + y, x - y
%precedence MULDIV EXP  // x * y, x / y, x % y, x ^ y
%precedence PRENEG PREOP LNEG CAST REF DEREF // -x, !x, (type)x, &x, *x
%precedence '[' //']'
%precedence INDEX	// x[2]
%precedence DOT		// x.m
%precedence SUBEXPR CALL BRACELIST POSTOP// (a+b), f(o), {1,2} */




/// Let bison complain about "useless precedence" and "useless associativity", it's just being a dick for no reason.
/// the current rules may lead to a parser identical to one with less rules, but this way we know for certain the rules aren't flipped.

//%left '.' 

%%

program :	decl_stmt_list	
							{	
								// YYLTYPE pos = @$;
								// ast_node *child1 = $1;
								// finalNode = ast_node_new(
								// 	ast_token_here("program",0,NULL,pos),
								// 	vector2_ptr_ast_node_here_from_list
								// 	(1, child1)
								// );
								// printParsed(finalNode);
								finalNode = production("program",0,NULL,@$,$1,0,0,0);
							}		
		;

stmt	:	imp_stmt		
							{
								// ast_node **res = &($$);
								// *res = ast_node_new(
								// 	ast_token_here("stmt",0,NULL,@$),
								// 	vector2_ptr_ast_node_here_from_list
								// 	(1, $1)
								// );
								// printParsed(*res);
								$$ = production("stmt",0,NULL,@$,$1,0,0,0);
							}
		|	decl_stmt		{/*printParsed($$);*/	$$ = production("stmt",1,NULL,@$,$1,0,0,0);}
		;
		
stmt_list	: stmt_list_ne						{$$ = production("stmt_list",0,NULL,@$,$1,0,0,0); ast_unroll_lists($$);}
			|					{@$.null = 1;	$$ = production("stmt_list",1,"<empty>",nullPos(),0,0,0,0);} %empty
			;
		
stmt_list_ne	: stmt_list_ne stmt				{$$ = production("stmt_list_ne",0,NULL,@$,$1,$2,0,0);}
				| stmt							{$$ = production("stmt_list_ne",1,NULL,@$,$1,0,0,0);}
				;
		
decl_stmt	:
			class_def							{$$ = production("decl_stmt",0,NULL,@$,$1,0,0,0);}
			|	func_def						{$$ = production("decl_stmt",1,NULL,@$,$1,0,0,0);}
			|	var_decl ';'					{$$ = production("decl_stmt",2,NULL,@$,$1,0,0,0);}
			|	error							{$$ = error_production(STR_ERR_END_OR_SEMICOLON, @$);}
			;	
			
decl_stmt_list	:	decl_stmt_list_ne			{$$ = production("decl_stmt_list",0,NULL,@$,$1,0,0,0);ast_unroll_lists($$);}
				|				{@$.null =  1;	$$ = production("decl_stmt_list",1,"<empty>",nullPos(),0,0,0,0);} %empty
				;

decl_stmt_list_ne	: decl_stmt_list_ne decl_stmt {$$ = production("decl_stmt_list_ne",0,NULL,@$,$1,$2,0,0);}
					| decl_stmt					{$$ = production("decl_stmt_list_ne",1,NULL,@$,$1,0,0,0);}
					;
imp_stmt	:	if_block						{$$ = production("imp_stmt",0,NULL,@$,$1,0,0,0);}
			|	while_loop						{$$ = production("imp_stmt",1,NULL,@$,$1,0,0,0);}
			|	expr ';'						{$$ = production("imp_stmt",2,NULL,@$,$1,0,0,0);}
			|	RETURN ';'						{$$ = production("imp_stmt",3,NULL,@$,0,0,0,0);}
			|	RETURN expr ';'					{$$ = production("imp_stmt",4,NULL,@$,$2,0,0,0);} //repaired from 5
			|	for_loop						{$$ = production("imp_stmt",5,NULL,@$,$1,0,0,0);}
			|	error 							{$$ = error_production(STR_ERR_END_OR_SEMICOLON, @$);}
			;
			
class_def	:	CLASS ID decl_stmt_list END		{const char *id_str = $2->token.value; $$ = production("class_def",0,id_str,@$,$2,$3,0,0);}

STAR		:   '*'	 							{$$ = production("stars",0,"*",@$,0,0,0,0);}
			;

ptr_stars	:	ptr_stars_ne					{$$ = production("ptr_stars",0,NULL,@$,$1,0,0,0); ast_unroll_lists($$);}
			|									{$$ = production("ptr_stars",1,"<empty>",nullPos(),0,0,0,0);}	%empty
			;

ptr_stars_ne:	ptr_stars_ne STAR 				{$$ = production("ptr_stars_ne",0,NULL,@$,$1,$2,0,0);}
			|	STAR							{$$ = production("ptr_stars_ne",1,NULL,@$,$1,0,0,0);}
			;

typename	:	TYPE	 ptr_stars 				{const char *type_str = $1->token.value; $$ = production("typename",0,type_str,@$,$1,$2,0,0);}
			|	CLASS ID ptr_stars 				{const char *type_str = $2->token.value; $$ = production("typename",1,type_str,@$,$2,$3,0,0);}
			;

func_def	:	typename ID '('	var_decl_list ')' stmt_list END	
												{const char *id_str = $2->token.value; $$ = production("func_def",0,id_str,@$,$1,$2,$4,$6);}

var_decl	:	typename ID						{const char *id_str = $2->token.value; $$ = production("var_decl",0,id_str,@$,$1,$2,0,0);}
			|	typename ID '[' expr ']'		{const char *id_str = $2->token.value; $$ = production("var_decl",1,id_str,@$,$1,$2,$4,0);}
			|	typename ID '=' expr			{const char *id_str = $2->token.value; $$ = production("var_decl_assign",0,id_str,@$,$1,$2,$4,0);}
			|	typename ID '(' expr_list ')'	{const char *id_str = $2->token.value; $$ = production("var_decl_constructor",0,id_str,@$,$1,$2,$4,0);}
			;

var_decl_list	:	var_decl_list_ne							{$$ = production("var_decl_list",0,NULL,@$,$1,0,0,0); ast_unroll_lists($$);}
				|												{$$ = production ("var_decl_list",1,"<empty>",nullPos(),0,0,0,0);} %empty //inform Bison/YACC that this rule was intentionally left empty
				;
				
var_decl_list_ne	:	var_decl_list_ne ',' var_decl			{$$ = production("var_decl_list_ne",0,NULL,@$,$1,$3,0,0);}
					|	var_decl								{$$ = production("var_decl_list_ne",1,NULL,@$,$1,0,0,0);}
					;
				
if_block	:	if_then END										{$$ = production("if_block",0,NULL,@$,$1,0,0,0);}
			|	if_then ELSE stmt_list END						{$$ = production("if_block",1,NULL,@$,$1,$3,0,0);}
			;

if_then		:	IF '(' expr ')' stmt_list						{$$ = production("if_then",0,NULL,@$,$3,$5,0,0);}
			|	if_then ELSEIF '(' expr ')' stmt_list			{$$ = production("if_then",2,NULL,@$,$1,$4,$6,0);}
			;
			
while_loop	:	WHILE '(' expr ')' stmt_list END				{$$ = production("while_loop",0,NULL,@$,$3,$5,0,0);}
			;

for_loop	:	FOR '(' stmt expr ';' expr ')' stmt_list END	{$$ = production("for_loop",0,NULL,@$,$3,$4,$6,$8);}
			;
			
expr_list	:	expr_list_ne					{$$ = production("expr_list",0,NULL,@$,$1,0,0,0);ast_unroll_lists($$);}
			|					{@$.null = 1; 	$$ = production("expr_list",1,"<empty>",nullPos(),0,0,0,0);} %empty
			;
			
expr_list_ne	: expr_list_ne ',' expr			{$$ = production("expr_list_ne",0,NULL,@$,$1,$3,0,0);}
				| expr							{$$ = production("expr_list_ne",1,NULL,@$,$1,0,0,0);}
				;

expr	:	ID									{$$ = production("expr_id",0,$1->token.value,@$,yylval,0,0,0);}
		|	INTEGER								{$$ = production("expr_const",0,$1->token.value,@$,yylval,0,0,0);}
		|	INTEGERX							{$$ = production("expr_const",1,$1->token.value,@$,yylval,0,0,0);}
		|	INTEGERB							{$$ = production("expr_const",2,$1->token.value,@$,yylval,0,0,0);}
		|	FLOATING							{$$ = production("expr_const",3,$1->token.value,@$,yylval,0,0,0);}
		|	CHARACTER							{$$ = production("expr_const",4,$1->token.value,@$,yylval,0,0,0);}
		|	STRING								{$$ = production("expr_const",5,$1->token.value,@$,yylval,0,0,0);}
		|	expr '[' expr ']'	%prec INDEX		{$$ = production("expr_index",0,NULL,@$,$1,$3,0,0);}
		|	'(' expr ')'	%prec SUBEXPR		{$$ = production("expr_subexpr",0,NULL,@$,$2,0,0,0);}
		|	'{' expr_list '}'	%prec BRACELIST	{$$ = production("expr_braced_list",0,NULL,@$,$2,0,0,0);}	
		|	expr '(' expr_list ')'	%prec CALL	{$$ = production("expr_call",0,NULL,@$,$1,$3,0,0);}
		|	expr '.' expr	%prec DOT			{$$ = production("expr_.",0,NULL,@$,$1,$3,0,0);}
		|	expr '^' expr	%prec EXP			{$$ = production("expr_^",0,NULL,@$,$1,$3,0,0);}
		|	expr '/' expr	%prec MULDIV		{$$ = production("expr_/",0,NULL,@$,$1,$3,0,0);}
		|	expr '*' expr	%prec MULDIV		{$$ = production("expr_*",0,NULL,@$,$1,$3,0,0);}
		|	expr '%' expr	%prec MULDIV		{$$ = production("expr_%",0,NULL,@$,$1,$3,0,0);}
		|	expr '-' expr	%prec ADDSUB		{$$ = production("expr_-",0,NULL,@$,$1,$3,0,0);}
		|	expr '+' expr	%prec ADDSUB		{$$ = production("expr_+",0,NULL,@$,$1,$3,0,0);}
		|	'(' typename ')' expr	%prec CAST	{$$ = production("expr_cast",0,NULL,@$,$2,$4,0,0);}
		|	'!' expr	%prec LNEG				{$$ = production("expr_not",0,NULL,@$,$2,0,0,0);}
		|	expr '&' expr	%prec LOGIC			{$$ = production("expr_and",0,NULL,@$,$1,$3,0,0);}
		|	expr '|' expr	%prec LOGIC			{$$ = production("expr_or",0,NULL,@$,$1,$3,0,0);}
		|	expr EQUAL expr	%prec COMPARE		{$$ = production("expr_==",0,NULL,@$,$1,$3,0,0);}
		|	expr NOTEQUAL expr %prec COMPARE	{$$ = production("expr_!=",0,NULL,@$,$1,$3,0,0);}
		|	expr '>' expr	%prec COMPARE 		{$$ = production("expr_>",0,NULL,@$,$1,$3,0,0);}
		|	expr '<' expr	%prec COMPARE 		{$$ = production("expr_<",0,NULL,@$,$1,$3,0,0);}
		|	expr '=' expr	%prec ASSIGN		{$$ = production("expr_=",0,NULL,@$,$1,$3,0,0);}
		|	expr INC	%prec POSTOP			{$$ = production("expr_++",0,NULL,@$,$1,0,0,0);}
		|	INC	expr    %prec PREOP				{$$ = production("expr_++",1,NULL,@$,$2,0,0,0);} // note: repaired from $1
		|	expr DEC	%prec POSTOP			{$$ = production("expr_--",0,NULL,@$,$1,0,0,0);}
		|	DEC	expr    %prec PREOP				{$$ = production("expr_--",1,NULL,@$,$2,0,0,0);}
		|	'-' expr 	%prec PRENEG			{$$ = production("expr_neg",0,NULL,@$,$2,0,0,0);}
		|	'*' expr	%prec DEREF				{$$ = production("expr_deref",0,NULL,@$,$2,0,0,0);}
		|	'&' expr	%prec REF				{$$ = production("expr_ref",0,NULL,@$,$2,0,0,0);}
		;

%%

