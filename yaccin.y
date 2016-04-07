%{
	#include "globals.h"
	#include "AST.h"
	#include "semantic.h"
	int finalNode;
	
	//		B = B C | C
	//beceomes
	//		B = C C C C C
	void ast_unroll_lists_helper(int nodeID2){
		printf("a");
		struct ast_node *N2 = ast_get_node(nodeID2);
		if(!N2->children.size){return;}
		printf("b");
		struct ast_node *N3 = ast_get_child(N2, 0);
		if(N2->token.type != N3->token.type){return;}
		printf("c");
		//if there is further recursion, unroll it now.
		ast_unroll_lists_helper(ast_get_child_id(N2, 0));
		//otherwise, move child contents up here.
		vector_remove(&N2->children, 0);
		int i;
		for(i = 0; i < N3->children.size; i++){
			printf("d");
			vector_push_back(&N2->children, vector_get_reference(&N3->children, i));
		}
	}
	//turns left-recursive lists into non-recursive ones
	//so	A = B | empty
	//		B = B C | C
	//becomes	A = C C C C C C | empty
	
	void ast_unroll_lists(int nodeID){
		struct ast_node *N1 = ast_get_node(nodeID);
		printf("unroll [%s] ",N1->token.type);
		printf("1");
		if(!N1->children.size){return;}
		struct ast_node *N2 = ast_get_child(N1, 0);
		printf("2");
		if(!N2->children.size){return;}
		struct ast_node *N3 = ast_get_child(N2, 0);
		printf("3");
		//if(N2->token.type != N3->token.type){return;}
		//otherwise, there is in fact a recursive list here!
		printf("4");
		ast_unroll_lists_helper(ast_get_child_id(N1, 0));
		vector_remove(&N1->children, 0);
		int i;
		for(i = 0; i < N2->children.size; i++){
			printf("5");
			vector_insert(&N1->children, vector_get_reference(&N2->children, i),0);
		}
		printf(" unroll done\n");
	}
%}
%code requires {
  #define YYLTYPE YYLTYPE
  typedef struct YYLTYPE
  {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
    char *filename;
  } YYLTYPE;
}
%code provides{
char *posToString(YYLTYPE pos);
}	
%token RETURN CLASS ID TYPE END IF THEN ELSE ELSEIF WHILE CONSTANT
%glr-parser
%debug
%error-verbose
%left '='
%left '+' '-'
%left '^' '/' '*'
%left '.' 
%right '(' ')'
%%

program :	decl_stmt_list	{finalNode = node((struct ast_token){"program",0,0},1,$1); }
		;

stmt	:	imp_stmt		{$$ = (int)node((struct ast_token){"stmt",0,0},1,$1);}
		|	decl_stmt		{$$ = (int)node((struct ast_token){"stmt",1,0},1,$1);}
		;
		
stmt_list	: stmt_list_ne		{$$ = (int)node((struct ast_token){"stmt_list",0,0},1,$1); ast_unroll_lists($$);}
			|					{$$ = (int)node((struct ast_token){"stmt_list",1,"<empty>"},0);}
			;
		
stmt_list_ne	: stmt_list_ne stmt	{$$ = (int)node((struct ast_token){"stmt_list_ne",0,0},2,$1,$2);}
				| stmt				{$$ = (int)node((struct ast_token){"stmt_list_ne",1,0},1,$1);}
				;
		
decl_stmt	:	class_def		{$$ = (int)node((struct ast_token){"decl_stmt",0,0},1,$1);}
			|	func_def		{$$ = (int)node((struct ast_token){"decl_stmt",1,0},1,$1);}
			|	var_decl ';'	{$$ = (int)node((struct ast_token){"decl_stmt",2,0},1,$1);}
			;
			
decl_stmt_list	:	decl_stmt_list_ne	{$$ = (int)node((struct ast_token){"decl_stmt_list",0,0},1,$1); ast_unroll_lists($$);}
				|						{$$ = (int)node((struct ast_token){"decl_stmt_list",0,"<empty>"},0);}
				;

decl_stmt_list_ne	: decl_stmt_list_ne decl_stmt {$$ = (int)node((struct ast_token){"decl_stmt_list_ne",0,0},2,$1,$2);}
					| decl_stmt						{$$ = (int)node((struct ast_token){"decl_stmt_list_ne",1,0},1,$1);}
					;
				
imp_stmt	:	if_block					{$$ = (int)node((struct ast_token){"imp_stmt",0,0},1,$1);}
			|	while_loop					{$$ = (int)node((struct ast_token){"imp_stmt",1,0},1,$1);}
			|	expr ';'					{$$ = (int)node((struct ast_token){"imp_stmt",2,0},1,$1);}
			|	RETURN ';'					{$$ = (int)node((struct ast_token){"imp_stmt",3,0},0);}
			|	RETURN expr ';'				{$$ = (int)node((struct ast_token){"imp_stmt",4,0},1,$2);}
			;
			
class_def	:	CLASS ID decl_stmt_list END	{printf("FUNC_DEF ID = [%s]\n",(char*)$2);$$ = (int)node((struct ast_token){"class_def",0,(char*)$2},1,$3);}

typename	:	TYPE		{$$ = (int)node((struct ast_token){"typename",0,(char*)$1},0);}				
			|	CLASS ID	{$$ = (int)node((struct ast_token){"typename",1,(char*)$2},0);}
			;

func_def	:	typename ID '('	var_decl_list ')' stmt_list END	{printf("FUNC_DEF ID = [%s]\n",(char*)$2);$$ = (int)node((struct ast_token){"func_def",0,(char*)$2},3,$1,$4,$6);}

var_decl	:	typename ID				{$$ = (int)node((struct ast_token){"var_decl",0,(char*)$2},1,$1);}
			|	typename ID '=' expr	{$$ = (int)node((struct ast_token){"var_decl_assign",1,(char*)$2},2,$1,$4);}
			;

var_decl_list	:	var_decl_list_ne			{$$ = (int)node((struct ast_token){"var_decl_list",0,0},1,$1); ast_unroll_lists($$);}
				|								{$$ = (int)node((struct ast_token){"var_decl_list",1,"<empty>"},0);}
				;
				
var_decl_list_ne	:	var_decl_list_ne ',' var_decl	{$$ = (int)node((struct ast_token){"var_decl_list_ne",0,0},2,$1,$3);}
					|	var_decl						{$$ = (int)node((struct ast_token){"var_decl_list_ne",1,0},1,$1);}
					;
				
if_block	:	if_then END										{$$ = (int)node((struct ast_token){"if_block",0,0},1,$1);}
			|	if_then ELSE stmt_list END						{$$ = (int)node((struct ast_token){"if_block",1,0},2,$1,$3);}
			;

if_then		:	IF '(' expr ')' THEN stmt_list				{$$ = (int)node((struct ast_token){"if_then",0,0},2,$3,$6);}
			|	if_then ELSEIF '(' expr ')' THEN stmt_list	{$$ = (int)node((struct ast_token){"if_then",2,0},2,$1,$3);}
			;
			
		/* 	if_block	:	if_start ELSE stmt_list END	{$$ = (int)node((struct ast_token){"if_block",0,0},2,$1,$3);}
		//	|	if_start ELSE if_block		{$$ = (int)node((struct ast_token){"if_block",1,0},2,$1,$2);}
			|	if_start END				{$$ = (int)node((struct ast_token){"if_block",2,0},1,$1);}
			;
			
if_start	:	IF '(' expr ')' THEN stmt_list 	{$$ = (int)node((struct ast_token){"if_start",0,0},2,$3,$6);}
			;
			 */
while_loop	:	WHILE '(' expr ')' stmt_list END	{$$ = (int)node((struct ast_token){"while_loop",0,0},2,$3,$5);}
			;

expr_list	:	expr_list_ne	{$$ = (int)node((struct ast_token){"expr_list",0,0},1,$1);ast_unroll_lists($$);}
			|					{$$ = (int)node((struct ast_token){"expr_list",1,"<empty>"},0);}
			;
			
expr_list_ne	: expr_list_ne ',' expr	{$$ = (int)node((struct ast_token){"expr_list_ne",0,0},2,$1,$3);}
				| expr					{$$ = (int)node((struct ast_token){"expr_list_ne",1,0},1,$1);}
				;

expr	:	ID						{$$ = (int)node((struct ast_token){"expr_id",0,(char*)$1},0);}
		|	CONSTANT				{$$ = (int)node((struct ast_token){"expr_const",1,(char*)$1},0);}
		|	'(' expr ')'			{$$ = (int)node((struct ast_token){"expr_subexpr",2,0},1,$1);}
		|	expr '(' expr_list ')'	{$$ = (int)node((struct ast_token){"expr_call",3,0},2,$1,$3);}
		|	expr '.' expr			{$$ = (int)node((struct ast_token){"expr_.",4,0},2,$1,$3);}
		|	expr '^' expr			{$$ = (int)node((struct ast_token){"expr_^",5,0},2,$1,$3);}
		|	expr '/' expr			{$$ = (int)node((struct ast_token){"expr_/",6,0},2,$1,$3);}
		|	expr '*' expr			{$$ = (int)node((struct ast_token){"expr_*",7,0},2,$1,$3);}
		|	expr '-' expr			{$$ = (int)node((struct ast_token){"expr_-",8,0},2,$1,$3);}
		|	expr '+' expr			{$$ = (int)node((struct ast_token){"expr_+",9,0},2,$1,$3);}
		|	expr '=' expr			{$$ = (int)node((struct ast_token){"expr_=",10,0},2,$1,$3);}
		;
/*			
expr	:	expr '+' expr2	{$$ = (int)node((struct ast_token){"expr",0,0},2,$1,$2);}
		|	expr '-' expr2	{$$ = (int)node((struct ast_token){"expr",1,0},2,$1,$2);}
		|	expr2			{$$ = (int)node((struct ast_token){"expr",2,0},1,$1);}
		;

expr2	:	expr2 '*' expr3	{$$ = (int)node((struct ast_token){"expr2",0,0},2,$1,$2);}
		|	expr2 '/' expr3	{$$ = (int)node((struct ast_token){"expr2",1,0},2,$1,$2);}
		|	expr3			{$$ = (int)node((struct ast_token){"expr2",2,0},1,$1);}
		;

expr3	:	expr3 '^' expr4	{$$ = (int)node((struct ast_token){"expr3",0,0},2,$1,$2);}
		|	expr4			{$$ = (int)node((struct ast_token){"expr3",1,0},1,$1);}
		;
		
expr4	:	expr4 '.' expr5			{$$ = (int)node((struct ast_token){"expr4",0,0},2,$1,$2);}
		|	expr4 '(' expr_list ')'	{$$ = (int)node((struct ast_token){"expr4",1,0},2,$1,$2);}
		|	expr5					{$$ = (int)node((struct ast_token){"expr4",2,0},1,$1);}
		;

expr5	:	ID						{$$ = (int)node((struct ast_token){"expr5",0,0},1,$1);}
		|	'(' expr ')'			{$$ = (int)node((struct ast_token){"expr5",1,0},1,$1);}
		|	CONSTANT				{$$ = (int)node((struct ast_token){"expr5",2,0},1,$1);}
		;
*/
%%
extern FILE *yyin;
FILE *yyin2;
int main(int argc, char **argv){
	if(argc == 2){
		yyin = fopen(argv[1],"r");
		yyin2 = fopen(argv[1],"r");
	}else{
		printf("usage: %s inputfile\n",argv[0]);
		return 0;
	}
	ast_init();
	yydebug = 1;
	yylloc.last_line = 1;
	lextokenend = 0;
	lexnumtabs = 0;
	yyparse();
	printf("\nPARSING DONE\n");
	FILE *fp = fopen("aout2.txt","w");
	if(!fp){return 1;}
	ast_print(ast_get_node(finalNode), fp);
	fclose(fp);
	fp = fopen("aout3.gv","w");
	if(!fp){return 1;}
	ast_print_graph(ast_get_node(finalNode),fp,"A");
	printf("\nPRINTING DONE\n");
	semantic_analyze(ast_get_node(finalNode));
	printf("\nGOOD BYE\n");
	return 0;
}

char *get_source_text(int start, int end){
	fseek(yyin2, start, SEEK_SET);
	char *buff = malloc(sizeof(char)*(end-start+1));
	int i;
	for(i = 0; i < end-start; i++){
		buff[i] = fgetc(yyin2);
	}
	buff[i] = 0;
	//fread(buff,end-start,1,yyin2);
	return buff;
}

char *get_source_line(int start){
	fseek(yyin2, start, SEEK_SET);
	int C = fgetc(yyin2);
	int len = 0;
	while((C != '\n') && (C != EOF)){C = fgetc(yyin2); len++;}
	return get_source_text(start,start+len);
}
// int yyerror (char *s) {
   // fprintf (stderr, "%s\n", s);
// }

int yyerror(char *s)
{	
	printf("\nyyerror()!\n");
	int x1 = yylloc.first_column;
	int y1 = yylloc.first_line;
	int x2 = yylloc.last_column;
	int y2 = yylloc.last_line;
	printf("pos: %d:%d:%d:%d\n",y1,x1,y2,x2);
    fprintf(stderr, "%s\n", get_source_line(lexlinestart));
	int i;
	for(i = 0; i < lexnumtabs; i++){fprintf(stderr, "\t");}
	for(i = 0; i < x1 - lexnumtabs; i++){fprintf(stderr, " ");}
	fprintf(stderr, "^\nline %d: %s\n",y1,s);
	exit(1);
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