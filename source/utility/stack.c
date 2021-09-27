#include "../global.h"

void stack_restore_add(int saved_size, um_Noun a) {
	stack_size = saved_size;
	if (stack_size < stack_capacity / 4) {
		stack_capacity = stack_size * 2;
		stack = (um_Noun*)realloc(stack,
					  stack_capacity * sizeof(um_Noun));
	}

	stack_add(a);
	garbage_collector_consider();
}

void stack_restore(int saved_size) {
	stack_size = saved_size;
	if (stack_size < stack_capacity / 4) {
		stack_capacity = stack_size * 2;
		stack = (um_Noun*)realloc(stack,
					  stack_capacity * sizeof(um_Noun));
	}

	garbage_collector_consider();
}
