#ifndef AST_GUARD
#define AST_GUARD
//#include "vector.h"
#include <stdio.h>
#include "vector2.h"

//extern vector ast_nodes;
typedef struct ast_token{
	const char *type;
	int production;
	char *value;
	YYLTYPE pos;
} ast_token;

definition_vector_of(int);

struct ast_node{
	ast_token token;
	struct vector2_int children; //ints
	//struct ast_node* children[AST_MAX_CHILDREN];
};
typedef struct ast_node ast_node;
typedef ast_node* ptr_ast_node;

definition_vector_of(ptr_ast_node);
extern vector2_ptr_ast_node ast_nodes;

void ast_init(void);
struct ast_node *ast_get_node(int id);
int ast_get_child_id(struct ast_node *N, int num);
struct ast_node *ast_get_child(struct ast_node *N, int num);
int node(struct ast_token token, int n, ...);
void ast_print(struct ast_node *N, FILE *fp);
void ast_print_graph(struct ast_node *N, FILE *fp, const char *str);
#endif