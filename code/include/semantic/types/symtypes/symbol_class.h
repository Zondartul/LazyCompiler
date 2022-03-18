#ifndef SYMBOL_CLASS_GUARD
#define SYMBOL_CLASS_GUARD

struct symbol_class{
	struct symbol_table *scope;
	struct symbol *defaultConstructor;
	struct symbol *defaultDestructor;
};


struct symbol_class symbol_class_default();
struct symbol_class symbol_class_here(
	struct symbol_table *scope,
	struct symbol *defaultConstructor,
	struct symbol *defaultDestructor
	);
	
#endif