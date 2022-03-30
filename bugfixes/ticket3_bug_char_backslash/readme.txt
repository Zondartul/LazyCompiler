---------- lexer / parser bugs: ------------------
[bug 3] lexer or yacc can't handle '\\'.
-------------
test mininal:
1. char value '\\'
test exteneded
1. char value '\\'
2. char value '\xnn'
3. char value '\unnnnn'
