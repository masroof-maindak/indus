#include <errno.h>
#include <linux/limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/utils.h"

extern struct USER_INFO currentUser;

void ensure_trash_dir_exists() {
	char *trashDirName	 = "/.trash\0";
	size_t homeSize		 = strlen(currentUser.home);
	size_t size			 = homeSize + 7;
	currentUser.trashDir = malloc(size + 1);

	if (currentUser.trashDir == NULL) {
		perror("malloc()");
		return;
	}

	memcpy(currentUser.trashDir, currentUser.home, homeSize);
	memcpy(currentUser.trashDir + homeSize, trashDirName, 8);

	struct stat st = {0};
	if (stat(currentUser.trashDir, &st) == -1) {
		if (mkdir(currentUser.trashDir, 0700) == -1) {
			perror("mkdir()");
			return;
		}
	}
}

char *expand_tilde(char *path) {
	if (path == NULL || path[0] != '~')
		return copy_string(path);

	size_t homeLen = strlen(currentUser.home);
	size_t pathLen = strlen(path);
	char *newPath  = malloc(sizeof(char) * (homeLen + pathLen));

	if (newPath == NULL) {
		perror("malloc()");
		return NULL;
	}

	memcpy(newPath, currentUser.home, homeLen);
	memcpy(newPath + homeLen, path + 1, pathLen - 1);
	newPath[homeLen + pathLen - 1] = '\0';

	return newPath;
}

char *get_pwd() {
	size_t size = PATH_MAXL;
	char *buf	= NULL;
	char *ptr	= NULL;
	char *real	= NULL;

	for (; ptr == NULL; size *= 2) {
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

	for (; real == NULL; size *= 2) {
		if ((ptr = realloc(ptr, size)) == NULL) {
			perror("realloc()");
			return NULL;
		}

		real = realpath(ptr, real);
		if (real == NULL && errno != ENAMETOOLONG) {
			perror("realpath()");
			return ptr;
		}
	}

	free(ptr);
	return real;
}

char *copy_string(const char *str) {
	if (str == NULL) {
		fprintf(stderr, "copy_string receieved NULL!\n");
		return NULL;
	}

	size_t size = strlen(str);
	char *copy	= malloc(size + 1);
	if (copy == NULL) {
		perror("malloc()");
		return NULL;
	}

	memcpy(copy, str, size);
	copy[size] = '\0';
	return copy;
}

void init_user_info(struct USER_INFO *curr_user) {
	struct passwd *pws;
	pws = getpwuid(getuid());

	if (pws) {
		curr_user->uid	= pws->pw_uid;
		curr_user->name = pws->pw_name;
		curr_user->home = pws->pw_dir;
	} else {
		curr_user->uid	= -1;
		curr_user->name = "ERROR_USER";
		curr_user->home = "/";
	}
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
