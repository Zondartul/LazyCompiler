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
	struct type_name *in_T1, const char *op, struct type_name *in_T2, 
	int is_symmetric, struct type_name *arg1_promote_to, struct type_name *arg2_promote_to,
	op_func strategy)
{
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

void init_op_typechecking(){
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "+", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "-", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "*", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "/", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "^", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "%%", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "any", T_int, 1, 0, 0, 0));
	
	m(OpStrats, push_back, TypeOpStrategy_new(T_float, "+", T_float, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_float, "-", T_float, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_float, "*", T_float, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_float, "/", T_float, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_float, "^", T_float, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_float, "%%", T_float, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_float, "any", T_float, 1, 0, 0, 0));
	
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "+", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "-", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "*", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "/", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "^", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "%%", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "any", T_int, 1, 0, 0, 0));
	
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "+", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "-", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "*", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "/", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "^", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "%%", T_int, 1, 0, 0, 0));
	m(OpStrats, push_back, TypeOpStrategy_new(T_int, "any", T_int, 1, 0, 0, 0));

}