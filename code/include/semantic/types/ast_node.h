#ifndef AST_NODE_GUARD
#define AST_NODE_GUARD
#include "types/ast_token.h"
#include "util/vector2.h"
#include "util/vector3int.h"

struct ast_node;
typedef struct ast_node* ptr_ast_node;
definition_vector_of(ptr_ast_node);

#define v2pan_here() vector2_ptr_ast_node_here()

struct ast_node{
	ast_token token;
	vector2_ptr_ast_node children;
	//struct vector3_int children;
};
typedef struct ast_node ast_node;
//typedef struct ast_node ast_node;
//typedef ast_node* ptr_ast_node;
//definition_vector_of(ptr_ast_node);

ptr_ast_node new_ast_node(ast_token newtoken, vector2_ptr_ast_node newchildren);

struct ast_node ast_node_default();
struct ast_node ast_node_here(
	ast_token token,
	vector2_ptr_ast_node children
	);

struct ast_node* ast_node_new0();
struct ast_node* ast_node_new(
	ast_token token,
	vector2_ptr_ast_node children
	);

#endif