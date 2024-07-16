#include "semantic_expr_op.h"
#include "yaccin.tab.h" //for get_source_text2()
#include <assert.h>
#include <stdint.h> // for uint64_t
#include <inttypes.h> // for PRIu64 for printf

int do_integer_coersion = 1;
int relax_integer_coersion = 1;

void semantic_analyze_expr_op_ifx(ast_node* node, const char* OP, expr_settings stg) {
	//const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	if (semantic_decl) { return; }
	//push_expr(new_lval());

	//const char* res1 = 0; struct type_info* res1type = 0;
	//expr_settings stg1 = { .res_type = E_LVAL, .res_out = &res1, .res_out_type = &res1type };
	//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };

	vector2_char vstr = vector2_char_here();

	PREP_RES(res1, E_RVAL);//E_LVAL); 12 july 2024 - idk where left and right are but I think the inputs to a + are both from right->
	m(vstr, clear);  vec_printf(&vstr, "op %s arg1", OP); res1stg.dest.author = vstr.data;
	semantic_expr_analyze(ast_get_child(node, 0), res1stg);
	VERIFY_RES(res1);
	//val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
	//expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
	PREP_RES(res2, E_RVAL);//E_LVAL);
	m(vstr, clear);  vec_printf(&vstr, "op %s arg2", OP); res2stg.dest.author = vstr.data;
	semantic_expr_analyze(ast_get_child(node, 1), res2stg);
	VERIFY_RES(res2);

	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("%s %s %s %s", 
		sanitize_string(OP), 
		sanitize_string(exprResult), 
		sanitize_string(res1.val), 
		sanitize_string(res2.val)
			);
	
	// integer arithmetic check
	if(do_integer_coersion){
		if(res2.T && res2.T->name //type is known
			&& (res2.T->pointerlevel == 0)) // not a pointer but a primitive
		{
			if(relax_integer_coersion){
				if(strcmp(OP, "DIV") == 0){
					const char* temp1 = IR_next_name(namespace_semantic, "temp");
					emit_code("FLOOR %s %s", temp1, exprResult);
					exprResult = temp1;
				}
			}else{
				int is_int = (strcmp(res2.T->name, "int") == 0);
				int is_char = (strcmp(res2.T->name, "char") == 0);
				int is_some_integer = is_int || is_char;
				//uint64_t max_limit = 0;
				int64_t min_limit = 0;
				int64_t total_limit = 0;
				if(is_int){min_limit = -2147483648; /*max_limit = 2147483647;*/ total_limit = 1L<<32;} // int always  sint32_t
				if(is_char){min_limit = 0; /*max_limit = 255;*/ total_limit = 256;}				 // char always uint8_t

				if(is_some_integer){
					/// we need to floor and clamp the number
					/// num = ((floor(num) - min_limit) % total_limit) + min_limit
					const char* temp1 = IR_next_name(namespace_semantic, "temp");
					const char* temp2 = IR_next_name(namespace_semantic, "temp");
					const char* temp3 = IR_next_name(namespace_semantic, "temp");
					const char* temp4 = IR_next_name(namespace_semantic, "temp");
					emit_code("COMMENT GENERAL \"(integer conversion to %s)\" // semantic_expr_op.c:58", res2.T->name);
					emit_code("FLOOR %s %s", temp1, exprResult);
					emit_code("SUB %s %s %" PRId64, temp2, temp1, min_limit); ///
					emit_code("MOD %s %s %" PRId64, temp3, temp2, total_limit);
					emit_code("ADD %s %s %" PRId64, temp4, temp3, min_limit);
					exprResult = temp4;
				}
			}
		}
	}
	val_handle result = { .val = exprResult, .T = res1.T, .rv_type = E_LVAL, .author = "expr_op_ifx"};
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
		assert(S_this->type == SYMBOL_PARAM);
		struct type_name* T = S_this->symvariable.type;
		assert(T->symclass);
		struct symbol_table* ST = T->symclass->symclass.scope;

		struct symbol* S = try_lookup_symbol_local(name, ST);
		return S;
	}
	return 0;
}

struct sae_id_result{
	struct type_name *T;
	const char* S_val;
	expr_type rv;	
};

/// helps figure out the IR name, data type and rlvalue type
struct sae_id_result sae_id_helper(struct symbol *S, ast_node* node, const char *name){
	struct sae_id_result res;
	switch(S->type){
		case SYMBOL_VARIABLE:	res = (struct sae_id_result){S->symvariable.type, S->IR_name, E_LVAL}; break;
		case SYMBOL_PARAM:		res = (struct sae_id_result){S->symvariable.type, S->IR_name, E_LVAL}; break;
		case SYMBOL_MEMBER: 	res = (struct sae_id_result){S->symvariable.type, S->IR_name, E_LVAL}; break;
		case SYMBOL_FUNCTION:	res = (struct sae_id_result){S->symfunction.signature, S->IR_name, E_FPTR/*E_RVAL*/}; break;
		default:
		{
			YYLTYPE pos = node->token.pos;
			err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
			error("semantic: '%' is not an expression\n", name);
		}
	};
	assert(res.T);
	//if((S->type == SYMBOL_VARIABLE) && S->symvariable.array){
	//	res.S_val = rename_and(res.S_val);
	//	res.rv = E_PTR;
	//}
	//if((S->type == SYMBOL_FUNCTION)){
	//	res.S_val = rename_and(res.S_val);
	//}
	//if((res.rv == E_PTR) && (res.T->pointerlevel == 0)){res.S_val = rename_and(res.S_val);}
	return res;
}

void semantic_analyze_expr_id(ast_node* node, expr_settings stg) {
	//expr	:	ID	

	char* name = stralloc(node->token.value);
	
	//if (!(strcmp(name, "x"))) {
	//	printf("debug trap");
	//}

	vector2_char vstr = vector2_char_here();
	const char* str_access_type = 0;

	//1. figure out which symbol the ID refers to
	struct symbol* S = 0;
	struct symbol* S_this = 0;
	const char* this_val = 0;
	if (is_member_access(stg)) {
		S = get_member_symbol(stg.sem_this, name);
		this_val = stg.sem_this.val;
	}
	else {
		//priority for looking up a symbol:
		//1. current ST (function local vars and args)
		//2. 'this' object (but we have to know we are accessing 'this')
		//3. anywhere else (parent scope / global vars)
		S = try_lookup_symbol_local(name, currentSymbolTable);
		if (!S) {
			S_this = try_lookup_symbol("this");
			if (S_this) {
				S = get_symbol_in_this(name, S_this);
				if (S) {
					this_val = S_this->IR_name;
				}
				else {
					S = lookup_symbol(name);
				}
			}
			else {
				S = lookup_symbol(name);
			}
		}
	}
	const char *S_val = 0;//S->IR_name;
	
	//2. make sure it's not a type-name.
	if ((S->type != SYMBOL_VARIABLE) && (S->type != SYMBOL_PARAM) && (S->type != SYMBOL_MEMBER) && (S->type != SYMBOL_FUNCTION)) {
		error("semantic: '%s' is not a variable, parameter or function name\n", name);
	}

	//3. figure out the type of the symbol.
	//const char* accessor = "";
	struct type_name* T = 0;

	expr_type rv = E_ERROR;

	//if (S->type == SYMBOL_VARIABLE)			{ T = S->symvariable.type;		S_val = rename_and(S_val); res.rv = E_PTR;}
	//else if (S->type == SYMBOL_PARAM)		{ T = S->symvariable.type;		S_val = rename_and(S_val); 		rv = E_PTR;}
	//else if (S->type == SYMBOL_MEMBER)		{ T = S->symvariable.type;		S_val = rename_and(S_val);		rv = E_PTR;}
	//else if (S->type == SYMBOL_FUNCTION)	{ T = S->symfunction.signature;	 								rv = E_LVAL;}//rv = E_PTR;}
	//else {
	//	YYLTYPE pos = node->token.pos;
	//	err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
	//	error("semantic: '%' is not an expression\n", name);
	//}
	struct sae_id_result res = sae_id_helper(S, node, name);
	T = res.T; S_val = res.S_val; rv = res.rv;

	assert(T);
	printf("found variable '%s' of type '%s'\n", name, T->name);
	
	const char* exprResult = S_val;
	//4. if we are doing member access, add the object address
	if(this_val && (S->type != SYMBOL_FUNCTION)){
		//emit_code("/* this.id */");
		str_access_type = "this.id";
		exprResult = IR_next_name(namespace_semantic, "temp");
		//emit_code("ADD %s %s %s%s",
		//	sanitize_string(exprResult),
		//	sanitize_string(this_val),
		//	sanitize_string(accessor),
		//	sanitize_string(S_val));
		vec_printf(&vstr, "ADD %s %s %s",
			sanitize_string(exprResult),
			sanitize_string(this_val),
			sanitize_string(S_val));
	}
	else {
		//emit_code("/* global id */");
		str_access_type = "global id";
	}

	//if (vstr.size) {
		vec_printf(&vstr, "   /* %s [%s] */", str_access_type, name);
		emit_code("%s", sanitize_string(vstr.data));
	//}
	//5. phone home

	val_handle result = { .val = exprResult, .rv_type = rv, .T = T, .author = "expr_id" };
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

	const char* author = "expr_const";
	
	switch (node->token.production) {
	case(0): //int
	{
		T->name = "int";
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL, .author = author };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(1): //hex int
	{
		T->name = "int";
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL, .author = author };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(2): //binary int
	{
		T->name = "int";
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL, .author = author };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(3): //float
	{
		T->name = "float";
		val_handle result = { .val = node->token.value, .T = T, .rv_type = E_LVAL, .author = author };
		output_res(stg, result, NO_EMIT);
		break;
	}
	case(4): //char
	{
		T->name = "char";
		vector2_char vstr = vector2_char_here();
		vec_printf(&vstr, "%d", node->token.value[1]);
		const char* str = stralloc(vstr.data);
		val_handle result = { .val = str, .T = T, .rv_type = E_LVAL, .author = author };
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
		val_handle result = { .val = str_name, .T = T, .rv_type = E_LVAL, .author = author }; //should we make this a ptr? string ptr?
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
	PREP_RES(res1, E_LVAL);
	res1stg.dest.author = "expr_index array (arr[])";
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr (array)
	VERIFY_RES(res1);

	const char* index = res1.val;
	//lval because numbers are ok

	//val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
	//expr_settings stg2 = { .dest = res2dest, .actual = &res2 };
	PREP_RES(res2, E_RVAL);//E_LVAL);
	res2stg.dest.author = "expr_index index ([idx])";
	semantic_expr_analyze(ast_get_child(node, 1), res2stg); //expr (index)
	VERIFY_RES(res2);
	const char* ptr = res2.val;

	//okay first of all, expr isn't necessarily a symtable symbol but
	//it could just be an rvalue expression (i.e. temporary value)

	struct type_name* T = res1.T;//res2.T;//pop_exprtype();
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
	val_handle result = { .val = resultExpr, .rv_type = E_PTR, .T = T2, .author = "expr_index" };
	//return;//you piece of shit, you didn't output the result
	output_res(stg, result, YES_EMIT);

}

struct type_name* shallow_copy(struct type_name* src){
	struct type_name *res = (struct type_name *)malloc(sizeof(struct type_name));
	*res = *src;
	return res;
}

void semantic_analyze_expr_call(ast_node* node, expr_settings stg) {
	//expr: expr '(' expr_list ')'

	//struct symbol* sym_args_this = try_lookup_symbol("this");

	//1. get a reference for the function itself
	//1.1 read the function expression

	// plain func (E_PTR) or   void *ptr; ptr (E_RVAL)
	//			        \     /
	// 			        E_RVAL

	PREP_RES(res1, E_RVAL);//E_LVAL);//so we can call funcptrs  E_PTR);//E_RVAL);
	res1stg.sem_this = stg.sem_this;
	res1stg.dest.author = "expr_call funcname";
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr (function name or reference expression)
	VERIFY_RES(res1);
	const char* name = res1.val;

	//1.2 make sure it's actually a function
	struct type_name* T = res1.T;
	/// 9 July 2024: allow us to call a pointer, result is deref type
	struct type_name* resT = 0;
	if(T->args){
		resT = m(*(T->args), get, 0);
	}else{
		if(T->pointerlevel){
			resT = shallow_copy(T);
			resT->pointerlevel--;
		}else{
			YYLTYPE pos = node->token.pos;
			err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
			error("semantic error: attempt to call '%s', which is not a function in this scope\n", name);
		}
	}
	
	//2.push the arguments of the function
	ast_node* list = ast_get_child(node, 1);
	int i;
	 //2.1 collect the values that we need to push
	 vector2_ptr_char ministack = vector2_ptr_char_here();
	  //2.1.1 if the function is called as a method, push the 'this'.
		if (stg.sem_this.rv_type != E_ERROR) {
			vector2_char vstr = vector2_char_here();
			vec_printf(&vstr, "%s", stg.sem_this.val); //should already be a pointer, e.g. &derp
			const char* this_ref = stralloc(vstr.data);
			m(ministack, push_back, this_ref);
		}
	  //2.1.2 add the arguments actually given
		for (i = 0; i < list->children.size; i++) {
			PREP_RES(res2, E_RVAL); //R for read, L for write //E_LVAL);
			res2stg.dest.author = "expr_call args";
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
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = resT, .author = "expr_call" };
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_dot(ast_node* node, expr_settings stg) {
	//expr: expr '.' expr

	if (semantic_decl) { return; }
	
	PREP_RES(res1, E_PTR);
	res1stg.dest.author = "expr_dot object(o.)";
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

	PREP_RES(res2, E_ASIS);//E_PTR);//E_RVAL);
	res2stg.sem_this = res1;
	if(strcmp(res1.val, "&derp1")==0){
		printf("debug breakpoint");
	}
	res2stg.dest.author = "expr_dot member(.m)";
	semantic_expr_analyze(ast_get_child(node, 1), res2stg); //expr (member)
	VERIFY_RES(res2);
	
	//semantic_this = 0;
	//pop_symbol_table();
	
	//is res2 magically at offset because of semantic_this?
	res2.author = "expr_dot";
	output_res(stg, res2, YES_EMIT);
}

void semantic_analyze_expr_increment(ast_node* node, expr_settings stg) {
	//expr: INC	expr    %prec PREINC

	//in rval, out lval

	if (semantic_decl) { return; }//goto semantic_exit;}

	const char* author = "expr_increment(++)";
	//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_RVAL);
	res1stg.dest.author = author;
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);
	
	if (node->token.production == 0) {
		//post-increment
		const char* resultExpr = IR_next_name(namespace_semantic, "temp");
		emit_code("MOV %s %s", sanitize_string(resultExpr), sanitize_string(res1.val));
		emit_code("ADD %s %s 1", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, result, res1type);
		val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T, .author = author };
		output_res(stg, result, NO_EMIT);
	}
	else {
		//pre-increment
		emit_code("ADD %s %s 1", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, res1, res1type);
		val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = res1.T, .author = author };
		output_res(stg, result, NO_EMIT);
	}
}

void semantic_analyze_expr_decrement(ast_node* node, expr_settings stg) {
	//expr: DEC	expr    %prec PREDEC	
	
	if (semantic_decl) { return; }

	const char* author = "expr_decrement(--)";
	//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_RVAL);
	res1stg.dest.author = author;
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);

	if (node->token.production == 0) {
		//post-decrement
		const char* resultExpr = IR_next_name(namespace_semantic, "temp");
		emit_code("MOV %s %s", sanitize_string(resultExpr), sanitize_string(res1.val));
		emit_code("SUB %s %s 1", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, result, res1type);
		val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T, .author = author };
		output_res(stg, result, NO_EMIT);
	}
	else {
		//pre-decrement
		emit_code("SUB %s %s 1", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, res1, res1type);
		val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = res1.T, .author = author };
		output_res(stg, result, NO_EMIT);
	}
}

void semantic_analyze_expr_neg(ast_node* node, expr_settings stg) {
	//expr: '-' expr 	%prec PRENEG

	
	if (semantic_decl) { return; }
	
	const char* author = "expr_neg(-)";
	//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_LVAL);
	res1stg.dest.author = author;
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);

	const char* resultExpr = IR_next_name(namespace_semantic, "temp");
	emit_code("NEG %s %s", sanitize_string(resultExpr), sanitize_string(res1.val));
	
	//output_res(stg, result, res1type);
	val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T, .author = author };
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_deref(ast_node* node, expr_settings stg) {
	//expr: '*' expr	%prec PREDEREF


	if (semantic_decl) { return; }

	const char* author = "expr_deref(*)";
	//in LVAL, out RVAL. 
	//if upstream wants RVAL, it takes it as is.
	//if upstream wants LVAL, output_res dereferences it.
	 
	//val_handle res1 = { .rv_type = E_ERROR }; val_handle res1dest = { .rv_type = E_LVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_LVAL);
	res1stg.dest.author = author;
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);

	struct type_name* T = res1.T;
	struct type_name* T2 = dereffed_type(T);

	//const char* exprResult = IR_next_name(namespace_semantic, "temp");
	//emit_code("MOV %s *%s  /* deref */", exprResult, res1.val);

	//reinterpret
	const char* exprResult = rename_star(res1.val);
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = T2, .author = author };

	

	//val_handle result = { .val = rename_star(res1.val), .rv_type = E_RVAL, .T = T2 }; 
	//emit_code("/* deref %s */", sanitize_string(result.val));
	output_res(stg, result, YES_EMIT);
}

void semantic_analyze_expr_ref(ast_node* node, expr_settings stg) {
	//expr: '&' expr	%prec PREREF

	if (semantic_decl) { return; }

	const char* author = "expr_ref(&)";
	//in RVAL, out PTR

	//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_PTR); //RVAL so we get unadorned number  //E_PTR);
	res1stg.dest.author = "expr_ref(&) expr";
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);
	
	struct type_name* T = res1.T;
	if(T->args){
		/// this has args, it's a function.
		/// do not dereference functions.
		error("semantic error: a function is already an address");
		return;
	}
	struct type_name* T2 = reffed_type(T);
	emit_code("/* ref %s */", sanitize_string(res1.val));
	
	
	const char* resultExpr = IR_next_name(namespace_semantic, "temp");
	emit_code("MOV %s %s", resultExpr, res1.val);
	/// we take a value, remove adorations (ref or deref as necessary)
	/// and then present it as a plain number (RValue=read)

	/// output as ptr of (&x) kind
	val_handle result = { .val = /*res1.val*/resultExpr, .rv_type = E_RVAL/*E_PTR*/, .T = T2, .author = author };

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

	PREP_RES(res1, E_LVAL); //R=read, L=write //E_RVAL);
	res1stg.dest.author = "expr_assign arg1 (... =)";
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);
	//assert(res1.val && (res1.val[0] == '*'));
	assert(res1.val);

	PREP_RES(res2, E_RVAL);//E_LVAL);
	res2stg.dest.author = "expr_assign arg2 (= ...)";
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
	val_handle result = { .val = res1.val, .rv_type = E_LVAL, .T = res1.T, .author = "expr_assign(=)" };
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_not(ast_node* node, expr_settings stg) {
	//expr: '!' expr
	
	if (semantic_decl) { return; }

	const char* author = "expr_not(!)";
	
	//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_LVAL);
	res1stg.dest.author = author;
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);

	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("NOT %s %s", sanitize_string(exprResult), sanitize_string(res1.val));
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = res1.T, .author = author };
	output_res(stg, result, NO_EMIT);
}


