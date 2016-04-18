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
#include "AST.h"
#include "stdarg.h"
/* struct symbol_table{
	vector classes;
	vector functions;
	vector variables;
};

struct symbol_class{
};

 */
 void semantic_init();
 void semantic_finalize();
struct symbol;
struct symbol_table;
struct type_name;
struct code_segment;
struct varvalue{
	union{
		int valint;
		float valfloat;
		char *valstring;
	};
};
struct symbol_variable{
	struct type_name *type;
	
};	
struct symbol_parameter{
	struct type_name *type;
	int pos;
};
struct symbol_function{
	struct type_name *returntype;
	struct type_name *signature;
	struct symbol_table *scope;
	struct code_segment *code;
	//will add code segment here later
};
struct symbol_class{
	struct symbol_table *scope;
};
struct symbol{
	enum {SYMBOL_VARIABLE, SYMBOL_PARAM, SYMBOL_FUNCTION, SYMBOL_CLASS, SYMBOL_ERROR}type;
	const char *username; //name in source code
	const char *IR_name; //name in IR code
	union{
		struct symbol_variable symvariable;
		struct symbol_parameter symparameter;
		struct symbol_function symfunction;
		struct symbol_class symclass;
	};
};
typedef struct symbol* ptr_symbol;
definition_vector_of(ptr_symbol);
struct symbol *new_symbol();
struct symbol *lookup_symbol(const char *name);
struct symbol *lookup_symbol_IR(const char *name);
void push_symbol(struct symbol *S);


struct symbol_table{
	const char *name;	//name of this table
	vector2_ptr_symbol symbols; //actual symbols
	struct symbol_table *parent; //table in whose scope this one is
};
typedef struct symbol_table* ptr_symbol_table;
definition_vector_of(ptr_symbol_table);
void new_symbol_table();
void push_symbol_table();
void pop_symbol_table();
/* //makes a new symbol table, pushes it as current;
struct symbol_table *new_symbol_table_make_current();
struct symbol_table *new_symbol_table_make_current_standalone();
void push_symbol_table(struct symbol_table *ST);
//return to old symbol table;
void pop_symbol_table(); */
void print_symbol_table(struct symbol_table *T);

struct type_name;
typedef struct type_name* ptr_type_name;
struct vector2_ptr_type_name;
struct type_name{
	//enum{TYPE_CLASS, TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_VOID} type;
	const char *name;
	struct symbol *symclass;
	struct vector2_ptr_type_name *args;
};
definition_vector_of(ptr_type_name);
struct type_name *semantic_get_type(const char *str);

typedef const char* ptr_char;
definition_vector_of(ptr_char);
struct code_segment{
	const char *name;
	struct symbol_table *scope;
	//vector commands; //vector<char*>
	vector2_ptr_char commands;
	int indent;
};
typedef struct code_segment* ptr_code_segment;
definition_vector_of(ptr_code_segment);
struct code_segment *currentCodeSegment;
void new_code_segment();
void push_code_segment();
void pop_code_segment();
/* struct code_segment *new_code_segment_make_current();
struct code_segment *new_code_segment_make_current_standalone();
void push_code_segment(struct code_segment *CS);
void pop_code_segment(); */
void emit_code(const char *fmt, ...);
void emit_code_segment(struct code_segment *CS);
char *IR_next_name(const char *prefix);
void push_expr(const char *expr);
const char *pop_expr();
void push_exprtype(struct type_name *T);
struct type_name *pop_exprtype();
void semantic_analyze(struct ast_node *node);
void print_semantic();
// scope1 
// 
// 
// 
// 
// 
// 
// 
// 
// 

#endif