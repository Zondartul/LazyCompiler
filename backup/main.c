#include "vector2.h"

definition_vector_of(int);
implementation_vector_of(int);
#define { {printf("oh hi\n");
#define } printf("ok bye\n");

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