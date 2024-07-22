#include "types/type_name.h"
#include "types/types.h"
#include "assert.h"
#include "semantic.h" // for sanitize_string()
#include "types/symbol.h"
#include "globals.h" // for error()
implementation_vector_of(ptr_type_name);


struct type_name type_name_default(){
	struct type_name S;
	S.name = 0;
	S.points_to = 0;
	S.symclass = 0;
	S.args = 0;
	S.is_literal = 0;
	S.is_array = 0;
	S.arraysize = 0;
	return S;
}

struct type_name type_name_here(
	const char *name,
	struct type_name *points_to,
	struct symbol *symclass,
	struct vector2_ptr_type_name *args,
	int is_literal,
	int is_array,
	int arraysize
	){
	struct type_name S;
	S.name = name;
	S.points_to = points_to;
	S.symclass = symclass;
	S.args = args;
	S.is_literal = is_literal;
	S.is_array = is_array;
	S.arraysize = arraysize;
	return S;
}

struct type_name* type_name_new0(){
	struct type_name *S = (struct type_name*)malloc(sizeof(struct type_name));
	*S = type_name_default();
	return S;
}

struct type_name* type_name_new(
	const char *name,
	struct type_name *points_to,
	struct symbol *symclass,
	struct vector2_ptr_type_name *args,
	int is_literal,
	int is_array,
	int arraysize
	){
	struct type_name *S = (struct type_name*)malloc(sizeof(struct type_name));
	S->name = name;
	S->points_to = points_to;
	S->symclass = symclass;
	S->args = args;
	S->is_literal = is_literal;
	S->is_array = is_array;
	S->arraysize = arraysize;
	return S;
}

int type_name_is_sane(struct type_name *T){
	if(T->name){sanitize_string(T->name);}

	if(T->points_to){
		int res = type_name_is_sane(T->points_to);
		if(!res){error("type_name->points_to broken type");}
	}

	if(T->symclass){
		if(T->symclass->type != SYMBOL_CLASS){error("type_name->symclass is not a class");}
	}

	if(T->args){
		if(T->args->size == 0){error("type_name: function with no return type");}
		else{
			for(int i = 0; i < T->args->size; i++){
				int res = type_name_is_sane(m(*(T->args), get, i));
				if(!res){error("type_name function args broken");}
			}
		}
	}

	#define IS_BOOL(x) ((x == 0) || (x == 1))
	if(!IS_BOOL(T->is_array)){error("type_name->is_array should be BOOL (0 or 1)");}
	if(T->arraysize < 0){error("type_name->arraysize can't be negative");}

	int is_function = (T->args != 0);
	int is_pointer = (T->points_to != 0);
	//int is_class = (T->symclass != 0);

	if(!is_function && !is_pointer && !T->name){error("primitive type with no name");}
	if(is_pointer && is_function){error("type_name can't be both a pointer and a function");}

	return 1; // is all good
}

struct type_name *type_name_shallow_copy(struct type_name* T){
	struct type_name *res = type_name_new0();
	*res = *T;
	return res;
}

const char *type_name_to_string(struct type_name *T){
	struct vector2_char vstr = vector2_char_here();

	int is_function = (T->args != 0);
	int is_pointer = (T->points_to != 0);
	int is_class = (T->symclass != 0);

	#define ARG(i) m(*(T->args),get,i)

	if(is_function){
		assert(T->args->size > 0);
		vec_printf(&vstr, "%s ", ARG(0));

		vec_printf(&vstr, is_pointer? "(func*)(" : "func(");
		for(int i = 1; i < T->args->size; i++){
			const char *arg_str = type_name_to_string(ARG(i));
			vec_printf(&vstr, (i == 1)? "%s" : ", %s", arg_str);
		}
		vec_printf(&vstr, ")");
	}else{
		if(is_class){vec_printf(&vstr, "class ");}
		if(is_pointer){
			const char *deref_str = type_name_to_string(T->points_to);
			vec_printf(&vstr, "%s*", deref_str);
		}else{
			vec_printf(&vstr, "%s", T->name);
		}
	}
	return stralloc(vstr.data);
}


int type_args_equals(struct vector2_ptr_type_name *args1, struct vector2_ptr_type_name *args2){
	if(!args1 && args2){return 0;}
	if(args1 && !args2){return 0;}
	if(!args1 && !args2){return 1;}
	if(args1->size != args2->size){return 0;}
	
	for(int i = 0; i < args1->size; i++){
		int eq = type_name_equals(m(*args1, get, i), m(*args2, get, i));
		if(!eq){return 0;}
	}
	return 1;
}
int type_name_equals(struct type_name *T1, struct type_name *T2){
	//const char *name;
	//int pointerlevel;
	//struct symbol *symclass;
	//struct vector2_ptr_type_name *args;
	if(T1->name && T2->name && strcmp(T1->name,T2->name) != 0){return 0;}
	if((T1->name && !T2->name) || (!T1->name && T2->name)){return 0;}
	if(T1->points_to){
		if(T2->points_to){
			int res = type_name_equals(T1->points_to, T2->points_to);
			if(!res){return 0;}
		}else{return 0;}
	}else{
		if(T2->points_to){return 0;}
	}
	if(T1->symclass != T2->symclass){return 0;}
	if(!type_args_equals(T1->args, T2->args)){return 0;}
	if(T1->is_literal != T2->is_literal){return 0;}
	return 1;
}
