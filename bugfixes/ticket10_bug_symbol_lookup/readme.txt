[bug 10] dafuq is going on here:
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
----------------
minimal test:
1. just this
extended test:
... see lazy2.txt, bug may be gone already
