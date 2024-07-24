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

struct type_name *T_char;
struct type_name *T_int;
struct type_name *T_float;
struct type_name *T_void;
struct type_name *T_void_ptr;
struct type_name *T_string;

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
enum TypeCheckVal get_type_compatibility(struct type_name *T_dest, struct type_name *T_src, int is_call);
int all_compatible(vector2_ptr_type_name *args_expect, vector2_ptr_type_name *args_got, int first, int last, int is_call);

///-------------------- operator typechecking --------

typedef const char *(*op_func)(val_handle res1, val_handle res2);

typedef struct TypeOpStrategy{
	struct type_name *in_T1;
	const char *op;
	struct type_name *in_T2;
	int is_symmetric;	/// 1 if operation can be done, 0 otherwise
	struct type_name *arg1_promote_to; /// what type to promote args to
	struct type_name *arg2_promote_to;
	struct type_name *result_type;
	op_func *strategy;
} TypeOpStrategy;

typedef TypeOpStrategy* ptr_TypeOpStrategy;

definition_vector_of(ptr_TypeOpStrategy);

TypeOpStrategy *TypeOpStrategy_new0();
TypeOpStrategy *TypeOpStrategy_new();

void init_op_typechecking();
