[bug 13] this doesn't work:
	|	int End = 0;
	|	while(!End)
	|	... loop doesn't execute at all.
	[note] this doesn't work either: if(!num)
---------------
minimal test:
1. while(!End)
extended test:
1. while(!End)
2. if(!num)
3. num = !num
4. for(int i = 0; !i; i = !i) end
