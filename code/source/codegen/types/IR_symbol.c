#include "types/IR_symbol.h"

implementation_vector_of(ptr_IR_symbol);



struct IR_symbol IR_symbol_default(){
	struct IR_symbol S;
	S.IR_name = 0;
	S.username = 0;
	S.type = 0;
	S.lbl_at = 0;
	S.lbl_from = 0;
	S.lbl_to = 0;
	S.temp = 0;
	S.pointerlevel = 0;
	S.pos = 0;
	S.framedepth = 0;
	S.arraysize = 0;
	S.str = 0;
	S.scope = 0;
	return S;
}

struct IR_symbol IR_symbol_here(
	const char *IR_name,
	const char *username,
	const char *type,
	const char *lbl_at,
	const char *lbl_from,
	const char *lbl_to,
	int temp,
	int pointerlevel,
	int pos,
	int framedepth,
	int arraysize,
	const char *str,
	struct frame *scope
	){
	struct IR_symbol S;
	S.IR_name = IR_name;
	S.username = username;
	S.type = type;
	S.lbl_at = lbl_at;
	S.lbl_from = lbl_from;
	S.lbl_to = lbl_to;
	S.temp = temp;
	S.pointerlevel = pointerlevel;
	S.pos = pos;
	S.framedepth = framedepth;
	S.arraysize = arraysize;
	S.str = str;
	S.scope = scope;
	return S;
}

struct IR_symbol* IR_symbol_new0(){
	struct IR_symbol *S = (struct IR_symbol*)malloc(sizeof(struct IR_symbol));
	*S = IR_symbol_default();
	return S;
}

struct IR_symbol* IR_symbol_new(
	const char *IR_name,
	const char *username,
	const char *type,
	const char *lbl_at,
	const char *lbl_from,
	const char *lbl_to,
	int temp,
	int pointerlevel,
	int pos,
	int framedepth,
	int arraysize,
	const char *str,
	struct frame *scope){
	struct IR_symbol *S = (struct IR_symbol*)malloc(sizeof(struct IR_symbol));
	S->IR_name = IR_name;
	S->username = username;
	S->type = type;
	S->lbl_at = lbl_at;
	S->lbl_from = lbl_from;
	S->lbl_to = lbl_to;
	S->temp = temp;
	S->pointerlevel = pointerlevel;
	S->pos = pos;
	S->framedepth = framedepth;
	S->arraysize = arraysize;
	S->str = str;
	S->scope = scope;
	return S;
}