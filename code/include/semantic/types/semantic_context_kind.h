#ifndef SEMANTIC_CONTEXT_KIND_GUARD
#define SEMANTIC_CONTEXT_KIND_GUARD
#include "util/vector2.h"

enum semantic_context_kind {SEMANTIC_NORMAL, SEMANTIC_PARAM, SEMANTIC_MEMBER};
typedef enum semantic_context_kind* ptr_semantic_context_kind;

definition_vector_of(ptr_semantic_context_kind);

//stack of contexts used by the semantic analyzer,
//mirrors the scopes of functions?
extern enum semantic_context_kind semantic_context; //current context
extern vector2_ptr_semantic_context_kind semantic_context_stack;
void push_semantic_context();
void pop_semantic_context();


#endif