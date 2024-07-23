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

#include "assert.h"
#include "ast_token.h" // to generate code for constructors
//defines

//generated
//definition_vector_of(ptr_file);
implementation_vector_of(val_handle);

//variables
int semantic_decl = 0;
int need_global_initializers = 0;
struct code_segment *init_CS  = 0;
struct code_segment *global_CS  = 0;
struct code_segment *global_init_CS = 0;
struct code_segment *global_deinit_CS = 0;
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

void semantic_fin_analyze(struct ast_node* /*node*/) {
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
	if (!strcmp(node->token.type, "var_decl_assign"))	{ 
		semantic_analyze_var_decl_assign(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_list"))		{ 
		semantic_analyze_expr_list	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_id"))		{ 
		semantic_analyze_expr_id		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_const"))	{ 
		semantic_analyze_expr_const	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_subexpr"))	{ 
		semantic_analyze_expr_subexpr	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_braced_list")){
		semantic_analyze_expr_braced_list(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_index"))	{ 
		semantic_analyze_expr_index	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_call"))		{ 
		semantic_analyze_expr_call	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_."))		{ 
		semantic_analyze_expr_dot		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_neg"))		{ 
		semantic_analyze_expr_neg		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_deref"))	{ 
		semantic_analyze_expr_deref	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_ref"))		{ 
		semantic_analyze_expr_ref		(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_="))		{ 
		semantic_analyze_expr_assign	(node, stg);	return 1; }
	if (!strcmp(node->token.type, "expr_cast"))		{
		semantic_analyze_expr_cast		(node, stg);	return 1; }
	if (semantic_dispatch_expr_op(node, stg)) { 
		return 1; }
	return 0;
}

int semantic_dispatch_if(struct ast_node* node, if_settings stg) {
	if (!strcmp(node->token.type, "if_then")) { semantic_analyze_if_then(node, stg); return 1; }
	return 0;
}

int semantic_dispatch_general(struct ast_node* node) {
	val_handle res = { .rv_type = E_DISCARD };
	val_handle this_handle = { .rv_type = E_ERROR };
	//if (symbolThis) {
	//	struct symbol* S = lookup_symbol("this");
	//	this_handle = (val_handle){ .val = S->IR_name, .T = S->symvariable.type, .rv_type = E_PTR };
	//}
	expr_settings stg = { .dest = res, .sem_this = this_handle };

	if (!strcmp(node->token.type, "program"))			{ 
		semantic_analyze_program(node);			return 1; }
	if (!strcmp(node->token.type, "decl_stmt_list"))	{ 
		semantic_analyze_decl_stmt_list(node);	return 1; }
	if (!strcmp(node->token.type, "decl_stmt"))			{ 
		semantic_analyze_decl_stmt(node);			return 1; }
	if (!strcmp(node->token.type, "func_def"))			{ 
		semantic_analyze_func_def(node);			return 1; }
	if (!strcmp(node->token.type, "var_decl_list"))		{ 
		semantic_analyze_var_decl_list(node);		return 1; }
	if (!strcmp(node->token.type, "var_decl"))			{ 
		semantic_analyze_var_decl(node);			return 1; }
	if (!strcmp(node->token.type, "var_decl_constructor")){ 
		semantic_analyze_var_decl_constructor(node);return 1; }
	if (!strcmp(node->token.type, "var_decl_assign"))	{ 
		semantic_analyze_var_decl_assign(node, stg);	return 1; }
	if (!strcmp(node->token.type, "var_decl_varargs"))	{ 
		semantic_analyze_var_decl_varargs(node);	return 1; }
	if (!strcmp(node->token.type, "stmt_list"))			{ 
		semantic_analyze_stmt_list(node);			return 1; }
	if (!strcmp(node->token.type, "stmt"))				{ 
		semantic_analyze_stmt(node);				return 1; }
	if (!strcmp(node->token.type, "imp_stmt"))			{ 
		semantic_analyze_imp_stmt(node);			return 1; }
	if (!strcmp(node->token.type, "if_block"))			{ 
		semantic_analyze_if_block(node);			return 1; }
	if (!strcmp(node->token.type, "while_loop"))		{ 
		semantic_analyze_while_loop(node);		return 1; }
	if (!strcmp(node->token.type, "for_loop"))			{ 
		semantic_analyze_for_loop(node);			return 1; }
	if (!strcmp(node->token.type, "class_def"))		{ 
		semantic_analyze_class_def(node);			return 1; }
	//it could be an expression (imperative statement)
	//where we ignore the result.
	if (semantic_dispatch_expr(node, stg)) {
		return 1;}
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

		emit_code_segment(global_init_CS);
		emit_code("CALL %s _main", sanitize_string(result)); //idk how to pass main's params yet.
		emit_code_segment(global_deinit_CS);
		emit_code("EXIT");

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

void add_symbol_this(){
	assert(symbolThis);
	//add param: this;
	struct type_name *T0 = semantic_get_type(symbolThis->username);
	struct type_name *T = type_name_new0();
	T->points_to = T0;
	//T->pointerlevel = 1;
	char *name = "this";
	struct symbol *S;
	S = symbol_new0();
	S->username = name;
	if(semantic_flatten){
		S->IR_name = IR_next_name(namespace_semantic,name);
	}else{
		S->IR_name = name;
	}
	S->type = SYMBOL_PARAM;
	S->symvariable.type = T;
	//S->symvariable.array = 0;
	//S->symvariable.arraysize = 0;
	S->store_adr = 0;
	S->symvariable.size = getTypeSize(T);
	S->storage = STORE_DATA_STACK;
	push_symbol(S);
}

void emit_member_initializers(struct symbol_table *table){
	int i = 0;
	for(i = 0; i < table->symbols.size; i++){
		struct symbol *S = m(table->symbols,get,i);
		if(S->type == SYMBOL_VARIABLE){
			emit_initializer(S);
		}
		else if(S->type == SYMBOL_MEMBER){
			emit_initializer(S);
		}
	}
}

void class_constructor_emit(struct symbol *S){
	assert(semantic_this);
	emit_code("/* default class constructor */ // semantic.c:265 ");
	emit_code("FUNCTION %s BEGIN", sanitize_string(S->IR_name));
	push_symbol_table();
	struct symbol_table* class_symtable = currentSymbolTable;
	new_symbol_table(0);
	add_symbol_this();
	
	push_code_segment();
	new_code_segment();
	S->symfunction.code = currentCodeSegment;
	S->symfunction.scope = currentSymbolTable;
		emit_code("FRAME ENTER");
		
		//emit_code("LABEL %s",S->IR_name);
		//emit_all_initializers();
		emit_all_declarations(DONT_USE_THIS); // need to declare "this"
		emit_member_initializers(class_symtable);
		emit_code("RET");
		//emit_all_undeclarations();
		emit_code("FRAME LEAVE");
	pop_code_segment();
	pop_symbol_table();
	emit_code_segment(S->symfunction.code);
	emit_code("FUNCTION %s END", sanitize_string(S->IR_name));
	emit_code("/* end */ // semantic.c:284 ");
}

void class_destructor_emit(struct symbol *S){
	emit_code("/* default class destructor */ // semantic.c:288 ");
	emit_code("FUNCTION %s BEGIN", sanitize_string(S->IR_name));
	push_symbol_table();
	new_symbol_table(0);
	push_code_segment();
	new_code_segment();
	S->symfunction.code = currentCodeSegment;
	S->symfunction.scope = currentSymbolTable;
		emit_code("FRAME ENTER");
		emit_all_declarations(DONT_USE_THIS);
		//emit_code("LABEL %s",S->IR_name);
		emit_all_deinitializers();
		emit_code("RET");
		emit_all_undeclarations();
		emit_code("FRAME LEAVE");
	pop_code_segment();
	pop_symbol_table();
	emit_code_segment(S->symfunction.code);	
	emit_code("FUNCTION %s END", sanitize_string(S->IR_name));
	emit_code("/* end */ // semantic.c:307 ");
}

void class_emit_start(){
	//emit struct code so member addresses are available
	emit_code("STRUCT %s BEGIN", sanitize_string(symbolThis->IR_name));
	//const char *smt = semantic_this;
	//semantic_this = 0;					// do not declare "this" among class members
	push_symbol_table();
	currentSymbolTable = symbolThis->symclass.scope;
	emit_all_declarations(DONT_USE_THIS);
	//pop_symbol_table();
	//semantic_this = smt;
	if(symbolThis->symclass.defaultConstructor){class_constructor_emit(symbolThis->symclass.defaultConstructor);}
	if(symbolThis->symclass.defaultDestructor){class_destructor_emit(symbolThis->symclass.defaultDestructor);}
	pop_symbol_table();
}

void class_emit_end(){
	emit_code("STRUCT %s END", sanitize_string(symbolThis->IR_name));
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
		struct type_name *T = semantic_get_type("void");
		S->symfunction.returntype = T;
		/// make a signature
			struct type_name *signature = type_name_new0();//malloc(sizeof(struct type_name));
			signature->name = 0;
			signature->symclass = 0;
			signature->args = vector2_ptr_type_name_new();
			m((*(signature->args)), push_back, T);
		S->symfunction.signature = signature;

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
		struct type_name *T = semantic_get_type("void");
		S->symfunction.returntype = T;
		/// make a signature
			struct type_name *signature = type_name_new0();//malloc(sizeof(struct type_name));
			signature->name = 0;
			signature->symclass = 0;
			signature->args = vector2_ptr_type_name_new();
			m((*(signature->args)), push_back, T); // first argument of a signature is the return type
		S->symfunction.signature = signature;
		
		S->symfunction.code = 0;
		S->symfunction.scope = 0;
		symbolThis->symclass.defaultDestructor = S;
	}else{symbolThis->symclass.defaultDestructor = 0;}
}

void emit_all_decl_helper(){
	if(symbolThis){
		emit_code("USING %s", sanitize_string(symbolThis->IR_name));
	}
	int i = 0;
	for(i = 0; i < currentSymbolTable->symbols.size; i++){
		struct symbol *S = m(currentSymbolTable->symbols,get,i);
		push_code_segment();
		if(S->global){
			currentCodeSegment = init_CS;
		}
		if(S->type == SYMBOL_PARAM || S->type == SYMBOL_VARIABLE || S->type == SYMBOL_MEMBER){
			vector2_char vstr = vector2_char_here();
			const char* typestr = 0;
			if (S->type == SYMBOL_PARAM) {typestr = "ARG";}
			if (S->type == SYMBOL_VARIABLE) {typestr = "VAR";}
			if (S->type == SYMBOL_MEMBER) {typestr = "MEMBER";}
			vec_printf(&vstr, "SYMBOL %s %s",
				sanitize_string(S->IR_name),
				sanitize_string(typestr));
			
				//calc size of var
			if ((S->type == SYMBOL_VARIABLE) 
			 || (S->type == SYMBOL_MEMBER)
			 || (S->type == SYMBOL_PARAM)) {
				int size = S->symvariable.size;
				if (size != 1) {
					vec_printf(&vstr, " SIZE %d", size);
				}
			}

			if (S->symvariable.type->is_array){//S->symvariable.array) {
				vec_printf(&vstr, " ARRAY %d", S->symvariable.type->arraysize);
			}
			emit_code("%s", vstr.data);
			//if(S->symvariable.array){
			//	if(S->type == SYMBOL_PARAM){
			//		emit_code("SYMBOL %s ARG ARRAY %d", 
			//			sanitize_string(S->IR_name), 
			//			S->symvariable.arraysize);
			//	}
			//	if(S->type == SYMBOL_VARIABLE){
			//		emit_code("SYMBOL %s VAR ARRAY %d", 
			//			sanitize_string(S->IR_name), 
			//			S->symvariable.arraysize);
			//	}
			//}else{
			//	if(S->type == SYMBOL_PARAM){
			//		emit_code("SYMBOL %s ARG", 
			//			sanitize_string(S->IR_name));
			//	}//emit_code("DEBUG BEGIN VAR %s %s",S->IR_name,S->username);}
			//	if(S->type == SYMBOL_VARIABLE){
			//		emit_code("SYMBOL %s VAR", 
			//			sanitize_string(S->IR_name));
			//	}//emit_code("DEBUG BEGIN VAR %s %s",S->IR_name,S->username);}
			//}
		}else{
			//if(S->type == SYMBOL_FUNCTION){emit_code("SYMBOL %s FUNC",S->IR_name);} nah, they're both define-anywhere
			//if(S->type == SYMBOL_CLASS){emit_code("SYMBOL %s STRUCT",S->IR_name);}
		}
		pop_code_segment();
	}
}

void emit_all_declarations(int use_this){
	emit_all_decl_helper();
	if(use_this){//if(semantic_this){
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
		if(S->type == SYMBOL_VARIABLE){
			emit_initializer(S);
		}
		else if(S->type == SYMBOL_MEMBER){
			emit_initializer(S);
		}
	}
}

struct ast_node *make_ast_ID(const char* name){
	struct ast_node *node = ast_node_new( // x...
		ast_token_here("expr_id",	0,name,nullPos()), 
		vector2_ptr_ast_node_here_from_list(1, 
			ast_token_here("ID", 0, name,nullPos())
		)
	);
	return node;
}

struct ast_node *make_ast_op(const char* name, struct ast_node *arg1, struct ast_node *arg2){
	struct ast_node *node = ast_node_new(
		ast_token_here(name, 0,NULL,nullPos()), 
		vector2_ptr_ast_node_here_from_list(2, 
			arg1, 
			arg2
		)
	);
	return node;
}

void emit_initializer(struct symbol *S){
	struct type_name *T = S->symvariable.type;
	/*
	if(S->storage == STORE_DATA_STACK){
		emit_code("ALLOC STACK %s %d",S->IR_name,S->symvariable.size); //getTypeSize(S->symvariable.type));
	}
	*/
	if(T->symclass){
		emit_code("/* initialize %s */", 
			sanitize_string(S->username));
		//emit_code("/* construct %s */",S->username);
		struct symbol *S2 = T->symclass;
		push_symbol_table();
		currentSymbolTable = S2->symclass.scope;
		struct symbol *S3 = lookup_symbol("constructor");
		const char *exprResult = IR_next_name(namespace_semantic,"temp");
		//emit_code("CALL %s %s %s",exprResult,S3->IR_name,S->IR_name);

		if(strcmp(S->username, "derp4")==0){
			printf("debug breakpoint");
		}

		emit_code("CALL %s %s &%s", 
			sanitize_string(exprResult), 
			sanitize_string(S3->IR_name), 
			sanitize_string(S->IR_name));
		pop_symbol_table();
	}else{
		//primitive type
		//emit_code("%s = 0",S->IR_name);
		//emit_code("MOV %s 999",S->IR_name); //magic number to tell when a var is initialized
		//do not initialize primivites
		if(semantic_this && (S->type == SYMBOL_MEMBER)){
			if(S->init_expr){
				// Generate the member initialization code like x = y;
				emit_code("COMMENT SOURCE \"this.%s = member initializer;\" // semantic.c:543", S->username);
				
				//struct ast_node *id_this = make_ast_ID("this");
				struct ast_node *lhs = make_ast_ID(S->username);
				//struct ast_node *dot = make_ast_op("expr_.", id_this, lhs);
				struct ast_node *rhs = S->init_expr;
				//struct ast_node *assignment = make_ast_op("expr_=", dot, rhs);
				struct ast_node *assignment = make_ast_op("expr_=", lhs, rhs);
				
				semantic_general_analyze(assignment);
							
				//val_handle res = { .rv_type = E_DISCARD };
				//val_handle this_handle = symbolThis; //{ .rv_type = E_ERROR };
				//expr_settings stg = { .dest = res, .sem_this = this_handle };
				/// so apparrently sem_this appears only when .dot expression is analyzed,
				/// and therein x.y sets sem_this to x once output_res emits X.
				/// ... it's easier to just use this.x in AST rather than monkey wrench
				/// the existing behavior
			}
		}
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
		emit_code("/* destroy %s */", 
			sanitize_string(S->username));
		struct symbol *S2 = T->symclass;
		push_symbol_table();
		currentSymbolTable = S2->symclass.scope;
		struct symbol *S3 = lookup_symbol("destructor");
		const char *exprResult = IR_next_name(namespace_semantic,"temp");
		emit_code("CALL %s %s &%s", 
			sanitize_string(exprResult), 
			sanitize_string(S3->IR_name), 
			sanitize_string(S->IR_name));
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
		if(need_global_initializers){
			need_global_initializers = 0; /// we only run this once
			//switch code segments so we can layout the overall file
			push_code_segment();
				new_code_segment();
				global_init_CS = currentCodeSegment;
				emit_all_declarations(0);
				emit_all_initializers();
			pop_code_segment();
			
			push_code_segment();
				new_code_segment();
				global_deinit_CS = currentCodeSegment;
				emit_all_deinitializers();
				emit_all_undeclarations();
			pop_code_segment();

			/// now do the 99% of the program code that isn't global vars.
			semantic_general_analyze(N);
		}else{
			emit_all_declarations((semantic_this != 0));
			emit_all_initializers();
			semantic_general_analyze(N);
			emit_all_deinitializers();
			emit_all_undeclarations();
		}
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
		(C == '\'')||
		(C == '\\')
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
		assert(buffi < (len+1));
	}
	buff[buffi] = 0;
	printf("string [%s] escaped to [%s]\n",str,buff);
	return buff;
}

char *unescape_string(const char *str){
	int i = 0;
	//int len = 0; //unused
	int len = strlen(str);
	char *buff = malloc(len+1);
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
		assert(buffi < (len+1));
		buff[buffi++] = C;
	}
	assert(buffi < (len+1));
	buff[buffi] = 0;
	printf("string [%s] unescaped to [%s]\n",str,buff);
	return buff;
}

struct type_name *parseTypename(struct ast_node *N){
	struct type_name *T = semantic_get_type(N->token.value);
	//struct ast_node *ptr_stars = m(N->children,get,0);//ast_get_node(m(N->children,get,0));
	struct ast_node *ptr_stars = m(N->children,get,1); //22.03.2022 ptr_stars were not being detected
	int pointerlevel = ptr_stars->children.size;
	for(int i = 0; i < pointerlevel; i++){
		struct type_name *T2 = type_name_new0();
		T2->points_to = T;
		T = T2; /// lol, memleak
	}
	return T;
}

struct type_name *semantic_get_type(const char *str){
	struct type_name *T = type_name_new0();//malloc(sizeof(struct type_name));
	T->args = 0;
	//T->pointerlevel = 0;
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

char* IR_name_helper(vector2_ptr_char* namespace, const char* prefix, int allowExact, int allowInexact) {
	vector2_char vstr = vector2_char_here();
	vec_printf(&vstr, "%s", sanitize_string(prefix));
	if (allowExact && !IR_name_in_use(namespace, vstr.data)){//buff)) {
		m(*namespace, push_back, vstr.data);//buff);
		return stralloc(vstr.data);//return buff;
	}
	if (!allowInexact) { error("Error: Exact IR_name (%s) already taken ", prefix); }
	int I = 1;
	while (1) {
		m(vstr, clear);
		vec_printf(&vstr, "%s_%d", prefix, I++);
		if (!IR_name_in_use(namespace, vstr.data)){//buff)) {
			m(*namespace, push_back, vstr.data);//buff);
			return stralloc(vstr.data);//return buff;
		}
	}
}



char *IR_next_name(vector2_ptr_char *namespace, const char* prefix){return IR_name_helper(namespace,prefix,1,1);}
char *IR_exact_name(vector2_ptr_char *namespace, const char* prefix){return IR_name_helper(namespace,prefix,1,0);}
char *IR_inexact_name(vector2_ptr_char *namespace, const char* prefix){return IR_name_helper(namespace,prefix,0,1);}

void emit_code(const char* fmt, ...) {
	if (!currentCodeSegment) { error("emit_code: no code segment\n"); }
	va_list ap;
	
	emit_code_pad = currentCodeSegment->indent;
	//int pad = emit_code_pad; //unused
	//buff = malloc(sizeof(char) * (len + 1 + emit_code_pad));
	//va_end(ap);
	va_start(ap, fmt);
	//1. pad the string
	vector2_char vstr = vector2_char_here();
	const char* pad80blanks = "                                                                                ";
	vec_nprintf(&vstr, emit_code_pad, pad80blanks);

	vec_vnprintf(&vstr, -1, fmt, ap);
	va_end(ap);
	//vector_push_back(&currentCodeSegment->commands,&buff);
	m(currentCodeSegment->commands, push_back, stralloc(vstr.data));//buff);
}

void emit_code_segment(struct code_segment *CS){
	if(!CS){error("emit_code_segment: no CS\n");}
	if(semantic_flatten){
		printf("emitting code segment %s\n",CS->name);
		int i;
		for(i = 0; i < CS->commands.size; i++){
			const char *cmd = m(CS->commands,get,i);
			printf("emitting command [%s]\n",cmd);
			emit_code("%s", sanitize_string(cmd));
		}
		//m(CS_list,pop_back);
	}else{
		emit_code("INSERT %s", sanitize_string(CS->name));
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
	semantic_context_stack = vector2_ptr_semantic_context_kind_here();
}


void print_semantic(){
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
	if(T->points_to != 0){
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

extern const char* path_out_IR;
extern int doing_semantic;

void maybePrintSemantic() {
	if (doing_semantic) {
		fprintf(stderr, "printing IR so far...");
		semantic_finalize();
		freopen(/*"aout_IR.txt"*/path_out_IR, "w", stdout);
		setbuf(stdout, NULL);
		print_semantic();
		fprintf(stderr, "\tdone\n");
	}
}

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
extern int compiler_restart_enabled;

int processing_error = 0;

void newerror(const char *file, int line, const char *func, const char *fmt, ...){
	if(processing_error){
		/// SUPER CRASH RIGHT NOW
		exit(*(int*)0);
	}
	processing_error = 1;
	flushAllFiles();
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "%s", KRED);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "%s", KNRM);
	err("\n@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	va_end(ap);
	
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

	maybePrintSemantic();

	processing_error = 0;
	if (compiler_restart_enabled) {
		exit(1);
	}
	else {
		exit(*(int*)0);
	}
}

