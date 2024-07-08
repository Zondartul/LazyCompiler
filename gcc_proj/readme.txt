LazyTongue compiler
(closest ever to completion)

.. this compiler used to work, but then I tried to add
.. structure support, and it broke. let's fix it.

.. uses BISON and YACC to generate lexxer and parser.

planned language features:
tested	done:	feature:		note:
v	v	expressions
v	v	if/else/for/while
v	v	functions
v	v	return
v	v	declare-assign
x	x	type safety
x	x	classes			constructor/destructor auto called.
v	v	declare-anywhere	try using "unresolved" type.
	
other stuff to do:
x	make .c files less huge-and-confusing
x	rewrite in .lz


last thing i did: 
started making a preprocessor for those #includes and #defines.
without the preprocessor, compiler works at about the same level as HL-ZASM compiler.
--------------------------
todo:
v	make sure all the right symbols are visible in all the right places
v	get rid of :this in generated code?
v	implement pointers
x	structs
x	actual types

future awesomeness maybe:
- easy time reinterpretion
- linking requires binary libraries to be present at compilation
- binary libraries have a text segment in lazytongue, with function definitions
- those definitions are inline calls to address
- "record": variable- or unknown-size object, typically as "size,data..."
- lazyBinary format where cleartext sourcecode is followed by unescaped binary data.
- inline assembly?
- compile-time computing can be based on interpretation
- interpreted code can emit compiled code and vise-versa
- make-like rebuild ordering may be used
- support for static, load-time and run-time linking
- .lzb or .dll .lib .a ?
- variant type (sets all other vars to null)
- nullable vars
- fast vars
- auto-stacking vars (override, not overwrite, when in a scope)
- auto-serialization (needs RTTI/reflection) or auto-ducking
- include compilation tree in the binary for RTTI/reflection and debugging


>lazy main.lz
compiling main.lz...
error:main.lz:line 3: import "netBeans.lzb"
                                          ^
file "netBeans.lzb" not found
searched in:
 C:/Users/bin
 C:/libs

