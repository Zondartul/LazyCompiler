#include "codegen_gen_command.h"
#include "codegen.h"
#include "semantic.h"
#include "assert.h"
#include "asm_template.h"


/// parses a string argument and returns it as a new string
char *read_string_argument(const char *str){
	const char *p1 = str;
	const char *p2 = p1;
	assert(str);
	assert(*p1 == '\"');
	p1++; p2 = p1;
	while(*p2){
		if(*p2 == '\\'){p2+=2;} // skip escape chars
		else if(*p2 == '\"'){break;}
		else{p2++;}
	}
	char *res = substr(p1,p2);
	res = unescape_string(res);
	printf("read_string_argument(%s) = [%s]\n", str, res);
	return res;
}

int countMembers() {
	int n = 0;
	for (int i = 0; i < curframe->symbols.size; i++) {
		ptr_IR_symbol S = m(curframe->symbols, get, i);
		if (!S || !S->type) { continue; }
		if (!(strcmp(S->type, "MEMBER"))) { n++; }
	}
	return n;
}

int allocMember() {
	int n = 0;
	for (int i = 0; i < curframe->symbols.size; i++) {
		ptr_IR_symbol S = m(curframe->symbols, get, i);
		if (!S || !S->type) { continue; }
		if (!(strcmp(S->type, "MEMBER"))) {
			int unit_size = S->size;
			if (S->arraysize) { unit_size *= S->arraysize; }
			n += unit_size;
		}
	}
	return n;
}

void gen_command_symbol() {
	//	const char* words[10];
	//	for (int i = 0; i < 6; i++) {
	//		words[i] = strtok(0, " ");
	//	}
	const char* word = 0;
	const char* val = strtok(0, " ");//words[0];
	const char* type = strtok(0, " ");//words[1];
	int sym_size = 1;
	int arraysize = 0;//1;
	//int wordI = 2;
	int ptrlvl = 0;
	int pos = 0;

	word = strtok(0, " ");
	if (word && !strcmp(word, "SIZE")) {
		word = strtok(0, " ");
		sscanf(word, "%d", &sym_size);
		printf("gen_command_symbol: read SIZE %d\n", sym_size);
		word = strtok(0, " ");
	}
	if (word && !strcmp(word, "ARRAY")) {
		word = strtok(0, " ");
		sscanf(word, "%d", &arraysize);
		printf("gen_command_symbol: read ARRAY %d\n", arraysize);
		//ptrlvl++;
		word = strtok(0, " ");
	}
	int byte_size = sym_size;
	if (arraysize) { byte_size *= arraysize; }
	//const char *val = strtok(0," ");
	//const char *type = strtok(0," ");
	//const char *word3 = strtok(0," ");
	//const char *word4 = strtok(0," ");


	if (codegen_decl) {
		ptr_IR_symbol S = find_IR_symbol(val);
		if (S) { error("[CODE GEN] Symbol already defined: %s (line %d)", S->IR_name, CurCMD + 1); }
		if (!S) {
			S = new_IR_symbol();
			m(curframe->symbols, push_back, S);
			S->IR_name = IR_exact_name(curframe->namespace, val);	//we're pretty much quitting with the whole "IR_name" thing //wait rly? since when?

		}

		if (!strcmp(type, "STRING")) {
			if (curframe->parent) { error("[CODE GEN] constant strings can only be defined in global IR scope "); }
			const char* cmdstring = strstr(codegen_str, "STRING") + strlen("STRING ");
			const char* str2 = read_string_argument(cmdstring);//unescape_string(cmdstring);
			S->str = str2;
			S->lbl_at = IR_inexact_name(curframe->namespace, val);
		}

		//if(word3 && word4 && !(strcmp(word3,"ARRAY"))){
		//	sscanf(word4,"%d",&arraysize);
		//	S->arraysize = arraysize;
		//	ptrlvl++;
		//}
		if (!(strcmp(type, "VAR"))) {
			if (!curframe->parent) {
				pos = 0;
				S->lbl_at = IR_inexact_name(curframe->namespace, val);
			}
			else {
				pos = allocStackVar(byte_size);
			}

		}
		if (!(strcmp(type, "MEMBER"))) {
			pos = allocMember();//countMembers();
		}
		if (!(strcmp(type, "ARG"))) {
			//if(!pos_str){pos = allocStackArg();}
			pos = allocStackArg(byte_size); //ignoring pos_string for now
		}
		if (!(strcmp(type, "LABEL"))) {
			//do nothing
		}
		S->type = stralloc(type);
		S->pointerlevel = ptrlvl;
		S->pos = pos;
		S->framedepth = curframe->depth;


		S->size = sym_size;
		//if (ptrlvl) { S->arraysize = arraysize; }
		if(arraysize){S->arraysize = arraysize;}

		//const char *lbl_from = IR_next_name(curframe->namespace,"lbl");
		//asm_println("%s:",lbl_from);
		//S->lbl_from = lbl_from;
		S->lbl_from = curframe->lbl_from;//lbl_frameStart;
		S->lbl_to = curframe->lbl_to; //may still be 0
		printf("got symbol: %s / %s / ptr=%d / pos=%d / byte_size=%d / frame=%d / from=%s\n", S->IR_name, S->type, S->pointerlevel, S->pos, byte_size, S->framedepth, S->lbl_from);
	}
	else {
		// do nothing
	}
	return;
}

void gen_command_function() {
	const char* name = strtok(0, " ");
	const char* tok3 = strtok(0, " ");
	if (codegen_decl) {
		if (strcmp(tok3, "BEGIN") == 0) {
			ptr_IR_symbol S = find_IR_symbol(name);
			if (S) { error("[CODE GEN] symbol '%s' already defined (line %d)\n", name, CurCMD + 1); }
			S = new_IR_symbol();
			S->type = stralloc("FUNC");
			S->IR_name = IR_exact_name(curframe->namespace, name);//stralloc(name);
			S->pointerlevel = 0;
			S->pos = 0;
			S->lbl_from = stralloc(name);
			S->lbl_to = IR_next_name(firstframe->namespace, "lbl");
			m(curframe->symbols, push_back, S);
			return;
		}
		else if (strcmp(tok3, "END") == 0) {
			return;
		}
	}
	else {
		if (strcmp(tok3, "BEGIN") == 0) {
			ptr_IR_symbol S = find_IR_symbol(name);
			fprintf(fasm, "\n");
			printindent();
			asm_println("%s:", S->lbl_from);
			return;
		}
		else if (strcmp(tok3, "END") == 0) {
			ptr_IR_symbol S = find_IR_symbol(name);
			printindent();
			asm_println("%s:", S->lbl_to);
			fprintf(fasm, "\n");
			return;
		}
	}
}

void gen_command_label() {
	if (codegen_decl) {
		//do nothing
	}
	else {
		const char* label = strtok(0, " ");
		printindent();
		asm_println("%s:", label);
		printTrace();
	}
	return;
}

void gen_command_ret() {
	if (codegen_decl) {
		//do nothing
	}
	else {
		printTrace();
		const char* val = strtok(0, " ");
		if (val) {
			printindent();
			asm_println("mov eax, %s", loadRValue(val));
		}
		//CurEBP = CurEBP - 1000;
		//fprintf(fasm,"leave\n");
		//freeStack();
		//curframe--;	//RET no longer recognized as end of frame (even though it is)
						//beacause without matching FRAME ENTER/LEAVE we get confused with counting frames.
		printindent();
		asm_println("leave");
		printindent();
		asm_println("ret");
	}
	return;
}

void gen_command_jmp() {
	if (codegen_decl) {
		//do nothing
	}
	else {
		//const char *val1 = loadRValue(strtok(0," "));
		//const char *val2 = loadRValue(strtok(0," "));
		const char* label = strtok(0, " ");
		printTrace();
		printindent();
		asm_println("jmp %s", label);
	}
	return;
}

void gen_command_jne() {
	if (codegen_decl) {
		//do nothing
	}
	else {
		const char* val1 = loadRValue(strtok(0, " "));
		const char* val2 = loadRValue(strtok(0, " "));
		const char* label = strtok(0, " ");
		printTrace();
		printindent();
		asm_println("cmp %s, %s", val1, val2);
		printindent();
		asm_println("jne %s", label);
	}
	return;
}

void gen_command_je() {
	if (codegen_decl) {
		//do nothing
	}
	else {
		printTrace();
		const char* val1 = loadRValue(strtok(0, " "));
		const char* val2 = loadRValue(strtok(0, " "));
		const char* label = strtok(0, " ");
		printindent();
		asm_println("cmp %s, %s", val1, val2);
		printindent();
		asm_println("je %s", label);
	}
	return;
}

const char* get_referenced_name(const char* val){
	if(val[0] == '*'){val++; return val;}
	else{
		vector2_char vstr = vector2_char_here();
		vec_printf(&vstr, "&%s", val);
		return stralloc(vstr.data);
	}
}

void gen_command_call() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* func = loadRValue(strtok(0, " "));
		//codegen_tok = strtok(0," ");
		const char* arr[20];
		int nargs = 0;
		int n_arg_bytes = 0;
		for (int i = 0; i < 20; i++) {
			arr[i] = strtok(0, " ");
			if (arr[i]) { nargs++; }
		}
		for (int i = 0; i < nargs; i++) {
			const char* codegen_tok = arr[nargs - 1 - i];
			if (!codegen_tok) { continue; }
			int arg_size = get_arg_size(codegen_tok);
			if((arg_size < 0) || (arg_size > 100)){
				int try_again = get_arg_size(codegen_tok);
				printf("debug val: %d", try_again);
				error("[CODE GEN] call: couldn't calculate argument size");
			}
			n_arg_bytes += arg_size;
			if(arg_size <= 1){
				const char* arg = loadRValue(codegen_tok);
				printindent(); asm_println("push %s", arg);
			}else{
				/// we need to copy the value byte-by-byte
				/// while preserving byte order
				/// void *ptr = (&obj+sizeof(obj))
				///	for i = 0, sizeof(obj):
				///		push(*ptr--);
				/// end
				if(comments){printindent(); asm_println("// load the %d-byte argument %s", arg_size, codegen_tok);}
				indent++;
				const char* obj_ptr = get_referenced_name(codegen_tok);
				const char* arg = loadLValue(obj_ptr);
				printindent(); asm_println("add %s, %d", arg, arg_size-1);
				for(int j = 0; j < arg_size; j++){
					printindent(); asm_println("push #%s", arg);
					printindent(); asm_println("dec %s", arg);
				}
				indent--;
				//ptr_reg R = allocRegister();
				//printindent(); asm_println("mov %s, ")
			}
			codegen_tok = strtok(0, " ");
		}
		printindent(); asm_println("call %s", func);
		printindent(); asm_println("add esp, %d", n_arg_bytes);
		storeValue(result, "eax");
		//printf("mov %s eax\n",result);
	}
	return;
}

void gen_command_mov() {
	if (codegen_decl) {
		//do nothing
		const char* dest = strtok(0, " ");
		checkResult(dest);
	}
	else {
		//const char *dest = strtok(0," ");
		//const char *src = loadRValue(strtok(0," "));
		const char* arg1 = strtok(0, " ");
		const char* arg2 = strtok(0, " ");
		//if (strcmp(arg2, "&x") == 0) {
		//	printf("debug trap");
		//}
		const char* dest = arg1;
		const char* src = loadRValue(arg2);
		printTrace();
		storeValue(dest, src);//store into value, a register
		//printf("mov %s %s\n",arg1,loadValue(arg2));
	}
	return;
}

void gen_command_memcpy(){
	if(codegen_decl){
		//do nothing
		//const char* dest = strtok(0, " ");
		//checkResult(dest2);
	}else{
		const char *arg1 = strtok(0, " ");
		const char *arg2 = strtok(0, " ");
		const char *arg3 = strtok(0, " ");

		int size = 0;
		if(!sscanf(arg3, "%d", &size)){error("CODE GEN: bad arg (size)");}
		const char* dest = loadLValue(arg1);
		const char* src = loadLValue(arg2);
		printTrace();

		for(int i = 0; i < size; i++){
			//storeValue(dest, src);
			printindent(); asm_println("mov #%s, #%s", dest, src);
			printindent(); asm_println("inc %s", dest);
			printindent(); asm_println("inc %s", src);
		}
	}
}

void gen_command_add() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		//storeValue etc etc
		//printf("mov %s %s\n",result,loadValue(arg1));
		//printf("add %s %s\n",result,loadValue(arg2));
		//storeValue(result, loadValue(arg1));
		printindent();
		asm_println("add %s, %s", arg1, arg2);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_sub() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		printTrace();
		//storeValue(result, loadValue(arg1));
		printindent();
		asm_println("sub %s, %s", arg1, arg2);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_div() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		//storeValue(result, loadValue(arg1));
		printindent();
		asm_println("div %s, %s", arg1, arg2);
		printindent();
		//asm_println("fint %s",arg1);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_mul() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		//storeValue(result, loadValue(arg1));
		printindent();
		asm_println("mul %s, %s", arg1, arg2);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_mod() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		//storeValue(result, loadValue(arg1));
		printindent();
		asm_println("mod %s, %s", arg1, arg2);
		printindent();
		asm_println("fint %s", arg1);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_exp() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		//storeValue(result, loadValue(arg1));
		printindent();
		asm_println("fpwr %s, %s", arg1, arg2);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_and() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		//storeValue(result, loadValue(arg1));
		printindent();
		asm_println("and %s, %s", arg1, arg2);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_or() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		//storeValue(result, loadValue(arg1));
		printindent();
		asm_println("or %s, %s", arg1, arg2);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_not() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		printTrace();
		printindent();
		asm_println("lneg %s", arg1);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_neg() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		printTrace();
		printindent();
		asm_println("neg %s", arg1);
		storeValue(result, arg1);
	}
	return;
}



void gen_command_deref() {
	/*
	if(codegen_decl){
		//do nothing
		const char *result = strtok(0," ");
		checkResult(result);
	}else{
		const char *result = strtok(0," ");
		const char *arg1 = loadRValue(strtok(0," "));
		printTrace();
		printindent();

	}
	*/
}


void gen_command_equal() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		printindent(); asm_println("sub %s, %s", arg1, arg2);
		printindent(); asm_println("fabs %s, %s", arg1, arg1);
		printindent(); asm_println("fsgn %s, %s", arg1, arg1);
		printindent(); asm_println("lneg %s", arg1);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_notequal() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		printindent(); asm_println("sub %s, %s", arg1, arg2);
		printindent(); asm_println("fabs %s, %s", arg1, arg1);
		printindent(); asm_println("fsgn %s, %s", arg1, arg1);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_greater() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		printindent(); asm_println("sub %s, %s", arg1, arg2);
		printindent(); asm_println("fsgn %s, %s", arg1, arg1);
		printindent(); asm_println("max %s, 0", arg1);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_less() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		printindent(); asm_println("sub %s, %s", arg1, arg2);
		printindent(); asm_println("fsgn %s, %s", arg1, arg1);
		printindent(); asm_println("min %s, 0", arg1);
		printindent(); asm_println("neg %s", arg1);
		storeValue(result, arg1);
	}
	return;
}


void gen_command_leq() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		//storeValue(result, loadValue(arg2));
		//const char *rA = loadValue(result);
		//const char *rB = loadValue(arg1);
		printindent(); asm_println("sub %s, %s", arg1, arg2);
		printindent(); asm_println("fsgn %s, %s", arg1, arg1);
		printindent(); asm_println("max %s, 0", arg1);
		printindent(); asm_println("lneg %s", arg1);
		storeValue(result, arg1);
	}
	return;
}

void gen_command_geq() {
	if (codegen_decl) {
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result);
	}
	else {
		printTrace();
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		const char* arg2 = loadRValue(strtok(0, " "));
		printindent(); asm_println("sub %s, %s", arg1, arg2);
		printindent(); asm_println("fsgn %s, %s", arg1, arg1);
		printindent(); asm_println("min %s, 0", arg1);
		printindent(); asm_println("neg %s", arg1);
		printindent(); asm_println("lneg %s", arg1);
		storeValue(result, arg1);
	}
	return;
}


void gen_command_alloc() {
	error("[CODE GEN] deprecated ");
	/*
	codegen_tok = strtok(0," ");
	if(strcmp(codegen_tok,"STACK")==0){
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

void gen_command_struct() {
	//create a new symbol table, add symbols (variables) there. Do not initialize them.
	//global <- symbol struct
	//new frame
	// symbols
	//struct <- frame
	if (codegen_decl) {
		const char* name = strtok(0, " ");
		codegen_tok = strtok(0, " ");
		if (!name || !codegen_tok) { error("[CODE GEN] bad command"); }
		if (strcmp(codegen_tok, "BEGIN") == 0) {
			ptr_IR_symbol S = find_IR_symbol(name);
			if (S) { error("[CODE GEN] symbol '%s' already defined (line %d)\n", name, CurCMD + 1); }
			S = new_IR_symbol();
			S->type = stralloc("STRUCT");
			S->IR_name = IR_exact_name(curframe->namespace, name);//stralloc(name);
			S->pointerlevel = 0;
			S->pos = 0;
			S->lbl_from = stralloc(name);
			S->lbl_to = IR_next_name(firstframe->namespace, "lbl");
			m(curframe->symbols, push_back, S);

			push_frame();
			new_frame();
			curframe->isStruct = 1;
			S->scope = curframe;
			curframe->cmd_index = cmd_index;
			curframe->lbl_from = S->lbl_from;
			printf("new struct frame:\n");
			printframe(curframe);
		}
		else if (strcmp(codegen_tok, "END") == 0) {
			ptr_IR_symbol S = find_IR_symbol(name);
			if (!S) { error("[CODE GEN] bad command"); }
			pop_frame();
		}
		else { error("[CODE GEN] bad command"); }
	}
	else {
		const char* name = strtok(0, " ");
		codegen_tok = strtok(0, " ");
		if (!name || !codegen_tok) { error("[CODE GEN] bad command"); }
		if (strcmp(codegen_tok, "BEGIN") == 0) {
			ptr_IR_symbol S = find_IR_symbol(name);
			if (!S) { error("[CODE GEN] internal error (line %d)\n", name, CurCMD + 1); }
			asm_println("%s:", S->lbl_from);
		}
		else if (strcmp(codegen_tok, "END") == 0) {
			ptr_IR_symbol S = find_IR_symbol(name);
			if (!S) { error("[CODE GEN] internal error"); }
			asm_println("%s:", S->lbl_to);
		}
		else { error("[CODE GEN] bad command"); }
	}
	return;
}

void gen_command_using() {
	//ignore
	return;
}

void gen_command_insert() {
	if (codegen_decl) {
		//do nothing
	}
	else {
		printTrace();
		const char* cs = strtok(0, " ");
		asm_println("#include \"%s\"", cs);
	}
	return;
}

void gen_command_frame() {
	if (codegen_decl) {
		codegen_tok = strtok(0, " ");
		if (!codegen_tok) { error("[CODE GEN] bad command"); }
		if (strcmp(codegen_tok, "ENTER") == 0) {
			const char* lbl = IR_next_name(firstframe->namespace, "lbl");
			push_frame();
			new_frame();
			curframe->cmd_index = cmd_index;
			curframe->lbl_from = lbl;
			printf("new current frame:\n");
			printframe(curframe);
			return;
		}
		if (strcmp(codegen_tok, "LEAVE") == 0) {
			const char* lbl = IR_next_name(firstframe->namespace, "lbl");
			curframe->lbl_to = lbl;
			int i;
			for (i = 0; i < curframe->symbols.size; i++) {
				ptr_IR_symbol S = m(curframe->symbols, get, i);
				//if(!S->lbl_to){S->lbl_to = curframe;}
				//or just unconditionally
				S->lbl_to = curframe->lbl_to;
			}
			pop_frame();
			printf("leaving for old frame:\n");
			printframe(curframe);
			return;
		}
	}
	else {
		//printTrace();
		codegen_tok = strtok(0, " ");
		if (!codegen_tok) { error("[CODE GEN] bad command"); }
		if (strcmp(codegen_tok, "ENTER") == 0) {

			//error("NOT IMPLEMENTED");
			//asm_println("enter %d",numVars);
			printTrace();
			push_frame();
			curframe = find_frame_by_cmd_index(cmd_index);
			//curframe++;
			printf("entering frame:\n");
			printframe(curframe);
			//CurEBP = CurEBP + 1000;
			//asm_println("mov esp, %d",CurEBP);
			printindent();
			asm_println("%s:", curframe->lbl_from);
			printf("lbl_from (%s) printed\n", curframe->lbl_from);
			printindent();
			asm_println("enter %d", curframe->stackvarsize);
			return;
		}
		if (strcmp(codegen_tok, "LEAVE") == 0) {
			//CurEBP = CurEBP - 1000;
			printindent();
			asm_println("leave");
			printindent();
			asm_println("ret"); //every ret needs a leave before it, and we can't have two leaves, so we ret after leave
			printindent();
			asm_println("%s:", curframe->lbl_to);
			//freeStack();
			printTrace();
			pop_frame();
			printf("back to frame:\n");
			printframe(curframe);
			return;
		}
	}
}

void gen_command_debug() {
	//debug info
	//DEBUG BEGIN FUNC foo
	//DEBUG BEGIN VAR a
	//DEBUG END VAR a
	//DEBUG END FUNC foo
	/*
	const char *tok2 = strtok(0," ");
	const char *tok3 = strtok(0," ");
	const char *tok4 = strtok(0," ");
	const char *tok5 = strtok(0," ");
	const char *tok6 = strtok(0," ");
	const char *label = IR_next_name(curframe->namespace,"lbl");

	if(codegen_decl){
		//do nothing
		printf("found [DEBUG] in decl, ignoring\n");
		return;
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
				V->framedepth = curframe->depth;
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
			//const char *label = IR_next_name(curframe->namespace,"debug_info");
			//make sure it's a reserved name at some point
			//printTraceCode();
			//printDebugInfo();
			return;
		}
	}
	*/
}

//char *gen_read_quoted_string(){
//	
//}

void gen_command_comment(){
	if (codegen_decl) {
	}
	else {
		#define TOK_IS(x) (strcmp(codegen_tok, x) == 0)
		codegen_tok = strtok(0, " ");
		if (!codegen_tok) { error("[CODE GEN] bad command"); }
		if (TOK_IS("GENERAL") || TOK_IS("SOURCE") || TOK_IS("LINE")) {
			codegen_tok = strtok(0,"\0");
			assert(codegen_tok);
			const char *str = read_string_argument(codegen_tok);//strtok(0,"\"");
			asm_println("// %s", str);
			return;
		}else error("[CODE GEN] bad command");
	}
}

void gen_command_floor(){
	if(codegen_decl){
		//do nothing
		const char* result = strtok(0, " ");
		checkResult(result); /// "declare" the result temporary
	}else{
		const char* result = strtok(0, " ");
		const char* arg1 = loadLValue(strtok(0, " "));
		printTrace();
		printindent();
		asm_println("fint %s", arg1);
		storeValue(result, arg1);
	}
}

enum CONVERT_TYPE {CONV_ERROR, CONV_I32, CONV_U8, CONV_F64};
enum CONVERT_TYPE str_to_convtype(const char *str){
	assert(str);
	if(strcmp(str, "I32") == 0){return CONV_I32;}
	if(strcmp(str, "U8") == 0){return CONV_U8;}
	if(strcmp(str, "F64") == 0){return CONV_F64;}
	error("[CODE GEN] unknown conversion type [%s]", str);
	return CONV_ERROR;
}

#define STR_INT_MAX "2147483647"
#define STR_INT_MIN "-2147483648"
#define STR_UINT_MAX "4294967295"

void gen_command_convert(){
	if(codegen_decl){
		//do noting
		const char* result = strtok(0, " ");
		checkResult(result); /// "declare" the result temporary
	}else{
		const char* result = strtok(0, " ");
		const char* str_arg1 = strtok(0, " ");
		const char *arg1 = loadLValue(str_arg1);
		enum CONVERT_TYPE type_from = str_to_convtype(strtok(0, " "));
		enum CONVERT_TYPE type_to = str_to_convtype(strtok(0, " "));

		switch(type_from){
			case CONV_I32:
			{
				switch(type_to){
					case CONV_I32:
					/// do nothing
					break;
					case CONV_U8:
					{
						printindent(); asm_println("mod %s, 255", arg1);
						printindent(); asm_println("fint %s", arg1);
					}
					break;
					case CONV_F64:
					/// do nothing
					break;
					default: error("[CODE_GEN] unsupported conversion"); break;
				}
			}
			break;
			case CONV_U8:
			{
				switch(type_to){
					case CONV_I32:
					/// do nothing
					break;
					case CONV_U8:
					/// do nothing
					break;
					case CONV_F64:
					/// do nothing
					break;
					default: error("[CODE GEN] unsupported conversion"); break;
				}
			}
			break;
			case CONV_F64:
			{
				switch(type_to){
					case CONV_I32:
					{
						printindent(); asm_println("sub %s, " STR_INT_MIN, arg1);
						printindent(); asm_println("mod %s, " STR_UINT_MAX, arg1);
						printindent(); asm_println("add %s, " STR_INT_MIN, arg1);
						printindent(); asm_println("fint %s", arg1);
					}
					break;
					case CONV_U8:
					{
						printindent(); asm_println("mod %s, 255", arg1);
						printindent(); asm_println("fint %s", arg1);
					}
					break;
					case CONV_F64:
					/// do nothing
					break;
					default: error("[CODE GEN] unsupported conversion"); break;
				}
			}
			break;
			default: error("[CODE GEN] unsupported conversion"); break;
		}
		storeValue(result, arg1);
	}
}

void gen_command_exit(){
	if(codegen_decl){
		//do nothing
	}else{
		printindent(); asm_println("int 1"); // halt
		printindent(); asm_println("int 0"); // reset
	}
}

void gen_command_asm(){
	if(codegen_decl){
		const char* cmdstring = strstr(codegen_str, "ASM") + strlen("ASM ");
		const char* str = read_string_argument(cmdstring);//unescape_string(cmdstring);
		
		struct asm_template tmpl = parse_asm_template(str);
		checkTemplateVals(&tmpl);
	}else{
		const char* cmdstring = strstr(codegen_str, "ASM") + strlen("ASM ");
		const char* str = read_string_argument(cmdstring);//unescape_string(cmdstring);
		//const char* arg_str_begin = str+1;
		//const char* arg_str_end = strchr(arg_str_begin, '"');
		//assert(arg_str_end);
		//str = substr(arg_str_begin, arg_str_end);

			printf("gen_command_asm begin;");
			printf("  cmdstring = [%s]\n",cmdstring);
			printf("  str = [%s]\n", str);
		
		struct asm_template tmpl = parse_asm_template(str);
			printf("  tmpl.src_text = [%s]\n", tmpl.src_text);
		load_template_registers(&tmpl);
		stitch_asm_template(&tmpl);
			printf("  tmpl.res_text = [%s]\n", tmpl.res_text);

		printindent(); asm_println("// --- asm block begin ---");
		asm_println("%s", tmpl.res_text);
		printindent(); asm_println("// --- asm block end ---"); 
		store_template_registers(&tmpl);
			printf("gen_command_asm end.");
	}
}