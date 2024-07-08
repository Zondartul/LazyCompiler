#include "globals.h"
#include "semantic.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "AST.h"
#include "stdarg.h"
#include "preprocess.h"
#include "codegen.h"
#include "yaccin.tab.h"

// read file line-by-line. If the line contains a directive, the line is removed and the following actions are done:
// #define WORD|"escaped-string" [REPLACEMENT|"escaped-replacement"]
//		add WORD to the list of defines (vector<string,string>)
//		if there is a replacement:
//			every time WORD is encountered in a line, it is replaced by replacement
//			and line is re-parsed.
//		if there is no replacement, no replacement is done.
//
// #undefine WORD|"escaped-string"
//		remove WORD from the list of defines
//
// #include "filename"
//		the string is replaced by every string in the included file.
//
// #ifdef WORD|"escaped-string"
// #ifndef WORD|"escaped-string"
// #endif

//defines


//generated
//definition_vector_of(ptr_file);
//implementation_vector_of(ptr_file);

//vars
const char *file = 0;
int line = 1;
vector2_ptr_char filestack;
vector3_int linestack;
//vector2_int linestack;
const char *linestr;
const char *parsePtr;
extern FILE *yyin;
FILE *fp;
typedef FILE* ptr_file;
vector2_ptr_file ppfiles;
vector2_ptr_char includedFiles;
vector2_ptr_char defines;
vector2_ptr_char replacements;


//functions
void pushFile(){
	m(filestack,push_back,file);
}
void popFile(){
	if(filestack.size){
		file = m(filestack,pop_back);
	}else{
		file = 0;
	}
}
void pushLine(){
	m(linestack,push_back,line);
}
void popLine(){
	if(linestack.size){
		line = m(linestack,pop_back);
	}else{
		line = 1;
	}
}
void reportline(){
	//fprintf(stderr,"\n%s, line %d:\n%s",file,line,linestr);
}

void parseInit(const char *str){parsePtr = str;}
int parseAccept(const char *str){
	if(!strncmp(parsePtr,str,strlen(str))){
		parsePtr = parsePtr + strlen(str);
		////fprintf(stderr,"accepted [%s]\n",escape_string(str));
		return 1;
	}
	////fprintf(stderr,"no [%s]\n",escape_string(str));
	return 0;
}
int parseAcceptSpace(){
	return parseAccept(" ") || parseAccept("\t");
}
void parseExpect(const char *str){
	if(!parseAccept(str)){reportline();error("[PREPROC] %s expected ",escape_string(str));}
}

char *parseWord(){
	////fprintf(stderr,"parsePtr[0] = [%c]\n",parsePtr[0]);
	char buff[2000];
	int buffI = 0;
	buff[0] = 0;
	int I = 0;
	if(parsePtr[0] == '\"'){
		////fprintf(stderr,"quote start\n");
		I++;
		while(1){
			if((parsePtr[I] == '\\') && (parsePtr[I+1]=='\"')){
				buff[buffI++] = parsePtr[I++];
				buff[buffI++] = parsePtr[I++];
			}else if(parsePtr[I] == '\"'){/*//fprintf(stderr,"quote end\n"); */break;}
			else if(!parsePtr[I]){reportline();error("[PREPROC] broken string literal ");}
			else{
				buff[buffI++] = parsePtr[I++];
			}
		}
		buff[buffI] = 0;
		if(!strlen(buff)){/* //fprintf(stderr,"nothing parsed\n"); */ return 0;}
		parsePtr = parsePtr + strlen(buff)+2; //""
		/* //fprintf(stderr,"parsed [%s]\n",escape_string(buff)); */
		return stralloc(buff);
	}else{
		////fprintf(stderr,"unquoted\n");
		while((parsePtr[I] != ' ')&&(parsePtr[I] != '\n')&&(parsePtr[I] != '\t')){
			buff[buffI++] = parsePtr[I++];
		}
		buff[buffI] = 0;
		if(!strlen(buff)){/* //fprintf(stderr,"nothing parsed\n"); */ return 0;}
		parsePtr = parsePtr + strlen(buff);
		/* //fprintf(stderr,"parsed [%s]\n",escape_string(buff)); */
		return stralloc(buff);
	}
}
void pushYyin(){
	m(ppfiles,push_back,yyin);
}

void popYyin(){
	////fprintf(stderr,"ppfiles.size = %d\n",ppfiles.size);
	fclose(yyin);
	if(ppfiles.size){
		yyin = m(ppfiles,pop_back);
	}else{
		yyin = 0;
	}
}

int contains_string(vector2_ptr_char *v, const char *str){
	int I;
	for(I = 0; I < v->size; I++){
		const char *str2 = m((*v),get,I);
		if(!str2){if(!str){return 1;}else{continue;}}
		if(!strcmp(str,str2)){
			return 1;
		}
	}
	return 0;
}

void concat(char **buff,const char *str, int len){
	memcpy(*buff,str,len);
	*buff += len;
}

void string_replace(char *str, const char *def, const char *rep){
	////fprintf(stderr,"replace string: [%s]\n",str);
	char buff[2000];
	char *buffptr = buff;
	char *strptr = str;
	char *s = strstr(strptr,def);
	while(s){
		//concat(&buffptr,".1.",strlen(".1."));
		concat(&buffptr,strptr,s-strptr);
		//concat(&buffptr,".2.",strlen(".2."));
		//memcpy(buffptr,str,s-str);
		//buffptr += s-str;
		concat(&buffptr,rep,strlen(rep));
		//concat(&buffptr,".3.",strlen(".3."));
		//memcpy(buffptr,rep,strlen(rep));
		//buffptr += strlen(rep);
		strptr = s+strlen(def);
		s = strstr(strptr,def);
	}
	
	concat(&buffptr,strptr,strlen(strptr));
	
	//memcpy(buffptr,str,strlen(str));
	//buffptr += strlen(str);
	//concat(&buffptr,".4.",strlen(".4."));
	
	buffptr[0] = 0;
	strcpy(str,buff);
	////fprintf(stderr,"final string: [%s]\n",str);
	//paste the rest of the string
}


void doReplacements(char *str){
	if(!str){return;}
	int I;
	for(I = 0; I < defines.size; I++){
		const char *def = m(defines,get,I);
		const char *rep = m(replacements,get,I);
		if(def && rep){
			const char *s = strstr(str,def);
			if(s){
				string_replace(str,def,rep);
				I = 0;
				////fprintf(stderr,"[%s]\n",str);
			}
		}
	}
}

void preprocess(const char *filename){
	yyin = fopen(filename, "r");
	if(!yyin){error("can't open \"%s\"\n",filename);}
	const char* filename2 = /*"ain_preproc.txt"*/ path_in_preproc;
	FILE *fp = fopen(filename2, "w");
	if(!fp){error("can't open file: %s",filename2);} 
	//fprintf(stderr,"preprocessing file opened: %s\n",filename2);
	
	
	defines = vector2_ptr_char_here();
	replacements = vector2_ptr_char_here();
	ppfiles = vector2_ptr_file_here();
	includedFiles = vector2_ptr_char_here();	
	filestack = vector2_ptr_char_here();
	linestack = vector3_int_here();//vector2_int_here();

	file = stralloc(filename);
	line = 1;
	
	char buff[2000];
//	char buff1[1000]; //unused
//	char buff2[1000]; //unused
	char *str; 
	char *word1;
	char *word2;
//	int found = 0; //unused
	int ends_with_newline;
	int ifyes = 1;
	int iflevel = 0;
	
	while(yyin){
		
		fprintf(fp,"#file \"%s\"\n",file);		//////////////////////////
		fprintf(fp,"#line %d\n",line/*+1*/);	///	LINES START FROM 1 ///
		str = fgets(buff,2000,yyin);			//////////////////////////
		ends_with_newline = 0;
		while(str){
			linestr = str;
			parseInit(str);
			if(ifyes && parseAccept("#define ")){
				////fprintf(stderr,"\nstr: [%s]\n",str);
				word1 = parseWord();
				if(!word1){reportline();error("[PREPROC] bad define: [%s] ",str);}
				parseAcceptSpace();
				word2 = parseWord();
				while(parseAcceptSpace()){}
				if(!parseAccept("\0")){parseExpect("\n");}
				//define parsed
				if(contains_string(&defines,word1)){error("[PREPROC] %s already defined\n",word1);}
				m(defines,push_back,word1);
				m(replacements,push_back,word2);
				if(word2){//fprintf(stderr,"defined [%s] as [%s]\n",word1,word2);
				}else{//fprintf(stderr,"defiend [%s]\n",word1);
				}
				line++;
				goto nextline;
			}
			if(ifyes && parseAccept("#undefine ")){
				word1 = parseWord();
				if(!word1){reportline();error("[PREPROC] bad undefine: [%s] ",str);}
				while(parseAcceptSpace()){}
				if(!parseAccept("\0")){parseExpect("\n");}
				//undefine parsed
				if(!contains_string(&defines,word1)){reportline();error("[PREPROC] %s is not defined\n",word1);}
				int I;
				for(I = 0; I < defines.size; I++){
					const char *def = m(defines,get,I);
					if(def && !strcmp(def,word1)){
						m(defines,set,(char*)0,I);
						m(replacements,set,(char*)0,I);
					}
				}
				//fprintf(stderr,"undefined [%s]\n",word1);
				line++;
				goto nextline;
			}
			if(ifyes && parseAccept("#include ")){
				word1 = parseWord();
				while(parseAcceptSpace()){}
				if(!parseAccept("\0")){parseExpect("\n");}
				// include parsed
				if(contains_string(&includedFiles,word1)){
					//fprintf(stderr,"file [%s] is already included\n",word1);
					fprintf(fp,"\n");
					goto nextline;
				}else{
					//fprintf(stderr,"included file [%s]\n",word1);
					pushYyin();
					yyin = fopen(word1,"r");
					if(!yyin){error("#include not found: \"%s\" ",word1);}
					pushFile();
					file = stralloc(word1);
					line++;
					pushLine();
					line = 1;
					m(includedFiles,push_back,stralloc(word1));
					fprintf(fp,"#file \"%s\"\n",word1);
					fprintf(fp,"#line 1\n");
					goto nextline;
				}
			}
			if(parseAccept("#ifdef ")){
				word1 = parseWord();
				while(parseAcceptSpace()){}
				if(!parseAccept("\0")){parseExpect("\n");}
				//ifdef parsed
				if(ifyes || (iflevel == 0)){
					ifyes = contains_string(&defines,word1);
					//fprintf(stderr,"if defined [%s] (%d)\n",word1,ifyes);				
				}
				iflevel++;
				line++;
				goto nextline;
			}
			if(parseAccept("#ifndef ")){
				word1 = parseWord();
				while(parseAcceptSpace()){}
				if(!parseAccept("\0")){parseExpect("\n");}
				//ifndef parsed
				if(ifyes || (iflevel == 0)){
					ifyes = !contains_string(&defines,word1);
					//fprintf(stderr,"if not defined [%s] (%d)\n",word1, ifyes);
				}
				iflevel++;
				line++;
				goto nextline;
			}
			if(parseAccept("#endif")){
				while(parseAcceptSpace()){}
				if(!parseAccept("\0")){parseExpect("\n");}
				//endif parsed
				iflevel--;
				if(iflevel == 0){
					ifyes = 1;
					//fprintf(stderr,"end if\n");
				}
				if(iflevel < 0){error("[PREPROC] unmatched #endif ");}
				line++;
				goto nextline;
			}
			if(ifyes){
				doReplacements(str);
				fprintf(fp,"%s",str);
				ends_with_newline = (str[strlen(str)-1] == '\n'); 
			}
			line++;
			nextline:
			str = fgets(buff,2000,yyin);
		}
		
		if(!ends_with_newline){fprintf(fp,"\n");}
		
		popYyin();
		popFile();
		popLine();
		
	}
	
	if(iflevel){error("[PREPROC] unfinished #if..#endif block ");}
	//error("BONK ");
	
	fclose(fp);
}