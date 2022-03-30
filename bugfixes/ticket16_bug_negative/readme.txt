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
----------------------
minimal test:
1. -123
