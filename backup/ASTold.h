#ifndef AST_GUARD
#define AST_GUARD
#define AST_MAX_CHILDREN 5
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
struct ast_token{
	const char *type;
	int production;
	char *value;
	// int start_line;
	// int start_char;
	// int end_line;
	// int end_char;
};

struct ast_node{
	struct ast_token token;
	int numChildren;
	struct ast_node* children[AST_MAX_CHILDREN];
};

//create a new node, given a token, and 'n' child nodes.
struct ast_node *node(struct ast_token token, int n, ...){
	va_list ap;
	printf("\nnode \"%s\": [%s] (%d/%d)\n",token.type,token.value,token.production,n);
	struct ast_node *N = malloc(sizeof(struct ast_node));
	N->token = token;
	N->numChildren = n;
	va_start(ap, n);
	int i;
	for(i = 0; i < n; i++){
		N->children[i] = va_arg(ap, struct ast_node*);
	}
	va_end(ap);
	return N;
}
//┌┬─┐ fancy!
//├┼─┤
//││ │
//└┴─┘
char pad[80];
int padI;
void ast_print_helper(struct ast_node *N, char *value, FILE *fp, int num, int hasnext){
	printf("printing ast %p, %p, %d\n",N,fp,num);
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
		fprintf(fp, "%s", N->token.type);
		if(N->token.value){fprintf(fp, " \"%s\"", N->token.value);}
		fprintf(fp, "\n");
		//if(hasnext){pad[padI++] = '|'; pad[padI] = 0;}
		//else{pad[padI++] = ' '; pad[padI] = 0;}
		if(N->numChildren){
			// if(N->token.value){
				// ast_print_helper(0,N->token.value,fp,0,1);
			// }
			for(i = 0; i < N->numChildren; i++){
				ast_print_helper(N->children[i],N->token.value,fp,i,(i+1) < N->numChildren);
			}
		}
	}else{
		if(value){
			fprintf(fp, "\"%s\"\n",value);
		}else{
			fprintf(fp, "<no data>\n");
		}
	}
	padI--;
	pad[padI] = 0;
}
void ast_print(struct ast_node N, FILE *fp){
	pad[0] = 0;
	padI = 0;
	ast_print_helper(&N, 0, fp, 0,0);
}


#endif