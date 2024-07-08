#ifndef SYMBOL_TABLE_GUARD
#define SYMBOL_TABLE_GUARD
#include "types/symbol.h"
#include "util/vector2.h"

struct symbol_table{
	const char *name;	//name of this table
	struct ast_node *node;	//node this table is associated with
	vector2_ptr_symbol symbols; //actual symbols
	struct symbol_table *parent; //table in whose scope this one is
};
//---------- structure internal things
typedef struct symbol_table* ptr_symbol_table;
definition_vector_of(ptr_symbol_table);


struct symbol_table symbol_table_default();
struct symbol_table symbol_table_here(
	const char *name,			
	struct ast_node *node,		
	vector2_ptr_symbol symbols, 
	struct symbol_table *parent
	);

struct symbol_table* symbol_table_new0();
struct symbol_table* symbol_table_new(
	const char *name,			
	struct ast_node *node,		
	vector2_ptr_symbol symbols, 
	struct symbol_table *parent
	);
//----------- functions to work with the structure
//-- usual stack stuff
extern struct symbol_table *currentSymbolTable;
extern vector2_ptr_symbol_table ST_stack;
extern vector2_ptr_symbol_table ST_list;
extern void push_symbol_table();
extern void pop_symbol_table();

//creates a new symbol table in the context of the semantic analyzer
//and sets it as current.
void new_symbol_table(struct ast_node *node);
//fetches a symbol table that corresponds to the given node
//from the listof all symbol tables
ptr_symbol_table find_symbol_table_by_node(struct ast_node *node);

//what it says on the tin.
void print_symbol_table_helper_short(struct symbol_table *T, int indent);
void print_symbol_table_helper(struct symbol_table *T, int indent);
void print_symbol_table(struct symbol_table *T);

#endif