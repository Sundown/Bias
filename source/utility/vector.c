#include "../global.h"

void vector_new(um_Vector* a) {
	a->capacity = sizeof(a->static_data) / sizeof(a->static_data[0]);
	a->size = 0;
	a->data = a->static_data;
}

void vector_add(um_Vector* a, um_Noun item) {
	if (a->size + 1 > a->capacity) {
		a->capacity *= VECTOR_GROWTH_FACTOR;
		if (a->data == a->static_data) {
			a->data
			    = (um_Noun*)calloc(a->capacity, sizeof(um_Noun));
			memcpy(
			    a->data, a->static_data, a->size * sizeof(um_Noun));
		} else {
			a->data = (um_Noun*)realloc(
			    a->data, a->capacity * sizeof(um_Noun));
		}
	}

	a->data[a->size] = item;
	a->size++;
}

void vector_clear(um_Vector* a) {
	a->size = 0;
}

void vector_free(um_Vector* a) {
	if (a->data != a->static_data) free(a->data);
}

void noun_to_vector(um_Noun a, um_Vector* v) {
	vector_new(v);
	for (; !isnil(a); a = cdr(a)) { vector_add(v, car(a)); }
}

um_Noun vector_to_noun(um_Vector* a, size_t start) {
	um_Noun r = nil;
	size_t i;
	for (i = start; i < a->size; i++) {
		if (!isnil(a->data[i])) { r = cons(a->data[i], r); }
	}

	return reverse_list(r);
}
