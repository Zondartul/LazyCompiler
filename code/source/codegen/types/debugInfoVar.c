#include "types/debugInfoVar.h"

implementation_vector_of(ptr_debugInfoVar);


struct debugInfoVar debugInfoVar_default(){
	struct debugInfoVar S;
	S.IR_name = 0;
	S.username = 0;
	S.visible_from = 0;
	S.visible_to = 0;
	S.pointerlevel = 0;
	S.pos = 0;
	S.framedepth = 0;
	return S;
}

struct debugInfoVar debugInfoVar_here(
	const char *IR_name,
	const char *username,
	const char *visible_from,
	const char *visible_to,
	int pointerlevel,
	int pos,
	int framedepth){
	struct debugInfoVar S;
	S.IR_name = IR_name;
	S.username = username;
	S.visible_from = visible_from;
	S.visible_to = visible_to;
	S.pointerlevel = pointerlevel;
	S.pos = pos;
	S.framedepth = framedepth;
	return S;
}

struct debugInfoVar* debugInfoVar_new0(){
	struct debugInfoVar *S = (struct debugInfoVar*)malloc(sizeof(struct debugInfoVar));
	*S = debugInfoVar_default();
	return S;
}

struct debugInfoVar* debugInfoVar_new(
	const char *IR_name,
	const char *username,
	const char *visible_from,
	const char *visible_to,
	int pointerlevel,
	int pos,
	int framedepth){
	struct debugInfoVar *S = (struct debugInfoVar*)malloc(sizeof(struct debugInfoVar));
	S->IR_name = IR_name;
	S->username = username;
	S->visible_from = visible_from;
	S->visible_to = visible_to;
	S->pointerlevel = pointerlevel;
	S->pos = pos;
	S->framedepth = framedepth;
	return S;
}
