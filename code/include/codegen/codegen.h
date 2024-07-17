#ifndef CODEGEN_GUARD
#define CODEGEN_GUARD
//void codegen_generate();
#include "types/IR_symbol.h"
#include "types/frame.h"
#include "types/reg.h"
#include "types/debugInfoFunc.h"
#include "types/debugInfoVar.h"
#include "types/code_segment.h"
#include "util/vector3int.h"
#include "types/symbol.h"
#include "types/symbol_table.h"

//constants
#define DEBUGMEM 68866
#define if_first(x) {static int count = 0; if(!first++){x;}}
#define if_not_first(x) {static int first = 0; if(first++){x;}}
#define funcstamp() concat2(5,"// @ ",__func__,":",my_itoa(__LINE__),(codegen_decl? " D":" I")) 
#define asm_println(...) asm_println2(funcstamp(), __VA_ARGS__ )
#define asm_printblk(...) asm_printblk2(funcstamp(), __VA_ARGS__ )


//generation
//vars
extern struct code_segment *currentCodeSegment;
extern int debugInvade;
extern int debugInvadeHalt;
extern int comments;
extern FILE *fasm;
extern int CurCMD;
extern vector2_ptr_char stringStore_labels;
extern vector2_ptr_char stringStore_strings;
extern ptr_code_segment CurCS;
extern ptr_frame curframe;
extern ptr_frame firstframe;
extern vector2_ptr_reg registers;
extern vector2_ptr_frame frames;
extern vector2_ptr_frame framestack;
extern vector2_ptr_debugInfoFunc debug_funcs;
extern vector2_ptr_debugInfoVar debug_vars;
extern int codegen_decl;
extern const char *lbl_frameStart;
extern const char *lbl_frameEnd;
extern vector2_ptr_char frameStarts;
//vector2_int frameIndices;
extern vector3_int frameIndices;
extern vector3_int varstacks;
extern vector3_int argstacks;
extern int indent;
extern int cmd_index;
extern const char *codegen_str;
extern char *codegen_tok;
//funcs

typedef const char* cstr;
const char* concat2(int num, ...);
void asm_println2(const char *postfix, const char *fmt, ...);
void asm_printblk2(const char *prefix, const char *fmt, ...);
const char* my_itoa(int N);

void printindent();
void printTraceCode();
void printDebugInfo();
void printStringStore();
void printGlobalVars();
const char *stringStore_add(const char *str);
void printStringStore();
void printTraceHelper(const char *str);
void printTrace();
void print_skeleton_start();
void print_skeleton_end();
void printTraceCode();
void countindent(const char *str);
void printindent();
void resizeStacks();
void new_frame();
void make_first_frame();
void printframe(ptr_frame F);
void printGlobalVars();
ptr_frame find_frame_by_cmd_index(int idx);
void push_frame();
void pop_frame();
void storeValue(const char *val, const char *reg);
ptr_debugInfoFunc find_debugInfoFunc(const char* name);
ptr_debugInfoVar find_debugInfoVar(const char* name);
void initializeRegTable();
ptr_reg allocRegister();
int isnumber(const char *str);
ptr_symbol find_symbol_by_ir_name(ptr_symbol_table ST, const char *name);
void printDebugInfo();
int allocStackVar(int size);
int allocStackArg(int size);
const char* loadRValue(const char *val);
const char* loadLValue(const char* val);
void storeValue(const char *val, const char *reg);
void checkResult(const char *val);
void codegen_gen_command(/*ptr_code_segment CS, unused*/const char *str, int cmd_index);
void codegen_gen_code_segment(ptr_code_segment CS);
void codegen_generate();
int get_arg_size(const char* val);

#endif