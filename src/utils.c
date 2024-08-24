#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/utils.h"

extern struct USER_INFO currentUser;

/* CHECK: Valgrind Errors */
char *expand_tilde(char *path) {
	if (path == NULL || path[0] != '~')
        return path;

    char *home = currentUser.home;
    size_t homeLen = strlen(home);
    size_t pathLen = strlen(path);
	char *newPath = realloc(path, homeLen + (pathLen - 1) + 1);

    if (newPath == NULL) {
        perror("expand_tilde malloc()");
        return NULL;
    }

	memmove(newPath + homeLen, newPath + 1, pathLen);
	memcpy(newPath, home, homeLen);

    return newPath;
}

char *get_pwd() {
	long int path_max;
	size_t size;
	char *buf;
	char *ptr;

	path_max = pathconf(".", _PC_PATH_MAX);
	if (path_max == -1)
		size = 1024;
	else if (path_max > 10240)
		size = 10240;
	else
		size = path_max;

	for (buf = ptr = NULL; ptr == NULL; size *= 2) {
		if ((buf = realloc(buf, size)) == NULL) {
			perror("realloc()");
			return NULL;
		}

		ptr = getcwd(buf, size);
		if (ptr == NULL && errno != ERANGE) {
			perror("getcwd()");
			return NULL;
		}
	}

	return ptr;
}

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

void init_user_info(struct USER_INFO *curr_user) {
	setpwent();

	struct passwd *entry;
	entry = getpwuid(getuid());

	if (entry) {
		curr_user->uid	= entry->pw_uid;
		curr_user->name = entry->pw_name;
		curr_user->home = entry->pw_dir;
	} else {
		curr_user->uid	= -1;
		curr_user->name = "ERROR_USER";
		curr_user->home = "/";
	}

	endpwent();
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
