#include "symtypes/symbol_function.h"

struct symbol_function symbol_function_default(){
	struct symbol_function S;
	//S.returntype = 0;
	S.signature = 0;
	S.scope = 0;
	S.code = 0;
	return S;
}

struct symbol_function symbol_function_here(
	//struct type_name *returntype,
	struct type_name *signature,
	struct symbol_table *scope,
	struct code_segment *code
	){
	struct symbol_function S;
	//S.returntype = returntype;
	S.signature = signature;
	S.scope = scope;
	S.code = code;
	return S;
}