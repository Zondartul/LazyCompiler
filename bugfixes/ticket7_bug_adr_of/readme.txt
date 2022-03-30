[buf 7] can't handle the expression &(scr.X)
-------
minimal test:
1. &(scr.X)
extended test:
1. &(scr.X)
2. &X
3. &arg
4. &(arr[5]);
5. (&arr)[5];
