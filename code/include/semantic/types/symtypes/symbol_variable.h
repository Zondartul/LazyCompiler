#ifndef SYMBOL_VARIABLE_GUARD
#define SYMBOL_VARIABLE_GUARD

struct symbol_variable{
	struct type_name *type;
	int pos;
	int size;//maybe int size, unless that's for codegen to decide?
	int arraysize;
	int array;
};	

struct symbol_variable symbol_variable_default();
struct symbol_variable symbol_variable_here(
	struct type_name *type,
	int pos,
	int size,
	int arraysize,
	int array
	);

#endif