#ifndef SEMANTIC_GUARD
#define SEMANTIC_GUARD
/*

variable type types:
primitive - 1 cell - char, int, float, pointer
array	-	char[], elephant[] = pointer to complex
complex - horse - own symbol table

symbol_table:
	vector<class_symbol>
	vector<function_symbol>
	vector<variable_symbol>

array:
	*type

complex:
	*symbol_table

symbol:
	class(name, symbol_table)
	function(type, name, arguments, symbol_table, code_segment)
	variable(type, name)
	

*/
//#include "vector.h"
/* struct symbol_table{
	vector classes;
	vector functions;
	vector variables;
};

struct symbol_class{
};

 */
#include "vector2.h"
#include "AST.h"
#include "types/symbol.h"
#include "types/code_segment.h"
#include "types/symbol_table.h"
#include "types/file.h"
#include "types/type_name.h"
#include "types/semantic_context_kind.h"

//types

//generated

//vars
extern int semantic_decl;
extern struct code_segment *init_CS;
extern ptr_symbol symbolThis;

extern vector2_ptr_char expr_stack;
extern vector2_ptr_type_name exprtype_stack;
extern vector2_ptr_char *namespace_semantic;
extern int semantic_analyze_pad;
extern const char *semantic_this;
extern int semantic_flatten;
extern int lvalue;
extern struct ast_node *curNode;
extern int emit_code_pad;
extern int nodeCountDecl;
extern int nodeCountImp;


//functions
void padprint(int indent);
void semantic_init();
void semantic_finalize();
struct symbol;
struct symbol_table;
struct type_name;
struct code_segment;
//struct symbol *new_symbol();
void push_symbol(struct symbol *S);

//wtf is with ast_node not being visible
void analyze_scope(ptr_ast_node N,//struct ast_node *N, 
					struct code_segment **CSin,
					struct code_segment **CSout,
					struct symbol_table **STin,
					struct symbol_table **STout,
					int noEnterLeave);
void class_finalize();
void class_def_finalize();
void class_emit_start();
void class_emit_end();
void emit_all_declarations();
void emit_all_undeclarations();
void emit_all_initializers();
void emit_initializer(struct symbol *S);
void emit_deinitializer();
void emit_all_deinitializers();
void emit_code(const char *fmt, ...);
void emit_code_segment(struct code_segment *CS);
void IR_init();
char *IR_next_name(vector2_ptr_char *namespace, const char* prefix);
char *IR_exact_name(vector2_ptr_char *namespace, const char* prefix);
char *IR_inexact_name(vector2_ptr_char *namespace, const char* prefix);
void push_expr(const char *expr);
const char *pop_expr();
void push_exprtype(struct type_name *T);
struct type_name *pop_exprtype();
void semantic_analyze(struct ast_node *node);
void print_semantic();
int getNumParameters();
int getNumMembers();
int getNumVariables();
struct type_name *parseTypename(struct ast_node *N);
int getTypeSize(ptr_type_name);
extern vector2_ptr_code_segment CS_list;
char *escape_string(const char *str);
char *unescape_string(const char *str);
char *removeComments(const char *str);

#endif