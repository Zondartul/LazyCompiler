#include "codegen.h"
#include "semantic.h"
#include "string.h"
#include "ctype.h"
#include "stdio.h"

int debugInvade = 1;
int debugInvadeHalt = 1;
int comments = 0;

FILE *fasm;
int CurCMD = 0;
int CurEBP = 0;
vector2_ptr_char stringStore_labels;
vector2_ptr_char stringStore_strings;
ptr_code_segment CurCS;
/*
asm file structure:
section_code:
	section_code_preamble:
	<preamble>
	section_code_body:
	<body>
	section_code_epilog:
	<epilog>
section_static:
	section_static_strings:
	<lbl><string><lbl><string>
	debug_info:
	<debug_info>
*/
void printTraceCode();
void printDebugInfo();
void printStringStore();

const char *stringStore_add(const char *str){
	const char *lbl = IR_next_name("str");
	m(stringStore_labels,push_back,lbl);
	m(stringStore_strings,push_back,stralloc(str));
	return lbl;
}

void printStringStore(){
	fprintf(fasm,"string_store:\n");
	int i;
	for(i = 0; i < stringStore_labels.size; i++){
		fprintf(fasm,"%s: db \"%s\",0\n",m(stringStore_labels,get,i),m(stringStore_strings,get,i));
	}
}

void printTraceHelper(const char *str){
	if(debugInvade){
		if(str){
			const char *lbl = stringStore_add(str);
			fprintf(fasm,"push %s\n",lbl);
		}else{
			fprintf(fasm,"push 0\n");
		}
		fprintf(fasm,"call trace\n");
		fprintf(fasm,"inc esp\n");//deallocate the argument
	}
}
void printTrace(){
	const char *str = m(CurCS->commands,get,CurCMD);
	printTraceHelper(str);
}

void print_skeleton_start(){
	fprintf(fasm,
	"section_code:\n"
	"section_code_preamble:\n"
	"mov ebp, 20000\n"		//so we can tell stack pointers from code pointers
	"mov esp, 19999\n"
	);
	CurEBP = 20000;
	printTraceHelper("program start");
	fprintf(fasm,
	"section_code_body:\n"
	"call main_1\n"//haaaaaaacks
	);
}

void print_skeleton_end(){
	fprintf(fasm,
	"section_code_epilog:\n"
	);
	printTraceCode();
	fprintf(fasm,
	"section_code_end:\n"
	"section_static:\n"
	);
	printStringStore();
	printDebugInfo();
	fprintf(fasm,
	"section_static_end:\n"
	);
}

void printTraceCode(){
	fprintf(fasm,
	"trace:\n"
	"cmp #65884,0\n" //debugMem + 18
	"je trace_exit\n"
	"pusha\n"
	"mov eax, 65866\n" //debugMem
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
	//"mov eax:#18, ESP:#18\n"
	"mov eax:#19, ESP\n"
	"mov eax:#20, debug_info\n"
	"popa\n"
	);
	if(debugInvadeHalt){fprintf(fasm,"int 1\n");}
	fprintf(fasm,
	"ret\n"
	"trace_exit:\n"
	"ret\n"
	);
}

/*
struct record{
	int size;
	const char *data;
};

struct record str_to_record(int size, const char *data){
	struct record R;
	if(data){
		R.size = strlen(data)+2; //[size][str][0]
		R.data = stralloc(data);
	}else{
		R.size = 0;
		R.data = 0;
	}
	return R;
}
void print_record(struct record R){
	fprintf(fasm,"db %d, \"%s\",0\n",R.size,R.data);
}
*/

struct IR_symbol{
	const char *IR_name;
	const char *username;
	const char *type;
	const char *lbl_from;
	const char *lbl_to;
	int pointerlevel;
	int pos;
	int framedepth;
};
typedef struct IR_symbol *ptr_IR_symbol;

ptr_IR_symbol new_IR_symbol(){
	ptr_IR_symbol S = malloc(sizeof(struct IR_symbol));
	S->IR_name = "";
	S->username = "";
	S->type = 0;
	S->lbl_from = "section_code";
	S->lbl_to = "section_code_end";
	S->pointerlevel = 0;
	S->pos = 0;
	S->framedepth = 0;
	return S;
}

definition_vector_of(ptr_IR_symbol);
implementation_vector_of(ptr_IR_symbol);
vector2_ptr_IR_symbol IR_symbol_table;

struct frame{
	int cmd_index;
	const char *lbl_from;
	const char *lbl_to;
	vector2_ptr_IR_symbol symbols;
};

typedef struct frame *ptr_frame;

definition_vector_of(ptr_frame);
implementation_vector_of(ptr_frame);
vector2_ptr_frame frames;

ptr_IR_symbol find_IR_symbol(const char *IR_name){
	int i;
	for(i = 0; i < IR_symbol_table.size; i++){
		ptr_IR_symbol S = m(IR_symbol_table,get,i);
		if(!strcmp(S->IR_name,IR_name)){
			return S;
		}
	}
	return 0;
}


struct reg{
	const char *name;
	const char *val;
	int age;
};
typedef struct reg* ptr_reg;

ptr_reg new_reg(const char *name, const char *val, int age){
	ptr_reg R = malloc(sizeof(struct reg));
	*R = (struct reg){name,val,age};
	return R;
}

definition_vector_of(ptr_reg);
implementation_vector_of(ptr_reg);
vector2_ptr_reg registers;

struct debugInfoFunc{
	const char *IR_name;
	const char *username;
	const char *entrance;
	const char *exit;
};
typedef struct debugInfoFunc *ptr_debugInfoFunc;
struct debugInfoVar{
	const char *IR_name;
	const char *username;
	const char *visible_from;
	const char *visible_to;
	int pointerlevel;
	int pos;
	int framedepth;
};
typedef struct debugInfoVar *ptr_debugInfoVar;
definition_vector_of(ptr_debugInfoFunc);
definition_vector_of(ptr_debugInfoVar);
implementation_vector_of(ptr_debugInfoFunc);
implementation_vector_of(ptr_debugInfoVar);
vector2_ptr_debugInfoFunc debug_funcs;
vector2_ptr_debugInfoVar debug_vars;

#define if_first(x) {static int count = 0; if(!first++){x;}}
#define if_not_first(x) {static int first = 0; if(first++){x;}}
void storeValue(const char *val, const char *reg);

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
	//m(registers,push_back,new_reg("R7",0,0));
	//m(registers,push_back,new_reg("R8",0,0));
	printf("reg table initialize\n");
}

int curframe = 0;

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
	/*
	if(R->val){
		printf("spilling register %d (%s)\n",minageI,R->name);
		storeValue(R->val,R->name);
	}else{
		printf("assuming register %s is free (no assoc. value)\n",R->name);
		error("unreachable code?...");
	}
	*/
	if(R->val){if(comments){fprintf(fasm,"//discarding %s from %s\n",R->val,R->name);}}
	R->val = 0;
	R->age = age++;
	printf("allocated register %d (%s)\n",minageI,R->name);
	return R;
}

int isnumber(const char *str){
	char C = *str++;
	while(C){
		if(!isdigit(C)){return 0;}
		C = *str++;
	}
	return 1;
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

void printDebugInfo(){
	printf("emitting debug info\n");
	fprintf(fasm,"debug_info:\n");
	fprintf(fasm,"debug_info_funcs:\n");
	int numFuncs = 0;
	int numVars = 0;
	int i;
	for(i = 0; i < IR_symbol_table.size; i++){
		ptr_IR_symbol S = m(IR_symbol_table,get,i);
		if(!strcmp(S->type,"FUNC")){
			numFuncs++;
		}
		if(!strcmp(S->type,"VAR")||!strcmp(S->type,"ARG")){
			numVars++;
		}
	}
	fprintf(fasm,"DB %d\n //number of entries\n",numFuncs);
	if(comments){fprintf(fasm,"//[len][entrance,exit,[len][IR_name],[len][username]]\n");}
	
	for(i = 0; i< IR_symbol_table.size; i++){
		ptr_IR_symbol S = m(IR_symbol_table,get,i);
		//record: db recordlen, data1, data2... datan
		//record2[1] = (record1+recordlen)[1]
		if(!strcmp(S->type,"FUNC")){
			int str1len = 0;
			if(S->IR_name){str1len = strlen(S->IR_name);}
			int record1len = str1len+1;
			int str2len = 0;
			if(S->username){str2len = strlen(S->username);}
			int record2len = str2len+1;
			int recordlen = 3+record1len+record2len;
			//fprintf(fasm,"DB %d, %s, %s,\"%s\",0\n",recordlen,F->entrance,F->exit,F->name);
			fprintf(fasm,"DB %d, ",recordlen);
			fprintf(fasm,"%s, ",S->lbl_from);
			fprintf(fasm,"%s, ",S->lbl_to);
			fprintf(fasm,"%d, \"%s\",0, ",str1len,S->IR_name);
			fprintf(fasm,"%d, \"%s\",0;\n",str2len,S->username);
		}
		
	}
	fprintf(fasm,"debug_info_vars:\n");
	fprintf(fasm,"DB %d\n //number of entries\n",numVars);
	if(comments){fprintf(fasm,"//[len][visible_from,visible_to,pos,framedepth,[len][IR_name],[len][username]]\n");}
	for(i = 0; i< IR_symbol_table.size; i++){
		ptr_IR_symbol S = m(IR_symbol_table,get,i);
		//record: db recordlen, data1, data2... datan
		//record2[1] = (record1+recordlen)[1]
		if(!strcmp(S->type,"VAR")||!strcmp(S->type,"ARG")){
			int str1len = 0;
			if(S->IR_name){str1len = strlen(S->IR_name);}
			int record1len = str1len+1;
			int str2len = 0;
			if(S->username){str2len = strlen(S->username);}
			int record2len = str2len+1;
			int recordlen = 4+record1len+record2len;
			
			fprintf(fasm,"DB %d, ",recordlen);
			fprintf(fasm,"%s, ",S->lbl_from);
			fprintf(fasm,"%s, ",S->lbl_to);
			fprintf(fasm,"%d, ",S->pos);
			fprintf(fasm,"%d, ",S->framedepth);
			fprintf(fasm,"%d, \"%s\",0, ",str1len,S->IR_name);
			fprintf(fasm,"%d, \"%s\",0;\n",str2len,S->username);
			//fprintf(fasm,"DB %d, %s, %s, %d, \"%s\",0\n",recordlen,V->visible_from,V->visible_to,V->framedepth,V->name);
		}
	}
}


vector2_int varstacks;
vector2_int argstacks;
void resizeStacks(){
	while(varstacks.size < curframe+1){
		m(varstacks,push_back,0);
		m(argstacks,push_back,-2);
	}
}
int allocStackVar(){
	fprintf(fasm,"dec ESP\n");
	resizeStacks();
	int V = m(varstacks,get,curframe);
	V++;
	m(varstacks,set,V,curframe);
	return V;
}
int allocStackArg(){
	resizeStacks();
	int A = m(argstacks,get,curframe);
	A--;
	m(argstacks,set,A,curframe);
	return A;	
}
void freeStack(){
	resizeStacks();
	m(varstacks,set,0,curframe);
	m(argstacks,set,-2,curframe);
	const char *label = IR_next_name("lbl");
	fprintf(fasm,"%s:\n",label);
	int i;
	for(i = 0; i < IR_symbol_table.size; i++){
		ptr_IR_symbol S = m(IR_symbol_table,get,i);
		if(S->framedepth == curframe){
			if(!strcmp(S->lbl_to,"section_code_end")){
				S->lbl_to = label;
			}			
		}
	}
}

//some register = value
const char* loadValue(const char* val){
	const char *reg2 = "R7";
	if(isnumber(val)){
		//immediate value
		ptr_reg reg = allocRegister();
		if(comments){fprintf(fasm,"//load %s into %s\n",val, reg->name);}
		reg->val = stralloc(val);
		fprintf(fasm,"mov %s, %s\n",reg->name,val);
		return reg->name;
	}else{
		//stored value
		int ref = 0;
		int deref = 0;
		if(val[0] == '*'){deref = 1; val++;}
		if(val[0] == '&'){ref = 1; val++;}
		//ptr_symbol S = find_symbol_by_ir_name(CurCS->scope,val);
		ptr_IR_symbol S = find_IR_symbol(val);
		if(S){
			int adr = -(S->pos);//S->store_adr+1; //because #[EBP:0] is prevEBP
			ptr_reg reg = allocRegister();
			int framediff = curframe-S->framedepth;
			if(comments){fprintf(fasm,"//load %s into %s (cf:%d, fd:%d)\n",val,reg->name,curframe,S->framedepth);}
			if(!strcmp(S->type,"VAR") || !strcmp(S->type,"ARG")){
				//if(adr < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: unknown address for val [%s]",val);}
				//it is negative for func arguments
				if(framediff < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: attempt to access val [%s] in a deeper frame (%d) than current (%d)",val,S->framedepth,curframe);}
				if(framediff == 0){
					fprintf(fasm,"mov %s, EBP:%d\n", reg->name, adr); //reg = ESP+adr
				}else{
					if(comments){fprintf(fasm,"//climbing %d frames\n",framediff);}
					fprintf(fasm,"mov %s, #EBP\n",reg->name); //frame climbing (r7 = EBP_prev)
					int i;
					for(i = 1; i < framediff; i++){
						fprintf(fasm,"mov %s, #%s\n", reg->name,reg->name);
					}
					fprintf(fasm,"mov %s, %s:%d\n", reg->name, reg->name, adr);
				}
			}
			if(!strcmp(S->type,"LABEL")){
				fprintf(fasm,"mov %s, %s\n",reg->name,val); //also label
			}
			/*
			switch(S->storage){
				case(STORE_DATA_STACK):
					if(adr < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: unknown address for val [%s]",val);}
					if(framediff < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: attempt to access val [%s] in a deeper frame (%d) than current (%d)",val,S->framedepth,curframe);}
					if(framediff == 0){
						fprintf(fasm,"mov %s, EBP:-%d\n", reg->name, adr); //reg = ESP+adr
					}else{
						fprintf(fasm,"//climbing %d frames\n",framediff);
						fprintf(fasm,"mov %s, #EBP\n",reg->name); //frame climbing (r7 = EBP_prev)
						int i;
						for(i = 1; i < framediff; i++){
							fprintf(fasm,"mov %s, #%s\n", reg->name,reg->name);
						}
						fprintf(fasm,"mov %s, %s:-%d\n", reg->name, reg->name, adr);
					}
				break;
				case(STORE_DATA_MEMBER):
					if(adr < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: unknown address for val [%s]",val);}
					fprintf(fasm,"mov %s, %d\n",reg->name,adr);
				break;
				case(STORE_CODE):
					fprintf(fasm,"mov %s, %s\n",reg->name,val); //address is label, which in turn is a fully-qualified (unique) name of func. 
				break;
				case(STORE_DATA_STATIC):
					fprintf(fasm,"mov %s, %s\n",reg->name,val); //also label
				break;
				default:
					error("[CODE GEN] Error: unknown storage class for [%s]: [%d]\n",val,S->storage);
					return 0;
				break;
			}
			*/
			if(!ref){
				fprintf(fasm,"mov %s, #%s\n",reg->name,reg->name);
			}
			if(deref){
				fprintf(fasm,"mov %s, #%s\n",reg->name,reg->name);
			}
			if(S->pointerlevel){
				fprintf(fasm,"mov %s, #%s\n",reg->name,reg->name);
			}
			reg->val = stralloc(val);
			return reg->name;
		}else{
			error("[CODE GEN] Error: undefined value [%s]\n",val);
			return 0;
		}
	}
}

//value = register
void storeValue(const char *val, const char *reg){
	if(isnumber(val)){if(comments){fprintf(fasm,"//discard %s from %s\n",val,reg);}return;}
	if(comments){fprintf(fasm,"//store %s into %s",reg,val);}
	const char *reg2 = "R7";
	//stored value
	int deref = 0;
	if(val[0] == '*'){deref = 1; val++;} //*val = reg is yep,&val = reg is nope
	//ptr_symbol S = find_symbol_by_ir_name(CurCS->scope,val);
	ptr_IR_symbol S = find_IR_symbol(val);
	if(S){
		//address calculation
		int adr = -(S->pos);//S->store_adr+1;
		int framediff = curframe - S->framedepth;
		if(comments){fprintf(fasm," (curframe:%d, framedepth:%d)\n",curframe,S->framedepth);}
		//ptr_reg reg2 = allocRegister(); //reg2 stays free by the time we're done
		if(!strcmp(S->type,"VAR") || !strcmp(S->type,"ARG")){
			//if(adr < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: unknown address for val [%s]",val);}
			if(framediff < 0){fprintf(fasm,"*RECORD SCRATCH*\n");error("[CODE GEN] Error: attempt to access val [%s] in a deeper frame (%d) than current (%d)",val,S->framedepth,curframe);}
			if(framediff == 0){
				fprintf(fasm,"mov %s, EBP:%d\n", reg2, adr); //reg = ESP+adr
			}else{
				if(comments){fprintf(fasm,"//climbing %d frames\n",framediff);}
				fprintf(fasm,"mov %s, #EBP\n",reg2); //frame climbing (r7 = EBP_prev)
				int i;
				for(i = 1; i < framediff; i++){
					fprintf(fasm,"mov %s, #%s\n", reg2,reg2);
				}
				fprintf(fasm,"mov %s, %s:%d\n", reg2, reg2, adr);
			}
		}
		if(!strcmp(S->type,"LABEL")){
			error("[CODE GEN]: can't write into labels");
		}
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
		if(deref){
			fprintf(fasm,"mov %s, #%s\n",reg2,reg2);
		}
		if(S->pointerlevel){
			fprintf(fasm,"mov %s, #%s\n",reg2,reg2);
		}
		//store value
		fprintf(fasm,"mov #%s, %s\n",reg2,reg);
	}else{
		fprintf(fasm,"\n");
		if(deref){
			fprintf(fasm,"*RECORD SCRATCH*\n");
			error("[CODE GEN] Error: undefined value [%s], can't dereference\n",val);
		}
		if(comments){fprintf(fasm,"//storing %s as new value\n",val);}
		//create new value
		/*
		struct symbol *S = new_symbol();
		S->IR_name = stralloc(val);
		S->type = SYMBOL_VARIABLE;
		//S->symvariable.pos = allocStack();
		S->store_adr = allocStackVar(); //ha, I do now.//allocStack(); //gotta know current stack pos tho...
		S->framedepth = curframe;
		S->symvariable.size = 1;
		S->storage = STORE_DATA_STACK;
		push_symbol(S);
		
		int adr = S->store_adr;
		//ptr_reg reg2 = allocRegister();
		fprintf(fasm,"mov %s, EBP:-%d\n", reg2, adr);
		fprintf(fasm,"mov #%s, %s\n", reg2, reg);
		*/
		ptr_IR_symbol S = new_IR_symbol();
		S->IR_name = stralloc(val);
		S->type = "VAR";
		S->pos = allocStackVar();
		S->framedepth = curframe;
		S->pointerlevel = 0;
		m(IR_symbol_table,push_back,S);
		printf("stored %s as new value\n",S->IR_name);
		fprintf(fasm,"mov %s, EBP:-%d\n", reg2, S->pos);
		fprintf(fasm,"mov #%s, %s\n", reg2, reg);
	}
}

int codegen_decl = 0;
const char *lbl_frameStart = 0;
const char *lbl_frameEnd = 0;
vector2_ptr_char frameStarts;
vector2_int frameIndices;

void codegen_gen_command(ptr_code_segment CS, const char *str, int i){
		if(!semantic_decl){if(comments){fprintf(fasm,"//[%s]\n",str);}}
		printf("codegen_decl [%s]\n",str);
		char buff[80];
		char *tok;
		strcpy(buff,str);
		tok = strtok(buff," ");
		
		if(strcmp(tok,"/*")==0){
			return;
		}
		
		// <val> <type> [pointerlevel] [pos]
		//type:
		//	var - local stack variable (ESP-d)
		//	arg - local stack argument (ESP+1+d)
		//	label - verbatim assembler label
		
		if(strcmp(tok,"SYMBOL")==0){
			const char *val = strtok(0," ");
			const char *type = strtok(0," ");
			const char *ptrlvl_str = strtok(0," ");
			const char *pos_str = strtok(0," ");
			int ptrlvl = 0;
			if(ptrlvl_str){sscanf(ptrlvl_str,"%d",&ptrlvl);}
			int pos = 0;
			ptr_IR_symbol S = find_IR_symbol(val);
			if(codegen_decl){				
				if(S){error("[CODE GEN] Symbol already defined: %s ",S->IR_name);}
				if(!S){
					S = new_IR_symbol();
					m(IR_symbol_table,push_back,S);
					S->IR_name = stralloc(val);
				}
				if(pos_str){
					sscanf(pos_str,"%d",&pos_str);
				}
				if(!(strcmp(type,"VAR"))){
					if(!pos_str){pos = allocStackVar();}
				}
				if(!(strcmp(type,"ARG"))){
					if(!pos_str){pos = allocStackArg();}
				}
				if(!(strcmp(type,"LABEL"))){
					//do nothing
				}
				S->type = stralloc(type);
				S->pointerlevel = ptrlvl;
				S->pos = pos;
				S->framedepth = curframe;
				//const char *lbl_from = IR_next_name("lbl");
				//fprintf(fasm,"%s:\n",lbl_from);
				//S->lbl_from = lbl_from;
				S->lbl_from = lbl_frameStart;
				printf("got symbol: %s / %s / ptr=%d / pos=%d / frame=%d / from=%s\n",S->IR_name,S->type,S->pointerlevel,S->pos,S->framedepth,S->lbl_from);
			}else{
				// do nothing
			}
			return;
		}
		if(strcmp(tok,"LABEL")==0){
			if(codegen_decl){
				//do nothing
			}else{
				const char *label = strtok(0," ");
				fprintf(fasm,"%s:\n",label);
				printTrace();
			}
			return;
		}
		if(strcmp(tok,"RET")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *val = strtok(0," ");
				if(val){
					fprintf(fasm, "mov eax, %s\n",loadValue(val));
				}
				CurEBP = CurEBP - 1000;
				fprintf(fasm,"leave\n");
				//freeStack();
				//curframe--;	//RET no longer recognized as end of frame (even though it is)
								//beacause without matching FRAME ENTER/LEAVE we get confused with counting frames.
				fprintf(fasm,"ret\n");
			}
			return;
		}
		if(strcmp(tok,"JMP")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *label = strtok(0," ");
				fprintf(fasm,"jmp %s\n",label);
			}
			return;
		}
		if(strcmp(tok,"JNE")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *val1 = loadValue(strtok(0," "));
				const char *val2 = loadValue(strtok(0," "));
				const char *label = strtok(0," ");
				fprintf(fasm,"cmp %s, %s\n",val1,val2);
				fprintf(fasm,"jne %s\n",label);
			}
			return;
		}
		if(strcmp(tok,"JE")==0){
			if(codegen_decl){
				//do nothing
			}else{
				const char *val1 = loadValue(strtok(0," "));
				const char *val2 = loadValue(strtok(0," "));
				const char *label = strtok(0," ");
				printTrace();
				fprintf(fasm,"cmp %s, %s\n",val1,val2);
				fprintf(fasm,"je %s\n",label);
			}
			return;
		}
		if(strcmp(tok,"CALL")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *result = strtok(0," ");
				const char *func = loadValue(strtok(0," "));
				tok = strtok(0," ");
				while(tok){
					const char *arg = loadValue(tok);
					fprintf(fasm,"push %s\n",arg);
					tok = strtok(0," ");
				}
				fprintf(fasm,"call %s\n",func);
				storeValue(result, "eax");
				//printf("mov %s eax\n",result);
			}
			return;
		}
		if(strcmp(tok,"MOV")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *dest = strtok(0," ");
				const char *src = loadValue(strtok(0," "));
				storeValue(dest, src);//store into value, a register
				//printf("mov %s %s\n",arg1,loadValue(arg2));
			}
			return;
		}
		if(strcmp(tok,"ADD")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *result = strtok(0," ");
				const char *arg1 = loadValue(strtok(0," "));
				const char *arg2 = loadValue(strtok(0," "));
				//storeValue etc etc
				//printf("mov %s %s\n",result,loadValue(arg1));
				//printf("add %s %s\n",result,loadValue(arg2));
				//storeValue(result, loadValue(arg1));
				fprintf(fasm,"add %s, %s\n",arg1,arg2);
				storeValue(result, arg1);
			}
			return;
		}
		if(strcmp(tok,"SUB")==0){			
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *result = strtok(0," ");
				const char *arg1 = loadValue(strtok(0," "));
				const char *arg2 = loadValue(strtok(0," "));
				//storeValue(result, loadValue(arg1));
				fprintf(fasm,"sub %s, %s\n",arg1,arg2);
				storeValue(result,arg1);
			}
			return;
		}
		if(strcmp(tok,"DIV")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *result = strtok(0," ");
				const char *arg1 = loadValue(strtok(0," "));
				const char *arg2 = loadValue(strtok(0," "));
				//storeValue(result, loadValue(arg1));
				fprintf(fasm,"div %s, %s\n",arg1,arg2);
				storeValue(result,arg1);
			}
			return;
		}
		if(strcmp(tok,"MUL")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *result = strtok(0," ");
				const char *arg1 = loadValue(strtok(0," "));
				const char *arg2 = loadValue(strtok(0," "));
				//storeValue(result, loadValue(arg1));
				fprintf(fasm,"mul %s, %s\n",arg1,arg2);
				storeValue(result,arg1);
			}
			return;
		}
		if(strcmp(tok,"EQUAL")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *result = strtok(0," ");
				const char *arg1 = loadValue(strtok(0," "));
				const char *arg2 = loadValue(strtok(0," "));
				//storeValue(result, loadValue(arg2));
				//const char *rA = loadValue(result);
				//const char *rB = loadValue(arg1);
				fprintf(fasm,"sub %s, %s\n",arg1,arg2);
				fprintf(fasm,"fabs %s, %s\n",arg1,arg1);
				fprintf(fasm,"fsgn %s, %s\n",arg1,arg1);
				fprintf(fasm,"lneg %s\n",arg1);
				storeValue(result,arg1);
			}
			return;
		}
		if(strcmp(tok,"NOTEQUAL")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *result = strtok(0," ");
				const char *arg1 = loadValue(strtok(0," "));
				const char *arg2 = loadValue(strtok(0," "));
				//storeValue(result, loadValue(arg2));
				//const char *rA = loadValue(result);
				//const char *rB = loadValue(arg1);
				fprintf(fasm,"sub %s, %s\n",arg1,arg2);
				fprintf(fasm,"fabs %s, %s\n",arg1,arg1);
				fprintf(fasm,"fsgn %s, %s\n",arg1,arg1);
				storeValue(result,arg1);
			}
			return;
		}
		if(strcmp(tok,"GREATER")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *result = strtok(0," ");
				const char *arg1 = loadValue(strtok(0," "));
				const char *arg2 = loadValue(strtok(0," "));
				//storeValue(result, loadValue(arg1));
				//const char *rA = loadValue(result);
				//const char *rB = loadValue(arg2);
				fprintf(fasm,"sub %s, %s\n",arg1,arg2);
				fprintf(fasm,"fsgn %s, %s\n",arg1,arg1);
				fprintf(fasm,"max %s, 0\n",arg1);
				storeValue(result,arg1);
			}
			return;
		}
		if(strcmp(tok,"LESS")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *result = strtok(0," ");
				const char *arg1 = loadValue(strtok(0," "));
				const char *arg2 = loadValue(strtok(0," "));
				//storeValue(result, loadValue(arg2));
				//const char *rA = loadValue(result);
				//const char *rB = loadValue(arg1);
				fprintf(fasm,"sub %s, %s\n",arg1,arg2);
				fprintf(fasm,"fsgn %s, %s\n",arg1,arg1);
				fprintf(fasm,"min %s, 0\n",arg1);
				storeValue(result,arg1);
			}
			return;
		}
		if(strcmp(tok,"ALLOC")==0){
			error("[CODE GEN] deprecated ");
			/*
			tok = strtok(0," ");
			if(strcmp(tok,"STACK")==0){
				stack++;
				const char *val = strtok(0," "); //ignored?
				const char *size = strtok(0," ");
				fprintf(fasm,"sub ESP, %s\n",size);
				if(debugInvade){fprintf(fasm,"call trace\n");}
				continue;
			}
			*/
			//continue;
		}
		if(strcmp(tok,"INSERT")==0){
			if(codegen_decl){
				//do nothing
			}else{
				printTrace();
				const char *cs = strtok(0," ");
				fprintf(fasm,"#include \"%s\"\n",cs);
			}
			return;
		}
		if(strcmp(tok,"FRAME")==0){
			if(codegen_decl){
				tok = strtok(0," ");
				if(!tok){error("[CODE GEN] bad command");}
				if(strcmp(tok,"ENTER")==0){
					curframe++;
					const char *lbl = IR_next_name("lbl");
					ptr_frame F = malloc(sizeof(struct frame));
					F->cmd_index = i;
					F->lbl_from = lbl;
					return;
				}
				if(strcmp(tok,"LEAVE")==0){
					curframe--;
					printf("current frame: %d\n",curframe);
					return;
				}
			}else{
				printTrace();
				tok = strtok(0," ");
				if(!tok){error("[CODE GEN] bad command");}
				if(strcmp(tok,"ENTER")==0){
					CurEBP = CurEBP + 1000;
					fprintf(fasm,"mov esp, %d\n",CurEBP);
					
					fprintf(fasm,"enter %d\n",numVars);
					printTrace();
					curframe++;
					printf("current frame: %d\n",curframe);
					return;
				}
				if(strcmp(tok,"LEAVE")==0){
					CurEBP = CurEBP - 1000;
					fprintf(fasm,"leave\n");
					freeStack();
					printTrace();
					curframe--;
					printf("current frame: %d\n",curframe);
					return;
				}
			}
		}
		//debug info
		//DEBUG BEGIN FUNC foo
		//DEBUG BEGIN VAR a
		//DEBUG END VAR a
		//DEBUG END FUNC foo
		if(strcmp(tok,"DEBUG")==0){	
			const char *tok2 = strtok(0," ");			
			const char *tok3 = strtok(0," ");
			const char *tok4 = strtok(0," ");
			const char *tok5 = strtok(0," ");
			const char *tok6 = strtok(0," ");
			const char *label = IR_next_name("lbl");
			
			if(codegen_decl){
				//do nothing
			}else{
				if(strcmp(tok2,"BEGIN")==0){	
					if(strcmp(tok3,"FUNC")==0){	//DEBUG BEGIN FUNC IR_name name
						ptr_debugInfoFunc F = malloc(sizeof(struct debugInfoFunc));
						F->IR_name = stralloc(tok4);
						F->username = stralloc(tok5);
						F->entrance = label;
						F->exit = 0;
						m(debug_funcs,push_back,F);
						printf("recorded symbol(func) [%s]\n",F->IR_name);
						fprintf(fasm,"%s:\n",label);
						return;
					}
					if(strcmp(tok3,"VAR")==0){	//DEBUG BEGIN VAR IR_name name pos
						ptr_debugInfoVar V = malloc(sizeof(struct debugInfoVar));
						V->IR_name = stralloc(tok4);
						V->username = stralloc(tok5);
						ptr_symbol S = find_symbol_by_ir_name(CurCS->scope,tok4); //meh, we need a unified symbol system for codegen
						if(!S){error("[CODE GEN] unknown pos for debug var [%s]",tok4);}
						V->pos = S->store_adr+1; 
						V->visible_from = label;
						V->visible_to = 0;
						V->framedepth = curframe;
						m(debug_vars,push_back,V);
						printf("recorded symbol(var) [%s]\n",V->IR_name);
						fprintf(fasm,"%s:\n",label);
						return;
					}
				}
				if(strcmp(tok2,"END")==0){
					if(strcmp(tok3,"FUNC")==0){
						ptr_debugInfoFunc F = find_debugInfoFunc(tok4);
						if(!F){error("[CODE GEN] debug symbol(func) not found: [%s]",tok4);}
						F->exit = label;
						fprintf(fasm,"%s:\n",label);
						return;
					}
					if(strcmp(tok3,"VAR")==0){
						ptr_debugInfoVar V = find_debugInfoVar(tok4);
						if(!V){error("[CODE GEN] debug symbol(var) not found: [%s]",tok4);}
						V->visible_to = label;
						fprintf(fasm,"%s:\n",label);
						return;
					}
				}
				if(strcmp(tok2,"INFO")==0){
					//const char *label = IR_next_name("debug_info");
					//make sure it's a reserved name at some point
					//printTraceCode();
					//printDebugInfo();
					return;
				}
			}
		}
		fprintf(fasm,"*RECORD SCRATCH*\n");
		error("[CODE GEN] Error: unsupported command [%s]\n",tok);
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
	print_skeleton_start();
	//fprintf(fasm,"enter 0\n");
	int i;
	codegen_decl = 1;
	for(i = 0; i < CS->commands.size; i++){
		CurCMD = i;
		const char *str = m(CS->commands,get,i);
		codegen_gen_command(CS,str,i);
	}
	codegen_decl = 0;
	for(i = 0; i < CS->commands.size; i++){
		CurCMD = i;
		const char *str = m(CS->commands,get,i);
		codegen_gen_command(CS,str,i);
	}
	print_skeleton_end();
}

void codegen_generate(){
	fasm = fopen("assembly.txt","w");
	initializeRegTable();
	debug_funcs = vector2_ptr_debugInfoFunc_here();
	debug_vars = vector2_ptr_debugInfoVar_here();
	varstacks = vector2_int_here();
	argstacks = vector2_int_here();
	IR_symbol_table = vector2_ptr_IR_symbol_here();
	stringStore_labels = vector2_ptr_char_here();
	stringStore_strings = vector2_ptr_char_here();
	frameStarts = vector2_ptr_char_here();
	frameIndices = vector2_int_here();
	frames = vector2_ptr_frame_here();
	int i;
	for(i = 0; i < CS_list.size; i++){
		codegen_gen_code_segment(m(CS_list,get,i));
	}
}
