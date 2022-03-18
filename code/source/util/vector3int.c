#include "vector3int.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

const char *vector3_int_canary = "vector3_int";
vector3_int* vector3_int_new(){
	struct vector3_int* V = malloc(sizeof(struct vector3_int));
	vector3_int_constructor(V);
	return V;
}	
	
struct vector3_int vector3_int_here(){	
	struct vector3_int V;	
	vector3_int_constructor(&V);	
	return V;	
}	
	
void vector3_int_constructor(struct vector3_int *this){
	this->canary = vector3_int_canary;
	this->size = 0;	
	this->capacity = 0;	
	this->data = 0;	
	this->constructor = &vector3_int_constructor;	
	this->destructor = &vector3_int_destructor;	
	this->clear = &vector3_int_clear;	
	this->internal_set = &vector3_int_internal_set;	
	this->internal_get = &vector3_int_internal_get;	
	this->internal_resize = &vector3_int_internal_resize;	
	this->push_back = &vector3_int_push_back;	
	this->push_front = &vector3_int_push_front;	
	this->pop_back = &vector3_int_pop_back;	
	this->pop_front = &vector3_int_pop_front;	
	this->insert = &vector3_int_insert;	
	this->erase = &vector3_int_erase;	
	this->get = &vector3_int_get;	
	this->set = &vector3_int_set;	
}	
	
void vector3_basic_check(struct vector3_int *this){
	if(!this){v3error("*this is null\n");}	
	if(this->canary != vector3_int_canary)
	{v3error("vector3 not initalized (or this isn't it)\n");}
}

void vector3_int_destructor(struct vector3_int *this){
	vector3_basic_check(this);
	free(this->data);
}

void vector3_int_clear(struct vector3_int *this){
	vector3_basic_check(this);
	this->size = 0;
	vector3_int_internal_resize(this,1);
}

void vector3_int_internal_set(struct vector3_int *this, T* element, int pos){
	vector3_basic_check(this);
	T* elem2 = vector3_int_internal_get(this, pos);
	*elem2 = *element;
}

T* vector3_int_internal_get(struct vector3_int *this, int pos){
	vector3_basic_check(this); 
	return &(this->data[pos]);
}

void vector3_int_internal_resize(struct vector3_int *this, int newsize){
	vector3_basic_check(this);
	void *newdata = realloc(this->data, newsize*sizeof(T));
	if(newdata){
		this->data = newdata;
		this->capacity = newsize;
	}else{
		v3error("out of memory\n");
	}
}

void vector3_int_push_back(struct vector3_int *this, T element){
	vector3_basic_check(this);
	if(this->size >= this->capacity){
		vector3_int_internal_resize(this, (this->capacity+1)*1.5);
	}
	vector3_int_internal_set(this, &element, this->size);
	this->size++;
}

void vector3_int_push_front(struct vector3_int *this, T element){
	vector3_basic_check(this);
	vector3_int_insert(this, element, 0);
}

T vector3_int_pop_back(struct vector3_int *this){
	vector3_basic_check(this);
	if(this->size == 0){v3error("trying to pop while empty\n");}
	T element = *vector3_int_internal_get(this, this->size-1);
	vector3_int_erase(this, this->size-1);
	return element;
}

T vector3_int_pop_front(struct vector3_int *this){
	vector3_basic_check(this);
	if(this->size == 0){v3error("trying to pop while empty\n");}
	T element = *vector3_int_internal_get(this, 0);
	vector3_int_erase(this, 0);
	return element;	
}

void vector3_int_insert(struct vector3_int *this, T element, int pos){
	vector3_basic_check(this);
	if((pos < 0) || (pos > this->size)){v3error("index out of bounds (%d out of max %d)\n",pos, this->size);}
	if(this->size >= this->capacity){
		vector3_int_internal_resize(this, (this->capacity+1)*1.5);
	}
	if(this->size > 0){
	memmove(vector3_int_internal_get(this, pos+1), vector3_int_internal_get(this,pos), (this->size-pos)*sizeof(T));
	}
	vector3_int_internal_set(this, &element, pos);
	this->size++;
}

void vector3_int_erase(struct vector3_int *this, int pos){
	vector3_basic_check(this);
	if((pos < 0) || (pos >= this->size)){v3error("index out of bounds (%d out of max %d)\n",pos, this->size);}
	if(pos < (this->size-1)){memmove(vector3_int_internal_get(this, pos), vector3_int_internal_get(this, pos+1), (this->size-pos)*sizeof(T));}
	this->size--;
}	

T vector3_int_get(struct vector3_int *this, int pos){
	vector3_basic_check(this);
	if((pos < 0) || (pos >= this->size)){v3error("index out of bounds (%d out of max %d)\n",pos, this->size);}
	return *vector3_int_internal_get(this, pos);
}	
	
void vector3_int_set(struct vector3_int *this, T element, int pos){
	vector3_basic_check(this);
	if((pos < 0) || (pos >= this->size)){v3error("index out of bounds (%d out of max %d)\n",pos, this->size);}
	vector3_int_internal_set(this, &element, pos);
}