#include "types/code_segment.h"
#include "semantic.h" //for IR_next_name()

implementation_vector_of(ptr_code_segment);


struct code_segment code_segment_default(){
	struct code_segment S;
	S.name = 0;
	S.scope = 0;
	vector2_ptr_char_constructor(&(S.commands));
	return S;
}

struct code_segment code_segment_here(
	const char *name,
	struct symbol_table *scope,
	vector2_ptr_char commands,
	int indent
	){
	struct code_segment S;
	S.name = name;
	S.scope = scope;
	S.commands = commands;
	S.indent = indent;
	return S;
}

struct code_segment* code_segment_new0(){
	struct code_segment *S = (struct code_segment*)malloc(sizeof(struct code_segment));
	*S = code_segment_default();
	return S;
}

struct code_segment* code_segment_new(
	const char *name,
	struct symbol_table *scope,
	vector2_ptr_char commands,
	int indent
	){
	struct code_segment *S = (struct code_segment*)malloc(sizeof(struct code_segment));
	S->name = name;
	S->scope = scope;
	S->commands = commands;
	S->indent = indent;
	return S;
}

//-------- functions to work with the struct -----
ptr_code_segment currentCodeSegment = 0;
vector2_ptr_code_segment CS_stack;
vector2_ptr_code_segment CS_list;

void new_code_segment(){
	struct code_segment *CS = malloc(sizeof(struct code_segment));
	CS->name = IR_next_name(namespace_semantic,"CS");
	CS->scope = currentSymbolTable;
	//vector_constructor(&CS->commands,sizeof(char*));
	CS->commands = vector2_ptr_char_here();
	CS->indent = CS_stack.size;
	m(CS_list,push_back,CS);
	currentCodeSegment = CS;
}

void push_code_segment(){
	m(CS_stack,push_back,currentCodeSegment);
}

void pop_code_segment(){
	currentCodeSegment = m(CS_stack,pop_back);
}

void print_code_segment(struct code_segment *CS){
	//printf("code segment '%s'->'%s'\n",CS->name,CS->scope->name);
	int i;
	for(i = 0; i < CS->commands.size; i++){
		printf(" %s\n",m(CS->commands,get,i));
	}
	//printf("end code segment\n");
}