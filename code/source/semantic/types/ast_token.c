#include "types/ast_token.h"

struct ast_token ast_token_default(){
	struct ast_token S;
	S.type = 0;
	S.production = 0;
	S.value = 0;
	S.pos = nullPos();
	return S;
}

struct ast_token ast_token_here(
	const char *type,
	int production,
	const char *value,
	YYLTYPE pos
	){
	struct ast_token S;
	S.type = type;
	S.production = production;
	S.value = value;
	S.pos = pos;
	return S;
}

struct ast_token* ast_token_new0(){
	struct ast_token *S = (struct ast_token*)malloc(sizeof(struct ast_token));
	*S = ast_token_default();
	return S;
}

struct ast_token* ast_token_new(
	const char *type,
	int production,
	const char *value,
	YYLTYPE pos){
	struct ast_token *S = (struct ast_token*)malloc(sizeof(struct ast_token));
	S->type = type;
	S->production = production;
	S->value = value;
	S->pos = pos;
	return S;
} 
