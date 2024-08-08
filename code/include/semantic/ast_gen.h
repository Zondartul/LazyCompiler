#ifndef ASTGEN_GUARD
#define ASTGEN_GUARD
/// AST generation so we can rewrite complex code into simple code!
/// - this also reduces the number of edge cases and points of failure
#include "semantic.h"

struct ast_node *ast_gen_id(const char *name);
struct ast_node *ast_gen_num(int num);
struct ast_node *ast_gen_dot(struct ast_node *base, struct ast_node *member);
struct ast_node *ast_gen_call(struct ast_node *func, struct ast_node *args);
struct ast_node *ast_gen_assign(struct ast_node *dest, struct ast_node *src);
struct ast_node *ast_gen_index(struct ast_node *arr, struct ast_node *index);
struct ast_node *ast_gen_constructor_call(const char *name, struct ast_node *arg_list);
struct ast_node *ast_gen_op(struct ast_node *A, const char *op, struct ast_node *B);

#endif //ASTGEN_GUARD