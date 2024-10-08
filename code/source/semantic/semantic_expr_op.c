#include "semantic_expr_op.h"
#include "yaccin.tab.h" //for get_source_text2()
#include <assert.h>
#include <stdint.h> // for uint64_t
#include <inttypes.h> // for PRIu64 for printf
#include "ast_gen.h"
#include "typecheck.h"

int do_integer_coersion = 0; /// no integer conversions until we figure out resulting expr type
int relax_integer_coersion = 1;
int do_typechecks = 1;

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

	PREP_RES(res3, E_ASIS);
	m(vstr, clear);  vec_printf(&vstr, "op %s arg2", OP); res3stg.dest.author = vstr.data;
	typecheck_op(res1, OP, res2, res3stg);
	VERIFY_RES(res3);
	/*
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("%s %s %s %s // semantic_expr_op.c:37", 
		sanitize_string(OP), 
		sanitize_string(exprResult), 
		sanitize_string(res1.val), 
		sanitize_string(res2.val)
			);
	
	// integer arithmetic check
	if(do_integer_coersion){
		if(res2.T && res2.T->name //type is known
			&& (res2.T->points_to == 0) /// not a pointer but a primitive
			&& (is_int(res2.T) || is_char(res2.T))) /// some integer type (int or char, not float)
		{
			if(relax_integer_coersion){
				if(strcmp(OP, "DIV") == 0){
					const char* temp1 = IR_next_name(namespace_semantic, "temp");
					emit_code("FLOOR %s %s // semantic_expr_op.c:52", temp1, exprResult);
					exprResult = temp1;
				}
			}else{
				if(is_int(res2.T) || is_char(res2.T)){
					emit_code("COMMENT GENERAL \"(integer coersion to ensure %s stays %s)\" // semantic_expr_op.c:82", res2.T->name);
					exprResult = emit_type_conversion(gen_type_name_float(), res2.T, res2);
				}
			}
		}
	}
	*/
	//val_handle result = { .val = exprResult, .T = res1.T, .rv_type = E_LVAL, .author = "expr_op_ifx"};
	val_handle result = { .val = res3.val, .T = res3.T, .rv_type = E_LVAL, .author = "expr_op_ifx"};
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_pow(ast_node* node, expr_settings stg)		{ semantic_analyze_expr_op_ifx(node, "EXP", stg); }
void semantic_analyze_expr_divide(ast_node* node, expr_settings stg)	{ semantic_analyze_expr_op_ifx(node, "DIV", stg); }
void semantic_analyze_expr_multiply(ast_node* node, expr_settings stg)	{ semantic_analyze_expr_op_ifx(node, "MUL", stg); }
void semantic_analyze_expr_modulo(ast_node* node, expr_settings stg)	{ semantic_analyze_expr_op_ifx(node, "MOD", stg); }
void semantic_analyze_expr_and(ast_node* node, expr_settings stg)		{ semantic_analyze_expr_op_ifx(node, "AND", stg); }
void semantic_analyze_expr_or(ast_node* node, expr_settings stg)		{ semantic_analyze_expr_op_ifx(node, "OR", stg); }
void semantic_analyze_expr_minus(ast_node* node, expr_settings stg)		{ semantic_analyze_expr_op_ifx(node, "SUB", stg); }
void semantic_analyze_expr_plus(ast_node* node, expr_settings stg)		{ semantic_analyze_expr_op_ifx(node, "ADD", stg); }
void semantic_analyze_expr_equals(ast_node* node, expr_settings stg)	{ semantic_analyze_expr_op_ifx(node, "EQUAL", stg);		stg.actual->T = type_name_new("int",0,0,0,0,0,0);}
void semantic_analyze_expr_notequal(ast_node* node, expr_settings stg)	{ semantic_analyze_expr_op_ifx(node, "NOTEQUAL", stg);	stg.actual->T = type_name_new("int",0,0,0,0,0,0);}
void semantic_analyze_expr_greater(ast_node* node, expr_settings stg)	{ semantic_analyze_expr_op_ifx(node, "GREATER", stg);	stg.actual->T = type_name_new("int",0,0,0,0,0,0);}
void semantic_analyze_expr_less(ast_node* node, expr_settings stg)		{ semantic_analyze_expr_op_ifx(node, "LESS", stg);		stg.actual->T = type_name_new("int",0,0,0,0,0,0);}
void semantic_analyze_expr_less_equal(ast_node* node, expr_settings stg)	{ semantic_analyze_expr_op_ifx(node, "LEQ", stg);	stg.actual->T = type_name_new("int",0,0,0,0,0,0);}
void semantic_analyze_expr_greater_equal(ast_node* node, expr_settings stg)	{ semantic_analyze_expr_op_ifx(node, "GEQ", stg);	stg.actual->T = type_name_new("int",0,0,0,0,0,0);}

void semantic_analyze_expr_list(ast_node* node, expr_settings stg) {
	//expr_list:	expr_list_ne | ;
	//expr_list_ne: expr_list_ne ',' expr | expr;
	//...
	//linearized: expr_list: expr ',' expr ... ',' expr ;

	if (semantic_decl) { return; }//goto semantic_exit;}
	if(!stg.out_list){
		error("Semantic error: expression list is not expected in this context");
	}
	//imperative pass: 
	//printf("got expr_list\n");
	for (int i = 0; i < node->children.size; i++) {
		//so some of these may be expressions and some, expression lists. idk.
		//well, actually, the list should be linearized.
		
		//val_handle res1; val_handle res1dest = { .rv_type = E_LVAL };
		//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
		PREP_RES(res1, E_RVAL); ///? before constructors, nobody used it, right? //E_LVAL);
		res1stg.dest.author = "expr_list item (,)";
		semantic_expr_analyze(ast_get_child(node, i), res1stg); //expr
		VERIFY_RES(res1);
		//give name to new expression result?
		//put it in stack?
		m(*stg.out_list, push_back, res1);
	}
	stg.actual->rv_type = E_LIST;
	stg.actual->val = "<expr_list>";
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
		T = dereffed_type(T);
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
		case SYMBOL_MEMBER: 	res = (struct sae_id_result){S->symvariable.type, S->IR_name, E_PTR/*E_LVAL*/}; break;
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
					if(stg.out_sem_this){*stg.out_sem_this = (val_handle){.author = "expr_id", .rv_type = E_PTR, .T = S_this->symvariable.type, .val = this_val};}
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
		emit_code("%s // semantic_expr_op.c:286", sanitize_string(vstr.data));
	//}
	//5. phone home

	val_handle result = { .val = exprResult, .rv_type = rv, .T = T, .author = "expr_id" };
	output_res(stg, result, YES_EMIT);
}

char unescape_char(char c){
	switch(c){
		case 'n': return '\n'; break;
		case 'r': return '\r'; break;
		case 't': return '\t'; break;
		case 'b': return '\b'; break;
		case 'e': return '\e'; break;
		case '0': return '\0'; break;
		case '\'': return '\''; break;
		case '\"': return '\"'; break;
		case '\\': return '\\'; break;
		default: error("unknown character escape sequence \\%c", c);
	}
	assert(!"unreachable");
	return 0;
}

int escape_char(char c){
	switch(c){
		case '\n': return 'n'; break;
		case '\r': return 'r'; break;
		case '\t': return 't'; break;
		case '\b': return 'b'; break;
		case '\e': return 'e'; break;
		case '\0': return '0'; break;
		case '\'': return '\''; break;
		case '\"': return '\"'; break;
		case '\\': return '\\'; break;
		default: return -1;
	}
	assert(!"unreachable");
	return 0;
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
	struct type_name* T = type_name_new0();//malloc(sizeof(struct type_name));
	T->symclass = 0;
	T->args = 0;
	T->is_literal = 1;
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
		const char *tok_str = node->token.value;
		assert(tok_str);
		int len = strlen(tok_str);
		int val = 0;
		if(len == 3){
			val = tok_str[1];
		}else if(len == 4){
			assert(tok_str[1] == '\\');
			val = unescape_char(tok_str[2]);
		}else{
			error("semantic error: bad char literal [%s]", tok_str);
		}
		//vec_printf(&vstr, "%d", node->token.value[1]);
		vec_printf(&vstr, "%d", val);
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
		emit_code("SYMBOL %s STRING \"%s\"", // no comment here cause string is until the end of line 
			sanitize_string(str_name), 
			//sanitize_string(escape_string(node->token.value))
			sanitize_string(escape_string(unescape_string(node->token.value))) /// why are we unescaping and then escaping?
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

void semantic_analyze_expr_braced_list(ast_node* node, expr_settings stg){
	semantic_expr_analyze(ast_get_child(node,0), stg);
}

//struct type_name* copy_type(struct type_name* T) {
//	assert(T);
//	struct type_name* T2 = type_name_new0();//malloc(sizeof(struct type_name));
//	*T2 = *T;
//	return T2;
//}

struct type_name* dereffed_type(struct type_name* T) {
	/*
	struct type_name* T2 = copy_type(T);
	if (T2->pointerlevel) {
		T2->pointerlevel--;
	}
	else {
		error("type %s is not a pointer or array\n", T->name);
	}
	return T2;
	*/
	if(T->is_array){
		struct type_name *T2 = type_name_shallow_copy(T);
		T2->is_array = 0;
		T2->arraysize = 0;
		return T2;
	}else if(T->points_to){
		return T->points_to;
	}else{
		error("[INTERNAL] type %s is not an array or pointer\n", type_name_to_string(T));
	}
	assert(!"unreachable");
	return 0;
}

struct type_name* reffed_type(struct type_name* T) {
	//struct type_name* T2 = copy_type(T);
	//T2->pointerlevel++;
	//return T2;
	struct type_name *T2 = type_name_new0();
	T2->points_to = T;
	return T2;
}

void semantic_analyze_expr_index(ast_node* node, expr_settings stg) {
	//expr '[' expr ']'

	//val_handle res1; val_handle res1dest = { .rv_type = E_RVAL };
	//expr_settings stg1 = { .dest = res1dest, .actual = &res1 };
	PREP_RES(res1, E_ASIS);//E_PTR);//E_LVAL);
	res1stg.dest.author = "expr_index array (arr[])";
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr (array)
	VERIFY_RES(res1);

	const char* index = res1.val;
	//lval because numbers are ok

	//val_handle res2; val_handle res2dest = { .rv_type = E_LVAL };
	//expr_settings stg2 = { .dest = res2dest, .actual = &res2 };

	struct type_name* T = res1.T;
	struct type_name* T2 = dereffed_type(T);
	
	/// never needed this but seems like a good idea?
	if(!T->is_array){
		if(T->points_to){
			/// we need to convert the pointer to RValue before indexing
			res1 = convert_rv_type(res1, E_RVAL, "expr_index array from ptr (*arr[])");
		}else{
			const char *str_T = type_name_to_string(T);
			error("Semantic error: Can't index into %s", str_T);
		}
	}

	// we need to multiply by size
	struct ast_node *node_byte_index = ast_get_child(node, 1);
	struct ast_node *node_size = ast_gen_num(getTypeSize(T2));
	struct ast_node *node_obj_index = ast_gen_op(node_byte_index, "expr_*", node_size);

	PREP_RES(res2, E_RVAL);//E_LVAL);
	res2stg.dest.author = "expr_index index ([idx])";
	semantic_expr_analyze(node_obj_index, res2stg);//(ast_get_child(node, 1), res2stg); //expr (index)
	VERIFY_RES(res2);
	const char* ptr = res2.val;

	//okay first of all, expr isn't necessarily a symtable symbol but
	//it could just be an rvalue expression (i.e. temporary value)


	const char* resultExpr = IR_next_name(namespace_semantic, "temp");

	emit_code("ADD %s %s %s // semantic_expr_op.c:463", 
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

int has_varargs(vector2_ptr_type_name *args){
	for(int i = 1; i < args->size; i++){
		struct type_name *T = m(*args, get, i);
		if(T->name && (strcmp(T->name, "varargs") == 0)){return i;}
	}
	return 0;
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
	val_handle arg_this; arg_this.rv_type = E_ERROR;
	if(stg.sem_this.rv_type != E_ERROR){
		arg_this = stg.sem_this;
	}
	res1stg.out_sem_this = &arg_this;
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr (function name or reference expression)
	VERIFY_RES(res1);
	const char* name = res1.val;
	
	//if(strcmp(name, "_test71") == 0){
	//	printf("debug breakpoint");
	//}

	//1.2 make sure it's actually a function
	struct type_name* T = res1.T;
	/// 9 July 2024: allow us to call a pointer, result is deref type
	struct type_name* resT = 0;
	int skip_typechecks = 0; /// used to skip some or all of type checks during argument parsing for this call only
	if(T->points_to){
		T = T->points_to;
		skip_typechecks = 1;
	}
	if(!T->args && !skip_typechecks){
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic error: attempt to call '%s', which is not a function in this scope\n", name);
	}
	if(skip_typechecks){
		resT = T;
	}else{
		resT = m(*(T->args), get, 0);
	}
	//2.push the arguments of the function
	vector2_val_handle arguments = vector2_val_handle_here();

	ast_node* list = ast_get_child(node, 1);
	int i;
	 //2.1 collect the values that we need to push
	 //vector2_ptr_char ministack = vector2_ptr_char_here();
	  //2.1.1 if the function is called as a method, push the 'this'.
		if (arg_this.rv_type != E_ERROR) {
			//vector2_char vstr = vector2_char_here();
			//vec_printf(&vstr, "%s", arg_this.val);//stg.sem_this.val); //should already be a pointer, e.g. &derp
			//const char* this_ref = stralloc(vstr.data);
			//m(ministack, push_back, this_ref);
			m(arguments, push_back, arg_this);
		}
	  //2.1.2 add the arguments actually given
		for (i = 0; i < list->children.size; i++) {
			PREP_RES(res2, E_RVAL); //R for read, L for write //E_LVAL);
			res2stg.dest.author = "expr_call args";
			semantic_expr_analyze(ast_get_child(list, i), res2stg); //expr? one of func args?
			VERIFY_RES(res2);
			//const char* expr = res2.val;
			//m(ministack, push_back, expr);
			m(arguments, push_back, res2);
		}

	int variadic_pos = 0;

	if(do_typechecks && !skip_typechecks){
		variadic_pos = has_varargs(T->args); // non-variadic arguments of variadic functions are still type-checked up to the "variadic" keyword
		struct type_name *T_expr = type_name_new0();
		T_expr->args = vector2_ptr_type_name_new();
		m(*(T_expr->args),push_back,resT);
		for(int i = 0; i < arguments.size; i++){
			val_handle arg = m(arguments, get, i);
			m(*(T_expr->args), push_back, arg.T);
		}
		if(!type_args_equals(T->args, T_expr->args)){
			/// check if types are compatible by checking all args except first (return type).
			const char *T1_str = type_name_to_string(T);
			const char *T2_str = type_name_to_string(T_expr);
			if(!all_compatible(T->args, T_expr->args, 1, variadic_pos, IS_CALL)){
				error("Semantic error: function arguments don't match:\nexpected %s;\ngot      %s;", T1_str, T2_str);
			}
		}
	}

	 //2.2 format them as a string
	 vector2_char vstr = vector2_char_here();
	 if (list->children.size == 0) { vec_printf(&vstr, ""); }

	 if(do_typechecks && !skip_typechecks){
		int n_args_expected = T->args->size-1;
		if(variadic_pos){n_args_expected = variadic_pos-1;}
		int n_args_got = arguments.size;

		if((n_args_got > n_args_expected) && !variadic_pos){
			error("Semantic error: too many arguments for a function call (expected %d, got %d)", n_args_expected, n_args_got);
		}else if(n_args_got < n_args_expected){
			error("Semantic error: not enough arguments for a function call (expected %d, got %d)", n_args_expected, n_args_got);
		}
	}

	 for(int i = 0; i < arguments.size; i++){//while(ministack.size){//for (i = 0; i < list->children.size; i++) {
 		//const char* expr = m(ministack, pop_front);
		val_handle res = m(arguments,get,i);
		if(i == variadic_pos-1){skip_typechecks = 1;}

		if(do_typechecks && !skip_typechecks){
			struct type_name *arg_expected = m(*(T->args), get, i+1);
			struct type_name *arg_got = res.T;
			/// checking compatibility
			const char *diag = 0;
			enum TypeCheckVal compat = get_type_compatibility(arg_expected, arg_got, IS_CALL, &diag);
			if((int)compat >= (int)TC_CONVERTIBLE_IMPL_NOOP){
				vec_printf(&vstr, " %s", res.val);
			}else if(compat == TC_CONVERTIBLE_IMPL){
				const char *converted_arg_val = emit_type_conversion(arg_expected, arg_got, res);
				vec_printf(&vstr, " %s", converted_arg_val);
			}else{
				assert(!"unreachable due to earlier typecheck");
			}
		}else{
			vec_printf(&vstr, " %s", res.val);
		}
	 }
	//3. actually emit the call
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("CALL %s %s%s // semantic_expr_op.c:620", 
		sanitize_string(exprResult), 
		sanitize_string(name), 
		sanitize_string(vstr.data));//buff);

	//4. phone home
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = resT, .author = "expr_call" };
	output_res(stg, result, NO_EMIT);
}
#define UNUSED(x) (void)x;

void semantic_analyze_expr_dot(ast_node* node, expr_settings stg) {
	//expr: expr '.' expr

	if (semantic_decl) { return; }
	
	PREP_RES(res1, E_PTR);
	res1stg.dest.author = "expr_dot object(o.)";
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr (first one)
	VERIFY_RES(res1);
	
	struct type_name *T = type_name_shallow_copy(res1.T);
	struct type_name *T2 = T;
	
	PREP_RES(res_this, E_RVAL);
	res_this.rv_type = E_RVAL;
	res_this.val = res1.val;
	UNUSED(res_thisstg);

	if(T->points_to){
		T2 = dereffed_type(T);
		res_this.T = T;
	}else{
		res_this.T = reffed_type(T);
	}

	if (!T2->symclass) {
		YYLTYPE pos = node->token.pos;
		err("line %d: [%s]\n", pos.first_line, get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
		error("semantic error: member access (x.y) impossible because x is a primitive (%s)\n", T->name);
	}
	
	
	if(T->points_to){ /// so that "ptr.x" is equivalent to "(*ptr).x"
		if(res1.val[0] == '&'){
			res1.val++;
			res_this.val = res1.val;
		}else{
			//res_this.val = res1.val;
			const char* deref_obj = IR_next_name(namespace_semantic, "temp");
			emit_code("MOV %s *%s // semantic_expr_op.c:670", sanitize_string(deref_obj), sanitize_string(res1.val));
			res1.val = deref_obj;
			res_this.val = res1.val;
		}
		res1.T = T2;
	}else{
		res1.T = T;

		if(stg.out_sem_this){
			//vector2_char vstr = vector2_char_here();
			if(res1.val[0] == '&'){
					/// referencing further causes pointer-to-pointer-to-obj,
					/// implying that the original type should have had T->pointed_to...
				//const char *ref_obj = IR_next_name(namespace_semantic, "temp");
				//emit_code("MOV %s %s // semantic_expr_op.c:681", sanitize_string(ref_obj), sanitize_string(res1.val));
				//vec_printf(&vstr, "&%s", ref_obj);
				//res_this.val = stralloc(vstr.data);
				res_this.val = res1.val;
			}else{
				/// ok so in case of arr[0].foo(), our res_this is temp = (&(arr[0]))
				/// we don't want to add another & and get &&(arr[0])
				/// is there a case where we want a & and how to distinguish them?
				/// nope, doesn't look like it.
				
				//vec_printf(&vstr, "&%s", res1.val);
				//res_this.val = stralloc(vstr.data);
				res_this.val = res1.val;
			}
		}
	}
	if(stg.out_sem_this){
		*stg.out_sem_this = res_this;//res1;
	}
	

	//push_symbol_table();
	//currentSymbolTable = T->symclass->symclass.scope;
	//no, we don't want to switch symbol table
	//because in A.foo(x), x should not be shadowed
	//by A.x.

	PREP_RES(res2, E_ASIS);//E_PTR);//E_RVAL);
	res2stg.sem_this = res1;

	//if(strcmp(res1.val, "&derp1")==0){
	//	printf("debug breakpoint");
	//}
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
		emit_code("MOV %s %s // semantic_expr_op.c:730", sanitize_string(resultExpr), sanitize_string(res1.val));
		emit_code("ADD %s %s 1 // semantic_expr_op.c:731", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, result, res1type);
		val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T, .author = author };
		output_res(stg, result, NO_EMIT);
	}
	else {
		//pre-increment
		emit_code("ADD %s %s 1 // semantic_expr_op.c:738", sanitize_string(res1.val), sanitize_string(res1.val));
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
		emit_code("MOV %s %s // semantic_expr_op.c:761", sanitize_string(resultExpr), sanitize_string(res1.val));
		emit_code("SUB %s %s 1 // semantic_expr_op.c:762", sanitize_string(res1.val), sanitize_string(res1.val));
		//output_res(stg, result, res1type);
		val_handle result = { .val = resultExpr, .rv_type = E_LVAL, .T = res1.T, .author = author };
		output_res(stg, result, NO_EMIT);
	}
	else {
		//pre-decrement
		emit_code("SUB %s %s 1 // semantic_expr_op.c:762", sanitize_string(res1.val), sanitize_string(res1.val));
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
	emit_code("NEG %s %s // semantic_expr_op.c:791", sanitize_string(resultExpr), sanitize_string(res1.val));
	
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
	const char* exprResult = res1.val;

	if(res1.val[0] == '*'){
		const char *temp = IR_next_name(namespace_semantic, "temp");
		emit_code("MOV %s %s // semantic_expr_op.c:855", temp, res1.val);
		exprResult = temp;
	}

	//reinterpret
	exprResult = rename_star(exprResult);
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
	emit_code("MOV %s %s // semantic_expr_op.c:860", resultExpr, res1.val);
	/// we take a value, remove adorations (ref or deref as necessary)
	/// and then present it as a plain number (RValue=read)

	/// output as ptr of (&x) kind
	val_handle result = { .val = /*res1.val*/resultExpr, .rv_type = E_RVAL/*E_PTR*/, .T = T2, .author = author };

	output_res(stg, result, NO_EMIT);
}

/*
/// if node refers to an array variable symbol, output that symbol to S and return 1. 
/// deprecated, check type_name instead
int check_if_array(struct ast_node *node, struct symbol **out_S){
	if(strcmp(node->token.type, "expr_id") == 0){
		struct symbol *S = lookup_symbol(node->token.value);
		if((S->type == SYMBOL_VARIABLE) || (S->type == SYMBOL_PARAM)){
			if(S->symvariable.array){
				*out_S = S;
				return 1;
			}
		}
	}
	return 0;
}
*/

extern int indent;

void semantic_analyze_expr_assign_brace_list(ast_node* node, expr_settings /*stg*/){
	assert(strcmp(ast_get_child(node,1)->token.type, "expr_braced_list") == 0);

	PREP_RES(res1, E_LVAL); //R=read, L=write //E_RVAL);
	res1stg.dest.author = "expr_assign_brace_list arg1 (... =)";
	semantic_expr_analyze(ast_get_child(node, 0), res1stg); //expr
	VERIFY_RES(res1);
	

	struct ast_node *ast_obj = ast_get_child(node,0);
	struct ast_node *ast_brace_list = ast_get_child(node,1);
	struct ast_node *ast_expr_list = ast_get_child(ast_brace_list,0);

	struct type_name *T = res1.T;
	if(T->symclass){
		emit_code("// brace-list assignment to object of class %d", T->symclass->username);
		struct symbol_table *scope = T->symclass->symclass.scope;
		
		int n_members = scope->symbols.size;
		
		indent++;
		int j = 0;
		for(int i = 0; i < n_members; i++){
			struct symbol *S2 = m(scope->symbols,get,i);
			//SYMBOL_ERROR, SYMBOL_VARIABLE, SYMBOL_PARAM, SYMBOL_MEMBER, SYMBOL_FUNCTION, SYMBOL_CLASS, SYMBOL_LABEL, SYMBOL_CONST
			if(S2->type == SYMBOL_MEMBER)
			{
				//obj.member = element
				struct ast_node *obj_elem = ast_gen_dot(ast_obj, ast_gen_id(S2->username));
				struct ast_node *list_elem = ast_get_child(ast_expr_list, j++);
				struct ast_node *ast_assign = ast_gen_assign(obj_elem, list_elem);
				
				PREP_RES(res2, E_DISCARD); //R=read, L=write //E_RVAL);
				res2stg.dest.author = "expr_assign_brace_list class element ( x.( ... ) = { (...), y, z} )";
				semantic_expr_analyze(ast_assign, res2stg); //expr
				VERIFY_RES(res2);
			}
		}
		indent--;
	}else if(T->is_array){
		/// wait, type_name can't represent arrays???
		/// we'll do a hack where we allow to assign directly to variable
		// if(S->symvariable.size == 1){
			// int array_size = S->symvariable.arraysize;
			// for(int i = 0; i < array_size; i++){
			// emit_code("// brace-list assignment to array (%s[%d])", S->username, array_size);
			// indent++;
			/// arr[i] = element
			// 	const char *arr = res1.val;
			// 	const char *element = (i < res2_list.size) ? m(res2_list, get, i).val : "0";
			// 	const char *arr_accessor = IR_next_name(namespace_semantic, "temp");
			// 	emit_code("ADD %s %s %d", arr_accessor, res1.val, i);
			// 	emit_code("MOV #%s, %d", arr_accessor, element);
			// }
			// indent--;
		// }
		int array_size = T->arraysize;
		int list_size = ast_expr_list->children.size;
		if(array_size != list_size){
			error("Semantic error: size mismatch: assigning a %d-element list to a %d-element array", list_size, array_size);
		}
		emit_code("// brace-list assignment to array[%d]", array_size);
		indent++;
		int j = 0;
		for(int i = 0; i < array_size; i++){
			/// arr[i] = element
			struct ast_node *arr_elem = ast_gen_index(ast_obj, ast_gen_num(i));
			struct ast_node *list_elem = ast_get_child(ast_expr_list, j++);
			struct ast_node *ast_assign = ast_gen_assign(arr_elem, list_elem);
			
			PREP_RES(res2, E_DISCARD); //R=read, L=write //E_RVAL);
			res2stg.dest.author = "expr_assign_brace_list arr element ( arr[ ... ] = { (...), y, z} )";
			semantic_expr_analyze(ast_assign, res2stg); //expr
			//VERIFY_RES(res2);
		}
		indent--;
	}
}

int safe_strcmp(const char *A, const char *B){
	if(A && B){return strcmp(A,B);}
	if(A && !B){return 1;}
	if(!A && B){return 1;}
	if(!A && !B){return 0;}
	assert(!"unreachable");
	return 1;
}

val_handle convert_rv_type(val_handle res_in, enum expr_type new_rv, const char *author){
	if(res_in.rv_type == new_rv){
		return res_in;
	}else{
		PREP_RES(res_out_, new_rv);
		res_out_stg.dest.author = author;
		output_res(res_out_stg, res_in, NO_EMIT);
		VERIFY_RES(res_out_);
		return res_out_;
	}
}

void semantic_analyze_expr_assign(ast_node* node, expr_settings stg) {
	//expr: expr '=' expr

	//ponder:
	// int *adr_scr;
	// adr_scr = 65536;  -> expr1 is a variable. we need to <var>
	// *adr_scr = 65536; -> expr1 is an address. if this address is calculated, we need to <*temp>
	//


	if (semantic_decl) { return; }

	if(strcmp(ast_get_child(node,1)->token.type,"expr_braced_list")==0){
		semantic_analyze_expr_assign_brace_list(node, stg); 
		return;
	}

	struct ast_node *dest = ast_get_child(node,0);
	struct ast_node *src = ast_get_child(node, 1);
	
	PREP_RES(res1, E_ASIS);//E_LVAL); //R=read, L=write //E_RVAL);
	res1stg.dest.author = "expr_assign arg1 (... =)";
	semantic_expr_analyze(dest, res1stg); //expr
	VERIFY_RES(res1);
	//assert(res1.val && (res1.val[0] == '*'));
	assert(res1.val);

	PREP_RES(res2, E_ASIS);//E_RVAL);//E_LVAL);
	res2stg.dest.author = "expr_assign arg2 (= ...)";
	semantic_expr_analyze(src, res2stg); //expr
	VERIFY_RES(res2);

	const char *exprSrc = 0;

	int size = getTypeSize(res2.T);
	if(size <= 1){
		res1 = convert_rv_type(res1, E_LVAL, "expr_assign(size=1) arg1 (...=   )");
		res2 = convert_rv_type(res2, E_RVAL, "expr_assign(size=1) arg2 (   =...)");
	}else{
		res1 = convert_rv_type(res1, E_PTR, "expr_assign(size=big) arg1 (...=   )");
		res2 = convert_rv_type(res2, E_PTR, "expr_assign(size=big) arg2 (   =...)");
	}

	if(do_typechecks){
		//typecheck_assign(res1, res2);
		struct type_name *T1 = res1.T; //dest
		struct type_name *T2 = res2.T; //src
		const char *T1_str = type_name_to_string(T1);
		const char *T2_str = type_name_to_string(T2);
		const char *diag = 0;
		enum TypeCheckVal compat = get_type_compatibility(T1, T2, NOT_A_CALL, &diag);
		if((int)compat >= (int)TC_CONVERTIBLE_EXPL){
			if((int)compat >= (int)TC_CONVERTIBLE_IMPL_NOOP)	{exprSrc = res2.val;}
			else												{exprSrc = emit_type_conversion(T1, T2, res2);}
		}else													{error("Semantic error: can't assign %s <- %s (%s)", T1_str, T2_str, diag);}
	}else{
		exprSrc = res2.val;
	}

	if(size <= 1){
		emit_code("MOV %s %s //=  // semantic_expr_op.c:1080",sanitize_string(res1.val), sanitize_string(exprSrc));
	}else{
		// maybe convert src from 
		emit_code("MEMCPY %s %s %d /= // semantic_expr_op.c:1083", sanitize_string(res1.val), sanitize_string(exprSrc), size);
	}
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
	emit_code("NOT %s %s // semantic_expr_op.c:1054", sanitize_string(exprResult), sanitize_string(res1.val));
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = res1.T, .author = author };
	output_res(stg, result, NO_EMIT);
}

void semantic_analyze_expr_cast(ast_node* node, expr_settings stg){
	/// tbh this is mostly a beurocratic operation to verify you really intend to do this.

	//expr_cast: '(' typename ')' expr
	if(semantic_decl){return;}
	const char* author = "expr_cast((type)x)";

	struct type_name *T1 = parseTypename(ast_get_child(node,0));

	PREP_RES(res1, E_RVAL);
	res1stg.dest.author = author;
	semantic_expr_analyze(ast_get_child(node, 1),res1stg);
	VERIFY_RES(res1);

	const char *exprResult = 0;

	struct type_name *T2 = res1.T;
		
	const char *T1_str = type_name_to_string(T1);
	const char *T2_str = type_name_to_string(T2);

	const char *diag = 0;
	enum TypeCheckVal compat = get_type_compatibility(T1, T2, NOT_A_CALL, &diag);
	if((int)compat >= (int)TC_CONVERTIBLE_EXPL){
		if((int)compat >= (int)TC_CONVERTIBLE_IMPL_NOOP)	{exprResult = res1.val;}
		else												{exprResult = emit_type_conversion(T1, T2, res1);}
	}else													{error("Semantic error: can't cast %s <- %s (%s)", T1_str, T2_str, diag);}

	assert(exprResult);
	val_handle result = { .val = exprResult, .rv_type = E_LVAL, .T = T1, .author = author };
	output_res(stg, result, NO_EMIT);
}
