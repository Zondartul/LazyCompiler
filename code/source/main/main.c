#include "stdio.h"
#include "preprocess.h"
#include "AST.h"
#include "semantic.h"
#include "codegen.h"
#include "yaccin.tab.h"
#include "main.h"
#include "typecheck.h"
extern FILE *yyin;
extern FILE *yyin2;

//yaccin.tab.h has "int yyparse (void);"
//void yyparse();

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"

void printstamp(){
	fprintf(stderr,"\n" KGRN
		"===============================\n"
		"=    #                        =\n"
		"= # #   Compiled Successfully =\n"
		"=  #                          =\n"
		"===============================\n"
		KNRM );
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

int argc_stored = 0;
char** argv_stored = 0;

int compiler_restart_enabled = 0;

int main_helper(int argc, char **argv);

void restart_handler() {
	//atexit: this pushes a new value onto stack rather than replaces it so 0 doesn't work//atexit(0); //reset the callback in case we die before we set everything up
	freopen(0, "w", stdout); //reset the stdout to terminal
	printf("Restart?");
	int repl = 0;
	int isY, isN, isRepl;
	do {
		printf("Y/n\n");
		repl = getchar();//getch();
		isY = ((repl == 'y') || (repl == 'Y'));
		isN = ((repl == 'n') || (repl == 'N'));
		isRepl = isY || isN;
	} while (!isRepl);
	if(isY)
	{
		atexit(restart_handler);
		main_helper(argc_stored, argv_stored);
	}
	else {
		exit(0);
	}
}


int main(int argc, char** argv) {
	if (compiler_restart_enabled) {
		argc_stored = argc;
		argv_stored = argv;
		atexit(restart_handler);
		main_helper(argc, argv);
		//atexit(0);
		return 0;
	}
	else {
		main_helper(argc, argv);
	}
}

int doing_semantic = 0;
const char *out_file = 0;
extern int yynerrs;


T_cli_options cli_options = {"ERROR", "ERROR", 1, 1};
extern int comments;   // codegen.c
extern int trace_gens; // codegen.c

int main_helper(int argc, char **argv){
	initFiles();

	//if(argc >= 2){
	//	yyin = fopen(argv[1],"r");
	//	yyin2 = fopen(argv[1],"r");
	//}else{
	//	printf("usage: %s inputfile\n",argv[0]);
	//	return 0;
	//}
	
	/// argc == 0 - never
	/// argc == 1 - this = [~/lazycomp]
	/// argc == 2+ - [~/lazycomp] [infile] ...
	
	/// ---- CLI command line options parser -------

	#define OPT_IS(x) strcmp(opt, x) == 0
	#define PARSE_OPT(x)	if(OPT_IS(x))
	#define PARSE_OPT_W_ARG(x)	\
		PARSE_OPT(x) for((arg = argv[I++]) && (once=1); once; once=0)
		// funky syntax hax yar har har ;)

	if(argc < 2){goto bad_args;}
	int I = 1;
	int once = 0;
	while(I < argc){
		const char *opt = argv[I++];
		const char *arg = 0;
		PARSE_OPT("-h"){printf("I'm helping!");}
		else PARSE_OPT_W_ARG("-i"){
			cli_options.in_file = arg;
		}
		else PARSE_OPT_W_ARG("-o"){
			cli_options.out_file = arg;
		}else PARSE_OPT("--no-asm-comments"){
			cli_options.b_asm_comments = 0;
		}else PARSE_OPT("--no-asm-trace"){
			cli_options.b_asm_trace = 0;
		}
		else goto bad_args;
	}
	goto good_args;

	/// ------------ end CLI --------
bad_args:
	printf("usage: lazycomp [-i input_file][-o output_file]\n");
	return 0;
good_args:
	fprintf(stderr, "final cli_options:\n{\n\tin_file: [%s]\n\tout_file: [%s]\n}\n", cli_options.in_file, cli_options.out_file);

	yyin  = fopen(cli_options.in_file,"r");
	yyin2 = fopen(cli_options.in_file,"r");

	/// set some options
	comments = cli_options.b_asm_comments;
	trace_gens = cli_options.b_asm_trace;
	fprintf(stderr, "THIS:\t\t\t%s\n", argv[0]);
	fprintf(stderr,"INPUT:\t\t\t%s\n",cli_options.in_file);
	fprintf(stderr,"PREPROC:\t\t%s\n",path_out_preproc);
	fprintf(stderr,"TOK/SYNTAX:\t\t%s\n",path_out_parse);
	fprintf(stderr,"SEMANTIC:\t\t%s\n",path_out_semantic);
	fprintf(stderr,"IR:\t\t\t%s\n",path_out_IR);
	fprintf(stderr,"CODEGEN:\t\t%s\n",path_out_codegen);
	fprintf(stderr,"ASSEMBLY:\t\t%s\n",path_out_assembly);
	fprintf(stderr,"\n");
	
	//test_formatAsTable();
	//test_vec_printf();
	//return 0;

	fprintf(stderr,"initializing...");
	//printf("sizeof ast_node = '%d'\n",sizeof(ast_node));
	//int *test = malloc(sizeof(int));
	//printf("test diff = '%d', size = '%d'\n", (char*)(test+1)-(char*)(test),sizeof(int));
	initallocs();
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	ast_init();
	init_typechecks();
	//yydebug = 1;
	yylloc.last_line = 1;
	yylloc.end = 0;
	lextokenend = 0;
	lexnumtabs = 0;
	fprintf(stderr,"\t\tdone\n");
	

	
	fprintf(stderr,"preprocessing...");
	freopen(/*"aout_preproc.txt"*/path_out_preproc,"w",stdout);
	preprocess(cli_options.in_file);
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
	int res = yyparse();
	if((res == 1) || (yynerrs != 0)){
		fprintf(stderr,"%sFailed to parse source code due to syntax error.\n%s", KRED, KNRM);
		return 1;
	}
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
	
	doing_semantic = 1;
	fprintf(stderr,"analyzing...");
	freopen(/*"aout_semantic.txt"*/path_out_semantic,"w",stdout);
	semantic_init();
	semantic_general_analyze(/*ast_get_node(finalNode)*/finalNode);
	fprintf(stderr,"\t\tdone\n");
	doing_semantic = 0;

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
	/*
	int isOnLaptop = (strcmp(argv[0], "E:\\projects\\LazyComp\\bin\\LazyComp.exe") == 0);

	const char* cmd_str = 0;
	if (isOnLaptop) {
		cmd_str = "copy /Y "
			"E:\\projects\\LazyComp\\data_out\\aout_assembly.txt "
			"E:\\SteamLibrary\\steamapps\\common\\GarrysMod\\garrysmod\\data\\cpuchip\\lazycomp\\aout_assembly.txt"
			;
	} else { //on main pc
		cmd_str = "copy "
			"\"D:\\Stride\\compiler\\LazyComp\\data_out\\aout_assembly.txt\" "
			"\"E:/PROGRA~2/Steam/steamapps/common/GarrysMod/garrysmod/data/cpuchip/lazycomp/aout_assembly.txt\" "
			;
	}
	*/
	if(cli_options.out_file){
		/// copy the output file
		char cmd_str[200];
		snprintf(cmd_str, 200, "cp ./data_out/aout_assembly.txt %s", cli_options.out_file);

		int res = system(cmd_str);
		if (res) { 
			fprintf(stderr, "%s\n", cmd_str);
			fprintf(stderr, "copy failed: %d\n", res); 
		}
		else {
			//fprintf(stderr, "file copied to garrysmod/data/cpuchip\n");
			printf("file copied to %s", cli_options.out_file);
		}
	}
	printstamp();
	
	
	return 0;
}

//return 0 if we just set the lexer to a new file,
//or 1 if there are no more files to process.
//also this should be done by %noyywrap option instead.
int yywrap(){return 1;}
