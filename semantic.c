#include "semantic.h"

void semantic_analyze(struct ast_node *node){
	int i;
	if(!strcmp(node->token.type, "program")){
		printf("got program\n");
		semantic_analyze(ast_get_child(node, 0));
		return;
	}
	if(!strcmp(node->token.type, "decl_stmt_list")){
		printf("got decl_stmt_list\n");
		for(i = 0; i < node->children.size; i++){
			semantic_analyze(ast_get_child(node, i));
		}
		return;
	}
	if(!strcmp(node->token.type, "decl_stmt")){
		printf("got decl_stmt\n");
		semantic_analyze(ast_get_child(node,0));
		return;
	}
	if(!strcmp(node->token.type, "func_def")){
		printf("got func_def\n");
		struct type_name *T = semantic_get_type(ast_get_child(node,0)->token.value);
		char *name = node->token.value;
		//get arguments
			//use new separate symbol table
		semantic_analyze(ast_get_child(node,1));
			//join symbol table to definition
		//get code
			//use new code segment
		semantic_analyze(ast_get_child(node,2));
			//join code segment to definition
		return;
	}
	if(!strcmp(node->token.type, "var_decl_list")){
		printf("got var_decl_list\n");
		for(i = 0; i < node->children.size; i++){
			semantic_analyze(ast_get_child(node, i));
		}
		return;
	}
	if(!strcmp(node->token.type, "var_decl")){
		printf("got var_decl\n");
		struct type_name *T = semantic_get_type(ast_get_child(node,0)->token.value);
		char *name = node->token.value;
		//put into symbol table
		return;
	}
	if(!strcmp(node->token.type, "stmt_list")){
		printf("got stmt_list\n");
		for(i = 0; i < node->children.size; i++){
			semantic_analyze(ast_get_child(node, i));
		}
		return;
	}
	if(!strcmp(node->token.type, "stmt")){
		printf("got stmt\n");
		semantic_analyze(ast_get_child(node,0));
		return;
	}
	if(!strcmp(node->token.type, "imp_stmt")){
		printf("got imp_stmt\n");
		switch(node->token.production){
			case(0)://if_block
			case(1)://while_loop
			case(2)://expr
				semantic_analyze(ast_get_child(node,0));
				break;
			case(3)://return
				//emit code: return
				break;
			case(4)://return expr
				semantic_analyze(ast_get_child(node,0));
				//emit code: return
				break;
		}
		return;
	}
	if(!strcmp(node->token.type, "if_block")){
		printf("got if_block\n");
		switch(node->token.production){
			case(0)://if_then END
				semantic_analyze(ast_get_child(node,0));
				break;
			case(1)://if_then ELSE stmt_list END
				semantic_analyze(ast_get_child(node,0));
				semantic_analyze(ast_get_child(node,1));
				break;
		}
		return;
	}
	if(!strcmp(node->token.type, "if_then")){
		printf("got if_then\n");
		switch(node->token.production){
			case(0): //IF ( expr ) THEN stmt_list
				//push new symbol table
				semantic_analyze(ast_get_child(node,0));
				semantic_analyze(ast_get_child(node,1));
				//pop symbol table
				break;
			case(1): //if_then ELSEIF ( expr ) THEN stmt_list
				semantic_analyze(ast_get_child(node,0));
				semantic_analyze(ast_get_child(node,1));
				semantic_analyze(ast_get_child(node,2));
				break;
		};
		return;
	}
	if(!strcmp(node->token.type, "while_loop")){
		printf("got while_loop\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		return;
	}
	if(!strcmp(node->token.type, "class_def")){
		printf("got class_def\n");
		char *name = node->token.value;
		//make new symbol table
		semantic_analyze(ast_get_child(node,0));
		//embed into existing symbol table
		return;
	}
	if(!strcmp(node->token.type, "expr_list")){
		printf("got expr_list\n");
		for(i = 0; i < node->children.size; i++){
			semantic_analyze(ast_get_child(node, i));
			//give name to new expression result?
			//put it in stack?
		}
		return;
	}
	if(!strcmp(node->token.type, "expr_id"){
		printf("got expr_id\n");
		char *name = node->token.value;
		//put variable <name> on the stack
		return;
	}
	if(!strcmp(node->token.type, "expr_const"){
		printf("got expr_const\n");
		//figure out what kind of const this is
		return;
	}
	if(!strcmp(node->token.type, "expr_subexpr"){
		printf("got expr_subexpr\n");
		semantic_analyze(ast_get_child(node,0));
		return;
	}
	if(!strcmp(node->token.type, "expr_call"){
		printf("got expr_call\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		//emit code: call
		return;
	}
	if(!strcmp(node->token.type, "expr_."){
		printf("got expr_.\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		//emit code: member access
		return;
	}
	if(!strcmp(node->token.type, "expr_^"){
		printf("got expr_^\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,0));
		//emit code: exp
		return;
	}
	if(!strcmp(node->token.type, "expr_/"){
		printf("got expr_/\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		//emit code: div
		return;
	}
	if(!strcmp(node->token.type, "expr_*"){
		printf("got expr_*\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		//emit code: mul
		return;
	}
	if(!strcmp(node->token.type, "expr_-"){
		printf("got expr_-\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		//emit code: sub
		return;
	}
	if(!strcmp(node->token.type, "expr_+"){
		printf("got expr_+\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		//emit code: add
		return;
	}
	if(!strcmp(node->token.type, "expr_="){
		printf("got expr_=\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		//emit code: assign
		return;
	}
	//unknown node type
	printf("semantic: unknown node type: [%s]\n",node->token.type);
	return;
}

struct type_name *semantic_get_type(const char *str){
}