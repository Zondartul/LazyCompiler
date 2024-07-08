#ifndef AST_TOKEN_GUARD
#define AST_TOKEN_GUARD
//#include "yaccin.tab.h"
#include "util/globals.h" //for YYLTYPE
#include "util/vector2.h"
#include "util/cobject.h"

typedef struct ast_token{
	const char *type;
	int production;
	const char *value;
	YYLTYPE pos;
} ast_token;


struct ast_token ast_token_default();
struct ast_token ast_token_here(
	const char *type,
	int production,
	const char *value,
	YYLTYPE pos
	);

struct ast_token* ast_token_new0();
struct ast_token* ast_token_new(
	const char *type,
	int production,
	const char *value,
	YYLTYPE pos);

#endif