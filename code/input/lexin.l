%option never-interactive 
%option noyywrap
%{
#include "yaccin.tab.h"
extern YYLTYPE yylloc;
//#define YY_USER_ACTION yylloc.null = 0; yylloc.last_column += yyleng; if((*yytext != ' ')&&(*yytext != '\n')&&(*yytext != '\t')){printf("Lexxed token [%s]\n", yytext);} 
#define YY_USER_ACTION \
	updatelloc();\
	printf("Lexxed token [%s]\n",yytext);
	
	//if(yylloc.filename){printf("Lexxed token [%s]=[%s]\n", yytext,get_source_text(yylloc.start,yylloc.end,yylloc.filename));}printmemory(stdout);
 
#include "globals.h"
int lextokenstart;
int lextokenend; 
int lexlinestart;
int lexnumtabs;
int lexnewline = 0;

YYLTYPE prev_yylloc[3];

void updatelloc(){
	prev_yylloc[2] = prev_yylloc[1];
	prev_yylloc[1] = prev_yylloc[0];
	prev_yylloc[0] = yylloc;

	yylloc.null = 0;
	yylloc.first_line = yylloc.last_line;
    yylloc.first_column = yylloc.last_column;	
	yylloc.start = yylloc.end;
	
	//?????
	//lextokenstart = lextokenend;
    int i = 0;
	for(i = 0; yytext[i] != '\0'; i++) {
		yylloc.end++; 
        if(yytext[i] == '\n') {
			//yylloc.end++;
            yylloc.last_line++;
            yylloc.last_column = 0;
			//lexnumtabs = 0;
			//lextokenend+=2;
			//string ending-specific
			//lexlinestart = lextokenend;
			printf("lex: newline i=%d\n",i);
			//lexnewline = 1;
		}
        else{
			//if(yytext[i] == '\t')
			//lexnumtabs++;
            yylloc.last_column++;
			//lextokenend++;
			//lexnewline = 0;
		}
    }
}
void setfile(char *str){
	yylloc.filename = str;
	yylloc.first_line = 0;
	yylloc.first_column = 0;
	yylloc.last_line = 0;
	yylloc.last_column = 0;
}
void setline(int L){
	yylloc.first_line = L;
	yylloc.first_column = 0;
	yylloc.last_line = L;
	yylloc.last_column = 0;
	yylloc.start = line_to_pos(L,yylloc.filename);
	yylloc.end = yylloc.start;
}
//#define if((*yytext != ' ')&&(*yytext != '\n')&&(*yytext != '\t')){printf("Lexxed token [%s]\n", yytext);} 

#include "string.h"
#include "stdlib.h"
#include "types/ast_node.h"


YYLTYPE tokPos(){return yylloc;}
//#define v2pan_here vector2_ptr_ast_node_here

 /*char *str = stralloc(yytext); strncpy(str,&yytext[1],strlen(yytext)-2); str[strlen(yytext)-2]=0;*/ 
//regular expressions cannot contain unquoted space
/*
{fprintf(stderr,"#file lexxed\n");}
{char *str = substring(yytext,7,strlen(yytext)-2); yylloc.filename = str;}
{fprintf(stderr,"#line lexxed\n");}
{char *str = substring(yytext,6,strlen(yytext)-1); sscanf(str,"%d",&yylloc.last_line);}
"#line "(0|([1-9][0-9]*))[^\n]*\n			{char *str = substring(yytext,6,strlen(yytext)-1); int L; sscanf(str,"%d",&L); setline(L);}

*/
%}
%%
%{
//yylloc.first_column = yylloc.last_column;
%}
"#file "\"([^\\"]|\\.)*\"[^\n]*\n			{char *str = substring(yytext,7,strlen(yytext)-2); setfile(str);}
"#line "(0|([1-9][0-9]*))[^\n]*\n			{char *str = substring(yytext,6,strlen(yytext)-1); int L; sscanf(str,"%d",&L); setline(L);}

return										{yylval = ast_node_new(ast_token_here("RETURN",	0,NULL,				tokPos()), v2pan_here()); return(RETURN);}
class										{yylval = ast_node_new(ast_token_here("CLASS",	0,NULL,				tokPos()), v2pan_here()); return(CLASS);}
int		|
float	|
char	|
void	|
string										{yylval = ast_node_new(ast_token_here("TYPE",	0,stralloc(yytext),	tokPos()),	v2pan_here());return(TYPE);}
varargs										{yylval = ast_node_new(ast_token_here("VARARGS",0,NULL,				tokPos()), v2pan_here()); return(VARARGS);}
end											{yylval = ast_node_new(ast_token_here("END",	0,NULL,				tokPos()), v2pan_here()); return(END);}
if											{yylval = ast_node_new(ast_token_here("IF",		0,NULL,				tokPos()), v2pan_here()); return(IF);}
else										{yylval = ast_node_new(ast_token_here("ELSE",	0,NULL,				tokPos()), v2pan_here()); return(ELSE);}
elseif										{yylval = ast_node_new(ast_token_here("ELSEIF",	0,NULL,				tokPos()),	v2pan_here()); return(ELSEIF);}
while										{yylval = ast_node_new(ast_token_here("WHILE",	0,NULL,				tokPos()),	v2pan_here()); return(WHILE);}
for											{yylval = ast_node_new(ast_token_here("FOR",	0,NULL,				tokPos()), v2pan_here()); return(FOR);}
\=\=										{yylval = ast_node_new(ast_token_here("EQUAL",	0,NULL,				tokPos()), v2pan_here()); return(EQUAL);}
\!\=										{yylval = ast_node_new(ast_token_here("NOTEQUAL",0,NULL,			tokPos()), v2pan_here()); return(NOTEQUAL);}
\-\-										{yylval = ast_node_new(ast_token_here("DEC",	0,NULL,				tokPos()), v2pan_here()); return(DEC);}
\+\+										{yylval = ast_node_new(ast_token_here("INC",	0,NULL,				tokPos()), v2pan_here()); return(INC);}
[a-zA-Z_][a-zA-Z_0-9]* 						{yylval = ast_node_new(ast_token_here("ID",		0,stralloc(yytext),	tokPos()), v2pan_here()); return(ID);}
([0-9]*\.[0-9]+)							{yylval = ast_node_new(ast_token_here("FLOATING",0,stralloc(yytext),tokPos()), v2pan_here()); return(FLOATING);}
(0x[0-9]+)									{yylval = ast_node_new(ast_token_here("INTEGERX",0,stralloc(yytext),tokPos()), v2pan_here()); return(INTEGERX);}
(0b[0-1]+)									{yylval = ast_node_new(ast_token_here("INTEGERB",0,stralloc(yytext),tokPos()), v2pan_here()); return(INTEGERB);}
(0|([1-9][0-9]*))\.(0-9)+					{yylval = ast_node_new(ast_token_here("FLOATING",0,stralloc(yytext),tokPos()), v2pan_here()); return(FLOATING);}
(0|([1-9][0-9]*))							{yylval = ast_node_new(ast_token_here("INTEGER",0,stralloc(yytext),	tokPos()), v2pan_here()); return(INTEGER);}
\'.\' 										{yylval = ast_node_new(ast_token_here("CHARACTER",0,stralloc(yytext),tokPos()), v2pan_here()); return(CHARACTER);}
\'\\.\' 									{yylval = ast_node_new(ast_token_here("CHARACTER",0,stralloc(yytext),tokPos()), v2pan_here()); return(CHARACTER);}
\"([^\\"]|\\.)*\"							{char *str =  substring(yytext,1,strlen(yytext)-1);
											 yylval = ast_node_new(ast_token_here("RETURN",	0,str,				tokPos()), v2pan_here()); return(STRING);}
[ \t\n\r]									;
\/\/[^\n]*\n								;
\/\*(([^*])|(\*[^/]))*\*\/					;
.											{yylval = ast_node_new(ast_token_here("rawchar", 0,stralloc(yytext), tokPos()), v2pan_here()); return *yytext;}
