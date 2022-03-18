#ifndef VARVALUE_GUARD
#define VARVALUE_GUARD

struct varvalue{
	union{
		int valint;
		float valfloat;
		char *valstring;
	};
};

#endif