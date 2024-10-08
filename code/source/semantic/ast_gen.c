#include "ast_gen.h"
// #include "semantic.h"
// #include "yaccin.tab.h" //for get_source_text2()
// #include "ctype.h"
// #include "semantic_analyze.h"
// #include "semantic_expr_op.h"
// #include <assert.h>

struct ast_node *ast_gen_id(const char *name){return ast_node_new(ast_token_here("expr_id",0,stralloc(name),nullPos()), vector2_ptr_ast_node_here_from_list(0));}

struct ast_node *ast_gen_num(int num){
	char buff[80]; snprintf(buff, 80, "%d", num);
	return ast_node_new(ast_token_here("expr_const",0,stralloc(buff),nullPos()),
		vector2_ptr_ast_node_here_from_list(0));
}

struct ast_node *ast_gen_dot(struct ast_node *base, struct ast_node *member){
	return ast_node_new(ast_token_here("expr_.",0,NULL,nullPos()), 
		vector2_ptr_ast_node_here_from_list(2, base, member));
}

struct ast_node *ast_gen_call(struct ast_node *func, struct ast_node *args){
	return ast_node_new(ast_token_here("expr_call",0,NULL,nullPos()), 
		vector2_ptr_ast_node_here_from_list(2, func, args));
}

struct ast_node *ast_gen_assign(struct ast_node *dest, struct ast_node *src){
	return ast_node_new(ast_token_here("expr_=",0,NULL,nullPos()),
		vector2_ptr_ast_node_here_from_list(2, dest, src));
}

struct ast_node *ast_gen_index(struct ast_node *arr, struct ast_node *index){
	return ast_node_new(ast_token_here("expr_index",0,NULL,nullPos()),
		vector2_ptr_ast_node_here_from_list(2, arr, index));
}


struct ast_node *ast_gen_constructor_call(const char *name, struct ast_node *arg_list){
	return ast_gen_call(
		ast_gen_dot(
			ast_gen_id(name),
			ast_gen_id("constructor")
		),
		arg_list
	);
}	

struct ast_node *ast_gen_op(struct ast_node *A, const char *op, struct ast_node *B){
	return ast_node_new(ast_token_here(op,0,NULL,nullPos()),
		vector2_ptr_ast_node_here_from_list(2, A, B)
	);
}
