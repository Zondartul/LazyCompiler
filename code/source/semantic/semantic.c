#include "stdio.h"
#include "vector2.h"

//#include "globals.h" do not include or fopen will be overridden
#include "semantic.h"
#include "stdlib.h"
#include "string.h"

#include "AST.h"
#include "stdarg.h"
#include "codegen.h"

#include "yaccin.tab.h" //for get_source_text2()
#include "semantic_analyze.h"
//defines

//generated
//definition_vector_of(ptr_file);

//variables
int semantic_decl = 0;
struct code_segment *init_CS  = 0;
ptr_symbol symbolThis = 0;

vector2_ptr_char expr_stack;
vector2_ptr_type_name exprtype_stack;
vector2_ptr_char *namespace_semantic = 0;
int semantic_analyze_pad = 0;
const char *semantic_this = 0;
int semantic_flatten = 1;
int lvalue = 0;
struct ast_node *curNode = 0;
int emit_code_pad = 0;
int nodeCountDecl = 0;
int nodeCountImp = 0;
//functions
//void point_out_error(int line, int pos, const char *file, const char *s, int length);
//extern void printPos(YYLTYPE pos);

//char *escape_string(const char *str);
//char *unescape_string(const char *str);
//char *get_source_text2(YYLTYPE pos);
//void class_emit();
//void class_def_finalize();

// vector ST_stack;
// vector CS_stack;
// vector expr_stack;
//what is the context of our parsing

void semantic_init_analyze(struct ast_node* node) {
	curNode = node;
	if (semantic_decl) { nodeCountDecl++; }
	else { nodeCountImp++; }
	//int trigger = (nodeCountImp == 8);
	int i;
	for (i = 0; i < semantic_analyze_pad; i++) { printf(" "); }
	semantic_analyze_pad++;
	printf("semantic got %s,", node->token.type);
	if (node->token.value) { printf(" \"%s\",", node->token.value); }
	printf(" prod %d,", node->token.production);
	printf(" @ %d-%d,", node->token.pos.start, node->token.pos.end);
	if (semantic_decl) { printf(" declarative pass (%d)\n", nodeCountDecl); }
	else { printf(" imperative pass (%d)\n", nodeCountImp); }
	printf("-----------------\n%s\n-----------------\n",
		removeComments(get_source_text2(node->token.pos)));//get_source_text(node->token.pos.start,node->token.pos.end,node->token.pos.filename));
	//printf("node->token.pos (1) =\n");
	//printPos(node->token.pos);	
	//printf("node->token.pos (2) =\n");
	//printPos(node->token.pos);
	//printf("----------------------\n");
}

void semantic_fin_analyze(struct ast_node* node) {
	//if(trigger){
	//	trigger = 0;
	//	printf("\n\npos after pass 8:===========\n");
	//	printPos(node->token.pos);
	//	printf("=============================\n\n\n");
	//}
	semantic_analyze_pad--;
	//printmemory(stdout);
}

void semantic_fail_dispatch(struct ast_node* node) {
	YYLTYPE pos = node->token.pos;
	err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
	error("semantic: unknown node type: [%s]\n", node->token.type);
}

int semantic_dispatch_expr_op(struct ast_node* node, expr_settings stg) {
	if (!strcmp(node->token.type, "expr_^"))	{ semantic_analyze_expr_pow			(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_/"))	{ semantic_analyze_expr_divide		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_*"))	{ semantic_analyze_expr_multiply	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_%"))	{ semantic_analyze_expr_modulo		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_and"))	{ semantic_analyze_expr_and			(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_or"))	{ semantic_analyze_expr_or			(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_not"))	{ semantic_analyze_expr_not			(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_-"))	{ semantic_analyze_expr_minus		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_+"))	{ semantic_analyze_expr_plus		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_++"))	{ semantic_analyze_expr_increment	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_--"))	{ semantic_analyze_expr_decrement	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_=="))	{ semantic_analyze_expr_equals		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_!="))	{ semantic_analyze_expr_notequal	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_>"))	{ semantic_analyze_expr_greater		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_<"))	{ semantic_analyze_expr_less		(node, stg);	return 1; }
	return 0;
}

int semantic_dispatch_expr(struct ast_node* node, expr_settings stg) {
	if (!strcmp(node->token.type, "var_decl_assign"))	{ semantic_analyze_var_decl_assign(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_list"))		{ semantic_analyze_expr_list	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_id"))		{ semantic_analyze_expr_id		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_const"))	{ semantic_analyze_expr_const	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_subexpr"))	{ semantic_analyze_expr_subexpr	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_index"))	{ semantic_analyze_expr_index	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_call"))		{ semantic_analyze_expr_call	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_."))		{ semantic_analyze_expr_dot		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_neg"))		{ semantic_analyze_expr_neg		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_deref"))	{ semantic_analyze_expr_deref	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_ref"))		{ semantic_analyze_expr_ref		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_="))		{ semantic_analyze_expr_assign	(node, stg);	return 1; }
	if (semantic_dispatch_expr_op(node, stg)) { return 1; }
	return 0;
}

int semantic_dispatch_if(struct ast_node* node, if_settings stg) {
	if (!strcmp(node->token.type, "if_then")) { semantic_analyze_if_then(node, stg); return 1; }
}

int semantic_dispatch_general(struct ast_node* node) {
	if (!strcmp(node->token.type, "program"))			{ semantic_analyze_program(node);			return 1; }
	if (!strcmp(node->token.type, "decl_stmt_list"))	{ semantic_analyze_decl_stmt_list(node);	return 1; }
	if (!strcmp(node->token.type, "decl_stmt"))			{ semantic_analyze_decl_stmt(node);			return 1; }
	if (!strcmp(node->token.type, "func_def"))			{ semantic_analyze_func_def(node);			return 1; }
	if (!strcmp(node->token.type, "var_decl_list"))		{ semantic_analyze_var_decl_list(node);		return 1; }
	if (!strcmp(node->token.type, "var_decl"))			{ semantic_analyze_var_decl(node);			return 1; }
	if (!strcmp(node->token.type, "var_decl_assign"))	{ semantic_analyze_var_decl_assign(node, EXPR_SETTINGS_NULL);	return 1; }
	if (!strcmp(node->token.type, "stmt_list"))			{ semantic_analyze_stmt_list(node);			return 1; }
	if (!strcmp(node->token.type, "stmt"))				{ semantic_analyze_stmt(node);				return 1; }
	if (!strcmp(node->token.type, "imp_stmt"))			{ semantic_analyze_imp_stmt(node);			return 1; }
	if (!strcmp(node->token.type, "if_block"))			{ semantic_analyze_if_block(node);			return 1; }
	if (!strcmp(node->token.type, "while_loop"))		{ semantic_analyze_while_loop(node);		return 1; }
	if (!strcmp(node->token.type, "for_loop"))			{ semantic_analyze_for_loop(node);			return 1; }
	if (!strcmp(node->token.type, "class_def"))		{ semantic_analyze_class_def(node);			return 1; }
	//it could be an expression (imperative statement)
	//where we ignore the result.
	expr_settings stg = { .res_type = E_DISCARD };
	if (semantic_dispatch_expr(node, stg)) {return 1;}
	return 0;
}



void semantic_general_analyze(struct ast_node *node){
	semantic_init_analyze(node);
	if (semantic_dispatch_general(node)) {
		//semantic_exit:
		semantic_fin_analyze(node);
	}else {
		//unknown node type
		semantic_fail_dispatch(node);
	}
	return;
}


void semantic_expr_analyze(struct ast_node* node, expr_settings stg) {
	semantic_init_analyze(node);
	if (semantic_dispatch_expr(node, stg)) {
		//semantic_exit:
		semantic_fin_analyze(node);
	}
	else {
		//unknown node type
		semantic_fail_dispatch(node);
	}
	return;
}

void semantic_if_analyze(struct ast_node* node, if_settings stg) {
	semantic_init_analyze(node);
	if (semantic_dispatch_if(node, stg)) {
		//semantic_exit:
		semantic_fin_analyze(node);
	}
	else {
		//unknown node type
		semantic_fail_dispatch(node);
	}
	return;
}



void bufferizeWithoutComments(char *buff, const char *str, int num);
char *removeComments(const char *str){
	char buff[2000];
	bufferizeWithoutComments(buff,str,2000);
	return stralloc(buff);
}

void semantic_finalize(){
	
	printf("=========================\n");
	printf("=== SEMANTIC FINALIZE ===\n");
	printf("=========================\n");
	
	currentSymbolTable = m(ST_list,get,0);
	struct symbol *M = lookup_symbol("main");
	if((!M)||(M->type != SYMBOL_FUNCTION)){
		error("semantic error: missing function main()\n");
	}
	if(semantic_flatten){
		currentCodeSegment = m(CS_list,get,0);
		printf("current CS = %s\n",currentCodeSegment->name);
		int i;
		struct code_segment *CS = currentCodeSegment;
		currentCodeSegment = init_CS;
		const char *result = IR_next_name(namespace_semantic,"temp");
		emit_code("CALL %s _main",result); //idk how to pass main's params yet.
		emit_code_segment(CS);
		/*
		for(i = 0; i < CS_list.size; i++){
			struct code_segment *CS = m(CS_list,get,i);
			if(CS != currentCodeSegment){
				emit_code_segment(CS);
			}
		}
		*/
		/*
		for(i = 1; i < CS_list.size;i++){
			printf("emitting %s\n",m(CS_list,get,i)->name);
			emit_code_segment(m(CS_list,get,i));
		}
		*/
		m(CS_list,clear);
		m(CS_list,push_back,currentCodeSegment);
		currentSymbolTable = M->symfunction.scope;
		while(currentSymbolTable->parent){currentSymbolTable = currentSymbolTable->parent;}
		for(i = 0; i < ST_list.size; i++){
			struct symbol_table *ST = m(ST_list,get,i);
			if(ST != currentSymbolTable){
				int j;
				for(j = 0; j < ST->symbols.size; j++){
					push_symbol(m(ST->symbols,get,j));
				}
			}
		}
		//emit_code("DEBUG INFO"); //also put it in non-flat versions somewhere
		m(ST_list,clear);
		m(ST_list,push_back,currentSymbolTable);
	}
}

void class_constructor_emit(struct symbol *S){
	emit_code("/* default class constructor */");
	emit_code("FUNCTION %s BEGIN",S->IR_name);
	push_symbol_table();
	new_symbol_table(0);
	push_code_segment();
	new_code_segment();
	S->symfunction.code = currentCodeSegment;
	S->symfunction.scope = currentSymbolTable;
		emit_code("FRAME ENTER");
		emit_all_declarations();
		//emit_code("LABEL %s",S->IR_name);
		emit_all_initializers();
		emit_code("RET");
		emit_all_undeclarations();
		emit_code("FRAME LEAVE");
	pop_code_segment();
	pop_symbol_table();
	emit_code_segment(S->symfunction.code);
	emit_code("FUNCTION %s END",S->IR_name);
	emit_code("/* end */");
}

void class_destructor_emit(struct symbol *S){
	emit_code("/* default class destructor */");
	emit_code("FUNCTION %s BEGIN",S->IR_name);
	push_symbol_table();
	new_symbol_table(0);
	push_code_segment();
	new_code_segment();
	S->symfunction.code = currentCodeSegment;
	S->symfunction.scope = currentSymbolTable;
		emit_code("FRAME ENTER");
		emit_all_declarations();
		//emit_code("LABEL %s",S->IR_name);
		emit_all_deinitializers();
		emit_code("RET");
		emit_all_undeclarations();
		emit_code("FRAME LEAVE");
	pop_code_segment();
	pop_symbol_table();
	emit_code_segment(S->symfunction.code);	
	emit_code("FUNCTION %s END",S->IR_name);
	emit_code("/* end */");
}

void class_emit_start(){
	//emit struct code so member addresses are available
	emit_code("STRUCT %s BEGIN", symbolThis->IR_name);
	//for(i = 0; i < currentSymbolTable->symbols.size; i++){}
	const char *smt = semantic_this;
	semantic_this = 0;
	push_symbol_table();
	currentSymbolTable = symbolThis->symclass.scope;
	emit_all_declarations();
	pop_symbol_table();
	semantic_this = smt;
	if(symbolThis->symclass.defaultConstructor){class_constructor_emit(symbolThis->symclass.defaultConstructor);}
	if(symbolThis->symclass.defaultDestructor){class_constructor_emit(symbolThis->symclass.defaultDestructor);}
	//emit_code("STRUCT %s END", symbolThis->IR_name);
}

void class_emit_end(){
	emit_code("STRUCT %s END", symbolThis->IR_name);
}

void class_def_finalize(){
	if(!symbolThis){error("[SEMANTIC] class_finalize should be called in the context of some class");}
//	struct symbol *symbol_this = lookup_symbol("this")->symvariable.type->symclass;
	
	int i;
	int constructor_found = 0;
	int destructor_found = 0;
	for(i = 0; i < currentSymbolTable->symbols.size; i++){
		struct symbol *S = m(currentSymbolTable->symbols,get,i);
		if(!strcmp(S->username,"constructor")){constructor_found = 1;}
		if(!strcmp(S->username,"destructor")){destructor_found = 1;}
	}
	if(!constructor_found){
		struct symbol *S = symbol_new0();//new_symbol();
		push_symbol(S);
		//push_symbol_table();
		//struct symbol_table *ST = currentSymbolTable;
		//new_symbol_table(0);
		//push_code_segment();
		//new_code_segment();
		const char *name = "constructor";
		const char *IR_name = IR_next_name(namespace_semantic,name);
		S->type = SYMBOL_FUNCTION;
		S->storage = STORE_CODE;
		S->username = name;
		S->IR_name = IR_name;
		S->symfunction.returntype = semantic_get_type("void");
		S->symfunction.code = 0;
		S->symfunction.scope = 0;
		symbolThis->symclass.defaultConstructor = S;
	}else{symbolThis->symclass.defaultConstructor = 0;}
	if(!destructor_found){
		struct symbol *S = symbol_new0();//new_symbol();
		push_symbol(S);
		//push_symbol_table();
		//struct symbol_table *ST = currentSymbolTable;
		//new_symbol_table(0);
		//push_code_segment();
		//new_code_segment();
		const char *name = "destructor";
		const char *IR_name = IR_next_name(namespace_semantic,name);
		S->type = SYMBOL_FUNCTION;
		S->storage = STORE_CODE;
		S->username = name;
		S->IR_name = IR_name;
		S->symfunction.returntype = semantic_get_type("void");
		S->symfunction.code = 0;
		S->symfunction.scope = 0;
		symbolThis->symclass.defaultDestructor = S;
	}else{symbolThis->symclass.defaultDestructor = 0;}
}

void emit_all_decl_helper(){
	if(symbolThis){
		emit_code("USING %s",symbolThis->IR_name);
	}
	int i = 0;
	for(i = 0; i < currentSymbolTable->symbols.size; i++){
		struct symbol *S = m(currentSymbolTable->symbols,get,i);
		push_code_segment();
		if(S->global){
			currentCodeSegment = init_CS;
		}
		if(S->type == SYMBOL_PARAM || S->type == SYMBOL_VARIABLE){
			if(S->symvariable.array){
				if(S->type == SYMBOL_PARAM){emit_code("SYMBOL %s ARG ARRAY %d",S->IR_name, S->symvariable.arraysize);}//emit_code("DEBUG BEGIN VAR %s %s",S->IR_name,S->username);}
				if(S->type == SYMBOL_VARIABLE){emit_code("SYMBOL %s VAR ARRAY %d",S->IR_name, S->symvariable.arraysize);}//emit_code("DEBUG BEGIN VAR %s %s",S->IR_name,S->username);}
			}else{
				if(S->type == SYMBOL_PARAM){emit_code("SYMBOL %s ARG",S->IR_name);}//emit_code("DEBUG BEGIN VAR %s %s",S->IR_name,S->username);}
				if(S->type == SYMBOL_VARIABLE){emit_code("SYMBOL %s VAR",S->IR_name);}//emit_code("DEBUG BEGIN VAR %s %s",S->IR_name,S->username);}
			}
		}else{
			//if(S->type == SYMBOL_FUNCTION){emit_code("SYMBOL %s FUNC",S->IR_name);} nah, they're both define-anywhere
			//if(S->type == SYMBOL_CLASS){emit_code("SYMBOL %s STRUCT",S->IR_name);}
		}
		pop_code_segment();
	}
}

void emit_all_declarations(){
	emit_all_decl_helper();
	if(semantic_this){
		//emit_code("semantic_this = \'%s\' tho",semantic_this);
		push_symbol_table();
		currentSymbolTable = lookup_symbol("this")->symvariable.type->symclass->symclass.scope;
		//emit_code("has like %d symbols in it",currentSymbolTable->symbols.size);
		emit_all_decl_helper();
		pop_symbol_table();
	}
}

void emit_all_undeclarations(){
	return; //we don't need undeclarations or forgets, because leaving a frame automatically erases local symbols
	/*
	int i = 0;
	for(i = 0; i < currentSymbolTable->symbols.size; i++){
		struct symbol *S = m(currentSymbolTable->symbols,get,i);
		if(S->type == SYMBOL_PARAM){emit_code("DEBUG END VAR %s",S->IR_name);}
		if(S->type == SYMBOL_VARIABLE){emit_code("DEBUG END VAR %s",S->IR_name);}
	}
	*/
}

void emit_all_initializers(){
	int i = 0;
	for(i = 0; i < currentSymbolTable->symbols.size; i++){
		struct symbol *S = m(currentSymbolTable->symbols,get,i);
		if(S->type == SYMBOL_VARIABLE){emit_initializer(S);}
	}
}

void emit_initializer(struct symbol *S){
	struct type_name *T = S->symvariable.type;
	/*
	if(S->storage == STORE_DATA_STACK){
		emit_code("ALLOC STACK %s %d",S->IR_name,S->symvariable.size); //getTypeSize(S->symvariable.type));
	}
	*/
	if(T->symclass){
		emit_code("/* initialize %s */",S->username);
		//emit_code("/* construct %s */",S->username);
		struct symbol *S2 = T->symclass;
		push_symbol_table();
		currentSymbolTable = S2->symclass.scope;
		struct symbol *S3 = lookup_symbol("constructor");
		const char *exprResult = IR_next_name(namespace_semantic,"temp");
		//emit_code("CALL %s %s %s",exprResult,S3->IR_name,S->IR_name);
		emit_code("CALL %s %s %s",exprResult, S3->IR_name,S->IR_name);
		pop_symbol_table();
	}else{
		//primitive type
		//emit_code("%s = 0",S->IR_name);
		//emit_code("MOV %s 999",S->IR_name); //magic number to tell when a var is initialized
		//do not initialize primivites
	}
}

void emit_all_deinitializers(){
	int i = 0;
	for(i = 0; i < currentSymbolTable->symbols.size; i++){
		struct symbol *S = m(currentSymbolTable->symbols,get,i);
		if(S->type == SYMBOL_VARIABLE){emit_deinitializer(S);}
	}
}

void emit_deinitializer(struct symbol *S){
	struct type_name *T = S->symvariable.type;
	if(T->symclass){
		emit_code("/* destroy %s */",S->username);
		struct symbol *S2 = T->symclass;
		push_symbol_table();
		currentSymbolTable = S2->symclass.scope;
		struct symbol *S3 = lookup_symbol("destructor");
		const char *exprResult = IR_next_name(namespace_semantic,"temp");
		emit_code("CALL %s %s %s",exprResult,S3->IR_name,S->IR_name);
		pop_symbol_table();
	}else{
		//primitive type
		//do nothing
		//emit_code("MOV %s 0",S->IR_name);
	}
}

void analyze_scope(struct ast_node *N, 
					struct code_segment **CSin,
					struct code_segment **CSout,
					struct symbol_table **STin,
					struct symbol_table **STout,
					int noEnterLeave){
	push_symbol_table();
	if(STin){currentSymbolTable = *STin;}
	else{
		if(semantic_decl){
			new_symbol_table(N);
		}else{
			currentSymbolTable = find_symbol_table_by_node(N);
		}
	}
	if(STout){*STout = currentSymbolTable;}
	
	if(semantic_decl){
		printf("analyzing (decl) scope %s\n",currentSymbolTable->name);
		printf("symbol table so far:\n");
		print_symbol_table(currentSymbolTable);
		semantic_general_analyze(N);
	}else{
		struct code_segment *CCS = currentCodeSegment;
		if(CCS){push_code_segment();}
		if(CSin){currentCodeSegment = *CSin;}
		else{
			new_code_segment();
			if(!noEnterLeave){emit_code("FRAME ENTER");}
		}
		if(CSout){*CSout = currentCodeSegment;}
		
		if(!currentSymbolTable){error("Error: no symbol table ");}
		if(!currentCodeSegment){error("Error: no code segment ");}
		printf("analyzing (imp) scope %s / %s\n",currentSymbolTable->name,currentCodeSegment->name);
		printf("symbol table so far:\n");
		print_symbol_table(currentSymbolTable);	
		emit_all_declarations();
		emit_all_initializers();
		semantic_general_analyze(N);
		emit_all_deinitializers();
		emit_all_undeclarations();
		if(!noEnterLeave){emit_code("FRAME LEAVE");}
		if(CCS){pop_code_segment();}			
	}
	pop_symbol_table();
}


/* replaced with symbol_new0()
struct symbol *new_symbol(){
	struct symbol *S = malloc(sizeof(struct symbol));
	S->type = SYMBOL_ERROR;
	S->global = 0;
	//S->storage = STORE_ERROR;
	S->username = 0;
	S->IR_name = 0;
	S->framedepth = 0;
	return S;
}
*/

int getSymbolTableDepth(ptr_symbol_table T){
	if(T->parent){return 1+getSymbolTableDepth(T->parent);}
	else{return 0;}
}

void push_symbol(struct symbol *S){
	printf("pushing symbol %s -> %s\n",S->username,currentSymbolTable->name);
	//vector_push_back(&currentSymbolTable->symbols, &S);
	S->framedepth = getSymbolTableDepth(currentSymbolTable);
	m(currentSymbolTable->symbols,push_back,S);
}

char *escape_string(const char *str){
	int i = 0;
	int len = 0;
	char C = 0;
	while(1){
		C = str[i++];
		if(C == 0){break;}
		if(
		(C == '\n')||
		(C == '\t')||
		(C == '\"')||
		(C == '\'')
		)
		{len += 2; continue;}
		len++;
	}
	char *buff = malloc(len+1);
	i = 0;
	int buffi = 0;
	while(1){
		C = str[i++];
		//printf("C = [%c]\n",C);
		if(C == 0){break;}
		if(C == '\n'){buff[buffi++] = '\\'; buff[buffi++] = 'n'; continue;}
		if(C == '\t'){buff[buffi++] = '\\'; buff[buffi++] = 't'; continue;}
		if(C == '\"'){buff[buffi++] = '\\'; buff[buffi++] = '\"'; continue;}
		if(C == '\''){buff[buffi++] = '\\'; buff[buffi++] = '\''; continue;}
		if(C == '\\'){buff[buffi++] = '\\'; buff[buffi++] = '\\'; continue;}
		buff[buffi++] = C;
	}
	buff[buffi] = 0;
	//printf("string [%s] escaped to [%s]\n",str,buff);
	return buff;
}

char *unescape_string(const char *str){
	int i = 0;
	//int len = 0; //unused
	char *buff = malloc(strlen(str));
	int buffi = 0;
	char C = 0;
	char C2 = 0;
	while(1){
		C = str[i++];
		if(C == 0){break;}
		if(C == '\\'){
			C2 = str[i++];
			if(C2 == 'n'){buff[buffi++] = '\n'; continue;}
			if(C2 == 't'){buff[buffi++] = '\t'; continue;}
			if(C2 == '\"'){buff[buffi++] = '\"'; continue;}
			if(C2 == '\''){buff[buffi++] = '\''; continue;}
			if(C2 == '\\'){buff[buffi++] = '\\'; continue;}
			error("error: unescape_string: unrecognized escape sequence \'%c%c\' ",C,C2);
		}
		buff[buffi++] = C;
	}
	buff[buffi] = 0;
	//printf("string [%s] unescaped to [%s]\n",str,buff);
	return buff;
}

struct type_name *parseTypename(struct ast_node *N){
	struct type_name *T = semantic_get_type(N->token.value);
	//struct ast_node *ptr_stars = m(N->children,get,0);//ast_get_node(m(N->children,get,0));
	struct ast_node *ptr_stars = m(N->children,get,1); //22.03.2022 ptr_stars were not being detected
	T->pointerlevel = ptr_stars->children.size;
	return T;
	/*
	switch(ptr_stars->token.production){
		case(0):
			ptr_stars_ne = ast_get_node(m(ptr_stars->children,get,0));
			T->pointerlevel = countStars(ptr_stars_ne);
			return T;
			break;
		case(1):
			return T;
			break;
		default:
			error("unknown production\n");
			break;
	};
	*/
}

struct type_name *semantic_get_type(const char *str){
	struct type_name *T = malloc(sizeof(struct type_name));
	T->args = 0;
	T->pointerlevel = 0;
	if(!strcmp(str, "int")){
		T->name = "int";
		T->symclass = 0;
		return T;
	}
	if(!strcmp(str, "float")){
		T->name = "float";
		T->symclass = 0;
		return T;
	}
	if(!strcmp(str, "char")){
		T->name = "char";
		T->symclass = 0;
		return T;
	}
	if(!strcmp(str, "string")){
		T->name = "string";
		T->symclass = 0;
		return T;
	}
	if(!strcmp(str, "void")){
		T->name = "void";
		T->symclass = 0;
		return T;
	}
	struct symbol *S = lookup_symbol(str);
	if(!S){error("semantic error: unknown type [%s]\n",S);}
	T->name = S->username; //IR name here maybe?
	T->symclass = S;
	return T;
}

void padprint(int indent){
	while(indent--){
		printf(" ");
	}
}

//vector2_int IR_namecounts;
void IR_init(){
	namespace_semantic = vector2_ptr_char_new();
}
//char *stralloc(const char *str);

//===========
//  Ideally, push all IR stuff into a separate file,
//  once I figure out which parts are responsible for it
//===========

//it seems every call to IR_next_name has to be paired with ensuring
//the code gen knows what symbol we are talking about
int IR_name_in_use(vector2_ptr_char *namespace, const char *name){
	int I;
	for(I = 0; I < namespace->size; I++){
		const char *str = m(*namespace,get,I);
		if(!strcmp(str,name)){return 1;}
	}
	return 0;
}

char *IR_name_helper(vector2_ptr_char *namespace, const char* prefix, int allowExact, int allowInexact){
	char *buff = malloc(strlen(prefix)+20);
	sprintf(buff,"%s",prefix);
	if(allowExact && !IR_name_in_use(namespace,buff)){
		m(*namespace,push_back,buff);
		return buff;
	}
	if(!allowInexact){error("Error: Exact IR_name (%s) already taken ", prefix);}
	int I = 1;
	while(1){
		sprintf(buff,"%s_%d",prefix,I++);
		if(!IR_name_in_use(namespace,buff)){
			m(*namespace,push_back,buff);
			return buff;
		}
	}
}

char *IR_next_name(vector2_ptr_char *namespace, const char* prefix){return IR_name_helper(namespace,prefix,1,1);}
char *IR_exact_name(vector2_ptr_char *namespace, const char* prefix){return IR_name_helper(namespace,prefix,1,0);}
char *IR_inexact_name(vector2_ptr_char *namespace, const char* prefix){return IR_name_helper(namespace,prefix,0,1);}

void emit_code(const char *fmt, ...){
	if(!currentCodeSegment){error("emit_code: no code segment\n");}
	va_list ap;
	va_start(ap, fmt);
	char buff2[1];
	char *buff;
	int len = vsnprintf(buff2,1,fmt,ap);
	emit_code_pad = currentCodeSegment->indent;
	//int pad = emit_code_pad; //unused
	buff = malloc(sizeof(char)*(len+1+emit_code_pad));
	va_end(ap);
	va_start(ap, fmt);
	int i;
	for(i = 0; i < emit_code_pad; i++){
		sprintf(buff+i," ");
	}
	vsprintf(buff+emit_code_pad,fmt,ap);
	va_end(ap);
	//vector_push_back(&currentCodeSegment->commands,&buff);
	m(currentCodeSegment->commands,push_back,buff);
}

void emit_code_segment(struct code_segment *CS){
	if(!CS){error("emit_code_segment: no CS\n");}
	if(semantic_flatten){
		printf("emitting code segment %s\n",CS->name);
		int i;
		for(i = 0; i < CS->commands.size; i++){
			const char *cmd = m(CS->commands,get,i);
			printf("emitting command [%s]\n",cmd);
			emit_code("%s",cmd);
		}
		//m(CS_list,pop_back);
	}else{
		emit_code("INSERT %s", CS->name);
	}
}

void push_expr(const char *expr){
	//vector_push_back(&expr_stack, &expr);
	m(expr_stack,push_back,expr);
}

const char *pop_expr(){
	return m(expr_stack,pop_back);//vector_pop_back(&expr_stack);
}

void push_exprtype(struct type_name *T){
	m(exprtype_stack,push_back,T);
}

struct type_name *pop_exprtype(){
	return m(exprtype_stack,pop_back);
}

void semantic_init(){
	IR_init();
	expr_stack = vector2_ptr_char_here();
	exprtype_stack = vector2_ptr_type_name_here();
	CS_stack = vector2_ptr_code_segment_here();
	CS_list = vector2_ptr_code_segment_here();
	ST_stack = vector2_ptr_symbol_table_here();
	ST_list = vector2_ptr_symbol_table_here();
	//IR_names = vector2_ptr_char_here();
	//IR_namecounts = vector2_int_here();
	semantic_context_stack = vector2_ptr_semantic_context_kind_here();
	//vector_constructor(&expr_stack, sizeof(char*));
	//vector_constructor(&CS_stack, sizeof(struct code_segment));
	//vector_constructor(&ST_stack, sizeof(struct symbol_table));
	//vector_constructor(&IR_names, sizeof(char *));
	//vector_constructor(&IR_namecounts, sizeof(int*));
	//currentCodeSegment = 0;
	//currentSymbolTable = 0;
}


void print_semantic(){
	//print_code_segment(m(CS_list,get,0));
	//printf("\n===========================\nSymbol table:\n");
	//print_symbol_table(m(ST_list,get,0));


	print_code_segment(m(CS_list,get,0));
	printf("\n===========================\n");

	printf("printing symbol tables (%d):\n",ST_list.size);
	int i;
	for(i = 0; i < ST_list.size; i++){
		print_symbol_table(m(ST_list,get,i));
	}
	//printf("printing code segments (%d):\n",CS_list.size);
	//for(i = 0; i < CS_list.size; i++){
	//	print_code_segment(m(CS_list,get,i));
	//}
	
}

int getNumParameters(){
	int N = 0;
	int I;
	for(I = 0; I < currentSymbolTable->symbols.size; I++){
		ptr_symbol S = m(currentSymbolTable->symbols,get,I);
		if(S->type == SYMBOL_PARAM){N++;}
	}
	return N;
}

int getNumMembers(){
	int N = 0;
	int I;
	for(I = 0; I < currentSymbolTable->symbols.size; I++){
		ptr_symbol S = m(currentSymbolTable->symbols,get,I);
		if(S->type == SYMBOL_MEMBER){N++;}
	}
	return N;
}

int getNumVariables(){
	int N = 0;
	int I;
	for(I = 0; I < currentSymbolTable->symbols.size; I++){
		ptr_symbol S = m(currentSymbolTable->symbols,get,I);
		if(S->type == SYMBOL_VARIABLE){N++;}
	}
	return N;
}

int getTypeSize(ptr_type_name T){
	if(T->pointerlevel != 0){
		//is a pointer
		return 1;
	}
	if(T->args != 0){
		//is a function
		return 0;
	}
	if(T->symclass != 0){
		//is a class
		int size = 0;
		struct symbol_table *ST = T->symclass->symclass.scope;
		//size = ST->symbols.size; //lazy solution
		int i;
		for(i = 0; i < ST->symbols.size; i++){
			struct symbol *S = m(ST->symbols,get,i);
			if((S->type == SYMBOL_MEMBER)||(S->type == SYMBOL_VARIABLE)){
				size = size + getTypeSize(S->symvariable.type);
			}
		}
		return size? size : 1;
	}
	//is a primitive
	return 1;
}

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

void newerror(const char *file, int line, const char *func, const char *fmt, ...){
	//va_list ap;
	//va_start(ap, fmt);
	//char *buff;
	//int len = vsnprintf(buff,0,fmt,ap);
	//buff = malloc(sizeof(char)*(len+1));
	//va_end(ap);
	//va_start(ap, fmt);
	//vsnprintf(buff,len,fmt,ap);
	//va_end(ap);
	flushAllFiles();
	//fprintf(stderr, "\n");
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "%s", KRED);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "%s", KNRM);
	err("\n@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	va_end(ap);
	////err("error: printing allocations:\n");
	//printmemory(stderr);
	/////printallocs(stderr);
	//err("error: activating self-destruct\n");
	
	fprintf(stdout, "\n");
	va_start(ap, fmt);
	fprintf(stdout, "%s", KRED);
	vfprintf(stdout, fmt, ap);
	fprintf(stdout, "%s", KNRM);
	fprintf(stdout, "@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	va_end(ap);

	if (curNode) {
		const char* str = get_source_text2(curNode->token.pos);
		//fprintf(stderr, "when analyzing source text: [%s]\n", str);
		YYLTYPE pos = curNode->token.pos;
		point_out_error(pos.first_line, pos.first_column, pos.filename, "", strlen(str));
	}
	else {
		fprintf(stderr, "no source text\n");
	}

	//char buff[200];
	//sprintf(buff,"notepad++ %s -n%d &",file,line);
	//system(buff);
	exit(*(int*)0);
}

