//todo:
/*
* 
* 
* [Bug] initial values for the class members do not get set (have to manually write in a constructor)
* [bug] class methods can't fetch member variables (might be frameclimbing, or even lack of 'this' ptr)
*  \[bug] variables and functions don't know they have a "this" symbol
*    \[bug?] semantic_flatten is in effect so the STs for individual structures are missing
*      \[issue] not sure how tightly coupled the codegen is, does it need the symbol table or not?
*        \[issue] does code gen even read aout_IR? Where does it get the IR?
* [buf] can't handle the expression &(scr.X)
*  \[check] check if it's solved
* [Issue] superfluous skeleton code is generated in assembly (debug info)
* [Issue] too many comments in assembly 
* [Bug] (null) values for lbl_to in debug info of some symbols
* [Issue] lots of repeated code in semantic_analyze of operators
* [Todo] display the source code location during semantic errors
* [Todo] codegen comments // or another symbol
* [Todo] codegen op as "OP ADD"
* [FIXED Issue] code not copied to gmod folder
* [Todo] in-code reflection to get symbol properties (check where it's at etc): rval/lval, type, symtable. symtable iteration, etc.
* 
*/