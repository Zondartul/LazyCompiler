#include "symtypes/symbol_const.h"


struct symbol_const symbol_const_default(){
	struct symbol_const S;
	S.type = 0;
	S.data_int = 0;
	S.data_float = 0;
	S.data_string = 0;
	return S;
}

struct symbol_const symbol_const_here(
	enum symbol_const_type type,
	int data_int,
	float data_float,
	const char *data_string
	){
	struct symbol_const S;
	S.type = type;
	S.data_int = data_int;
	S.data_float = data_float;
	S.data_string = data_string;
	return S;
}
