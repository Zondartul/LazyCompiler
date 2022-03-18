#include "symtypes/symbol_variable.h"

struct symbol_variable symbol_variable_default(){
	struct symbol_variable S;
	S.type = 0;
	S.pos = 0;
	S.size = 0;
	S.arraysize = 0;
	S.array = 0;
	return S;
}

struct symbol_variable symbol_variable_here(
	struct type_name *type,
	int pos,
	int size,
	int arraysize,
	int array
	){
	struct symbol_variable S;
	S.type = type;
	S.pos = pos;
	S.size = size;
	S.arraysize = arraysize;
	S.array = array;
	return S;
}