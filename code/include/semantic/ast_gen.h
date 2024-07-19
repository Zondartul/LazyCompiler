#ifndef ASTGEN_GUARD
#define ASTGEN_GUARD
/// AST generation so we can rewrite complex code into simple code!
/// - this also reduces the number of edge cases and points of failure
#include "semantic.h"

struct ast_node *ast_gen_id(const char *name);
struct ast_node *ast_gen_dot(struct ast_node *base, struct ast_node *member);
struct ast_node *ast_gen_call(struct ast_node *func, struct ast_node *args);
struct ast_node *ast_gen_constructor_call(const char *name, struct ast_node *arg_list);


#endif //ASTGEN_GUARD