#include "yylloc.h"
#include "stdio.h"

void printPos(YYLTYPE pos){
	printf("null: %d\n",pos.null);
	printf("first_line: %d\n",pos.first_line);
	printf("first_column: %d\n",pos.first_column);
	printf("last_line: %d\n", pos.last_line);
	printf("last_column: %d\n", pos.last_column);
	printf("start: %d\n", pos.start);
	printf("end: %d\n", pos.end);
	printf("filename: [%s]\n",pos.filename);
}


void printPosErr(YYLTYPE pos){
	fprintf(stderr,"null: %d\n",pos.null);
	fprintf(stderr,"first_line: %d\n",pos.first_line);
	fprintf(stderr,"first_column: %d\n",pos.first_column);
	fprintf(stderr,"last_line: %d\n", pos.last_line);
	fprintf(stderr,"last_column: %d\n", pos.last_column);
	fprintf(stderr,"start: %d\n", pos.start);
	fprintf(stderr,"end: %d\n", pos.end);
	fprintf(stderr,"filename: [%s]\n",pos.filename);
}
