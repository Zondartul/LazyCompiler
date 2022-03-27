#include "stdio.h"
#include "preprocess.h"
#include "ast.h"
#include "semantic.h"
#include "codegen.h"
#include "yaccin.tab.h"
extern FILE *yyin;
extern FILE *yyin2;

//yaccin.tab.h has "int yyparse (void);"
//void yyparse();

void printstamp(){
	fprintf(stderr,"\n"
		"===============================\n"
		"=    #                        =\n"
		"= # #   Compiled Successfully =\n"
		"=  #                          =\n"
		"===============================\n"
		);
}

const char *path_out_preproc = "data_out/aout_preproc.txt";
const char *path_in_preproc = "data_out/ain_preproc.txt";
const char *path_out_parse = "data_out/aout_parse.txt";
const char *path_out_semantic = "data_out/aout_semantic.txt";
const char *path_out_IR = "data_out/aout_IR.txt";
const char *path_out_codegen = "data_out/aout_codegen.txt";
const char *path_out_assembly = "data_out/assembly.txt";
const char *path_out_2 = "data_out/aout2.txt";
const char *path_out_graphviz = "data_out/aout3.gv";

const char* formatAsTable(const char* str);

void test_formatAsTable() {
	//this was a table test. it passes after fixes of 18.03.2022
	const char* myTable = "beep\tT\tval\tpos\n"			//4
		"oh\thi\twhat's\tup?\n"		//4
		"this\tline\tshort\n"			//3
		"and\t\tone\tmissing\n"		//3
		"this\tis\tthe\tlast\tline";	//5
	const char* formtable = formatAsTable(myTable);
	//printf("table:----------------\n%s\n-------------------\n",formtable);
	printf("table:\n%s\n", formtable);
}

void test_vec_printf() {
	vector2_char vstr = vector2_char_here();
	vec_printf(&vstr, "hello this is a test [%d] [%f], ", 123, 3.14);
	vec_printf(&vstr, "this is another test [%s], ", "hello");
	vec_nprintf(&vstr, 13, "quick [%s] fox jumps over the lazy dog", "lazy");
	vec_printf(&vstr, ", that's all folks.");
	printf("%s\n", vstr.data);
}

int main(int argc, char **argv){
	initFiles();
	if(argc == 2){
		yyin = fopen(argv[1],"r");
		yyin2 = fopen(argv[1],"r");
	}else{
		printf("usage: %s inputfile\n",argv[0]);
		return 0;
	}
	
	fprintf(stderr,"INPUT:\t\t\t%s\n",argv[1]);
	fprintf(stderr,"PREPROC:\t\t%s\n",path_out_preproc);
	fprintf(stderr,"TOK/SYNTAX:\t\t%s\n",path_out_parse);
	fprintf(stderr,"SEMANTIC:\t\t%s\n",path_out_semantic);
	fprintf(stderr,"IR:\t\t\t%s\n",path_out_IR);
	fprintf(stderr,"CODEGEN:\t\t%s\n",path_out_codegen);
	fprintf(stderr,"ASSEMBLY:\t\t%s\n",path_out_assembly);
	fprintf(stderr,"\n");
	
	test_formatAsTable();
	test_vec_printf();
	return 0;

	fprintf(stderr,"initializing...");
	//printf("sizeof ast_node = '%d'\n",sizeof(ast_node));
	//int *test = malloc(sizeof(int));
	//printf("test diff = '%d', size = '%d'\n", (char*)(test+1)-(char*)(test),sizeof(int));
	initallocs();
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	ast_init();
	//yydebug = 1;
	yylloc.last_line = 1;
	yylloc.end = 0;
	lextokenend = 0;
	lexnumtabs = 0;
	fprintf(stderr,"\t\tdone\n");
	

	
	fprintf(stderr,"preprocessing...");
	freopen(/*"aout_preproc.txt"*/path_out_preproc,"w",stdout);
	preprocess(argv[1]);
	fprintf(stderr,"\tdone\n");
	
	fprintf(stderr,"parsing...");
	//fclose(stdout);
	freopen(/*"aout_parse.txt"*/path_out_parse,"w",stdout);
	//fclose(yyin);
	fclose(yyin2);
	yyin = fopen(/*"ain_preproc.txt"*/path_in_preproc,"r");
	if(!yyin){error("can't fopen ");}
	yyin2 = fopen(/*"ain_preproc.txt"*/path_in_preproc,"r");
	if(!yyin2){error("can't freopen ");}
	yyparse();
	fprintf(stderr,"\t\tdone\n");
	
	
	fprintf(stderr,"printing ast...");
	FILE *fp = fopen(/*"aout2.txt"*/path_out_2,"w");
	if(!fp){return 1;}
	ast_print(/*ast_get_node(finalNode)*/ finalNode, fp);
	fclose(fp);
	fp = fopen(/*"aout3.gv"*/path_out_graphviz,"w");
	if(!fp){return 1;}
	ast_print_graph(/*ast_get_node(finalNode)*/finalNode,fp,"A"); //graphViz file
	fprintf(stderr,"\t\tdone\n");
	
	fprintf(stderr,"analyzing...");
	freopen(/*"aout_semantic.txt"*/path_out_semantic,"w",stdout);
	semantic_init();
	semantic_general_analyze(/*ast_get_node(finalNode)*/finalNode);
	fprintf(stderr,"\t\tdone\n");
	
	fprintf(stderr,"printing semantic...");
	freopen(/*"aout_IR.txt"*/path_out_IR,"w",stdout);
	setbuf(stdout, NULL);
	print_semantic();
	fprintf(stderr,"\tdone\n");
	
	fprintf(stderr,"generating...");
	freopen(/*"aout_codegen.txt"*/path_out_codegen,"w",stdout);
	codegen_generate();
	fprintf(stderr,"\t\tdone\n");

	flushAllFiles();
	//copy the result over to gmod folder
	const char* cmd_str = "copy "
		"\"D:\\Stride\\compiler\\LazyComp\\data_out\\aout_assembly.txt\" "
		"\"E:/PROGRA~2/Steam/steamapps/common/GarrysMod/garrysmod/data/cpuchip/lazycomp/aout_assembly.txt\" "
		//	"\"E:\\Program Files(x86)\\Steam\\steamapps\\common\\GarrysMod\\garrysmod\\data\\cpuchip\\lazycomp\\aout_assembly.txt\""
		;
	fprintf(stderr, "%s\n", cmd_str);
	int res = system(cmd_str);
	if (res) { fprintf(stderr, "copy failed: %d\n", res); }
	else {fprintf(stderr, "file copied to garrysmod/data/cpuchip\n");}

	printstamp();
	
	
	return 0;
}

//return 0 if we just set the lexer to a new file,
//or 1 if there are no more files to process.
//also this should be done by %noyywrap option instead.
int yywrap(){return 1;}
