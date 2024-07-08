#ifndef CODE_SEGMENT_GUARD
#define CODE_SEGMENT_GUARD
#include "types/types.h"
#include "util/vector2.h"

struct code_segment{
	const char *name;
	struct symbol_table *scope;
	vector2_ptr_char commands;
	int indent;
};
//--------- struct internal funcs
typedef struct code_segment* ptr_code_segment;
definition_vector_of(ptr_code_segment);

struct code_segment code_segment_default();
struct code_segment code_segment_here(
	const char *name,
	struct symbol_table *scope,
	vector2_ptr_char commands,
	int indent
	);

struct code_segment* code_segment_new0();
struct code_segment* code_segment_new(
	const char *name,
	struct symbol_table *scope,
	vector2_ptr_char commands,
	int indent
	);
//-------- functions to work with the struct
extern ptr_code_segment currentCodeSegment;
extern vector2_ptr_code_segment CS_stack;
extern vector2_ptr_code_segment CS_list;
void push_code_segment();
void pop_code_segment();

void new_code_segment();


void print_code_segment(struct code_segment *CS);


#endif