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
#include "vector.h"
#include "AST.h"

struct symbol_table{
	vector classes;
	vector functions;
	vector variables;
};

struct symbol_class{
};

struct type_name{
	int primitive;
	char *name;
	struct symbol_class *symbol;
};
struct type_name *semantic_get_type(const char *str);
#endif