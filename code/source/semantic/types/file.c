#include "types/file.h"

implementation_vector_of(ptr_file);

vector2_ptr_file files;

void initFiles(){
	files = vector2_ptr_file_here();
}

FILE* newfopen(const char *filename, const char *mode){
	FILE *fp = fopen(filename, mode);
	if(!fp){return 0;}
	m(files,push_back,fp);
	return fp;
}

void newfclose(FILE* fp){
	//1. check if file is still in the list of open files
	//(to prevent double-close)
	int I;
	for(I = 0; I < files.size; I++){
		FILE *fp2 = m(files,get,I);
		if(fp2 == fp){
			fflush(fp);
			fclose(fp);
			m(files,erase,I);
			break;
		}
	}
}

void flushAllFiles(){
	int I;
	for(I = 0; I < files.size; I++){
		FILE *fp = m(files,get,I);
		fflush(fp);
		fclose(fp);
	}
	m(files,resize,0);
}