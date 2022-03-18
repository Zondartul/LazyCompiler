#ifndef SYMBOL_CONST_GUARD
#define SYMBOL_CONST_GUARD


enum symbol_const_type {CONST_INT,CONST_FLOAT,CONST_STRING};

struct symbol_const{
	enum symbol_const_type type;
	int data_int;
	float data_float;
	const char *data_string;
};

struct symbol_const symbol_const_default();
struct symbol_const symbol_const_here(
	enum symbol_const_type type,
	int data_int,
	float data_float,
	const char *data_string
	);

#endif