#include "semantic.h"

void semantic_analyze_program(ast_node *node);
void semantic_analyze_decl_stmt_list(ast_node *node);
void semantic_analyze_decl_stmt(ast_node *node);
void semantic_analyze_func_def(ast_node *node);
void semantic_analyze_var_decl_list(ast_node *node);
void semantic_analyze_var_decl_constructor(ast_node *node);
void semantic_analyze_var_decl(ast_node *node);
void semantic_analyze_var_decl_assign(ast_node *node, expr_settings stg);
void semantic_analyze_var_decl_varargs(ast_node *node);
void semantic_analyze_stmt_list(ast_node *node);
void semantic_analyze_stmt(ast_node *node);
void semantic_analyze_imp_stmt(ast_node *node);
void semantic_analyze_if_block(ast_node *node);
void semantic_analyze_if_then(ast_node *node, if_settings stg);
void semantic_analyze_while_loop(ast_node *node);
void semantic_analyze_for_loop(ast_node *node);
void semantic_analyze_asm_stmt(ast_node *node);
void semantic_analyze_class_def(ast_node *node);
void semantic_analyze_expr_list(ast_node *node, expr_settings stg);
void semantic_analyze_expr_id(ast_node *node, expr_settings stg);
void semantic_analyze_expr_const(ast_node *node, expr_settings stg);
void semantic_analyze_expr_subexpr(ast_node *node, expr_settings stg);
void semantic_analyze_expr_braced_list(ast_node *node, expr_settings stg);
void semantic_analyze_expr_index(ast_node *node, expr_settings stg);
void semantic_analyze_expr_call(ast_node *node, expr_settings stg);
void semantic_analyze_expr_dot(ast_node *node, expr_settings stg);
void semantic_analyze_expr_pow(ast_node *node, expr_settings stg);
void semantic_analyze_expr_divide(ast_node *node, expr_settings stg);
void semantic_analyze_expr_multiply(ast_node *node, expr_settings stg);
void semantic_analyze_expr_modulo(ast_node *node, expr_settings stg);
void semantic_analyze_expr_and(ast_node *node, expr_settings stg);
void semantic_analyze_expr_or(ast_node *node, expr_settings stg);
void semantic_analyze_expr_not(ast_node *node, expr_settings stg);
void semantic_analyze_expr_minus(ast_node *node, expr_settings stg);
void semantic_analyze_expr_plus(ast_node *node, expr_settings stg);
void semantic_analyze_expr_increment(ast_node *node, expr_settings stg);
void semantic_analyze_expr_decrement(ast_node *node, expr_settings stg);
void semantic_analyze_expr_neg(ast_node *node, expr_settings stg);
void semantic_analyze_expr_deref(ast_node *node, expr_settings stg);
void semantic_analyze_expr_ref(ast_node *node, expr_settings stg);
void semantic_analyze_expr_equals(ast_node *node, expr_settings stg);
void semantic_analyze_expr_notequal(ast_node *node, expr_settings stg);
void semantic_analyze_expr_greater(ast_node *node, expr_settings stg);
void semantic_analyze_expr_less(ast_node *node, expr_settings stg);
void semantic_analyze_expr_less_equal(ast_node *node, expr_settings stg);
void semantic_analyze_expr_greater_equal(ast_node *node, expr_settings stg);
void semantic_analyze_expr_assign(ast_node *node, expr_settings stg);
void semantic_analyze_expr_cast(ast_node *node, expr_settings stg);




