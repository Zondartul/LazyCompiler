#ifndef DEBUG_INFO_FUNC_GUARD
#define DEBUG_INFO_FUNC_GUARD
#include "util/vector2.h"

struct debugInfoFunc{
	const char *IR_name;
	const char *username;
	const char *entrance;
	const char *exit;
};
typedef struct debugInfoFunc *ptr_debugInfoFunc;
definition_vector_of(ptr_debugInfoFunc);

struct debugInfoFunc debugInfoFunc_default();
struct debugInfoFunc debugInfoFunc_here(
	const char *IR_name,
	const char *username,
	const char *entrance,
	const char *exit);

struct debugInfoFunc* debugInfoFunc_new0();
struct debugInfoFunc* debugInfoFunc_new(
	const char *IR_name,
	const char *username,
	const char *entrance,
	const char *exit);

#endif