#include "semantic.h"
#include "yaccin.tab.h" //for get_source_text2()
#include "ctype.h"
#include "semantic_analyze.h"
#include "semantic_expr_op.h"
#include <assert.h>

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

//verifies that a string is an actual printable ascii-string
int is_sanitary(const char* str) {
	//int len = strlen(str);
	const char* p = str;
	while (*p) {
		char c = *p++;
		if (!isprint(c)) {
			return 0;
		}
	}
	return 1;
}

const char *sanitize_string(const char* str) {
	if (is_sanitary(str)) {
		return str;
	}else {
		error("internal error: unclean string");
	}
	return 0;
}

const char* emit_push_label(const char* lbl) {
	const char* lbl2 = IR_next_name(namespace_semantic, lbl);
	push_expr(lbl2);
	emit_code("SYMBOL %s LABEL", sanitize_string(lbl2));
	return lbl2;
}

const char* rename_star(const char* val) {
	if (!val) {error("internal error");}
	vector2_char vstr = vector2_char_here();
	m(vstr, clear);
	vec_printf(&vstr, "*%s", val);
	return stralloc(vstr.data);
}

const char* rename_unstar(const char* val) {
	if (!val) { error("internal error"); }
	return stralloc(&(val[1]));
}

const char* rename_and(const char* val) {
	if (!val) { error("internal error"); }
	vector2_char vstr = vector2_char_here();
	m(vstr, clear);
	vec_printf(&vstr, "&%s", val);
	return stralloc(vstr.data);
}

const char* rename_unand(const char* val) {
	if (!val) { error("internal error"); }
	return rename_unstar(val);
}

void output_res(expr_settings stg, val_handle src, int /*do_emit*/) {
	/*
	*  IR VALUE RULES
	*   LVAL - raw typeless number, without a storage location.
	*   PTR - a number that refers to a location in memory, where something can be stored.
	*   RVAL - either 1) a "dereferenced pointer" expression (can't be stored in another value) (*x)
	*	  		or 2) a "primitive rvalue" aka register or function ( _main )
	*   --- stored values (left-side of mov-to LVAL or PTR) must not have accessors (&, *) 
	* 
	*  OUTPUT RES RULES
	*   parent node analyzer requests a value of specific type,
	*   and the current node analyzer MUST return a node of the same type.
	*   emitting is needed to perform a conversion (promition) that copies a value to temporary.
	* 
	*  Conversion matrix:
	*  dest val						IR expression syntax
	*  |							IR_out <- IR_in
	*  v
	*		LVAL	RVAL			PTR		<-- src val
	*  LVAL pass	rename.unstar	rename.unand	
	*		x <- x	x <r- *x		x <r- &x	   (switch from adr-repr to val-repr)
	*				(switch a2v)	rename.star-and-copy
	*								x <c- *x <r- x (deref a pointer here)
	*								actually nvm, just pass (when throwing a function into argument)
	*								x <- x pass
	*								copy
	*								x <c- *x		(also deref)
	* 
	*  RVAL	?   	pass			copy-and-rename.star
	*				*x <- *x		*x <r- x <c- &x
	*								rename
	*								*x <r- x
	*							
	*  PTR  pass	rename			pass
	*		x <- x	x <r- *x		x <- x
	*								copy    // actually unand
	*								x <c- &x//   x <r- &x
	*								copy
	*								x <c- *x
	* 
	* 3x3x3 possibilities
	* 
	*/

	if((strcmp(src.val, "&p1") == 0) && (strcmp(stg.dest.author, "expr_call funcname")==0)){
		printf("debug breakpoint");
	}

	//enum passType {
	//	PASS_DEST_LVAL = 1,
	//	PASS_DEST_RVAL = 2,
	//	PASS_DEST_PTR = 3,
	//	PASS_SRC_LVAL = 10,
	//	PASS_SRC_RVAL = 20,
	//	PASS_SRC_PTR = 30,
	//	PASS_ACC_NONE = 100,
	//	PASS_ACC_STAR = 200,
	//	PASS_ACC_AND = 300 //ampersand
	//};

	if (stg.dest.rv_type == E_ERROR) { error("internal semantic error: expr_settings null"); }
	if (stg.dest.rv_type == E_DISCARD) {
		if (stg.actual) {*(stg.actual) = (val_handle){ .val = 0 };}
		return;
	}
	VERIFY_RES(src);

	//expr_type _src_type = src.rv_type;
	//expr_type _dest_type = stg.dest.rv_type;
	//const char* _src_val = src.val;

	int dest_lval = (stg.dest.rv_type == E_LVAL);
	int dest_rval = (stg.dest.rv_type == E_RVAL);
	int dest_ptr = (stg.dest.rv_type == E_PTR);
	int src_lval = (src.rv_type == E_LVAL);
	int src_rval = (src.rv_type == E_RVAL);
	int src_ptr = (src.rv_type == E_PTR);
	int acc_star = (src.val[0] == '*');
	int acc_and = (src.val[0] == '&');
	int acc_none = (!acc_star) && (!acc_and);

	assert(src.val);
	assert(strlen(src.val));

	//verify that the value we are outputting is initialized and stuff
	if ((src.rv_type == E_ERROR)) {
		error("internal semantic error: invalid source value");
	}
	printf("output_res (%s)\n", src.val);

	//const char* resVal = stg.dest.val; //we may do a copy if the destination name is requested
	//if (!resVal) { resVal = src.val; }
	//int resValTemp = 0;
	const char* resVal = 0;
	//full emit string will be stored here
	vector2_char vstr = vector2_char_here();

	const char* op_str;

	if (dest_lval && src_lval && acc_none) { goto out_pass; }
	if (dest_lval && src_rval && acc_star) { goto out_pass; } // what happens when we put int *p; *p in an arg?//{ goto out_rename_unstar; }
	if (dest_lval && src_ptr && acc_and) { goto out_rename_unand; } // and this is the (*ptr)() situtation
	if (dest_lval && src_ptr && acc_none) { goto out_rename_star_and_copy; } // out_pass (idk, need star to read from arrays)
	if (dest_lval && src_ptr && acc_star) { goto out_copy; } 
#pragma message("something with casting an Lvalue to Rvalue")
	if (dest_rval && src_lval) {goto out_pass;}//{ error("internal semantic error: can't promote lval to rval"); }
	if (dest_rval && src_rval && acc_star) { goto out_pass; }
	if (dest_rval && src_ptr && acc_and) { goto out_copy_and_rename_star; }
	if (dest_rval && src_ptr && acc_none) { goto out_rename_star; }

	if (dest_ptr && src_lval && acc_none) { goto out_pass; }
	if (dest_ptr && src_rval && acc_star) { goto out_rename_unstar; }
	if (dest_ptr && src_ptr && acc_none) { goto out_pass; }
	if (dest_ptr && src_ptr && acc_and) { goto out_copy; } // out_rename_unand - idk, need out_copy to print(&local_var)
														   // but rename_unand for calling a function pointer... the expr_call not give us a &?
	if (dest_ptr && src_ptr && acc_star) { goto out_copy; }

	goto out_unexpected;
out_unexpected:
	error("output_res: unexpected case");
	return;

out_pass:
	op_str = "pass";
	resVal = src.val;
	goto out_finish;

out_copy:
	op_str = "copy";
	resVal = IR_next_name(namespace_semantic, "temp");
	vec_printf(&vstr, "MOV %s %s", sanitize_string(resVal), sanitize_string(src.val));
	goto out_finish;

out_rename_unstar:
	op_str = "rename_unstar";
	resVal = rename_unstar(src.val);
	goto out_finish;

out_rename_unand:
	op_str = "rename_unand";
	resVal = rename_unand(src.val);
	goto out_finish;

out_copy_and_rename_star:
	op_str = "copy_and_rename_star";
	resVal = IR_next_name(namespace_semantic, "temp");
	vec_printf(&vstr, "MOV %s %s", sanitize_string(resVal), sanitize_string(src.val));
	resVal = rename_star(resVal);
	goto out_finish;

out_rename_star_and_copy:
	op_str = "rename_star_and_copy";
	resVal = IR_next_name(namespace_semantic, "temp");
	vec_printf(&vstr, "MOV %s *%s", sanitize_string(resVal), sanitize_string(src.val));
	goto out_finish;

out_rename_star:
	op_str = "rename_star";
	resVal = rename_star(src.val);
	goto out_finish;

out_finish:
	
	const char* dest_rv_str = 0;
	if (dest_lval) { dest_rv_str = "L"; }
	if (dest_rval) { dest_rv_str = "R"; }
	if (dest_ptr)  { dest_rv_str = "P";	}
	const char* src_rv_str = 0;
	if (src_lval) { src_rv_str = "L"; }
	if (src_rval) { src_rv_str = "R"; }
	if (src_ptr)  { src_rv_str = "P"; }
	const char* dest_val = resVal;
	const char* src_val = src.val;
	const char* dest_auth = stg.dest.author;
	const char* src_auth = src.author;

	vec_printf(&vstr, "   /* out(%s <- %s): %s from %s (%s) //auth: to %s from %s */", 
		dest_rv_str, src_rv_str,
		dest_val, src_val,
		op_str,
		dest_auth, src_auth
	);

	emit_code(vstr.data);

	if (stg.actual) {
		*(stg.actual) = src;
		stg.actual->val = resVal;
		VERIFY_RES((*(stg.actual)));
	}
	

	//if (do_emit) {
	//	if (!stg.dest.val) {
	//		resVal = IR_next_name(namespace_semantic, "temp");
	//		resValTemp = 1;
	//	}
	//	if (stg.dest.rv_type == E_RVAL) {	
	//		if (src.rv_type == E_RVAL) { // x = y
	//			emit_code("MOV %s %s //output(R<R)", sanitize_string(resVal), sanitize_string(src.val));
	//		}
	//		if (src.rv_type == E_LVAL) { // *x = y;
	//			if (resValTemp) {
	//				error("can't output(R<L) to a temporary value, provide a destination!");
	//			}
	//			emit_code("MOV *%s %s //output(R<L)", sanitize_string(resVal), sanitize_string(src.val));
	//		}
	//		if (src.rv_type == E_PTR) { //temp_ptr = arr + 5;
	//			emit_code("MOV %s %s //output(R<P)", sanitize_string(resVal), sanitize_string(src.val));
	//			vector2_char vstr = vector2_char_here();
	//			vec_printf(&vstr, "*%s", resVal);
	//			resVal = stralloc(vstr.data);
	//		}
	//	}
	//	if (stg.dest.rv_type == E_LVAL) { 
	//		if (src.rv_type == E_RVAL) { // temp_val = 1 + x;
	//			emit_code("MOV %s *%s //output(L<R)", sanitize_string(resVal), sanitize_string(src.val));
	//		}
	//		if (src.rv_type == E_LVAL) { // temp_val = 1 + 1;	
	//			emit_code("MOV %s %s //output(L<L)", sanitize_string(resVal), sanitize_string(src.val));
	//		}
	//		if (src.rv_type == E_PTR) { // x = *y
	//			emit_code("MOV %s *%s //output(L<P)", sanitize_string(resVal), sanitize_string(src.val));
	//		}
	//	}
	//	if (stg.dest.rv_type == E_PTR) {
	//		if (src.rv_type == E_RVAL) {
	//			emit_code("MOV %s &%s //output(P<R)", sanitize_string(resVal), sanitize_string(src.val));
	//		}
	//		if (src.rv_type == E_LVAL) {
	//			error("can't take address of a literal");
	//		}
	//		if (src.rv_type == E_PTR) { // temp_ptr = temp_ptr
	//			emit_code("MOV %s %s //output(P<P)", sanitize_string(resVal), sanitize_string(src.val));
	//		}
	//	}
	//}
	
}

//void output_res_old(expr_settings stg, const char* res_val, struct type_name* T) {
//	if (stg.res_type == E_ERROR) { error("internal semantic error: expr_settings null"); }
//	if (stg.res_type == E_DISCARD) {
//		if (stg.res_out) { *(stg.res_out) = 0; }
//		if (stg.res_out_type) { *(stg.res_out_type) = 0; }
//		return;
//	}
//	if (!stg.res_out) { error("internal semantic error: res_out null"); }
//	if (!stg.res_out_type) { error("internal semantic error: res_out_type null"); }
//
//	*(stg.res_out) = res_val;
//	*(stg.res_out_type) = T;
//	//we also need to check if it's rval/lval
//	//and also if we need to emit anything or not.
//}

//void output_res(expr_settings stg, const char* res_val, struct type_name *T) {
//	output_res_helper(stg, res_val, T, 0);
//}
//
//void output_res_and_emit(expr_settings stg, const char* res_val, struct type_name* T) {
//	output_res_helper(stg, res_val, T, 1);
//}

void semantic_analyze_decl_stmt_list(ast_node *node){
	//decl_stmt_list:	decl_stmt_list_ne | ;
	//decl_stmt_list_ne: decl_stmt_list_ne decl_stmt | decl_stmt;
	//...
	//linearized: decl_stmt_list: decl_stmt ... decl_stmt ;

	for(int i = 0; i < node->children.size; i++){
		semantic_general_analyze(ast_get_child(node, i)); //decl_stmt
	}

}

void semantic_analyze_decl_stmt(ast_node *node){
	//decl_stmt:		class_def | func_def | var_decl ';' ;
	//push_expr("DISCARD");
	semantic_general_analyze(ast_get_child(node,0)); //class_def | func_def | var_decl
}

void semantic_analyze_func_def(ast_node *node){
	//func_def	:	typename ID '('	var_decl_list ')' stmt_list END ;
	
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
	
	struct type_name *T = parseTypename(node_typename);
	const char *name = node->token.value;
	if(semantic_decl){
		//27.03.2022: make sure the symbol is not 'already declared'
		struct symbol* Sprev = try_lookup_symbol(name);
		if (Sprev) {
			error("semantic error: symbol [%s] already declared", name);
		}

		struct symbol *S = symbol_new0();
		S->type = SYMBOL_FUNCTION;
		S->storage = STORE_CODE;
		S->username = name;
		if(semantic_flatten){
			vector2_char vstr = vector2_char_here();
			vec_printf(&vstr, "_%s", sanitize_string(name));
			S->IR_name = IR_next_name(namespace_semantic, vstr.data);//buff);
		}else{
			S->IR_name = name;
		}
		//get arguments
		S->symfunction.returntype = T;
		push_symbol_table();
		new_symbol_table(node);

			//fancy symtable name
			vector2_char vstr = vector2_char_here();
			vec_printf(&vstr, "%s_func_%s",
				sanitize_string(currentSymbolTable->name),
				sanitize_string(name));
			currentSymbolTable->name = stralloc(vstr.data);

		S->symfunction.scope = currentSymbolTable;
		S->symfunction.code = 0;
		if(symbolThis){
			//add param: this;
			struct type_name *T = semantic_get_type(symbolThis->username);
			T->pointerlevel = 1;
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
			S->symvariable.array = 0;
			S->symvariable.arraysize = 0;
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
		m((*(signature->args)), push_back, T);

		for(i = 0; i < /*list*/node_var_decl_list->children.size; i++){
			struct ast_node *arg = ast_get_child(node_var_decl_list,i);
			//struct type_name *T2 = parseTypename(node_typename);
			//m((*(signature->args)),push_back,T2);
			semantic_general_analyze(arg); //var_decl
		}
		S->symfunction.signature = signature;
			//join symbol table to definition
		semantic_context = SEMANTIC_NORMAL;
		//do not get code until imperative pass
		pop_symbol_table();
		push_symbol(S);
		push_symbol_table();
		currentSymbolTable = S->symfunction.scope;
		analyze_scope(node_stmt_list,0,0,&currentSymbolTable,0,0);
		pop_symbol_table();
	}else{
		//printf("semantic_analyze (imperative) of func_def %s\n",name);
		struct symbol *S = lookup_symbol(name);
		if(S->type != SYMBOL_FUNCTION){error("semantic: '%s' is not a function\n",S->username);}
		
		
		const char *s_typename = get_source_text2(node_typename->token.pos);
		const char *s_args = get_source_text2(node_var_decl_list->token.pos);
		
		emit_code("COMMENT SOURCE \"%s %s(%s)\" // semantic_analyze.c:480 ",//"/* %s %s(%s) */", 
		sanitize_string(s_typename), sanitize_string(name), sanitize_string(s_args));
		
		push_symbol_table();
		currentSymbolTable = S->symfunction.scope;
		//the analyze_scope will handle symbols (they are inside a frame anyway)
	
		emit_code("FUNCTION %s BEGIN", sanitize_string(S->IR_name));			
		push_semantic_context();
		
		pop_symbol_table();
		semantic_context = SEMANTIC_NORMAL;
		printf("analyze statement list...\n");
		analyze_scope(node_stmt_list,0,
						&S->symfunction.code,
						&S->symfunction.scope,
						0,0);
		printf("st.list done\n");
		semantic_this = 0;
		pop_semantic_context();
		emit_code_segment(S->symfunction.code);
		emit_code("RET");
		emit_all_undeclarations();
		emit_code("FUNCTION %s END", sanitize_string(S->IR_name));
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
		//push_expr(new_rval());//push_expr("NODISCARD");
		semantic_general_analyze(ast_get_child(node, i)); //var_decl
	}
}

void semantic_analyze_var_decl(ast_node *node){
	//var_decl:	typename ID			<<< this one
	//| typename ID '[' expr ']'    <<< or this one
	//| typename ID '=' expr		x	not this one
	//;

	if(!semantic_decl){return;}//goto semantic_exit;}
	//printf("got var_decl\n");
	struct type_name *T = parseTypename(ast_get_child(node,0));//semantic_get_type(ast_get_child(node,0)->token.value);
	const char *name = node->token.value;
	int array = 0;
	int arraysize = 0;
	if(node->token.production == 1){
		array = 1;
		struct ast_node *Nsize = ast_get_child(node,2);
		if ((strcmp(Nsize->token.type, "expr_const") != 0) || (!atoi(Nsize->token.value))){
			error("semantic error: array size has to be positive integer");
		}
		arraysize = atoi(Nsize->token.value);
	}
	struct symbol *S;

	S = symbol_new0();
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
	}
	else if (semantic_context == SEMANTIC_MEMBER) {
		S->type = SYMBOL_MEMBER;
		//S->symvariable.pos = getNumVariables();
		S->store_adr = getNumMembers();
		S->symvariable.size = getTypeSize(T);
		S->storage = STORE_DATA_STACK;
		S->symvariable.array = 0;
	}else {
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

void semantic_analyze_var_decl_assign(ast_node *node, expr_settings stg){
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	//production | typename ID '=' expr
	//yacc refs	 |    $1    $2  $3  $4
	//children   |    (0)   (1)     (2)
	struct ast_node *node_typename = ast_get_child(node,0);
	struct ast_node *node_expr = ast_get_child(node,2);
	
	struct symbol *S;
	if(semantic_decl){
		struct type_name *T = parseTypename(node_typename);
		const char *name = node->token.value;
		S = symbol_new0();
		S->username = name;
		if(semantic_flatten){
			S->IR_name = IR_next_name(namespace_semantic,name);
		}else{
			S->IR_name = name;
		}
		S->symvariable.type = T;
		if(semantic_context == SEMANTIC_PARAM){
			S->type = SYMBOL_PARAM;
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
		}
		else if (semantic_context == SEMANTIC_MEMBER) {
			S->type = SYMBOL_MEMBER;
			S->store_adr = getNumMembers();
			S->symvariable.size = getTypeSize(T);
			S->symvariable.array = 0;
			S->storage = STORE_DATA_MEMBER;
		}else{
			S->type = SYMBOL_VARIABLE;
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
			//const char *result1 = S->IR_name; struct type_name* result1type = S->symvariable.type;
			val_handle res1 = { .val = S->IR_name, .T = S->symvariable.type };

			push_code_segment();
			if(!currentSymbolTable->parent){
				currentCodeSegment = init_CS;
			}
			
			//val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
			//expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
			//semantic_expr_analyze(node_expr, stg2); //expr (what to assign)
			PREP_RES(res2, E_LVAL);
			res2stg.dest.author = "var_decl_asign expr (= ...)";
			semantic_expr_analyze(node_expr, res2stg);
			VERIFY_RES(res2);

			emit_code("MOV %s %s", sanitize_string(res1.val), sanitize_string(res2.val));
			//if (stg.res_type == E_ERROR) {stg.res_type = E_DISCARD;}
			//output_res(stg, result1, result1type);
			res2.author = "var_decl_assign";
			output_res(stg, res2, YES_EMIT);
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
		semantic_general_analyze(ast_get_child(node, i)); //stmt
	}
}

void semantic_analyze_stmt(ast_node *node){
	//stmt	:	imp_stmt	|	decl_stmt	;
	semantic_general_analyze(ast_get_child(node,0)); // imp_stmt | decl_stmt
}

void semantic_analyze_imp_stmt(ast_node *node){
	//imp_stmt:		if_block
	//| while_loop
	//| expr ';'
	//| RETURN ';'
	//| RETURN expr ';'
	//| for_loop
	//;

	////imperative pass: 
	//some things are now both declarative and imperative, oddly.
	switch(node->token.production){
		case(0)://if_block
			semantic_general_analyze(ast_get_child(node,0)); //if_block
			break;
		case(1)://while_loop
			semantic_general_analyze(ast_get_child(node,0)); //while_loop
			break;
		case(2)://expr
			if(semantic_decl){return;}//this one still imp-only though
			const char *str = removeComments(get_source_text2(node->token.pos));
			if (!is_sanitary(str)) {str = "expression";}
			emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:711 ",//"/* %s */", 
				str);
			semantic_general_analyze(ast_get_child(node,0)); //expr (unusued, increment or function call)
			break;
		case(3)://return
			if(semantic_decl){return;}
			emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:717 ",//"/* %s */",
				sanitize_string(removeComments(get_source_text2(node->token.pos))));
			emit_code("RET");
			break;
		case(4)://return expr
			if(semantic_decl){return;}
			emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:722 ", //"/* %s */",
				sanitize_string(removeComments(get_source_text2(node->token.pos))));
			PREP_RES(res1, E_RVAL);
			res1stg.dest.author = "return expr";
			semantic_expr_analyze(ast_get_child(node,0), res1stg); //expr (what to return)
			VERIFY_RES(res1);

			emit_code("RET %s", sanitize_string(res1.val));
			break;
		case(5)://for_loop
			semantic_general_analyze(ast_get_child(node,0)); //for_loop
			break;
		default:
			error("semantic error: unknown switch case");
			break;
	}
}

void semantic_analyze_if_block(ast_node *node){
	//if_block	:	if_then END |	if_then ELSE stmt_list END ;

	const char* if_exit = 0;
	const char* if_else = 0;
	if(semantic_decl){
		push_symbol_table();
		switch(node->token.production){
			case(0)://if_then END
			{
				if_settings stg1 = { .exit_label = 0, .else_label = 0 };
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				break;
			}
			case(1)://if_then ELSE stmt_list END
			{
				if_settings stg1 = { .exit_label = 0, .else_label =0 };
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				new_symbol_table(ast_get_child(node, 1));
				analyze_scope(ast_get_child(node, 1), 0, 0, &currentSymbolTable, 0, 1);
				break;
			}
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
			{
				if_exit = emit_push_label("lbl_if_exit");
				//push_expr("NOELSE");
				if_settings stg1 = { 
					.exit_label = if_exit, 
					//.else_label = 0, 
					//.out_exit_label = &if_exit, 
					//.out_else_label = &if_else 
				};
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				//if_else = pop_expr(); assert_is_if_else(if_else);
				//if_exit = pop_expr(); assert_is_if_exit(if_exit);
				emit_code("LABEL %s", sanitize_string(if_exit));
				emit_code("COMMENT SOURCE \"end if\" // semantic_analyze.c:788 ");//"/* end if */");
				break;
			}
			case(1)://if_then ELSE stmt_list END
			{
				if_exit = emit_push_label("lbl_if_exit");
				if_else = emit_push_label("lbl_if_else");

				if_settings stg1 = { 
					.exit_label = if_exit, 
					.else_label = if_else, 
					//.out_exit_label = &if_exit, 
					//.out_else_label = &if_else 
				};
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				emit_code("COMMENT SOURCE \"else\" // semantic_analyze.c:803 ");//"/* else */");
				emit_code("LABEL %s", sanitize_string(if_else));
				struct code_segment* CSinsert;
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 1));
				analyze_scope(ast_get_child(node, 1), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_else = pop_expr();	assert_is_if_else(if_else);
				//if_exit = pop_expr();	assert_is_if_exit(if_exit);
				emit_code("LABEL %s", sanitize_string(if_exit));
				emit_code("COMMENT SOURCE \"end if\" // semantic_analyze.c:812 ");//"/* end if */");
				break;
			}
			default:
				error("semantic error: unknown switch case");
				break;
		}
		pop_symbol_table();
	}
}

void semantic_analyze_if_then(ast_node *node, if_settings stg){
	//if_then:	IF '(' expr ')' stmt_list
	//| if_then ELSEIF '(' expr ')' stmt_list
	//;

	const char* if_exit = 0;
	const char* if_else = 0;
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
				if_settings stg1 = { .exit_label = if_exit, .else_label = if_else, .out_exit_label = &if_exit, .out_else_label = &if_else };
				semantic_if_analyze(ast_get_child(node,0), stg1); //if_then
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

		//if_else = pop_expr();	assert_is_if_else(if_else);
		//if_exit = pop_expr();	assert_is_if_exit(if_exit);
		if_else = stg.else_label;
		if_exit = stg.exit_label;
		const char* nextLabel = if_else;
		if (!if_else) {nextLabel = if_exit;}//if (strcmp(if_else, "NOELSE") == 0) { nextLabel = if_exit; }
		else { nextLabel = if_else; }

		switch(node->token.production){
			case(0): 
			{
				//IF ( expr ) stmt_list
				//(expr)
				pos = ast_get_child(node, 0)->token.pos;
				emit_code("COMMENT SOURCE \"if(%s)\" // semantic_analyze.c:871 ",//"/* if(%s) */", 
				sanitize_string(get_source_text2(pos)));//get_source_text(pos.start,pos.end,pos.filename));
				
				//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
				//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
				PREP_RES(res1, E_LVAL);
				res1stg.dest.author = "if_then condition";
				semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr (condition)
				VERIFY_RES(res1);
				
				const char* condition_result = res1.val;
				//if then
				emit_code("/* this skips untrue if's */ // semantic_analyze.c:883");
				emit_code("JE 0 %s %s", 
					sanitize_string(condition_result), 
					sanitize_string(nextLabel));
				emit_code("/* 'if' is true: */ // semantic_analyze.c:887");

				//stmt_list
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 1));
				struct code_segment* CSinsert;
				analyze_scope(ast_get_child(node, 1), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_exit = IR_next_name(namespace_semantic,"lbl_if_exit");
				//emit_code("SYMBOL %s LABEL",if_exit);
				if (nextLabel != if_exit) {  
					emit_code("JMP %s", sanitize_string(if_exit));
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
				//push_expr(if_exit);
				const char* if_elseif = emit_push_label("lbl_if_elseif");

				//if_then
				if_settings stg1 = { .exit_label = if_exit, .else_label = if_elseif, .out_exit_label = &if_exit, .out_else_label = &if_elseif };
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				//assert_is_if_else(pop_expr()); //discard the labels from inner if_then
				//assert_is_if_exit(pop_expr()); 
				//else if
				YYLTYPE pos = ast_get_child(node, 1)->token.pos;
				emit_code("COMMENT SOURCE \"else if(%s)\" // semantic_analyze.c:918 ", //"/* else if(%s) */", 
				sanitize_string(get_source_text2(pos)));//get_source_text(pos.start,pos.end,pos.filename));
				emit_code("LABEL %s", sanitize_string(if_elseif));
				//( expr )
				
				//val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
				//expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
				PREP_RES(res2, E_LVAL);
				res2stg.dest.author = "if_then elseif condition";
				semantic_expr_analyze(ast_get_child(node, 1), res2stg); //expr (condition)
				VERIFY_RES(res2);
				
				const char* condition_result = res2.val;
				//const char* condition_result = pop_expr(); assert_temp_val(condition_result);
				//then
				//const char* label1 = IR_next_name(namespace_semantic, "lbl_elseif_then");
				//emit_code("SYMBOL %s LABEL",label1);
				//emit_code("JE 0 %s %s", pop_expr(), label1);
				emit_code("/* this skips untrue if's */ // semantic_analyze.c:936");
				emit_code("JE 0 %s %s", 
					sanitize_string(condition_result), 
					sanitize_string(nextLabel));
				emit_code("COMMENT SOURCE \"then\" // semantic_analyze.c:940 ");//"/* then */");
				//stmt_list
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 2));
				struct code_segment* CSinsert;
				analyze_scope(ast_get_child(node, 2), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_exit = pop_expr(); //using existing exit
				if (nextLabel != if_exit) {
					emit_code("JMP %s", sanitize_string(if_exit));
				} //else, we can skip the jmp as the next instruction is already at the end of the if-block.
				//push_expr(if_exit);
				//emit_code("LABEL %s", label1);
				break;
			}
			default:
				error("semantic error: unknown switch case");
				break;
		};
		//push_expr(if_exit);
		//push_expr(if_else);
		//fprintf(stderr, "warning: if_then should probably output some stg stuff\n");
#pragma message("warning: if_then should output stg stuff\n")

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
		emit_code("COMMENT SOURCE \"while(%s)\" // semantic_analyze.c:977 ",//"/* while(%s) */",
			sanitize_string(get_source_text2(pos1)));
		const char *label1 = IR_next_name(namespace_semantic,"lbl_while_do");
		const char *label2 = IR_next_name(namespace_semantic,"lbl_while_exit");
		emit_code("SYMBOL %s LABEL", sanitize_string(label1));
		emit_code("SYMBOL %s LABEL", sanitize_string(label2));
		emit_code("LABEL %s",sanitize_string(label1));
		
		//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
		//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
		PREP_RES(res1, E_LVAL);
		res1stg.dest.author = "while condition";
		semantic_expr_analyze(ast_get_child(node,0), res1stg); //expr (condition)
		VERIFY_RES(res1);
		
		emit_code("JE 0 %s %s", 
			sanitize_string(res1.val), 
			sanitize_string(label2));
		emit_code("COMMENT SOURCE \"do\" // semantic_analyze.c:995 ");//"/* do */");
		push_symbol_table();
		currentSymbolTable = find_symbol_table_by_node(node);
		struct code_segment *CSinsert;
		analyze_scope(ast_get_child(node,1),0,&CSinsert,&currentSymbolTable,0,1);
		emit_code_segment(CSinsert);
		emit_code("JMP %s", sanitize_string(label1));
		emit_code("LABEL %s",sanitize_string(label2));
		emit_code("COMMENT SOURCE \"end while\" // semantic_analyze.c:1003 ");//"/* end while*/");
		pop_symbol_table();
	}
}

void semantic_analyze_for_loop(ast_node *node){
	//for_loop	:	FOR '(' stmt expr ';' expr ')' stmt_list END ;
	ast_node* node_init = ast_get_child(node, 0);
	ast_node* node_cond = ast_get_child(node, 1);
	ast_node* node_post = ast_get_child(node, 2);
	ast_node* node_body = ast_get_child(node, 3);
	if(semantic_decl){
		push_symbol_table();
		 new_symbol_table(node);
		 semantic_general_analyze(node_init);//(ast_get_child(node,0)); //stmt (first)
		 semantic_general_analyze(node_body);//(ast_get_child(node,3)); //stmt_list
		 analyze_scope(node_body,0,0,&currentSymbolTable,0,1);
		pop_symbol_table();
	}else{
		YYLTYPE pos1 = node_init->token.pos; //ast_get_child(node,0)->token.pos;
		YYLTYPE pos2 = node_cond->token.pos; //ast_get_child(node, 1)->token.pos;
		YYLTYPE pos3 = node_post->token.pos; //ast_get_child(node, 2)->token.pos;
		YYLTYPE pos4 = pos1;
		pos4.last_line = pos3.last_line;
		pos4.last_column = pos3.last_column;

		//comment that this is a for-loop
		emit_code("COMMENT SOURCE \"for(%s)\" // semantic_analyze.c:1030 ",//"/* for(%s) */", 
			sanitize_string(get_source_text2(pos4)));

		//1. enter the scope of the for-loop
		//1.1 get the symbol table
		push_symbol_table();
		 currentSymbolTable = find_symbol_table_by_node(node);
		//1.2 emit symbol declarations
		 struct code_segment* CSinsert;
		 analyze_scope(node_init, 0, &CSinsert, &currentSymbolTable, 0, 1);
		 semantic_decl = 1; //only capture declarations, not commands
		 analyze_scope(node_body, 0, &CSinsert, &currentSymbolTable, 0, 1);
		 semantic_decl = 0;
		 emit_code_segment(CSinsert);

		//2. process the initializer statement
		//2.1 comment that this is an initializer (stmt/expr 1)
		 emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:1047 ",//"/* %s */", 
		 	sanitize_string(removeComments(get_source_text2(pos1))));
		//2.2 emit the statement code 
		 semantic_general_analyze(node_init);

		//3. declare the jump labels that we will later use
		 const char *loopCondition = IR_next_name(namespace_semantic,"lbl_for");
		 const char *loopExit = IR_next_name(namespace_semantic,"lbl_for_exit");
		 emit_code("SYMBOL %s LABEL", sanitize_string(loopCondition));
		 emit_code("SYMBOL %s LABEL", sanitize_string(loopExit));
		
		//4. process the condition expression
		//4.1 emit the label that the loop will jump to, to begin each iteration.
		 emit_code("LABEL %s", sanitize_string(loopCondition));
		//4.2 comment that this is a condition (expr 2)
		 emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:1062 ",//"/* %s */", 
		 	sanitize_string(removeComments(get_source_text2(pos2))));
		//4.3 emit the code to calculate the condition value
		 PREP_RES(res1, E_LVAL);
		 res1stg.dest.author = "for condition";
		 semantic_expr_analyze(node_cond, res1stg); //expr (i < 10;)
		 VERIFY_RES(res1);
		 const char* condition = res1.val;
		//4.4 emit the code to skip the loop if the condition is not met
		 emit_code("JE 0 %s %s",
			sanitize_string(condition),
			sanitize_string(loopExit));

		//5. process the loop body
		//5.1 comment that this is the loop body
		 emit_code("COMMENT SOURCE \"loop body\" // semantic_analyze.c:1077 ");//"/* loop body */");
		//5.2 emit the code of the loop body statements
		 semantic_general_analyze(node_body); //stmt_list

		//6. process the post-loop expression
		//6.1 comment that this is the post-expression
		 emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:1083 ",//"/* %s */", 
		 	sanitize_string(removeComments(get_source_text2(pos3))));
		//6.2 emit the code to execute the post-expression
		 semantic_general_analyze(ast_get_child(node,2)); //expr (i++)
		
		//7. jump back to condition to begin the next iteration
		 emit_code("JMP %s", sanitize_string(loopCondition));

		//8. emit the loop exit label
		 emit_code("LABEL %s", sanitize_string(loopExit));
		
		//9. clean up
		 emit_all_deinitializers();
		
		//comment that we are done
		 emit_code("COMMENT SOURCE \"end for\" // semantic_analyze.c:1098 ");//"/* end for */");
		
		pop_symbol_table();
	}
}

void semantic_analyze_class_def(ast_node *node){
	//const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//assert_no_result(res_dest);

	//production|	CLASS ID decl_stmt_list END
	//yacc refs |		  $1  $2     $3
	//child		|		  (0) (1)
	//struct ast_node *node_id  = ast_get_child(node,0);
	struct ast_node *node_dsl = ast_get_child(node,1);
	
	if(semantic_decl){
		//printf("got class_def\n");
		const char *name = node->token.value;
		struct symbol *S = symbol_new0();
		push_symbol(S);
		S->username = name;
		S->storage = STORE_CODE;
		if(semantic_flatten){
			S->IR_name = IR_next_name(namespace_semantic,name);
		}else{
			S->IR_name = name;
		}
		S->type = SYMBOL_CLASS;
		push_symbol_table();
		new_symbol_table(node);

		//fancy symtable name
		vector2_char vstr = vector2_char_here();
		vec_printf(&vstr, "%s_class_%s",
			sanitize_string(currentSymbolTable->name),
			sanitize_string(name));
		currentSymbolTable->name = stralloc(vstr.data);

		S->symclass.scope = currentSymbolTable;
		push_semantic_context();
		semantic_context = SEMANTIC_MEMBER;

		symbolThis = S;
		semantic_this = S->IR_name;
		
		//03.04.2022 - we don't need to add "symbol this" to a struct because
		// function calls do that automatically, plus 
		// it messes up symbol lookup from within a function.
		//struct symbol *T = symbol_new0();
		//T->username = stralloc("this");
		//T->IR_name = IR_next_name(namespace_semantic,"this");
		//T->type = SYMBOL_PARAM; //wait a sec, wtf is a SYMBOL_MEMBER?
		//T->storage = STORE_DATA_STACK; //no idea if this does anything
		//T->store_adr = 0;
		//T->global = 0;
		//T->symvariable.pos = 0;
		//	struct type_name *Tn = malloc(sizeof(struct type_name));
		//	Tn->name = S->username;
		//	Tn->pointerlevel = 1;
		//	Tn->symclass = S;
		//	Tn->args = 0;
		//T->symvariable.type = Tn;
		//T->symvariable.array = 0;
		//T->symvariable.arraysize = 0;
		//
		//push_symbol(T);
		
		semantic_general_analyze(node_dsl); //decl_stmt_list
		
		pop_semantic_context();
		class_def_finalize();
		semantic_this = 0;
		symbolThis = 0;
		pop_symbol_table();
	}else{
		symbolThis = lookup_symbol(node->token.value);
		class_emit_start();
		
		push_symbol_table();
		currentSymbolTable = symbolThis->symclass.scope; 
		
		semantic_context = SEMANTIC_MEMBER;
		semantic_general_analyze(node_dsl);//this adds functions that were actually defined in code
		semantic_context = SEMANTIC_NORMAL;
		
		pop_symbol_table();
		class_emit_end();
		symbolThis = 0;
		
	}
}

#if 0
//--- random crap backup ------------

/*
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
*/

//outputs the value res_val into the destination res_dest,
//changing r-v-qualification if necessary.
//returns 1 if something was outputted and 0 otherwise.
//... note: also maybe ask before emitting random code?
//old
/*
int output_res(const char* res_dest, const char* res_val) {
	assert_expr_res(res_dest);
	assert_val(res_val);
	if (strcmp(res_dest, "DISCARD")==0) {
		return 0;
	}
	else if (is_rval(res_dest)) {
		if (is_rval(res_val)) {
			emit_code("MOV %s %s // R<-R, ref copy ", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
		else if(is_lval(res_val)){

			//for both lvals and constants
			//emit("MOV %s &%s // R<-L, promote ", res_dest, res_val);
			error("internal semantic error: cannot promote Lvalue (%s) to Rvalue", res_val);
		}
		else {
			error("internal semantic error: cannot promote I-Lvalue (%s) to Rvalue", res_val);
		}
	}
	else if (is_lval(res_dest)) {
		if (is_rval(res_val)) {
			emit_code("MOV %s *%s // L<-R, demote/deref ", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
		else if (is_lval(res_val)) {
			emit_code("MOV %s %s // L<-L, val copy ", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
		else{
			//probably a costant
			emit_code("MOV %s %s // L<-I, load immediate ", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
	}
	else {
		error("internal semantic error: can only write to rvals and lvals");
	}
	return 0; //unreachable
}
*/
#endif
