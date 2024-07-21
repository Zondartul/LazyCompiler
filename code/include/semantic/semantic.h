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

 //this section aims to remove all push_expr and such
typedef enum expr_type {
	E_ERROR = 0, E_DISCARD, E_RVAL, E_LVAL, E_PTR, E_FPTR, E_ASIS, E_LIST
} expr_type;

typedef struct val_handle {
	const char* val;
	expr_type rv_type;
	struct type_name* T;
	//struct symbol* sym_this;
	const char* author;  //this is to indicate the expr/node that made this
} val_handle;

definition_vector_of(val_handle);

typedef struct expr_settings {
	//expr_type res_type;   //this tells the analyzer if node should output an rvalue or lvalue or nothing
	//const char* res_dest; //if given, result should be put into this existing IR-val
	//const char** res_out; //out-ptr for actual value of result
	//struct type_name** res_out_type; //out-ptr for actual var type of result
	//struct symbol* sym_this; //symbol (struct) from which to look up symbols
	val_handle dest;	//what value we want filled
	val_handle *actual;	//what was it actually filled with
	val_handle sem_this; //the 'this' value for a method call (but not for it's other args)
	val_handle *out_sem_this; // fill out with 'this' value if discovered by expr_dot
	vector2_val_handle *out_list;
} expr_settings;

#define EXPR_SETTINGS_NULL ((expr_settings){E_ERROR,0,0,0,0})


typedef struct if_settings {
	const char* else_label;
	const char* exit_label;
	const char** out_else_label;
	const char** out_exit_label;
} if_settings;
#define IF_SETTINGS_NULL ((if_settings){0,0,0,0})

//void output_res(expr_settings stg, const char* res_val, struct type_name* T);
#define YES_EMIT 1
#define NO_EMIT 0
void output_res(expr_settings stg, val_handle src, int do_emit);
//types

#define PREP_RES(res, type) \
	val_handle res = { .rv_type = E_ERROR }; \
	val_handle res##dest = { .rv_type = type }; \
	vector2_val_handle res##_list = vector2_val_handle_here(); \
	expr_settings res##stg = { .dest = res##dest, .actual = & res, .out_list = &res##_list };
#define VERIFY_RES(res) if((res.rv_type == E_ERROR) || (res.val==0)){error("internal semantic error: invalid expression result");}

//generated

//vars
extern int semantic_decl;
extern int need_global_initializers;
extern struct code_segment *init_CS;
extern struct code_segment *global_CS;
extern struct code_segment *global_init_CS;
extern struct code_segment *global_deinit_CS;
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
void emit_all_declarations(int use_this);
#define USE_THIS 1
#define DONT_USE_THIS 0
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
void semantic_general_analyze(struct ast_node *node);
void semantic_expr_analyze(struct ast_node* node, expr_settings stg);
void semantic_if_analyze(struct ast_node* node, if_settings stg);
int semantic_dispatch_general(struct ast_node* node);
int semantic_dispatch_expr(struct ast_node* node, expr_settings stg);
int semantic_dispatch_if(struct ast_node* node, if_settings stg);
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
const char *sanitize_string(const char* str);


const char* rename_star(const char* val);
const char* rename_unstar(const char* val);
const char* rename_and(const char* val);
const char* rename_unand(const char* val);

#endif