//todo:
/* num tickets: 41
* ----------- preproc bugs: -------------------------------
* [Bug 1] can't find an include in the same directory as source file
* [Todo 2] implement >=
* ---------- lexer / parser bugs: ------------------
* [bug 3] lexer or yacc can't handle '\\'.
* \[note] neither can HL-ZASM but '\' is fine
* [bug 4] pre-increment is not handled by semantic dispatch << c = *++fmt; >>
* [BUUG 5] can't read floats: 3.14 reads as "access member 14 of object 3"
* [BUUG 6] compiler stack corruption in yyparse->printParsed->getSourceText2
* \[check] check if solved; was overflow of a string buffer apparrently
* ---------- semantic bugs: ------------------------------
* [buf 7] can't handle the expression &(scr.X)
*  \[check] check if it's solved
* [todo 12] make all declarations look check if symbol is already declared and throw an error if so.
* [bug 14] *buff++ = 0; doesn't work, but buff[0] = 0; buff++; does.
*  \[note] *buff = 0; also doesn't work.
* ---------- class/struct bugs: ---------------------------
* [Bug 17] initial values for the class members do not get set (have to manually write in a constructor)
* [bug 18] class methods can't fetch member variables (might be frameclimbing, or even lack of 'this' ptr)
*  |[bug] variables and functions don't know they have a "this" symbol
*  | \[bug?] semantic_flatten is in effect so the STs for individual structures are missing
*  |   \[issue] not sure how tightly coupled the codegen is, does it need the symbol table or not?
*  |     \[issue] does code gen even read aout_IR? Where does it get the IR?
   \[fixed note] the first argument in a function is considered the 'second' argument
   |       and 'this' is considered the first argument,
   |	   but 'this' isn't pushed by IR.
   \[note] semantic: function's symtable (ST8) is parented to the struct symtable (ST9), 
   |		but symbol 'this' in ST8 refers to class symtable 'Derp' which is empty.
   |		huh.
   |		\[edit] 'this' doesn't even link to a scope, it's a param-type.
   |			\[note] fixed lookup_symbol_this to take note of this fact and look in this.type.symclass
   \[fixed note] now an individual struct works but we are still not accounting for its size
   |		in either semantic or codegen.
   \[CHECK IF FIXED]

* --------- codegen bugs: -------------------------
* [Issue 19] superfluous skeleton code is generated in assembly (debug info)
* [Issue 20] too many comments in assembly 
* [Bug 21] (null) values for lbl_to in debug info of some symbols
* [Todo 22] codegen comments // or another symbol
* [Todo 23] codegen op as "OP ADD"
* [Todo 24] add the ops: div_i32, div_f64, fint
* -------- overal bugs: ---------------------------------------------
* -------- language wishlist: ----------------------------------------
* [Todo 26] in-code reflection to get symbol properties (check where it's at etc): rval/lval, type, symtable. symtable iteration, etc.
* [Todo 27] in-code keywords to inject assembly, IR code, etc.
* ---------errors -------------------------------------------------
* [Bug 28] error message cuts off:  <<if(sw == 0) return "Lorem ipsum dolor sit amet, " end>> -> en
* [Issue 29] meh error message: missing ; -> "unexpected END" (next token)
* 
* 
* --------- unsorted -------------------
* [Todo 31] memory safety - remove all manually managed buffers, replace with vec.
* [issue 34] gst2 overshoot report happens a lot, what's that do?
* [zCPU bug 35] hlzasm tokenizer can't handle \\ (skips it) or \xnnn,
*            so only way to insert \\ into string is as a number 92.
* [Todo 37] make a restart mechanism for the compiler, though we need to reset some globals...
* [issue 38] Codegen shows wrong line when error reporting
* [issue 39] codegen doesn't show IR line that caused an error
* [Todo 40] IR emit comments about ID, const, func, should tell which one they are referring to
* [bug 41] while fixing RV for structs - member &x is looked up incorrectly in code gen:
*			foo(&x) <- EBP:#-1;		
*			...which kind of resembles the struct being pushed whole by value onto the stack.
*			should be:	foo(&x) <- <x.pos = 0>
*		\[note] the idea is that a "frame is a scope" and not actual function frame,
*				and so the frame of a method function is inside the frame (scope) of a struct,
*				BUT this makes "frame climbing" impossible in assembly as the struct frame doesn't
*				physically exist (unless the entire struct is pushed to stack before call and saved
*				from struct after call)...
*				When the IR symbol &x is looked up, there is no way to tell which scope it belongs to
*		\[idea] just make symbols in a struct have "MEMBER" type instead of "VAR"
* [bug 42] class object in global scope is not initialized
*			- added unit test: test_7_class_global
* [bug 43] class: can't call user constructor:
*		"class Dev_address_bus addr_bus(65536, 20000,20000,20000)"
*										^ unexpected INTEGER, expecting ')'
* [todo 44] "adr? you mean addr?" - text distance on "undefined symbol" hit
* [todo 45] (int*) type conversion
* [todo 46] {1,2,3} list initializer
* [todo 47] type checking (e.g. class vs int)
*			47.1 : argument type checking
*			47.2 : assignment type checking
* [todo 48] warning when a variable is shadowed
* [todo 49] add setting to decide where the stack lives (codegen.c: print_skeleton_start)
* [bug 50] IR_next_name needs to be case-insensitive because stupid zASM has case-insensitive labels
* [bug 51] 'void *ptr = class' is allowed but it's a bug! need need NEED type checks on assign!
* [bug 52] number of arguments in a function is not checked (can this break 'this'?)
* [todo 53] change struct type_name to tell if type is an array, also change 'poitnerlevel' to 'is_pointer' and add 'pointed-type'
* [FIXED 54] some derp4 class gets constructed in prologue after call to main (unreachable code) i.e. never.
*		-- turns out semantic_analyze_program would insert a "call main" at the beginning* of a scope and didn't control for when the initializers appeared
*
*
*
* ----- FIXED bugs: ----------------
* [FIXED Issue 8] lots of repeated code in semantic_analyze of operators
* [FIXED Todo 9] display the source code location during semantic errors
* [FIXED bug 10] dafuq is going on here:
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
		\[note] turns out it was due to the "x" string constant, which takes a username "str",
				thus shadowing the char *str arg of Prints().
				changed string constants to have no username (they're supposed to be anon)
* [(MERGED 12) bug 11] dafuq is THIS shit then?
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
* [FIXED bug 13] this doesn't work:
	|	int End = 0;
	|	while(!End)
	|	... loop doesn't execute at all.
	[note] this doesn't work either: if(!num)
	 \[note] turns out, "not" isn't an asm op but "lneg" is.
* [FIXED BUUUG 15] if-then is completely fucked. psuedocode:
		func test(x): if(x) print(yes) else print(no) end
		>>test(1): yes no end
		>>test(0): end
		if-else will either do both or none, not A or B. this is stupid.
		\[note] check if fixed - altered which if_settings are passed in if_then
* [FIXED bug 16] expr_neg result is discarded, wrong result goes to function arg. see:
		//[ NEG 123 123]                                                               // @ codegen_gen_command:924 I
		//discarding 1 from R2                                                         // @ allocRegister:415 I
		//load 123 into R2                                                             // @ loadLValue:649 I
		mov R2, 123                                                                    // @ loadLValue:652 I
		neg R2                                                                         // @ gen_command_neg:415 I
		//discard 123 from R2                                                          // @ storeValue:729 I
		//[ CALL temp_1118 _printf str_91 0 1 123]                                     // @ codegen_gen_command:924 I
		push 123
		>>> 123 is pushed instead of -123
* [FIXED Issue 25] code not copied to gmod folder
* [FIXED Todo 30] memory safety - replace all sprintfs with snprintfs or other safety things.
*  \[note] replaced s(n)printf with vec_printf
* [FIXED BUG 32] somehow this causes a segfault in vector2_ptr_char_internal_resize.
	printf("hi%dm\nye\n\\n\\\\",1);
	\[note] that was because of unsanitized user input for format.
* [FIXED issue 33] lazy2.txt printf can't see \ slashes, maybe because of internally unescaped strings?
*  \[note] turns out HL-ZASM erases \\, so we replace it with db 92.
* [FIXED bug 36] pointOutError eats the last character of the line

*
* Note 17 July 2024:
* 	we finally have classes working! most obvious stuff is now handled by
*   89 unit tests that run on zCPU (unit_tests.txt)
*   Next we wanna package this version as "LazyCompiler base"
*	though it's more like "raw". No new language features in this version.
*	New features only after this is usable in day-to-day.
*	Stuff that's missing before "base" is product-like:
*   - Running without VScode (probably already works)
*	- asm command to embed assembly (with $ replacement or some way to interact with vars and functions)
*	- Debug IDE features:
*	--- step-through-code (maybe instrumentation breakpoints)
*	--- call stack trace
*	--- view local variables
*	 (should IDE be in zCPU, gmod wire, gmod derma, or stand-alone?)
*	- simulation?
*	--- IR sim?
*	--- zCPU emulator? 
*	- assembler?
*	- optimizer? or is it a feature? idk. Maybe good for LazyCompiler base v1.1
*
*/