#ifndef AST_GUARD
#define AST_GUARD
#include <stdio.h>
#include "vector2.h"
#include "globals.h"
#include "vector3int.h"
#include "types/ast_node.h"
//defines

//generated
//definition_vector_of(int);

//variables
extern vector2_ptr_ast_node ast_nodes;

//functions
void ast_init(void);
struct ast_node *ast_get_node(int id) __attribute__ ((deprecated)); 
int ast_get_child_id(struct ast_node *N, int num)__attribute__ ((deprecated));
struct ast_node *ast_get_child(struct ast_node *N, int num);
int node(struct ast_token token, int n, ...)__attribute__ ((deprecated));
void ast_print(struct ast_node *N, FILE *fp);
void ast_print_graph(struct ast_node *N, FILE *fp, const char *str);

#endif