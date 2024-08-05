#include "types.h"

struct asm_template{
	vector2_ptr_char seq_begin; // pointers to starts of escape sequences
	vector2_ptr_char seq_end;	// pointers to ends of escape sequences
	vector2_ptr_char vals;		// unique values encountered
	vector2_ptr_char regs;		// registers associated with those values
	const char *src_text;		// r/o copy of the source text
	char *res_text;				// resulting text with substitutions
};

int asm_template_lookup_val_idx(struct asm_template *templ, const char *val);
/// returns the register associated with a value if there is one
const char* asm_template_lookup_val(struct asm_template *templ, const char *val);

struct asm_template asm_template_here0();
struct asm_template parse_asm_template(const char *str);

char asm_templ_seq_type(struct asm_template *tmpl, int idx);
const char *asm_templ_val_name(struct asm_template *tmpl, int idx);
void checkTemplateVals(struct asm_template *tmpl);
void load_template_registers(struct asm_template *tmpl);
void store_template_registers(struct asm_template *tmpl);

char *substr(const char *idx_from, const char *idx_to);

const char *asm_templ_get_seq_replacement(struct asm_template *tmpl, int idx);

void stitch_asm_template(struct asm_template *tmpl);
