#ifndef DEBUG_INFO_VAR_GUARD
#define DEBUG_INFO_VAR_GUARD
#include "util/vector2.h"

struct debugInfoVar{
	const char *IR_name;
	const char *username;
	const char *visible_from;
	const char *visible_to;
	int pointerlevel;
	int pos;
	int framedepth;
};
typedef struct debugInfoVar *ptr_debugInfoVar;
definition_vector_of(ptr_debugInfoVar);


struct debugInfoVar debugInfoVar_default();
struct debugInfoVar debugInfoVar_here(
	const char *IR_name,
	const char *username,
	const char *visible_from,
	const char *visible_to,
	int pointerlevel,
	int pos,
	int framedepth);

struct debugInfoVar* debugInfoVar_new0();
struct debugInfoVar* debugInfoVar_new(
	const char *IR_name,
	const char *username,
	const char *visible_from,
	const char *visible_to,
	int pointerlevel,
	int pos,
	int framedepth);

#endif