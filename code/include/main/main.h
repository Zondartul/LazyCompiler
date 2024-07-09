#pragma once

typedef struct T_cli_options{
	const char *in_file;
	const char *out_file;
	int b_asm_comments;
	int b_asm_trace;
} T_cli_options;

extern T_cli_options cli_options;
