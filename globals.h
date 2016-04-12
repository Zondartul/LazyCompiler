#ifndef GLOBALS_GUARD
#define GLOBALS_GUARD
#include "stdlib.h"
#include "stdarg.h"
#include "stdio.h"
#define BUMP_ALLOCATOR
extern int lextokenstart;
extern int lextokenend;
extern int lexlinestart;
extern int lexnumtabs;
extern int memused;
extern int numallocs;
void initallocs();
char *stralloc(const char *str);
#define malloc(x) newmalloc(x, &malloc,__FILE__,__func__,__LINE__)
void *newmalloc(size_t size, void *(oldmalloc)(size_t size), const char* file, const char* func, int line);
#define realloc(x,y) newrealloc(x,y,&realloc,__FILE__,__func__,__LINE__);
void *newrealloc(void *ptr, size_t size, void *(oldrealloc)(void *ptr, size_t size), const char* file, const char* func, int line);
#define free(x) newfree(x,__FILE__,__func__,__LINE__, &free)
void newfree(void *ptr,const char* file, const char* func, int line, void (oldfree)(void *ptr));
void printmemory(FILE *fp);
#define error(...) newerror(__FILE__,__LINE__,__func__,__VA_ARGS__)
void newerror(const char* file, int line, const char* func, const char *fmt, ...);
#define err(...) fprintf(stderr, __VA_ARGS__)
#endif