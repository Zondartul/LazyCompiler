#include "globals.h"
#include "semantic.h" //for curNode
#include "codegen.h" //for fasm
#include "ctype.h"
//vars
int indent = 0;

//gets a string of source-code corresnonding to the range "YYLTYPE pos".
//returns a new cstring.
char *get_source_text2(YYLTYPE pos){ //this is still stupid-broken
	if(pos.null){return "(nullPos:text)";}
	if(!pos.filename){error("get_source_text2 called with no filename\n");}
	FILE *fp = fopen(pos.filename,"r");
	if(!fp){error("can't open %s ",pos.filename);}
	//fprintf(stderr,"gst2 %s, line %d\n",pos.filename,pos.first_column);
	int C;
	int line = 1;
	int col = 0;
	//int first_line = pos.first_line;
	//int last_line = pos.last_line;
	//int first_column = pos.first_column;
	//int last_column = pos.last_column;
	//const char *filename = pos.filename;
	//1) seek to start
	C = fgetc(fp);
	while(line/*+1*/ < pos.first_line){while((C != '\n')&&(C != EOF)){C = fgetc(fp);} line++; C = fgetc(fp);}
	while(col != pos.first_column){
		if(C == '\n'){
				fprintf(stderr,"\nWARNING: gst2: overshoot1: report\n");
				fprintf(stderr,"pos:\n");
				fprintf(stderr,"=====================\n");
				printPosErr(pos);
				break;
			//error("gst2: overshot1 ");
		} 
		C = fgetc(fp);
		col++;
	}
	//int fpos = ftell(fp);
	
	//2) count chars
	//int len = 0;
	//C = fgetc(fp);
	//while((line != pos.last_line)&&(C!=EOF)){while((C != '\n')&&(C != EOF)){C = fgetc(fp); len++;} line++; len++;}
	//len+= pos.last_column;
	
	
	//2) write the string down
	//char *str = malloc(sizeof(char)*(len+1));
	char str[2000];
	char *strP = str;
	//int err = fseek(fp,fpos,SEEK_SET);
	//if(err){perror("fseek: "); error("gst2: fseek: err = %d",err);}
	//line = pos.first_line;
	line = pos.first_line;
	int report = 1; 
	//C = fgetc(fp);
	while((line < pos.last_line)&&(C!=EOF)){
		//we should be at the start-line,
		//write down every line until the last
		while((C != '\n')&&(C != EOF)){
			//write down every character until a newline
			*strP++ = C; C = fgetc(fp);
		}
		col = 0;
		//write down the newline char
		*strP++ = C; line++; C = fgetc(fp);
	}
	while((col < pos.last_column)&&(C != EOF)){
		//write down characters until we reach the column
		if(C == '\n'){
			//*strP++ = 0;
			if(report){
				fprintf(stderr,"\nWARNING: gst2: overshoot2: report\n");
				fprintf(stderr,"node ==================\n");
				const char *snode = 0;
				if(curNode){snode = get_source_text2(curNode->token.pos);}
				fprintf(stderr,"%s\n",snode);
				fprintf(stderr,"semantic ============\n");
				fprintf(stderr,"decl = %d, nodesDecl = %d, nodesImp = %d\n",
						semantic_decl, nodeCountDecl, nodeCountImp);
				fprintf(stderr,"str =================\n"
							   "%s\n"
							   "=====================\n",str);
				fprintf(stderr,"pos:\n");
				fprintf(stderr,"=====================\n");
				printPosErr(pos);
			}
			//the requested column further than the length of the line
			//error("gst2: overshot2 ");
			break;
		}else{ 
			*strP++ = C; C = fgetc(fp); col++;
		}
	}
	
	//3) finalize string and return
	fclose(fp);
	*strP = 0;
	return stralloc(str);
}


char *get_source_text(int start, int end,const char *file){
	if(!file){error("get_source_text: file is null ");}
	if((start < 0)||(end < 0)||(end < start)){error("get_source_text: bad args (%d,%d) ",start,end);}
	//fprintf(stderr,"get_source_text(%d,%d,%s)\n",start,end,file);
	//fseek(yyin2, start, SEEK_SET);
	FILE *fp = fopen(file,"r");
	if(!fp){error("can't open %s ",file);}
	int i;
	//fseek(fp, 0, SEEK_SET);
	for(i = 0; i < start; i++){fgetc(fp);}
	//fseek(fp,start,SEEK_SET);
	char *buff = malloc(sizeof(char)*(end-start+2));
	// int i;
	// for(i = 0; i < end-start; i++){
		// buff[i] = fgetc(yyin2);
	// }
	fread(buff,1,end-start,fp);
	buff[end-start] = 0;
	//fprintf(stderr,"got [%s]\n",buff);
	//fread(buff,end-start,1,yyin2);
	fclose(fp);
	return buff;
}

char *get_source_line(int start, const char *file){
	FILE *fp = fopen(file,"r");
	if(!fp){error("can't open %s ",file);}
	fprintf(stderr,"get_source_line(%d,%s)\n",start,file);
	fseek(fp, start, SEEK_SET);
	int C = fgetc(fp);
	int len = 0;
	while((C != '\n') && (C != EOF)){C = fgetc(fp); len++;}
	fclose(fp);
	return get_source_text(start,start+len,file);
}

int line_to_pos(int line, const char *file){
	FILE *fp = fopen(file,"r");
	if(!fp){error("can't open %s ",file);}
	//fseek(yyin2, 0, SEEK_SET);
	int C = fgetc(fp);
	int start = 0;
	line--;
	while(line--){
		//printf("skipping line %d:%d:[%s]\n",14-line,start,get_source_line(start));
		//printf("C = '%c'\n",C);
		while((C != '\n') && (C != EOF)){
			C = fgetc(fp);
			start++;
		}
		C = fgetc(fp); 
		start++;
	}
	//int pos = ftell(fp);
	fclose(fp);
	return start;
}

char *get_source_line2(int line,const char *file){
	FILE *fp = fopen(file,"r");
	if(!fp){error("can't open %s ",file);}
	//fseek(yyin2, 0, SEEK_SET);
	int C = fgetc(fp);
	int start = 0;
	line--;
	while(line--){
		//printf("skipping line %d:%d:[%s]\n",14-line,start,get_source_line(start));
		//printf("C = '%c'\n",C);
		while((C != '\n') && (C != EOF)){
			C = fgetc(fp);
			start++;
		}
		C = fgetc(fp); 
		start++;
	}
	//int pos = ftell(fp);
	C = fgetc(fp);
	int len = 0;
	while((C != '\n') && (C != EOF)){C = fgetc(fp); len++;}
	fclose(fp);
	return get_source_text(start,start+len,file);
	//int pos = ftell(yyin2);
	//return get_source_line(pos);
}

int countTabs(const char *str){
	int t = 0;
	while(*str++ == '\t'){t++;}
	return t;
}


void point_out_error(int line, int pos, const char *file, const char *s, int length){
	//printf("pos: %d:%d:%d:%d\n",y1,x1,y2,x2);
    const char *str = get_source_line2(line,file);
	//fprintf(stderr, "%s\n", get_source_line(lexlinestart));
	fprintf(stderr, "\n\nfile: %s, line %d:",file, line);
	fprintf(stderr, "\n%s\n", str);
	int i;
	//for(i = 0; i < lexnumtabs; i++){fprintf(stderr, "\t");}
	//for(i = 0; i < x1 - lexnumtabs; i++){fprintf(stderr, " ");}
	if(length != 0){
		for(i = 0; i < countTabs(str); i++){fprintf(stderr, "\t");}
		for(i = 0; i < pos-countTabs(str); i++){fprintf(stderr, " ");}
		for(i = 0; i < length; i++){fprintf(stderr,"~");}
		fprintf(stderr,"\n");
	}
	
	for(i = 0; i < countTabs(str); i++){fprintf(stderr, "\t");}
	for(i = 0; i < pos - countTabs(str); i++){fprintf(stderr, " ");}
	fprintf(stderr, "^\n");
	if(s && strcmp(s,"")!=0){
		fprintf(stderr,"%s\n",s);
	}
	
}

char *stralloc(const char *str){
	int len = strlen(str);
	char *str2 = (char *)malloc(sizeof(char)*(len+1));
	strcpy(str2,str);
	return str2;
}

char *substring(const char *text, int from, int to){
	char *str = stralloc(text); 
	int len = to-from;
	strncpy(str,&text[from],len);
	str[len]=0;
	return str;
}

typedef const char* cstr;
const char* concat2(int num, ...){
	va_list args;
	va_start(args, num);
	
	char buff[2000];
	char *bp = buff;
	
	for(int I = 0; I < num; I++){
		const char *str = va_arg(args,cstr);
		int len = strlen(str);
		strcpy(bp,str);
		bp += len;
	}
	return stralloc(buff);
}

const char* itoa(int N){
	char buff[80];
	sprintf(buff,"%d",N);
	return stralloc(buff);
}


void countindent(const char *str){
	char C = *str++;
	indent = 0;
	while((C == ' ')||(C == '\t')){
		indent++;
		C = *str++;
	}
}
void printindent(){
	int i;
	for(i = 1; i < indent; i++){
		fprintf(fasm," ");
	}
}

int isnumber(const char *str){
	char C = *str++;
	while(C){
		if(!isdigit(C)){return 0;}
		C = *str++;
	}
	return 1;
}

vector2_ptr_char explode(const char *str, char delim){
	vector2_ptr_char V = vector2_ptr_char_here();
	char buff[2000];
	char *pbuff = buff;
	char C = *str++;
	while(C){
		if(C != delim){
			*pbuff++ = C;
			*pbuff = 0;
		}else{
			const char *cbuff = stralloc(buff);
			m(V,push_back,cbuff);
			pbuff = buff;
			*pbuff = 0;
		}
		C = *str++;
	}
	return V;
}

vector2_ptr_char explodeSkipEmpty(const char *str, char delim){
	vector2_ptr_char V = vector2_ptr_char_here();
	char buff[2000];
	char *pbuff = buff;
	char C = *str++;
	while(C){
		if(C != delim){
			*pbuff++ = C;
			*pbuff = 0;
		}else{
			if(strlen(buff)){
				const char *cbuff = stralloc(buff);
				m(V,push_back,cbuff);
			}
			pbuff = buff;
			*pbuff = 0;
		}
		C = *str++;
	}
	return V;
}

// formats the values as a table. The table will have aligned columns.
// The number of columns will match the number of values in the first row.
// additional values will be discarded and missing values will be padded with empty strings.
// Each line of the input string should have values separated by tab ('\t').

const char *formatAsTable(const char *str){
	//vector to store each line from str
	vector2_ptr_char V = explodeSkipEmpty(str,'\n');
	//vector to store each cell from str
	vector2_ptr_char V2 = vector2_ptr_char_here();
	
	//1. split into cells
	int numCols = 0;
	vector3_int Vcolwidth = vector3_int_here();
	for(int I = 0; I < V.size; I++){
		//for every line in the input string
		const char *line = m(V,get,I);
		//split the line into values
		vector2_ptr_char V3 = explode(line,'\t');
		if(I == 0){
			//for the first line, also initialize
			//the columns of the table.
			numCols = V3.size;
			for(int J = 0; J < numCols; J++){
				//initialize the column widths to zero
				m(Vcolwidth,push_back,0); 
			}
		}
		for(int J = 0; J < numCols; J++){
			//for every column
			const char *cell = "";
			//fetch the cell value from the line, if it has one.
			if(J < V3.size){
				cell = m(V3,get,J);
			}
			//recalculate cell width
			int cell_len = strlen(cell);
			int colwidth = m(Vcolwidth,get,J);
			if(cell_len > colwidth){
				colwidth = cell_len;
				m(Vcolwidth,set,colwidth,J);
			}
			//push the cell into the cell table.
			m(V2,push_back,cell);
		}
	}
	//2. calculate final size
	int sepWidth = 1;
	int totalWidth = sepWidth;
	for(int I = 0; I < Vcolwidth.size; I++){
		int w = m(Vcolwidth,get,I);
		totalWidth += w + sepWidth;
	}
	int sepHeight = 0;
	int lineHeight = 1;
	int numLines = V.size;
	int totalHeight = sepHeight + (sepHeight+lineHeight)*numLines;
	int numBytes = (totalWidth+1)*totalHeight; //+1 for newlines
	
	char *buff = (char*)malloc(numBytes);
	char *pbuff = buff;
	int cellnum = 0;
	//3. 
	//1. make a horizontal line
	for(int I = 0; I < totalWidth; I++){*pbuff++ = '-';}
	*pbuff++ = '\n';
	//2. make all the cell lines
	for(int I = 0; I < numLines; I++){
		*pbuff++ = '|';
		for(int J = 0; J < Vcolwidth.size; I++){
			const char *str = m(V2,get,cellnum++);
			int width = m(Vcolwidth,get,J);
			for(int K = 0; K < width; K++){
				*pbuff++ = str[K];
			}
			*pbuff++ = '|';
		}
		*pbuff++ = '\n';
		//3. make after-cell horizontal line
		for(int I = 0; I < totalWidth; I++){*pbuff++ = '-';}
		*pbuff++ = '\n';
	}
	return buff;
}






