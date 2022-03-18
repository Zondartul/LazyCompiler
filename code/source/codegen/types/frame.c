#include "types/frame.h"

implementation_vector_of(ptr_frame);


struct frame frame_default(){
	struct frame S;
	S.depth = 0;
	S.cmd_index = 0;
	S.lbl_from = 0;
	S.lbl_to = 0;
	vector2_ptr_IR_symbol_constructor(&(S.symbols));
	S.parent = 0;
	S.namespace = 0;
	S.stackvarsize = 0;
	S.stackargsize = 0;
	return S;
}

struct frame frame_here(
	int depth,
	int cmd_index,
	const char *lbl_from,
	const char *lbl_to,
	vector2_ptr_IR_symbol symbols,
	struct frame *parent,
	vector2_ptr_char *namespace,
	int stackvarsize,
	int stackargsize){
	struct frame S;
	S.depth = depth;
	S.cmd_index = cmd_index;
	S.lbl_from = lbl_from;
	S.lbl_to = lbl_to;
	S.symbols = symbols;
	S.parent = parent;
	S.namespace = namespace;
	S.stackvarsize = stackvarsize;
	S.stackargsize = stackargsize;
	return S;
}

struct frame* frame_new0(){
	struct frame *S = (struct frame*)malloc(sizeof(struct frame));
	*S = frame_default();
	return S;
}

struct frame* frame_new(
	int depth,
	int cmd_index,
	const char *lbl_from,
	const char *lbl_to,
	vector2_ptr_IR_symbol symbols,
	struct frame *parent,
	vector2_ptr_char *namespace,
	int stackvarsize,
	int stackargsize){
	struct frame *S = (struct frame*)malloc(sizeof(struct frame));
	S->depth = depth;
	S->cmd_index = cmd_index;
	S->lbl_from = lbl_from;
	S->lbl_to = lbl_to;
	S->symbols = symbols,
	S->parent = parent;
	S->namespace = namespace;
	S->stackvarsize = stackvarsize;
	S->stackargsize = stackargsize;
	return S;
}