#ifndef SYMBOL_FUNCTION_GUARD
#define SYMBOL_FUNCTION_GUARD

struct symbol_function{
	//struct type_name *returntype;
	struct type_name *signature;
	struct symbol_table *scope;
	struct code_segment *code;
	//will add code segment here later
};

struct symbol_function symbol_function_default();

struct symbol_function symbol_function_here(
	//struct type_name *returntype,
	struct type_name *signature,
	struct symbol_table *scope,
	struct code_segment *code
	);

#endif