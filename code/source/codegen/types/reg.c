#include "types/reg.h"

implementation_vector_of(ptr_reg);


struct reg reg_default(){
	struct reg S;
	S.name = 0;
	S.val = 0;
	S.age = 0;
	return S;
}

struct reg reg_here(
	const char *name,
	const char *val,
	int age
	){
	struct reg S;
	S.name = name;
	S.val = val;
	S.age = age;
	return S;
}

struct reg* reg_new0(){
	struct reg *S = (struct reg*)malloc(sizeof(struct reg));
	*S = reg_default();
	return S;
}

struct reg* reg_new(
	const char *name,
	const char *val,
	int age){
	struct reg *S = (struct reg*)malloc(sizeof(struct reg));
	S->name = name;
	S->val = val;
	S->age = age;
	return S;
}