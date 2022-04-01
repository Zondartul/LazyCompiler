#ifndef SYMBOL_GUARD
#define SYMBOL_GUARD

#include "symtypes/symbol_class.h"
#include "symtypes/symbol_const.h"
#include "symtypes/symbol_function.h"
#include "symtypes/symbol_variable.h"
#include "util/vector2.h"

enum symboltype {SYMBOL_ERROR, SYMBOL_VARIABLE, SYMBOL_PARAM, SYMBOL_MEMBER, SYMBOL_FUNCTION, SYMBOL_CLASS, SYMBOL_LABEL, SYMBOL_CONST};
enum symbolstorage {STORE_ERROR, STORE_CODE, STORE_DATA_DYNAMIC, STORE_DATA_POINTER, STORE_DATA_STATIC, STORE_DATA_STACK, STORE_DATA_MEMBER};
	

struct symbol{
	enum symboltype type;
	enum symbolstorage storage;
	int store_adr;
	int framedepth;
	int global;
	const char *username; //name in source code
	const char *IR_name; //name in IR code
	
	union{
		struct symbol_variable symvariable; //also for param
		struct symbol_const symconst;
		//struct symbol_member symmember;
		//struct symbol_parameter symparameter;
		struct symbol_function symfunction;
		struct symbol_class symclass;
	};// impl;
};
//---------- structure internal stuff --------------
typedef struct symbol* ptr_symbol;
definition_vector_of(ptr_symbol);


struct symbol symbol_default();
struct symbol symbol_here(
	enum symboltype type,
	enum symbolstorage storage,
	int store_adr,
	int framedepth,
	int global,
	const char *username,
	const char *IR_name
	);

struct symbol* symbol_new0();
struct symbol* symbol_new(
	enum symboltype type,
	enum symbolstorage storage,
	int store_adr,
	int framedepth,
	int global,
	const char *username,
	const char *IR_name
	);
//----------- functions to work with the struct

//in the current symbol table, or in a symbol table accessible from it,
//find a symbol with the given name. Errors if symbol is not found.
struct symbol *lookup_symbol(const char *name);
//return the symbol or zero if it is not found.
struct symbol* try_lookup_symbol(const char* name);
//look for a symbol but only in the given symtable, not parents
struct symbol* try_lookup_symbol_local(const char* name, struct symbol_table* ST);

//helper function for above
struct symbol *lookup_symbol_helper(const char *name, struct symbol_table *T);
//in the current symbol table, or a symbol table accessible from it,
//find a symbol with the matching IR_name.
struct symbol *lookup_symbol_IR(const char *name);
//helper for above.
struct symbol *lookup_symbol_IR_helper(const char *name, struct symbol_table *T);

#endif