#ifndef REG_GUARD
#define REG_GUARD
#include "util/vector2.h"

struct reg{
	const char *name;
	const char *val;
	int age;
};
typedef struct reg* ptr_reg;
definition_vector_of(ptr_reg);

ptr_reg new_reg(const char *name, const char *val, int age);

struct reg reg_default();
struct reg reg_here(
	const char *name,
	const char *val,
	int age
	);

struct reg* reg_new0();
struct reg* reg_new(
	const char *name,
	const char *val,
	int age);

#endif