* [BUG 32] somehow this causes a segfault in vector2_ptr_char_internal_resize. 
	printf("hi%dm\nye\n\\n\\\\",1);

// note: this is likely due to printf(user_input) as format string.

minimal test:
1. printf("hi%dm\nye\n\\n\\\\",1);