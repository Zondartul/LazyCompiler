#include "semantic.h"

enum TypeCheckVal {
	TC_ERROR = 0, 					/// var not initialized
	TC_INCOMPATIBLE = 1,			/// types cannot be converted
	TC_CONVERTIBLE_EXPL = 2, 		/// explicit (cast) needed for conversion
	TC_CONVERTIBLE_IMPL = 3, 		/// implicit conversion is allowed
	TC_CONVERTIBLE_IMPL_NOOP = 4,	/// implicit conversion is a no-op
	TC_SAME = 5						/// types are exactly the same
	};
#define IS_CALL 1
#define NOT_A_CALL 0


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
