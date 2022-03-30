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
	val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1);
	//const char* res2 = 0; struct type_info* res2type = 0;
	//expr_settings stg2 = { .res_type = E_LVAL, .res_out = &res2, .res_out_type = &res2type };
	val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
	expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
	semantic_expr_analyze(ast_get_child(node, 1), stg2);
	//const char* result2 = pop_expr();
	//const char* result1 = pop_expr();
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("%s %s %s %s", OP, exprResult, res1.val, res2.val);
	//output_res(stg, exprResult, res1type);
	val_handle result = { .val = exprResult, .T = res1.T, .rv_type = res1.rv_type };
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
		//push_expr(new_lval());//LV or RV? idk //push_expr("NODISCARD");

		//const char* res1 = 0; struct type_info* res1type = 0;
		//expr_settings stg1 = { .res_type = E_LVAL, .res_out = &res1, .res_out_type = &res1type };
		val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
		expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
		semantic_expr_analyze(ast_get_child(node, i), stg1); //expr
		//give name to new expression result?
		//put it in stack?
	}
}

void semantic_analyze_expr_id(ast_node* node, expr_settings stg) {
	//expr	:	ID	

	//const char* res_dest = pop_expr(); assert_expr_res(res_dest);

	//printf("got expr_id = [%s]\n",node->token.value);
	char* name = stralloc(node->token.value);
	struct symbol* S = lookup_symbol(name);
	if ((S->type != SYMBOL_VARIABLE) && (S->type != SYMBOL_PARAM) && (S->type != SYMBOL_MEMBER) && (S->type != SYMBOL_FUNCTION)) {
		error("semantic: '%s' is not a variable, parameter or function name\n", name);
	}
	struct type_name* T = 0;
	if (S->type == SYMBOL_VARIABLE) {
		T = S->symvariable.type;
	}
	else if (S->type == SYMBOL_PARAM) {
		T = S->symvariable.type;
	}
	else if (S->type == SYMBOL_MEMBER) {
		T = S->symvariable.type;
	}
	else if (S->type == SYMBOL_FUNCTION) {
		T = S->symfunction.signature;
	}
	else {
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic: '%' is not an expression\n", name);
	}
	assert(T);
	printf("found variable '%s' of type '%s'\n", name, T->name);
	//exprResult = name;
	const char* S_full = IR_next_name(namespace_semantic, "temp");//to_rval_name(S->IR_name);

	if(stg.dest.sym_this){//if (semantic_this) {
		const char* semantic_this = stg.dest.sym_this->IR_name;
		//char buff[80];

		if (S->type == SYMBOL_VARIABLE) {
			//const char *S_full = IR_next_name(namespace_semantic,S->IR_name);//IR_next_name(namespace_semantic,S->IR_name);
			emit_code("ADD %s %s &%s /*id:this.var*/", S_full, semantic_this, S->IR_name);
			//if (!discardResult) { push_expr(stralloc(buff)); }
			//output_res(stg, S_full, T);
			val_handle result = { .val = S_full, .rv_type = E_RVAL, .T = T };
			output_res(stg, result, NO_EMIT);
		}
		else if (S->type == SYMBOL_FUNCTION) {
			emit_code("/*id:this.func*/");
			//if (!discardResult) {push_expr(S->IR_name);}
			//output_res(stg, S->IR_name, T); //we use the globally unique IR name of the function (static dispatch)
			val_handle result = { .val = S->IR_name, .rv_type = E_LVAL, .T = T };
			output_res(stg, result, NO_EMIT);
		}
		else {
			error("[SEMANTIC] Unknown symbol type ");
		}
		//const char *result1 = stralloc(buff);
		//push_expr(result1);
	}
	else {
		//if (!discardResult) { push_expr(S->IR_name); }
		emit_code("/*id:global*/");
		//fprintf(stderr, "warning: id always global, can't see 'this'\n");
#pragma message("warning: id always global");
		//output_res(stg, S->IR_name, T);
		val_handle result = { .val = S->IR_name, .rv_type = E_RVAL, .T = T };
		output_res(stg, result, T);
	}
	//push_exprtype(T);
	//put variable <name> on the stack
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
		//output_res(stg, node->token.value, T);
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(1): //hex int
	{
		T->name = "int";
		//output_res(stg, node->token.value, T);
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(2): //binary int
	{
		T->name = "int";
		//output_res(stg, node->token.value, T);
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(3): //float
	{
		T->name = "float";
		//output_res(stg, node->token.value, T);
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(4): //char
	{
		T->name = "char";
		vector2_char vstr = vector2_char_here();
		vec_printf(&vstr, "%d", node->token.value[1]);
		const char* str = stralloc(vstr.data);//stralloc(buff);
		//output_res(stg, str, T);
		val_handle result = { .val = str, .T = T, .rv_type = E_LVAL };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(5): //string
		T->name = "string";
		const char* str_name = IR_next_name(namespace_semantic, "str");
		struct symbol* S = symbol_new0();//new_symbol();
		S->type = SYMBOL_CONST;
		S->storage = STORE_DATA_STATIC;
		S->symconst.type = CONST_STRING;
		S->symconst.data_string = stralloc(node->token.value);
		S->username = 0;// "";//"str"; //constants shouldn't have a username
		S->IR_name = str_name;
		push_symbol(S);

		push_code_segment();
		currentCodeSegment = init_CS;
		emit_code("SYMBOL %s STRING %s", str_name, escape_string(node->token.value));
		pop_code_segment();
		
		//output_res(stg, str_name, T);
		val_handle result = { .val = str_name, .T = T, .rv_type = E_LVAL };
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

	
	
	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_RVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 1), stg1); //expr (array)
	const char* index = res1.val;
	//lval because numbers are ok

	//const char* res2 = 0; struct type_info* res2type = 0;
	//expr_settings stg2 = { .res_type = E_LVAL, .res_out = &res2, .res_out_type = &res2type };
	val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
	expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
	semantic_expr_analyze(ast_get_child(node, 0), stg2); //expr (index)
	const char* ptr = res2.val;

	//okay first of all, expr isn't necessarily a symtable symbol but
	//it could just be an rvalue expression (i.e. temporary value)

	struct type_name* T = res1.T;//pop_exprtype();
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

	emit_code("ADD %s %s %s", resultExpr, index, ptr);
	
	//vector2_char vstr = vector2_char_here();
	//vec_printf(&vstr, "*%s", result);
	//const char* res_val = stralloc(vstr.data);
	
	//output_res(stg, res_val, T2);
	val_handle result = { .val = resultExpr, .rv_type = E_RVAL, .T = T2 };
	return;

	//error("Semantic error: array access into %s, which is not a variable\n");
}

void semantic_analyze_expr_call(ast_node* node, expr_settings stg) {
	//expr: expr '(' expr_list ')'

	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_RVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr (function name or reference expression)
	const char* name = res1.val;
	
	struct type_name* T = res1.T;//pop_exprtype();//S->symfunction.returntype;
	if (!T->args) {
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic error: attempt to call '%s', which is not a function in this scope\n", name);
	}

	struct type_name* resT = m(*(T->args), get, 0);

	ast_node* list = ast_get_child(node, 1);
	int i;
	vector2_ptr_char ministack = vector2_ptr_char_here();
	for (i = 0; i < list->children.size; i++) {

		//const char* res2 = 0; struct type_info* res2type = 0;
		//expr_settings stg2 = { .res_type = E_RVAL, .res_out = &res2, .res_out_type = &res2type };
		val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
		expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
		semantic_expr_analyze(ast_get_child(list, i), stg2); //expr? one of func args?
		const char* expr = res2.val;
		m(ministack, push_back, expr);
	}
	vector2_char vstr = vector2_char_here();
	if (list->children.size == 0) { vec_printf(&vstr, ""); } //{ buff = ""; }
	for (i = 0; i < list->children.size; i++) {
		const char* expr = m(ministack, pop_front);
		vec_printf(&vstr, " %s", expr);
	}
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("CALL %s %s%s", exprResult, name, vstr.data);//buff);

	//output_res(stg, exprResult, T);
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = resT };
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_dot(ast_node* node, expr_settings stg) {
	//expr: expr '.' expr

	if (semantic_decl) { return; }
	
	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_RVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr (first one)
	//what if $0 isn't an ID? need to figure out
	//expression type... also instance... fuck, just redo this.
	struct type_name* T = res1.T;//pop_exprtype();
	if (!T->symclass) {
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic error: member access (x.y) impossible because x is a primitive (%s)\n", T->name);
	}
	push_symbol_table();
	currentSymbolTable = T->symclass->symclass.scope;
	semantic_this = res1.val;//pop_expr();
	//const char* res2 = 0; struct type_name* res2type = 0;
	//expr_settings stg2 = { .res_type = E_RVAL, .res_out = &res2, .res_out_type = &res2type };
	val_handle res2; val_handle res2dest = { .rv_type = E_RVAL };
	expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
	semantic_expr_analyze(ast_get_child(node, 1), stg2); //expr (member)
	semantic_this = 0;
	pop_symbol_table();
	
	//output_res(stg, res1, T);
	
	//is res2 magically at offset because of semantic_this?
	output_res(stg, res2, NO_EMIT);
}

void semantic_analyze_expr_increment(ast_node* node, expr_settings stg) {
	//expr: INC	expr    %prec PREINC

	//in rval, out lval

	if (semantic_decl) { return; }//goto semantic_exit;}
	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_RVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr
	if (node->token.production == 0) {
		//post-increment
		const char* resultExpr = IR_next_name(namespace_semantic, "temp");
		emit_code("MOV %s %s", resultExpr, res1.val);
		emit_code("ADD %s %s 1", res1.val, res1.val);
		//output_res(stg, result, res1type);
		val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T };
		output_res(stg, result, NO_EMIT);
	}
	else {
		//pre-increment
		emit_code("ADD %s %s 1", res1.val, res1.val);
		//output_res(stg, res1, res1type);
		val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = res1.T };
		output_res(stg, result, NO_EMIT);
	}
}

void semantic_analyze_expr_decrement(ast_node* node, expr_settings stg) {
	//expr: DEC	expr    %prec PREDEC	
	
	if (semantic_decl) { return; }
	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_RVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr
	if (node->token.production == 0) {
		//post-decrement
		const char* resultExpr = IR_next_name(namespace_semantic, "temp");
		emit_code("MOV %s %s", resultExpr, res1.val);
		emit_code("SUB %s %s 1", res1.val, res1.val);
		//output_res(stg, result, res1type);
		val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T };
		output_res(stg, result, NO_EMIT);
	}
	else {
		//pre-decrement
		emit_code("SUB %s %s 1", res1.val, res1.val);
		//output_res(stg, res1, res1type);
		val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = res1.T };
		output_res(stg, result, NO_EMIT);
	}
}

void semantic_analyze_expr_neg(ast_node* node, expr_settings stg) {
	//expr: '-' expr 	%prec PRENEG

	
	if (semantic_decl) { return; }
	
	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_LVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr
	
	const char* resultExpr = IR_next_name(namespace_semantic, "temp");
	emit_code("NEG %s %s", resultExpr, res1.val);
	
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
	 
	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_RVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr
	
	struct type_name* T = res1.T;
	struct type_name* T2 = dereffed_type(T);

	//const char* result = IR_next_name(namespace_semantic, "temp");
	//fprintf(stderr, "warning: deref should change expr pointerlevel, it doesn't atm\n");
//#pragma message("warning: deref pointerlevel")
	//output_res_and_emit(stg, result, res1type);
	val_handle result = { .val = res1.val, .rv_type = E_RVAL, .T = T2 };
	output_res(stg, result, YES_EMIT);
}

void semantic_analyze_expr_ref(ast_node* node, expr_settings stg) {
	//expr: '&' expr	%prec PREREF

	if (semantic_decl) { return; }

	//in RVAL, out LVAL

	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_LVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr
	//const char* result = IR_next_name(namespace_semantic, "temp");
	//emit_code("MOV %s &%s", result, res1);
	//output_res(stg, result, res1type);
	struct type* T = res1.T;
	struct type* T2 = reffed_type(T);
	val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = T2 };
	output_res(stg, result, YES_EMIT);
}



void semantic_analyze_expr_assign(ast_node* node, expr_settings stg) {
	//expr: expr '=' expr

	if (semantic_decl) { return; }
//	lvalue = 1;
//#pragma message("warning: lvalue=1, lvalue=0 get rid of it")

	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_RVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr
	//lvalue = 0;
	
	//const char* res2 = 0; struct type_info* res2type = 0;
	//expr_settings stg2 = { .res_type = E_LVAL, .res_out = &res2, .res_out_type = &res2type };
	val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
	expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
	semantic_expr_analyze(ast_get_child(node, 1), stg2); //expr
	emit_code("MOV %s %s //=", res1.val, res2.val);
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
	
	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_LVAL, .res_out = &res1, .res_out_type = &res1type };
	val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr
	
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("NOT %s %s", exprResult, res1);
	//output_res(stg, exprResult, res1type);
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = res1.T };
}

#if 0
//old impl, it bork
//-----------------------------
//void semantic_analyze_expr_pow(ast_node *node, expr_settings stg){
//	//expr: expr '^' expr	
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_^\n");
//	push_expr(new_lval());//push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr (base)
//	push_expr(new_lval());//push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr (exponent)
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = EXPONENT %s %s",exprResult, result1, result2);
//	emit_code("EXP %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: exp
//}

//void semantic_analyze_expr_divide(ast_node *node, expr_settings stg){
//	//expr: expr '/' expr
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_/\n");
//	push_expr(new_lval());//push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr (top)
//	push_expr(new_lval());//push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr (bottom)
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = DIVIDE %s %s",exprResult, result1, result2);
//	emit_code("DIV %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: div
//}


//void semantic_analyze_expr_multiply(ast_node *node, expr_settings stg){
//	//expr: expr '*' expr
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_*\n");
//	push_expr(new_lval());//push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr 1
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr 2
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = MULTIPLY %s %s",exprResult, result1, result2);
//	emit_code("MUL %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: mul
//}


//void semantic_analyze_expr_modulo(ast_node *node, expr_settings stg){
//	//expr: expr '%' expr
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_*\n");
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = MULTIPLY %s %s",exprResult, result1, result2);
//	emit_code("MOD %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: mul
//}

//void semantic_analyze_expr_and(ast_node *node, expr_settings stg){
//	//expr: expr '&' expr
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_*\n");
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = MULTIPLY %s %s",exprResult, result1, result2);
//	emit_code("AND %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: mul
//}


//void semantic_analyze_expr_or(ast_node *node, expr_settings stg){
//	//expr: expr '|' expr
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_*\n");
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = MULTIPLY %s %s",exprResult, result1, result2);
//	emit_code("OR %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: mul
//}
//void semantic_analyze_expr_minus(ast_node *node, expr_settings stg){
//	//expr: expr '-' expr
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_-\n");
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = SUBTRACT %s %s",exprResult, result1, result2);
//	emit_code("SUB %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: sub
//}

//void semantic_analyze_expr_plus(ast_node *node, expr_settings stg){
//	//expr: expr '+' expr
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_+\n");
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = ADD %s %s",exprResult, result1, result2);
//	emit_code("ADD %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: add
//}
// 
//void semantic_analyze_expr_equals(ast_node *node, expr_settings stg){
//	//expr: expr EQUAL expr	
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_+\n");
//	push_expr(new_lval());//push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = EQUAL %s %s",exprResult, result1, result2);
//	emit_code("EQUAL %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: add
//}

//void semantic_analyze_expr_notequal(ast_node *node, expr_settings stg){
//	//expr: expr NOTEQUAL expr
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_+\n");
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = NOTEQUAL %s %s",exprResult, result1, result2);
//	emit_code("NOTEQUAL %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: add
//}



//void semantic_analyze_expr_greater(ast_node *node, expr_settings stg){
//	//expr: expr '>' expr
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_+\n");
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = GREATER %s %s",exprResult, result1, result2);
//	emit_code("GREATER %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//	//emit code: add
//}

//void semantic_analyze_expr_less(ast_node *node, expr_settings stg){
//	//expr: expr '<' expr
//
//	const char* res_dest = pop_expr(); assert_expr_res(res_dest);
//	//int discardResult = (strcmp(res, "DISCARD") == 0);
//
//	if(semantic_decl){return;}//goto semantic_exit;}
//	//printf("got expr_+\n");
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,0)); //expr
//	push_expr(new_lval()); //push_expr("NODISCARD");
//	semantic_analyze(ast_get_child(node,1)); //expr
//	const char *result2 = pop_expr();
//	const char *result1 = pop_expr();
//	const char *exprResult = IR_next_name(namespace_semantic,"temp");
//	//emit_code("%s = LESS %s %s",exprResult, result1, result2);
//	emit_code("LESS %s %s %s",exprResult, result1, result2);
//	//if (!discardResult) {
//	//	push_expr(exprResult);
//	//}
//	output_res(stg, exprResult);
//}

#endif
