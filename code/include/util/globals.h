#ifndef GLOBALS_GUARD
#define GLOBALS_GUARD
#include "stdlib.h"
#include "stdarg.h"
#include "stdio.h"
//#define BUMP_ALLOCATOR
extern int lextokenstart;
extern int lextokenend;
extern int lexlinestart;
extern int lexnumtabs;
extern int memused;
extern int numallocs;
void initFiles();
#define fopen(x,y) newfopen(x,y)
#define fclose(x) newfclose(x)
FILE *newfopen(const char *filename, const char *mode);
void newfclose(FILE *fp);

#define error(...) newerror(__FILE__,__LINE__,__func__,__VA_ARGS__)
void newerror(const char* file, int line, const char* func, const char *fmt, ...);
#define err(...) fprintf(stderr, __VA_ARGS__)
//uncomment this line to use the built-in allocation instrumentation
//#define OVERRIDE_MALLOC
#include "allocator_override.h"
int yylex();
//char *get_source_text(int start, int end,const char *file);
//char *get_source_line(int start, const char *file);
//char *get_source_line2(int start, const char *file);
//int line_to_pos(int line, const char *file);
//void point_out_error(int line, int pos, const char *file, const char *s, int length);

#include "yylloc.h"

struct ast_node;
typedef struct ast_node* ptr_ast_node;
typedef ptr_ast_node YYSTYPE;
# define YYSTYPE_IS_DECLARED 1

#define YY_NO_INPUT //removes the unused "int input()" function
#define YY_NO_UNPUT


#include "stringUtils.h"

#endif