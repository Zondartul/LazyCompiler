#ifndef PREPROCESS_GUARD
#define PREPROCESS_GUARD
//void preprocess(const char *filename);

#include "globals.h"
#include "semantic.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "AST.h"
#include "stdarg.h"
#include "codegen.h"
#include "yaccin.tab.h"
#include "types/file.h"

//defines

//generated
//implementation_vector_of(ptr_file);

//vars
extern const char *path_out_preproc;
extern const char *path_in_preproc;
extern const char *path_out_parse;
extern const char *path_out_semantic;
extern const char *path_out_IR;
extern const char *path_out_codegen;
extern const char *path_out_assembly;
extern const char *path_out_2;
extern const char *path_out_graphviz;
extern const char *file;
extern const char *linestr;
extern const char *parsePtr;
extern int line;
extern FILE *yyin;
extern FILE *fp;
extern vector2_ptr_char filestack;
//vector2_int linestack;
extern vector3_int linestack;
//typedef FILE* ptr_file;
extern vector2_ptr_file ppfiles;
extern vector2_ptr_char defines;
extern vector2_ptr_char replacements;

//functions
void pushFile();
void popFile();
void pushLine();
void popLine();
void reportline();
void parseInit(const char *str);
int parseAccept(const char *str);
int parseAcceptSpace();
void parseExpect(const char *str);
char *parseWord();
void pushYyin();
void popYyin();
extern vector2_ptr_char includedFiles;
int contains_string(vector2_ptr_char *v, const char *str);
void concat(char **buff,const char *str, int len);
void string_replace(char *str, const char *def, const char *rep);
void doReplacements(char *str);
void preprocess(const char *filename);

#endif