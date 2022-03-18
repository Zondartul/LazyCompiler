#ifndef TYPE_NAME_GUARD
#define TYPE_NAME_GUARD
#include "util/vector2.h"

struct type_name;
typedef struct type_name* ptr_type_name;
struct vector2_ptr_type_name;

struct type_name{
	//enum{TYPE_CLASS, TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING, TYPE_VOID} type;
	const char *name;
	int pointerlevel;
	struct symbol *symclass;
	struct vector2_ptr_type_name *args;
	//put indirection number / ptr-to-ptr-to-ptr number here
};

definition_vector_of(ptr_type_name);

struct type_name *semantic_get_type(const char *str);


struct type_name type_name_default();
struct type_name type_name_here(
	const char *name,
	int pointerlevel,
	struct symbol *symclass,
	struct vector2_ptr_type_name *args
	);

struct type_name* type_name_new0();
struct type_name* type_name_new(
	const char *name,
	int pointerlevel,
	struct symbol *symclass,
	struct vector2_ptr_type_name *args
	);


#endif