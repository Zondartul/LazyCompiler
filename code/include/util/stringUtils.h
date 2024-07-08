#ifndef STRING_UTILS_GUARD
#define STRING_UTILS_GUARD
#include "yylloc.h"
#include "vector2.h"
#include "types.h"
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

//the vec_printf family of functions appends all writes to a provided vector2 using vector2.push,
//which automatically resizes the vector when needed.
//returns the number of bytes written.
//if size == 0, returns the number of bytes that WOULD be written.
//if size == -1, number of bytes is unlimited.
//else, writes only up to size bytes.
int vec_vnprintf(vector2_char* vstr, int size, const char* format, va_list vlist);
int vec_nprintf(vector2_char* vstr, int size, const char* format, ...);
int vec_printf(vector2_char* vstr, const char* format, ...);

#endif





