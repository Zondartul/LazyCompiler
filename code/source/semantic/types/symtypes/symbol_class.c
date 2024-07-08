#include "symtypes/symbol_class.h"


struct symbol_class symbol_class_default(){
	struct symbol_class S;
	S.scope = 0;
	S.defaultConstructor = 0;
	S.defaultDestructor = 0;
	return S;
}

struct symbol_class symbol_class_here(
	struct symbol_table *scope,
	struct symbol *defaultConstructor,
	struct symbol *defaultDestructor
	){
	struct symbol_class S;
	S.scope = scope;
	S.defaultConstructor = defaultConstructor;
	S.defaultDestructor = defaultDestructor;
	return S;
}
