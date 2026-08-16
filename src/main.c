#include <stdio.h>
#include <assert.h>

#include "arena.h"

#define LARGE_NUM 10000

int main(void) {
	printf("hello, world\n");

	Arena arena;
	arena_init(&arena, 256);

	int *large_array = arena_alloc(&arena, LARGE_NUM * sizeof(int));

	for (int i = 0; i < LARGE_NUM; i++) {
		large_array[i] = i;
		assert(large_array[i] == i);
	}

	printf("successfully tested arena write and read\n");
	arena_destroy(&arena);

	return 0;
}