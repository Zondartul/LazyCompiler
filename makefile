CC = gcc
CCFLAGS = -fmax-errors=1 -g -Werror -Wfatal-errors
SOURCEFILES = lex.yy.c yaccin.tab.c vector.c AST.c semantic.c
LEX = flex.exe
BISON = bison.exe
LIBS = -L"C:\GnuWin32\lib" -lfl

a.exe: $(SOURCEFILES)
	$(CC) $(SOURCEFILES) $(LIBS) $(CCFLAGS)
	
lex.yy.c: lexin.l
	$(LEX) lexin.l
	
yaccin.tab.c: yaccin.y
	$(BISON) yaccin.y -d -v
	