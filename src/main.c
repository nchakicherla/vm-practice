#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"

#include "arena.h"
#include "file.h"

int main(void) {

	// char *line;

	// while ((line = linenoise("> ")) != NULL) {
	// 	linenoiseHistoryAdd(line);

	// 	if (0 == strcmp(line, ".exit")) {
	// 		linenoiseFree(line);
	// 		break;
	// 	}

	// 	size_t line_len = strlen(line);

	// 	printf("you entered: %s\n", line);
	// 	printf("line_len: %zu\n", line_len);

	// 	linenoiseFree(line);
	// }

	Arena arena;
	arena_init(&arena, 256);

	const char *csv_path = "./resources/sample.csv";

	size_t csv_len;
	char *csv_buffer = file_read_all(&arena, csv_path, &csv_len);
	if (!csv_buffer) {
		fprintf(stderr, "failed to read csv: %s\n", csv_path);
	}

	if (csv_buffer) {
		printf("%s\n", csv_buffer);
	}

	arena_destroy(&arena);

	return 0;
}