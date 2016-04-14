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
	struct type_name *type;
	struct symbol_table *scope;
	struct code_segment *code;
	//will add code segment here later
};
struct symbol_class{
	struct symbol_table *scope;
};
struct symbol{
	enum {SYMBOL_VARIABLE, SYMBOL_PARAM, SYMBOL_FUNCTION, SYMBOL_CLASS, SYMBOL_ERROR}type;
	const char *name;
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
void push_symbol(struct symbol *S);


struct symbol_table{
	//vector symbols;
	vector2_ptr_symbol symbols;
	struct symbol_table *parent;
};
typedef struct symbol_table* ptr_symbol_table;
definition_vector_of(ptr_symbol_table);
struct symbol_table *new_symbol_table();
//makes a new symbol table, pushes it as current;
struct symbol_table *new_symbol_table_make_current();
void push_symbol_table(struct symbol_table *ST);
//return to old symbol table;
void pop_symbol_table();
void print_symbol_table(struct symbol_table *T);


struct type_name{
	//enum{TYPE_CLASS, TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_VOID} type;
	const char *name;
	struct symbol *symclass;
};
struct type_name *semantic_get_type(const char *str);

typedef const char* ptr_char;
definition_vector_of(ptr_char);
struct code_segment{
	struct symbol_table *scope;
	//vector commands; //vector<char*>
	vector2_ptr_char commands;
};
typedef struct code_segment* ptr_code_segment;
definition_vector_of(ptr_code_segment);
struct code_segment *currentCodeSegment;
struct code_segment *new_code_segment();
struct code_segment *new_code_segment_make_current();
void push_code_segment(struct code_segment *CS);
void pop_code_segment();
void emit_code(const char *fmt, ...);
char *IR_next_name(const char *prefix);
void push_expr(const char *expr);
const char *pop_expr();
void semantic_analyze(struct ast_node *node);
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