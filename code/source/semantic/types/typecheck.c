#include "typecheck.h"
#include "assert.h"
#include "semantic_expr_op.h" //for dereffed_type

struct type_name *T_char = 0;
struct type_name *T_int = 0;
struct type_name *T_float = 0;
struct type_name *T_void = 0;
struct type_name *T_void_ptr = 0;
struct type_name *T_string = 0;

void init_typechecking(){
	T_char = gen_type_name_char();
	T_int = gen_type_name_int();
	T_float = gen_type_name_float();
	T_void = gen_type_name_void();
	T_void_ptr = gen_type_name_void_ptr();
	T_string = gen_type_name_string();

	init_op_typechecking();
}

struct type_name *gen_type_name_char(){struct type_name *T = type_name_new0(); T->name = "char"; return T;}
struct type_name *gen_type_name_int(){struct type_name *T = type_name_new0(); T->name = "int"; return T;}
struct type_name *gen_type_name_float(){struct type_name *T = type_name_new0(); T->name = "float"; return T;}
struct type_name *gen_type_name_void(){struct type_name *T = type_name_new0(); T->name = "void"; return T;}
struct type_name *gen_type_name_void_ptr(){struct type_name *T = type_name_new0(); T->points_to = gen_type_name_void(); return T;}
struct type_name *gen_type_name_string(){struct type_name *T = type_name_new0(); T->name = "string"; return T;}


int is_pointer(struct type_name *T){return (T->points_to != 0);}
int is_array(struct type_name *T){return T->is_array;}
int is_function(struct type_name *T){return (T->args != 0);}
int is_class(struct type_name *T){return (T->symclass != 0);}
int is_primitive(struct type_name *T){return (!is_pointer(T)) && (!is_array(T)) && (!is_function(T)) && (!is_class(T));}

int is_char(struct type_name *T){return is_primitive(T) && T->name && (strcmp(T->name, "char")==0);}
int is_char_ptr(struct type_name *T){return T->points_to &&is_char(T->points_to);}
int is_int(struct type_name *T){return is_primitive(T) && T->name && (strcmp(T->name, "int")==0);}
int is_float(struct type_name *T){return is_primitive(T) && T->name && (strcmp(T->name, "float")==0);}
int is_void(struct type_name *T){return is_primitive(T) && T->name && (strcmp(T->name,"void")==0);}
int is_void_ptr(struct type_name *T){return T->points_to && is_void(T->points_to);}
int is_string(struct type_name *T){return is_primitive(T) && T->name && (strcmp(T->name,"string")==0);}

int is_numeric(struct type_name *T){return is_char(T) || is_int(T) || is_float(T);}
int is_integer(struct type_name *T){return is_char(T) || is_int(T);}
int is_indexible(struct type_name *T){return is_pointer(T) || is_array(T) || is_string(T);}

const char *emit_type_conversion(struct type_name *T_dest, struct type_name *T_src, val_handle src_res){
	//if(strcmp(src_res.val, "*temp_244")==0){
	//	printf("debug breakpoint");
	//}
	const char *dest_str = 0;
	const char *src_str = 0;
    if(T_dest->points_to){
        dest_str = "I32"; // type for pointers
    }else{
        if(is_char (T_dest))    {dest_str = "U8";}
        if(is_int  (T_dest))	{dest_str = "I32";}
        if(is_float(T_dest))    {dest_str = "F64";}
    }
    if(T_src->points_to){
        src_str = "I32";
    }else{
        if(is_char (T_src))		{src_str = "U8";}
        if(is_int  (T_src))		{src_str = "I32";}
        if(is_float(T_src))     {src_str = "F64";}
    }
    
	if(!dest_str || !src_str){error("[INTERNAL] bad arguments for type conversion");}
	const char *exprRes = IR_next_name(namespace_semantic, "temp");
	emit_code("CONVERT %s %s %s %s // typecheck.c:46", exprRes, src_res.val, src_str, dest_str);
	return exprRes;
}

enum TypeCheckVal get_type_compatibility(struct type_name *T_dest, struct type_name *T_src, int is_call, const char** out_diag){
	enum TypeCheckVal compat = TC_ERROR;
	/// hierarchy of type parameters:
	/// array of... 	(is_array, arraysize)
	///   pointer to..	(pointer_to != 0)
	///	  XOR
	///   function 		(args != 0)
	///		class		(symclass != 0)
	///		or
	///		primitive
    ///       numeric (char, int, float )
    ///       string
    ///       void
	///		        literal	(is_literal)

	/// conversions:
	/// array <- not array - forbidden
	/// array <- array - if same size and TC_CONVERTIBLE_IMPL_NOOP contents
	///	not array <- array - forbidden
	/// if both not array, continue
	///
	/// pointer <- not pointer:
	///		pointer <- primitive: allowed as TC_CONVERTIBLE_EXPL (to manually set pointers)
    ///     pointer <- function: allowed as TC_CONVERTIBLE_IMPL_NOOP (as long as pointed-to type is the same as return type)
    ///     pointer to char <- string: allowed as TC_CONVERTIBLE_IMPL_NOOP
	///		otherwise:			  forbidden, diagnostic "try &"		
	///
	/// not pointer <- pointer:
	///		primitive <- pointer: allowed as TC_CONVERTIBLE_EXPL (to read the pointer as a number)
	///		otherwise:			  forbidden, diagnostic "try *"
	///
	/// pointer <- pointer - always TC_CONVERTIBLE_EXPL
	/// if both not pointer, continue
	///
	/// function <- function - only in call: TC_CONVERTIBLE_IMPL if all args IMPL or better.
	/// not in call: all function <-> forbidden
	/// if both not a function, continue
	///
	/// if class <- different class: forbidden (unless we ship user conversion operators)
	/// if both not a class, continue
	///
	/// both primitive:
	///		numeric <-> numeric: allowed as TC_CONVERTIBLE_IMPL or better
	///		void <->: in call, TC_SAME if same; not in call: forbidden
	///	
	///	not literal <- literal is TC_CONVERTIBLE_IMPL_NOOP if both-not-literal would be TC_SAME 
	/// literal <- forbidden
	#define DIAG(x) if(out_diag){*out_diag = x;}
	if(T_dest->is_array && !T_src->is_array){DIAG("non-array to array"); return TC_INCOMPATIBLE;}
	if(T_dest->is_array && T_src->is_array){
		if(T_dest->arraysize != T_src->arraysize){DIAG("arrays of different size"); return TC_INCOMPATIBLE;}
		struct type_name *T2_dest = type_name_shallow_copy(T_dest);
		struct type_name *T2_src = type_name_shallow_copy(T_src);
		T2_dest->is_array = 0; T2_dest->arraysize = 0;
		T2_src->is_array = 0; T2_src->arraysize = 0;
		enum TypeCheckVal compat = TC_ERROR;
		const char *diag = 0;
		compat = get_type_compatibility(T2_dest, T2_src, is_call, &diag);
		if((int)compat >= (int)TC_CONVERTIBLE_IMPL_NOOP){return compat;}
		else{DIAG("arrays of types that are too different"); return TC_INCOMPATIBLE;} // can't explicit-cast a whole array
	}
	if(!T_dest->is_array && T_src->is_array){DIAG("array to non-array"); return TC_INCOMPATIBLE;}
	/// both are not arrays

	if(T_dest->points_to && !T_src->points_to){
		if(is_numeric(T_src)){DIAG("number to pointer needs expicit cast"); return TC_CONVERTIBLE_EXPL;}
		//else if(is_void_ptr(T_dest) && T_src->args){return TC_CONVERTIBLE_IMPL_NOOP;}
        else if(T_src->args){
			if(type_name_equals(m(*(T_src->args), get, 0), T_dest->points_to)){return TC_CONVERTIBLE_IMPL_NOOP;}
			else{DIAG("pointer type does not match function return type"); return TC_INCOMPATIBLE;}
        }else if(is_char_ptr(T_dest) && is_string(T_src)){return TC_CONVERTIBLE_IMPL_NOOP;}
        else{DIAG("source not convertible to pointer"); return TC_INCOMPATIBLE;}
	}
	if(!T_dest->points_to && T_src->points_to){
		if(is_numeric(T_dest)){DIAG("pointer to number needs explicit cast"); return TC_CONVERTIBLE_EXPL;}
		else{DIAG("pointer not convertible to dest"); return TC_INCOMPATIBLE;}
	}
	if(T_dest->points_to && T_src->points_to){DIAG("pointers of different types need explicit cast"); compat = TC_CONVERTIBLE_EXPL; goto literal_check;}
	/// both are not pointers

	if(T_dest->args && T_src->args && is_call){
		if(all_compatible(T_dest->args, T_src->args, 1, 0, is_call)){compat = TC_CONVERTIBLE_IMPL; goto literal_check;}
		else{DIAG("function arguments mismatch"); return TC_INCOMPATIBLE;}
	}
	/// not in a call, can't assign to or from functions
	if(T_dest->args || T_src->args){DIAG("can't convert between functions and non-functions"); return TC_INCOMPATIBLE;}
	/// both are not functions

	if(T_dest->symclass && T_src->symclass){
		if(T_dest->symclass == T_src->symclass){compat = TC_SAME; goto literal_check;}
		else{DIAG("can't convert different classes"); return TC_INCOMPATIBLE;}
	}
	if(T_dest->symclass || T_src->symclass){
		DIAG("can't convert between class and non-class"); return TC_INCOMPATIBLE;
	}
	/// both not a class

	/// both primtive
	if(is_void(T_dest) && is_void(T_src) && is_call){compat = TC_SAME; goto literal_check;}
	if(is_void(T_dest) || is_void(T_src)){DIAG("can't convert to or from void"); return TC_INCOMPATIBLE;} /// can't assign to or from void
	/// not void

	/// both numeric
	if(is_numeric(T_dest) && is_numeric(T_src)){
		if(is_float(T_dest)){compat = TC_CONVERTIBLE_IMPL_NOOP; goto literal_check;} /// conversion to bigger types is always no-op, given zASM memory model
		if(is_int(T_dest) && is_char(T_src)){compat = TC_CONVERTIBLE_IMPL_NOOP; goto literal_check;}
		compat = TC_CONVERTIBLE_IMPL; goto literal_check;
	}

	/// strings?
	if(is_string(T_src)){
		/// should have been handled above in the is_pointer / is_ptr_char
		DIAG("A string can only be converted to a char pointer"); return TC_INCOMPATIBLE;
	}
	/// all cases checked, unless the type system changes we should never get down here
	assert(!"unreachable");

	literal_check:
    /// On this stage, the compatibility will either be returned as calculated previously,
    /// or adjusted if the types are a) exactly the same, or b) differ only in literal-ness.

	if(type_name_equals(T_dest, T_src)){return TC_SAME;}
	/// not exactly same
	if(T_dest->is_literal){DIAG("can't assign to literal"); return TC_INCOMPATIBLE;} // can't assign to literal
	if(!T_dest->is_literal && T_src->is_literal){
		/// try to un-literal the source
		struct type_name *T3_src = type_name_shallow_copy(T_src);
		T3_src->is_literal = 0;
		if(type_name_equals(T_dest, T3_src)){return TC_CONVERTIBLE_IMPL_NOOP;}
		/// else, the types are two different numeric primitives. continue.
	}
    /// else we go with the original compatibility
    return compat;
}

/// checks type compatibility for arguments first ... last-1.
int all_compatible(vector2_ptr_type_name *args_expect, vector2_ptr_type_name *args_got, int first, int last, int is_call){
	if(!last){last = args_expect->size;}
	//if(args_expect->size != args_got->size){return 0;}
    if(last > args_got->size){return 0;}
    for(int i = first; i < last; i++){
		struct type_name *T1 = m(*args_expect, get, i);
		struct type_name *T2 = m(*args_got, get, i);
		const char *diag = 0;
		enum TypeCheckVal compat = get_type_compatibility(T1, T2, is_call, &diag);
		if((int)compat < (int)TC_CONVERTIBLE_IMPL){return 0;}
	}
	return 1;
}

///--------------- operator type-checking -----------------

// typedef struct Type_LFC_Data{
// 	// expression input
// 	val_handle res1;
// 	const char *op;
// 	val_handle res2;
// 	//
// 	enum eLFCData job;
// 	enum eLFCData side;
// } LFCData;

LFCData LFCData_here0(){
	LFCData data;
	data.res1.rv_type = E_ERROR;
	data.op = 0;
	data.res2.rv_type = E_ERROR;
	data.job = LFCD_ERROR;
	data.side = LFCD_ERROR;
	return data;
}

struct Type_Literal_Filter_or_Calc LFC_here0(){
	LFC lfc;
	lfc.type = LFCT_ERROR;
	lfc.filter = 0;
	lfc.calc = 0;
	lfc.T = 0;
	return lfc;
}

struct Type_Literal_Filter_or_Calc LFC_any(){
	LFC lfc = LFC_here0();
	lfc.type = LFCT_ANY;
	return lfc;
}

struct Type_Literal_Filter_or_Calc LFC_literal(struct type_name *T){
	LFC lfc = LFC_here0();
	lfc.type = LFCT_LITERAL;
	lfc.T = T;
	return lfc;
}
struct Type_Literal_Filter_or_Calc LFC_filter(type_filter_func func){
	LFC lfc = LFC_here0();
	lfc.type = LFCT_FILTER;
	lfc.filter = func;
	return lfc;
}
struct Type_Literal_Filter_or_Calc LFC_calc(type_calculator_func func){
	LFC lfc = LFC_here0();
	lfc.type = LFCT_CALC;
	lfc.calc = func;
	return lfc;
}

implementation_vector_of(ptr_TypeOpStrategy);

vector2_ptr_TypeOpStrategy OpStrats;

TypeOpStrategy *TypeOpStrategy_new0(){
	struct TypeOpStrategy *strat = (TypeOpStrategy*)malloc(sizeof(TypeOpStrategy));
	strat->in_T1 = LFC_here0();
	strat->op = 0;
	strat->in_T2 = LFC_here0();
	strat->is_symmetric = 0;
	strat->arg1_promote_to = LFC_here0();
	strat->arg2_promote_to = LFC_here0();
	strat->strategy = 0;
	return strat;
}

TypeOpStrategy *TypeOpStrategy_new(
	LFC in_T1, const char *op, LFC in_T2, int allow,
	int is_symmetric, LFC arg1_promote_to, LFC arg2_promote_to, LFC result_type,
	op_func strategy)
{
	struct TypeOpStrategy *strat = (TypeOpStrategy*)malloc(sizeof(TypeOpStrategy));
	strat->in_T1 = in_T1;
	strat->op = op;
	strat->in_T2 = in_T2;
	strat->allow = allow;
	strat->is_symmetric = is_symmetric;
	strat->arg1_promote_to = arg1_promote_to;
	strat->arg2_promote_to = arg2_promote_to;
	strat->result_type = result_type;
	strat->strategy = strategy;
	return strat;
}

void strat_default(LFCData data, expr_settings stg){
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("%s %s %s %s // typecheck.c:264", 
		sanitize_string(data.op), 
		sanitize_string(exprResult), 
		sanitize_string(data.res1.val), 
		sanitize_string(data.res2.val)
			);
	*(stg.actual) = (val_handle){.author = "strat_default", .rv_type = E_RVAL, .T = 0, .val = exprResult};
}

void strat_integer_arithmetic(LFCData data, expr_settings stg){
	const char* temp1 = IR_next_name(namespace_semantic, "temp");
	emit_code("%s %s %s %s // typecheck.c:275", 
		sanitize_string(data.op), 
		sanitize_string(temp1), 
		sanitize_string(data.res1.val), 
		sanitize_string(data.res2.val)
			);
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("FLOOR %s %s // typecheck.c:282",
		sanitize_string(exprResult),
		sanitize_string(temp1)
			);
	*(stg.actual) = (val_handle){.author = "strat_default_arithmetic", .rv_type = E_RVAL, .T = 0, .val = exprResult};
}

void strat_err_noindex(LFCData data, expr_settings)
{error("Attempt to index into %s by [%s]", type_name_to_string(data.res1.T), type_name_to_string(data.res2.T));}

//int (*type_filter_func)(struct type_name *T);


int one_sided_filter(LFCData data, int (*func)(struct type_name *T)){
	assert(data.job == LFCD_FILTER);
	if(data.side == LFCD_SIDE_A){return func(data.res1.T);}
	else if(data.side == LFCD_SIDE_B){return func(data.res2.T);}
	assert(!"unreachable");
	return 0;
}

int Tff_is_integer	(LFCData data){return one_sided_filter(data, is_integer);}
int Tff_is_numeric	(LFCData data){return one_sided_filter(data, is_numeric);}
int Tff_is_indexible(LFCData data){return one_sided_filter(data, is_indexible);}
int Tff_is_pointer	(LFCData data){return one_sided_filter(data, is_pointer);}

int Tff_is_numeric_same(LFCData data){
	return type_name_equals(data.res1.T, data.res2.T) && is_numeric(data.res1.T) && is_numeric(data.res2.T);
}

int Tff_same(LFCData data){
	return type_name_equals(data.res1.T, data.res2.T);
}

struct type_name *Tcf_first(LFCData data){return data.res1.T;}

struct type_name *Tcf_number_promotion(LFCData data){
	struct type_name *T1 = data.res1.T;
	struct type_name *T2 = data.res2.T;

	assert(is_primitive(T1) && is_primitive(T2));
	if(is_float(T1) || is_float(T2)){return T_float;}
	if(is_int(T1) || is_int(T2)){return T_int;}
	if(is_char(T1) || is_char(T2)){return T_char;}
	assert(!"unreachable");
	return 0;
}

struct type_name *Tcf_index_result(LFCData data){
	return dereffed_type(data.res1.T); /// btw dereferencing void* is an error because we can't produce void
}

/// how strategy choice works:
/// 1 - see if the operation is forbidden (PS: forbidden ops not implemented yet)
/// 			check T1-T2, then T2-T1+symmetry, then T1-any, then any-T2, then any-T1+symmetry then any-any
///				and then OP, then "any"
///				- still run the strategy but but the strat is just an error message
/// 2 - see if there is an allowed strat match
///				check T-T, then T-any, then any-T, then any-any
///				then OP, then "any"
/// 3 - if nothing found, error "can't do this op with these types"
///
/// note: if "is-symmetric" is true, then types A-B == B-A
/// ...
/// actually, just check strategies in the order they are supplied

void init_op_typechecking(){
	int allow = 1;
	int forbid = 0;
	int yes_symmetry = 1;
	int no_symmetry = 0;
	//op_func no_strat = 0;
	//op_func strat_index = 0;
	//op_func strat_err_noindex = 0;

	LFC Ta_any = LFC_any();
	LFC Ta_no_promo = LFC_any();
	LFC Tl_int = LFC_literal(T_int);
	LFC Tf_same = LFC_filter(Tff_same);
	LFC Tf_integer = LFC_filter(Tff_is_integer);
	LFC Tf_numeric = LFC_filter(Tff_is_numeric);
	LFC Tf_numeric_same = LFC_filter(Tff_is_numeric_same);
	LFC Tf_pointer = LFC_filter(Tff_is_pointer);
	LFC Tf_indexable = LFC_filter(Tff_is_indexible);
	LFC Tc_num_promo = LFC_calc(Tcf_number_promotion);
	LFC Tc_index_result = LFC_calc(Tcf_index_result);
	LFC Tc_first = LFC_calc(Tcf_first);


	OpStrats = vector2_ptr_TypeOpStrategy_here();
	/// allow string[idx], ptr[idx], array[idx]
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_indexable, "INDEX",   Tf_integer,   
		allow, no_symmetry, Ta_no_promo, Ta_no_promo, /*->*/ Tc_index_result,  strat_default));
	/// forbid [] for anything else
	m(OpStrats, push_back, TypeOpStrategy_new(Ta_any,       "INDEX",   Ta_any,   
		forbid, no_symmetry, Ta_no_promo, Ta_no_promo, /*->*/ Ta_any,  strat_err_noindex));
	/// integer divisions
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_integer,   "DIV",   Tf_integer,   
		allow, yes_symmetry, Tc_num_promo, Tc_num_promo, /*->*/ Tc_num_promo,  strat_integer_arithmetic));
	/// integer exponents
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_integer,   "EXP",   Tf_integer,   
		allow, yes_symmetry, Tc_num_promo, Tc_num_promo, /*->*/ Tc_num_promo,  strat_integer_arithmetic));
	/// general comparison ops
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_same,  "GREATER", Tf_same, 
		allow, yes_symmetry, Ta_no_promo,  Ta_no_promo, /*->*/ Tl_int, strat_default));
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_same,  "LESS", Tf_same, 
		allow, yes_symmetry, Ta_no_promo, Ta_no_promo, /*->*/ Tl_int, strat_default));
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_same,  "EQUAL", Tf_same, 
		allow, yes_symmetry, Ta_no_promo, Ta_no_promo, /*->*/ Tl_int, strat_default));
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_same,  "NOTEQUAL", Tf_same, 
		allow, yes_symmetry, Ta_no_promo, Ta_no_promo, /*->*/ Tl_int, strat_default));
	/// normal arithmetic for any two same numeric types
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_numeric_same,  "any", Tf_numeric_same, 
		allow, yes_symmetry, Ta_no_promo,  Ta_no_promo, /*->*/ Tc_first, strat_default));
	/// normal arithmetic (any combination of numbers, integers or not)
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_numeric,  "any", Tf_numeric, 
		allow, yes_symmetry, Tc_num_promo,  Tc_num_promo, /*->*/ Tc_num_promo, strat_default));
	/// ... also write some rules for pointers
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_pointer,  "ADD", Tf_integer, 
		allow, yes_symmetry, Ta_no_promo,  Ta_no_promo, /*->*/ Tc_first, strat_default));
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_pointer,  "SUB", Tf_integer, 
		allow, yes_symmetry, Ta_no_promo,  Ta_no_promo, /*->*/ Tc_first, strat_default));

}

int op_matches(const char *op_template, const char *op_in){
	assert(op_template);
	assert(op_in);
	if(strcmp(op_template, "any")==0){return 1;}
	if(strcmp(op_template, op_in)==0){return 1;}
	return 0;
}


int LFC_matches(LFCData data, LFC lfc){
	assert(data.job == LFCD_FILTER);
	if(lfc.type == LFCT_ANY){return 1;}
	if(lfc.type == LFCT_FILTER){return lfc.filter(data);}
	if(lfc.type == LFCT_LITERAL){
		struct type_name *T = 0;
			 if(data.side == LFCD_SIDE_A){T = data.res1.T;}
		else if(data.side == LFCD_SIDE_B){T = data.res2.T;}
		else assert(!"unreachable");
		return type_name_equals(lfc.T, T);
	}
	assert(!"unreachable");
	return 0;
}

int strategy_matches(TypeOpStrategy *strat, LFCData data){
	LFCData data_sideA = data; data_sideA.side = LFCD_SIDE_A;
	LFCData data_sideB = data; data_sideB.side = LFCD_SIDE_B;

	if(op_matches(strat->op, data.op)){
		if(LFC_matches(data_sideA, strat->in_T1) && 
			LFC_matches(data_sideB, strat->in_T2))
			{return 1;}
		/// symmetry check
		if(strat->is_symmetric && 
			LFC_matches(data_sideB, strat->in_T1) && 
			LFC_matches(data_sideA, strat->in_T2))
			{return 1;}
	}
	return 0;
}

TypeOpStrategy *lookup_op_strategy(LFCData data){
	for(int i = 0; i < OpStrats.size; i++){
		TypeOpStrategy *strat = m(OpStrats, get, i);
		if(strategy_matches(strat, data)){
			return strat;
		}
	}
	return 0;
}

struct type_name *LFC_get_type(LFCData data, LFC lfc){
	if(data.job == LFCD_PROMOTE){
		 if(lfc.type == LFCT_ANY){
			if(data.side == LFCD_SIDE_A){return data.res1.T;}
			if(data.side == LFCD_SIDE_B){return data.res2.T;}
			}
	}
	if((data.job == LFCD_PROMOTE) || (data.job == LFCD_OUTPUT)){
		if(lfc.type == LFCT_LITERAL){return lfc.T;}
		if(lfc.type == LFCT_CALC){return lfc.calc(data);}
	}
	assert(!"unreachable");
}

val_handle run_strat_promotions(LFCData data, val_handle res_in, LFC lfc){
	val_handle res_out = res_in;
	assert(data.job = LFCD_PROMOTE);
	res_out.T = LFC_get_type(data, lfc);
	return res_out;
}


/// now we need to write the thing that actually calls these strategies...
void typecheck_op(val_handle res1, const char *op, val_handle res2, expr_settings stg){
	/// 1. get strategy
	/// 2. check if forbidden
	/// if allowed:
	/// 3. perform promotions
	/// 4. activate op strategy
	/// 5. output result and new type
	/// else (forbidden):
	/// 3. activate err strategy (noreturn)

	const char *type1_str = type_name_to_string(res1.T);
	const char *type2_str = type_name_to_string(res2.T);
	printf("type checking [%s] %s [%s]\n", type1_str, op, type2_str);

	LFCData data = LFCData_here0();
	data.res1 = res1;
	data.op = op;
	data.res2 = res2;

	data.job = LFCD_FILTER;
	TypeOpStrategy *strat = lookup_op_strategy(data);
	if(!strat){error("operator does not apply between these types: [%s] %s [%s]", type_name_to_string(res1.T), op, type_name_to_string(res2.T));}

	if(strat->allow){
		//struct type_name *T1 = res1.T;
		//struct type_name *T2 = res2.T;
		data.job = LFCD_PROMOTE;
		data.side = LFCD_SIDE_A;
		res1 = run_strat_promotions(data, res1, strat->arg1_promote_to);
		data.side = LFCD_SIDE_B;
		res2 = run_strat_promotions(data, res2, strat->arg2_promote_to);

		PREP_RES(res, E_ASIS);
		data.job = LFCD_DO_OP; data.side = LFCD_ERROR;
		(*strat->strategy)(data, resstg);
		VERIFY_RES(res);
		data.job = LFCD_OUTPUT; data.side = LFCD_ERROR;
		struct type_name *T = LFC_get_type(data, strat->result_type);

		val_handle result = {.author = "typecheck_op", .rv_type = res.rv_type, .T = T, .val = res.val};
		output_res(stg, result, NO_EMIT);
	}else{
		PREP_RES(res, E_ASIS);
		data.job = LFCD_ERROR;
		(strat->strategy)(data, resstg); /// this should be a strategy that only prints an error.

		assert(!"unreachable");
	}
}