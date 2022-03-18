#ifndef IR_SYMBOL_GUARD
#define IR_SYMBOL_GUARD
#include "util/vector2.h"

struct IR_symbol{
	const char *IR_name;	//name of the symbol in the IR code, unique over the entire program
	const char *username;	//user-readable name of the symbol
	const char *type;		//typename of the value type
	const char *lbl_at;		//assembly label that denotes the RAM location of the global variable (if framedepth = 0)
	const char *lbl_from;	//assembly label from which symbol visibility starts (unused)
	const char *lbl_to;		//assembly label at which symbol visibility ends	(unused)
	int temp;				//1 for codegen-generated symbols to store intermediary expression results (unused)
	int pointerlevel;		//number of stars in the "int *****ptr" pointer type
	int pos;				//frame-relative stack address of the variable (if framedepth > 0)
	int framedepth;			//number of scope-inside-scope levels for the frame that stores this var
	int arraysize;			//capacity of the array (num copies or num bytes?)
	const char *str;		//string, if the symbol is a string literal (type = STRING).
	struct frame *scope;	//if the symbol is a struct, this frame is it's scope.
};
typedef struct IR_symbol *ptr_IR_symbol;
definition_vector_of(ptr_IR_symbol);

ptr_IR_symbol new_IR_symbol();
ptr_IR_symbol find_IR_symbol(const char *IR_name);


struct IR_symbol IR_symbol_default();
struct IR_symbol IR_symbol_here(
	const char *IR_name,
	const char *username,
	const char *type,
	const char *lbl_at,
	const char *lbl_from,
	const char *lbl_to,
	int temp,
	int pointerlevel,
	int pos,
	int framedepth,
	int arraysize,
	const char *str,
	struct frame *scope
	);

struct IR_symbol* IR_symbol_new0();
struct IR_symbol* IR_symbol_new(
	const char *IR_name,
	const char *username,
	const char *type,
	const char *lbl_at,
	const char *lbl_from,
	const char *lbl_to,
	int temp,
	int pointerlevel,
	int pos,
	int framedepth,
	int arraysize,
	const char *str,
	struct frame *scope);

#endif