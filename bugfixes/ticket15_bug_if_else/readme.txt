[BUUUG 15] if-then is completely fucked. psuedocode:
		func test(x): if(x) print(yes) else print(no) end
		>>test(1): yes no end
		>>test(0): end
		if-else will either do both or none, not A or B. this is stupid.
----------------------------
minimal test:
1. if(x) yes else no end
extended test:
1. if(x) yes else no end
2. if(x) yes end if(x == 0) no end
3. if(x == 1) one elseif (x==2) two else three end
4. if(x == 1) one else if(x == 2) two else three end end
5. if(x == 1) one elseif (x==2) two end