#include "codegen.h"
#include "semantic.h"
#include "string.h"
#include "ctype.h"
#include "stdio.h"
#include "codegen_gen_command.h"
#include "assert.h"

// todo:
// make it so each function has only one frame
// no need to look at parent frame for any reason
// global vars have absolute address
// SYMBOL means var becomes visible (but space needs to be pre-allocated at entry)
// FORGET means var becomes invisible (space can be deallocated)
//
// allocation:
// check frame-claimed stackspace for FREE nodes
//	if found, return free node
//  if not found, claim more stackspace and return next node
// deallocated node becomes FREE but does not become unclaimed.
//
// alternative: all symbols preallocated and never deallocated
//

//vars
int debugInvade = 0;
int debugInvadeHalt = 1;
int comments = 1;
FILE *fasm;
int CurCMD = 0;
vector2_ptr_char stringStore_labels;
vector2_ptr_char stringStore_strings;
ptr_code_segment CurCS = 0;
ptr_frame curframe = 0;
ptr_frame firstframe = 0;
int codegen_decl = 0;
const char *lbl_frameStart = 0;
const char *lbl_frameEnd = 0;
vector2_ptr_char frameStarts;
vector3_int frameIndices;
vector2_ptr_frame frames;
vector2_ptr_frame framestack;
vector3_int varstacks;
vector3_int argstacks;
vector2_ptr_reg registers;
vector2_ptr_debugInfoFunc debug_funcs;
vector2_ptr_debugInfoVar debug_vars;
int cmd_index = 0;
const char *codegen_str = 0;
char *codegen_tok = 0;
int trace_gens = 1;	//if 1, every assembly line will have "emitted from here" trace



void asm_println2(const char* postfix, const char* fmt, ...) {
	assert(fmt);
	//1. print the args
	va_list args;
	va_start(args, fmt);
	vector2_char vstr = vector2_char_here();
	vec_vnprintf(&vstr, -1, fmt, args);
	va_end(args);
	
	//2. pad the end of the string with whitespace
	int len = vstr.size - 1;//strlen(buff);
	const char* pad80blanks = "                                                                                ";
	vec_nprintf(&vstr, 80 - len, pad80blanks);
	//int pos = 80;
	//for (int I = len; I < pos; I++) {
	//	buff[I] = ' ';
	//}
	
	//3. print the postfix string
	if (trace_gens) {
		vec_printf(&vstr, "%s", postfix);
	}

	//4. force the output to be on one line
	//len = strlen(buff);
	//for (int I = 0; I < len; I++) {
	//	if ((buff[I] == '\n') || (buff[I] == '\r')) {
	//		buff[I] = ' ';
	//	}
	//}
	len = vstr.size - 1;
	for (int I = 0; I < len; I++) {
		if ((vstr.data[I] == '\n') || (vstr.data[I] == '\r')) {
			vstr.data[I] = ' ';
		}
	}
	vec_printf(&vstr, "\n");
	fprintf(fasm, "%s", vstr.data);
}

void asm_printblk2(const char *prefix, const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	if(trace_gens){asm_println(prefix, "// block begin");}
	vfprintf(fasm, fmt, args);
	va_end(args);
	if(trace_gens){asm_println(prefix, "// block end");}
}


ptr_IR_symbol new_IR_symbol(){
	ptr_IR_symbol S = malloc(sizeof(struct IR_symbol));
	S->IR_name = "";
	S->username = "";
	S->type = 0;
	S->lbl_at = 0;
	S->lbl_from = 0;
	S->lbl_to = 0;
	S->temp = 0;
	S->pointerlevel = 0;
	S->pos = 0;
	S->framedepth = 0;
	S->arraysize = 0;
	S->scope = 0;
	return S;
}

const char *stringStore_add(const char *str){
	if(!curframe){error("[CODE GEN] no frame ");}
	const char *lbl = IR_next_name(firstframe->namespace,"str");
	m(stringStore_labels,push_back,lbl);
	m(stringStore_strings,push_back,stralloc(str));
	return lbl;
}

void printStringStore(){
	asm_println("string_store:");
	int i;
	for(i = 0; i < stringStore_labels.size; i++){
		const char *str_lbl = m(stringStore_labels,get,i);
		const char *str_str = m(stringStore_strings,get,i);
		asm_println("%s: db \"%s\",0",str_lbl,str_str);
	}
}

void printTraceHelper(const char *str){
	if(debugInvade){
		asm_println("#ifdef DEBUG");
		if(str){
			const char *lbl = stringStore_add(str);
			printindent();
			asm_println("push %s",lbl);
		}else{
			printindent();
			asm_println("push 0");
		}
		printindent();
		asm_println("call trace"); fprintf(fasm,"call trace\n");
		printindent();
		asm_println("inc esp");
		asm_println("#endif");
	}
}
void printTrace(){
	const char *str = m(CurCS->commands,get,CurCMD);
	printTraceHelper(str);
}

void print_skeleton_start(){
	int debugMem = DEBUGMEM;

	asm_println("code_segment_start:");
	asm_println("#define DEBUG");
	asm_println("mov ebp, 20000");
	asm_println("mov esp, 19999");
	asm_println("mov #%d, debug_info", debugMem+20);
	
	//CurEBP = 20000;
	printTraceHelper("program start");

	asm_println("section_code_body:");
}

void print_skeleton_end(){
	asm_println("code_segment_epilog:");
	
	printTraceCode();
	asm_println("code_segment_end:");
	asm_println("data_segment_dynamic_start:");
	printGlobalVars();
	asm_println("data_segment_dynamic_end:");
	asm_println("data_segment_static_start:");
	printStringStore();
	printDebugInfo();
	asm_println("data_segment_static_end:");
}

void printTraceCode(){
	int debugMem = DEBUGMEM;
	asm_printblk(
	"trace:\n"
	"cmp #%d,0\n"
	"je trace_exit\n"
	"pusha\n"
	"mov eax, %d\n"
	"mov eax:#0, ESP:#0\n"
	"mov eax:#1, ESP:#1\n"
	"mov eax:#2, ESP:#2\n"
	"mov eax:#3, ESP:#3\n"
	"mov eax:#4, ESP:#4\n"
	"mov eax:#5, ESP:#5\n"
	"mov eax:#6, ESP:#6\n"
	"mov eax:#7, ESP:#7\n"
	"mov eax:#8, ESP:#8\n"
	"mov eax:#9, ESP:#9\n"
	"mov eax:#10, ESP:#10\n"
	"mov eax:#11, ESP:#11\n"
	"mov eax:#12, ESP:#12\n"
	"mov eax:#13, ESP:#13\n"
	"mov eax:#14, ESP:#14\n"
	"mov eax:#15, ESP:#15\n"
	"mov eax:#16, ESP:#16\n"
	"mov eax:#17, ESP:#17\n"
	"mov eax:#19, ESP\n"
	"mov eax:#20, debug_info\n"
	"popa\n"
	,debugMem+18,debugMem);
	if(debugInvadeHalt){fprintf(fasm,"int 1\n");}
	asm_printblk(
	"ret\n"
	"trace_exit:\n"
	"ret\n"
	);
}

void new_frame(){
	ptr_frame F = malloc(sizeof(struct frame));
	if(curframe){
		F->depth = curframe->depth+1;
	}else{
		F->depth = 0;
	}
	F->cmd_index = 0;
	F->lbl_from = 0;
	F->lbl_to = 0;
	F->symbols = vector2_ptr_IR_symbol_here();
	F->parent = curframe;
	F->namespace = vector2_ptr_char_new();
	F->stackvarsize = 0;
	F->stackargsize = 0;
	F->isStruct = 0;
	curframe = F;
	m(frames,push_back,F);
	printf("new frame, depth:%d, vs.size:%d, as.size:%d\n",F->depth,varstacks.size,argstacks.size);
}

void make_first_frame(){
	new_frame();
	curframe->lbl_from = "code_segment_start";
	curframe->lbl_to = "code_segment_end";
	curframe->cmd_index = -1;
	firstframe = curframe;
}

void printframe(ptr_frame F){
	if(!F){printf("+------\n|(null)\n+------\n");return;}
	printf("+------------------\n");
	printf("|frame %p\n",F);
	printf("|frame.depth: %d\n",F->depth);
	printf("|frame.cmd_index: %d\n",F->cmd_index);
	printf("|frame.lbl_from: %s\n",F->lbl_from);
	printf("|frame.lbl_to: %s\n",F->lbl_to);
	printf("|frame.symbols.size: %d\n",F->symbols.size);
	printf("|frame.parent: %p\n",F->parent);
	printf("+------------------\n");
}

void asm_printdbstring(const char* lbl, const char* str) {
	//const char* str2 = escape_string(str);
	vector2_char vstr = vector2_char_here();
	vec_printf(&vstr, "%s: db ", lbl);
	int I = 0;
	while (str[I]) {
		vec_printf(&vstr, "%d", str[I]);
		if (str[I+1]) {
			vec_printf(&vstr, ", ");
		}
		I++;
	}
	vec_printf(&vstr, ", 0; //%s", str);
	asm_println("%s", vstr.data);
}

void printGlobalVars(){
	while(curframe->parent){curframe = curframe->parent;}
	for(int I = 0; I < curframe->symbols.size; I++){
		ptr_IR_symbol S = m(curframe->symbols,get,I);
		if(!strcmp(S->type,"VAR")){
			if(!S->arraysize){
				asm_println("%s: db 0 //%s",S->lbl_at,S->IR_name);
			}else{
				asm_println("%s: alloc %d //%s",S->lbl_at,S->arraysize,S->IR_name);
			}
		}
		if(!strcmp(S->type,"STRING")){
			asm_printdbstring(S->lbl_at, S->str);
			//asm_println("%s: db \"%s\",0",S->lbl_at,escape_string(S->str));
		}
	}
}

ptr_frame find_frame_by_cmd_index(int idx){
	int i;
	for(i = 0; i < frames.size; i++){
		ptr_frame F = m(frames,get,i);
		if(F->cmd_index == idx){return F;}
	}
	error("can't find frame for cmd_index %d\n",idx);
	return 0;
}

void push_frame(){
	m(framestack,push_back,curframe);
}

void pop_frame(){
	curframe = m(framestack,pop_back);
}

ptr_IR_symbol find_IR_symbol(const char *IR_name){
	int i;
	ptr_IR_symbol S;
	for(i = 0; i < curframe->symbols.size;i++){
		S = m(curframe->symbols,get,i);
		if(!strcmp(S->IR_name,IR_name)){
			return S;
		}
		if(!strcmp(S->type,"STRUCT")){
			if(!S->scope){error("[CODE GEN] struct without a scope ");}
			int j;
			for(j = 0; j < S->scope->symbols.size; j++){
				ptr_IR_symbol S2 = m(S->scope->symbols,get,j);
				if(!strcmp(S2->IR_name,IR_name)){
					return S2;
				}
			}
		}
	}
	if(curframe->parent){
		ptr_frame F = curframe;
		curframe = curframe->parent;
		S = find_IR_symbol(IR_name);
		curframe = F;
		return S;
	}
	return 0;
}

ptr_reg new_reg(const char *name, const char *val, int age){
	ptr_reg R = malloc(sizeof(struct reg));
	*R = (struct reg){name,val,age};
	return R;
}


ptr_debugInfoFunc find_debugInfoFunc(const char* name){
	int i;
	for(i = 0; i < debug_funcs.size; i++){
		ptr_debugInfoFunc F = m(debug_funcs,get,i);
		if(!strcmp(F->IR_name,name)){
			return F;
		}
	}
	return 0;
}

ptr_debugInfoVar find_debugInfoVar(const char* name){
	int i;
	for(i = 0; i < debug_vars.size; i++){
		ptr_debugInfoVar V = m(debug_vars,get,i);
		if(!strcmp(V->IR_name,name)){
			return V;
		}
	}
	return 0;
}

void initializeRegTable(){
	if_not_first(return);
	printf("initializing register table\n");
	registers = vector2_ptr_reg_here();
	m(registers,push_back,new_reg("R1",0,0));
	m(registers,push_back,new_reg("R2",0,0));
	m(registers,push_back,new_reg("R3",0,0));
	m(registers,push_back,new_reg("R4",0,0));
	m(registers,push_back,new_reg("R5",0,0));
	m(registers,push_back,new_reg("R6",0,0));
	printf("reg table initialize\n");
}

ptr_reg allocRegister(){
	initializeRegTable();
	static int age = 0;
	int i;
	for(i = 0; i < registers.size; i++){
		ptr_reg R = m(registers,get,i);
		if(R->val == 0){
			printf("allocated register %d (%s)\n",i,R->name);
			R->age = age++;
			return R;
		}
	}
	//all regs in use, need to spill.
	int minage = m(registers,get,0)->age;
	int minageI = 0;
	for(i = 1; i < registers.size; i++){
		ptr_reg R = m(registers,get,i);
		if(R->age < minage){minage = R->age; minageI = i;}
	}
	ptr_reg R = m(registers,get,minageI);

	if(R->val){if(comments){asm_println("//discarding %s from %s",R->val,R->name);}}
	R->val = 0;
	R->age = age++;
	printf("allocated register %d (%s)\n",minageI,R->name);
	return R;
}


ptr_symbol find_symbol_by_ir_name(ptr_symbol_table ST, const char *name){
	int i;
	for(i = 0; i < ST->symbols.size; i++){
		ptr_symbol S = m(ST->symbols,get,i);
		if(strcmp(S->IR_name,name)==0){
			return S;
		}
	}
	return 0;
}

void asm_printdb_nonnull(const char* str) {
	if (str == 0) {
		asm_println("0, //(null)");
	}
	else {
		asm_println("%s, ", str);
	}
}

void printDebugInfo(){
	printf("emitting debug info\n");
	int b_printframes = 0;
	int b_printfuncs = 1;
	int b_printvars = 1;

	int j;
	int i;
	int numFuncs = 0;
	int numVars = 0;

	asm_println("debug_info:");
	if (b_printframes) {
		fprintf(fasm,"debug_info_frames:\n");
		fprintf(fasm,"DB %d\n //number of frames\n",frames.size);
		fprintf(fasm,"//lbl_frm, lbl_to, depth\n");
		for(j = 0; j < frames.size; j++) {
			ptr_frame F = m(frames,get,j);
			fprintf(fasm,"DB %s,%s,%d\n",F->lbl_from,F->lbl_to,F->depth);
		}
	}
	if (b_printfuncs) {
		asm_println("debug_info_funcs:");
		for (j = 0; j < frames.size; j++) {
			ptr_frame F = m(frames, get, j);
			for (i = 0; i < F->symbols.size; i++) {
				ptr_IR_symbol S = m(F->symbols, get, i);
				if (!strcmp(S->type, "FUNC")) {
					numFuncs++;
				}
				if ((!strcmp(S->type, "VAR") || !strcmp(S->type, "ARG")) && (!S->temp)) {
					numVars++;
				}
			}
		}
		asm_println("DB %d\n //number of functions\n", numFuncs);
		if (comments) { asm_println("//[len][entrance,exit,[len][IR_name],[len][username]]\n"); }

		for (j = 0; j < frames.size; j++) {
			ptr_frame F = m(frames, get, j);
			for (i = 0; i < F->symbols.size; i++) {
				ptr_IR_symbol S = m(F->symbols, get, i);
				//record: db recordlen, data1, data2... datan
				//record2[1] = (record1+recordlen)[1]
				if (!strcmp(S->type, "FUNC")) {
					int str1len = 0;
					if (S->IR_name) { str1len = strlen(S->IR_name); }
					int record1len = str1len + 1;
					int str2len = 0;
					if (S->username) { str2len = strlen(S->username); }
					int record2len = str2len + 1;
					int recordlen = 3 + record1len + record2len;
					//fprintf(fasm,"DB %d, %s, %s,\"%s\",0\n",recordlen,F->entrance,F->exit,F->name);
					asm_println("DB %d, ", recordlen);
					asm_printdb_nonnull(S->lbl_from); //asm_println("%s, ", S->lbl_from);
					asm_printdb_nonnull(S->lbl_to);   //asm_println("%s, ", S->lbl_to);
					asm_println("%d, \"%s\",0, ", str1len, S->IR_name);
					asm_println("%d, \"%s\",0;", str2len, S->username);
				}
			}
		}
	}
	if (b_printvars) {
		asm_println("debug_info_vars:\n");
		asm_println("DB %d\n //number of variables\n", numVars);
		if (comments) { asm_println("//[len][visible_from,visible_to,pos,framedepth,[len][IR_name],[len][username]]\n"); }
		for (j = 0; j < frames.size; j++) {
			ptr_frame F = m(frames, get, j);
			for (i = 0; i < F->symbols.size; i++) {
				ptr_IR_symbol S = m(F->symbols, get, i);
				//record: db recordlen, data1, data2... datan
				//record2[1] = (record1+recordlen)[1]
				if ((!strcmp(S->type, "VAR") || !strcmp(S->type, "ARG")) && (!S->temp)) {
					int str1len = 0;
					if (S->IR_name) { str1len = strlen(S->IR_name); }
					int record1len = str1len + 1;
					int str2len = 0;
					if (S->username) { str2len = strlen(S->username); }
					int record2len = str2len + 1;
					int recordlen = 4 + record1len + record2len;

					//quick fix for that one bug where lbl_to is null.
					const char* lbl_from = S->lbl_from;
					if (!lbl_from) {lbl_from = "0";}
					const char* lbl_to = S->lbl_to;
					if (!lbl_to) {lbl_to = "0";}

					asm_println("\n//%s %s", S->type, S->IR_name);
					asm_println("DB %d //recordlen", recordlen);
					asm_println("DB %s //%s->lbl_from", lbl_from, S->IR_name);
					asm_println("DB %s //%s->lbl_to", lbl_to, S->IR_name);
					if (S->framedepth) {
						asm_println("DB %d //%s->pos", S->pos, S->IR_name);
					}
					else {
						asm_println("DB %s //%s->lbl_at", S->lbl_at, S->IR_name);
					}
					asm_println("DB %d //%s->framedepth", S->framedepth, S->IR_name);
					asm_println("DB %d, \"%s\",0 //%s->IR_name", str1len, S->IR_name, S->IR_name);
					asm_println("DB %d, \"%s\",0//%s->username", str2len, S->username, S->IR_name);
					//fprintf(fasm,"DB %d, %s, %s, %d, \"%s\",0\n",recordlen,V->visible_from,V->visible_to,V->framedepth,V->name);
				}
			}
		}
	}
}

int allocStackVar(int size){
	if(!curframe){error("[CODE GEN] no frame ");}
	//int cursize = curframe->stackvarsize; //unused
	curframe->stackvarsize+=size;
	return -curframe->stackvarsize; //remember that the stack grows downwards because magic
}

int allocStackArg(int size){
	if(!curframe){error("[CODE GEN] no frame ");}
	int cursize = curframe->stackargsize;
	curframe->stackargsize += size;
	return 2+cursize;
}
#define DEBUG_CHECK(x) if(strcmp(val, x) == 0)

/// NULL string MUST NOT reach assembly AT ANY COST
#define CHECK_STR(text) 	\
	assert(text && (strcmp(text,"") != 0) && (strcmp(text,"(null)") != 0))

//some register = value
//returns something you can put into "mov eax, X"
const char* loadRValue(const char* val) {
	//if(strcmp(val, "x_2") == 0){
	//	printf("debug breakpoint");
	//}

	if (!val) { goto err_null_val; }
	if (isnumber(val)) { goto out_val; }
	//DEBUG_CHECK("&derp"){
	//	printf("debug breakpoint");
	//}
	int acc_deref = 0;
	int acc_ref = 0;
	int acc_none = 0;
	//char buff[80];
	vector2_char vstr = vector2_char_here();

	if (val[0] == '*') { acc_deref = 1; val++; }
	if (val[0] == '&') { acc_ref = 1; val++; }
	if (acc_ref && acc_deref) {goto err_ref_deref;} //error("[CODE GEN] can't reference AND dereference");}
	acc_none = (!acc_deref) && (!acc_ref);

	ptr_IR_symbol S = find_IR_symbol(val);
	if (!S) { goto err_undefined; }
	
	int is_func = (strcmp(S->type, "FUNC") == 0);
	int is_label = (strcmp(S->type, "LABEL") == 0);
	int is_string = (strcmp(S->type, "STRING") == 0);
	int is_member = (strcmp(S->type, "MEMBER") == 0);
	int is_var = (strcmp(S->type, "VAR") == 0);
	int is_arg = (strcmp(S->type, "ARG") == 0);
	int is_var_arg = is_var || is_arg;
	int store_global = (S->framedepth == 0);
	int store_local = !store_global;
	int is_array = (S->arraysize != 0);
	int not_array = !is_array;

	if(is_func){
		if(acc_deref)				{goto err_deref_func;}	// ERROR
		if(acc_ref)					{goto err_ref_func;}	// ERROR
		if(acc_none)				{goto out_val;}			// 'x'
	}
	if(is_label){
		if(acc_deref)				{goto out_deref_val;}	// '#x'
		if(acc_ref)					{goto err_ref_lbl;}		// ERROR
		if(acc_none)				{goto out_val;}			// 'x'
	}
	if(is_string){
		if(acc_deref)				{goto out_deref_lbl_at;}// '#lbl'
		if(acc_ref)					{goto err_ref_str;}		// ERROR
		if(acc_none)				{goto out_lbl_at;}		// 'lbl'
	}
	if(is_member){
		if(acc_deref)				{goto out_member_deref;}// mov t, #lbl; '#t'
		if(acc_ref || is_array)		{goto out_pos;}			// '123'
		if(acc_none && not_array)	{goto out_lbl_at;}		//'lbl'
	}
	if(is_var_arg && store_global){
		if(acc_deref)				{goto out_global_deref;}// mov t, #lbl; 't'/'#t'
		if(acc_ref || is_array)		{goto out_lbl_at;}		// 'lbl'
		if(acc_none && not_array)	{goto out_deref_lbl_at;}// '#lbl'
	}
	if(is_var_arg && store_local){
		if(acc_deref)				{goto out_local_deref;}	// rstack t, EBP:123; 't'/'#t'
		if(acc_ref || is_array)		{goto out_stack_pos;}	// 'EBP:123'
		if(acc_none && not_array)	{goto out_deref_stack_pos;}	// 'EBP:#123'
	}
	goto err_bad_type;

err_bad_type:	error("[CODE GEN] symbol type is not recognized");
err_null_val:	error("[CODE GEN] trying to load null value ");
err_undefined:  error("[CODE GEN] undefined IR symbol '%s'", val);
err_ref_deref:  error("[CODE GEN] can't reference AND dereference (remove either * or &)");
err_deref_func: error("[CODE GEN] can't dereference a function (remove *)");
err_ref_func:	error("[CODE GEN] function names are already references (remove &)");
err_ref_lbl:	error("[CODE GEN] labels are already references (remove &)");
err_ref_str:	error("[CODE GEN] strings are already references (remove &)");

out_val: 
	CHECK_STR(val); 
	return stralloc(val);

out_deref_val: 
	CHECK_STR(val);
	vec_printf(&vstr, "#%s", val);
	return stralloc(vstr.data);

out_lbl_at: 
	CHECK_STR(S->lbl_at); 
	return stralloc(S->lbl_at);

out_deref_lbl_at:
	CHECK_STR(S->lbl_at);
	vec_printf(&vstr, "#%s", S->lbl_at);
	return stralloc(vstr.data);

out_pos:
	vec_printf(&vstr, "%d", S->pos);
	return stralloc(vstr.data);

out_member_deref:
{
	ptr_reg R = allocRegister();
	//const char* reg = R->name;
	CHECK_STR(val);
	CHECK_STR(S->lbl_at);
	CHECK_STR(R->name);
	R->val = stralloc(val);
	printindent(); asm_println("mov %s, #%s", R->name, S->lbl_at);
	vec_printf(&vstr, "#%s", R->name);
	return stralloc(vstr.data);
}
out_global_deref:
{
	ptr_reg R = allocRegister();
	//const char* reg = R->name;
	CHECK_STR(val);
	CHECK_STR(S->lbl_at);
	CHECK_STR(R->name);
	R->val = stralloc(val);
	printindent(); asm_println("mov %s, #%s", R->name, S->lbl_at);
	vec_printf(&vstr, (is_array ? "%s" : "#%s"), R->name);
	return stralloc(vstr.data);
}
out_local_deref:
{
	ptr_reg R = allocRegister();
	CHECK_STR(R->name);
	printindent(); asm_println("rstack %s, EBP:%d", R->name, S->pos);
	vec_printf(&vstr, (is_array? "%s" : "#%s"), R->name);
	return stralloc(vstr.data);
}
out_stack_pos:
	vec_printf(&vstr, "EBP:%d", S->pos);
	return stralloc(vstr.data);

out_deref_stack_pos:
	vec_printf(&vstr, "EBP:#%d", S->pos);
	return stralloc(vstr.data);

/*
	if (!strcmp(S->type, "FUNC")) {
		if (deref) { error("[CODE GEN] can't dereference a function "); }
		if (ref) { error("[CODE GEN] function names are already references (line %d)", CurCMD + 1); }
		return out_val(val);//goto ret_val; //return val;
	}
	if (!strcmp(S->type, "LABEL")) {
		if (deref) { return out_deref_val(&vstr, val);}//vec_printf(&vstr, "#%s", val); goto ret_vstr;}//return stralloc(vstr.data); }
		if (ref) { error("[CODE GEN] labels are already references "); }
		else { return out_val(val); }//goto ret_val; }//return val; }
	}
	if (!strcmp(S->type, "STRING")) {
		if (deref) { CHECK_vec_printf1(&vstr, "#%s", S->lbl_at); goto ret_vstr;}//return stralloc(vstr.data); }
		if (ref) { error("[CODE GEN] const strings are already references "); }
		else { goto ret_lbl_at;}//return S->lbl_at; }
	}
	if (!strcmp(S->type, "MEMBER")) {
		if (deref) {
			//todo: check if some register already contains that val
			ptr_reg R = allocRegister();
			const char* reg = R->name;
			R->val = stralloc(val);
			printindent();
			CHECK_asm_println_ss("mov %s, #%s", reg, S->lbl_at);
			CHECK_vec_printf_d(&vstr, "#%s", reg);
			goto ret_vstr;
		}
		if (ref || S->arraysize) {
			CHECK_vec_printf_d(&vstr, "%d", S->pos);
			goto ret_vstr;
		}
		CHECK_vec_printf1(&vstr, "#%s", S->lbl_at);
		goto ret_vstr;//return stralloc(vstr.data); //return stralloc(buff);
	}
	if (!strcmp(S->type, "VAR") || !strcmp(S->type, "ARG")) {
		if (S->framedepth == 0) {
			//global var
			if (deref) {
				//todo: check if some register already contains that val
				ptr_reg R = allocRegister();
				const char* reg = R->name;
				R->val = stralloc(val);
				printindent();
				CHECK_asm_println2("mov %s, #%s", reg, S->lbl_at);
				if(S->arraysize){
					CHECK_vec_printf1(&vstr, "%s", reg);
				}else{
					CHECK_vec_printf1(&vstr, "#%s", reg);
				}
				goto ret_vstr;
			}
			if (ref || S->arraysize) {
				//vec_printf(&vstr, "%d", S->pos);
				//return stralloc(vstr.data);//return stralloc(buff);
				/// hold on, if the var is global, then we should just plop down the label?
				goto ret_lbl_at;
			}
			CHECK_vec_printf1(&vstr, "#%s", S->lbl_at);
			goto ret_vstr;
		}
		else {
			//local var
			if (deref) {
				//todo: check if some register already contains that val
				ptr_reg R = allocRegister();
				const char* reg = R->name;
				R->val = stralloc(val);
				printindent();
				CHECK_asm_println2("rstack %s, EBP:%d", reg, S->pos);
				if(S->arraysize){
					CHECK_vec_printf1(&vstr, "%s", reg);
				}else{
					CHECK_vec_printf1(&vstr, "#%s", reg);
				}
				goto ret_vstr;
			}
			if (ref || S->arraysize) {
				CHECK_vec_printf1(&vstr, "EBP:%d", S->pos);
				goto ret_vstr;
			}
			CHECK_vec_printf1(&vstr, "EBP:#%d", S->pos);
			goto ret_vstr;
		}
	}
	error("[CODE GEN] trying to load unsupported symbol type %s ", S->type);
	return 0;



ret_vstr:	CHECK_AND_RETURN(vstr.data);
ret_lbl_at:	CHECK_AND_RETURN(S->lbl_at);
ret_val:	CHECK_AND_RETURN(val);
*/
}

void load_lbl_and_deref_n_times(const char* reg_name, const char *src, int derefs_requested){
	#define FIRST_LOAD_L  printindent(); asm_println("mov %s, %s",reg_name,src);	// 1st load
	#define FIRST_DEREF_L printindent(); asm_println("mov %s, #%s",reg_name,src);	// 1st deref
	#define NTH_DEREF_L   printindent(); asm_println("mov %s, #%s",reg_name,reg_name); //2nd...n'th deref
	switch(derefs_requested){
		case 0:	FIRST_LOAD_L;  break;
		case 1: FIRST_DEREF_L; break;
		case 2: FIRST_DEREF_L; NTH_DEREF_L; break;
		case 3: FIRST_DEREF_L; NTH_DEREF_L; NTH_DEREF_L; break;
		default: assert(!"unreachable");
	}
}

void load_stack_and_deref_n_times(const char* reg_name, int pos, int derefs_requested){
	#define FIRST_LOAD_S  printindent(); asm_println("mov %s, EBP:%d",reg_name,pos);	// 1st load
	#define FIRST_DEREF_S printindent(); asm_println("rstack %s, EBP:#%d",reg_name,pos);	// 1st deref
	#define NTH_DEREF_S   printindent(); asm_println("mov %s, #%s",reg_name,reg_name); //2nd...n'th deref
	switch(derefs_requested){
		case 0:	FIRST_LOAD_S;  break;
		case 1: FIRST_DEREF_S; break;
		case 2: FIRST_DEREF_S; NTH_DEREF_S; break;
		case 3: FIRST_DEREF_S; NTH_DEREF_S; NTH_DEREF_S; break;
		default: assert(!"unreachable");
	}
}



int count_derefs(int acc_ref, int acc_deref, int is_array, int is_pointer){
	int derefs_requested = 0;
	if(!acc_ref && !is_array)	{derefs_requested++;}
	if(acc_deref)				{derefs_requested++;}
	if(is_pointer)				{derefs_requested++;}
	return derefs_requested;
}

/// puts a frame pointer to N-th enclosing stack frame into the specified register 
void climb_n_frames(int framediff, const char *reg_name){
	printindent(); asm_println("mov %s, #EBP",reg_name); //frame climbing (r7 = EBP_prev)
	for(int i = 1; i < framediff; i++){
		printindent(); asm_println("mov %s, #%s", reg_name,reg_name);
	}
}

//same but also puts the value in a register
const char* loadLValue(const char* val){
	if(!val){error("[CODE GEN] trying to load null value ");}
	ptr_reg reg;
	
	if(isnumber(val)){goto out_load_val;}
	//stored value
	int acc_ref = 0;
	int acc_deref = 0;
	if(val[0] == '*'){acc_deref = 1; val++;}
	if(val[0] == '&'){acc_ref = 1; val++;}

	ptr_IR_symbol S = find_IR_symbol(val);
	if(!S){goto err_undefined;}
	
	int adr = S->pos;
	reg = allocRegister();
	int framediff = curframe->depth-S->framedepth;
	
	CHECK_STR(val); CHECK_STR(reg->name);
	if(comments){printindent(); asm_println("//load %s into %s (cf:%d, fd:%d)",val,reg->name,curframe->depth,S->framedepth);}
	reg->val = stralloc(val);
	

	int is_func = (strcmp(S->type, "FUNC") == 0);
	int is_label = (strcmp(S->type, "LABEL") == 0);
	//int is_string = (strcmp(S->type, "STRING") == 0); /// string literals can't be L-values
	int is_member = (strcmp(S->type, "MEMBER") == 0);
	int is_var = (strcmp(S->type, "VAR") == 0);
	int is_arg = (strcmp(S->type, "ARG") == 0);
	int is_var_arg = is_var || is_arg;
	int is_lbl_func = is_label || is_func;
	int store_global = (S->framedepth == 0);
	int store_local = !store_global;
	int is_array = (S->arraysize != 0);
	int derefs_requested = 0;
	int is_pointer = (S->pointerlevel != 0);


	if(is_member){
		if( acc_ref)		{goto out_load_lbl_at;}
		if(!acc_ref)		{goto err_member_ref;}
	}
	if(is_var_arg){
		if(store_global)	{goto out_load_lbl_at_with_derefs;}
		if(store_local){
			if(framediff  < 0){goto err_deeper_frame;}
			if(framediff == 0){goto out_load_stack_with_derefs;}
			if(framediff  > 0){goto out_local_frameclimb;}
		}
	}
	if(is_lbl_func)			{goto out_load_val_with_derefs;}

	goto err_bad_type;


err_bad_type:		error("[CODE GEN] symbol type is not recognized");
err_undefined:		error("[CODE GEN] undefined IR symbol '%s'", val);
err_member_ref:		error("[CODE GEN] member var %s may only be used as '&%s'", val, val);
err_deeper_frame:	error("[CODE GEN] attempt to access val [%s] in a deeper frame (%d) than current (%d)",val,S->framedepth,curframe);


out_load_val:
	//immediate value
	reg = allocRegister();
	CHECK_STR(val);
	CHECK_STR(reg->name);
	reg->val = stralloc(val);
	if(comments){printindent(); asm_println2("//load %s into %s",val, reg->name);}
	printindent(); asm_println("mov %s, %s",reg->name,val);
	return stralloc(reg->name);

out_load_lbl_at:
	CHECK_STR(reg->name);
	CHECK_STR(S->lbl_at);
	printindent(); asm_println("mov %s, %s",reg->name, S->lbl_at);
	// reg and reg->val are premade
	return stralloc(reg->name);

out_load_lbl_at_with_derefs:
	CHECK_STR(reg->name);
	CHECK_STR(S->lbl_at);
	derefs_requested = count_derefs(acc_ref, acc_deref, is_array, is_pointer);
	load_lbl_and_deref_n_times(reg->name, S->lbl_at, derefs_requested);
	// reg and reg->val are premade
	return stralloc(reg->name);

out_load_stack_with_derefs:
	CHECK_STR(reg->name);
	derefs_requested = count_derefs(acc_ref, acc_deref, is_array, is_pointer);
	load_stack_and_deref_n_times(reg->name, adr, derefs_requested);
	// reg and reg->val are premade
	return stralloc(reg->name);

out_local_frameclimb:
	CHECK_STR(reg->name);
	if(comments){printindent(); asm_println("//climbing %d frames",framediff);}
	climb_n_frames(framediff, reg->name);
	printindent(); asm_println("mov %s, %s:%d", reg->name, reg->name, adr); /// get address of local variable
	derefs_requested = count_derefs(acc_ref, acc_deref, is_array, is_pointer);
	if(derefs_requested){load_lbl_and_deref_n_times(reg->name, reg->name, derefs_requested);}
	// reg and reg->val are premade
	return stralloc(reg->name);

out_load_val_with_derefs:
	CHECK_STR(reg->name);
	CHECK_STR(val);
	derefs_requested = count_derefs(acc_ref, acc_deref, is_array, is_pointer);
	load_lbl_and_deref_n_times(reg->name, val, derefs_requested);
	// reg and reg->val are premade
	return stralloc(reg->name);


/*
	if(isnumber(val)){
		//immediate value
		reg = allocRegister();
		if(comments){printindent(); asm_println2("//load %s into %s",val, reg->name);}
		reg->val = stralloc(val);
		printindent();
		asm_println("mov %s, %s",reg->name,val);
		goto ret_reg_name;
	}else{
		//stored value
		int ref = 0;
		int deref = 0;
		if(val[0] == '*'){deref = 1; val++;}
		if(val[0] == '&'){ref = 1; val++;}
		//ptr_symbol S = find_symbol_by_ir_name(CurCS->scope,val);
		ptr_IR_symbol S = find_IR_symbol(val);
		if(S){
			int adr = S->pos;//S->store_adr+1; //because #[EBP:0] is prevEBP
			reg = allocRegister();
			int framediff = curframe->depth-S->framedepth;
			if(comments){printindent(); CHECK_asm_println4("//load %s into %s (cf:%d, fd:%d)",val,reg->name,curframe->depth,S->framedepth);}
			if (!strcmp(S->type, "MEMBER")) {
				if (ref) { printindent(); CHECK_asm_println2("mov %s, %s", reg->name, S->lbl_at); }
				else {
					error("codegen error: member var %s may only be used as '&%s'", val, val);
				}
				//else { printindent(); asm_println("mov %s, #%s", reg->name, S->lbl_at); }
				//if (deref) { printindent(); asm_println("mov %s, #%s", reg->name, reg->name); }
				//if (S->pointerlevel) { printindent(); asm_println("mov %s, #%s", reg->name, reg->name); }


				reg->val = stralloc(val);
				goto ret_reg_name;
			}
			if(!strcmp(S->type,"VAR") || !strcmp(S->type,"ARG")){
				//if(adr < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: unknown address for val [%s]",val);}
				//it is negative for func arguments
				if(S->framedepth == 0){
					//global
					if(ref || S->arraysize)	{printindent(); CHECK_asm_println2("mov %s, %s",reg->name,S->lbl_at);}
					else				{printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,S->lbl_at);}
					if(deref)			{printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,reg->name);}
					if(S->pointerlevel)	{printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,reg->name);}
				}else{
					//local
					if(framediff < 0){asm_println("*RECORD SCRATCH*");error("[CODE GEN] Error: attempt to access val [%s] in a deeper frame (%d) than current (%d)",val,S->framedepth,curframe);}
					if(framediff == 0){
						if(ref || S->arraysize)				{printindent(); CHECK_asm_println2("mov %s, EBP:%d", reg->name, adr);} //reg = ESP+adr
						else				{printindent(); CHECK_asm_println2("rstack %s, EBP:%d", reg->name, adr);}
						if(deref)			{printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,reg->name);}
						if(S->pointerlevel)	{printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,reg->name);}
					}else{
						//error("[CODE GEN] Error: frame-climbing is disabled\n");
						if(comments){printindent(); CHECK_asm_println1("//climbing %d frames",framediff);}
						printindent(); CHECK_asm_println1("mov %s, #EBP",reg->name); //frame climbing (r7 = EBP_prev)
						int i;
						for(i = 1; i < framediff; i++){
							printindent(); CHECK_asm_println2("mov %s, #%s", reg->name,reg->name);
						}
						printindent(); CHECK_asm_println3("mov %s, %s:%d", reg->name, reg->name, adr);
						if(!(ref || S->arraysize))	{printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,reg->name);} //reg = ESP+adr
						if(deref)			{printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,reg->name);}
						if(S->pointerlevel)	{printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,reg->name);}
					}
				}
				reg->val = stralloc(val);
				goto ret_reg_name;
			}
			if(!strcmp(S->type,"LABEL") || !strcmp(S->type,"FUNC")){
				if(deref){
					printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,val);
				}else{
					printindent(); CHECK_asm_println2("mov %s, %s",reg->name,val); //also label
				}
				if(S->pointerlevel)	{printindent(); CHECK_asm_println2("mov %s, #%s",reg->name,reg->name);}
				reg->val = stralloc(val);
				goto ret_reg_name;
			}
			
			error("[CODE GEN] trying to load unsupported symbol type %s ",S->type);
			
		}else{
			error("[CODE GEN] Error: undefined value [%s]\n",val);
			return 0;
		}
	}
	assert(!"unreachable");
	return 0;
*/
}



//value = register
void storeValue(const char *val, const char *reg){
	if(!val){error("[CODE GEN] trying to store null value ");}
	if(!reg){error("[CODE GEN] trying to store to null register ");}
	printf("storeValue(%s, %s)\n",val,reg);
	if(isnumber(val)){if(comments){asm_println("//discard %s from %s",val,reg);}return;}
	if(comments){asm_println("//store %s into %s",reg,val);}
	const char *reg2 = "R7";
	//stored value
	int deref = 0;
	if(val[0] == '*'){deref = 1; val++;} //*val = reg is yep,&val = reg is nope
	//ptr_symbol S = find_symbol_by_ir_name(CurCS->scope,val);
	ptr_IR_symbol S = find_IR_symbol(val);
	if(S){
		//address calculation
		int adr = S->pos;//S->store_adr+1;
		int framediff = curframe->depth - S->framedepth;
		if(comments){printindent();asm_println("//(curframe:%d, framedepth:%d)",curframe->depth,S->framedepth);}
		//ptr_reg reg2 = allocRegister(); //reg2 stays free by the time we're done
		if(!strcmp(S->type,"VAR") || !strcmp(S->type,"ARG")){
			//if(adr < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: unknown address for val [%s]",val);}
			if(S->framedepth == 0){
				if(!S->lbl_at){error("[CODE GEN] global var has no lbl_at: %s ",val);}
				//global // && !S->pointerlevel
				if(!deref){printindent();asm_println("mov #%s, %s",S->lbl_at,reg);}
				else{
					printindent();asm_println("mov %s, %s",reg2,S->lbl_at);
					if(deref)			{printindent();asm_println("mov %s, #%s",reg2,reg2);}
					//if(S->pointerlevel)	{printindent();fprintf(fasm,"mov %s, #%s\n",reg2,reg2);}
					printindent();asm_println("mov #%s, %s",reg2,reg);
				}
				return;
			}else{
				if(framediff < 0){printindent();asm_println("*RECORD SCRATCH*");error("[CODE GEN] Error: attempt to access val [%s] in a deeper frame (%d) than current (%d)",val,S->framedepth,curframe->depth);}
				if(framediff == 0){
					if(!deref && !S->pointerlevel){
						printindent();asm_println("sstack EBP:%d, %s",adr,reg);
					}else{
						printindent();asm_println("mov %s, EBP:%d", reg2, adr); //reg = ESP+adr
						if(deref)			{printindent();asm_println("mov %s, #%s",reg2,reg2);}
						//if(S->pointerlevel)	{printindent();fprintf(fasm,"mov %s, #%s\n",reg2,reg2);}
						printindent();asm_println("mov #%s, %s",reg2,reg);
					}
				}else{
					//error("[CODE GEN] Error: frame-climbing is disabled\n");
					if(comments){printindent();asm_println("//climbing %d frames",framediff);}
					printindent();asm_println("mov %s, #EBP",reg2); //frame climbing (r7 = EBP_prev)
					int i;
					for(i = 1; i < framediff; i++){
						printindent();asm_println("mov %s, #%s", reg2,reg2);
					}
					if(!deref && ! S->pointerlevel){
						//printindent();asm_println("sstack %%EBP:%d, %s",adr,reg);
						printindent(); asm_println("sstack EBP:%d, %s", adr, reg); //why the %%?
					}else{				
						printindent();asm_println("mov %s, %s:%d", reg2, reg2, adr);
						if(deref)			{printindent();asm_println("mov %s, #%s",reg2,reg2);}
						//if(S->pointerlevel)	{printindent();fprintf(fasm,"mov %s, #%s\n",reg2,reg2);}
						//store value
						printindent();asm_println("mov #%s, %s",reg2,reg);
					}
				}
				return;
			}
		}
		if(!strcmp(S->type,"LABEL")){
			error("[CODE GEN]: can't write into labels");
		}
		if(!strcmp(S->type,"FUNC")){
			error("[CODE GEN]: trying to store value into a function");
		}
		error("[CODE GEN]: unknown symbol type: [%s] (line %d)",S->type,CurCMD+1);
		/*
		switch(S->storage){
			case(STORE_DATA_STACK):
				if(framediff < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: attempt to access val [%s] in a deeper frame (%d) than current (%d)",val,S->framedepth,curframe);}
				if(framediff == 0){
					fprintf(fasm,"mov %s, EBP:-%d\n", reg2, adr); //reg = ESP+adr
				}else{
					fprintf(fasm,"//climbing %d frames\n",framediff);
					fprintf(fasm,"mov %s, #EBP\n",reg2); //frame climbing (r7 = EBP_prev)
					int i;
					for(i = 1; i < framediff; i++){
						fprintf(fasm,"mov %s, #%s\n", reg2,reg2);
					}
					fprintf(fasm,"mov %s, %s:-%d\n", reg2, reg2, adr);
				}
			break;
			case(STORE_DATA_MEMBER):
				fprintf(fasm,"mov %s, %d\n",reg2,adr);
			break;
			case(STORE_DATA_POINTER):
				fprintf(fasm,"mov %s, EBP:-%d\n",reg2,adr);
				fprintf(fasm,"mov %s, #%s\n",reg2,reg2);
			break;
			case(STORE_CODE):
				fprintf(fasm,"*RECORD SCRATCH*\n");
				error("[CODE GEN] Error: can't write into code storage\n");
			break;
			default:
				fprintf(fasm,"*RECORD SCRATCH*\n");
				error("[CODE GEN] Error: unknown storage class for [%s]: [%d]\n",val,S->storage);
			break;
		}
		*/
		
	}else{
		//fprintf(fasm,"\n");
		if(deref){
			asm_println("*RECORD SCRATCH*");
			error("[CODE GEN] Error: undefined value [%s], can't dereference\n",val);
		}
		//if(comments){fprintf(fasm,"//storing %s as new value\n",val);}
		//create new value
		error("[CODE GEN] temp vals should be found in decl pass ");
		//ptr_IR_symbol S = new_IR_symbol();
		//S->IR_name = stralloc(val);
		//S->type = stralloc("VAR");
		//S->pos = allocStackVar(1);
		// S->temp = 1;
		// S->framedepth = curframe->depth;
		// S->pointerlevel = 0;
		// S->lbl_from = curframe->lbl_from;
		// S->lbl_to = curframe->lbl_to;
		// m(curframe->symbols,push_back,S);
		// printf("stored %s as new value\n",S->IR_name);
		// if(comments){printindent();fprintf(fasm, "//temp value %s at EBP+%d\n",S->IR_name,S->pos);}
		// printindent();fprintf(fasm, "sstack EBP:%d, %s\n",S->pos,reg);
	}
}

void checkResult(const char *val){
	ptr_IR_symbol S = find_IR_symbol(val);
	if(S){return;}
	S = new_IR_symbol();
	S->IR_name = stralloc(val);
	S->type = stralloc("VAR");
	S->pos = allocStackVar(1);
	S->temp = 1;
	S->framedepth = curframe->depth;
	if(!S->framedepth){S->lbl_at = IR_inexact_name(curframe->namespace,S->IR_name);}
	S->pointerlevel = 0;
	S->lbl_from = curframe->lbl_from;
	S->lbl_to = curframe->lbl_to;
	m(curframe->symbols,push_back,S);
	printf("stored %s as new value\n",S->IR_name);
	//if(comments){printindent();fprintf(fasm, "//temp value %s at EBP+%d\n",S->IR_name,S->pos);}
	//printindent();fprintf(fasm, "sstack EBP:%d, %s\n",S->pos,reg);
}

//copies up to num bytes of string str into buff, skipping over comments.
void bufferizeWithoutComments(char *buff, const char *str, int num){
	int inComment = 0;
	char C, C2;
	C = *str++;
	if(C){C2 = *str++;}
	while(C && num){
		if(!inComment){
			if((C == '/') && (C2 == '*')){
				inComment = 1;
				C = C2;
				C2 = *str++;
			}else if((C == '/') && (C2 == '/')){
				// we're done, everything else is a comment
				break;
			}else	
			{
				//normal char, put into buffer
				*buff++ = C;
				num--;
			}
		}else{
			if((C == '*') && (C2 == '/')){
				inComment = 0;
				C = C2;
				C2 = *str++;
			}else{
				//do nothing
			}
		}
		C = C2;
		if(C){C2 = *str++;}
	}
	*buff = 0;
}

const char *makeSingleLine(const char *str){
	char buff[2000];
	int len = strlen(str);
	if(len > 2000){len = 2000;}
	memcpy(buff,str,len);
	buff[len] = 0;
	for(int I = 0; I < len; I++){
		char C = buff[I];
		if((C == '\n')||(C == '\r')){
			buff[I] = ' ';
		}
	}
	return stralloc(buff);
}

void codegen_gen_command(/*ptr_code_segment CS unused,*/ const char *str, int new_cmd_index){
		cmd_index = new_cmd_index;
		codegen_str = str;
		if(!codegen_decl){if(comments){asm_println("//[%s]",makeSingleLine(str));}}
		countindent(str);
		printf("codegen_decl [%s]\n",str);
		//int len = strlen(str);
		//if(len > 120){
			//for some reason we got a really long line...\n"
		//	fprintf(stderr, "line %d too long (%d chars). Line:\n", cmd_index+1,len); 
		//	fprintf(stderr, "-----------------------------------------\n");
		//	fprintf(stderr, "%s",str);
		//	fprintf(stderr, "-----------------------------------------\n");
		//	len = 120;
		// }
		//char buff[121];
		//char *tok;
		//strcpy(buff,str);
		char buff[120];
		bufferizeWithoutComments(buff,str,120);
		
		//fprintf(stderr,"\ncode_gen_command, input str:\n");
		//fprintf(stderr, "-----------------------------------------\n");
		//fprintf(stderr, "%s",str);
		//fprintf(stderr, "\n-----------------------------------------\n");
		//fprintf(stderr,"code_gen_command, output str:\n");
		//fprintf(stderr, "-----------------------------------------\n");
		//fprintf(stderr, "%s",buff);
		//fprintf(stderr, "\n-----------------------------------------\n\n");
		//int fd = -1;
		//if(curframe){fd = curframe->depth;}
		//fprintf(stderr, "code_gen_command [%s]\n", buff);
		//fprintf(stderr, ", FR %p, fd %d\n", curframe, fd);
		
		if(strlen(buff) == 0){
			//fprintf(stderr, "codegen_gen_command, resulting str is empty, leaving\n");
			return;
		}
		
		//memcpy(buff,str,len);
		//buff[len] = 0;
		/*tok*/
		codegen_tok = strtok(buff," ");
		if(!codegen_tok){
			//fprintf(stderr, "codegen_gen_command: no tokens\n");
			return;
		}
		//codegen_tok = tok; //illegal reference smuggling
		
		if(strcmp(codegen_tok,"/*")==0){return;}
		
		// <val> <type> [pointerlevel] [pos]
		//type:
		//	var - local stack variable (ESP-d)
		//	arg - local stack argument (ESP+1+d)
		//	label - verbatim assembler label
		
		if(strcmp(codegen_tok,"SYMBOL")==0)		{gen_command_symbol(); 		return;}
		if(strcmp(codegen_tok,"FUNCTION")==0)	{gen_command_function();	return;}
		if(strcmp(codegen_tok,"LABEL")==0)		{gen_command_label();		return;}
		if(strcmp(codegen_tok,"RET")==0)		{gen_command_ret();			return;}
		if(strcmp(codegen_tok,"JMP")==0)		{gen_command_jmp();			return;}
		if(strcmp(codegen_tok,"JNE")==0)		{gen_command_jne();			return;}
		if(strcmp(codegen_tok,"JE")==0)			{gen_command_je();			return;}
		if(strcmp(codegen_tok,"CALL")==0)		{gen_command_call();		return;}
		if(strcmp(codegen_tok,"MOV")==0)		{gen_command_mov();			return;}
		if(strcmp(codegen_tok,"ADD")==0)		{gen_command_add();			return;}
		if(strcmp(codegen_tok,"SUB")==0)		{gen_command_sub();			return;}
		if(strcmp(codegen_tok,"DIV")==0)		{gen_command_div();			return;}
		if(strcmp(codegen_tok,"MUL")==0)		{gen_command_mul();			return;}
		if(strcmp(codegen_tok,"MOD")==0)		{gen_command_mod();			return;}
		if(strcmp(codegen_tok,"AND")==0)		{gen_command_and();			return;}
		if(strcmp(codegen_tok,"OR")==0)			{gen_command_or();			return;}
		if(strcmp(codegen_tok,"NOT")==0)		{gen_command_not();			return;}
		if(strcmp(codegen_tok,"NEG")==0)		{gen_command_neg();			return;}
		if(strcmp(codegen_tok,"DEREF")==0)		{gen_command_deref();		return;}
		if(strcmp(codegen_tok,"EQUAL")==0)		{gen_command_equal();		return;}
		if(strcmp(codegen_tok,"NOTEQUAL")==0)	{gen_command_notequal();	return;}
		if(strcmp(codegen_tok,"GREATER")==0)	{gen_command_greater();		return;}
		if(strcmp(codegen_tok,"LESS")==0)		{gen_command_less();		return;}
		if(strcmp(codegen_tok,"ALLOC")==0)		{gen_command_alloc();		return;}
		if(strcmp(codegen_tok,"STRUCT")==0)		{gen_command_struct();		return;}
		if(strcmp(codegen_tok,"USING")==0)		{gen_command_using();		return;}
		if(strcmp(codegen_tok,"INSERT")==0)		{gen_command_insert();		return;}
		if(strcmp(codegen_tok,"FRAME")==0)		{gen_command_frame();		return;}
		if(strcmp(codegen_tok,"DEBUG")==0)		{gen_command_debug();		return;}
		if(strcmp(codegen_tok,"COMMENT")==0)	{gen_command_comment();		return;}
		if(strcmp(codegen_tok,"FLOOR")==0)		{gen_command_floor();		return;}
		asm_println("*RECORD SCRATCH*\n");
		error("[CODE GEN] Error: unsupported command [%s] (line %d)",codegen_tok,CurCMD+1);
		/*
		while(tok){
			printf("<%s>",tok);
			tok = strtok(0," ");
		}*/
		//printf("\n");
}

void codegen_gen_code_segment(ptr_code_segment CS){
	printf("CS %p:\n",CS);
	CurCS = CS;
	make_first_frame();
	print_skeleton_start();
	//fprintf(fasm,"enter 0\n");
	printf("\nCS DECLARATIVE PASS ===============================\n\n");
	int i;
	codegen_decl = 1;
	for(i = 0; i < CS->commands.size; i++){
		CurCMD = i;
		const char *str = m(CS->commands,get,i);
		codegen_gen_command(/*CS,*/str,i);
	}
	printf("\nCS IMPERATIVE PASS ===============================\n\n");
	codegen_decl = 0;
	for(i = 0; i < CS->commands.size; i++){
		CurCMD = i;
		const char *str = m(CS->commands,get,i);
		codegen_gen_command(/*CS,*/str,i);
	}
	print_skeleton_end();
}

void codegen_generate(){
	fasm = fopen("data_out/aout_assembly.txt","w");
	IR_init();
	initializeRegTable();
	debug_funcs = vector2_ptr_debugInfoFunc_here();
	debug_vars = vector2_ptr_debugInfoVar_here();
	varstacks = vector3_int_here();//vector2_int_here();
	argstacks = vector3_int_here();//vector2_int_here();
	//IR_symbol_table = vector2_ptr_IR_symbol_here();
	stringStore_labels = vector2_ptr_char_here();
	stringStore_strings = vector2_ptr_char_here();
	frameStarts = vector2_ptr_char_here();
	frameIndices = vector3_int_here();//vector2_int_here();
	frames = vector2_ptr_frame_here();
	framestack = vector2_ptr_frame_here();
	int i;
	for(i = 0; i < CS_list.size; i++){
		codegen_gen_code_segment(m(CS_list,get,i));
	}
	asm_println("// CODEGEN: ASSEMBLY GENERATION COMPLETE //");
}


