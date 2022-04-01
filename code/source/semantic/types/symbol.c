#include "types/symbol.h"
#include "types/symbol_table.h"
#include "semantic.h"
#include "globals.h"

implementation_vector_of(ptr_symbol);


struct symbol symbol_default(){
	struct symbol S;
	S.type = SYMBOL_ERROR;
	S.storage = STORE_ERROR;
	S.store_adr = 0;
	S.framedepth = 0;
	S.global = 0;
	S.username = 0;
	S.IR_name = 0;
	return S;
}

struct symbol symbol_here(
	enum symboltype type,
	enum symbolstorage storage,
	int store_adr,
	int framedepth,
	int global,
	const char *username,
	const char *IR_name
	){
	struct symbol S;
	S.type = type;
	S.storage = storage;
	S.store_adr = store_adr;
	S.framedepth = framedepth;
	S.global = global;
	S.username = username;
	S.IR_name = IR_name;
	return S;
}

struct symbol* symbol_new0(){
	struct symbol *S = (struct symbol*)malloc(sizeof(struct symbol));
	*S = symbol_default();
	return S;
}

struct symbol* symbol_new(
	enum symboltype type,
	enum symbolstorage storage,
	int store_adr,
	int framedepth,
	int global,
	const char *username,
	const char *IR_name
	){
	struct symbol *S = (struct symbol*)malloc(sizeof(struct symbol));
	S->type = type;
	S->storage = storage;
	S->store_adr = store_adr;
	S->framedepth = framedepth;
	S->global = global;
	S->username = username;
	S->IR_name = IR_name;
	return S;
}
//---------- methods to work with the structure

//in the current symbol table, or in a symbol table accessible from it,
//find a symbol with the given name.
struct symbol* try_lookup_symbol(const char* name) {
	//if (strcmp(name, "trap") == 0) {
	//	printf("trap symbol\n");
	//}
	struct symbol* S = lookup_symbol_helper(name, currentSymbolTable);
	//debug
	//if (S && (strcmp(name, "this")==0)) {
	//	//another trap
	//	S = lookup_symbol_helper(name, currentSymbolTable);
	//}
	return S;
}

struct symbol *lookup_symbol(const char *name){
	struct symbol* S = try_lookup_symbol(name);
	if(S){
		return S;
	}else{
		//printf("SEMANTIC ERROR. printing semantic info so far:\n");
		//print_semantic();
		printf("semantic error: undefined symbol '%s'->'%s'\n",name,currentSymbolTable->name);
		printf("symbol table so far:\n");
		print_symbol_table(currentSymbolTable);
		int length = curNode->token.pos.last_column - curNode->token.pos.first_column;
		if(length == 1){length = 0;}
		//point_out_error(curNode->token.pos.first_line,curNode->token.pos.first_column,curNode->token.pos.filename,"",length);
		printf("also, pos = \n");
		printPos(curNode->token.pos);
		printf("===============\n");
		fflush(stdout);
		error("semantic error: undefined symbol '%s' (looking in symtable %s)\n",name,currentSymbolTable->name);
		return 0;
	}	
}

struct symbol* try_lookup_symbol_local(const char* name, struct symbol_table* ST) {
	int i;
	for (i = 0; i < ST->symbols.size; i++) {
		struct symbol* S;
		//S = vector_get_reference(&T->symbols, i);
		S = m(ST->symbols, get, i);
		if (S->username && !strcmp(S->username, name)) {
			printf("found symbol '%s'->'%s'\n", name, ST->name);
			return S;
		}
	}
	return 0;
}

//helper function for above
struct symbol *lookup_symbol_helper(const char *name, struct symbol_table *ST){
	struct symbol* S = try_lookup_symbol_local(name, ST);
	if (!S && ST->parent) {
		S = lookup_symbol_helper(name, ST->parent);
	}
	return S;
}

//in the current symbol table, or a symbol table accessible from it,
//find a symbol with the matching IR_name.
struct symbol *lookup_symbol_IR(const char *name){
	struct symbol *S = lookup_symbol_IR_helper(name, currentSymbolTable);
	if(S){
		return S;
	}else{
		//printf("SEMANTIC ERROR. printing semantic info so far:\n");
		//print_semantic();
		error("semantic error: undefined IR symbol '%s'->'%s'\n",name,currentSymbolTable->name);
		return 0;
	}	
}

//helper for above.
struct symbol *lookup_symbol_IR_helper(const char *name, struct symbol_table *T){
	int i;
	for(i = 0; i < T->symbols.size; i++){
		struct symbol *S;
		//S = vector_get_reference(&T->symbols, i);
		S = m(T->symbols,get,i);
		if(!strcmp(S->IR_name,name)){
			printf("found symbol '%s'->'%s'\n",name,T->name);
			return S;
		}
	}
	if(T->parent){
		return lookup_symbol_IR_helper(name, T->parent);
	}else{
		return 0;
	}
}
//================ OLD STUFF ======================
//struct symbol_variable make_symbol_variable(){
//	struct symbol_variable S;
//	S.type = 0;
//	S.pos = 0;
//	S.size = 0;
//	S.arraysize = 0;
//	S.array = 0;
//	return S;
//}
/* struct symbol_parameter{
	struct type_name *type;
	int pos;
};
struct symbol_member{
	struct type_name *type;
}; */

//struct symbol_function make_symbol_functio(){
//	struct symbol_function S;
//	S.returntype = 0;
//	S.signature = 0;
//	S.scope = 0;
//	S.code = 0;
//	return S;
//}


//struct symbol_const make_symbol_const(){
//	struct symbol_const S;
//	S.type = CONST_INT;
//	S.data_int = 0;
//	S.data_float = 0.f;
//	S.data_string = 0;
//}


//struct symbol_class make_symbol_class(){
//	struct symbol_class S;
//	S.scope = 0;
//	S.defaultConstructor = 0;
//	S.defaultDestructor = 0;
//	return S;
//}




//void symbol_setVariant(struct symbol *this, int type){
//	this->type = type;
//	switch(type){
//		case SYMBOL_VARIABLE: this->impl.symvariable = make_symbol_variable(); break;
//		case SYMBOL_PARAM:	  this->impl.symvariable = make_symbol_variable(); break;
//		case SYMBOL_FUNCTION: this->impl.symfunction = make_symbol_function(); break;
//		case SYMBOL_CLASS:	  this->impl.symclass	 = make_symbol_class();    break;
//		case SYMBOL_LABEL:	    /* do nothing */ break;
//		case SYMBOL_CONST:	  this->impl.symconst	 = make_symbol_const();    break;
//		case SYMBOL_ERROR: 	  	/* do nothing */ break;
//		case SYMBOL_MEMBER: 	error("symbol member variant (unimpl)\n"); break;
//		default: 				error("unknownn symbol type variant (unimpl)\n"); break;
//	};
//}
//struct symbol_variable *symbol_getvar_variable(struct symbol *this){if(this->type == SYMBOL_VARIABLE){return &(this->impl.symvariable);}else{return 0;}}
//struct symbol_variable *symbol_getvar_param   (struct symbol *this){if(this->type == SYMBOL_PARAM)	 {return &(this->impl.symvariable);}else{return 0;}}
//struct symbol_function *symbol_getvar_function(struct symbol *this)(if(this->type == SYMBOL_FUNCTION){return &(this->impl.symfunction);}else{return 0;}}
//struct symbol_class	   *symbol_getvar_class   (struct symbol *this){if(this->type == SYMBOL_CLASS)	 {return &(this->impl.symclass);   }else{return 0;}}
//struct symbol_const    *symbol_getvar_const   (struct symbol *this){if(this->type == SYMBOL_CONST)	 {return &(this->impl.symconst);   }else{return 0;}}
