#ifndef TYPE_NAME_GUARD
#define TYPE_NAME_GUARD
#include "util/vector2.h"

struct type_name;
typedef struct type_name* ptr_type_name;
struct vector2_ptr_type_name;

struct type_name{
	//enum{TYPE_CLASS, TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_VOID} type;
	const char *name;
	struct type_name *points_to;
	struct symbol *symclass;
	struct vector2_ptr_type_name *args;
	int is_literal;
	int is_array;
	int arraysize;
	//put indirection number / ptr-to-ptr-to-ptr number here
};

definition_vector_of(ptr_type_name);

struct type_name *semantic_get_type(const char *str);


struct type_name type_name_default();
struct type_name type_name_here(
	const char *name,
	struct type_name *points_to,
	struct symbol *symclass,
	struct vector2_ptr_type_name *args,
	int is_literal,
	int is_array,
	int arraysize
	);

struct type_name* type_name_new0();
struct type_name* type_name_new(
	const char *name,
	struct type_name *points_to,
	struct symbol *symclass,
	struct vector2_ptr_type_name *arg,
	int is_literal,
	int is_array,
	int arraysize
	);
const char *type_name_to_string(struct type_name *T);
int type_args_equals(struct vector2_ptr_type_name *args1, struct vector2_ptr_type_name *args2);
int type_name_equals(struct type_name *T1, struct type_name *T2);
int type_name_is_sane(struct type_name *T);
struct type_name *type_name_shallow_copy(struct type_name* T);

#endif