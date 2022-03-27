#ifndef VECTOR2_GUARD
#define VECTOR2_GUARD
#include "stdio.h"
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"

//this calls a method on an object-like struct.
#define m(x,y,...) (x).y(&(x),##__VA_ARGS__)

//this declares the variable type of a template's given instantiation.
//also, use "typedef struct X X;" or else whitespace will give you trouble.
//put it in .h files.
#define definition_vector_of(T)	\
struct vector2_##T{	\
	const char *canary; /* should be set to the name of this vector so we know it's initialized */\
	int size;	/* how many elements there currently are */\
	int capacity;	/* how many elements can fit before automatic resizing occurs */\
	T *data;	/* pointer to the vector's buffer. this may be relocated following any vector operation. */\
	/* the rest are methods of the vector object. */\
	/* initializes the vector (do it or suffer segfaults) */\
	void	(*constructor)(struct vector2_##T *this);	\
	/* destroys the vector by freeing the data buffer. */\
	void	(*destructor)(struct vector2_##T *this);	\
	/* makes the vector empty */\
	void	(*clear)(struct vector2_##T *this);	\
	/* private. do not use. */\
	void	(*internal_set)(struct vector2_##T *this, T* element, int pos);	\
	/* private. do not use. */\
	T*		(*internal_get)(struct vector2_##T *this, int pos);	\
	/* private. do not use. */\
	void	(*internal_resize)(struct vector2_##T *this, int newsize);	\
	/* add the element to the end of the vector */\
	void	(*push_back)(struct vector2_##T *this, T element);	\
	/* add the element to the beginning of the vector */\
	void	(*push_front)(struct vector2_##T *this, T element);	\
	/* remove and return an element from the end */\
	T		(*pop_back)(struct vector2_##T *this);	\
	/* remove and return an element from the beginning */\
	T		(*pop_front)(struct vector2_##T *this);	\
	/* put the element into the given position, shifting previous contents forward */\
	void	(*insert)(struct vector2_##T *this, T element, int pos);	\
	/* remove the element from the given position, shifting remaining contents backwards */\
	void	(*erase)(struct vector2_##T *this, int pos);	\
	/* return a copy of the element at the position */\
	T		(*get)(struct vector2_##T *this, int pos);	\
	/* overwrite the element at the position */\
	void	(*set)(struct vector2_##T *this, T element, int pos);	\
	/* normal vector resize operation, size (num of elements) changes, capacity may also change. */\
	void	(*resize)(struct vector2_##T *this, int size); \
};	\
typedef struct vector2_##T vector2_##T;	\
	/* creates and initializes a new vector on the heap, returns a pointer to it. */\
vector2_##T* vector2_##T##_new();	\
	/* creates and initializes a new vector on the stack, returns it. */\
	/* also adds the vector to the "destroy warning" list */\
struct vector2_##T vector2_##T##_here();	\
	/* creates a new vector from the list, and returns it*/ \
struct vector2_##T *vector2_##T##_new_from_list(int num, ...); \
	/* same but on the stack*/ \
struct vector2_##T vector2_##T##_here_from_list(int num, ...); \
	/* Actual implementations of the above methods. Do not worry about the rest of these. */\
void vector2_##T##_constructor(struct vector2_##T *this);	\
void vector2_##T##_destructor(struct vector2_##T *this);	\
void vector2_##T##_clear(struct vector2_##T *this);	\
void vector2_##T##_internal_set(struct vector2_##T *this, T* element, int pos);	\
T* vector2_##T##_internal_get(struct vector2_##T *this, int pos);	\
void vector2_##T##_internal_resize(struct vector2_##T *this, int newsize);	\
void vector2_##T##_push_back(struct vector2_##T *this, T element);	\
void vector2_##T##_push_front(struct vector2_##T *this, T element);	\
T vector2_##T##_pop_back(struct vector2_##T *this);	\
T vector2_##T##_pop_front(struct vector2_##T *this);	\
void vector2_##T##_insert(struct vector2_##T *this, T element, int pos);	\
void vector2_##T##_erase(struct vector2_##T *this, int pos);	\
T vector2_##T##_get(struct vector2_##T *this, int pos);	\
void vector2_##T##_set(struct vector2_##T *this, T element, int pos);\
void vector2_##T##_resize(struct vector2_##T *this, int size); 

//put this in the .cpp file where you want the
//template instantiation's implementation to be.
#define v2print(...) fprintf(stderr, "vector2 error: " __VA_ARGS__)
#define v2flush()	 fflush(stderr)
#define v2crash()	 exit(*(int*)0)
#define v2error(...) v2print(__VA_ARGS__); v2flush(); v2crash();
#define implementation_vector_of(T)	\
const char *vector2_##T##_canary = "vector2_" #T;\
vector2_##T* vector2_##T##_new(){	\
	struct vector2_##T* V = malloc(sizeof(struct vector2_##T));	\
	vector2_##T##_constructor(V);	\
	return V;	\
}	\
	\
struct vector2_##T vector2_##T##_here(){	\
	struct vector2_##T V;	\
	vector2_##T##_constructor(&V);	\
	return V;	\
}	\
	\
struct vector2_##T *vector2_##T##_new_from_list(int num, ...){ \
	va_list args;\
	va_start(args, num);	\
	struct vector2_##T *pV = vector2_##T##_new();	\
	for(int I = 0; I < num; I++){	\
		T val = va_arg(args,T);			\
		vector2_##T##_push_back(pV, val);	\
	}								\
	return pV;						\
}	\
	\
struct vector2_##T vector2_##T##_here_from_list(int num, ...){ \
	va_list args;\
	va_start(args, num);	\
	struct vector2_##T V = vector2_##T##_here();	\
	for(int I = 0; I < num; I++){	\
		T val = va_arg(args,T);			\
		vector2_##T##_push_back(&V, val);	\
	}								\
	return V;						\
}	\
	\
	\
void vector2_##T##_constructor(struct vector2_##T *this){	\
	this->canary = vector2_##T##_canary;\
	this->size = 0;	\
	this->capacity = 0;	\
	this->data = 0;	\
	this->constructor = &vector2_##T##_constructor;	\
	this->destructor = &vector2_##T##_destructor;	\
	this->clear = &vector2_##T##_clear;	\
	this->internal_set = &vector2_##T##_internal_set;	\
	this->internal_get = &vector2_##T##_internal_get;	\
	this->internal_resize = &vector2_##T##_internal_resize;	\
	this->push_back = &vector2_##T##_push_back;	\
	this->push_front = &vector2_##T##_push_front;	\
	this->pop_back = &vector2_##T##_pop_back;	\
	this->pop_front = &vector2_##T##_pop_front;	\
	this->insert = &vector2_##T##_insert;	\
	this->erase = &vector2_##T##_erase;	\
	this->get = &vector2_##T##_get;	\
	this->set = &vector2_##T##_set;	\
	this->resize = &vector2_##T##_resize; \
}	\
	\
void vector2_##T##__basic_check(struct vector2_##T *this){	\
	if(!this){v2error("*this is null\n");}			\
	if(this->canary != vector2_##T##_canary)		\
	{v2error("vector2 not initalized (or this isn't it)\n");}	\
}	\
void vector2_##T##_destructor(struct vector2_##T *this){	\
	vector2_##T##__basic_check(this);	\
	free(this->data);	\
}	\
	\
void vector2_##T##_clear(struct vector2_##T *this){	\
	vector2_##T##__basic_check(this); \
	this->size = 0;	\
	vector2_##T##_internal_resize(this,1);	\
}	\
	\
void vector2_##T##_internal_set(struct vector2_##T *this, T* element, int pos){	\
	vector2_##T##__basic_check(this); \
	T* elem2 = vector2_##T##_internal_get(this, pos);	\
	*elem2 = *element;	\
}	\
	\
T* vector2_##T##_internal_get(struct vector2_##T *this, int pos){	\
	vector2_##T##__basic_check(this);  \
	return &(this->data[pos]);	\
}	\
	\
void vector2_##T##_internal_resize(struct vector2_##T *this, int newsize){	\
	vector2_##T##__basic_check(this);	\
	void *newdata = realloc(this->data, newsize*sizeof(T));	\
	if(newdata){	\
		this->data = newdata;	\
		this->capacity = newsize;	\
	}else{	\
		v2error("out of memory\n");	\
	}	\
}	\
	\
void vector2_##T##_push_back(struct vector2_##T *this, T element){	\
	vector2_##T##__basic_check(this); \
	if(this->size >= this->capacity){	\
		vector2_##T##_internal_resize(this, (this->capacity+1)*1.5);	\
	}	\
	vector2_##T##_internal_set(this, &element, this->size);	\
	this->size++;	\
}	\
	\
void vector2_##T##_push_front(struct vector2_##T *this, T element){	\
	vector2_##T##__basic_check(this); \
	vector2_##T##_insert(this, element, 0);	\
}	\
	\
T vector2_##T##_pop_back(struct vector2_##T *this){	\
	vector2_##T##__basic_check(this); \
	if(this->size == 0){v2error("trying to pop while empty\n");}	\
	T element = *vector2_##T##_internal_get(this, this->size-1);	\
	vector2_##T##_erase(this, this->size-1);	\
	return element;	\
}	\
	\
T vector2_##T##_pop_front(struct vector2_##T *this){	\
	vector2_##T##__basic_check(this); \
	if(this->size == 0){v2error("trying to pop while empty\n");}	\
	T element = *vector2_##T##_internal_get(this, 0);	\
	vector2_##T##_erase(this, 0);	\
	return element;		\
}	\
	\
void vector2_##T##_insert(struct vector2_##T *this, T element, int pos){	\
	vector2_##T##__basic_check(this); \
	if((pos < 0) || (pos > this->size)){v2error("index out of bounds (%d out of max %d)\n",pos, this->size);}	\
	if(this->size >= this->capacity){	\
		vector2_##T##_internal_resize(this, (this->capacity+1)*1.5);	\
	}	\
	if(this->size > 0){	\
	memmove(vector2_##T##_internal_get(this, pos+1), vector2_##T##_internal_get(this,pos), (this->size-pos)*sizeof(T));	\
	}	\
	vector2_##T##_internal_set(this, &element, pos);	\
	this->size++;	\
}	\
	\
void vector2_##T##_erase(struct vector2_##T *this, int pos){	\
	vector2_##T##__basic_check(this); \
	if((pos < 0) || (pos >= this->size)){v2error("index out of bounds (%d out of max %d)\n",pos, this->size);}	\
	if(pos < (this->size-1)){memmove(vector2_##T##_internal_get(this, pos), vector2_##T##_internal_get(this, pos+1), (this->size-pos)*sizeof(T));}	\
	this->size--;	\
}	\
	\
T vector2_##T##_get(struct vector2_##T *this, int pos){	\
	vector2_##T##__basic_check(this); \
	if((pos < 0) || (pos >= this->size)){v2error("index out of bounds (%d out of max %d)\n",pos, this->size);}	\
	return *vector2_##T##_internal_get(this, pos);	\
}	\
	\
void vector2_##T##_set(struct vector2_##T *this, T element, int pos){	\
	vector2_##T##__basic_check(this); \
	if((pos < 0) || (pos >= this->size)){v2error("index out of bounds (%d out of max %d)\n",pos, this->size);}	\
	vector2_##T##_internal_set(this, &element, pos);	\
} \
void vector2_##T##_resize(struct vector2_##T *this, int size){ \
	vector2_##T##__basic_check(this); \
	if(size > this->capacity){	\
		vector2_##T##_internal_resize(this, size); \
	} \
	this->size = size; \
}

/* Usage:
#include "vector2.h"

definition_vector_of(int);
implementation_vector_of(int);

int main(int argc, char **argv){
	struct vector2_int V;
	V = vector2_int_here();
	m(V,push_back,2);
	m(V,push_back,4);
	m(V,push_back,7);
	int i;
	for(i = 0; i < V.size; i++){
		printf("%d\n",m(V,get,i));
	}
}
*/

#endif