#include "semantic.h"

enum TypeCheckVal {
	TC_ERROR = 0, 					/// var not initialized
	TC_INCOMPATIBLE = 1,			/// types cannot be converted
	TC_CONVERTIBLE_EXPL = 2, 		/// explicit (cast) needed for conversion
	TC_CONVERTIBLE_IMPL = 3, 		/// implicit conversion is allowed
	TC_CONVERTIBLE_IMPL_NOOP = 4,	/// implicit conversion is a no-op
	TC_SAME = 5						/// types are exactly the same
	};

void init_typechecking();

#define IS_CALL 1
#define NOT_A_CALL 0

extern struct type_name *T_char;
extern struct type_name *T_int;
extern struct type_name *T_float;
extern struct type_name *T_void;
extern struct type_name *T_void_ptr;
extern struct type_name *T_string;

struct type_name *gen_type_name_char();
struct type_name *gen_type_name_int();
struct type_name *gen_type_name_float();
struct type_name *gen_type_name_void();
struct type_name *gen_type_name_void_ptr();
struct type_name *gen_type_name_string();
int is_char(struct type_name *T);
int is_char_ptr(struct type_name *T);
int is_int(struct type_name *T);
int is_float(struct type_name *T);
int is_void(struct type_name *T);
int is_void_ptr(struct type_name *T);
int is_string(struct type_name *T);
int is_numeric(struct type_name *T);

const char *emit_type_conversion(struct type_name *T_dest, struct type_name *T_src, val_handle src_res);
enum TypeCheckVal get_type_compatibility(struct type_name *T_dest, struct type_name *T_src, int is_call, const char **out_diag);
int all_compatible(vector2_ptr_type_name *args_expect, vector2_ptr_type_name *args_got, int first, int last, int is_call);

///-------------------- operator typechecking --------
////    LFC (Literal, Filter or Calculator) is a framework for functional programming of type checks and type conversions (by me :v)

enum eLFCData {LFCD_ERROR, LFCD_FILTER, LFCD_PROMOTE, LFCD_DO_OP, LFCD_OUTPUT, LFCD_SIDE_A, LFCD_SIDE_B, LFCD_OUT};


typedef struct Type_LFC_Data{
	// expression input
	val_handle res1;
	const char *op;
	val_handle res2;
	//
	enum eLFCData job;
	enum eLFCData side;
} LFCData;

LFCData LFCData_here0();

typedef void (*op_func)(LFCData data, expr_settings stg); /// ptr to function that returns an IR value after emitting some processing code
typedef int (*type_filter_func)(LFCData data); /// ptr to function that returns 1 or 0 depending on some properties of type T
typedef struct type_name *(*type_calculator_func)(LFCData data); /// calculates new type based on old types

enum eLFCType {LFCT_ERROR, LFCT_NONE, LFCT_ANY, LFCT_LITERAL, LFCT_FILTER, LFCT_CALC};

typedef struct Type_Literal_Filter_or_Calc{
	enum eLFCType type;
	struct type_name *T;
	type_filter_func filter;
	type_calculator_func calc;
} LFC;

struct Type_Literal_Filter_or_Calc LFC_here0();
struct Type_Literal_Filter_or_Calc LFC_any();
struct Type_Literal_Filter_or_Calc LFC_literal(struct type_name *T);
struct Type_Literal_Filter_or_Calc LFC_filter(type_filter_func func);
struct Type_Literal_Filter_or_Calc LFC_calc(type_calculator_func func);

typedef struct TypeOpStrategy{
	LFC in_T1; 		/// specific type or 0 for 'any'
	const char *op;		/// specific operator or "any" for 'any'
	LFC in_T2; 		/// specific type or 0 for 'any'
	int allow;			/// 1 if operation is allowed, 0 if forbidden
	int is_symmetric;	/// 1 if order of operands doesn't matter
	LFC arg1_promote_to; /// what type to promote args to
	LFC arg2_promote_to;
	LFC result_type; /// what the output type is
	op_func strategy;
} TypeOpStrategy;

typedef TypeOpStrategy* ptr_TypeOpStrategy;

definition_vector_of(ptr_TypeOpStrategy);

TypeOpStrategy *TypeOpStrategy_new0();
TypeOpStrategy *TypeOpStrategy_new();

void init_op_typechecking();

void typecheck_op(val_handle res1, const char *op, val_handle res2, expr_settings stg);