[bug 4] pre-increment is not handled by semantic dispatch << c = *++fmt; >>
-----------------
minimal test:
1. ++x
extended test:
1. ++x
2. ++x--;
3. ++++x;
3. c = *++fmt;
