#include "globals.h"
#include "AST.h"
#include <stdarg.h>
#include <stdlib.h>
#include "string.h"

//implementation_vector_of(int);
vector2_ptr_ast_node ast_nodes;
//vector ast_nodes;
void ast_init(){
	ast_nodes = vector2_ptr_ast_node_here();
	//vector_constructor(&ast_nodes, sizeof(struct ast_node));
}
//create a new node, given a token, and 'n' child nodes.
//basically never use this, it sucks
/*
int make_node(struct ast_token token, int n, ...){
	va_list ap;
	printf("node \"%s\": [%s] (%d/%d)\n",token.type,token.value,token.production,n);
	struct ast_node *N = malloc(sizeof(ast_node));
	N->token = token;
	N->children = vector3_int_here();//vector2_int_here();
	//vector_constructor(&N->children, sizeof(int));
	va_start(ap, n);
	int i;
	for(i = 0; i < n; i++){
		//N->children[i] = va_arg(ap, struct ast_node*);
		int id = va_arg(ap, int);
		m(N->children,push_back,id);
		//vector_push_back(&N->children, &id);
	}
	va_end(ap);
	m(ast_nodes,push_back,N);
	//vector_push_back(&ast_nodes, (void *)N);
	return ast_nodes.size-1;
}
struct ast_node *ast_get_node(int id){
	//return (struct ast_node*)vector_get_reference(&ast_nodes, id);
	return m(ast_nodes,get,id);
}

int ast_get_child_id(struct ast_node *N, int num){
	//return *(int*)vector_get_reference(&N->children, num);
	if(N->children.size){
		return m(N->children, get, num);
	}else{
		fprintf(stderr,"warning: ast_get_child_id when node has no children\n");
		return 0;
	}
}
*/
struct ast_node *ast_get_child(struct ast_node *N, int num){
	//return ast_get_node(ast_get_child_id(N,num));
	//return ast_get_node(ast_get_child_id(N,num));
	if(!N){error("nullptr");}
	return m(N->children, get, num);
}
//+--+ fancy!
//++-� nuuu notepad broke my fancy
//�� �
//+--+
char pad[80];
int padI;
void ast_print_helper(struct ast_node *N, const char *value, FILE *fp, int num, int hasnext){
	printf("printing ast %p, %p, %d",N,fp,num);
	//char oldP = pad[padI-1]; //unused
	if(padI == 0){padI = 1;}
	if(hasnext){
		pad[padI-1] = '+';
		fprintf(fp,"%s",pad);
		pad[padI-1] = '|';
	}else{
		pad[padI-1] = '\\';
		fprintf(fp,"%s",pad);
		pad[padI-1] = ' ';
	}
	pad[padI++] = ' ';
	pad[padI] = 0;
	int i;
	if(N){
		printf(", [%s]\n",N->token.type);
		fprintf(fp, "%s", N->token.type);
		if(N->token.value){fprintf(fp, " \"%s\"", N->token.value);}
		fprintf(fp, "\n");
		//if(hasnext){pad[padI++] = '|'; pad[padI] = 0;}
		//else{pad[padI++] = ' '; pad[padI] = 0;}
		if(N->children.size){
			// if(N->token.value){
				// ast_print_helper(0,N->token.value,fp,0,1);
			// }
			for(i = 0; i < N->children.size; i++){
				ast_print_helper(ast_get_child(N, i),N->token.value,fp,i,(i+1) < N->children.size);
			}
		}
	}else{
		printf("\n");
		if(value){
			fprintf(fp, "\"%s\"\n",value);
		}else{
			fprintf(fp, "<no data>\n");
		}
	}
	if(padI){padI--;}
	pad[padI] = 0;
}
void ast_print(struct ast_node *N, FILE *fp){
	pad[0] = 0;
	padI = 0;
	ast_print_helper(N, 0, fp, 0,0);
}

void ast_print_graph_helper(struct ast_node *N, FILE *fp, const char *str){
	vector2_char vstr = vector2_char_here();
	//vector Names;
	int len = strlen(str);
	//int namelen = len+5;
	//char *namebuff = (char *)malloc(namelen);
	//strcpy(namebuff, str);
	//char *suffix = namebuff+len;
	//vector_constructor(&Names, namelen);
	vec_printf(&vstr, "%s", str);
	if(N->token.value){
		//fprintf(fp, "%s [label = \"%s\n\\\"%s\\\"\"]\n", namebuff, N->token.type, N->token.value);
	}else{
		//fprintf(fp, "%s [label = \"%s\"]\n", namebuff, N->token.type);
	}
	int i;
	for(i = 0; i < N->children.size; i++){
		//snprintf(suffix,4,"_%d",i);
		vec_printf(&vstr, "_%d", i);
		//ast_print_graph_helper(ast_get_child(N,i),fp,namebuff);
		ast_print_graph_helper(ast_get_child(N, i), fp, stralloc(vstr.data));
		if(len){
			//fprintf(fp, "%s -- ", str);
			//fprintf(fp, "%s\n", namebuff);
		}
	}
}
void ast_print_graph(struct ast_node *N, FILE *fp, const char *str){
	fprintf(fp, "graph program{\n");
	ast_print_graph_helper(N,fp,str);
	fprintf(fp, "}");
}
