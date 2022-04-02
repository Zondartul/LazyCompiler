#ifndef FRAME_H_GUARD
#define FRAME_H_GUARD
#include "types/IR_symbol.h"
#include "types/types.h"
#include "util/vector2.h"

struct frame{
	int depth;
	int cmd_index;
	const char *lbl_from;
	const char *lbl_to;
	vector2_ptr_IR_symbol symbols;
	struct frame *parent;
	vector2_ptr_char *namespace;
	int stackvarsize;
	int stackargsize;
	int isStruct;
};

typedef struct frame *ptr_frame;
definition_vector_of(ptr_frame);

struct frame frame_default();

struct frame frame_here(
	int depth,
	int cmd_index,
	const char *lbl_from,
	const char *lbl_to,
	vector2_ptr_IR_symbol symbols,
	struct frame *parent,
	vector2_ptr_char *namespace,
	int stackvarsize,
	int stackargsize);

struct frame* frame_new0();

struct frame* frame_new(
	int depth,
	int cmd_index,
	const char *lbl_from,
	const char *lbl_to,
	vector2_ptr_IR_symbol symbols,
	struct frame *parent,
	vector2_ptr_char *namespace,
	int stackvarsize,
	int stackargsize);

#endif