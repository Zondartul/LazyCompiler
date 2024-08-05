#include "asm_template.h"
#include "stringUtils.h"
#include "assert.h"
#include "globals.h"
#include "codegen.h"

int asm_template_lookup_val_idx(struct asm_template *templ, const char *val){
	for(int i = 0; i < templ->vals.size; i++){
		const char *val2 = m(templ->vals, get, i);
		if(strcmp(val2, val)==0){
			return i;
		}
	}
	return -1;
}

/// returns the register associated with a value if there is one
const char* asm_template_lookup_val(struct asm_template *templ, const char *val){
	int idx = asm_template_lookup_val_idx(templ, val);
	if(idx != -1){
		return m(templ->regs, get, idx);
	}else{
		return 0;
	}
}

struct asm_template asm_template_here0(){
	struct asm_template tmpl;
	tmpl.seq_begin = vector2_ptr_char_here();
	tmpl.seq_end = vector2_ptr_char_here();
	tmpl.src_text = 0;
	tmpl.res_text = 0;
	tmpl.vals = vector2_ptr_char_here();
	tmpl.regs = vector2_ptr_char_here();
	return tmpl;
}

struct asm_template parse_asm_template(const char *str){
	/// find escape sequences
	/// $$ -> $
	/// $S[x] -> storeValue(x, reg)
	/// $L[x] -> reg = loadLValue(x)
	/// $R[x] -> reg = loadRValue(x)
	struct asm_template tmpl = asm_template_here0();
	tmpl.src_text = str;
	tmpl.res_text = stralloc(str);
	/// create a template along the way
	const char *str_iter = str;
	while(1){
		const char *seq_begin = str_iter;
		const char *seq_end = 0;
		const char *p = strchr(str_iter, '$');
		if(p){
			char C1 = *p;
			assert(C1 == '$');
			seq_begin = p;
			str_iter = p++;
			char C2 = *p;
			if(C2 == '$'){
					seq_end = p+1; 
					goto save_seq;
			}else if(
				(C2 == 'S') || 
				(C2 == 'L') ||
				(C2 == 'R')){
					p++;
					if(*p != '['){goto bad_escape;}
					p = strchr(str_iter, ']');
					if(!p){goto bad_escape;}
					str_iter = p++;
					seq_end = p;
					goto save_seq;
			}else if(C2 == 0){goto bad_escape;}
			else{
				error("CODE GEN: asm template error: bad escape sequence [%c%c]",C1,C2);
			}
		}else{break;}
		assert(!"unreachable");
		bad_escape:
		error("CODE GEN: asm template error: bad escape sequence starting from [%s]", seq_begin);
		save_seq:
		m(tmpl.seq_begin, push_back, seq_begin);
		m(tmpl.seq_end, push_back, seq_end);
	}		
	return tmpl;
}

char asm_templ_seq_type(struct asm_template *tmpl, int idx){
	return m(tmpl->seq_begin, get, idx)[1];
}

const char *asm_templ_val_name(struct asm_template *tmpl, int idx){
	///  $S[hello]
	///  ^  |    |^
	///  p  |    |p2
	///   p+3    p2-1
	///  len = 5 = (p2-1) - (p+3) = 5

	const char *p = m(tmpl->seq_begin, get, idx);
	const char *p2 = m(tmpl->seq_end, get, idx);
	const char *n_begin = p+3;
	const char *n_end = p2-1;
	//int len = n_end - n_begin;
	//char *valname = (char*)malloc(len+1);
	//memcpy(valname, n_begin,len);
	//valname[len] = 0;
	//return valname;	
    return substr(n_begin, n_end);
}

void checkTemplateVals(struct asm_template *tmpl){
	for(int i = 0; i < tmpl->seq_begin.size; i++){
		char Ct = asm_templ_seq_type(tmpl, i);
		if(Ct == 'S'){
			const char *valname = asm_templ_val_name(tmpl, i);
			checkResult(valname);
		}
	}
}

void load_template_registers(struct asm_template *tmpl){
	for(int i = 0; i < tmpl->seq_begin.size; i++){
		char Ct = asm_templ_seq_type(tmpl, i);
		if(Ct == 'R'){
			const char *valname = asm_templ_val_name(tmpl, i);
			const char *prev_reg = asm_template_lookup_val(tmpl, valname);
            printf("asm_template::load_template_registers(R): valname = [%s], prev_reg = [%s]\n", valname, prev_reg);
			if(!prev_reg){
				const char *reg = loadRValue(valname);
                assert(reg);
                printf("  push val/reg [%s|%s]\n", valname, reg);
				m(tmpl->vals, push_back, valname);
				m(tmpl->regs, push_back, reg);
			}else{
                printf("   already known val/reg [%s|%s]\n", valname, prev_reg);
            }
        }
		else if(Ct == 'L'){
			const char *valname = asm_templ_val_name(tmpl, i);
			const char *prev_reg = asm_template_lookup_val(tmpl, valname);
            printf("asm_template::load_template_registers(L): valname = [%s], prev_reg = [%s]\n", valname, prev_reg);
			if(!prev_reg){
				const char *reg = loadLValue(valname);
                assert(reg);
                printf("  push val/reg [%s|%s]\n", valname, reg);
				m(tmpl->vals, push_back, valname);
				m(tmpl->regs, push_back, reg);
			}else{
                printf("   already known val/reg [%s|%s]\n", valname, prev_reg);
            }
        }
		else if(Ct == 'S'){
			const char *valname = asm_templ_val_name(tmpl, i);
			const char *prev_reg = asm_template_lookup_val(tmpl, valname);
            printf("asm_template::load_template_registers(S): valname = [%s], prev_reg = [%s]\n", valname, prev_reg);
			if(!prev_reg){
				ptr_reg  R = allocRegister();
                const char *reg = R->name;
                assert(reg);
                printf("  push val/reg [%s|%s]\n", valname, reg);
				m(tmpl->vals, push_back, valname);
				m(tmpl->regs, push_back, reg);
			}else{
                printf("   already known val/reg [%s|%s]\n", valname, prev_reg);
            }
        }
		else if(Ct == '$'){
			/// do nothing
            printf("asm_template::load_template_registers($$): do nothing\n");
		}else{assert(!"unreachable");}
	}
}

void store_template_registers(struct asm_template *tmpl){
	for(int i = 0; i < tmpl->seq_begin.size; i++){
		char Ct = asm_templ_seq_type(tmpl, i);
		if(Ct == 'S'){
			const char *valname = asm_templ_val_name(tmpl, i);
			int val_idx = asm_template_lookup_val_idx(tmpl, valname);
			const char *reg = m(tmpl->regs, get, val_idx);
			if(reg){
				storeValue(valname, reg);
				m(tmpl->regs, set, 0, val_idx);
			}else{
                error("CODE GEN: asm_template::store_template_registers: no register associated with value");
            }
		}
	}
}

char *substr(const char *idx_from, const char *idx_to){
	int len = (int)(idx_to - idx_from);
	char *buff = (char*)malloc(len+1);
	memcpy(buff, idx_from, len);
    buff[len] = 0;
	return buff;
}

const char *asm_templ_get_seq_replacement(struct asm_template *tmpl, int idx){
	const char *valname = asm_templ_val_name(tmpl, idx);
	char Ct = asm_templ_seq_type(tmpl, idx);
	if(Ct == '$'){return "$";}
	else{
		const char *reg = asm_template_lookup_val(tmpl, valname);
		return reg;
	}
}

void stitch_asm_template(struct asm_template *tmpl){
	/// calculate the string size
	/// set size for the longest reg-name+1
	/// (max(strlen(regs))+1) * n_sequences

	/// or just use vec printf to allocate the string as we go
	
	vector2_char vstr = vector2_char_here();
	int src_len = strlen(tmpl->src_text);
	const char *prev_end = tmpl->src_text;
	const char *end_of_text = tmpl->src_text + src_len;
    printf("stitch_asm_template begin;\n");
	for(int i = 0; i < tmpl->seq_begin.size; i++){
		/// [raw text]? + [seq replacement]?
		const char* seq_begin = m(tmpl->seq_begin, get, i);
		const char* seq_end = m(tmpl->seq_end, get, i);
            const char *seq = substr(seq_begin, seq_end);
            const char *val = asm_templ_val_name(tmpl, i);
		/// returns "" if nothing there
		const char *raw_text_snip = substr(prev_end, seq_begin);
		/// also returns "" if repl is empty
		const char *repl = asm_templ_get_seq_replacement(tmpl, i);
            printf("  i = %d, text before = [%s], seq = [%s]\n", i, raw_text_snip, seq);
            printf("     val = [%s], repl = [%s]\n", val, repl);
        assert(repl != 0);
		prev_end = seq_end;
		vec_printf(&vstr, "%s%s",raw_text_snip, repl);
	}
	/// there is raw text before and after all the replacements
	/// this is the last 'after' block.
	if(prev_end < end_of_text){
		char *raw_text_snip = substr(prev_end, end_of_text);
		vec_printf(&vstr, "%s", raw_text_snip);
	}
	tmpl->res_text = stralloc(vstr.data);
}
