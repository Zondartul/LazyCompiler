#include "AST.h"
#include <stdarg.h>
#include <stdlib.h>
#include "string.h"
vector ast_nodes;
void ast_init(){
	vector_constructor(&ast_nodes, sizeof(struct ast_node));
}
//create a new node, given a token, and 'n' child nodes.
int node(struct ast_token token, int n, ...){
	va_list ap;
	printf("node \"%s\": [%s] (%d/%d)\n",token.type,token.value,token.production,n);
	struct ast_node *N = malloc(sizeof(struct ast_node));
	N->token = token;
	vector_constructor(&N->children, sizeof(int));
	va_start(ap, n);
	int i;
	for(i = 0; i < n; i++){
		//N->children[i] = va_arg(ap, struct ast_node*);
		int id = va_arg(ap, int);
		vector_push_back(&N->children, &id);
	}
	va_end(ap);
	vector_push_back(&ast_nodes, (void *)N);
	return ast_nodes.size-1;
}
struct ast_node *ast_get_node(int id){
	return (struct ast_node*)vector_get_reference(&ast_nodes, id);
}
int ast_get_child_id(struct ast_node *N, int num){
	return *(int*)vector_get_reference(&N->children, num);
}
struct ast_node *ast_get_child(struct ast_node *N, int num){
	return ast_get_node(ast_get_child_id(N,num));
}
//+--+ fancy!
//++-¦ nuuu notepad broke my fancy
//¦¦ ¦
//+--+
char pad[80];
int padI;
void ast_print_helper(struct ast_node *N, char *value, FILE *fp, int num, int hasnext){
	printf("printing ast %p, %p, %d",N,fp,num);
	char oldP = pad[padI-1];
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
	padI--;
	pad[padI] = 0;
}
void ast_print(struct ast_node *N, FILE *fp){
	pad[0] = 0;
	padI = 0;
	ast_print_helper(N, 0, fp, 0,0);
}

void ast_print_graph_helper(struct ast_node *N, FILE *fp, const char *str){
	//vector Names;
	int len = strlen(str);
	int namelen = len+5;
	char *namebuff = (char *)malloc(namelen);
	strcpy(namebuff, str);
	char *suffix = namebuff+len;
	//vector_constructor(&Names, namelen);
	if(N->token.value){
		fprintf(fp, "%s [label = \"%s\n\\\"%s\\\"\"]\n", namebuff, N->token.type, N->token.value);
	}else{
		fprintf(fp, "%s [label = \"%s\"]\n", namebuff, N->token.type);
	}
	int i;
	for(i = 0; i < N->children.size; i++){
		snprintf(suffix,4,"_%d",i);
		ast_print_graph_helper(ast_get_child(N,i),fp,namebuff);
		if(len){
			fprintf(fp, "%s -- ", str);
			fprintf(fp, "%s\n", namebuff);
		}
	}
}
void ast_print_graph(struct ast_node *N, FILE *fp, const char *str){
	fprintf(fp, "graph program{\n");
	ast_print_graph_helper(N,fp,str);
	fprintf(fp, "}");
}
