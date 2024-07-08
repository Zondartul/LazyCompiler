#include "globals.h"

/*

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

*/

YYLTYPE nullPos(){
	YYLTYPE pos;
	pos.null = 1;
	pos.first_line = 0;
	pos.first_column = 0;
	pos.last_line = 0;
	pos.last_column = 0;
	pos.start = 0;
	pos.end = 0;
	pos.filename = 0;
	return pos;
}
