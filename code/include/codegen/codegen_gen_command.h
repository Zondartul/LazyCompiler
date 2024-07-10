#ifndef CODEGEN_GEN_COMMAND_GUARD
#define CODEGEN_GEN_COMMAND_GUARD

void gen_command_symbol();
void gen_command_function();
void gen_command_label();
void gen_command_ret();
void gen_command_jmp();
void gen_command_jne();
void gen_command_je();
void gen_command_call();
void gen_command_mov();
void gen_command_add();
void gen_command_sub();
void gen_command_div();
void gen_command_mul();
void gen_command_mod();
void gen_command_and();
void gen_command_or();
void gen_command_not();
void gen_command_neg();
void gen_command_deref();
void gen_command_equal();
void gen_command_notequal();
void gen_command_greater();
void gen_command_less();
void gen_command_alloc();
void gen_command_struct();
void gen_command_using();
void gen_command_insert();
void gen_command_frame();
void gen_command_debug();
void gen_command_comment();
void gen_command_floor();

#endif
