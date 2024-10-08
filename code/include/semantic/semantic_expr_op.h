#include "semantic.h"
#include "semantic_analyze.h"

//--- value-introducing expressions
void semantic_analyze_expr_const(ast_node* node, expr_settings stg);
void semantic_analyze_expr_id(ast_node* node, expr_settings stg);
//--- reference-calcuation expressions
void semantic_analyze_expr_list(ast_node* node, expr_settings stg);
void semantic_analyze_expr_subexpr(ast_node* node, expr_settings stg);
void semantic_analyze_expr_index(ast_node* node, expr_settings stg);
void semantic_analyze_expr_call(ast_node* node, expr_settings stg);
void semantic_analyze_expr_dot(ast_node* node, expr_settings stg);
void semantic_analyze_expr_deref(ast_node* node, expr_settings stg);
void semantic_analyze_expr_ref(ast_node* node, expr_settings stg);
//--- unary ops pure -------
void semantic_analyze_expr_not(ast_node* node, expr_settings stg);
void semantic_analyze_expr_neg(ast_node* node, expr_settings stg);
//--- unary ops with side effects ---
void semantic_analyze_expr_increment(ast_node* node, expr_settings stg);
void semantic_analyze_expr_decrement(ast_node* node, expr_settings stg);
//--- infix ops pure ---
void semantic_analyze_expr_op_ifx(ast_node* node, const char* OP, expr_settings stg);
void semantic_analyze_expr_pow(ast_node* node, expr_settings stg);
void semantic_analyze_expr_divide(ast_node* node, expr_settings stg);
void semantic_analyze_expr_multiply(ast_node* node, expr_settings stg);
void semantic_analyze_expr_modulo(ast_node* node, expr_settings stg);
void semantic_analyze_expr_and(ast_node* node, expr_settings stg);
void semantic_analyze_expr_or(ast_node* node, expr_settings stg);
void semantic_analyze_expr_minus(ast_node* node, expr_settings stg);
void semantic_analyze_expr_plus(ast_node* node, expr_settings stg);
void semantic_analyze_expr_equals(ast_node* node, expr_settings stg);
void semantic_analyze_expr_notequal(ast_node* node, expr_settings stg);
void semantic_analyze_expr_greater(ast_node* node, expr_settings stg);
void semantic_analyze_expr_less(ast_node* node, expr_settings stg);
//--- infix ops with side effects ---
void semantic_analyze_expr_assign(ast_node* node, expr_settings stg);

/// --------- internal funcs ---------------
/// internal use - check types before doing stuff
void typecheck_assign(val_handle res1, val_handle res2);
struct type_name* dereffed_type(struct type_name* T);
val_handle convert_rv_type(val_handle res_in, enum expr_type new_rv, const char *author);
