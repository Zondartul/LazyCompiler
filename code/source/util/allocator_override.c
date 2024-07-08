#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#define err(...) fprintf(stderr, __VA_ARGS__)
#define error(...) newerror(__FILE__,__LINE__,__func__,__VA_ARGS__)
void newerror(const char* file, int line, const char* func, const char *fmt, ...);


#define MAX_ALLOCS 20000
void *allocs[MAX_ALLOCS];
int allocsizes[MAX_ALLOCS];
const char *allocfiles[MAX_ALLOCS];
const char *allocfuncs[MAX_ALLOCS];
int alloclines[MAX_ALLOCS];
int memused;
int numallocs;
#define BUMP_MAX 1000000
char bump_buff[BUMP_MAX];
int bump_free;

void initallocs(){
	memused = 0;
	numallocs = 0;
	int i;
	for(i = 0; i < MAX_ALLOCS; i++){
		allocs[i] = 0;
	}
	for(i = 0; i < MAX_ALLOCS; i++){
		allocsizes[i] = 0;
	}
	for(i = 0; i < MAX_ALLOCS; i++){
		allocfiles[i] = 0;
	}
	for(i = 0; i < MAX_ALLOCS; i++){
		allocfuncs[i] = 0;
	}
	for(i = 0; i < MAX_ALLOCS; i++){
		alloclines[i] = 0;
	}
	bump_free = 0;
}
void *bumpalloc(size_t size){
	if((bump_free+size+2*sizeof(int))<BUMP_MAX){		
		void *result = (void*)(bump_buff+bump_free+2*sizeof(int));
		bump_free+=size+2*sizeof(int);
		((int*)result)[-1]=size;
		printf("bump %d bytes: %d bytes left\n", (int)size, BUMP_MAX-bump_free);
		return result;
	}else{
		return 0;
	}
}
void *bumprlloc(void *ptr, size_t size){
	if(ptr){
		void *result = bumpalloc(size);
		memcpy(result,ptr,((int*)ptr)[-1]);
		return result;
	}else{return bumpalloc(size);}
}
void *newmalloc(size_t size, void *(oldmalloc)(size_t size), const char* file, const char* func, int line){
	//err("info: malloc(%d)\n",size);
	//err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	#ifdef BUMP_ALLOCATOR
	void *m = bumpalloc(size);
	#else
	void *m = oldmalloc(size);
	#endif
	if(!m){
		//err("newmalloc: malloc(%d) failed\n",size);
		//err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	}
	else{
		allocs[numallocs] = m;
		allocsizes[numallocs] = size;
		allocfiles[numallocs] = file;
		allocfuncs[numallocs] = func;
		alloclines[numallocs] = line;
		numallocs++;
		memused += size;
	}
	return m;
}
void *newrealloc(void *ptr, size_t size, void *(oldrealloc)(void *ptr, size_t size), const char* file, const char* func, int line){
	//err("info: realloc(%p, %d)\n",ptr, size);
	int i = 0;
	int found = 0;
	if(ptr){
		for(i = 0; i < MAX_ALLOCS; i++){
			if(allocs[i] == ptr){found = 1; break;}
		}
		if(!found){
			err("newrealloc: attempt to realloc(%p,%d): bad pointer\n",ptr,(int)size);
			error("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	
		}
	}
	#ifdef BUMP_ALLOCATOR
	void *m = bumprlloc(ptr, size);
	#else
	void *m = oldrealloc(ptr, size);
	#endif
	if(m){
		if(found){
			//err("info: old size = %d\n",allocsizes[i]);
			memused -= allocsizes[i];
			memused += size;
			allocsizes[i] = size;
			allocs[i] = m;
		}else{
			//err("info: new allocation\n");
			allocs[numallocs] = m;
			allocsizes[numallocs] = size;
			allocfiles[numallocs] = file;
			allocfuncs[numallocs] = func;
			alloclines[numallocs] = line;
			numallocs++;
			memused += size;
		}
	}else{
		//err("newrealloc: realloc(%p,%d) failed\n",ptr,size);
		//err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	}
	//err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	return m;
}
void newfree(void *ptr, const char *file, const char* func, int line/*,unused void (oldfree)(void *ptr)*/){
	//err("info: free(%p)\n",ptr);
	int i = 0;
	int found = 0;
	if(ptr){
		for(i = 0; i < MAX_ALLOCS; i++){
			if(allocs[i] == ptr){found = 1; break;}
		}
		if(!found){
			err("newfree: free(%p): bad pointer\n",ptr);
			error("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
		}
		//err("info: old size = %d\n",allocsizes[i]);
		allocs[i] = 0;
		allocsizes[i] = 0;
		allocfiles[i] = 0;
		alloclines[i] = 0;
		allocfuncs[i] = 0;
	}else{
		//err("note: free(0)\n");
		//err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
	}
	//oldfree(ptr);
	//err("@ file \"%s\", line %d, func \"%s\"\n",file,line,func);
}
void printmemory(FILE *fp){
	fprintf(fp, "num allocs: %d, mem used: %d\n", numallocs, memused);
}
void printallocs(FILE *fp){
	int i;
	for(i = 0; i < MAX_ALLOCS; i++){
		if(allocs[i]){
			fprintf(fp, "alloc(%d)\t=\t%p\tfrom file \"%s\"\tline %d,\tfunc \"%s\"\n", allocsizes[i],allocs[i],allocfiles[i],alloclines[i],allocfuncs[i]);
		}
	}
}