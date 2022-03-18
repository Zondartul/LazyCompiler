#include "types/type_name.h"

implementation_vector_of(ptr_type_name);


struct type_name type_name_default(){
	struct type_name S;
	S.name = 0;
	S.pointerlevel = 0;
	S.symclass = 0;
	S.args = 0;
	return S;
}

struct type_name type_name_here(
	const char *name,
	int pointerlevel,
	struct symbol *symclass,
	struct vector2_ptr_type_name *args
	){
	struct type_name S;
	S.name = name;
	S.pointerlevel = pointerlevel;
	S.symclass = symclass;
	S.args = args;
	return S;
}

struct type_name* type_name_new0(){
	struct type_name *S = (struct type_name*)malloc(sizeof(struct type_name));
	*S = type_name_default();
	return S;
}

struct type_name* type_name_new(
	const char *name,
	int pointerlevel,
	struct symbol *symclass,
	struct vector2_ptr_type_name *args
	){
	struct type_name *S = (struct type_name*)malloc(sizeof(struct type_name));
	S->name = name;
	S->pointerlevel = pointerlevel;
	S->symclass = symclass;
	S->args = args;
	return S;
}
