#ifndef AST_GUARD
#define AST_GUARD
#include "vector.h"
#include <stdio.h>

extern vector ast_nodes;
struct ast_token{
	const char *type;
	int production;
	char *value;
	// int start_line;
	// int start_char;
	// int end_line;
	// int end_char;
};

struct ast_node{
	struct ast_token token;
	vector children; //ints
	//struct ast_node* children[AST_MAX_CHILDREN];
};
void ast_init(void);
struct ast_node *ast_get_node(int id);
int ast_get_child_id(struct ast_node *N, int num);
struct ast_node *ast_get_child(struct ast_node *N, int num);
int node(struct ast_token token, int n, ...);
void ast_print(struct ast_node *N, FILE *fp);
void ast_print_graph(struct ast_node *N, FILE *fp, const char *str);
#endif