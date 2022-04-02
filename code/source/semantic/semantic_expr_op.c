#include "semantic_expr_op.h"
#include "yaccin.tab.h" //for get_source_text2()
#include <assert.h>
//#include "ctype.h"

void semantic_analyze_expr_op_ifx(ast_node* node, const char* OP, expr_settings stg) {
	//const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	if (semantic_decl) { return; }
	//push_expr(new_lval());

	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_LVAL, .res_out = &res1, .res_out_type = &res1type };
	//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_LVAL);
	semantic_expr_analyze(ast_get_child(node, 0), res1stg);
	VERIFY_RES(res1);
	//val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
	//expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
	PREP_RES(res2, E_LVAL);
	semantic_expr_analyze(ast_get_child(node, 1), res2stg);
	VERIFY_RES(res2);

	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("%s %s %s %s", 
		sanitize_string(OP), 
		sanitize_string(exprResult), 
		sanitize_string(res1.val), 
		sanitize_string(res2.val)
			);
	
	val_handle result = { .val = exprResult, .T = res1.T, .rv_type = E_LVAL };
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_pow(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "EXP", stg); }
void semantic_analyze_expr_divide(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "DIV", stg); }
void semantic_analyze_expr_multiply(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "MUL", stg); }
void semantic_analyze_expr_modulo(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "MOD", stg); }
void semantic_analyze_expr_and(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "AND", stg); }
void semantic_analyze_expr_or(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "OR", stg); }
void semantic_analyze_expr_minus(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "SUB", stg); }
void semantic_analyze_expr_plus(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "ADD", stg); }
void semantic_analyze_expr_equals(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "EQUAL", stg); }
void semantic_analyze_expr_notequal(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "NOTEQUAL", stg); }
void semantic_analyze_expr_greater(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "GREATER", stg); }
void semantic_analyze_expr_less(ast_node* node, expr_settings stg) { semantic_analyze_expr_op_ifx(node, "LESS", stg); }

void semantic_analyze_expr_list(ast_node* node) {
	//expr_list:	expr_list_ne | ;
	//expr_list_ne: expr_list_ne ',' expr | expr;
	//...
	//linearized: expr_list: expr ',' expr ... ',' expr ;

	if (semantic_decl) { return; }//goto semantic_exit;}
	//imperative pass: 
	//printf("got expr_list\n");
	for (int i = 0; i < node->children.size; i++) {
		//so some of these may be expressions and some, expression lists. idk.
		//well, actually, the list should be linearized.
		
		//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
		//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
		PREP_RES(res1, E_LVAL);
		semantic_expr_analyze(ast_get_child(node, i), res1stg); //expr
		VERIFY_RES(res1);
		//give name to new expression result?
		//put it in stack?
	}
}


int is_member_access(expr_settings stg) {return (stg.sem_this.rv_type != E_ERROR);}

struct symbol* get_member_symbol(val_handle sem_this, const char* name) {
	if (sem_this.rv_type == E_ERROR) {error("internal semantic error: null sem_this");}
	if (!sem_this.T->symclass) {error("internal semantic error: can't get member of a non-class");}

	struct symbol_table* ST = sem_this.T->symclass->symclass.scope;
	struct symbol *S = try_lookup_symbol_local(name, ST);
	if (!S) {
		error("semantic error: symbol [%s] not found in object [%s] of class [%s]", name, sem_this.val, sem_this.T->name);
	}
	return S;
}

struct symbol* get_symbol_in_this(const char* name, struct symbol* S_this) {
	
	if (S_this) {
		//if (!S_this->symclass) {error("internal semantic error: 'this' is not of class type");}
		struct symbol* S = try_lookup_symbol_local(name, S_this->symclass.scope);
		return S;
	}
	return 0;
}

void semantic_analyze_expr_id(ast_node* node, expr_settings stg) {
	//expr	:	ID	

	char* name = stralloc(node->token.value);
	
	//1. figure out which symbol the ID refers to
	struct symbol* S = 0;
	struct symbol* S_this = 0;
	const char* this_val = 0;
	if (is_member_access(stg)) {
		S = get_member_symbol(stg.sem_this, name);
		this_val = stg.sem_this.val;
	}
	else {
		S_this = try_lookup_symbol("this");
		if (S_this) {
			S = get_symbol_in_this(name, S_this);
			if (S) {
				this_val = S_this->IR_name;
			}else {
				S = lookup_symbol(name);
			}
		}
		else {
			S = lookup_symbol(name);
		}
	}
	const char *S_val = S->IR_name;
	
	//2. make sure it's not a type-name.
	if ((S->type != SYMBOL_VARIABLE) && (S->type != SYMBOL_PARAM) && (S->type != SYMBOL_MEMBER) && (S->type != SYMBOL_FUNCTION)) {
		error("semantic: '%s' is not a variable, parameter or function name\n", name);
	}

	//3. figure out the type of the symbol.
	const char* accessor = "";
	struct type_name* T = 0;

	expr_type rv = E_ERROR;

	if (S->type == SYMBOL_VARIABLE)			{ T = S->symvariable.type;		 S_val = rename_and(S_val); rv = E_PTR;}
	else if (S->type == SYMBOL_PARAM)		{ T = S->symvariable.type;		 S_val = rename_and(S_val); rv = E_PTR;}
	else if (S->type == SYMBOL_MEMBER)		{ T = S->symvariable.type;		 S_val = rename_and(S_val); rv = E_PTR;}
	else if (S->type == SYMBOL_FUNCTION)	{ T = S->symfunction.signature;								rv = E_PTR;}
	else {
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic: '%' is not an expression\n", name);
	}
	assert(T);
	printf("found variable '%s' of type '%s'\n", name, T->name);
	
	const char* exprResult = S_val;
	//4. if we are doing member access, add the object address
	if(this_val){
		emit_code("/* this.id */");
		exprResult = IR_next_name(namespace_semantic, "temp");
		//emit_code("ADD %s %s %s%s",
		//	sanitize_string(exprResult),
		//	sanitize_string(this_val),
		//	sanitize_string(accessor),
		//	sanitize_string(S_val));
		emit_code("ADD %s %s %s",
			sanitize_string(exprResult),
			sanitize_string(this_val),
			sanitize_string(S_val));
	}
	else {
		emit_code("/* global id */");
	}
	//5. phone home

	val_handle result = { .val = exprResult, .rv_type = rv, .T = T };
	output_res(stg, result, YES_EMIT);
}

void semantic_analyze_expr_const(ast_node* node, expr_settings stg) {
	//expr:
		//| INTEGER
		//| INTEGERX
		//| INTEGERB
		//| FLOATING
		//| CHARACTER
		//| STRING
		// 
	//sanitize the value, maybe put it somewhere,
	//get pointers, whatever
	struct type_name* T = malloc(sizeof(struct type_name));
	T->symclass = 0;
	T->args = 0;

	
	switch (node->token.production) {
	case(0): //int
	{
		T->name = "int";
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(1): //hex int
	{
		T->name = "int";
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(2): //binary int
	{
		T->name = "int";
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(3): //float
	{
		T->name = "float";
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(4): //char
	{
		T->name = "char";
		vector2_char vstr = vector2_char_here();
		vec_printf(&vstr, "%d", node->token.value[1]);
		const char* str = stralloc(vstr.data);
		val_handle result = { .val = str, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(5): //string
		T->name = "string";
		const char* str_name = IR_next_name(namespace_semantic, "str");
		struct symbol* S = symbol_new0();
		S->type = SYMBOL_CONST;
		S->storage = STORE_DATA_STATIC;
		S->symconst.type = CONST_STRING;
		S->symconst.data_string = stralloc(node->token.value);
		S->username = 0;// "";//"str"; //constants shouldn't have a username
		S->IR_name = str_name;
		push_symbol(S);

		push_code_segment();
		currentCodeSegment = init_CS;
		emit_code("SYMBOL %s STRING %s", 
			sanitize_string(str_name), 
			sanitize_string(escape_string(node->token.value))
		);
		pop_code_segment();
		
		//output_res(stg, str_name, T);
		val_handle result = { .val = str_name, .T = T, .rv_type = E_LVAL }; //should we make this a ptr? string ptr?
		output_res(stg, result, NO_EMIT);
		break;
	default:
		error("semantic error: unknown switch case");
		break;
	};
}

void semantic_analyze_expr_subexpr(ast_node* node, expr_settings stg) {
	//expr: '(' expr ')'

	//printf("got expr_subexpr\n");
	//push_expr("NODISCARD"); 
	//-- just propagate the res_type we got.
	semantic_expr_analyze(ast_get_child(node, 0), stg); //expr
}

struct type_name* copy_type(struct type_name* T) {
	assert(T);
	struct type_name* T2 = malloc(sizeof(struct type_name));
	*T2 = *T;
	return T2;
}

struct type_name* dereffed_type(struct type_name* T) {
	struct type_name* T2 = copy_type(T);
	if (T2->pointerlevel) {
		T2->pointerlevel--;
	}
	else {
		error("type %s is not a pointer or array\n", T->name);
	}
	return T2;
}

struct type_name* reffed_type(struct type_name* T) {
	struct type_name* T2 = copy_type(T);
	T2->pointerlevel++;
	return T2;
}

void semantic_analyze_expr_index(ast_node* node, expr_settings stg) {
	//expr '[' expr ']'

	//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_PTR);
	semantic_expr_analyze(ast_get_child(node, 1), res1stg); //expr (array)
	VERIFY_RES(res1);

	const char* index = res1.val;
	//lval because numbers are ok

	//val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
	//expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
	PREP_RES(res2, E_LVAL);
	semantic_expr_analyze(ast_get_child(node, 0), res2stg); //expr (index)
	VERIFY_RES(res2);
	const char* ptr = res2.val;

	//okay first of all, expr isn't necessarily a symtable symbol but
	//it could just be an rvalue expression (i.e. temporary value)

	struct type_name* T = res2.T;//pop_exprtype();
	//if (T->pointerlevel == 0) {
	//	//error("Semantic error: array access into %s, which has non-pointer type %s\n",S->username,T->name);
	//	const char* expr1_text = 0; //how do
	//	const char* expr1_type_text = 0;
	//	error("Semantic error: array access into %s, which has non-pointer type %s\n", expr1_text, expr1_type_text);
	//}
	//struct type_name* T2 = malloc(sizeof(struct type_name));
	//*T2 = *T;
	//T2->pointerlevel--;
	//push_exprtype(T2);
	struct type_name* T2 = dereffed_type(T);

	const char* resultExpr = IR_next_name(namespace_semantic, "temp");

	emit_code("ADD %s %s %s", 
		sanitize_string(resultExpr), 
		sanitize_string(index), 
		sanitize_string(ptr));
	
	//vector2_char vstr = vector2_char_here();
	//vec_printf(&vstr, "*%s", result);
	//const char* res_val = stralloc(vstr.data);
	
	//output_res(stg, res_val, T2);
	val_handle result = { .val = resultExpr, .rv_type = E_PTR, .T = T2 };
	//return;//you piece of shit, you didn't output the result
	output_res(stg, result, YES_EMIT);

}

void semantic_analyze_expr_call(ast_node* node, expr_settings stg) {
	//expr: expr '(' expr_list ')'

	struct symbol* sym_args_this = try_lookup_symbol("this");

	//1. get a reference for the function itself
	//1.1 read the function expression
	PREP_RES(res1, E_PTR);//E_RVAL);
	res1stg.sem_this = stg.sem_this;
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr (function name or reference expression)
	VERIFY_RES(res1);
	const char* name = res1.val;

	//1.2 make sure it's actually a function
	struct type_name* T = res1.T;
	if (!T->args) {
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic error: attempt to call '%s', which is not a function in this scope\n", name);
	}
	struct type_name* resT = m(*(T->args), get, 0);

	//2.push the arguments of the function
	ast_node* list = ast_get_child(node, 1);
	int i;
	 //2.1 collect the values that we need to push
	 vector2_ptr_char ministack = vector2_ptr_char_here();
	  //2.1.1 if the function is called as a method, push the 'this'.
		if (stg.sem_this.rv_type != E_ERROR) {
			vector2_char vstr = vector2_char_here();
			vec_printf(&vstr, "&%s", stg.sem_this.val);
			const char* this_ref = stralloc(vstr.data);
			m(ministack, push_back, this_ref);
		}
	  //2.1.2 add the arguments actually given
		for (i = 0; i < list->children.size; i++) {
			PREP_RES(res2, E_LVAL);
			semantic_expr_analyze(ast_get_child(list, i), res2stg); //expr? one of func args?
			VERIFY_RES(res2);
			const char* expr = res2.val;
			m(ministack, push_back, expr);
		}
	 //2.2 format them as a string
	 vector2_char vstr = vector2_char_here();
	 if (list->children.size == 0) { vec_printf(&vstr, ""); }
	 while(ministack.size){//for (i = 0; i < list->children.size; i++) {
 		const char* expr = m(ministack, pop_front);
		vec_printf(&vstr, " %s", expr);
	 }
	//3. actually emit the call
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("CALL %s %s%s", 
		sanitize_string(exprResult), 
		sanitize_string(name), 
		sanitize_string(vstr.data));//buff);

	//4. phone home
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = resT };
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_dot(ast_node* node, expr_settings stg) {
	//expr: expr '.' expr

	if (semantic_decl) { return; }
	
	PREP_RES(res1, E_PTR);
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr (first one)
	VERIFY_RES(res1);

	struct type_name* T = res1.T;
	if (!T->symclass) {
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic error: member access (x.y) impossible because x is a primitive (%s)\n", T->name);
	}
	//push_symbol_table();
	//currentSymbolTable = T->symclass->symclass.scope;
	//no, we don't want to switch symbol table
	//because in A.foo(x), x should not be shadowed
	//by A.x.

	PREP_RES(res2, E_PTR);//E_RVAL);
	res2stg.sem_this = res1;
	semantic_expr_analyze(ast_get_child(node, 1), res2stg); //expr (member)
	VERIFY_RES(res2);
	
	//semantic_this = 0;
	//pop_symbol_table();
	
	//is res2 magically at offset because of semantic_this?
	output_res(stg, res2, YES_EMIT);
}

void semantic_analyze_expr_increment(ast_node* node, expr_settings stg) {
	//expr: INC	expr    %prec PREINC

	//in rval, out lval

	if (semantic_decl) { return; }//goto semantic_exit;}
	//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_RVAL);
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);
	
	if (node->token.production == 0) {
		//post-increment
		const char* resultExpr = IR_next_name(namespace_semantic, "temp");
		emit_code("MOV %s %s", sanitize_string(resultExpr), sanitize_string(res1.val));
		emit_code("ADD %s %s 1", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, result, res1type);
		val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T };
		output_res(stg, result, NO_EMIT);
	}
	else {
		//pre-increment
		emit_code("ADD %s %s 1", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, res1, res1type);
		val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = res1.T };
		output_res(stg, result, NO_EMIT);
	}
}

void semantic_analyze_expr_decrement(ast_node* node, expr_settings stg) {
	//expr: DEC	expr    %prec PREDEC	
	
	if (semantic_decl) { return; }
	//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_RVAL);
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);

	if (node->token.production == 0) {
		//post-decrement
		const char* resultExpr = IR_next_name(namespace_semantic, "temp");
		emit_code("MOV %s %s", sanitize_string(resultExpr), sanitize_string(res1.val));
		emit_code("SUB %s %s 1", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, result, res1type);
		val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T };
		output_res(stg, result, NO_EMIT);
	}
	else {
		//pre-decrement
		emit_code("SUB %s %s 1", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, res1, res1type);
		val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = res1.T };
		output_res(stg, result, NO_EMIT);
	}
}

void semantic_analyze_expr_neg(ast_node* node, expr_settings stg) {
	//expr: '-' expr 	%prec PRENEG

	
	if (semantic_decl) { return; }
	
	//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_LVAL);
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);

	const char* resultExpr = IR_next_name(namespace_semantic, "temp");
	emit_code("NEG %s %s", sanitize_string(resultExpr), sanitize_string(res1.val));
	
	//output_res(stg, result, res1type);
	val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T };
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_deref(ast_node* node, expr_settings stg) {
	//expr: '*' expr	%prec PREDEREF


	if (semantic_decl) { return; }

	//in LVAL, out RVAL. 
	//if upstream wants RVAL, it takes it as is.
	//if upstream wants LVAL, output_res dereferences it.
	 
	//val_handle res1 = { .rv_type = E_ERROR }; val_handle res1dest = { .rv_type = E_LVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_LVAL);
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);

	struct type_name* T = res1.T;
	struct type_name* T2 = dereffed_type(T);

	//const char* exprResult = IR_next_name(namespace_semantic, "temp");
	//emit_code("MOV %s *%s  /* deref */", exprResult, res1.val);

	//reinterpret
	const char* exprResult = rename_star(res1.val);
	val_handle result = { .val = exprResult, .rv_type = E_RVAL, .T = T2 };

	

	//val_handle result = { .val = rename_star(res1.val), .rv_type = E_RVAL, .T = T2 }; 
	//emit_code("/* deref %s */", sanitize_string(result.val));
	output_res(stg, result, YES_EMIT);
}

void semantic_analyze_expr_ref(ast_node* node, expr_settings stg) {
	//expr: '&' expr	%prec PREREF

	if (semantic_decl) { return; }

	//in RVAL, out LVAL

	//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_PTR);//E_RVAL);
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);
	struct type* T = res1.T;
	struct type* T2 = reffed_type(T);
	emit_code("/* ref %s */", sanitize_string(res1.val));
	
	//we get ptr, we want to reinterpret it as E_LVAL 

	val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = T2 };

	output_res(stg, result, NO_EMIT);
}



void semantic_analyze_expr_assign(ast_node* node, expr_settings stg) {
	//expr: expr '=' expr

	//ponder:
	// int *adr_scr;
	// adr_scr = 65536;  -> expr1 is a variable. we need to <var>
	// *adr_scr = 65536; -> expr1 is an address. if this address is calculated, we need to <*temp>
	//


	if (semantic_decl) { return; }

	PREP_RES(res1, E_RVAL);
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);
	assert(res1.val && (res1.val[0] == '*'));

	PREP_RES(res2, E_LVAL);
	semantic_expr_analyze(ast_get_child(node, 1), res2stg); //expr
	VERIFY_RES(res2);

	emit_code("MOV %s %s //=",sanitize_string(res1.val), sanitize_string(res2.val));
	//-- old note:
	//currently the assignment can't have a value because
	//we are using push_expr for other things too.
	//also means we can't have unused values on the stack? foo()?
	//emit code: assign
	//-- new note:
	//now it can, lol

	//output_res(stg, res1, res1type);
	val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = res1.T };
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_not(ast_node* node, expr_settings stg) {
	//expr: '!' expr
	
	if (semantic_decl) { return; }
	
	//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_LVAL);
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);

	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("NOT %s %s", sanitize_string(exprResult), sanitize_string(res1.val));
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = res1.T };
	output_res(stg, result, NO_EMIT);
}


