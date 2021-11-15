#include "../global.h"

um_Vector* vector_new(void) {
	um_Vector* v = calloc(1, sizeof(um_Vector));
	v->capacity = sizeof(v->static_data) / sizeof(v->static_data[0]);
	v->size = 0;
	v->data = v->static_data;

	return v;
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

um_Vector* noun_to_vector(um_Noun a) {
	um_Vector* v = vector_new();
	for (; !isnil(a); a = cdr(a)) { vector_add(v, car(a)); }

	return v;
}

um_Noun vector_to_noun(um_Vector* a, size_t start) {
	um_Noun r = nil;
	size_t i;
	for (i = start; i < a->size; i++) {
		if (!isnil(a->data[i])) { r = cons(a->data[i], r); }
	}

	return reverse_list(r);
}
