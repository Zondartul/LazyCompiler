* [bug 18] class methods can't fetch member variables (might be frameclimbing, or even lack of 'this' ptr)
*  \[bug] variables and functions don't know they have a "this" symbol
*    \[bug?] semantic_flatten is in effect so the STs for individual structures are missing
*      \[issue] not sure how tightly coupled the codegen is, does it need the symbol table or not?
*        \[issue] does code gen even read aout_IR? Where does it get the IR?
--------------------
minimal test:
1. X;
extended test:
1. X;
2. this;
3. this->X;
