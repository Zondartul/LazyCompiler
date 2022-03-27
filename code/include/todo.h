//todo:
/*
* ----------- preproc bugs: -------------------------------
* [Bug] can't find an include in the same directory as source file
* [Todo] implement >=
* ---------- lexer / parser bugs: ------------------
* [bug] lexer or yacc can't handle '\\'.
* [bug] pre-increment is not handled by semantic dispatch << c = *++fmt; >>
* [BUUG] can't read floats: 3.14 reads as "access member 14 of object 3"
* [BUUG] compiler stack corruption in yyparse->printParsed->getSourceText2
* \[check] check if solved; was overflow of a string buffer apparrently
* ---------- semantic bugs: ------------------------------
* [buf] can't handle the expression &(scr.X)
*  \[check] check if it's solved
* [FIXED Issue] lots of repeated code in semantic_analyze of operators
* [FIXED Todo] display the source code location during semantic errors
* [bug] dafuq is going on here:
	 void Prints(char *str, int col)
		int dbgNum = 0;
		while(*str)
			char c = *str;
			print("prints: i=");printnum2(dbgNum);print(",c=");
			dbgNum++;
			PrintChar(*str++, col);
		end             ^
	end                 ^
	>>> 'str' is not a variable, parameter of function name
	\[note] printnum2 irrelevant, only print() matters
	 \[bug] lookup_symbol() says str is CONST but its a VAR.
	  \[note] there may be a name conflict between arg str of Prints() and arg str of print()
* [bug] dafuq is THIS shit then?
		void delay(int n)
			int k = 1000;
		end
	>>> undefined symbol 'k' (looking in symtable ST_85).
	\[note] imperative var_decl_assign.
	 \[note] same error if we do this:
	  	int k;
		k = 1000;
		\[note] can't print symbol table at this time because of buffer overflow in getSourceText2(). Stack fuckery?
* ---------- class/struct bugs: ---------------------------
* [Bug] initial values for the class members do not get set (have to manually write in a constructor)
* [bug] class methods can't fetch member variables (might be frameclimbing, or even lack of 'this' ptr)
*  \[bug] variables and functions don't know they have a "this" symbol
*    \[bug?] semantic_flatten is in effect so the STs for individual structures are missing
*      \[issue] not sure how tightly coupled the codegen is, does it need the symbol table or not?
*        \[issue] does code gen even read aout_IR? Where does it get the IR?
* --------- codegen bugs: -------------------------
* [Issue] superfluous skeleton code is generated in assembly (debug info)
* [Issue] too many comments in assembly 
* [Bug] (null) values for lbl_to in debug info of some symbols
* [Todo] codegen comments // or another symbol
* [Todo] codegen op as "OP ADD"
* -------- overal bugs: ---------------------------------------------
* [FIXED Issue] code not copied to gmod folder
* -------- language wishlist: ----------------------------------------
* [Todo] in-code reflection to get symbol properties (check where it's at etc): rval/lval, type, symtable. symtable iteration, etc.
* [Todo] in-code keywords to inject assembly, IR code, etc.
* ---------errors -------------------------------------------------
* [Bug] error message cuts off:  <<if(sw == 0) return "Lorem ipsum dolor sit amet, " end>> -> en
* [Issue] meh error message: missing ; -> "unexpected END" (next token)
* 
* 
* --------- unsorted -------------------
* [FIXED Todo] memory safety - replace all sprintfs with snprintfs or other safety things.
*  \[note] replaced s(n)printf with vec_printf
* [Todo] memory safety - remove all manually managed buffers, replace with vec.
* 
* 
* 
*/