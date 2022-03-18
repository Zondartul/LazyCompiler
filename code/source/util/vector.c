#include "vector.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "globals.h"

void vector_constructor(vector *this, int element_size){
	//printf("vector_constructor\n");
	if(element_size == 0){error("vector error: can't have 0 element_size\n");}
	this->data = 0;
	this->size = 0;
	this->reserved = 0;
	this->element_size = element_size;
	printf("vector constructed: %p\n",this);
	vector_resize(this,1);
	//elem_constructor = element_constructor;
	//elem_destructor = element_destructor;
}

void vector_destructor(vector *this, funcptr elem_destructor){
	if(elem_destructor){
		int i;
		for(i = 0; i < this->size; i++){
			elem_destructor(((size_t)this->data)+i*this->element_size);
		}
	}
	free(this->data);
}

//internal function to calculate pointers
void *vector_get_reference(vector *this, int pos){
	//printf("vector_get_reference %d\n",pos);
	if((pos < 0) || (pos >= this->size)){error("vector error: index out of bounds (%d/%d)\n",pos,this->size);}
	return ((size_t)this->data)+pos*this->element_size;
}
void vector_push_back(vector *this, void *newelem){
	//printf("vector_push_back %d\n",this->size);
	if(this->size >= this->reserved){
		vector_resize(this, (this->reserved+1)*1.5);
	}
	this->size++;
	if(newelem){
		vector_set(this,this->size-1,newelem);
	}
}

void *vector_pop_back(vector *this){
	//printf("vector_pop_back %d\n",this->size);
	void *result = vector_get_copy(this, this->size-1);
	this->size--;
	return result;
}

// 0 1 2 3 4	 ->	0 1 2 3 4 5
//           5 (push)
//
// 0 1 2 3 4	 -> 0 1 2 3 3 4 
//	     3  
void vector_insert(vector *this, void *newelem, int pos){
	//printf("vector_insert %d\n",pos);
	if((pos < 0) || (pos > this->size)){error("vector error: index out of bounds (%d/%d)\n",pos,this->size);}
	if(pos < this->size){
		vector_push_back(this, 0);
		//printf("vector_insert memmove");
		memmove(vector_get_reference(this, pos+1), vector_get_reference(this, pos), (this->size-pos-1)*this->element_size);
	}
	vector_set(this, pos, newelem);
}

// 0 1 2 3 4	->	0 1 2 3
//	       4
//
// 0 1 2 3 4	->	0 1 3 4
// 	   2
void *vector_remove(vector *this, int pos){
	//printf("vector_remove(%d) started with %d\n",pos,this->size);
	if((pos < 0) || (pos >= this->size)){error("vector error: index out of bounds (%d/%d)\n",pos,this->size);}
	void *result = vector_get_copy(this, pos);
	if(pos+1 < this->size){
		printf("vector_remove memmove\n");
		memmove(vector_get_reference(this, pos), vector_get_reference(this, pos+1), (this->size-pos)*this->element_size);
	}
	vector_pop_back(this);
	//printf("vector_remove(%d) ended with %d\n",pos,this->size);
	return result;
}

void *vector_get_copy(vector *this, int pos){
	//printf("vector_get_copy %d from %p\n",pos,this);
	if((pos < 0) || (pos >= this->size)){error("vector error: index out of bounds (%d/%d)\n",pos,this->size);}
	void *buff = malloc(this->element_size);
	if(!buff){error("vector error: out of memory\n");}
	memcpy(buff, vector_get_reference(this, pos), this->element_size);
	return buff;
}

void vector_set(vector *this, int pos, void *newelem){
	//printf("vector_set %d\n",pos);
	if((pos < 0) || (pos > this->size)){error("vector error: index out of bounds (%d/%d)\n",pos,this->size);}
	if(pos < this->size){
		memcpy(vector_get_reference(this, pos), newelem, this->element_size);
	}else{
		vector_push_back(this, newelem);
	}
}
void vector_resize(vector *this, int newsize){
	//printf("vector_resize %d\n",newsize);
	//printf("resizing vector %p (d = %p, s = %d, ns = %d, es = %d)\n",this, this->data, this->size, newsize, this->element_size);
	int wantedsize = this->element_size*newsize;
	void *newdata = realloc(this->data, wantedsize);
	if(newdata){
		//printf("resized, newd = %p\n",newdata);
		this->data = newdata;
		this->reserved = newsize;
	}else{
		err("vector error: out of memory (ns:%d, os:%d, es:%d, d:%p)\n",newsize, this->size, this->element_size,this->data); 
		void *test = malloc(wantedsize);
		if(test){error("(malloc(%d) works though)\n",wantedsize);}
		else{error("(no malloc(%d) either)\n",wantedsize);}
	}
}