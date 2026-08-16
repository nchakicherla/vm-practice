#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct ArenaBlock {
	struct ArenaBlock *next;
	unsigned char *buffer;
	size_t capacity;
	size_t offset;
} ArenaBlock;

typedef struct Arena {
	ArenaBlock *head;
	ArenaBlock *current;
	size_t block_size; // default size for new blocks
} Arena;

void arena_init(Arena *arena, size_t capacity);
void arena_destroy(Arena *arena);
void *arena_alloc(Arena *arena, size_t size);
void *arena_zalloc(Arena *arena, size_t size);
void arena_reset(Arena *arena);
void *arena_realloc(Arena *arena, void *ptr, size_t old_size, size_t new_size); // only supports growth
void arena_info(const Arena *arena, size_t *remaining, size_t *block_size);
char *arena_strdup(Arena *arena, const char *s);

#endif // ARENA_H
