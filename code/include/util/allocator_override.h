
#ifdef OVERRIDE_MALLOC
#define free(x) newfree(x,__FILE__,__func__,__LINE__ /*, &free*/)
#define malloc(x) newmalloc(x, &malloc,__FILE__,__func__,__LINE__)
#define realloc(x,y) newrealloc(x,y,&realloc,__FILE__,__func__,__LINE__);
#endif

void initallocs();
char *stralloc(const char *str);
void *newmalloc(size_t size, void *(oldmalloc)(size_t size), const char* file, const char* func, int line);
void *newrealloc(void *ptr, size_t size, void *(oldrealloc)(void *ptr, size_t size), const char* file, const char* func, int line);
void newfree(void *ptr,const char* file, const char* func, int line/*, unused void (oldfree)(void *ptr)*/);
void printmemory(FILE *fp);
