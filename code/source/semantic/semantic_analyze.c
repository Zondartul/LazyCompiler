#include "semantic.h"
#include "yaccin.tab.h" //for get_source_text2()
#include "ctype.h"

void semantic_analyze_program(ast_node *node){
	//program :	decl_stmt_list	;
	
	//printf("got program\n");
	new_symbol_table(node);
	new_code_segment();
	push_code_segment();
	new_code_segment();
	init_CS = currentCodeSegment;
	pop_code_segment();
	semantic_decl = 1;
	analyze_scope(ast_get_child(node,0),&currentCodeSegment,0,&currentSymbolTable,0,0);
	semantic_decl = 0;
	analyze_scope(ast_get_child(node,0),&currentCodeSegment,0,&currentSymbolTable,0,1);

	semantic_finalize();
	printf("semantic: done\n");
}


const char* emit_push_label(const char* lbl) {
	const char* lbl2 = IR_next_name(namespace_semantic, lbl);
	push_expr(lbl2);
	emit_code("SYMBOL %s LABEL", lbl2);
	return lbl2;
}

int is_prefix_of(const char* str1, const char* str2) {
	return (strncmp(str1, str2, strlen(str1)) == 0);
}

int is_label_type(const char* lbl1, const char* lbl2) {
	return is_prefix_of(lbl2, lbl1);
}

int is_temp_val(const char* val) {
	return is_prefix_of("temp", val);
}

void assert_label_type(const char* lbl1, const char* lbl2) {
	if (!is_label_type(lbl1, lbl2)) {
		error("internal semantic error: got lbl [%s], expected one of [%s]", lbl1, lbl2);
	}
}

void assert_temp_val(const char* val) {
	if (!is_temp_val(val)) {
		error("internal semantic error: got [%s], expected a temporary value", val);
	}
}

void assert_is_if_exit(const char* lbl) {
	assert_label_type(lbl, "lbl_if_exit");
}

void assert_is_if_else(const char* lbl) {
	if (strcmp(lbl, "NOELSE") != 0) {
		assert_label_type(lbl, "lbl_if_else"); //elseif also matches
	}
}

int is_rval(const char* val) {
	return is_prefix_of("rval", val);
}

void assert_rval(const char* val) {
	if (!is_rval(val)) {
		error("internal semantic error: got [%s], expected RValue", val);
	}
}

int is_lval(const char* val) {
	return is_prefix_of("lval", val);
}

void assert_lval(const char* val) {
	if (!is_lval(val)) {
		error("internal semantic error: got [%s], expected LValue", val);
	}
}

int is_number(const char* val) {
	//1. check if it only has digits and format symbols
	int len = strlen(val);
	for (int i = 0; i < len; i++) {
		char c = val[i];
		if (isdigit(c) || (c == '.')) //how do we deal with 0x, 0c, 0b?
		{
			continue;
		}
		else {
			return 0;
		}
	}
	//2. maybe check if it actually reads? nah
	return 1;
}

int is_ival(const char* val) {
	return is_number(val);
}

int is_val(const char* val) {
	return is_lval(val) || is_rval(val) || is_ival(val);
}

void assert_val(const char* val) {
	if (!is_val(val)) {
		error("inernal semantic error: got [%s], expected val", val);
	}
}

int is_expr_res(const char* val) {
	return (strcmp(val, "DISCARD") == 0) || is_val(val);
}

void assert_expr_res(const char* val) {
	if (!is_expr_res(val)) {	
		error("internal semantic error: got [%s], expected expr_res type");
	}
}

void assert_no_result(const char* val) {
	if (!(strcmp(val, "DISCARD") == 0)) {
		error("internal semantic error: expr_res is [%s] but only DISCARD is allowed because there is no result.");
	}
}


const char* new_rval() {return IR_next_name(namespace_semantic, "rval");}
const char* new_lval() {return IR_next_name(namespace_semantic, "lval");}

const char* to_prefixed_name(const char* prefix, const char *val) {
	char buff[80];
	snprintf(buff, "%s%s", prefix, val);
	const char* res = IR_next_name(namespace_semantic, buff);
	return res;
}

const char* to_rval_name(const char* val) { return to_prefixed_name("rval_", val); }
const char* to_lval_name(const char* val) { return to_prefixed_name("lval_", val); }

//outputs the value res_val into the destination res_dest,
//changing r-v-qualification if necessary.
//returns 1 if something was outputted and 0 otherwise.
//... note: also maybe ask before emitting random code?
int output_res(const char* res_dest, const char* res_val) {
	assert_expr_res(res_dest);
	assert_val(res_val);
	if (strcmp(res_dest, "DISCARD")==0) {
		return 0;
	}
	else if (is_rval(res_dest)) {
		if (is_rval(res_val)) {
			emit_code("MOV %s %s /* R<-R, ref copy */", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
		else if(is_lval(res_val)){

			//for both lvals and constants
			//emit("MOV %s &%s /* R<-L, promote */", res_dest, res_val);
			error("internal semantic error: cannot promote Lvalue (%s) to Rvalue", res_val);
		}
		else {
			error("internal semantic error: cannot promote I-Lvalue (%s) to Rvalue", res_val);
		}
	}
	else if (is_lval(res_dest)) {
		if (is_rval(res_val)) {
			emit_code("MOV %s *%s /* L<-R, demote/deref */", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
		else if (is_lval(res_val)) {
			emit_code("MOV %s %s /* L<-L, val copy */", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
		else{
			//probably a costant
			emit_code("MOV %s %s /* L<-I, load immediate */", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
	}
	else {
		error("internal semantic error: can only write to rvals and lvals");
	}
	return 0; //unreachable
}

void semantic_analyze_decl_stmt_list(ast_node *node){
	//decl_stmt_list:	decl_stmt_list_ne | ;
	//decl_stmt_list_ne: decl_stmt_list_ne decl_stmt | decl_stmt;
	//...
	//linearized: decl_stmt_list: decl_stmt ... decl_stmt ;

	for(int i = 0; i < node->children.size; i++){
		semantic_analyze(ast_get_child(node, i)); //decl_stmt
	}

}

void semantic_analyze_decl_stmt(ast_node *node){
	//decl_stmt:		class_def | func_def | var_decl ';' ;
	push_expr("DISCARD");
	semantic_analyze(ast_get_child(node,0)); //class_def | func_def | var_decl
}

void semantic_analyze_func_def(ast_node *node){
	const char* res_dest = pop_expr(); //assert_expr_res(res_dest);
	assert_no_result(res_dest);
	//func_def	:	typename ID '('	var_decl_list ')' stmt_list END ;
	
	//if(!semantic_decl){return;}
	//printf("got func_def\n");
	/*
	func_def(ID)+3
	+-typename(type)/(<class> ID)
	| +- ptr_stars
	+-var_decl_list
	| +-var_decl_list_ne
	|   +- var_decl(ID)
	|   |  +-typename(type)/(<class> ID)
	|   |	   +- ptr_stars
	|   /
	|   +-var_decl_list_ne...
	+-stmt_list
	  +- stmt_list_ne
	  |  +- stmt
	  |  /
	  /  +- stmt_list_ne...
	  +-<empty>
	*/
	
	struct ast_node *node_func_def 		= node;
	struct ast_node *node_typename 		= ast_get_child(node_func_def,0);
	//struct ast_node *node_id			= ast_get_child(node_func_def,1);
	struct ast_node *node_var_decl_list	= ast_get_child(node_func_def,2);
	struct ast_node *node_stmt_list 	= ast_get_child(node_func_def,3);
	
	struct type_name *T = parseTypename(node_typename);//(ast_get_child(node,0));//semantic_get_type(ast_get_child(node,0)->token.value);
	const char *name = node->token.value;
	if(semantic_decl){
		struct symbol *S = symbol_new0();//new_symbol();
		S->type = SYMBOL_FUNCTION;
		S->storage = STORE_CODE;
		S->username = name;
		if(semantic_flatten){
			char buff[200];
			sprintf(buff,"_%s",name);
			S->IR_name = IR_next_name(namespace_semantic,buff);
		}else{
			S->IR_name = name;
		}
		//get arguments
		//struct symbol_table *ST = new_symbol_table();
		S->symfunction.returntype = T;
		push_symbol_table();
		new_symbol_table(node);
		S->symfunction.scope = currentSymbolTable;
		S->symfunction.code = 0;
		//m(ST_stack,push_back,currentSymbolTable);
		//if(semantic_context == SEMANTIC_MEMBER){
		if(symbolThis){
			//add param: this;
			struct type_name *T = semantic_get_type(symbolThis->username);
			T->pointerlevel = 1;
			char *name = "this";
			struct symbol *S;
			S = symbol_new0();//new_symbol();
			S->username = name;
			if(semantic_flatten){
				S->IR_name = IR_next_name(namespace_semantic,name);
			}else{
				S->IR_name = name;
			}
			S->type = SYMBOL_PARAM;
			S->symvariable.type = T;
			S->symvariable.array = 0;
			S->symvariable.arraysize = 0;
			//S->symvariable.pos = 0;
			S->store_adr = 0;
			S->symvariable.size = getTypeSize(T);
			S->storage = STORE_DATA_STACK;
			push_symbol(S);
		}
		semantic_context = SEMANTIC_PARAM;
			//use new separate symbol table;
		struct type_name *signature = malloc(sizeof(struct type_name));
		signature->name = 0;
		signature->symclass = 0;
		signature->args = vector2_ptr_type_name_new();
		//parameter as visible from scope VS signature parameter, is confusing.
		int i;
		//wait, shouldn't we go through var_decl_list_ne first?
		//no, it's linearized now
		fprintf(stderr,"func_def, node_var_decl_list.token.type = [%s]\n",node_var_decl_list->token.type);
		for(i = 0; i < /*list*/node_var_decl_list->children.size; i++){
			struct ast_node *arg = ast_get_child(node_var_decl_list,i);//(list,i);
			fprintf(stderr,"func_def, child of node_var_decl_list is [%s]\n",arg->token.type);
			//struct ast_node *arg_typename = ast_get_child(arg,0); //unused
			struct type_name *T2 = parseTypename(node_typename);//(ast_get_child(arg,0));//semantic_get_type(ast_get_child(arg,0)->token.value);
			m((*(signature->args)),push_back,T2);
			push_expr(new_rval());//push_expr("NODISCARD");
			semantic_analyze(arg); //var_decl
		}
		S->symfunction.signature = signature;
		//S->size = 0;
			//join symbol table to definition
		semantic_context = SEMANTIC_NORMAL;
		//do not get code until imperative pass
		//currentSymbolTable = m(ST_stack,pop_back);
		pop_symbol_table();
		push_symbol(S);
		push_symbol_table();
		currentSymbolTable = S->symfunction.scope;
		analyze_scope(/*ast_get_child(node,2)*/node_stmt_list,0,0,&currentSymbolTable,0,0);
		pop_symbol_table();
		//ast_node *N =  ast_get_child(node,2);
		//ast_node *N1 = ast_get_child(N,0); //error, no children
		//ast_node *Nx = ast_get_child(N,N->children.size-1);
		//N is node_stmt_list
		//N1 is empty or stmt_list_ne
		//Nx is N1
		//YYLTYPE pos1 = N1->token.pos;
		//YYLTYPE pos2 = Nx->token.pos;
		//fprintf(stderr,"function [%s]\nfrom (file %s, line %d:%d)\nto (file %s, line %d:%d)\n",S->username,pos1.filename,pos1.first_line,pos1.first_column,pos2.filename,pos2.first_line,pos2.first_column);
	}else{
		printf("semantic_analyze (imperative) of func_def %s\n",name);
		struct symbol *S = lookup_symbol(name);
		if(S->type != SYMBOL_FUNCTION){error("semantic: '%s' is not a function\n",S->username);}
		//YYLTYPE pos1 = ast_get_child(node,0)->token.pos;
		//YYLTYPE pos2 = ast_get_child(node,1)->token.pos;	//note: do not get source text by start/end, as that includes #preprocessor directives, and those have been deleted
		//fprintf(stderr,"get_source_text(%d,%d,%s) = [%s]\n",pos1.start,pos1.end,pos1.filename,get_source_text2(pos1));//get_source_text(pos1.start,pos1.end,pos1.filename));
		//emit_code("/* %s %s(%s) */",get_source_text(pos1.start,pos1.end,pos1.filename), name, get_source_text(pos2.start,pos2.end,pos2.filename));
		
		//emit_code("/* %s %s(%s) */",get_source_text2(pos1), name, get_source_text2(pos2));
		
		const char *s_typename = get_source_text2(node_typename->token.pos);
		const char *s_args = get_source_text2(node_var_decl_list->token.pos);
		
		emit_code("/* %s %s(%s) */", s_typename, name, s_args);
		
		//emit_code("DEBUG BEGIN FUNC %s %s",S->IR_name, S->username);
		push_symbol_table();
		currentSymbolTable = S->symfunction.scope;
		//emit_all_declarations(); //the analyze_scope will handle symbols (they are inside a frame anyway)
		//emit_code("SYMBOL %s LABEL",S->IR_name);
		//emit_code("LABEL %s",S->IR_name);			
		emit_code("FUNCTION %s BEGIN",S->IR_name);			
		push_semantic_context();
		
		//const char *prev_semantic_this = semantic_this;
		//if(semantic_this){semantic_this = lookup_symbol(semantic_this)->IR_name;}
		//if(semantic_context == SEMANTIC_MEMBER){semantic_this = lookup_symbol("this")->IR_name;}
		pop_symbol_table();
		semantic_context = SEMANTIC_NORMAL;
		printf("analyze statement list...\n");
		analyze_scope(/*ast_get_child(node,2)*/ node_stmt_list,0,
						&S->symfunction.code,
						&S->symfunction.scope,
						0,0);
		printf("st.list done\n");
		//semantic_this = prev_semantic_this;
		semantic_this = 0;
		pop_semantic_context();
		emit_code_segment(S->symfunction.code);
		emit_code("RET");
		emit_all_undeclarations();
		emit_code("FUNCTION %s END",S->IR_name);
		//emit_code("DEBUG END FUNC %s",S->IR_name);
		emit_code("/* end */");
		printf("semantic_analyze (imperative) of func %s done\n",name);
	}
}

void semantic_analyze_var_decl_list(ast_node *node){
	//var_decl_list:	var_decl_list_ne | ;
	//var_decl_list_ne:	var_decl_list_ne ',' var_decl | var_decl;
	//...
	//linearized: var_decl ',' ... var_decl
	//printf("got var_decl_list\n");
	for(int i = 0; i < node->children.size; i++){
		push_expr(new_rval());//push_expr("NODISCARD");
		semantic_analyze(ast_get_child(node, i)); //var_decl
	}
}

void semantic_analyze_var_decl(ast_node *node){

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	//var_decl:	typename ID
	//| typename ID '[' expr ']'
	//| typename ID '=' expr
	//;

	if(!semantic_decl){return;}//goto semantic_exit;}
	//printf("got var_decl\n");
	struct type_name *T = parseTypename(ast_get_child(node,0));//semantic_get_type(ast_get_child(node,0)->token.value);
	const char *name = node->token.value;
	int array = 0;
	int arraysize = 0;
	if(node->token.production == 1){
		array = 1;
		struct ast_node *Nsize = ast_get_child(node,1);
		arraysize = atoi(Nsize->token.value);
		if(!arraysize){error("semantic error: array size has to be positive integer");}
	}
	struct symbol *S;// = lookup_symbol_here(name);
	// if(S){
		// error("semantic error: S is already defined");
	// }
	S = symbol_new0();//new_symbol();
	S->username = name;
	if(semantic_flatten){
		S->IR_name = IR_next_name(namespace_semantic,name);
	}else{
		S->IR_name = name;
	}
	S->symvariable.type = T;
	if(semantic_context == SEMANTIC_PARAM){
		S->type = SYMBOL_PARAM;
		//S->symparameter.pos = currentSymbolTable->symbols.size;
		//S->symvariable.pos = getNumParameters();
		S->store_adr = getNumParameters();
		S->symvariable.size = getTypeSize(T);
		S->symvariable.array = 0;
		S->storage = STORE_DATA_STACK;
		if(array){error("semantic error: arrays as function parameter not implemented ");}
	/*}else if(semantic_context == SEMANTIC_MEMBER){ //SEMANTIC_MEMBER isn't very descriptive anyway
		S->type = SYMBOL_MEMBER;
		//S->symvariable.pos = getNumMembers();
		S->store_adr = getNumMembers();
		S->symvariable.size = getTypeSize(T);
		S->symvariable.array = 0;
		S->storage = STORE_DATA_MEMBER;
	*/	
	}else{
		S->type = SYMBOL_VARIABLE;
		//S->symvariable.pos = getNumVariables();
		S->store_adr = getNumVariables();
		S->symvariable.size = getTypeSize(T);
		S->storage = STORE_DATA_STACK;
		S->symvariable.array = 0;
	}
	if(array){S->symvariable.array = 1; S->symvariable.type->pointerlevel++; S->symvariable.arraysize = arraysize;}
	if(!currentSymbolTable->parent){S->global = 1;}
	push_symbol(S);
	//put into symbol table
}

void semantic_analyze_var_decl_assign(ast_node *node){

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	//production | typename ID '=' expr
	//yacc refs	 |    $1    $2  $3  $4
	//children   |    (0)   (1)     (2)
	struct ast_node *node_typename = ast_get_child(node,0);
	//struct ast_node *node_id = ast_get_child(node,1);
	struct ast_node *node_expr = ast_get_child(node,2);
	
	struct symbol *S;
	if(semantic_decl){
		//printf("got var_decl\n");
		struct type_name *T = parseTypename(node_typename);//ast_get_child(node,0));//semantic_get_type(ast_get_child(node,0)->token.value);
		const char *name = node->token.value;
		S = symbol_new0();//new_symbol();
		S->username = name;
		/* if(semantic_flatten){
			S->IR_name = IR_next_name(namespace_semantic,name);
		}else{
			S->IR_name = name;
		}
		if(semantic_context == SEMANTIC_PARAM){
			S->type = SYMBOL_PARAM;
			S->symparameter.type = T;
			S->symparameter.pos = currentSymbolTable->symbols.size;
		}else{
			S->type = SYMBOL_VARIABLE;
			S->symvariable.type = T;
		} */
		if(semantic_flatten){
			S->IR_name = IR_next_name(namespace_semantic,name);
		}else{
			S->IR_name = name;
		}
		S->symvariable.type = T;
		if(semantic_context == SEMANTIC_PARAM){
			S->type = SYMBOL_PARAM;
			//S->symparameter.pos = currentSymbolTable->symbols.size;
			//S->symvariable.pos = getNumParameters();
			S->store_adr = getNumParameters();
			S->symvariable.size = getTypeSize(T);
			S->symvariable.array = 0;
			S->storage = STORE_DATA_STACK;
		/*}else if(semantic_context == SEMANTIC_MEMBER){
			S->type = SYMBOL_MEMBER;
			//S->symvariable.pos = getNumMembers();
			S->store_adr = getNumMembers();
			S->symvariable.size = getTypeSize(T);
			S->symvariable.array = 0;
			S->storage = STORE_DATA_MEMBER;*/
		}else{
			S->type = SYMBOL_VARIABLE;
			//S->symvariable.pos = getNumVariables();
			S->store_adr = getNumVariables();
			S->symvariable.size = getTypeSize(T);
			S->symvariable.array = 0;
			S->storage = STORE_DATA_STACK;
		}
		if(!currentSymbolTable->parent){S->global = 1;}
		push_symbol(S);
		//put into symbol table
		//will deal with assignment later; ignore for now.
		//semantic_analyze(ast_get_child(node,1)); //expr (assignment)
		//put expression as "initial value"?
	}else{
		if(semantic_context != SEMANTIC_MEMBER){
			S = lookup_symbol(node->token.value);
			const char *result1 = S->IR_name;
			push_code_segment();
			if(!currentSymbolTable->parent){
				currentCodeSegment = init_CS;
			}
			//emit_code("INITIALIZER %s BEGIN",result1);
			push_expr(new_lval()); //only L-values can be written down //push_expr("NODISCARD");
			semantic_analyze(node_expr); //expr (what to assign)
			const char *result2 = pop_expr();
			emit_code("MOV %s %s", result1, result2);
			//emit_code("INITIALIZER %s END",result1);
			//if (!discardResult) { push_expr(result1); }
			output_res(res_dest, result1);
			pop_code_segment();
		}
	}
}

void semantic_analyze_stmt_list(ast_node *node){
	//stmt_list: stmt_list_ne | ;
	//stmt_list_ne: stmt_list_ne stmt | stmt;
	//...
	//linearized: stmt stmt ... stmt

	//printf("got stmt_list\n");
	for(int i = 0; i < node->children.size; i++){
		semantic_analyze(ast_get_child(node, i)); //stmt
	}
}

void semantic_analyze_stmt(ast_node *node){
	//stmt	:	imp_stmt	|	decl_stmt	;
	semantic_analyze(ast_get_child(node,0)); // imp_stmt | decl_stmt
}

void semantic_analyze_imp_stmt(ast_node *node){
	//imp_stmt:		if_block
	//| while_loop
	//| expr ';'
	//| RETURN ';'
	//| RETURN expr ';'
	//| for_loop
	//;

	//if(semantic_decl){goto semantic_exit;}
	////imperative pass: 
	//some things are now both declarative and imperative, oddly.
	switch(node->token.production){
		case(0)://if_block
			semantic_analyze(ast_get_child(node,0)); //if_block
			break;
		case(1)://while_loop
			semantic_analyze(ast_get_child(node,0)); //while_loop
			break;
		case(2)://expr
			if(semantic_decl){return;}//{goto semantic_exit;} //this one still imp-only though
			emit_code("/* %s */",
				/*get_source_text(node->token.pos.start,
								node->token.pos.end,
								node->token.pos.filename)*/
				removeComments(get_source_text2(node->token.pos)));
			push_expr("DISCARD");
			semantic_analyze(ast_get_child(node,0)); //expr (unusued, increment or function call)
			break;
		case(3)://return
			if(semantic_decl){return;}//{goto semantic_exit;}
			emit_code("/* %s */",
				/*get_source_text(node->token.pos.start,
								node->token.pos.end,
								node->token.pos.filename)*/
				removeComments(get_source_text2(node->token.pos)));
			emit_code("RET");
			break;
		case(4)://return expr
			if(semantic_decl){return;}//{goto semantic_exit;}
			emit_code("/* %s */",
				/*get_source_text(node->token.pos.start,
								node->token.pos.end,
								node->token.pos.filename)*/
				removeComments(get_source_text2(node->token.pos)));
			push_expr(new_lval());//push_expr("NODISCARD");
			semantic_analyze(ast_get_child(node,0)); //expr (what to return)
			emit_code("RET %s",pop_expr());
			break;
		case(5)://for_loop
			semantic_analyze(ast_get_child(node,0)); //for_loop
			break;
		default:
			error("semantic error: unknown switch case");
			break;
	}
}


void semantic_analyze_if_block(ast_node *node){
	//if_block	:	if_then END |	if_then ELSE stmt_list END ;

	const char *if_exit;
	const char* if_else;
	if(semantic_decl){
		push_symbol_table();
		switch(node->token.production){
			case(0)://if_then END
				semantic_analyze(ast_get_child(node,0)); //if_then
				break;
			case(1)://if_then ELSE stmt_list END
				semantic_analyze(ast_get_child(node,0)); //if_then
				new_symbol_table(ast_get_child(node,1)); 
				analyze_scope(ast_get_child(node,1),0,0,&currentSymbolTable,0,1);
				break;
			default:
				error("semantic error: unknown switch case");
				break;
		}
		pop_symbol_table();
	}else{
		//imperative pass: 
		//const char *CSname; //unused
		push_symbol_table();
		switch(node->token.production){
			case(0)://if_then END
				if_exit = emit_push_label("lbl_if_exit");
				push_expr("NOELSE");
				semantic_analyze(ast_get_child(node,0)); //if_then
				if_else = pop_expr(); assert_is_if_else(if_else);
				if_exit = pop_expr(); assert_is_if_exit(if_exit);
				emit_code("LABEL %s",if_exit);
				emit_code("/* end if */");
				break;
			case(1)://if_then ELSE stmt_list END
				if_exit = emit_push_label("lbl_if_exit");
				if_else = emit_push_label("lbl_if_else");

				semantic_analyze(ast_get_child(node,0)); //if_then
				emit_code("/* else */");
				emit_code("LABEL %s", if_else);
				struct code_segment *CSinsert;
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node,1));
				analyze_scope(ast_get_child(node,1),0,&CSinsert,&currentSymbolTable,0,1);
				emit_code_segment(CSinsert);
				if_else = pop_expr();	assert_is_if_else(if_else);
				if_exit = pop_expr();	assert_is_if_exit(if_exit);
				emit_code("LABEL %s",if_exit);
				emit_code("/* end if */");
			break;
			default:
				error("semantic error: unknown switch case");
				break;
		}
		pop_symbol_table();
	}
}

void semantic_analyze_if_then(ast_node *node){
	//if_then:	IF '(' expr ')' stmt_list
	//| if_then ELSEIF '(' expr ')' stmt_list
	//;

	const char *if_exit;
	const char* if_else;
	if(semantic_decl){
		push_symbol_table();
		switch(node->token.production){
			case(0): //IF ( expr ) stmt_list
				new_symbol_table(ast_get_child(node,1));
				analyze_scope(ast_get_child(node,1),0,0,&currentSymbolTable,0,1);
				break;
			case(2): //if_then ELSEIF ( expr ) stmt_list
				//new_symbol_table(ast_get_child(node,1));
				//analyze_scope(ast_get_child(node,1),0,0,&currentSymbolTable,0);
				semantic_analyze(ast_get_child(node,0)); //if_then
				new_symbol_table(ast_get_child(node,2));
				analyze_scope(ast_get_child(node,2),0,0,&currentSymbolTable,0,1);
				break;
			default:
				error("semantic error: unknown switch case");
				break;
		};
		pop_symbol_table();
	}else{
		//imperative pass: 
		//printf("got if_then\n");
		//const char *CSname; //unused
		YYLTYPE pos;
		push_symbol_table();

		if_else = pop_expr();	assert_is_if_else(if_else);
		if_exit = pop_expr();	assert_is_if_exit(if_exit);
		const char* nextLabel = if_else;
		if (strcmp(if_else, "NOELSE") == 0) { nextLabel = if_exit; }
		else { nextLabel = if_else; }

		switch(node->token.production){
			case(0): 
			{
				//IF ( expr ) stmt_list
				//(expr)
				pos = ast_get_child(node, 0)->token.pos;
				emit_code("/* if(%s) */", get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
				push_expr(new_lval());//push_expr("NODISCARD");
				semantic_analyze(ast_get_child(node, 0)); //expr (condition)
				const char* condition_result = pop_expr(); assert_temp_val(condition_result);
				//if then
				//const char *nextLabel = IR_next_name(namespace_semantic,"lbl_if_then");
				emit_code("/* this skips untrue if's */");
				emit_code("JE 0 %s %s", condition_result, nextLabel);
				emit_code("/* 'if' is true: */");

				//stmt_list
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 1));
				struct code_segment* CSinsert;
				analyze_scope(ast_get_child(node, 1), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_exit = IR_next_name(namespace_semantic,"lbl_if_exit");
				//emit_code("SYMBOL %s LABEL",if_exit);
				if (nextLabel != if_exit) {  
					emit_code("JMP %s", if_exit);
				} //else, we can skip the jmp as the next instruction is already at the end of the if-block.
				//push_expr(if_exit);
				//end
				//emit_code("LABEL %s", nextLabel);
				break;
			}
			case(2): 
			{
				//if_then ELSEIF ( expr ) THEN stmt_list
				//the inner if-then should use the same exit as us, but use us as the else.
				push_expr(if_exit);
				const char* if_elseif = emit_push_label("lbl_if_elseif");

				//if_then
				semantic_analyze(ast_get_child(node, 0)); //if_then
				assert_is_if_else(pop_expr()); //discard the labels from inner if_then
				assert_is_if_exit(pop_expr()); 
				//else if
				YYLTYPE pos = ast_get_child(node, 1)->token.pos;
				emit_code("/* else if(%s) */", get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
				emit_code("LABEL %s", if_elseif);
				//( expr )
				push_expr(new_lval());//push_expr("NODISCARD");
				semantic_analyze(ast_get_child(node, 1)); //expr (condition)
				const char* condition_result = pop_expr(); assert_temp_val(condition_result);
				//then
				//const char* label1 = IR_next_name(namespace_semantic, "lbl_elseif_then");
				//emit_code("SYMBOL %s LABEL",label1);
				//emit_code("JE 0 %s %s", pop_expr(), label1);
				emit_code("/* this skips untrue if's */");
				emit_code("JE 0 %s %s", condition_result, nextLabel);
				emit_code("/* then */");
				//stmt_list
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 2));
				struct code_segment* CSinsert;
				analyze_scope(ast_get_child(node, 2), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_exit = pop_expr(); //using existing exit
				if (nextLabel != if_exit) {
					emit_code("JMP %s", if_exit);
				} //else, we can skip the jmp as the next instruction is already at the end of the if-block.
				//push_expr(if_exit);
				//emit_code("LABEL %s", label1);
				break;
			}
			default:
				error("semantic error: unknown switch case");
				break;
		};
		push_expr(if_exit);
		push_expr(if_else);

		pop_symbol_table();
	}
}

void semantic_analyze_while_loop(ast_node *node){
	//while_loop	:	WHILE '(' expr ')' stmt_list END ;

	if(semantic_decl){
		push_symbol_table();
		new_symbol_table(node);
		analyze_scope(ast_get_child(node,1),0,0,&currentSymbolTable,0,1);
		pop_symbol_table();
	}else{
		YYLTYPE pos1 = ast_get_child(node,0)->token.pos;
		emit_code("/* while(%s) */",get_source_text2(pos1));//get_source_text(pos1.start,pos1.end,pos1.filename));
		const char *label1 = IR_next_name(namespace_semantic,"lbl_while_do");
		const char *label2 = IR_next_name(namespace_semantic,"lbl_while_exit");
		emit_code("SYMBOL %s LABEL",label1);
		emit_code("SYMBOL %s LABEL",label2);
		emit_code("LABEL %s",label1);
		push_expr(new_lval());//push_expr("NODISCARD");
		semantic_analyze(ast_get_child(node,0)); //expr (condition)
		//analyze_scope(ast_get_child(node,0),0,&CSinsert,0,0);
		//emit_code_segment(CSinsert);
		emit_code("JE 0 %s %s",pop_expr(), label2);
		emit_code("/* do */");
		push_symbol_table();
		currentSymbolTable = find_symbol_table_by_node(node);
		struct code_segment *CSinsert;
		analyze_scope(ast_get_child(node,1),0,&CSinsert,&currentSymbolTable,0,1);
		emit_code_segment(CSinsert);
		emit_code("JMP %s",label1);
		emit_code("LABEL %s",label2);
		emit_code("/* end while*/");
		pop_symbol_table();
	}
}

void semantic_analyze_for_loop(ast_node *node){
	//for_loop	:	FOR '(' stmt expr ';' expr ')' stmt_list END ;

	if(semantic_decl){
		push_symbol_table();
		new_symbol_table(node);
		semantic_analyze(ast_get_child(node,0)); //stmt (first)
		semantic_analyze(ast_get_child(node,3)); //stmt_list
		//analyze_scope(ast_get_child(node,3),0,0,&currentSymbolTable,0);
		pop_symbol_table();
	}else{
		YYLTYPE pos1 = ast_get_child(node,0)->token.pos;
		YYLTYPE pos2 = ast_get_child(node,1)->token.pos;
		YYLTYPE pos3 = ast_get_child(node,2)->token.pos;
		YYLTYPE pos4 = pos1;
		pos4.last_line = pos3.last_line;
		pos4.last_column = pos3.last_column;
		emit_code("/* for(%s) */",get_source_text2(pos4));//get_source_text(pos1.start,pos3.end,pos1.filename));
		push_symbol_table();
		currentSymbolTable = find_symbol_table_by_node(node);
		push_code_segment();
		new_code_segment();
		//emit_code("FRAME ENTER"); -- nope
		emit_code("/* %s */",removeComments(get_source_text2(pos1)));//get_source_text(pos1.start,pos1.end,pos1.filename));
		semantic_analyze(ast_get_child(node,0)); //stmt (int i = 0;)
		const char *loopCondition = IR_next_name(namespace_semantic,"lbl_for");
		const char *loopExit = IR_next_name(namespace_semantic,"lbl_for_exit");
		emit_code("SYMBOL %s LABEL",loopCondition);
		emit_code("SYMBOL %s LABEL",loopExit);
		//LOOP CONDITION
		emit_code("LABEL %s",loopCondition);
		emit_code("/* %s */",removeComments(get_source_text2(pos2)));//get_source_text(pos2.start,pos2.end,pos2.filename));
		push_expr(new_lval());//push_expr("NODISCARD");
		semantic_analyze(ast_get_child(node,1)); //expr (i < 10;)
		const char *condition = pop_expr();
		emit_code("JE 0 %s %s",condition,loopExit);
		emit_code("/* loop body */");
		//LOOP BODY
		semantic_analyze(ast_get_child(node,3)); //stmt_list
		emit_code("/* %s */",removeComments(get_source_text2(pos3)));//get_source_text(pos3.start,pos3.end,pos3.filename));
		push_expr("DISCARD");
		semantic_analyze(ast_get_child(node,2)); //expr (i++)
		emit_code("JMP %s",loopCondition);
		emit_code("LABEL %s",loopExit);
		emit_all_deinitializers();
		//emit_code("FRAME LEAVE");
		emit_code("/* end for */");
		ptr_code_segment CS = currentCodeSegment;
		pop_code_segment();
		emit_code_segment(CS);
		pop_symbol_table();
	}
}

void semantic_analyze_class_def(ast_node *node){
	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	assert_no_result(res_dest);

	//production|	CLASS ID decl_stmt_list END
	//yacc refs |		  $1  $2     $3
	//child		|		  (0) (1)
	//struct ast_node *node_id  = ast_get_child(node,0);
	struct ast_node *node_dsl = ast_get_child(node,1);
	
	if(semantic_decl){
		//printf("got class_def\n");
		const char *name = node->token.value;
		struct symbol *S = symbol_new0();//new_symbol();
		push_symbol(S);
		S->username = name;
		S->storage = STORE_CODE;
		if(semantic_flatten){
			S->IR_name = IR_next_name(namespace_semantic,name);
		}else{
			S->IR_name = name;
		}
		S->type = SYMBOL_CLASS;
		//make new symbol table
		//struct symbol_table *ST = new_symbol_table();
		push_symbol_table();
		new_symbol_table(node);
		S->symclass.scope = currentSymbolTable;
		push_semantic_context();
		//semantic_context = SEMANTIC_MEMBER;
		symbolThis = S;
		semantic_this = S->IR_name;
		
		struct symbol *T = symbol_new0();//new_symbol();
		T->username = stralloc("this");
		T->IR_name = IR_next_name(namespace_semantic,"this");
		T->type = SYMBOL_PARAM; //wait a sec, wtf is a SYMBOL_MEMBER?
		T->storage = STORE_DATA_STACK; //no idea if this does anything
		T->store_adr = 0;
		T->global = 0;
		T->symvariable.pos = 0;
			struct type_name *Tn = malloc(sizeof(struct type_name));
			Tn->name = S->username;
			Tn->pointerlevel = 1;
			Tn->symclass = S;
			Tn->args = 0;
		T->symvariable.type = Tn;
		T->symvariable.array = 0;
		T->symvariable.arraysize = 0;
		
		push_symbol(T);
		
		semantic_analyze(node_dsl); //decl_stmt_list
		pop_semantic_context();
		class_def_finalize();
		semantic_this = 0;
		symbolThis = 0;
		pop_symbol_table();
		//embed into existing symbol table
		//push_symbol(S);
	}else{
		symbolThis = lookup_symbol(node->token.value);
		class_emit_start();
		
		push_symbol_table();
		//const char *name = node->token.value; //unused
		currentSymbolTable = symbolThis->symclass.scope; //lookup_symbol(name)->symclass.scope;
		//semantic_this = "ERROR";//lookup_symbol("this")->IR_name;//symbolThis->IR_name;//"this";
		
		semantic_context = SEMANTIC_MEMBER;
		semantic_analyze(node_dsl);//decl_stmt_list //this adds functions that were actually defined in code
		semantic_context = SEMANTIC_NORMAL;
		
		//semantic_this = 0;
		pop_symbol_table();
		class_emit_end();
		symbolThis = 0;
		
	}
}

void semantic_analyze_expr_list(ast_node *node){
	//expr_list:	expr_list_ne | ;
	//expr_list_ne: expr_list_ne ',' expr | expr;
	//...
	//linearized: expr_list: expr ',' expr ... ',' expr ;

	if(semantic_decl){return;}//goto semantic_exit;}
	//imperative pass: 
	//printf("got expr_list\n");
	for(int i = 0; i < node->children.size; i++){
		//so some of these may be expressions and some, expression lists. idk.
		//well, actually, the list should be linearized.
		push_expr(new_lval());//LV or RV? idk //push_expr("NODISCARD");
		semantic_analyze(ast_get_child(node, i)); //expr
		//give name to new expression result?
		//put it in stack?
	}
}

void semantic_analyze_expr_id(ast_node *node){
	//expr	:	ID	

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);

	//printf("got expr_id = [%s]\n",node->token.value);
	char *name = stralloc(node->token.value);
	struct symbol *S = lookup_symbol(name);
	if((S->type != SYMBOL_VARIABLE)&&(S->type != SYMBOL_PARAM)&&(S->type != SYMBOL_MEMBER)&&(S->type != SYMBOL_FUNCTION)){
		error("semantic: '%s' is not a variable, parameter or function name\n",name);
	}
	struct type_name* T = 0;
	if(S->type == SYMBOL_VARIABLE){
		T = S->symvariable.type;
	}else if(S->type == SYMBOL_PARAM){
		T = S->symvariable.type;
	}else if(S->type == SYMBOL_MEMBER){
		T = S->symvariable.type;
	}else if(S->type == SYMBOL_FUNCTION){
		T = S->symfunction.signature;
	}else{
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n",pos.first_line,get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic: '%' is not an expression\n",name);
	}
	printf("found variable '%s' of type '%s'\n",name,T->name);
	//exprResult = name;
	const char* S_full = to_rval_name(S->IR_name);

	if(semantic_this){
		//char buff[80];
		
		if(S->type == SYMBOL_VARIABLE){
			//const char *S_full = IR_next_name(namespace_semantic,S->IR_name);//IR_next_name(namespace_semantic,S->IR_name);
			emit_code("ADD %s %s &%s",S_full, semantic_this, S->IR_name);
			//sprintf(buff,"*%s",S_full);
			//if (!discardResult) { push_expr(stralloc(buff)); }
			output_res(res_dest, S_full);
		}else if(S->type == SYMBOL_FUNCTION){
			//if (!discardResult) {push_expr(S->IR_name);}
			output_res(res_dest, S_full);
		}else{
			error("[SEMANTIC] Unknown symbol type ");
		}
		//sprintf(buff,"%s:%s",semantic_this,S->IR_name);
		//const char *result1 = stralloc(buff);
		//push_expr(result1);
	}else{
		//if (!discardResult) { push_expr(S->IR_name); }
		output_res(res_dest, S_full);
		//push_expr(S->username);
	}
	push_exprtype(T);
	//put variable <name> on the stack
}

void semantic_analyze_expr_const(ast_node *node){
	//expr:
		//| INTEGER
		//| INTEGERX
		//| INTEGERB
		//| FLOATING
		//| CHARACTER
		//| STRING

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	
	//printf("got expr_const\n");
	//char *exprResult = IR_next_name(namespace_semantic,"$reg");
	//sanitize the value, maybe put it somewhere,
	//get pointers, whatever
	struct type_name *T = malloc(sizeof(struct type_name));
	T->symclass = 0;
	T->args = 0;
	//YYLTYPE pos; //unused
	switch(node->token.production){
		case(0): //int
			T->name = "int";
			
			//if (!discardResult) {
			//	push_expr(node->token.value);
			//}
			output_res(res_dest, node->token.value);
		break;
		case(1): //hex int
			T->name = "int";
			//if (!discardResult) {
			//	push_expr(node->token.value);
			//}
			output_res(res_dest, node->token.value);
		break;
		case(2): //binary int
			T->name = "int";
			//if (!discardResult) {
			//	push_expr(node->token.value);
			//}
			output_res(res_dest, node->token.value);
		break;
		case(3): //float
			T->name = "float";
			//if (!discardResult) {
			//	push_expr(node->token.value);
			//}
			output_res(res_dest, node->token.value);
		break;
		case(4): //char
			T->name = "char";
			char buff[80];
			sprintf(buff,"%d",node->token.value[1]);
			const char* str = stralloc(buff);
			//if (!discardResult) {
			//	push_expr(stralloc(buff));
			//}
			output_res(res_dest, str);
		break;
		case(5): //string
			T->name = "string";
			const char *str_name = IR_next_name(namespace_semantic,"str");
			struct symbol *S = symbol_new0();//new_symbol();
			S->type = SYMBOL_CONST;
			S->storage = STORE_DATA_STATIC;
			S->symconst.type = CONST_STRING;
			S->symconst.data_string = stralloc(node->token.value);
			S->username = "str";
			S->IR_name = str_name;
			push_symbol(S);
			
			push_code_segment();
			currentCodeSegment = init_CS;
			emit_code("SYMBOL %s STRING %s",str_name,escape_string(node->token.value));
			pop_code_segment();
			//emit_code("DATA STATIC %s %s",str_name, node->token.value);
			//if (!discardResult) {
			//	push_expr(str_name);
			//}
			output_res(res_dest, str_name);
		break;
		default:
			error("semantic error: unknown switch case");
			break;
	};
	//emit_code("MOV %s %s",exprResult, node->token.value);
	//push_expr(exprResult);
	//push_expr(node->token.value);
	push_exprtype(T);
	//figure out what kind of const this is?
}

void semantic_analyze_expr_subexpr(ast_node *node){
	//expr: '(' expr ')'
	
	//printf("got expr_subexpr\n");
	//push_expr("NODISCARD"); 
	//-- just propagate the res_type we got.
	semantic_analyze(ast_get_child(node,0)); //expr
}

void semantic_analyze_expr_index(ast_node *node){
	//expr '[' expr ']'

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	push_expr(new_rval()); //Rvalue, because we need a reference to memory //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr (array)
	const char *index = pop_expr();
	push_expr(new_lval()); //lval because numbers are ok //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr (index)
	const char *ptr = pop_expr();
	
	//okay first of all, expr isn't necessarily a symtable symbol but
	//it could just be an rvalue expression (i.e. temporary value)
	//struct symbol *S = lookup_symbol_IR(ptr);//lookup_symbol(ptr);
	//if((S->type == SYMBOL_PARAM) ||
	//(S->type == SYMBOL_MEMBER) ||
	//(S->type == SYMBOL_VARIABLE)){
		//struct type_name *T = S->symvariable.type;
		struct type_name* T = pop_exprtype();
		if(T->pointerlevel == 0){
			//error("Semantic error: array access into %s, which has non-pointer type %s\n",S->username,T->name);
			const char* expr1_text = 0; //how do
			const char* expr1_type_text = 0;
			error("Semantic error: array access into %s, which has non-pointer type %s\n", expr1_text, expr1_type_text);
		}
		struct type_name *T2 = malloc(sizeof(struct type_name));
		*T2 = *T;
		T2->pointerlevel--;
		push_exprtype(T2);
		
		const char *result = IR_next_name(namespace_semantic,"temp");
		
		//if(lvalue){emit_code("SYMBOL %s VAR 1",result);}
		//if(S->symvariable.array){
		//	emit_code("ADD %s %s &%s",result,index,ptr);
		//}else{
			emit_code("ADD %s %s %s",result,index,ptr);
		//}
		//if(!lvalue){emit_code("MOV %s *%s",result,result);}
		char buff[80];
		sprintf(buff,"*%s",result);
		const char* res_val = stralloc(buff);
		//if (!discardResult) {
		//	push_expr(stralloc(buff));
		//}
		output_res(res_dest, res_val);
		return;//goto semantic_exit;
	//}
	error("Semantic error: array access into %s, which is not a variable\n");
}

void semantic_analyze_expr_call(ast_node *node){
	//expr: expr '(' expr_list ')'

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	//printf("got expr_call\n");
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr (function name or reference expression)
	const char *name = pop_expr();
	//struct symbol *S = lookup_symbol_IR(name);
	struct type_name *T = pop_exprtype();//S->symfunction.returntype;
	if(!T->args){
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n",pos.first_line,get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic error: attempt to call '%s', which is not a function in this scope\n",name);
	}
	ast_node *list = ast_get_child(node,1);
	int i;
	int len = 0;
	vector2_ptr_char ministack = vector2_ptr_char_here();
	for(i = 0; i < list->children.size; i++){
		push_expr(new_lval());//push_expr("NODISCARD");
		semantic_analyze(ast_get_child(list,i)); //expr? one of func args?
		const char *expr = pop_expr();
		len += snprintf(0,0,"%s ",expr);
		m(ministack,push_back,expr);
	}
	char *buff = malloc(sizeof(char)*(len+1));
	char *buff2 = buff;
	if(list->children.size == 0){buff = "";}
	for(i = 0; i < list->children.size; i++){
		const char *expr = m(ministack,pop_front);
		buff2 = buff2+sprintf(buff2," %s",expr); //writes to beginning of string, and moves the beginning?
		//buff = buff+sprintf(buff," %s",expr);
	}
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = CALL [%s][%s]",exprResult, name, buff);
	emit_code("CALL %s %s%s",exprResult,name,buff);
	
	//if (!discardResult) {
	//	push_expr(exprResult);
	//	push_exprtype(T);
	//}
	if (output_res(res_dest, exprResult)) {
		push_exprtype(T);
	}
}

void semantic_analyze_expr_dot(ast_node *node){
	//expr: expr '.' expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//{goto semantic_exit;}
	//printf("got expr_.\n");
	push_expr(new_rval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr (first one)
	//what if $0 isn't an ID? need to figure out
	//expression type... also instance... fuck, just redo this.
	//struct ast_node *N = ast_get_child(node,0); //unused
	struct type_name *T = pop_exprtype();
	if(!T->symclass){
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n",pos.first_line,get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic error: member access (x.y) impossible because x is a primitive (%s)\n",T->name);
	}
	//push_symbol_table(T->symclass->symclass.scope);
	push_symbol_table();
	currentSymbolTable = T->symclass->symclass.scope;
	semantic_this = pop_expr(); 
	push_expr(new_rval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr (member)
	const char* res2 = pop_expr();
	semantic_this = 0;
	pop_symbol_table();
	//emit code: member access
	//if (!discardResult) {
	//	push_expr(res2);
	//}
	output_res(res_dest, res2);
}

void semantic_analyze_expr_pow(ast_node *node){
	//expr: expr '^' expr	

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_^\n");
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr (base)
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr (exponent)
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = EXPONENT %s %s",exprResult, result1, result2);
	emit_code("EXP %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: exp
}

void semantic_analyze_expr_divide(ast_node *node){
	//expr: expr '/' expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_/\n");
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr (top)
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr (bottom)
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = DIVIDE %s %s",exprResult, result1, result2);
	emit_code("DIV %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: div
}

void semantic_analyze_expr_multiply(ast_node *node){
	//expr: expr '*' expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_*\n");
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr 1
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr 2
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = MULTIPLY %s %s",exprResult, result1, result2);
	emit_code("MUL %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: mul
}

void semantic_analyze_expr_modulo(ast_node *node){
	//expr: expr '%' expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_*\n");
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = MULTIPLY %s %s",exprResult, result1, result2);
	emit_code("MOD %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: mul
}

void semantic_analyze_expr_and(ast_node *node){
	//expr: expr '&' expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_*\n");
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = MULTIPLY %s %s",exprResult, result1, result2);
	emit_code("AND %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: mul
}

void semantic_analyze_expr_or(ast_node *node){
	//expr: expr '|' expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_*\n");
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = MULTIPLY %s %s",exprResult, result1, result2);
	emit_code("OR %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: mul
}

void semantic_analyze_expr_not(ast_node *node){
	//expr: '!' expr
	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_*\n");
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = MULTIPLY %s %s",exprResult, result1, result2);
	emit_code("NOT %s %s",exprResult, result1);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: mul
}

void semantic_analyze_expr_minus(ast_node *node){
	//expr: expr '-' expr
	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_-\n");
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = SUBTRACT %s %s",exprResult, result1, result2);
	emit_code("SUB %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: sub
}

void semantic_analyze_expr_plus(ast_node *node){
	//expr: expr '+' expr
	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_+\n");
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = ADD %s %s",exprResult, result1, result2);
	emit_code("ADD %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: add
}

void semantic_analyze_expr_increment(ast_node *node){
	//expr: INC	expr    %prec PREINC

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	push_expr(new_rval()); //because x = x+1; push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	if(node->token.production == 0){
		//post-increment
		const char *arg = pop_expr();
		const char *result = IR_next_name(namespace_semantic,"temp");
		emit_code("MOV %s %s",result,arg);
		emit_code("ADD %s %s 1",arg,arg);
		//if (!discardResult) {
		//	push_expr(result);
		//}
		output_res(res_dest, result);
	}else{
		//pre-increment
		const char *result = pop_expr();
		emit_code("ADD %s %s 1",result,result);
		//if (!discardResult) {
		//	push_expr(result);
		//}
		output_res(res_dest, result);
	}
}

void semantic_analyze_expr_decrement(ast_node *node){
	//expr: DEC	expr    %prec PREDEC	
	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	push_expr(new_rval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	if(node->token.production == 0){
		//post-decrement
		const char *arg = pop_expr();
		const char *result = IR_next_name(namespace_semantic,"temp");
		emit_code("MOV %s %s",result,arg);
		emit_code("SUB %s %s 1",arg,arg);
		//if (!discardResult) {
		//	push_expr(result);
		//}
		output_res(res_dest, result);
	}else{
		//pre-decrement
		const char *result = pop_expr();
		emit_code("SUB %s %s 1",result,result);
		//if (!discardResult) {
		//	push_expr(result);
		//}
		output_res(res_dest, result);
	}
}

void semantic_analyze_expr_neg(ast_node *node){
	//expr: '-' expr 	%prec PRENEG

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	const char *result = pop_expr();
	emit_code("NEG %s %s",result,result);
	//if (!discardResult) {
	//	push_expr(result);
	//}
	output_res(res_dest, result);
}

void semantic_analyze_expr_deref(ast_node *node){
	//expr: '*' expr	%prec PREDEREF

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	const char *arg = pop_expr();
	const char *result = IR_next_name(namespace_semantic,"temp");
	emit_code("MOV %s *%s",result, arg);
	//if (!discardResult) {
	//	push_expr(result);
	//}
	output_res(res_dest, result);
}

void semantic_analyze_expr_ref(ast_node *node){
	//expr: '&' expr	%prec PREREF

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	push_expr(new_rval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	const char *arg = pop_expr();
	const char *result = IR_next_name(namespace_semantic,"temp");
	emit_code("MOV %s &%s",result, arg);
	//if (!discardResult) {
	//	push_expr(result);
	//}
	output_res(res_dest, result);
}

void semantic_analyze_expr_equals(ast_node *node){
	//expr: expr EQUAL expr	

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_+\n");
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = EQUAL %s %s",exprResult, result1, result2);
	emit_code("EQUAL %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: add
}

void semantic_analyze_expr_notequal(ast_node *node){
	//expr: expr NOTEQUAL expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_+\n");
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = NOTEQUAL %s %s",exprResult, result1, result2);
	emit_code("NOTEQUAL %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: add
}

void semantic_analyze_expr_greater(ast_node *node){
	//expr: expr '>' expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_+\n");
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = GREATER %s %s",exprResult, result1, result2);
	emit_code("GREATER %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: add
}

void semantic_analyze_expr_less(ast_node *node){
	//expr: expr '<' expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_+\n");
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	push_expr(new_lval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	const char *exprResult = IR_next_name(namespace_semantic,"temp");
	//emit_code("%s = LESS %s %s",exprResult, result1, result2);
	emit_code("LESS %s %s %s",exprResult, result1, result2);
	//if (!discardResult) {
	//	push_expr(exprResult);
	//}
	output_res(res_dest, exprResult);
	//emit code: add
}

void semantic_analyze_expr_assign(ast_node *node){
	//expr: expr '=' expr

	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	if(semantic_decl){return;}//goto semantic_exit;}
	//printf("got expr_=\n");
	lvalue = 1;
	push_expr(new_rval()); //push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,0)); //expr
	lvalue = 0;
	push_expr(new_lval());//push_expr("NODISCARD");
	semantic_analyze(ast_get_child(node,1)); //expr
	const char *result2 = pop_expr();
	const char *result1 = pop_expr();
	//emit_code("%s = %s", result1, result2);
	emit_code("MOV %s %s", result1, result2);
	//push_expr(result1); //currently the assignment can't have a value because
							//we are using push_expr for other things too.
							//also means we can't have unused values on the stack? foo()?
	//emit code: assign
	output_res(res_dest, result1);
}






