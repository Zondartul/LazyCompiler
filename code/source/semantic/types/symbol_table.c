#include "types/symbol_table.h"
#include "semantic.h"

implementation_vector_of(ptr_symbol_table);


struct symbol_table symbol_table_default(){
	struct symbol_table S;
	S.name = 0;
	S.node = 0;
	vector2_ptr_symbol_constructor(&(S.symbols));
	S.parent = 0;
	return S;
}

struct symbol_table symbol_table_here(
	const char *name,			
	struct ast_node *node,		
	vector2_ptr_symbol symbols, 
	struct symbol_table *parent
	){
	struct symbol_table S;
	S.name = name;
	S.node = node;
	S.symbols = symbols;
	S.parent = parent;
	return S;
}

struct symbol_table* symbol_table_new0(){
	struct symbol_table *S = (struct symbol_table*)malloc(sizeof(struct symbol_table));
	*S = symbol_table_default();
	return S;
}

struct symbol_table* symbol_table_new(
	const char *name,			
	struct ast_node *node,		
	vector2_ptr_symbol symbols, 
	struct symbol_table *parent
	){
	struct symbol_table *S = (struct symbol_table*)malloc(sizeof(struct symbol_table));
	S->name = name;
	S->node = node;
	S->symbols = symbols;
	S->parent = parent;
	return S;
}
//---------------------
struct symbol_table *currentSymbolTable;

vector2_ptr_symbol_table ST_stack;
vector2_ptr_symbol_table ST_list;

void push_symbol_table(){
	printf("symbol table %s saved\n",currentSymbolTable->name);
	m(ST_stack,push_back,currentSymbolTable);
}

void pop_symbol_table(){
	currentSymbolTable = m(ST_stack,pop_back);
	printf("symbol table %s loaded\n",currentSymbolTable->name);
}

//creates a new symbol table in the context of the semantic analyzer
//and sets it as current.
void new_symbol_table(struct ast_node *node){
	struct symbol_table *T = symbol_table_new0();//malloc(sizeof(struct symbol_table));
	T->name = IR_next_name(namespace_semantic,"ST");
	printf("new symbol table: %s\n",T->name);
	T->symbols = vector2_ptr_symbol_here();
	//vector_constructor(&T->symbols,sizeof(struct symbol));
	T->parent = currentSymbolTable;
	T->node = node;
	m(ST_list,push_back,T);
	currentSymbolTable = T;
}

//fetches a symbol table that corresponds to the given node
//from the listof all symbol tables
ptr_symbol_table find_symbol_table_by_node(struct ast_node *node){
	int i;
	if(node == 0){return 0;}
	for(i = 0; i <ST_list.size; i++){
		ptr_symbol_table ST = m(ST_list,get,i);
		if(ST->node == node){
			return ST;
		}
	}
	return 0;
}


const char* formatAsTable(const char* str);

void print_symbol_table_helper_short(struct symbol_table *T, int indent){
	padprint(indent);
	if(!T->parent){
		printf("Symbol table '%s':\n", T->name);
	}else{
		printf("Symbol table '%s'->'%s':\n", T->name,T->parent->name);
	}
	//const char *symbol = 0; //unused
	//const char *name = 0; //unused
	//const char *ir_name = 0; //unused
	//const char *storage;
	//const char *type = 0; //unused
	//const char *scope = 0; //unused
	//const char *code = 0; //unused
	//char *pos; //unused
	//printf("symbol\tname\tir_name\ttype\tscope\tcode\tpos\n");
	
	//size_t buff_size = 80 * T->symbols.size;
	//char* buff = malloc(buff_size);
	//char* buffp;
	//char* buff_watermark = buff + buff_size - 80;
	vector2_char vstr = vector2_char_here();
	int i = 0;
	int j = 0;
	for(j = 0; j < T->symbols.size; j++){
		struct symbol *S2 = m(T->symbols,get,j);
		if(S2->type == SYMBOL_CLASS){
			printf("classes:\n");
			vec_printf(&vstr, "name\tir_name\tscope\n");

			for(i = 0; i < T->symbols.size; i++){
				struct symbol *S = m(T->symbols,get,i);
				if(S->type == SYMBOL_CLASS){
					vec_printf(&vstr, "%s\t%s\t%s\n", S->username, S->IR_name, S->symclass.scope->name);
				}
			}
			const char* tbl = formatAsTable(vstr.data);//(buff);
			printf("%s\n", tbl);
		break;
		}
	}
	
	for(j = 0; j < T->symbols.size; j++){
		struct symbol *S2 = m(T->symbols,get,j);
		if(S2->type == SYMBOL_FUNCTION){
			printf("functions:\n");
			m(vstr, clear);
			vec_printf(&vstr, "type\tname\tir_name\tscope\tcode\n");
			for(i = 0; i < T->symbols.size; i++){
				struct symbol *S = m(T->symbols,get,i);
				
				if(S->type == SYMBOL_FUNCTION){
					const char *type = "?";
					if(S->symfunction.returntype){type = S->symfunction.returntype->name;}
					const char *name = S->username;
					const char *ir_name = S->IR_name;
					const char *scope = "?";
					if(S->symfunction.scope){scope = S->symfunction.scope->name;}
					const char *code = "?";
					if(S->symfunction.code){code = S->symfunction.code->name;}
					vec_printf(&vstr, "%s\t%s\t%s\t%s\t%s\n", type, name, ir_name, scope, code);
				}
			}
			const char* tbl = formatAsTable(vstr.data);//(buff);
			printf("%s\n", tbl);
			break;
		}
	}
	
	for(j = 0; j < T->symbols.size; j++){
		struct symbol *S2 = m(T->symbols,get,j);
		if(S2->type == SYMBOL_PARAM){
			m(vstr, clear);
			printf("parameters:\n");
			vec_printf(&vstr, "type\tname\tir_name\tsize\tpos\n");

			for(i = 0; i < T->symbols.size; i++){
				struct symbol *S = m(T->symbols,get,i);
				if(S->type == SYMBOL_PARAM){
					const char *typename = S->symvariable.type->name;	vec_printf(&vstr, "%s\t", typename);
					const char *username = S->username;					vec_printf(&vstr, "%s\t", username);
					const char *IRname = S->IR_name;					vec_printf(&vstr, "%s\t", IRname);
					int size = S->symvariable.size;						vec_printf(&vstr, "%d\t", size);
					int addr = S->store_adr;							vec_printf(&vstr, "%d\n", addr);
				}
			}
		const char* tbl = formatAsTable(vstr.data);//(buff);
		printf("%s\n", tbl);
		break;
		}
	}
	
	
	for(j = 0; j < T->symbols.size; j++){
		struct symbol *S2 = m(T->symbols,get,j);
		if(S2->type == SYMBOL_MEMBER){
			m(vstr, clear);
			printf("members:\n");
			vec_printf(&vstr, "type\tname\tir_name\tsize\tpos\n");

			for(i = 0; i < T->symbols.size; i++){
				struct symbol *S = m(T->symbols,get,i);
				if(S->type == SYMBOL_MEMBER){
					vec_printf(&vstr, "%s\t%s\t%s\t%d\t%d\n", S->symvariable.type->name, S->username, S->IR_name, S->symvariable.size, S->store_adr);
				}
			}
			const char* tbl = formatAsTable(vstr.data);//(buff);
			printf("%s\n", tbl);
			break;
		}
	}
	
	
	for(j = 0; j < T->symbols.size; j++){
		struct symbol *S2 = m(T->symbols,get,j);
		if(S2->type == SYMBOL_VARIABLE){
			m(vstr, clear);
			printf("variables:\n");
			vec_printf(&vstr, "type\tname\tir_name\tsize\tpos\n");
			for(i = 0; i < T->symbols.size; i++){
				struct symbol *S = m(T->symbols,get,i);
				if(S->type == SYMBOL_VARIABLE){
					vec_printf(&vstr, "%s\t%s\t%s\t%d\t%d\n", S->symvariable.type->name, S->username, S->IR_name, S->symvariable.size, S->store_adr);
				}
			}
		const char* tbl = formatAsTable(vstr.data);//(buff);
		printf("%s\n", tbl);
		break;
		}
	}
}
void print_symbol_table_helper(struct symbol_table *T, int indent){
	padprint(indent); 
	if(!T->parent){
		printf("Symbol table '%s':\n", T->name);
	}else{
		printf("Symbol table '%s'->'%s':\n", T->name,T->parent->name);
	}
	int i;
	for(i = 0; i < T->symbols.size; i++){
		struct symbol *S = m(T->symbols,get,i);//vector_get_reference(&T->symbols,i);
		padprint(indent); printf("symbol [%s]: ",S->username);
		switch(S->type){
			case(SYMBOL_VARIABLE):
				printf("variable\n");
				padprint(indent); printf(" IR_name = %s\n",S->IR_name);
				padprint(indent); printf(" type = %s\n", S->symvariable.type->name);
			break;
			case(SYMBOL_PARAM):
				printf("parameter\n");
				padprint(indent); printf(" IR_name = %s\n",S->IR_name);
				padprint(indent); printf(" type = %s\n", S->symvariable.type->name);
				padprint(indent); printf(" pos = %d\n", S->symvariable.pos);
			break;
			case(SYMBOL_FUNCTION):
				printf("function\n");
				padprint(indent); printf(" IR_name = %s\n",S->IR_name);
				padprint(indent); printf(" return type = %s\n", S->symfunction.returntype->name);
				padprint(indent); printf(" scope = %s\n", S->symfunction.scope->name);
				padprint(indent); printf(" code = %s\n", S->symfunction.code->name);
				//print_symbol_table_helper(S->symfunction.scope, indent+1);
			break;
			case(SYMBOL_CLASS):
				printf("class\n");
				padprint(indent); printf(" IR_name = %s\n",S->IR_name);
				padprint(indent); printf(" scope = %s\n", S->symclass.scope->name);
				//print_symbol_table_helper(S->symclass.scope,indent+1);
			break;
			default:
				error("semantic error: unknown switch case");
				break;
		};
	}
	padprint(indent); printf("End Symbol table\n");
}
void print_symbol_table(struct symbol_table *T){
	print_symbol_table_helper_short(T,0);
}
