#include "arena.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_ALIGNMENT _Alignof(max_align_t)
#define ARENA_HOG_FACTOR 4

static ArenaBlock *arena_block_create(size_t capacity) {
	ArenaBlock *block = malloc(sizeof(ArenaBlock));
	if (!block) {
		fprintf(stderr, "arena: out of memory\n");
		abort();
	}

	block->buffer = malloc(capacity);
	if (!block->buffer) {
		fprintf(stderr, "arena: out of memory\n");
		abort();
	}

	block->next = NULL;
	block->offset = 0;
	block->capacity = capacity;

	return block;
}

void arena_init(Arena *arena, size_t capacity) {
	ArenaBlock *block = arena_block_create(capacity);

	arena->head = block;
	arena->current = block;
	arena->block_size = capacity;
}

void arena_destroy(Arena *arena) {
	ArenaBlock *block = arena->head;

	while (block) {
		ArenaBlock *next = block->next;
		free(block->buffer);
		free(block);
		block = next;
	}

	arena->head = NULL;
	arena->current = NULL;
	arena->block_size = 0;
}

static size_t align_up(size_t n, size_t alignment) {
	return (n + (alignment - 1)) & ~(alignment - 1);
}

void *arena_alloc(Arena *arena, size_t size) {
	ArenaBlock *block = arena->current;
	size_t aligned_offset = align_up(block->offset, ARENA_ALIGNMENT);
	size_t remaining = aligned_offset <= block->capacity ? block->capacity - aligned_offset : 0;

	if (size > remaining) {
		if (block->next && block->next->capacity >= size) {
			block = block->next;
		} else {
			size_t block_capacity = size > arena->block_size ? size : arena->block_size;
			size_t new_capacity = block_capacity * ARENA_HOG_FACTOR;
			ArenaBlock *new_block = arena_block_create(new_capacity);

			if (size <= arena->block_size) {
				arena->block_size = new_capacity;
			}

			new_block->next = block->next;
			block->next = new_block;
			block = new_block;
		}

		arena->current = block;
		aligned_offset = 0;
	}

	void *ptr = block->buffer + aligned_offset;
	block->offset = aligned_offset + size;

	return ptr;
}

void *arena_zalloc(Arena *arena, size_t size) {
	void *ptr = arena_alloc(arena, size);
	memset(ptr, 0, size);
	return ptr;
}

void *arena_realloc(Arena *arena, void *ptr, size_t old_size, size_t new_size) {
	if (new_size < old_size) {
		fprintf(stderr, "arena: arena_realloc does not support shrinking\n");
		abort();
	}

	void *new_ptr = arena_alloc(arena, new_size);
	if (old_size > 0) {
		memcpy(new_ptr, ptr, old_size);
	}
	return new_ptr;
}

void arena_reset(Arena *arena) {
	ArenaBlock *block = arena->head;

	while (block) {
		block->offset = 0;
		block = block->next;
	}

	arena->current = arena->head;
}

void arena_info(const Arena *arena, size_t *remaining, size_t *block_size) {
	if (remaining) {
		size_t aligned_offset = align_up(arena->current->offset, ARENA_ALIGNMENT);
		*remaining = aligned_offset >= arena->current->capacity
			? 0
			: arena->current->capacity - aligned_offset;
	}

	if (block_size) {
		*block_size = arena->block_size;
	}
}

char *arena_strdup(Arena *arena, const char *s) {
	size_t len = strlen(s) + 1; /* +1 for the NUL terminator */
	char *copy = arena_alloc(arena, len);
	memcpy(copy, s, len);
	return copy;
}
