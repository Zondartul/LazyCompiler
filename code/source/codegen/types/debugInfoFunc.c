#include "types/debugInfoFunc.h"

implementation_vector_of(ptr_debugInfoFunc);


struct debugInfoFunc debugInfoFunc_default(){
	struct debugInfoFunc S;
	S.IR_name = 0;
	S.username = 0;
	S.entrance = 0;
	S.exit = 0;
	return S;
}

struct debugInfoFunc debugInfoFunc_here(
	const char *IR_name,
	const char *username,
	const char *entrance,
	const char *exit){
	struct debugInfoFunc S;
	S.IR_name = IR_name;
	S.username = username;
	S.entrance = entrance;
	S.exit = exit;
	return S;
}

struct debugInfoFunc* debugInfoFunc_new0(){
	struct debugInfoFunc *S = (struct debugInfoFunc*)malloc(sizeof(struct debugInfoFunc));
	*S = debugInfoFunc_default();
	return S;
}

struct debugInfoFunc* debugInfoFunc_new(
	const char *IR_name,
	const char *username,
	const char *entrance,
	const char *exit){
	struct debugInfoFunc *S = (struct debugInfoFunc*)malloc(sizeof(struct debugInfoFunc));
	S->IR_name = IR_name;
	S->username = username;
	S->entrance = entrance;
	S->exit = exit;
	return S;
}
