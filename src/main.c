#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"
#include "csv.h"

#include "arena.h"
#include "file.h"

static void cb1(void *field, size_t len, void *data) {
	(void)data;
	printf("field: %.*s\n", (int)len, (char *)field);
}

static void cb2(int c, void *data) {
	(void)c;
	(void)data;
	printf("row completed\n");
}

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

	const char *csv_path = "./resources/sample_messy.csv";

	size_t csv_len;
	char *csv_buffer = file_read_all(&arena, csv_path, &csv_len);
	if (!csv_buffer) {
		fprintf(stderr, "failed to read csv: %s\n", csv_path);
		arena_destroy(&arena);
		return 1;
	}

	if (csv_buffer) {
		printf("%s\n", csv_buffer);
	}

	struct csv_parser parser;

	if (csv_init(&parser, 0) != 0) {
		fprintf(stderr, "failed to parse csv: %s\n", csv_path);
		arena_destroy(&arena);
		return 2;
	}

	size_t parsed = csv_parse(&parser, csv_buffer, csv_len, cb1, cb2, NULL);
	if (parsed != csv_len) {
		fprintf(stderr, "csv parse error: (message) %s\n", csv_strerror(csv_error(&parser)));
		csv_free(&parser);
		arena_destroy(&arena);
		return 3;
	}

	if (csv_fini(&parser, cb1, cb2, NULL) != 0) {
		fprintf(stderr, "csv parse error: (message ) %s\n", csv_strerror(csv_error(&parser)));
		csv_free(&parser);
		arena_destroy(&arena);
		return 4;
	}

	csv_free(&parser);
	arena_destroy(&arena);

	return 0;
}