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
	|[note] imperative var_decl_assign.
	|\[note] same error if we do this:
	| 	int k;
	|	k = 1000;
	|	\[FIXED note] can't print symbol table at this time because of buffer overflow in getSourceText2(). Stack fuckery?
	[note] was actually because function delay() was already defined elsewhere so we fetched an older symtable.
* [todo] make all declarations look check if symbol is already declared and throw an error if so.
* [bug] this doesn't work:
	|	int End = 0;
	|	while(!End)
	|	... loop doesn't execute at all.
	[note] this doesn't work either: if(!num)
* [bug] *buff++ = 0; doesn't work, but buff[0] = 0; buff++; does.
*  \[note] *buff = 0; also doesn't work.
* [BUUUG] if-then is completely fucked. psuedocode:
		func test(x): if(x) print(yes) else print(no) end
		>>test(1): yes no end
		>>test(0): end
		if-else will either do both or none, not A or B. this is stupid.
* [bug] expr_neg result is discarded, wrong result goes to function arg. see:
		//[ NEG 123 123]                                                               // @ codegen_gen_command:924 I
		//discarding 1 from R2                                                         // @ allocRegister:415 I
		//load 123 into R2                                                             // @ loadLValue:649 I
		mov R2, 123                                                                    // @ loadLValue:652 I
		neg R2                                                                         // @ gen_command_neg:415 I
		//discard 123 from R2                                                          // @ storeValue:729 I
		//[ CALL temp_1118 _printf str_91 0 1 123]                                     // @ codegen_gen_command:924 I
		push 123
		>>> 123 is pushed instead of -123
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
* [Todo] add the ops: div_i32, div_f64, fint
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
* [BUG] somehow this causes a segfault in vector2_ptr_char_internal_resize. 
	printf("hi%dm\nye\n\\n\\\\",1);
* [issue] lazy2.txt printf can't see \ slashes, maybe because of internally unescaped strings?
* [issue] gst2 overshoot report happens a lot, what's that do?
* [zCPU bug] hlzasm tokenizer can't handle \\ (skips it) or \xnnn,
*            so only way to insert \\ into string is as a number 92.
* [FIXED bug] pointOutError eats the last character of the line
* [Todo] make a restart mechanism for the compiler, though we need to reset some globals...
* 
*/