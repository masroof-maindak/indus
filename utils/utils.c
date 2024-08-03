#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *copy_string(char *str) {
	if (str == NULL) {
		// fprintf(stderr, "Input string is null...\n");
		fprintf(stderr, "%s\n", "Input string is null...");
		// CHECK: Any reason to go for one of the aforementioned over the other?
		return NULL;
	}
    char *copy = malloc(strlen(str) + 1);
    if (copy == NULL) {
        perror("copy malloc() error");
        return NULL;
    }
    strcpy(copy, str);
    return copy;
}
