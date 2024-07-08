#include "types/ast_node.h"

implementation_vector_of(ptr_ast_node);

ptr_ast_node new_ast_node(ast_token newtoken, vector2_ptr_ast_node newchildren){
	ast_node *node = (ast_node*)malloc(sizeof(ast_node));
	node->token = newtoken;
	node->children = newchildren;
	return node;
}


struct ast_node ast_node_default(){
	struct ast_node S;
	S.token = ast_token_default();
	vector2_ptr_ast_node_constructor(&(S.children));
	return S;
}

struct ast_node ast_node_here(
	ast_token token,
	vector2_ptr_ast_node children
	){
	struct ast_node S;
	S.token = token;
	S.children = children;
	return S;
}

struct ast_node* ast_node_new0(){
	struct ast_node *S = (struct ast_node*)malloc(sizeof(struct ast_node));
	*S = ast_node_default();
	return S;
}

struct ast_node* ast_node_new(
	ast_token token,
	vector2_ptr_ast_node children
	){
	struct ast_node *S = (struct ast_node*)malloc(sizeof(struct ast_node));
	S->token = token;
	S->children = children;
	return S;
}
