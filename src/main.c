#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"

#include "arena.h"

#define LARGE_NUM 10000

int main(void) {

	char *line;

	while ((line = linenoise("> ")) != NULL) {
		linenoiseHistoryAdd(line);

		if (0 == strcmp(line, ".exit")) {
			linenoiseFree(line);
			break;
		}

		size_t line_len = strlen(line);

		printf("you entered: %s\n", line);
		printf("line_len: %zu\n", line_len);

		linenoiseFree(line);
	}

	return 0;
}