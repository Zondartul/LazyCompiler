#include "semantic.h"
#include "stdlib.h"
#include "globals.h"
#include "string.h"
implementation_vector_of(ptr_symbol);
implementation_vector_of(ptr_symbol_table);
implementation_vector_of(ptr_code_segment);
implementation_vector_of(ptr_char);
int semantic_decl;
struct symbol_table *currentSymbolTable;
vector2_ptr_symbol_table ST_stack;
vector2_ptr_code_segment CS_stack;
vector2_ptr_char expr_stack;
// vector ST_stack;
// vector CS_stack;
// vector expr_stack;
//what is the context of our parsing
enum{SEMANTIC_NORMAL, SEMANTIC_PARAM} semantic_context;
//const char *exprResult;
void semantic_analyze(struct ast_node *node){
	printmemory(stdout);
	int i;
	if(!strcmp(node->token.type, "program")){
		printf("got program\n");
		currentSymbolTable = new_symbol_table();
		semantic_analyze(ast_get_child(node, 0));
		printf("semantic: done\n");
		print_symbol_table(currentSymbolTable);
		return;
	}
	if(!strcmp(node->token.type, "decl_stmt_list")){
		printf("got decl_stmt_list\n");
		semantic_decl = 1;
		for(i = 0; i < node->children.size; i++){
			semantic_analyze(ast_get_child(node, i));
		}
		semantic_decl = 0;
		for(i = 0; i < node->children.size; i++){
			semantic_analyze(ast_get_child(node, i));
		}
		return;
	}
	if(!strcmp(node->token.type, "decl_stmt")){
		//if(!semantic_decl){return;}
		printf("got decl_stmt\n");
		semantic_analyze(ast_get_child(node,0));
		return;
	}
	if(!strcmp(node->token.type, "func_def")){
		//if(!semantic_decl){return;}
		printf("got func_def\n");
		struct type_name *T = semantic_get_type(ast_get_child(node,0)->token.value);
		const char *name = node->token.value;
		if(semantic_decl){
			struct symbol *S = new_symbol();
			S->type = SYMBOL_FUNCTION;
			S->name = name;
			//get arguments
			struct symbol_table *ST = new_symbol_table();
			S->symfunction.type = T;
			S->symfunction.scope = ST;
			struct symbol_table *oldST = currentSymbolTable;
			currentSymbolTable = ST;
			semantic_context = SEMANTIC_PARAM;
				//use new separate symbol table
			semantic_analyze(ast_get_child(node,1));
				//join symbol table to definition
			semantic_context = SEMANTIC_NORMAL;
			//get code
				//use new code segment
			semantic_analyze(ast_get_child(node,2));
			//imperative pass: semantic_analyze(ast_get_child(node,2));
				//join code segment to definition
			currentSymbolTable = oldST;
			push_symbol(S);
		}else{
			struct symbol *S = lookup_symbol(name);
			push_symbol_table(S->symfunction.scope);
			S->symfunction.code = new_code_segment_make_current();
			semantic_analyze(ast_get_child(node,2));
			pop_code_segment();
		}
		return;
	}
	if(!strcmp(node->token.type, "var_decl_list")){
		printf("got var_decl_list\n");
		for(i = 0; i < node->children.size; i++){
			semantic_analyze(ast_get_child(node, i));
		}
		return;
	}
	if(!strcmp(node->token.type, "var_decl")){
		if(!semantic_decl){return;}
		printf("got var_decl\n");
		struct type_name *T = semantic_get_type(ast_get_child(node,0)->token.value);
		char *name = node->token.value;
		struct symbol *S = new_symbol();
		if(semantic_context == SEMANTIC_PARAM){
			S->type = SYMBOL_PARAM;
			S->name = name;
			S->symparameter.type = T;
			S->symparameter.pos = currentSymbolTable->symbols.size;
		}else{
			S->type = SYMBOL_VARIABLE;
			S->name = name;
			S->symvariable.type = T;
		}
		push_symbol(S);
		//put into symbol table
		return;
	}
	if(!strcmp(node->token.type, "var_decl_assign")){
		if(!semantic_decl){return;}
		printf("got var_decl\n");
		struct type_name *T = semantic_get_type(ast_get_child(node,0)->token.value);
		char *name = node->token.value;
		struct symbol *S = new_symbol();
		if(semantic_context == SEMANTIC_PARAM){
			S->type = SYMBOL_PARAM;
			S->name = name;
			S->symparameter.type = T;
			S->symparameter.pos = currentSymbolTable->symbols.size;
		}else{
			S->type = SYMBOL_VARIABLE;
			S->name = name;
			S->symvariable.type = T;
		}
		push_symbol(S);
		//put into symbol table
		//will deal with assignment later; ignore for now.
		//semantic_analyze(ast_get_child(node,1));
		//put expression as "initial value"?
		return;
	}
	if(!strcmp(node->token.type, "stmt_list")){
		printf("got stmt_list\n");
		for(i = 0; i < node->children.size; i++){
			semantic_analyze(ast_get_child(node, i));
		}
		return;
	}
	if(!strcmp(node->token.type, "stmt")){
		printf("got stmt\n");
		semantic_analyze(ast_get_child(node,0));
		return;
	}
	if(!strcmp(node->token.type, "imp_stmt")){
		if(semantic_decl){return;}
		//imperative pass: 
		printf("got imp_stmt\n");
		switch(node->token.production){
			case(0)://if_block
			case(1)://while_loop
			case(2)://expr
				semantic_analyze(ast_get_child(node,0));
				break;
			case(3)://return
				emit_code("RETURN");
				break;
			case(4)://return expr
				semantic_analyze(ast_get_child(node,0));
				emit_code("RETURN %s",pop_expr());
				break;
		}
		return;
	}
	if(!strcmp(node->token.type, "if_block")){
		//if(semantic_decl){return;}
		//imperative pass: 
		printf("got if_block\n");
		switch(node->token.production){
			case(0)://if_then END
				semantic_analyze(ast_get_child(node,0));
				break;
			case(1)://if_then ELSE stmt_list END
				semantic_analyze(ast_get_child(node,0));
				semantic_analyze(ast_get_child(node,1));
				break;
		}
		return;
	}
	if(!strcmp(node->token.type, "if_then")){
		if(semantic_decl){return;}
		//imperative pass: 
		printf("got if_then\n");
		switch(node->token.production){
			case(0): //IF ( expr ) THEN stmt_list
				//push new symbol table
				semantic_analyze(ast_get_child(node,0));
				const char *nextLabel = IR_next_name("l");
				emit_code("IFNOT %s %s", pop_expr(), nextLabel); 
				push_symbol_table(new_symbol_table_make_current());
				push_code_segment(new_code_segment_make_current());
				semantic_decl = 1;
				semantic_analyze(ast_get_child(node,1));
				semantic_decl = 0;
				semantic_analyze(ast_get_child(node,1));
				pop_code_segment();
				pop_symbol_table();
				emit_code("LABEL %s", nextLabel);
				//pop symbol table
				break;
			case(1): //if_then ELSEIF ( expr ) THEN stmt_list
				semantic_analyze(ast_get_child(node,0));
				const char *label1 = IR_next_name("l");
				const char *label2 = IR_next_name("l");
				emit_code("IFNOT %s %s", pop_expr(), label1);
				semantic_analyze(ast_get_child(node,1));
				emit_code("GOTO %s", label2);
				emit_code("LABEL %s", label1);
				push_symbol_table(new_symbol_table_make_current());
				push_code_segment(new_code_segment_make_current());
				semantic_analyze(ast_get_child(node,2));
				pop_code_segment();
				pop_symbol_table();
				emit_code("LABEL %s", label2);
				break;
		};
		return;
	}
	if(!strcmp(node->token.type, "while_loop")){
		if(semantic_decl){return;}
		//imperative pass: 
		printf("got while_loop\n");
		semantic_analyze(ast_get_child(node,0));
		const char *label1 = IR_next_name("l");
		const char *label2 = IR_next_name("l");
		emit_code("LABEL %s",label1);
		emit_code("IFNOT %s %s",pop_expr(), label2);
		//double-push
		push_symbol_table(new_symbol_table_make_current());
		push_code_segment(new_code_segment_make_current());
		semantic_analyze(ast_get_child(node,1));
		pop_code_segment();
		pop_symbol_table();
		emit_code("GOTO %s",label1);
		emit_code("LABEL %s",label2);
		return;
	}
	if(!strcmp(node->token.type, "class_def")){
		if(!semantic_decl){return;}
		printf("got class_def\n");
		char *name = node->token.value;
		struct symbol *S = new_symbol();
		S->name = name;
		S->type = SYMBOL_CLASS;
		//make new symbol table
		struct symbol_table *ST = new_symbol_table();
		S->symclass.scope = ST;
		struct symbol_table *oldST = currentSymbolTable;
		currentSymbolTable = ST;
		semantic_analyze(ast_get_child(node,0));
		currentSymbolTable = oldST;
		//embed into existing symbol table
		push_symbol(S);
		return;
	}
	if(!strcmp(node->token.type, "expr_list")){
		if(semantic_decl){return;}
		//imperative pass: 
		printf("got expr_list\n");
		for(i = 0; i < node->children.size; i++){
			semantic_analyze(ast_get_child(node, i));
			//give name to new expression result?
			//put it in stack?
		}
		return;
	}
	if(!strcmp(node->token.type, "expr_id")){
		printf("got expr_id = [%s]\n",node->token.value);
		char *name = stralloc(node->token.value);
		//exprResult = name;
		push_expr(name);
		//put variable <name> on the stack
		return;
	}
	if(!strcmp(node->token.type, "expr_const")){
		printf("got expr_const\n");
		char *exprResult = IR_next_name("r");
		emit_code("MOV ",exprResult, node->token.value);
		push_expr(exprResult);
		//figure out what kind of const this is?
		return;
	}
	if(!strcmp(node->token.type, "expr_subexpr")){
		printf("got expr_subexpr\n");
		semantic_analyze(ast_get_child(node,0));
		return;
	}
	if(!strcmp(node->token.type, "expr_call")){
		printf("got expr_call\n");
		semantic_analyze(ast_get_child(node,0));
		return;
	}
	if(!strcmp(node->token.type, "expr_.")){
		printf("got expr_.\n");
		semantic_analyze(ast_get_child(node,0));
		//what if $0 isn't an ID? need to figure out
		//expression type... also instance... fuck, just redo this.
		struct ast_node *N = ast_get_child(node,0);
		struct symbol *S = lookup_symbol(N->token.value);
		if(S->type != SYMBOL_CLASS){
			error("semantic error: \"%s\" is not a class\n",N->token.value);
		}
		push_symbol_table(S->symclass.scope);
		semantic_analyze(ast_get_child(node,1));
		pop_symbol_table();
		//emit code: member access
		return;
	}
	if(!strcmp(node->token.type, "expr_^")){
		printf("got expr_^\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		const char *result2 = pop_expr();
		const char *result1 = pop_expr();
		const char *exprResult = IR_next_name("r");
		emit_code("EXPONENT %s %s %s",exprResult, result1, result2);
		push_expr(exprResult);
		//emit code: exp
		return;
	}
	if(!strcmp(node->token.type, "expr_/")){
		printf("got expr_/\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		const char *result2 = pop_expr();
		const char *result1 = pop_expr();
		const char *exprResult = IR_next_name("r");
		emit_code("DIVIDE %s %s %s",exprResult, result1, result2);
		push_expr(exprResult);
		//emit code: div
		return;
	}
	if(!strcmp(node->token.type, "expr_*")){
		printf("got expr_*\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		const char *result2 = pop_expr();
		const char *result1 = pop_expr();
		const char *exprResult = IR_next_name("r");
		emit_code("MULTIPLY %s %s %s",exprResult, result1, result2);
		push_expr(exprResult);
		//emit code: mul
		return;
	}
	if(!strcmp(node->token.type, "expr_-")){
		printf("got expr_-\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		const char *result2 = pop_expr();
		const char *result1 = pop_expr();
		const char *exprResult = IR_next_name("r");
		emit_code("SUBTRACT %s %s %s",exprResult, result1, result2);
		push_expr(exprResult);
		//emit code: sub
		return;
	}
	if(!strcmp(node->token.type, "expr_+")){
		printf("got expr_+\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		const char *result2 = pop_expr();
		const char *result1 = pop_expr();
		const char *exprResult = IR_next_name("r");
		emit_code("ADD %s %s %s",exprResult, result1, result2);
		push_expr(exprResult);
		//emit code: add
		return;
	}
	if(!strcmp(node->token.type, "expr_=")){
		printf("got expr_=\n");
		semantic_analyze(ast_get_child(node,0));
		semantic_analyze(ast_get_child(node,1));
		const char *result2 = pop_expr();
		const char *result1 = pop_expr();
		emit_code("SET %s %s", result1, result2);
		push_expr(result1);
		//emit code: assign
		return;
	}
	//unknown node type
	error("semantic: unknown node type: [%s]\n",node->token.type);
	return;
}

struct symbol_table *new_symbol_table(){
	struct symbol_table *T = malloc(sizeof(struct symbol_table));
	T->symbols = vector2_ptr_symbol_here();
	//vector_constructor(&T->symbols,sizeof(struct symbol));
	T->parent = currentSymbolTable;
}

struct symbol_table *new_symbol_table_make_current(){
	struct symbol_table *ST = new_symbol_table();
	push_symbol_table(ST);
	return ST;
}
void push_symbol_table(struct symbol_table *ST){
	//vector_push_back(&ST_stack, currentSymbolTable);
	m(ST_stack,push_back,currentSymbolTable);
	currentSymbolTable = ST;
}
void pop_symbol_table(){
	currentSymbolTable = m(ST_stack,pop_back);//vector_pop_back(&ST_stack);
}

struct symbol *lookup_symbol_helper(const char *name, struct symbol_table *T){
	int i;
	for(i = 0; i < T->symbols.size; i++){
		struct symbol *S;
		//S = vector_get_reference(&T->symbols, i);
		S = m(T->symbols,get,i);
		if(!strcmp(S->name,name)){
			return S;
		}
	}
	if(T->parent){
		return lookup_symbol_helper(name, T->parent);
	}else{
		return 0;
	}
}

struct code_segment *new_code_segment(){
	struct code_segment *CS = malloc(sizeof(struct code_segment));
	CS->scope = currentSymbolTable;
	//vector_constructor(&CS->commands,sizeof(char*));
	CS->commands = vector2_ptr_char_here();
	return CS;
}

struct code_segment *new_code_segment_make_current(){
	struct code_segment *CS = new_code_segment();
	push_code_segment(CS);
	return CS;
}

void push_code_segment(struct code_segment *CS){
	//vector_push_back(&CS_stack, currentCodeSegment);
	m(CS_stack,push_back,currentCodeSegment);
	currentCodeSegment = CS;
}

void pop_code_segment(){
	currentCodeSegment = m(CS_stack,pop_back);//vector_pop_back(&CS_stack);
}
struct symbol *new_symbol(){
	struct symbol *S = malloc(sizeof(struct symbol));
	S->type = SYMBOL_ERROR;
	S->name = 0;
}
struct symbol *lookup_symbol(const char *name){
	return lookup_symbol_helper(name, currentSymbolTable);
}

void push_symbol(struct symbol *S){
	//vector_push_back(&currentSymbolTable->symbols, &S);
	m(currentSymbolTable->symbols,push_back,S);
}

struct type_name *semantic_get_type(const char *str){
	struct type_name *T = malloc(sizeof(struct type_name));
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
	T->name = S->name;
	T->symclass = S;
	return T;
}
void padprint(int indent){
	while(indent--){
		printf(" ");
	}
}
void print_symbol_table_helper(struct symbol_table *T, int indent){
	padprint(indent); printf("Symbol table:\n");
	int i;
	for(i = 0; i < T->symbols.size; i++){
		struct symbol *S = m(T->symbols,get,i);//vector_get_reference(&T->symbols,i);
		padprint(indent); printf("symbol [%s]: ",S->name);
		switch(S->type){
			case(SYMBOL_VARIABLE):
				printf("variable\n");
				padprint(indent); printf(" type = %s\n", S->symvariable.type->name);
			break;
			case(SYMBOL_PARAM):
				printf("parameter\n");
				padprint(indent); printf(" type = %s\n", S->symparameter.type->name);
				padprint(indent); printf(" pos = %d\n", S->symparameter.pos);
			break;
			case(SYMBOL_FUNCTION):
				printf("function\n");
				padprint(indent); printf(" type = %s\n", S->symfunction.type->name);
				padprint(indent); printf(" scope = %p\n", S->symfunction.scope);
				print_symbol_table_helper(S->symfunction.scope, indent+1);
			break;
			case(SYMBOL_CLASS):
				printf("class\n");
				print_symbol_table_helper(S->symclass.scope,indent+1);
			break;
		};
	}
	padprint(indent); printf("End Symbol table\n");
}
void print_symbol_table(struct symbol_table *T){
	print_symbol_table_helper(T,0);
}

vector2_ptr_char IR_names;
vector2_int IR_namecounts;
//vector IR_names;
//vector IR_namecounts;
void IR_init(){
	IR_names = vector2_ptr_char_here();
	IR_namecounts = vector2_int_here();
	//vector_constructor(&IR_names,sizeof(char*));
	//vector_constructor(&IR_namecounts,sizeof(int));
}
char *stralloc(const char *str);
char *IR_next_name(const char *prefix){
	int i;
	int count;
	for(i = 0; i < IR_names.size; i++){
		//char *str = *(char**)vector_get_copy(&IR_names,i);
		const char *str = m(IR_names,get,i);
		if(!strcmp(str,prefix)){
			//int *count = (int*)vector_get_reference(&IR_namecounts,i);
			goto IR_next_name_found;
		}
	}	
	const char *newprefix = stralloc(prefix);
	//int z = 0;
	m(IR_names,push_back,newprefix);
	m(IR_namecounts,push_back,0);
	//vector_push_back(&IR_names,&newprefix);
	//vector_push_back(&IR_namecounts,&z);
	//count = (int*)vector_get_reference(&IR_namecounts,IR_namecounts.size-1);
IR_next_name_found:
	count = m(IR_namecounts,get,i);
	count++;
	m(IR_namecounts,set,count,i);
	//*count = *count+1;
	char *buff;
	int len = snprintf(buff, 0, "%s%d",prefix,count);
	buff = malloc(sizeof(char)*(len+1));
	sprintf(buff, "%s%d",prefix,count);
	printf("IR_next_name: returning [%s]\n",buff);
	return buff;
}
void emit_code(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	char *buff;
	int len = vsnprintf(buff,0,fmt,ap);
	buff = malloc(sizeof(char)*(len+1));
	va_end(ap);
	va_start(ap, fmt);
	vsnprintf(buff,len,fmt,ap);
	va_end(ap);
	if(!currentCodeSegment){error("emit_code: no code segment\n");}
	//vector_push_back(&currentCodeSegment->commands,&buff);
	m(currentCodeSegment->commands,push_back,buff);
}
void push_expr(const char *expr){
	//vector_push_back(&expr_stack, &expr);
	m(expr_stack,push_back,expr);
}
const char *pop_expr(){
	return m(expr_stack,pop_back);//vector_pop_back(&expr_stack);
}
void semantic_init(){
	IR_init();
	expr_stack = vector2_ptr_char_here();
	ST_stack = vector2_ptr_symbol_table_here();
	IR_names = vector2_ptr_char_here();
	IR_namecounts = vector2_int_here();
	//vector_constructor(&expr_stack, sizeof(char*));
	//vector_constructor(&CS_stack, sizeof(struct code_segment));
	//vector_constructor(&ST_stack, sizeof(struct symbol_table));
	//vector_constructor(&IR_names, sizeof(char *));
	//vector_constructor(&IR_namecounts, sizeof(int*));
	//currentCodeSegment = 0;
	//currentSymbolTable = 0;
}
#define MAX_ALLOCS 2000
void *allocs[MAX_ALLOCS];
int allocsizes[MAX_ALLOCS];
const char *allocfiles[MAX_ALLOCS];
const char *allocfuncs[MAX_ALLOCS];
int alloclines[MAX_ALLOCS];
int memused;
int numallocs;
#define BUMP_MAX 1000000
char bump_buff[BUMP_MAX];
int bump_free;
void initallocs(){
	memused = 0;
	numallocs = 0;
	int i;
	for(i = 0; i < 2000; i++){
		allocs[i] = 0;
	}
	for(i = 0; i < 2000; i++){
		allocsizes[i] = 0;
	}
	for(i = 0; i < 2000; i++){
		allocfiles[i] = 0;
	}
	for(i = 0; i < 2000; i++){
		allocfuncs[i] = 0;
	}
	for(i = 0; i < 2000; i++){
		alloclines[i] = 0;
	}
	bump_free = 0;
}
void *bumpalloc(size_t size){
	if((bump_free+size+2*sizeof(int))<BUMP_MAX){		
		void *result = (void*)(bump_buff+bump_free+2*sizeof(int));
		bump_free+=size+2*sizeof(int);
		((int*)result)[-1]=size;
		printf("bump %d bytes: %d bytes left\n", size, BUMP_MAX-bump_free);
		return result;
	}else{
		return 0;
	}
}
void *bumprlloc(void *ptr, size_t size){
	if(ptr){
		void *result = bumpalloc(size);
		memcpy(result,ptr,((int*)ptr)[-1]);
		return result;
	}else{return bumpalloc(size);}
}
void *newmalloc(size_t size, void *(oldmalloc)(size_t size), const char* file, const char* func, int line){
	#ifdef BUMP_ALLOCATOR
	void *m = bumpalloc(size);
	#else
	void *m = oldmalloc(size);
	#endif
	if(!m){
		err("newmalloc: malloc(%d) failed\n",size);
		err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	}
	else{
		allocs[numallocs] = m;
		allocsizes[numallocs] = size;
		allocfiles[numallocs] = file;
		allocfuncs[numallocs] = func;
		alloclines[numallocs] = line;
		numallocs++;
		memused += size;
	}
	return m;
}
void *newrealloc(void *ptr, size_t size, void *(oldrealloc)(void *ptr, size_t size), const char* file, const char* func, int line){
	int i = 0;
	int found = 0;
	if(ptr){
		for(i = 0; i < 2000; i++){
			if(allocs[i] == ptr){found = 1; break;}
		}
		if(!found){
			err("newrealloc: attempt to realloc(%p,%d): bad pointer\n",ptr,size);
			error("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	
		}
	}
	#ifdef BUMP_ALLOCATOR
	void *m = bumprlloc(ptr, size);
	#else
	void *m = oldrealloc(ptr, size);
	#endif
	if(m){
		if(found){
			memused -= allocsizes[i];
			memused += size;
			allocsizes[i] = size;
			allocs[i] = m;
		}else{
			allocs[numallocs] = m;
			allocsizes[numallocs] = size;
			allocfiles[numallocs] = file;
			allocfuncs[numallocs] = func;
			alloclines[numallocs] = line;
			numallocs++;
			memused += size;
		}
	}else{
		err("newrealloc: realloc(%p,%d) failed\n",ptr,size);
		err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	}
	return m;
}
void newfree(void *ptr, const char *file, const char* func, int line, void (oldfree)(void *ptr)){
	int i = 0;
	int found = 0;
	if(ptr){
		for(i = 0; i < 2000; i++){
			if(allocs[i] == ptr){found = 1; break;}
		}
		if(!found){
			err("newfree: free(%d): bad pointer\n",ptr);
			error("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
		}
		allocs[i] = 0;
		allocsizes[i] = 0;
		allocfiles[i] = 0;
		alloclines[i] = 0;
		allocfuncs[i] = 0;
	}else{
		err("note: free(0)\n");
		err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	}
	//oldfree(ptr);
}
void printmemory(FILE *fp){
	fprintf(fp, "num allocs: %d, mem used: %d\n", numallocs, memused);
}
void printallocs(FILE *fp){
	int i;
	for(i = 0; i < 2000; i++){
		if(allocs[i]){
			fprintf(fp, "alloc(%d)\t=\t%p\tfrom file \"%s\"\tline %d,\tfunc \"%s\"\n", allocsizes[i],allocs[i],allocfiles[i],alloclines[i],allocfuncs[i]);
		}
	}
}
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
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	va_end(ap);
	err("error: printing allocations:\n");
	printmemory(stderr);
	printallocs(stderr);
	err("error: activating self-destruct\n");
	exit(*(int*)0);
}