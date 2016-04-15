%{
#include "yaccin.tab.h"
extern YYLTYPE yylloc;
//#define YY_USER_ACTION yylloc.last_column += yyleng; if((*yytext != ' ')&&(*yytext != '\n')&&(*yytext != '\t')){printf("Lexxed token [%s]\n", yytext);} 
#define YY_USER_ACTION updatelloc(); if((*yytext != ' ')&&(*yytext != '\n')&&(*yytext != '\t')){printf("Lexxed token [%s]\n", yytext);printmemory(stdout);} 
 
#include "globals.h"
int lextokenstart;
int lextokenend; 
int lexlinestart;
int lexnumtabs;
void updatelloc(){
	yylloc.first_line = yylloc.last_line;
    yylloc.first_column = yylloc.last_column;
	lextokenstart = lextokenend;
    int i = 0;
	for(i = 0; yytext[i] != '\0'; i++) {
        if(yytext[i] == '\n') {
            yylloc.last_line++;
            yylloc.last_column = 0;
			lexnumtabs = 0;
			lextokenend+=2;
			lexlinestart = lextokenend;
        }
        else {
			if(yytext[i] == '\t')
			lexnumtabs++;
            yylloc.last_column++;
			lextokenend++;
		}
    }
}
//#define if((*yytext != ' ')&&(*yytext != '\n')&&(*yytext != '\t')){printf("Lexxed token [%s]\n", yytext);} 

#include "string.h"
#include "stdlib.h"
char *stralloc(const char *str){
	int len = strlen(str);
	char *str2 = (char *)malloc(sizeof(char)*(len+1));
	strcpy(str2,str);
	return str2;
}
%}
%%
%{
//yylloc.first_column = yylloc.last_column;
%}

return	{return(RETURN);}
class	{return(CLASS);}
int		|
float	|
char	|
void	|
string	{yylval = (int)stralloc(yytext); return(TYPE);}
end		{return(END);}
if		{return(IF);}
then	{return(THEN);}
else	{return(ELSE);}
elseif	{return(ELSEIF);}
while	{return(WHILE);}
[a-zA-Z_][a-zA-Z_0-9]* {yylval = (int)stralloc(yytext); return(ID);}
(0x[0-9]+)						{yylval = (int)stralloc(yytext); return(INTEGERX);}//{sscanf(yytext,"%x",&yylval); return(INTEGER);}
(0b[0-1]+)						{yylval = (int)stralloc(yytext); return(INTEGERB);}//{sscanf(yytext,"%x",&yylval); return(INTEGER);}
(0|([1-9][0-9]*))\.(0-9)+		{yylval = (int)stralloc(yytext); return(FLOATING);}//{float *F = malloc(sizeof(float)); sscanf(yytext,"%f",&F); yylval = (int)F; return(FLOATING);}
(0|([1-9][0-9]*))				{yylval = (int)stralloc(yytext); return(INTEGER);}//{sscanf(yytext,"%d",&yylval); return(INTEGER);}
\'.\' 							{yylval = (int)stralloc(yytext); return(CHARACTER);}//{sscanf(yytext,"%c",&yylval); return(CHARACTER);}
\".*\"							{yylval = (int)stralloc(yytext); return(STRING);}
[ \t\n]	;
.	{return *yytext;}
