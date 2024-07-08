#ifndef YYLLOC_GUARD
#define YYLLOC_GUARD

typedef struct YYLTYPE
{
int null;
int first_line;
int first_column;
int last_line;
int last_column;
int start;
int end;
char *filename;
} YYLTYPE;

YYLTYPE nullPos();
void printPos(YYLTYPE pos);
void printPosErr(YYLTYPE pos);

# define YYLTYPE_IS_DECLARED 1

#endif