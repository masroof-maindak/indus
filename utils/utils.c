#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *copy_string(char *str) {
	if (str == NULL) {
		fputs("copy_string receieved null input string\n", stderr);
		return NULL;
	}

	char *copy = malloc(strlen(str) + 1);
	if (copy == NULL) {
		perror("copy_string malloc() error");
		return NULL;
	}

	strcpy(copy, str);
	return copy;
}
