#ifndef vector3INT_GUARD
#define vector3INT_GUARD

typedef int T;
//#define definition_vector_of(T)
struct vector3_int{	
	const char *canary; /* should be set to the name of this vector so we know it's initialized */
	int size;	/* how many elements there currently are */
	int capacity;	/* how many elements can fit before automatic resizing occurs */
	T *data;	/* pointer to the vector's buffer. this may be relocated following any vector operation. */
	/* the rest are methods of the vector object. */
	/* initializes the vector (do it or suffer segfaults) */
	void	(*constructor)(struct vector3_int *this);
	/* destroys the vector by freeing the data buffer. */
	void	(*destructor)(struct vector3_int *this);
	/* makes the vector empty */
	void	(*clear)(struct vector3_int *this);
	/* private. do not use. */
	void	(*internal_set)(struct vector3_int *this, T* element, int pos);
	/* private. do not use. */
	T*		(*internal_get)(struct vector3_int *this, int pos);
	/* private. do not use. */
	void	(*internal_resize)(struct vector3_int *this, int newsize);
	/* add the element to the end of the vector */
	void	(*push_back)(struct vector3_int *this, T element);	
	/* add the element to the beginning of the vector */
	void	(*push_front)(struct vector3_int *this, T element);
	/* remove and return an element from the end */
	T		(*pop_back)(struct vector3_int *this);
	/* remove and return an element from the beginning */
	T		(*pop_front)(struct vector3_int *this);
	/* put the element into the given position, shifting previous contents forward */
	void	(*insert)(struct vector3_int *this, T element, int pos);
	/* remove the element from the given position, shifting remaining contents backwards */
	void	(*erase)(struct vector3_int *this, int pos);
	/* return a copy of the element at the position */
	T		(*get)(struct vector3_int *this, int pos);
	/* overwrite the element at the position */
	void	(*set)(struct vector3_int *this, T element, int pos);
};
typedef struct vector3_int vector3_int;
	/* creates and initializes a new vector on the heap, returns a pointer to it. */
vector3_int* vector3_int_new();
	/* creates and initializes a new vector on the stack, returns it. */
	/* also adds the vector to the "destroy warning" list */
struct vector3_int vector3_int_here();
	/* Actual implementations of the above methods. Do not worry about the rest of these. */
void vector3_int_constructor(struct vector3_int *this);
void vector3_int_destructor(struct vector3_int *this);
void vector3_int_clear(struct vector3_int *this);
void vector3_int_internal_set(struct vector3_int *this, T* element, int pos);
T* vector3_int_internal_get(struct vector3_int *this, int pos);
void vector3_int_internal_resize(struct vector3_int *this, int newsize);
void vector3_int_push_back(struct vector3_int *this, T element);
void vector3_int_push_front(struct vector3_int *this, T element);
T vector3_int_pop_back(struct vector3_int *this);
T vector3_int_pop_front(struct vector3_int *this);
void vector3_int_insert(struct vector3_int *this, T element, int pos);
void vector3_int_erase(struct vector3_int *this, int pos);
T vector3_int_get(struct vector3_int *this, int pos);
void vector3_int_set(struct vector3_int *this, T element, int pos);

//put this in the .cpp file where you want the
//template instantiation's implementation to be.
#define v3print(...) fprintf(stderr, "vector3 error: " __VA_ARGS__)
#define v3flush()	 fflush(stderr)
#define v3crash()	 exit(*(int*)0)
#define v3error(...) v3print(__VA_ARGS__); v3flush(); v3crash();
//#define implementation_vector_of(T)


#endif