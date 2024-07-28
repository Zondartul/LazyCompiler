#include "typecheck.h"
#include "assert.h"

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

int is_char(struct type_name *T){return T->name && (strcmp(T->name, "char")==0);}
int is_char_ptr(struct type_name *T){return T->points_to &&is_char(T->points_to);}
int is_int(struct type_name *T){return T->name && (strcmp(T->name, "int")==0);}
int is_float(struct type_name *T){return T->name && (strcmp(T->name, "float")==0);}
int is_void(struct type_name *T){return T->name && (strcmp(T->name,"void")==0);}
int is_void_ptr(struct type_name *T){return T->points_to && is_void(T->points_to);}
int is_string(struct type_name *T){return T->name && (strcmp(T->name,"string")==0);}

int is_numeric(struct type_name *T){
	return is_char(T) || is_int(T) || is_float(T);
}

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

enum TypeCheckVal get_type_compatibility(struct type_name *T_dest, struct type_name *T_src, int is_call){
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
    ///     pointer to void <- function: allowed as TC_CONVERTIBLE_IMPL_NOOP
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

	if(T_dest->is_array && !T_src->is_array){return TC_INCOMPATIBLE;}
	if(T_dest->is_array && T_src->is_array){
		if(T_dest->arraysize != T_src->arraysize){return TC_INCOMPATIBLE;}
		struct type_name *T2_dest = type_name_shallow_copy(T_dest);
		struct type_name *T2_src = type_name_shallow_copy(T_src);
		T2_dest->is_array = 0; T2_dest->arraysize = 0;
		T2_src->is_array = 0; T2_src->arraysize = 0;
		enum TypeCheckVal compat = TC_ERROR;
		compat = get_type_compatibility(T2_dest, T2_src, is_call);
		if((int)compat >= (int)TC_CONVERTIBLE_IMPL_NOOP){return compat;}
		else{return TC_INCOMPATIBLE;} // can't explicit-cast a whole array
	}
	if(!T_dest->is_array && T_src->is_array){return TC_INCOMPATIBLE;}
	/// both are not arrays

	if(T_dest->points_to && !T_src->points_to){
		if(is_numeric(T_src)){return TC_CONVERTIBLE_EXPL;}
		else if(is_void_ptr(T_dest) && T_src->args){return TC_CONVERTIBLE_IMPL_NOOP;}
        else if(is_char_ptr(T_dest) && is_string(T_src)){return TC_CONVERTIBLE_IMPL_NOOP;}
        else{return TC_INCOMPATIBLE;}
	}
	if(!T_dest->points_to && T_src->points_to){
		if(is_numeric(T_dest)){return TC_CONVERTIBLE_EXPL;}
		else{return TC_INCOMPATIBLE;}
	}
	if(T_dest->points_to && T_src->points_to){compat = TC_CONVERTIBLE_EXPL; goto literal_check;}
	/// both are not pointers

	if(T_dest->args && T_src->args && is_call){
		if(all_compatible(T_dest->args, T_src->args, 1, 0, is_call)){compat = TC_CONVERTIBLE_IMPL; goto literal_check;}
		else{return TC_INCOMPATIBLE;}
	}
	/// not in a call, can't assign to or from functions
	if(T_dest->args || T_src->args){return TC_INCOMPATIBLE;}
	/// both are not functions

	if(T_dest->symclass && T_src->symclass){
		if(T_dest->symclass == T_src->symclass){compat = TC_SAME; goto literal_check;}
		else{return TC_INCOMPATIBLE;}
	}
	/// both not a class

	/// both primtive
	if(is_void(T_dest) && is_void(T_src) && is_call){compat = TC_SAME; goto literal_check;}
	if(is_void(T_dest) || is_void(T_src)){return TC_INCOMPATIBLE;} /// can't assign to or from void
	/// not void

	/// both numeric
	if(is_numeric(T_dest) && is_numeric(T_src)){
		if(is_float(T_dest)){compat = TC_CONVERTIBLE_IMPL_NOOP; goto literal_check;} /// conversion to bigger types is always no-op, given zASM memory model
		if(is_int(T_dest) && is_char(T_src)){compat = TC_CONVERTIBLE_IMPL_NOOP; goto literal_check;}
		compat = TC_CONVERTIBLE_IMPL; goto literal_check;
	}
	/// all cases checked, unless the type system changes we should never get down here
	assert(!"unreachable");

	literal_check:
    /// On this stage, the compatibility will either be returned as calculated previously,
    /// or adjusted if the types are a) exactly the same, or b) differ only in literal-ness.

	if(type_name_equals(T_dest, T_src)){return TC_SAME;}
	/// not exactly same
	if(T_dest->is_literal){return TC_INCOMPATIBLE;} // can't assign to literal
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
		enum TypeCheckVal compat = get_type_compatibility(T1, T2, is_call);
		if((int)compat < (int)TC_CONVERTIBLE_IMPL){return 0;}
	}
	return 1;
}

///--------------- operator type-checking -----------------

struct Type_Literal_Filter_or_Calc LFC_here0(){
	LFC lfc;
	lfc.is_literal = 0;
	lfc.is_calc = 0;
	lfc.is_filter = 0;
	lfc.filter = 0;
	lfc.calc = 0;
	lfc.T = 0;
	return lfc;
}
struct Type_Literal_Filter_or_Calc LFC_literal(struct type_name *T){
	LFC lfc = LFC_here0();
	lfc.is_literal = 1;
	lfc.T = T;
	return lfc;
}
struct Type_Literal_Filter_or_Calc LFC_filter(type_filter_func func){
	LFC lfc = LFC_here0();
	lfc.is_filter = 1;
	lfc.filter = func;
	return lfc;
}
struct Type_Literal_Filter_or_Calc LFC_calc(type_calculator_func_2 func){
	LFC lfc = LFC_here0();
	lfc.is_calc = 1;
	lfc.calc = func;
	return lfc;
}

implementation_vector_of(ptr_TypeOpStrategy);

vector2_ptr_TypeOpStrategy OpStrats;

TypeOpStrategy *TypeOpStrategy_new0(){
	struct TypeOpStrategy *strat = (TypeOpStrategy*)malloc(sizeof(TypeOpStrategy));
	strat->in_T1 = 0;
	strat->op = 0;
	strat->in_T2 = 0;
	strat->is_symmetric = 0;
	strat->arg1_promote_to = 0;
	strat->arg2_promote_to = 0;
	strat->strategy = 0;
	return strat;
}

TypeOpStrategy *TypeOpStrategy_new(
	LFC *in_T1, const char *op, LFC *in_T2, int allow,
	int is_symmetric, LFC *arg1_promote_to, LFC *arg2_promote_to, LFC *result_type,
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

void strat_default(val_handle res1, const char *op, val_handle res2, expr_settings stg){
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("%s %s %s %s // typecheck.c:264", 
		sanitize_string(op), 
		sanitize_string(exprResult), 
		sanitize_string(res1.val), 
		sanitize_string(res2.val)
			);
	*(stg.actual) = (val_handle){.author = "strat_default", .rv_type = E_RVAL, .T = 0, .val = exprResult};
}

void strat_integer_arithmetic(val_handle res1, const char *op, val_handle res2, expr_settings stg){
	const char* temp1 = IR_next_name(namespace_semantic, "temp");
	emit_code("%s %s %s %s // typecheck.c:275", 
		sanitize_string(op), 
		sanitize_string(temp1), 
		sanitize_string(res1.val), 
		sanitize_string(res2.val)
			);
	const char* exprResult = IR_next_name(namespace_semantic, "temp");
	emit_code("FLOOR %s %s // typecheck.c:282",
		sanitize_string(exprResult),
		sanitize_string(temp1)
			);
	*(stg.actual) = (val_handle){.author = "strat_default_arithmetic", .rv_type = E_RVAL, .T = 0, .val = exprResult};
}

void strat_err_noindex(val_handle res1, const char *, val_handle res2, expr_settings)
{error("Attempt to index into %s by [%s]", type_name_to_string(res1.T), type_name_to_string(res2.T));}

//int (*type_filter_func)(struct type_name *T);
int Tff_is_integer(struct type_name *T){
	return is_char(T) || is_int(T);
}

int Tff_is_numeric(struct type_name *T){
	return is_char(T) || is_int(T) || is_float(T);
}

int Tff_is_indexible(struct type_name *T){
	return (T->points_to) || (T->is_array) || is_string(T);
}

int Tff_is_pointer(struct type_name *T){
	return (T->points_to != 0);
}

struct type_name *Tcf_first(struct type_name *T1, struct type_name *T2){
	return T1;
}

struct type_name *Tcf_number_promotion(struct type_name *T1, struct type_name *T2){
	if(is_float(T1) || is_float(T2)){return T_float;}
	if(is_int(T1) || is_int(T2)){return T_int;}
	if(is_char(T1) || is_char(T2)){return T_char;}
	assert(!"unreachable");
	return 0;
}

struct type_name *Tcf_index_result(struct type_name *T1, struct type_name *T2){
	return dereffed_type(T1); /// btw dereferencing void* is an error because we can't produce void
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
	struct type_name *T_any = 0;
	struct type_name *no_promo = 0;
	int allow = 1;
	int forbid = 0;
	int yes_symmetry = 1;
	int no_symmetry = 0;
	op_func no_strat = 0;
	//op_func strat_index = 0;
	//op_func strat_err_noindex = 0;

	LFC Tl_any = LFC_literal(T_any);
	LFC Tl_no_promo = LFC_literal(no_promo);
	LFC Tf_integer = LFC_filter(Tff_is_integer);
	LFC Tf_numeric = LFC_filter(Tff_is_numeric);
	LFC Tf_pointer = LFC_filter(Tff_is_pointer);
	LFC Tf_indexable = LFC_filter(Tff_is_indexible);
	LFC Tc_num_promo = LFC_calc(Tcf_number_promotion);
	LFC Tc_index_result = LFC_calc(Tcf_index_result);
	LFC Tc_first = LFC_calc(Tcf_first);


	/// allow string[idx], ptr[idx], array[idx]
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_indexable, "INDEX",   Tf_integer,   
		allow, no_symmetry, Tl_no_promo, Tl_no_promo, /*->*/ Tc_index_result,  strat_default));
	/// forbid [] for anything else
	m(OpStrats, push_back, TypeOpStrategy_new(Tl_any,       "INDEX",   Tl_any,   
		forbid, no_symmetry, Tl_no_promo, Tl_no_promo, /*->*/ Tl_any,  strat_err_noindex));
	/// integer divisions
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_integer,   "DIV",   Tf_integer,   
		allow, yes_symmetry, Tc_num_promo, Tc_num_promo, /*->*/ Tc_num_promo,  strat_integer_arithmetic));
	/// integer exponents
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_integer,   "EXP",   Tf_integer,   
		allow, yes_symmetry, Tc_num_promo, Tc_num_promo, /*->*/ Tc_num_promo,  strat_integer_arithmetic));
	/// normal arithmetic (any combination of numbers, integers or not)
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_numeric,  "any", Tf_numeric, 
		allow, yes_symmetry, Tc_num_promo,  Tc_num_promo, /*->*/ Tc_num_promo, strat_default));
	/// ... also write some rules for pointers
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_pointer,  "ADD", Tf_integer, 
		allow, yes_symmetry, Tc_num_promo,  Tc_num_promo, /*->*/ Tc_first, strat_default));
	m(OpStrats, push_back, TypeOpStrategy_new(Tf_pointer,  "SUB", Tf_integer, 
		allow, yes_symmetry, Tc_num_promo,  Tc_num_promo, /*->*/ Tc_first, strat_default));

}

int op_matches(const char *op_template, const char *op_in){
	assert(op_template);
	assert(op_in);
	if(strcmp(op_template, "any")==0){return 1;}
	if(strcmp(op_template, op_in)==0){return 1;}
	return 0;
}

int LFC_matches(LFC lfc, val_handle res){
	if(lfc.is_literal && type_name_equals(lfc.T, res.T)){return 1;}
	if(lfc.is_filter && (*lfc.filter)(res.T)){return 1;}
	return 0;
}

int strategy_matches(TypeOpStrategy *strat, val_handle res1, const char *op, val_handle res2){
	if(op_matches(strat->op, op)){
		if(LFC_matches(*strat->in_T1, res1) && LFC_matches(*strat->in_T2, res2)){
			return 1;
		}
		/// symmetry check
		if(strat->is_symmetric && LFC_matches(*strat->in_T1, res2) && LFC_matches(*strat->in_T2, res1)){
			return 1;
		}
	}
	return 0;
}

TypeOpStrategy *lookup_op_strategy(val_handle res1, const char *op, val_handle res2){
	for(int i = 0; i < OpStrats.size; i++){
		TypeOpStrategy *strat = m(OpStrats, get, i);
		if(strategy_matches(strat, res1, op, res2)){
			return strat;
		}
	}
	return 0;
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

	TypeOpStrategy *strat = lookup_op_strategy(res1, op, res2);
	if(!strat){error("operator does not apply between these types: [%s] %s [%s]", type_name_to_string(res1.T), op, type_name_to_string(res2.T));}

	if(strat->allow){
		struct type_name *T1 = res1.T;
		struct type_name *T2 = res2.T;
		res1 = run_strat_promotions(*strat->arg1_promote_to, res1);
		res2 = run_strat_promotions(*strat->arg2_promote_to, res2);

		PREP_RES(res, E_ASIS);
		(*strat->strategy)(res1, res2, resstg);
		VERIFY_RES(res);
		struct type_name *T = LFC_get_type(strat->result_type, T1, T2);

		val_handle result = {.author = "typecheck_op", .rv_type = res.rv_type, .T = T, .val = res.val};
		output_res(stg, result, NO_EMIT);
	}else{
		PREP_RES(res, E_ASIS);
		(*strat->strategy)(res1, res2, resstg); /// this should be a strategy that only prints an error.

		assert(!"unreachable");
	}
}