#include "utils.h"

#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

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
	endpwent();

	if (pw)
		return pw->pw_name;

	return NULL;
}

char **parse_input(char *input) {
	if (input == NULL || input[0] == '\0')
		return NULL;

	char delim[] = " \t\r\n\a";
	char *save	 = input;
	int numArgs	 = 8;
	int counter	 = 0;
	char *arg	 = NULL;
	char **args	 = malloc(numArgs * sizeof(char *));

	if (args == NULL) {
		perror("malloc()");
		return NULL;
	}

	if ((arg = strtok(input, delim)) != NULL) {
		do {
			args[counter++] = arg;

			if (counter > numArgs) {
				numArgs *= 2;
				args = realloc(args, numArgs);
				if (args == NULL) {
					perror("realloc()");
					return NULL;
				}
			}

		} while ((arg = strtok(NULL, delim)) != NULL);
	}

	args[counter] = NULL;
	input		  = save;

	return args;
}
