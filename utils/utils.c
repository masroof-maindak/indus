#include "utils.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

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

char *get_username() {
	setpwent();
	struct passwd *pw = getpwuid(geteuid());

	if (pw)
		return pw->pw_name;

	endpwent();
	return NULL;
}

char *get_line() {
	char *input;
	input = readline(NULL);
	add_history(input);
	return input;
}
