#include "types/semantic_context_kind.h"

implementation_vector_of(ptr_semantic_context_kind);


enum semantic_context_kind semantic_context;

vector2_ptr_semantic_context_kind semantic_context_stack;

void push_semantic_context(){
	ptr_semantic_context_kind sc = (enum semantic_context_kind*)malloc(sizeof(enum semantic_context_kind));
	*sc = semantic_context;
	m(semantic_context_stack,push_back,sc);
}

void pop_semantic_context(){
	ptr_semantic_context_kind sc = m(semantic_context_stack,pop_back);
	semantic_context = *sc;
	free(sc);
}
