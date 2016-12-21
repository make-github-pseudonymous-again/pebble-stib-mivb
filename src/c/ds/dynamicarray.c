#include <pebble.h>
#include "ds/dynamicarray.h"

int ds_DynamicArray_init(ds_DynamicArray *a, size_t initialSize) {
	a->data = (void**)malloc(initialSize * sizeof(void*));
	if (a->data == NULL) return 1;

	a->length = 0;
	a->capacity = initialSize;
	
	return 0;
}

int ds_DynamicArray_push(ds_DynamicArray *a, void *element) {
	if (a->capacity == 0) {
		ds_DynamicArray_init(a, 1);
	}
	else if (a->length == a->capacity) {
		a->capacity <<= 1;
 		a->data = (void**)realloc(a->data, a->capacity * sizeof(void*));
		if (a->data == NULL) return 1;
	}
	a->data[a->length++] = element;
	return 0;
}

void ds_DynamicArray_free(ds_DynamicArray *a) {
  free(a->data);
  a->data = NULL;
  a->length = a->capacity = 0;
}