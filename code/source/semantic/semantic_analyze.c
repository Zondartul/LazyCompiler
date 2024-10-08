#include "semantic.h"
#include "yaccin.tab.h" //for get_source_text2()
#include "ctype.h"
#include "semantic_analyze.h"
#include "semantic_expr_op.h"
#include "ast_gen.h"
#include <assert.h>
#include "typecheck.h"
#include "asm_template.h"

void semantic_analyze_program(ast_node *node){
	//program :	decl_stmt_list	;
	
	//printf("got program\n");
	new_symbol_table(node);
	new_code_segment();
	push_code_segment();
	new_code_segment();
	init_CS = currentCodeSegment;
	pop_code_segment();
	global_CS = currentCodeSegment;
	semantic_decl = 1;
	analyze_scope(ast_get_child(node,0),&currentCodeSegment,0,&currentSymbolTable,0,0);
	semantic_decl = 0;
	need_global_initializers = 1;
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
	emit_code("SYMBOL %s LABEL // semantic_analyze.c:56", sanitize_string(lbl2));
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
	*   RVAL - "R means read" - value at the right (->) hand side of an expression, not write-able
	*   LVAL - "L means write" - value at the left (<-) hand side of an expression, write-able and has a memory location
	*   PTR - "pointer" - temporary value to calculate the address of something, will turn into RVAL or LVAL by the time user sees it.
	*			automatically dereferenced when converting to RVAL or LVAL.
	*	FPTR - "function pointer" - special type of PTR for pointers to functions. Cannot be referenced or dereferenced, only called.
	*	ASIS - "return the source value as-is" - pass-through of the unmodified source vale, preserving rv-type.
	*   
	*  OUTPUT RES RULES
	*   parent node analyzer requests a value of specific type,
	*   and the current node analyzer MUST return a node of the same type.
	*	Some expressions want values without decorators (&*), in that case, copy is used.   
	*
	*  HOW IT WORKS:
	*	Semantic analyze of parent node -->calls-> semantic analyze of child node
	*		(stg.actual = &out_res)	 \									|
	*		    ^				 	  v									v
	*			|	"i want to get a value of this rv-type"			locates or generates a value
	*			|										\			 /<- "i have a value of this rv-type"
	*		 *out_res = result							|			/		 	
	*	"result value has 								v          v
	*		this name and rv_type"	<-------output_res(stg.dest, src)			
	*					  						|
	*											v
	*									 [maybe rename IR value (book-keeping)]
	*									 [maybe copy IR value to temporary (code-gen)]
	*									 [probably change rv_type (book-keeping)]
	*/

	/// i forgot what all of this does so new idea:
	/// R=read, L=write, Ptr = &L
	///	also assume that we want unadorned value at output (no *&)
	/// Rvalue - on right side ->
	//		rvalue - you can read it
	//		rvalue - you can do arithmetic (get rvalue)
	//		rvalue - you can deref (*this) - get lvalue
	//			r  <-  r(x) // pass
	//			l   x  r(x) // forbidden
	//			f  <-  r(x) // pass
	//			a  <-  r(x) // pass
	//			p  <r&- r(x) // rename-and (for &(arr+i))... but not if it's a function?
	//			 
	//			r(*x) //nope, that's L
	//			r(&x) //nope, that's P
	//  
	//  Lvalue - <- on the left side
	//		lvalue - you can read it (x = this)
	//		lvalue - you can write it (this = x)
	//		lvalue - you can take it's address (&this) - result is ptr
	//		lvalue - you can deref (*this) - get lvalue
	//			r	<-	l(x)  //pass
	//			r	<c-	l(*x) //copy
	//			l	<-	l(x)  //pass
	//			*l	<-	l(*x) //pass (have to output *)
	//			&p  <r&-	l(x)  //rename-and
	//			p  <r-*	l(*x) //rename-unstar
	//			f   x   l(x)  //forbidden (idk, can be pass i guess)
	//			a  <-   l(x)  //pass
	//			&a  <-  l(&x)  //pass
	//					l(&x) //nope, that's P
	//			
	//  Ptr - you need to add * to get Lvalue
	//		ptr - you can do arithmetic to get another ptr
	//		ptr - you can deref (*this) - get lvalue
	//
	//			*r	<r*-	p(x)  //rename-star
	//			r	<c-		p(&x) //copy
	//			*l	<r*-	p(x)  //rename-star
	//			l	<r-&-	p(&x)	//un-and
	//			p	<-	p(x)	//pass
	//			p	<c-	p(&x)	//copy
	//			f   <-  p(x)    //pass
	//			f   <c- p(&x)	//copy
	//			a   <-  p(x)    //pass
	//			&a   <-  p(&x)	//pass
	//  
	//		 	p(*x) //nope, that's L
	//
	//  Fptr - function pointer that should not be referenced or dereferenced
	//    	Fptr - you can call it
	//		Fptr - you can read it (the address, no the memory)
	//	
	//			r	<-	f(x)	//pass
	//			r	<c-  f(*x)	//copy
	//			l	x	f(x)	//forbidden
	//			p	<-  f(x)	//pass
	//			p   <c- f(*x)	//copy
	//			a	<-	f(x)	//pass
	//			a	<-  f(*x)	//copy		
	//
	//			f(&x) //nope, that's P
	//
	//	Asis - preserve and pass through the rv type
	//		Asis - destination becomes the source type
	//		Asis - cannot be the source type
	//			
	//			a	<- x// asis-pass
	//			&a	<- &x// asis-pass
	//			*a	<- *x// asis-pass

	if (stg.dest.rv_type == E_ERROR) { error("internal semantic error: expr_settings null"); }
	if (stg.dest.rv_type == E_DISCARD) {
		if (stg.actual) {*(stg.actual) = (val_handle){ .val = 0 };}
		return;
	}
	VERIFY_RES(src);

	
	expr_type out_rv_type = stg.dest.rv_type;

	int dest_lval = (stg.dest.rv_type == E_LVAL);
	int dest_rval = (stg.dest.rv_type == E_RVAL);
	int dest_ptr = (stg.dest.rv_type == E_PTR);
	int dest_fptr = (stg.dest.rv_type == E_FPTR);
	int dest_asis = (stg.dest.rv_type == E_ASIS);
	int src_lval = (src.rv_type == E_LVAL);
	int src_rval = (src.rv_type == E_RVAL);
	int src_ptr = (src.rv_type == E_PTR);
	int src_fptr = (src.rv_type == E_FPTR);
	int src_asis = (src.rv_type == E_ASIS);
	int acc_star = (src.val[0] == '*');
	int acc_and = (src.val[0] == '&');
	int acc_none = (!acc_star) && (!acc_and);

	const char* dest_rv_str = 0;
	if (dest_lval) { dest_rv_str = "L"; }
	if (dest_rval) { dest_rv_str = "R"; }
	if (dest_ptr)  { dest_rv_str = "P";	}
	if (dest_fptr) { dest_rv_str = "F"; }
	if (dest_asis) { dest_rv_str = "A";}
	const char* src_rv_str = 0;
	if (src_lval) { src_rv_str = "L"; }
	if (src_rval) { src_rv_str = "R"; }
	if (src_ptr)  { src_rv_str = "P"; }
	if (src_fptr) { src_rv_str = "F"; }
	if (src_asis) { src_rv_str = "A"; }

	assert(src.val);
	assert(strlen(src.val));

	//verify that the value we are outputting is initialized and stuff
	if ((src.rv_type == E_ERROR)) {
		error("internal semantic error: invalid source value");
	}
	if ((src.rv_type == E_ASIS)){
		error("internal semantic error: source type can't be 'as-is'");
		// only dest type can be as-is
		// it is up to the analyzed expression that outputs a value (our caller) 
		// to tell output_res what the source type is.
	}
	printf("output_res (%s)\n", src.val);

	//const char* resVal = stg.dest.val; //we may do a copy if the destination name is requested
	//if (!resVal) { resVal = src.val; }
	//int resValTemp = 0;
	const char* resVal = 0;
	//full emit string will be stored here
	vector2_char vstr = vector2_char_here();

	const char* op_str;

	/// RVAL LVAL conversion
	/// ---------- from RVAL
	///     to RVAL <-
	if (dest_rval && src_rval && acc_none)	{goto out_pass;}
	if (dest_rval && src_rval && acc_star)	{goto out_nope_is_L;}
	if (dest_rval && src_rval && acc_and)	{goto out_nope_is_P;}
	///		to LVAL <-
	if (dest_lval && src_rval && acc_none)	{goto out_forbidden;}
	if (dest_lval && src_rval && acc_star)	{goto out_nope_is_L;;}
	if (dest_lval && src_rval && acc_and)	{goto out_nope_is_P;}
	///		to PTR  <-
	if (dest_ptr  && src_rval && acc_none)	{goto out_rename_and;}//{goto out_pass;}
	if (dest_ptr  && src_rval && acc_star)	{goto out_nope_is_L;}
	if (dest_ptr  && src_rval && acc_and)	{goto out_nope_is_P;}
	//		to FPTR <-
	if (dest_fptr  && src_rval && acc_none)	{goto out_pass;}//{goto out_pass;}
	if (dest_fptr  && src_rval && acc_star)	{goto out_nope_is_L;}
	if (dest_fptr  && src_rval && acc_and)	{goto out_nope_is_P;}
	//		to ASIS <-
	if (dest_asis  && src_rval && acc_none) {goto out_asis_pass;}
	if (dest_asis  && src_rval && acc_star) {goto out_nope_is_L;}
	if (dest_asis  && src_rval && acc_and)  {goto out_nope_is_P;}

	/// ---------- from LVAL
	///     to RVAL <-
	if (dest_rval && src_lval && acc_none)	{goto out_pass;}
	if (dest_rval && src_lval && acc_star)	{goto out_copy;}
	if (dest_rval && src_lval && acc_and)	{goto out_nope_is_P;}
	///		to LVAL <-
	if (dest_lval && src_lval && acc_none)	{goto out_pass;}
	if (dest_lval && src_lval && acc_star)	{goto out_pass;} // *x is L, and we have to keep it that way to get another L
	if (dest_lval && src_lval && acc_and)	{goto out_nope_is_P;}
	///		to PTR  <-
	if (dest_ptr  && src_lval && acc_none)	{goto out_rename_and;}
	if (dest_ptr  && src_lval && acc_star)	{goto out_rename_unand;}
	if (dest_ptr  && src_lval && acc_and)	{goto out_nope_is_P;}
		//		to FPTR <-
	if (dest_fptr  && src_lval && acc_none)	{goto out_forbidden;}//{goto out_pass;}
	if (dest_fptr  && src_lval && acc_star)	{goto out_forbidden;}
	if (dest_fptr  && src_lval && acc_and)	{goto out_nope_is_P;}
	//		to ASIS <-
	if (dest_asis  && src_lval && acc_none) {goto out_asis_pass;}
	if (dest_asis  && src_lval && acc_star) {goto out_asis_pass;}
	if (dest_asis  && src_lval && acc_and)  {goto out_nope_is_P;}

	/// ---------- from PTR
	///     to RVAL <-
	//if (dest_rval && src_ptr  && acc_none && is_func)	{goto out_pass;}
	if (dest_rval && src_ptr  && acc_none)	{goto out_rename_star;}
	if (dest_rval && src_ptr  && acc_star)	{goto out_nope_is_L;}
	if (dest_rval && src_ptr  && acc_and)	{goto out_copy;}
	///		to LVAL <-
	if (dest_lval && src_ptr  && acc_none)	{goto out_rename_star;}
	if (dest_lval && src_ptr  && acc_star)	{goto out_nope_is_L;}
	if (dest_lval && src_ptr  && acc_and)	{goto out_rename_unand;}
	///		to PTR  <-
	if (dest_ptr  && src_ptr  && acc_none)	{goto out_pass;}
	if (dest_ptr  && src_ptr  && acc_star)	{goto out_nope_is_L;}
	if (dest_ptr  && src_ptr  && acc_and)	{goto out_copy;}
	//		to FPTR <-
	if (dest_fptr  && src_ptr && acc_none)	{goto out_pass;}//{goto out_pass;}
	if (dest_fptr  && src_ptr && acc_star)	{goto out_nope_is_L;}
	if (dest_fptr  && src_ptr && acc_and)	{goto out_copy;}
	//		to ASIS <-
	if (dest_asis  && src_ptr && acc_none) {goto out_asis_pass;}
	if (dest_asis  && src_ptr && acc_star) {goto out_nope_is_L;}
	if (dest_asis  && src_ptr && acc_and)  {goto out_asis_pass;}
	/// ---------- from FPTR
	//		r	<-	f(x)	//pass
	//		r	<c-  f(*x)	//copy
	//		l	x	f(x)	//forbidden
	//		p	<-  f(x)	//pass
	//		p   <c- f(*x)	//copy
	//		f   <-  f(x)	//pass
	//		f	<c- f(*x)	//copy
	//		a	<-	f(x)	//pass
	//		a	<-  f(*x)	//copy	
	//				f(&x) //nope, that's P
	///     to RVAL <-
	if (dest_rval && src_fptr  && acc_none)	{goto out_pass;}
	if (dest_rval && src_fptr  && acc_star)	{goto out_copy;}
	if (dest_rval && src_fptr  && acc_and)	{goto out_nope_is_P;}
	///		to LVAL <-
	if (dest_lval && src_fptr  && acc_none)	{goto out_forbidden;}
	if (dest_lval && src_fptr  && acc_star)	{goto out_forbidden;}
	if (dest_lval && src_fptr  && acc_and)	{goto out_nope_is_P;}
	///		to PTR  <-
	if (dest_ptr  && src_fptr  && acc_none)	{goto out_pass;}
	if (dest_ptr  && src_fptr  && acc_star)	{goto out_copy;}
	if (dest_ptr  && src_fptr  && acc_and)	{goto out_nope_is_P;}
	//		to FPTR <-
	if (dest_fptr  && src_fptr && acc_none)	{goto out_pass;}
	if (dest_fptr  && src_fptr && acc_star)	{goto out_copy;}
	if (dest_fptr  && src_fptr && acc_and)	{goto out_nope_is_P;}
	//		to ASIS <-
	if (dest_asis  && src_fptr && acc_none) {goto out_asis_pass;}
	if (dest_asis  && src_fptr && acc_star) {goto out_asis_pass;}
	if (dest_asis  && src_fptr && acc_and)  {goto out_nope_is_P;}
	

	goto out_unexpected;
out_unexpected:
	error("output_res: unexpected case");
	return;

out_forbidden:
	error("output_res: conversion %s <- %s is forbidden",dest_rv_str, src_rv_str);
	return;

out_nope_is_L:
	error("output_res: value %s is not a %s but an LVALUE (L=write)",src.val,src_rv_str);
	return;

out_nope_is_P:
	error("output_res: value %s is not a %s but an PTR (need *)",src.val,src_rv_str);

out_pass:
	op_str = "pass";
	resVal = src.val;
	goto out_finish;

out_copy:
	op_str = "copy";
	resVal = IR_next_name(namespace_semantic, "temp");
	vec_printf(&vstr, "MOV %s %s", sanitize_string(resVal), sanitize_string(src.val));
	goto out_finish;

/*
out_rename_unstar:
	op_str = "rename_unstar";
	resVal = rename_unstar(src.val);
	goto out_finish;
*/

out_rename_unand:
	op_str = "rename_unand";
	resVal = rename_unand(src.val);
	goto out_finish;

out_rename_and:
	op_str = "rename_and";
	resVal = rename_and(src.val);
	goto out_finish;

/*
out_copy_and_rename_star:
	op_str = "copy_and_rename_star";
	resVal = IR_next_name(namespace_semantic, "temp");
	vec_printf(&vstr, "MOV %s %s", sanitize_string(resVal), sanitize_string(src.val));
	resVal = rename_star(resVal);
	goto out_finish;
*/

/*
out_rename_star_and_copy:
	op_str = "rename_star_and_copy";
	resVal = IR_next_name(namespace_semantic, "temp");
	vec_printf(&vstr, "MOV %s *%s", sanitize_string(resVal), sanitize_string(src.val));
	goto out_finish;
*/

out_rename_star:
	op_str = "rename_star";
	resVal = rename_star(src.val);
	goto out_finish;

out_asis_pass:
	op_str = "as-is_pass";
	resVal = src.val;
	out_rv_type = src.rv_type;
	goto out_finish;
	
out_finish:
	
	const char* dest_val = resVal;
	const char* src_val = src.val;
	const char* dest_auth = stg.dest.author;
	const char* src_auth = src.author;
	assert(dest_auth);
	assert(src_auth);

	vec_printf(&vstr, "   /* out(%s <- %s): %s from %s (%s) //auth: to %s from %s */", 
		dest_rv_str, src_rv_str,
		dest_val, src_val,
		op_str,
		dest_auth, src_auth
	);

	emit_code(vstr.data);

	if (stg.actual) {
		// not sure what are we doing here. we want to tell the caller
		// what actually was in the value we were given?
		// or do we wanna tell it what we ended up with?
		
		*(stg.actual) = src;
		stg.actual->rv_type = out_rv_type;//stg.dest.rv_type;
		stg.actual->val = resVal;
		VERIFY_RES((*(stg.actual)));
	}
	
}


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
	
	struct type_name *retT/*T*/ = parseTypename(node_typename);
	const char *name = node->token.value;

	//if(strcmp(name, "ba_malloc")==0){
	//	printf("debug breakpoint\n");
	//}

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
		//S->symfunction.returntype = T;
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
		semantic_context = SEMANTIC_PARAM;
			//use new separate symbol table;
		struct type_name *signature = type_name_new0();//malloc(sizeof(struct type_name));
		//signature->name = 0;
		//signature->symclass = 0;
		signature->args = vector2_ptr_type_name_new();
		//parameter as visible from scope VS signature parameter, is confusing.
		int i;
		//wait, shouldn't we go through var_decl_list_ne first?
		//no, it's linearized now
		m(*signature->args, push_back, retT); /// the return type is stored as the 'first argument' in signature.

		for(i = 0; i < /*list*/node_var_decl_list->children.size; i++){
			struct ast_node *arg = ast_get_child(node_var_decl_list,i);
			//struct type_name *T2 = parseTypename(node_typename);
			//m((*(signature->args)),push_back,T2);
			semantic_general_analyze(arg); //var_decl
		}
		/// write down the argument types in the signature
		for(i = 0; i < S->symfunction.scope->symbols.size; i++){
			struct symbol *S2 = m(S->symfunction.scope->symbols, get, i);
			if(S2->type == SYMBOL_PARAM){
				assert(S2->symvariable.type);
				m(*signature->args, push_back, S2->symvariable.type);
			}
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
		
		emit_code("COMMENT SOURCE \"%s %s(%s)\" // semantic_analyze.c:603 ",//"/* %s %s(%s) */", 
		sanitize_string(s_typename), sanitize_string(name), sanitize_string(s_args));
		
		push_symbol_table();
		currentSymbolTable = S->symfunction.scope;
		//the analyze_scope will handle symbols (they are inside a frame anyway)
	
		emit_code("FUNCTION %s BEGIN // semantic_analyze.c:610", sanitize_string(S->IR_name));			
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
		emit_code("RET // semantic_analyze.c:624");
		emit_all_undeclarations();
		emit_code("FUNCTION %s END // semantic_analyze.c:627", sanitize_string(S->IR_name));
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
	if(array){
		T->is_array = 1;
		T->arraysize = arraysize;
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
		//S->symvariable.array = 0;
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
		//S->symvariable.array = 0;
		S->init_expr = 0;
	}else {
		S->type = SYMBOL_VARIABLE;
		//S->symvariable.pos = getNumVariables();
		S->store_adr = getNumVariables();
		S->symvariable.size = getTypeSize(T);
		S->storage = STORE_DATA_STACK;
		//S->symvariable.array = 0;
	}
	//if(array){S->symvariable.array = 1; S->symvariable.type->is_array = 1; S->symvariable.type->arraysize = arraysize;}
	if(!currentSymbolTable->parent){S->global = 1;}
	push_symbol(S);
	//put into symbol table
}

void semantic_analyze_var_decl_assign(ast_node *node, expr_settings){
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
			//S->symvariable.array = 0;
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
			//S->symvariable.array = 0;
			S->storage = STORE_DATA_MEMBER;
			S->init_expr = node_expr;
		}else{
			S->type = SYMBOL_VARIABLE;
			S->store_adr = getNumVariables();
			S->symvariable.size = getTypeSize(T);
			//S->symvariable.array = 0;
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
			//val_handle res1 = { .val = S->IR_name, .T = S->symvariable.type };

			push_code_segment();
			if(!currentSymbolTable->parent){
				currentCodeSegment = init_CS;
			}
			/*
			PREP_RES(res2, E_RVAL); // R=read, L=write //E_LVAL);
			res2stg.dest.author = "var_decl_asign expr (= ...)";
			semantic_expr_analyze(node_expr, res2stg);
			VERIFY_RES(res2);

			emit_code("MOV %s %s // semantic_analyze.c:792", sanitize_string(res1.val), sanitize_string(res2.val));
			
			res2.author = "var_decl_assign";
			output_res(stg, res2, YES_EMIT);
			*/

			struct ast_node *node_dest = ast_gen_id(S->username);
			struct ast_node *node_src = node_expr;
			struct ast_node *node_assign = ast_gen_assign(node_dest, node_src);

			PREP_RES(res2, E_DISCARD);
			res2stg.dest.author = "var_decl_assign";
			semantic_expr_analyze(node_assign, res2stg);

			pop_code_segment();
		}
	}
}


void semantic_analyze_var_decl_constructor(ast_node *node){//, expr_settings stg){
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	//production | typename ID '(' expr_list ')'
	//yacc refs	 |    $1    $2  $3  $4
	//children   |    (0)   (1)     (2)
	struct ast_node *node_typename = ast_get_child(node,0);
	struct ast_node *node_expr_list = ast_get_child(node,2);
	
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
		if(semantic_context == SEMANTIC_PARAM){error("Semantic error: can't use constructors in function parameter declarations");}

		else if (semantic_context == SEMANTIC_MEMBER) { /// i wonder what would happen if you tried to put a constructor call in the declaration of a class member
			S->type = SYMBOL_MEMBER;
			S->store_adr = getNumMembers();
			S->symvariable.size = getTypeSize(T);
			//S->symvariable.array = 0;
			S->storage = STORE_DATA_MEMBER;
			S->init_expr = ast_gen_constructor_call(name, node_expr_list);
		}else{
			S->type = SYMBOL_VARIABLE;
			S->store_adr = getNumVariables();
			S->symvariable.size = getTypeSize(T);
			//S->symvariable.array = 0;
			S->storage = STORE_DATA_STACK;
			S->init_expr = ast_gen_constructor_call(name, node_expr_list);
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
			//val_handle res1 = { .val = S->IR_name, .T = S->symvariable.type };

			push_code_segment();
			if(!currentSymbolTable->parent){
				currentCodeSegment = init_CS;
			}
			
			PREP_RES(res2, E_DISCARD); // R=read, L=write //E_LVAL);
			res2stg.dest.author = "var_decl_constructor call obj.c(...)";
			semantic_expr_analyze(S->init_expr, res2stg);

			pop_code_segment();
		}
	}
}

void semantic_analyze_var_decl_varargs(ast_node* /*node*/){
	//var_decl:
	//| VARARGS						<<< this one
	//;

	if(!semantic_decl){return;}

	struct symbol *S;

	S = symbol_new0();
	S->username = "varargs";
	if(semantic_flatten){
		S->IR_name = IR_next_name(namespace_semantic,"varargs");
	}else{
		S->IR_name = "varargs";
	}
	struct type_name *T = gen_type_name_void_ptr();
	T->name = "varargs";
	S->symvariable.type = T;
	if(semantic_context == SEMANTIC_PARAM){
		S->type = SYMBOL_PARAM;
		S->store_adr = getNumParameters();
		S->symvariable.size = getTypeSize(T);
		S->storage = STORE_DATA_STACK;
	}
	push_symbol(S);
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


void sanitize_force(char *buff){
	int L = strlen(buff);
	for(int i = 0; i < L; i++){
		if(!isprint(buff[i])){
			buff[i] = '~';
		}
	}
}
/// use this as backup if source text lookup is broken
const char *reconstruct_node_string(ast_node *node){
	int rem = 80; // remaining space for chars
	char *buff = (char*)malloc(rem);
	const char *prefix = 0;
	const char *postfix = 0;
	const char *infix = node->token.value;

	if(node->children.size){
		if(node->children.size == 1){
			postfix = reconstruct_node_string(ast_get_child(node,0));
			if(infix && strcmp(infix, postfix) == 0){infix = 0;}
			if(infix){
				snprintf(buff, rem, "[%s:%s]", infix, postfix);
			}else{
				snprintf(buff, rem, "%s", postfix);
			}
			sanitize_force(buff);
			return buff;
		}
		else if(node->children.size == 2){
			prefix  = reconstruct_node_string(ast_get_child(node, 0));
			postfix = reconstruct_node_string(ast_get_child(node, 1));
			if(infix){
				snprintf(buff, rem, "{%s_%s_%s}", prefix, infix, postfix);
			}else{
				snprintf(buff, rem, "%s(%s)", prefix, postfix);
			}
			sanitize_force(buff);
			return buff;
		}else{
			/// many children, probably a function call
			int rem2 = rem;
			if(rem2){
				char *postfix2 = (char*)malloc(rem2);
				postfix2[0] = 0;

				for(int i = 0; i < node->children.size; i++){
					const char *buff2 = reconstruct_node_string(ast_get_child(node, i));
					//strncat(postfix, buff2, rem2);
					snprintf(postfix2+strlen(postfix2), rem2, (i == 0)? "%s" : ", %s", buff2);
					int L = strlen(buff2);
					rem2 = (rem2 > L)? rem2-L : 0;
				}
				if(infix){
					snprintf(buff, rem, "(%s:%s)", infix, postfix2);
				}else{
					snprintf(buff, rem, "%s", postfix2);
				}
			}
			sanitize_force(buff);
			return buff;
		}
	}else{
		if(infix){
			snprintf(buff,rem, "%s",infix);
		}
		sanitize_force(buff);
		return buff;
	}
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
			const char *warning = "";
			if (!is_sanitary(str)) {
				str = reconstruct_node_string(node);
				warning = "expression (no source text): ";
				if(!is_sanitary(str)){
					str = "expression";
					warning = "error: double failure while printing: ";
				}
			}//"expression";}
			emit_code("COMMENT SOURCE \"%s%s\" // semantic_analyze.c:1014 ",//"/* %s */", 
				escape_string(warning), escape_string(str));
			semantic_general_analyze(ast_get_child(node,0)); //expr (unusued, increment or function call)
			break;
		case(3)://return
			if(semantic_decl){return;}
			emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:1020 ",//"/* %s */",
				sanitize_string(escape_string(removeComments(get_source_text2(node->token.pos)))));
			emit_code("RET // semantic_analyze.c:1022");
			break;
		case(4)://return expr
			if(semantic_decl){return;}
			emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:1026 ", //"/* %s */",
				sanitize_string(escape_string(removeComments(get_source_text2(node->token.pos)))));
			PREP_RES(res1, E_RVAL);
			res1stg.dest.author = "return expr";
			semantic_expr_analyze(ast_get_child(node,0), res1stg); //expr (what to return)
			VERIFY_RES(res1);

			emit_code("RET %s // semantic_analyze.c:1033", sanitize_string(res1.val));
			break;
		case(5)://for_loop
			semantic_general_analyze(ast_get_child(node,0)); //for_loop
			break;
		case(6)://asm_stmt
			semantic_general_analyze(ast_get_child(node,0)); //asm_stmt
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
				emit_code("LABEL %s // semantic_analyze.c:1089", sanitize_string(if_exit));
				emit_code("COMMENT SOURCE \"end if\" // semantic_analyze.c:1090 ");//"/* end if */");
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
				emit_code("COMMENT SOURCE \"else\" // semantic_analyze.c:1105 ");//"/* else */");
				emit_code("LABEL %s // semantic_analyze.c:1106", sanitize_string(if_else));
				struct code_segment* CSinsert;
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 1));
				analyze_scope(ast_get_child(node, 1), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_else = pop_expr();	assert_is_if_else(if_else);
				//if_exit = pop_expr();	assert_is_if_exit(if_exit);
				emit_code("LABEL %s // semantic_analyze.c:1113", sanitize_string(if_exit));
				emit_code("COMMENT SOURCE \"end if\" // semantic_analyze.c:1114 ");//"/* end if */");
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
				emit_code("COMMENT SOURCE \"if(%s)\" // semantic_analyze.c:1173 ",//"/* if(%s) */", 
				sanitize_string(get_source_text2(pos)));//get_source_text(pos.start,pos.end,pos.filename));
				
				//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
				//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
				PREP_RES(res1, E_LVAL);
				res1stg.dest.author = "if_then condition";
				semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr (condition)
				VERIFY_RES(res1);
				
				const char* condition_result = res1.val;
				//if then
				emit_code("/* this skips untrue if's */ // semantic_analyze.c:1185");
				emit_code("JE 0 %s %s", 
					sanitize_string(condition_result), 
					sanitize_string(nextLabel));
				emit_code("/* 'if' is true: */ // semantic_analyze.c:1189");

				//stmt_list
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 1));
				struct code_segment* CSinsert;
				analyze_scope(ast_get_child(node, 1), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_exit = IR_next_name(namespace_semantic,"lbl_if_exit");
				//emit_code("SYMBOL %s LABEL",if_exit);
				if (nextLabel != if_exit) {  
					emit_code("JMP %s // semantic_analyze.c:1199", sanitize_string(if_exit));
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
				emit_code("COMMENT SOURCE \"else if(%s)\" // semantic_analyze.c:1220 ", //"/* else if(%s) */", 
				sanitize_string(get_source_text2(pos)));//get_source_text(pos.start,pos.end,pos.filename));
				emit_code("LABEL %s // semantic_analyze.c:1222", sanitize_string(if_elseif));
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
				emit_code("/* this skips untrue if's */ // semantic_analyze.c:1238");
				emit_code("JE 0 %s %s // semantic_analyze.c:1239", 
					sanitize_string(condition_result), 
					sanitize_string(nextLabel));
				emit_code("COMMENT SOURCE \"then\" // semantic_analyze.c:1242 ");//"/* then */");
				//stmt_list
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 2));
				struct code_segment* CSinsert;
				analyze_scope(ast_get_child(node, 2), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_exit = pop_expr(); //using existing exit
				if (nextLabel != if_exit) {
					emit_code("JMP %s // semantic_analyze.c:1250", sanitize_string(if_exit));
				} //else, we can skip the jmp as the next instruction is already at the end of the if-block.
				//push_expr(if_exit);
				//emit_code("LABEL %s", label1);
				break;
			}
			default:
				error("semantic error: unknown switch case");
				break;
		};
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
		emit_code("COMMENT SOURCE \"while(%s)\" // semantic_analyze.c:1274 ",//"/* while(%s) */",
			sanitize_string(get_source_text2(pos1)));
		const char *label1 = IR_next_name(namespace_semantic,"lbl_while_do");
		const char *label2 = IR_next_name(namespace_semantic,"lbl_while_exit");
		emit_code("SYMBOL %s LABEL // semantic_analyze.c:1278", sanitize_string(label1));
		emit_code("SYMBOL %s LABEL // semantic_analyze.c:1279", sanitize_string(label2));
		emit_code("LABEL %s",sanitize_string(label1));
		
		//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
		//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
		PREP_RES(res1, E_LVAL);
		res1stg.dest.author = "while condition";
		semantic_expr_analyze(ast_get_child(node,0), res1stg); //expr (condition)
		VERIFY_RES(res1);
		
		emit_code("JE 0 %s %s // semantic_analyze.c:1289", 
			sanitize_string(res1.val), 
			sanitize_string(label2));
		emit_code("COMMENT SOURCE \"do\" // semantic_analyze.c:1292");//"/* do */");
		push_symbol_table();
		currentSymbolTable = find_symbol_table_by_node(node);
		struct code_segment *CSinsert;
		analyze_scope(ast_get_child(node,1),0,&CSinsert,&currentSymbolTable,0,1);
		emit_code_segment(CSinsert);
		emit_code("JMP %s // semantic_analyze.c:1298", sanitize_string(label1));
		emit_code("LABEL %s // semantic_analyze.c:1299",sanitize_string(label2));
		emit_code("COMMENT SOURCE \"end while\" // semantic_analyze.c:1300");//"/* end while*/");
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
		emit_code("COMMENT SOURCE \"for(%s)\" // semantic_analyze.c:1327",//"/* for(%s) */", 
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
		 emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:1344",//"/* %s */", 
		 	sanitize_string(removeComments(get_source_text2(pos1))));
		//2.2 emit the statement code 
		 semantic_general_analyze(node_init);

		//3. declare the jump labels that we will later use
		 const char *loopCondition = IR_next_name(namespace_semantic,"lbl_for");
		 const char *loopExit = IR_next_name(namespace_semantic,"lbl_for_exit");
		 emit_code("SYMBOL %s LABEL // semantic_analyze.c:1352", sanitize_string(loopCondition));
		 emit_code("SYMBOL %s LABEL // semantic_analyze.c:1353", sanitize_string(loopExit));
		
		//4. process the condition expression
		//4.1 emit the label that the loop will jump to, to begin each iteration.
		 emit_code("LABEL %s // semantic_analyze.c:1357", sanitize_string(loopCondition));
		//4.2 comment that this is a condition (expr 2)
		 emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:1359",//"/* %s */", 
		 	sanitize_string(removeComments(get_source_text2(pos2))));
		//4.3 emit the code to calculate the condition value
		 PREP_RES(res1, E_LVAL);
		 res1stg.dest.author = "for condition";
		 semantic_expr_analyze(node_cond, res1stg); //expr (i < 10;)
		 VERIFY_RES(res1);
		 const char* condition = res1.val;
		//4.4 emit the code to skip the loop if the condition is not met
		 emit_code("JE 0 %s %s // semantic_analyze.c:1368",
			sanitize_string(condition),
			sanitize_string(loopExit));

		//5. process the loop body
		//5.1 comment that this is the loop body
		 emit_code("COMMENT SOURCE \"loop body\" // semantic_analyze.c:1374");//"/* loop body */");
		//5.2 emit the code of the loop body statements
		 semantic_general_analyze(node_body); //stmt_list

		//6. process the post-loop expression
		//6.1 comment that this is the post-expression
		 emit_code("COMMENT SOURCE \"%s\" // semantic_analyze.c:1380",//"/* %s */", 
		 	sanitize_string(removeComments(get_source_text2(pos3))));
		//6.2 emit the code to execute the post-expression
		 semantic_general_analyze(ast_get_child(node,2)); //expr (i++)
		
		//7. jump back to condition to begin the next iteration
		 emit_code("JMP %s // semantic_analyze.c:1386", sanitize_string(loopCondition));

		//8. emit the loop exit label
		 emit_code("LABEL %s // semantic_analyze.c:1389", sanitize_string(loopExit));
		
		//9. clean up
		 emit_all_deinitializers();
		
		//comment that we are done
		 emit_code("COMMENT SOURCE \"end for\" // semantic_analyze.c:1395");//"/* end for */");
		
		pop_symbol_table();
	}
}


void semantic_analyze_asm_stmt(ast_node *node){
	if(semantic_decl){
		/// do nothing
	}else{
		//#pragma warning need to replace asm_stmt usernames with IR_names!
		printf("semantic_analyze_asm_stmt: debug, token val is [%s]\n", node->token.value);
		struct asm_template tmpl = parse_asm_template(node->token.value);
		vector2_char vstr = vector2_char_here();

		const char *str_iter = tmpl.src_text;

		for(int i = 0; i < tmpl.seq_begin.size; i++){
			const char *seq_begin = m(tmpl.seq_begin, get, i);
			const char *seq_end   = m(tmpl.seq_end, get, i);
			const char *raw_text_before = substr(str_iter, seq_begin);
			printf("i=%d, raw_text_before = [%s]\n", i, raw_text_before);
				const char *seq = substr(seq_begin, seq_end);
				printf("  seq = [%s]\n", seq);

			str_iter = seq_end;

			const char *username = asm_templ_val_name(&tmpl, i);
			struct symbol *S = lookup_symbol(username);
			if(!S){error("Semantic error: unknown symbol %s (only pre-existing symbols can be used in an asm statement)", username);}
			const char *IR_name = S->IR_name;
				printf("  username = [%s], IR_name = [%s]\n", username, IR_name);
			
			vec_printf(&vstr, "%s$%c[%s]", raw_text_before, seq_begin[1], IR_name);
		}
		/// last bit
		const char *raw_text_after = str_iter;//substr(str_iter, tmpl.src_text+strlen(tmpl.src_text));
		vec_printf(&vstr, "%s", raw_text_after);
		
		const char *asm_str = vstr.data;

		printf("Resulting tmpl text is [%s]", asm_str);
		//const char *asm_str = escape_string(node->token.value);
		emit_code("ASM \"%s\" // smeanitc_analyze.c:1414", sanitize_string(asm_str));
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

		semantic_general_analyze(node_dsl); //decl_stmt_list
		
		pop_semantic_context();
		class_def_finalize();
		semantic_this = 0;
		symbolThis = 0;
		pop_symbol_table();
	}else{
		symbolThis = lookup_symbol(node->token.value);
		semantic_this = symbolThis->IR_name;
		class_emit_start();
		
		push_symbol_table();
		currentSymbolTable = symbolThis->symclass.scope; 
		
		semantic_this = 0; /// idk why but having semantic_this messes up plain methods
		semantic_context = SEMANTIC_MEMBER;
		semantic_general_analyze(node_dsl);//this adds functions that were actually defined in code
		semantic_context = SEMANTIC_NORMAL;
		
		pop_symbol_table();
		class_emit_end();
		symbolThis = 0;
		//semantic_this = 0;
	}
}
