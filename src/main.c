#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "linenoise.h"
#include "csv.h"

#include "arena.h"
#include "file.h"

typedef struct FieldRowCounter {
	size_t num_cols;
	size_t num_rows;
	size_t num_cols_header;
	size_t num_cols_first_row; // only used if header row doesn't exist
	bool header_row_exists;
	bool error_encountered;
	size_t total_col_length[64];
} FieldRowCounter;

static void cb1(void *field, size_t len, void *data) {
	FieldRowCounter *counter = (FieldRowCounter *)data;
	if (counter->header_row_exists && counter->num_rows == 0) {
		counter->num_cols_header++;
		counter->num_cols_first_row++;
	}
	if (!counter->header_row_exists && counter->num_rows == 0) {
		counter->num_cols_first_row++;
	}
	if (counter->header_row_exists && counter->num_rows > 0) {
		counter->total_col_length[counter->num_cols] += len + 1;
	}
	if (!counter->header_row_exists) {
		counter->total_col_length[counter->num_cols] += len + 1;		
	}
	counter->num_cols++;
	printf("field: %.*s\n", (int)len, (char *)field);
}

static void cb2(int c, void *data) {
	(void)c;
	
	FieldRowCounter *counter = (FieldRowCounter *)data;
	counter->num_rows++;

	if (counter->header_row_exists && counter->num_cols_header != counter->num_cols) {
		counter->error_encountered = true;
	}
	counter->num_cols = 0;

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

	const char *csv_path = "./resources/sample.csv";

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

	FieldRowCounter counter = {0};
	counter.header_row_exists = true;

	size_t parsed = csv_parse(&parser, csv_buffer, csv_len, cb1, cb2, &counter);
	if (parsed != csv_len) {
		fprintf(stderr, "csv parse error: (message) %s\n", csv_strerror(csv_error(&parser)));
		csv_free(&parser);
		arena_destroy(&arena);
		return 3;
	}

	if (csv_fini(&parser, cb1, cb2, &counter) != 0) {
		fprintf(stderr, "csv parse error: (message ) %s\n", csv_strerror(csv_error(&parser)));
		csv_free(&parser);
		arena_destroy(&arena);
		return 4;
	}

	printf("counter.num_rows: %zu\n", counter.num_rows);
	printf("counter.num_cols_header: %zu\n", counter.num_cols_header);
	printf("counter.num_cols: %zu\n", counter.num_cols);
	printf("counter.error_encountered: %d\n", counter.error_encountered);

	for (size_t i = 0; i < counter.num_cols_header; i++) {
		printf("counter.total_col_length[%zu]: %zu\n", i, counter.total_col_length[i]);
	}

	csv_free(&parser);
	arena_destroy(&arena);

	return 0;
}