#include "semantic.h"
#include "yaccin.tab.h" //for get_source_text2()
#include "ctype.h"
#include "semantic_analyze.h"
#include "semantic_expr_op.h"

void semantic_analyze_program(ast_node *node){
	//program :	decl_stmt_list	;
	
	//printf("got program\n");
	new_symbol_table(node);
	new_code_segment();
	push_code_segment();
	new_code_segment();
	init_CS = currentCodeSegment;
	pop_code_segment();
	semantic_decl = 1;
	analyze_scope(ast_get_child(node,0),&currentCodeSegment,0,&currentSymbolTable,0,0);
	semantic_decl = 0;
	analyze_scope(ast_get_child(node,0),&currentCodeSegment,0,&currentSymbolTable,0,1);

	semantic_finalize();
	printf("semantic: done\n");
}

const char* emit_push_label(const char* lbl) {
	const char* lbl2 = IR_next_name(namespace_semantic, lbl);
	push_expr(lbl2);
	emit_code("SYMBOL %s LABEL", lbl2);
	return lbl2;
}

void output_res(expr_settings stg, const char* res_val, struct type_name *T) {
	if (stg.res_type == E_DISCARD) {
		if (stg.res_out) { *(stg.res_out) = 0; }
		if (stg.res_out_type) {*(stg.res_out_type) = 0;}
		return;
	}
	if (!stg.res_out) {error("internal semantic error: res_out null");}
	if (!stg.res_out_type) {error("internal semantic error: res_out_type null");}
	
	*(stg.res_out) = res_val;
	*(stg.res_out_type) = T;
	//we also need to check if it's rval/lval
	//and also if we need to emit anything or not.
}

void semantic_analyze_decl_stmt_list(ast_node *node){
	//decl_stmt_list:	decl_stmt_list_ne | ;
	//decl_stmt_list_ne: decl_stmt_list_ne decl_stmt | decl_stmt;
	//...
	//linearized: decl_stmt_list: decl_stmt ... decl_stmt ;

	for(int i = 0; i < node->children.size; i++){
		semantic_general_analyze(ast_get_child(node, i)); //decl_stmt
	}

}

void semantic_analyze_decl_stmt(ast_node *node){
	//decl_stmt:		class_def | func_def | var_decl ';' ;
	push_expr("DISCARD");
	semantic_general_analyze(ast_get_child(node,0)); //class_def | func_def | var_decl
}

void semantic_analyze_func_def(ast_node *node){
	//const char* res_dest = pop_expr(); //assert_expr_res(res_dest);
	//assert_no_result(res_dest);
	//func_def	:	typename ID '('	var_decl_list ')' stmt_list END ;
	
	//if(!semantic_decl){return;}
	//printf("got func_def\n");
	/*
	func_def(ID)+3
	+-typename(type)/(<class> ID)
	| +- ptr_stars
	+-var_decl_list
	| +-var_decl_list_ne
	|   +- var_decl(ID)
	|   |  +-typename(type)/(<class> ID)
	|   |	   +- ptr_stars
	|   /
	|   +-var_decl_list_ne...
	+-stmt_list
	  +- stmt_list_ne
	  |  +- stmt
	  |  /
	  /  +- stmt_list_ne...
	  +-<empty>
	*/
	
	struct ast_node *node_func_def 		= node;
	struct ast_node *node_typename 		= ast_get_child(node_func_def,0);
	//struct ast_node *node_id			= ast_get_child(node_func_def,1);
	struct ast_node *node_var_decl_list	= ast_get_child(node_func_def,2);
	struct ast_node *node_stmt_list 	= ast_get_child(node_func_def,3);
	
	struct type_name *T = parseTypename(node_typename);//(ast_get_child(node,0));//semantic_get_type(ast_get_child(node,0)->token.value);
	const char *name = node->token.value;
	if(semantic_decl){
		struct symbol *S = symbol_new0();//new_symbol();
		S->type = SYMBOL_FUNCTION;
		S->storage = STORE_CODE;
		S->username = name;
		if(semantic_flatten){
			char buff[200];
			sprintf(buff,"_%s",name);
			S->IR_name = IR_next_name(namespace_semantic,buff);
		}else{
			S->IR_name = name;
		}
		//get arguments
		//struct symbol_table *ST = new_symbol_table();
		S->symfunction.returntype = T;
		push_symbol_table();
		new_symbol_table(node);
		S->symfunction.scope = currentSymbolTable;
		S->symfunction.code = 0;
		//m(ST_stack,push_back,currentSymbolTable);
		//if(semantic_context == SEMANTIC_MEMBER){
		if(symbolThis){
			//add param: this;
			struct type_name *T = semantic_get_type(symbolThis->username);
			T->pointerlevel = 1;
			char *name = "this";
			struct symbol *S;
			S = symbol_new0();//new_symbol();
			S->username = name;
			if(semantic_flatten){
				S->IR_name = IR_next_name(namespace_semantic,name);
			}else{
				S->IR_name = name;
			}
			S->type = SYMBOL_PARAM;
			S->symvariable.type = T;
			S->symvariable.array = 0;
			S->symvariable.arraysize = 0;
			//S->symvariable.pos = 0;
			S->store_adr = 0;
			S->symvariable.size = getTypeSize(T);
			S->storage = STORE_DATA_STACK;
			push_symbol(S);
		}
		semantic_context = SEMANTIC_PARAM;
			//use new separate symbol table;
		struct type_name *signature = malloc(sizeof(struct type_name));
		signature->name = 0;
		signature->symclass = 0;
		signature->args = vector2_ptr_type_name_new();
		//parameter as visible from scope VS signature parameter, is confusing.
		int i;
		//wait, shouldn't we go through var_decl_list_ne first?
		//no, it's linearized now
		fprintf(stderr,"func_def, node_var_decl_list.token.type = [%s]\n",node_var_decl_list->token.type);
		for(i = 0; i < /*list*/node_var_decl_list->children.size; i++){
			struct ast_node *arg = ast_get_child(node_var_decl_list,i);//(list,i);
			fprintf(stderr,"func_def, child of node_var_decl_list is [%s]\n",arg->token.type);
			//struct ast_node *arg_typename = ast_get_child(arg,0); //unused
			struct type_name *T2 = parseTypename(node_typename);//(ast_get_child(arg,0));//semantic_get_type(ast_get_child(arg,0)->token.value);
			m((*(signature->args)),push_back,T2);
			//push_expr(new_rval());//push_expr("NODISCARD");
			semantic_general_analyze(arg); //var_decl
		}
		S->symfunction.signature = signature;
		//S->size = 0;
			//join symbol table to definition
		semantic_context = SEMANTIC_NORMAL;
		//do not get code until imperative pass
		//currentSymbolTable = m(ST_stack,pop_back);
		pop_symbol_table();
		push_symbol(S);
		push_symbol_table();
		currentSymbolTable = S->symfunction.scope;
		analyze_scope(/*ast_get_child(node,2)*/node_stmt_list,0,0,&currentSymbolTable,0,0);
		pop_symbol_table();
		//ast_node *N =  ast_get_child(node,2);
		//ast_node *N1 = ast_get_child(N,0); //error, no children
		//ast_node *Nx = ast_get_child(N,N->children.size-1);
		//N is node_stmt_list
		//N1 is empty or stmt_list_ne
		//Nx is N1
		//YYLTYPE pos1 = N1->token.pos;
		//YYLTYPE pos2 = Nx->token.pos;
		//fprintf(stderr,"function [%s]\nfrom (file %s, line %d:%d)\nto (file %s, line %d:%d)\n",S->username,pos1.filename,pos1.first_line,pos1.first_column,pos2.filename,pos2.first_line,pos2.first_column);
	}else{
		printf("semantic_analyze (imperative) of func_def %s\n",name);
		struct symbol *S = lookup_symbol(name);
		if(S->type != SYMBOL_FUNCTION){error("semantic: '%s' is not a function\n",S->username);}
		//YYLTYPE pos1 = ast_get_child(node,0)->token.pos;
		//YYLTYPE pos2 = ast_get_child(node,1)->token.pos;	//note: do not get source text by start/end, as that includes #preprocessor directives, and those have been deleted
		//fprintf(stderr,"get_source_text(%d,%d,%s) = [%s]\n",pos1.start,pos1.end,pos1.filename,get_source_text2(pos1));//get_source_text(pos1.start,pos1.end,pos1.filename));
		//emit_code("/* %s %s(%s) */",get_source_text(pos1.start,pos1.end,pos1.filename), name, get_source_text(pos2.start,pos2.end,pos2.filename));
		
		//emit_code("/* %s %s(%s) */",get_source_text2(pos1), name, get_source_text2(pos2));
		
		const char *s_typename = get_source_text2(node_typename->token.pos);
		const char *s_args = get_source_text2(node_var_decl_list->token.pos);
		
		emit_code("/* %s %s(%s) */", s_typename, name, s_args);
		
		//emit_code("DEBUG BEGIN FUNC %s %s",S->IR_name, S->username);
		push_symbol_table();
		currentSymbolTable = S->symfunction.scope;
		//emit_all_declarations(); //the analyze_scope will handle symbols (they are inside a frame anyway)
		//emit_code("SYMBOL %s LABEL",S->IR_name);
		//emit_code("LABEL %s",S->IR_name);			
		emit_code("FUNCTION %s BEGIN",S->IR_name);			
		push_semantic_context();
		
		//const char *prev_semantic_this = semantic_this;
		//if(semantic_this){semantic_this = lookup_symbol(semantic_this)->IR_name;}
		//if(semantic_context == SEMANTIC_MEMBER){semantic_this = lookup_symbol("this")->IR_name;}
		pop_symbol_table();
		semantic_context = SEMANTIC_NORMAL;
		printf("analyze statement list...\n");
		analyze_scope(/*ast_get_child(node,2)*/ node_stmt_list,0,
						&S->symfunction.code,
						&S->symfunction.scope,
						0,0);
		printf("st.list done\n");
		//semantic_this = prev_semantic_this;
		semantic_this = 0;
		pop_semantic_context();
		emit_code_segment(S->symfunction.code);
		emit_code("RET");
		emit_all_undeclarations();
		emit_code("FUNCTION %s END",S->IR_name);
		//emit_code("DEBUG END FUNC %s",S->IR_name);
		emit_code("/* end */");
		printf("semantic_analyze (imperative) of func %s done\n",name);
	}
}

void semantic_analyze_var_decl_list(ast_node *node){
	//var_decl_list:	var_decl_list_ne | ;
	//var_decl_list_ne:	var_decl_list_ne ',' var_decl | var_decl;
	//...
	//linearized: var_decl ',' ... var_decl
	//printf("got var_decl_list\n");
	for(int i = 0; i < node->children.size; i++){
		//push_expr(new_rval());//push_expr("NODISCARD");
		semantic_general_analyze(ast_get_child(node, i)); //var_decl
	}
}

void semantic_analyze_var_decl(ast_node *node){

	//const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	//var_decl:	typename ID
	//| typename ID '[' expr ']'
	//| typename ID '=' expr
	//;

	if(!semantic_decl){return;}//goto semantic_exit;}
	//printf("got var_decl\n");
	struct type_name *T = parseTypename(ast_get_child(node,0));//semantic_get_type(ast_get_child(node,0)->token.value);
	const char *name = node->token.value;
	int array = 0;
	int arraysize = 0;
	if(node->token.production == 1){
		array = 1;
		struct ast_node *Nsize = ast_get_child(node,1);
		arraysize = atoi(Nsize->token.value);
		if(!arraysize){error("semantic error: array size has to be positive integer");}
	}
	struct symbol *S;// = lookup_symbol_here(name);
	// if(S){
		// error("semantic error: S is already defined");
	// }
	S = symbol_new0();//new_symbol();
	S->username = name;
	if(semantic_flatten){
		S->IR_name = IR_next_name(namespace_semantic,name);
	}else{
		S->IR_name = name;
	}
	S->symvariable.type = T;
	if(semantic_context == SEMANTIC_PARAM){
		S->type = SYMBOL_PARAM;
		//S->symparameter.pos = currentSymbolTable->symbols.size;
		//S->symvariable.pos = getNumParameters();
		S->store_adr = getNumParameters();
		S->symvariable.size = getTypeSize(T);
		S->symvariable.array = 0;
		S->storage = STORE_DATA_STACK;
		if(array){error("semantic error: arrays as function parameter not implemented ");}
	/*}else if(semantic_context == SEMANTIC_MEMBER){ //SEMANTIC_MEMBER isn't very descriptive anyway
		S->type = SYMBOL_MEMBER;
		//S->symvariable.pos = getNumMembers();
		S->store_adr = getNumMembers();
		S->symvariable.size = getTypeSize(T);
		S->symvariable.array = 0;
		S->storage = STORE_DATA_MEMBER;
	*/	
	}else{
		S->type = SYMBOL_VARIABLE;
		//S->symvariable.pos = getNumVariables();
		S->store_adr = getNumVariables();
		S->symvariable.size = getTypeSize(T);
		S->storage = STORE_DATA_STACK;
		S->symvariable.array = 0;
	}
	if(array){S->symvariable.array = 1; S->symvariable.type->pointerlevel++; S->symvariable.arraysize = arraysize;}
	if(!currentSymbolTable->parent){S->global = 1;}
	push_symbol(S);
	//put into symbol table
}

void semantic_analyze_var_decl_assign(ast_node *node, expr_settings stg){

	//const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//int discardResult = (strcmp(res, "DISCARD") == 0);

	//production | typename ID '=' expr
	//yacc refs	 |    $1    $2  $3  $4
	//children   |    (0)   (1)     (2)
	struct ast_node *node_typename = ast_get_child(node,0);
	//struct ast_node *node_id = ast_get_child(node,1);
	struct ast_node *node_expr = ast_get_child(node,2);
	
	struct symbol *S;
	if(semantic_decl){
		//printf("got var_decl\n");
		struct type_name *T = parseTypename(node_typename);//ast_get_child(node,0));//semantic_get_type(ast_get_child(node,0)->token.value);
		const char *name = node->token.value;
		S = symbol_new0();//new_symbol();
		S->username = name;
		/* if(semantic_flatten){
			S->IR_name = IR_next_name(namespace_semantic,name);
		}else{
			S->IR_name = name;
		}
		if(semantic_context == SEMANTIC_PARAM){
			S->type = SYMBOL_PARAM;
			S->symparameter.type = T;
			S->symparameter.pos = currentSymbolTable->symbols.size;
		}else{
			S->type = SYMBOL_VARIABLE;
			S->symvariable.type = T;
		} */
		if(semantic_flatten){
			S->IR_name = IR_next_name(namespace_semantic,name);
		}else{
			S->IR_name = name;
		}
		S->symvariable.type = T;
		if(semantic_context == SEMANTIC_PARAM){
			S->type = SYMBOL_PARAM;
			//S->symparameter.pos = currentSymbolTable->symbols.size;
			//S->symvariable.pos = getNumParameters();
			S->store_adr = getNumParameters();
			S->symvariable.size = getTypeSize(T);
			S->symvariable.array = 0;
			S->storage = STORE_DATA_STACK;
		/*}else if(semantic_context == SEMANTIC_MEMBER){
			S->type = SYMBOL_MEMBER;
			//S->symvariable.pos = getNumMembers();
			S->store_adr = getNumMembers();
			S->symvariable.size = getTypeSize(T);
			S->symvariable.array = 0;
			S->storage = STORE_DATA_MEMBER;*/
		}else{
			S->type = SYMBOL_VARIABLE;
			//S->symvariable.pos = getNumVariables();
			S->store_adr = getNumVariables();
			S->symvariable.size = getTypeSize(T);
			S->symvariable.array = 0;
			S->storage = STORE_DATA_STACK;
		}
		if(!currentSymbolTable->parent){S->global = 1;}
		push_symbol(S);
		//put into symbol table
		//will deal with assignment later; ignore for now.
		//semantic_analyze(ast_get_child(node,1)); //expr (assignment)
		//put expression as "initial value"?
	}else{
		if(semantic_context != SEMANTIC_MEMBER){
			S = lookup_symbol(node->token.value);
			const char *result1 = S->IR_name; struct type_name* result1type = S->symvariable.type;
			push_code_segment();
			if(!currentSymbolTable->parent){
				currentCodeSegment = init_CS;
			}
			//emit_code("INITIALIZER %s BEGIN",result1);
			//push_expr(new_lval()); //only L-values can be written down //push_expr("NODISCARD");
			const char* result2 = 0; struct type_name* result2type = 0;
			expr_settings stg2 = { .res_type = E_LVAL, .res_out = &result2, .res_out_type = &result2type };
			semantic_expr_analyze(node_expr, stg2); //expr (what to assign)
			//const char *result2 = pop_expr();
			emit_code("MOV %s %s", result1, result2);
			//emit_code("INITIALIZER %s END",result1);
			//if (!discardResult) { push_expr(result1); }
			output_res(stg, result1, result1type);
			pop_code_segment();
		}
	}
}

void semantic_analyze_stmt_list(ast_node *node){
	//stmt_list: stmt_list_ne | ;
	//stmt_list_ne: stmt_list_ne stmt | stmt;
	//...
	//linearized: stmt stmt ... stmt

	//printf("got stmt_list\n");
	for(int i = 0; i < node->children.size; i++){
		semantic_general_analyze(ast_get_child(node, i)); //stmt
	}
}

void semantic_analyze_stmt(ast_node *node){
	//stmt	:	imp_stmt	|	decl_stmt	;
	semantic_general_analyze(ast_get_child(node,0)); // imp_stmt | decl_stmt
}

void semantic_analyze_imp_stmt(ast_node *node){
	//imp_stmt:		if_block
	//| while_loop
	//| expr ';'
	//| RETURN ';'
	//| RETURN expr ';'
	//| for_loop
	//;

	//if(semantic_decl){goto semantic_exit;}
	////imperative pass: 
	//some things are now both declarative and imperative, oddly.
	switch(node->token.production){
		case(0)://if_block
			semantic_general_analyze(ast_get_child(node,0)); //if_block
			break;
		case(1)://while_loop
			semantic_general_analyze(ast_get_child(node,0)); //while_loop
			break;
		case(2)://expr
			if(semantic_decl){return;}//{goto semantic_exit;} //this one still imp-only though
			emit_code("/* %s */",
				/*get_source_text(node->token.pos.start,
								node->token.pos.end,
								node->token.pos.filename)*/
				removeComments(get_source_text2(node->token.pos)));
			push_expr("DISCARD");
			semantic_general_analyze(ast_get_child(node,0)); //expr (unusued, increment or function call)
			break;
		case(3)://return
			if(semantic_decl){return;}//{goto semantic_exit;}
			emit_code("/* %s */",
				/*get_source_text(node->token.pos.start,
								node->token.pos.end,
								node->token.pos.filename)*/
				removeComments(get_source_text2(node->token.pos)));
			emit_code("RET");
			break;
		case(4)://return expr
			if(semantic_decl){return;}//{goto semantic_exit;}
			emit_code("/* %s */",
				/*get_source_text(node->token.pos.start,
								node->token.pos.end,
								node->token.pos.filename)*/
				removeComments(get_source_text2(node->token.pos)));
			//push_expr(new_lval());//push_expr("NODISCARD");
			const char* res1 = 0; struct type_info* res1type = 0;
			expr_settings stg1 = { .res_type = E_RVAL, .res_out = &res1, .res_out_type = &res1type };
			semantic_expr_analyze(ast_get_child(node,0), stg1); //expr (what to return)
			emit_code("RET %s",res1);
			break;
		case(5)://for_loop
			semantic_general_analyze(ast_get_child(node,0)); //for_loop
			break;
		default:
			error("semantic error: unknown switch case");
			break;
	}
}

void semantic_analyze_if_block(ast_node *node){
	//if_block	:	if_then END |	if_then ELSE stmt_list END ;

	const char* if_exit = 0;
	const char* if_else = 0;
	if(semantic_decl){
		push_symbol_table();
		switch(node->token.production){
			case(0)://if_then END
			{
				if_settings stg1 = { .exit_label = if_exit, .else_label = if_else };
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				break;
			}
			case(1)://if_then ELSE stmt_list END
			{
				if_settings stg1 = { .exit_label = if_exit, .else_label = if_else };
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				new_symbol_table(ast_get_child(node, 1));
				analyze_scope(ast_get_child(node, 1), 0, 0, &currentSymbolTable, 0, 1);
				break;
			}
			default:
				error("semantic error: unknown switch case");
				break;
		}
		pop_symbol_table();
	}else{
		//imperative pass: 
		//const char *CSname; //unused
		push_symbol_table();
		switch(node->token.production){
			case(0)://if_then END
			{
				if_exit = emit_push_label("lbl_if_exit");
				//push_expr("NOELSE");
				if_settings stg1 = { .exit_label = if_exit, .else_label = 0, .out_exit_label = &if_exit, .out_else_label = &if_else };
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				//if_else = pop_expr(); assert_is_if_else(if_else);
				//if_exit = pop_expr(); assert_is_if_exit(if_exit);
				emit_code("LABEL %s", if_exit);
				emit_code("/* end if */");
				break;
			}
			case(1)://if_then ELSE stmt_list END
			{
				if_exit = emit_push_label("lbl_if_exit");
				if_else = emit_push_label("lbl_if_else");

				if_settings stg1 = { .exit_label = if_exit, .else_label = 0, .out_exit_label = &if_exit, .out_else_label = &if_else };
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				emit_code("/* else */");
				emit_code("LABEL %s", if_else);
				struct code_segment* CSinsert;
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 1));
				analyze_scope(ast_get_child(node, 1), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_else = pop_expr();	assert_is_if_else(if_else);
				//if_exit = pop_expr();	assert_is_if_exit(if_exit);
				emit_code("LABEL %s", if_exit);
				emit_code("/* end if */");
				break;
			}
			default:
				error("semantic error: unknown switch case");
				break;
		}
		pop_symbol_table();
	}
}

void semantic_analyze_if_then(ast_node *node, if_settings stg){
	//if_then:	IF '(' expr ')' stmt_list
	//| if_then ELSEIF '(' expr ')' stmt_list
	//;

	const char* if_exit = 0;
	const char* if_else = 0;
	if(semantic_decl){
		push_symbol_table();
		switch(node->token.production){
			case(0): //IF ( expr ) stmt_list
				new_symbol_table(ast_get_child(node,1));
				analyze_scope(ast_get_child(node,1),0,0,&currentSymbolTable,0,1);
				break;
			case(2): //if_then ELSEIF ( expr ) stmt_list
				//new_symbol_table(ast_get_child(node,1));
				//analyze_scope(ast_get_child(node,1),0,0,&currentSymbolTable,0);
				if_settings stg1 = { .exit_label = if_exit, .else_label = if_else, .out_exit_label = &if_exit, .out_else_label = &if_else };
				semantic_if_analyze(ast_get_child(node,0), stg1); //if_then
				new_symbol_table(ast_get_child(node,2));
				analyze_scope(ast_get_child(node,2),0,0,&currentSymbolTable,0,1);
				break;
			default:
				error("semantic error: unknown switch case");
				break;
		};
		pop_symbol_table();
	}else{
		//imperative pass: 
		//printf("got if_then\n");
		//const char *CSname; //unused
		YYLTYPE pos;
		push_symbol_table();

		//if_else = pop_expr();	assert_is_if_else(if_else);
		//if_exit = pop_expr();	assert_is_if_exit(if_exit);
		if_else = stg.else_label;
		if_exit = stg.exit_label;
		const char* nextLabel = if_else;
		if (strcmp(if_else, "NOELSE") == 0) { nextLabel = if_exit; }
		else { nextLabel = if_else; }

		switch(node->token.production){
			case(0): 
			{
				//IF ( expr ) stmt_list
				//(expr)
				pos = ast_get_child(node, 0)->token.pos;
				emit_code("/* if(%s) */", get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
				//push_expr(new_lval());//push_expr("NODISCARD");

				const char* res1 = 0; struct type_info* res1type = 0;
				expr_settings stg1 = { .res_type = E_LVAL, .res_out = &res1, .res_out_type = &res1type };
				semantic_expr_analyze(ast_get_child(node, 0), stg1); //expr (condition)
				const char* condition_result = res1;
				//const char* condition_result = pop_expr(); assert_temp_val(condition_result);
				//if then
				//const char *nextLabel = IR_next_name(namespace_semantic,"lbl_if_then");
				emit_code("/* this skips untrue if's */");
				emit_code("JE 0 %s %s", condition_result, nextLabel);
				emit_code("/* 'if' is true: */");

				//stmt_list
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 1));
				struct code_segment* CSinsert;
				analyze_scope(ast_get_child(node, 1), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_exit = IR_next_name(namespace_semantic,"lbl_if_exit");
				//emit_code("SYMBOL %s LABEL",if_exit);
				if (nextLabel != if_exit) {  
					emit_code("JMP %s", if_exit);
				} //else, we can skip the jmp as the next instruction is already at the end of the if-block.
				//push_expr(if_exit);
				//end
				//emit_code("LABEL %s", nextLabel);
				break;
			}
			case(2): 
			{
				//if_then ELSEIF ( expr ) THEN stmt_list
				//the inner if-then should use the same exit as us, but use us as the else.
				//push_expr(if_exit);
				const char* if_elseif = emit_push_label("lbl_if_elseif");

				//if_then
				if_settings stg1 = { .exit_label = if_exit, .else_label = if_elseif, .out_exit_label = &if_exit, .out_else_label = &if_elseif };
				semantic_if_analyze(ast_get_child(node, 0), stg1); //if_then
				//assert_is_if_else(pop_expr()); //discard the labels from inner if_then
				//assert_is_if_exit(pop_expr()); 
				//else if
				YYLTYPE pos = ast_get_child(node, 1)->token.pos;
				emit_code("/* else if(%s) */", get_source_text2(pos));//get_source_text(pos.start,pos.end,pos.filename));
				emit_code("LABEL %s", if_elseif);
				//( expr )
				//push_expr(new_lval());//push_expr("NODISCARD");

				const char* res2 = 0; struct type_info* res2type = 0;
				expr_settings stg2 = { .res_type = E_LVAL, .res_out = &res2, .res_out_type = &res2type };
				semantic_expr_analyze(ast_get_child(node, 1), stg2); //expr (condition)
				const char* condition_result = res2;
				//const char* condition_result = pop_expr(); assert_temp_val(condition_result);
				//then
				//const char* label1 = IR_next_name(namespace_semantic, "lbl_elseif_then");
				//emit_code("SYMBOL %s LABEL",label1);
				//emit_code("JE 0 %s %s", pop_expr(), label1);
				emit_code("/* this skips untrue if's */");
				emit_code("JE 0 %s %s", condition_result, nextLabel);
				emit_code("/* then */");
				//stmt_list
				currentSymbolTable = find_symbol_table_by_node(ast_get_child(node, 2));
				struct code_segment* CSinsert;
				analyze_scope(ast_get_child(node, 2), 0, &CSinsert, &currentSymbolTable, 0, 1);
				emit_code_segment(CSinsert);
				//if_exit = pop_expr(); //using existing exit
				if (nextLabel != if_exit) {
					emit_code("JMP %s", if_exit);
				} //else, we can skip the jmp as the next instruction is already at the end of the if-block.
				//push_expr(if_exit);
				//emit_code("LABEL %s", label1);
				break;
			}
			default:
				error("semantic error: unknown switch case");
				break;
		};
		//push_expr(if_exit);
		//push_expr(if_else);
		fprintf(stderr, "warning: if_then should probably output some stg stuff\n");
#pragma message("warning: if_then should output stg stuff\n");

		pop_symbol_table();
	}
}

void semantic_analyze_while_loop(ast_node *node){
	//while_loop	:	WHILE '(' expr ')' stmt_list END ;

	if(semantic_decl){
		push_symbol_table();
		new_symbol_table(node);
		analyze_scope(ast_get_child(node,1),0,0,&currentSymbolTable,0,1);
		pop_symbol_table();
	}else{
		YYLTYPE pos1 = ast_get_child(node,0)->token.pos;
		emit_code("/* while(%s) */",get_source_text2(pos1));//get_source_text(pos1.start,pos1.end,pos1.filename));
		const char *label1 = IR_next_name(namespace_semantic,"lbl_while_do");
		const char *label2 = IR_next_name(namespace_semantic,"lbl_while_exit");
		emit_code("SYMBOL %s LABEL",label1);
		emit_code("SYMBOL %s LABEL",label2);
		emit_code("LABEL %s",label1);
		//push_expr(new_lval());//push_expr("NODISCARD");
		const char* res1 = 0; struct type_info* res1type = 0;
		expr_settings stg1 = { .res_type = E_RVAL, .res_out = &res1, .res_out_type = &res1type }; 
		semantic_expr_analyze(ast_get_child(node,0), stg1); //expr (condition)
		//analyze_scope(ast_get_child(node,0),0,&CSinsert,0,0);
		//emit_code_segment(CSinsert);
		emit_code("JE 0 %s %s",res1, label2);
		emit_code("/* do */");
		push_symbol_table();
		currentSymbolTable = find_symbol_table_by_node(node);
		struct code_segment *CSinsert;
		analyze_scope(ast_get_child(node,1),0,&CSinsert,&currentSymbolTable,0,1);
		emit_code_segment(CSinsert);
		emit_code("JMP %s",label1);
		emit_code("LABEL %s",label2);
		emit_code("/* end while*/");
		pop_symbol_table();
	}
}

void semantic_analyze_for_loop(ast_node *node){
	//for_loop	:	FOR '(' stmt expr ';' expr ')' stmt_list END ;

	if(semantic_decl){
		push_symbol_table();
		new_symbol_table(node);
		semantic_general_analyze(ast_get_child(node,0)); //stmt (first)
		semantic_general_analyze(ast_get_child(node,3)); //stmt_list
		//analyze_scope(ast_get_child(node,3),0,0,&currentSymbolTable,0);
		pop_symbol_table();
	}else{
		YYLTYPE pos1 = ast_get_child(node,0)->token.pos;
		YYLTYPE pos2 = ast_get_child(node,1)->token.pos;
		YYLTYPE pos3 = ast_get_child(node,2)->token.pos;
		YYLTYPE pos4 = pos1;
		pos4.last_line = pos3.last_line;
		pos4.last_column = pos3.last_column;
		emit_code("/* for(%s) */",get_source_text2(pos4));//get_source_text(pos1.start,pos3.end,pos1.filename));
		push_symbol_table();
		currentSymbolTable = find_symbol_table_by_node(node);
		push_code_segment();
		new_code_segment();
		//emit_code("FRAME ENTER"); -- nope
		emit_code("/* %s */",removeComments(get_source_text2(pos1)));//get_source_text(pos1.start,pos1.end,pos1.filename));
		semantic_general_analyze(ast_get_child(node,0)); //stmt (int i = 0;)
		const char *loopCondition = IR_next_name(namespace_semantic,"lbl_for");
		const char *loopExit = IR_next_name(namespace_semantic,"lbl_for_exit");
		emit_code("SYMBOL %s LABEL",loopCondition);
		emit_code("SYMBOL %s LABEL",loopExit);
		//LOOP CONDITION
		emit_code("LABEL %s",loopCondition);
		emit_code("/* %s */",removeComments(get_source_text2(pos2)));//get_source_text(pos2.start,pos2.end,pos2.filename));
		//push_expr(new_lval());//push_expr("NODISCARD");
		const char* res1 = 0; struct type_info* res1type = 0;
		expr_settings stg1 = { .res_type = E_LVAL, .res_out = &res1, .res_out_type = &res1type }; 
		semantic_expr_analyze(ast_get_child(node,1), stg1); //expr (i < 10;)
		const char* condition = res1;
		//const char *condition = pop_expr();
		emit_code("JE 0 %s %s",condition,loopExit);
		emit_code("/* loop body */");
		//LOOP BODY
		semantic_general_analyze(ast_get_child(node,3)); //stmt_list
		emit_code("/* %s */",removeComments(get_source_text2(pos3)));//get_source_text(pos3.start,pos3.end,pos3.filename));
		//push_expr("DISCARD");
		semantic_general_analyze(ast_get_child(node,2)); //expr (i++)
		emit_code("JMP %s",loopCondition);
		emit_code("LABEL %s",loopExit);
		emit_all_deinitializers();
		//emit_code("FRAME LEAVE");
		emit_code("/* end for */");
		ptr_code_segment CS = currentCodeSegment;
		pop_code_segment();
		emit_code_segment(CS);
		pop_symbol_table();
	}
}

void semantic_analyze_class_def(ast_node *node){
	//const char* res_dest = pop_expr(); assert_expr_res(res_dest);
	//assert_no_result(res_dest);

	//production|	CLASS ID decl_stmt_list END
	//yacc refs |		  $1  $2     $3
	//child		|		  (0) (1)
	//struct ast_node *node_id  = ast_get_child(node,0);
	struct ast_node *node_dsl = ast_get_child(node,1);
	
	if(semantic_decl){
		//printf("got class_def\n");
		const char *name = node->token.value;
		struct symbol *S = symbol_new0();//new_symbol();
		push_symbol(S);
		S->username = name;
		S->storage = STORE_CODE;
		if(semantic_flatten){
			S->IR_name = IR_next_name(namespace_semantic,name);
		}else{
			S->IR_name = name;
		}
		S->type = SYMBOL_CLASS;
		//make new symbol table
		//struct symbol_table *ST = new_symbol_table();
		push_symbol_table();
		new_symbol_table(node);
		S->symclass.scope = currentSymbolTable;
		push_semantic_context();
		//semantic_context = SEMANTIC_MEMBER;
		symbolThis = S;
		semantic_this = S->IR_name;
		
		struct symbol *T = symbol_new0();//new_symbol();
		T->username = stralloc("this");
		T->IR_name = IR_next_name(namespace_semantic,"this");
		T->type = SYMBOL_PARAM; //wait a sec, wtf is a SYMBOL_MEMBER?
		T->storage = STORE_DATA_STACK; //no idea if this does anything
		T->store_adr = 0;
		T->global = 0;
		T->symvariable.pos = 0;
			struct type_name *Tn = malloc(sizeof(struct type_name));
			Tn->name = S->username;
			Tn->pointerlevel = 1;
			Tn->symclass = S;
			Tn->args = 0;
		T->symvariable.type = Tn;
		T->symvariable.array = 0;
		T->symvariable.arraysize = 0;
		
		push_symbol(T);
		
		semantic_general_analyze(node_dsl); //decl_stmt_list
		pop_semantic_context();
		class_def_finalize();
		semantic_this = 0;
		symbolThis = 0;
		pop_symbol_table();
		//embed into existing symbol table
		//push_symbol(S);
	}else{
		symbolThis = lookup_symbol(node->token.value);
		class_emit_start();
		
		push_symbol_table();
		//const char *name = node->token.value; //unused
		currentSymbolTable = symbolThis->symclass.scope; //lookup_symbol(name)->symclass.scope;
		//semantic_this = "ERROR";//lookup_symbol("this")->IR_name;//symbolThis->IR_name;//"this";
		
		semantic_context = SEMANTIC_MEMBER;
		semantic_general_analyze(node_dsl);//decl_stmt_list //this adds functions that were actually defined in code
		semantic_context = SEMANTIC_NORMAL;
		
		//semantic_this = 0;
		pop_symbol_table();
		class_emit_end();
		symbolThis = 0;
		
	}
}

#if 0
//--- random crap backup ------------

/*
int is_prefix_of(const char* str1, const char* str2) {
	return (strncmp(str1, str2, strlen(str1)) == 0);
}

int is_label_type(const char* lbl1, const char* lbl2) {
	return is_prefix_of(lbl2, lbl1);
}

int is_temp_val(const char* val) {
	return is_prefix_of("temp", val);
}

void assert_label_type(const char* lbl1, const char* lbl2) {
	if (!is_label_type(lbl1, lbl2)) {
		error("internal semantic error: got lbl [%s], expected one of [%s]", lbl1, lbl2);
	}
}

void assert_temp_val(const char* val) {
	if (!is_temp_val(val)) {
		error("internal semantic error: got [%s], expected a temporary value", val);
	}
}

void assert_is_if_exit(const char* lbl) {
	assert_label_type(lbl, "lbl_if_exit");
}

void assert_is_if_else(const char* lbl) {
	if (strcmp(lbl, "NOELSE") != 0) {
		assert_label_type(lbl, "lbl_if_else"); //elseif also matches
	}
}

int is_rval(const char* val) {
	return is_prefix_of("rval", val);
}

void assert_rval(const char* val) {
	if (!is_rval(val)) {
		error("internal semantic error: got [%s], expected RValue", val);
	}
}

int is_lval(const char* val) {
	return is_prefix_of("lval", val);
}

void assert_lval(const char* val) {
	if (!is_lval(val)) {
		error("internal semantic error: got [%s], expected LValue", val);
	}
}

int is_number(const char* val) {
	//1. check if it only has digits and format symbols
	int len = strlen(val);
	for (int i = 0; i < len; i++) {
		char c = val[i];
		if (isdigit(c) || (c == '.')) //how do we deal with 0x, 0c, 0b?
		{
			continue;
		}
		else {
			return 0;
		}
	}
	//2. maybe check if it actually reads? nah
	return 1;
}

int is_ival(const char* val) {
	return is_number(val);
}

int is_val(const char* val) {
	return is_lval(val) || is_rval(val) || is_ival(val);
}

void assert_val(const char* val) {
	if (!is_val(val)) {
		error("inernal semantic error: got [%s], expected val", val);
	}
}

int is_expr_res(const char* val) {
	return (strcmp(val, "DISCARD") == 0) || is_val(val);
}

void assert_expr_res(const char* val) {
	if (!is_expr_res(val)) {
		error("internal semantic error: got [%s], expected expr_res type");
	}
}

void assert_no_result(const char* val) {
	if (!(strcmp(val, "DISCARD") == 0)) {
		error("internal semantic error: expr_res is [%s] but only DISCARD is allowed because there is no result.");
	}
}


const char* new_rval() {return IR_next_name(namespace_semantic, "rval");}
const char* new_lval() {return IR_next_name(namespace_semantic, "lval");}

const char* to_prefixed_name(const char* prefix, const char *val) {
	char buff[80];
	snprintf(buff, "%s%s", prefix, val);
	const char* res = IR_next_name(namespace_semantic, buff);
	return res;
}

const char* to_rval_name(const char* val) { return to_prefixed_name("rval_", val); }
const char* to_lval_name(const char* val) { return to_prefixed_name("lval_", val); }
*/

//outputs the value res_val into the destination res_dest,
//changing r-v-qualification if necessary.
//returns 1 if something was outputted and 0 otherwise.
//... note: also maybe ask before emitting random code?
//old
/*
int output_res(const char* res_dest, const char* res_val) {
	assert_expr_res(res_dest);
	assert_val(res_val);
	if (strcmp(res_dest, "DISCARD")==0) {
		return 0;
	}
	else if (is_rval(res_dest)) {
		if (is_rval(res_val)) {
			emit_code("MOV %s %s // R<-R, ref copy ", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
		else if(is_lval(res_val)){

			//for both lvals and constants
			//emit("MOV %s &%s // R<-L, promote ", res_dest, res_val);
			error("internal semantic error: cannot promote Lvalue (%s) to Rvalue", res_val);
		}
		else {
			error("internal semantic error: cannot promote I-Lvalue (%s) to Rvalue", res_val);
		}
	}
	else if (is_lval(res_dest)) {
		if (is_rval(res_val)) {
			emit_code("MOV %s *%s // L<-R, demote/deref ", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
		else if (is_lval(res_val)) {
			emit_code("MOV %s %s // L<-L, val copy ", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
		else{
			//probably a costant
			emit_code("MOV %s %s // L<-I, load immediate ", res_dest, res_val);
			push_expr(res_dest);
			return 1;
		}
	}
	else {
		error("internal semantic error: can only write to rvals and lvals");
	}
	return 0; //unreachable
}
*/
#endif
