#ifndef FILE_GUARD
#define FILE_GUARD
#include "stdio.h"
#include "util/vector2.h"

typedef FILE* ptr_file;
definition_vector_of(ptr_file);

//tracking of opened files
extern vector2_ptr_file files;
//initializes file tracker
void initFiles();
//opens file (overrides the fopen command)
FILE* newfopen(const char *filename, const char *mode);
//closes file (overrides the fclose command)
void newfclose(FILE* fp);
//flushes and closes all opened files - possibly used in error()?
void flushAllFiles();

#endif