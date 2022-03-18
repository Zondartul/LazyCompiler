#ifndef STRING_UTILS_GUARD
#define STRING_UTILS_GUARD
#include "yylloc.h"
//gets a string of source-code corresnonding to the range "YYLTYPE pos".
//returns a new cstring.
char *get_source_text2(YYLTYPE pos);
char *get_source_text(int start, int end,const char *file);
char *get_source_line(int start, const char *file);
int line_to_pos(int line, const char *file);
char *get_source_line2(int line,const char *file);
int countTabs(const char *str);
void point_out_error(int line, int pos, const char *file, const char *s, int length);
char *stralloc(const char *str);
char *substring(const char *text, int from, int to);
typedef const char* cstr;
const char* concat2(int num, ...);
const char* my_itoa(int N);
void countindent(const char *str);
void printindent();
int isnumber(const char *str);

#endif