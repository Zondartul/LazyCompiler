#ifndef VECTOR_GUARD
#define VECTOR_GUARD
typedef void (*funcptr)(void *arg);
struct vector{
	//data is untyped;
	//when the vector accepts an element, it
	//is bitwise-copied from a void pointer to
	//a location within the data buffer that is multiple
	//of the element size.
	//when the vector returns an element, it
	//copies it to malloc'ated memory and returns
	//a void pointer. It is the caller's responsibility
	//to free the pointer after use.
	
	//current size of the vector (readonly)
	int size;
	//size of each element (readonly)
	int element_size;
	//funcptr elem_constructor;
	//funcptr elem_destructor;
	//how many elements will fit before a reallocation is needed (readonly)
	int reserved;
	//buffer where data is stored BY VALUE (private)
	void *data;
};
typedef struct vector vector;
//initialize a vector to store elements of given size
void vector_constructor(vector *this, int element_size);
//destroy the vector, optionally calling element destructors.
void vector_destructor(vector *this, funcptr element_destructor);
//add a new element to the end of the vector.
void vector_push_back(vector *this, void *newelem);
//return the last element and remove it from the vector.
void *vector_pop_back(vector *this);
//insert an element into position, shifting current and later elements forward.
void vector_insert(vector *this, void *newelem, int pos);
//return and remove the element from the given position.
void *vector_remove(vector *this, int pos);
//returns a void pointer to a copy of the element
void *vector_get_copy(vector *this, int pos);
//returns a void pointer to the element within the array
void *vector_get_reference(vector *this, int pos);
//sets the new contents to the element at position
void vector_set(vector *this, int pos, void *newelem);
//resize the vector to fit more elements (automatic)
void vector_resize(vector *this, int newsize);
#endif