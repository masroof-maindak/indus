#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/utils.h"

extern struct USER_INFO currUser;

void ensure_trash_dir_exists() {
	char *trashDirName = "/.trash\0";
	size_t homeSize	   = strlen(currUser.home);
	size_t size		   = homeSize + 7;
	currUser.trashDir  = malloc(size + 1);

	if (currUser.trashDir == NULL) {
		perror("malloc()");
		return;
	}

	memcpy(currUser.trashDir, currUser.home, homeSize);
	memcpy(currUser.trashDir + homeSize, trashDirName, 8);

	struct stat st = {0};
	if (stat(currUser.trashDir, &st) == -1) {
		if (mkdir(currUser.trashDir, 0700) == -1) {
			perror("mkdir()");
			return;
		}
	}
}

/**
 * @detail Expands the tilde in the path to the home directory of the current
 * user. Must be freed by the caller.
 */
char *expand_tilde(char *path) {
	if (path != NULL && path[0] != '~')
		return copy_string(path);

	size_t homeLen = strlen(currUser.home);
	size_t pathLen = strlen(path);
	char *newPath  = malloc(sizeof(char) * (homeLen + pathLen));

	if (newPath == NULL) {
		perror("malloc()");
		return NULL;
	}

	memcpy(newPath, currUser.home, homeLen);
	memcpy(newPath + homeLen, path + 1, pathLen - 1);
	newPath[homeLen + pathLen - 1] = '\0';

	return newPath;
}

char *get_pwd() {
	char *buf, *tmp, *real;

	buf = malloc(PATH_MAX);
	if (buf == NULL) {
		perror("malloc() in get_pwd() - buf");
		return NULL;
	}

	tmp = getcwd(buf, PATH_MAX);
	if (tmp == NULL) {
		perror("getcwd()");
		free(buf);
		return NULL;
	}
	tmp = buf;

	real = malloc(PATH_MAX);
	if (real == NULL) {
		perror("malloc() in get_pwd() - real");
		return NULL;
	}

	/* FIXME */
	real = realpath(buf, real);
	if (real == NULL) {
		perror("realpath()");
		return buf;
	}

	free(buf);
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
		perror("malloc() in copy_string()");
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

char **tokenise_input(char *input) {
	if (input == NULL || input[0] == '\0')
		return NULL;

	const char delim[] = " \t\r\n\a";
	int numArgs		   = 8;
	int counter		   = 0;

	char *arg = NULL, *tmp = NULL, *save = input;
	char **args = malloc(numArgs * sizeof(char *));

	if (args == NULL) {
		perror("malloc() in parse_input()");
		return NULL;
	}

	if ((arg = strtok(input, delim)) != NULL) {
		do {
			args[counter++] = arg;

			if (counter > numArgs) {
				numArgs *= 2;
				tmp = realloc(args, numArgs);
				if (tmp == NULL) {
					perror("realloc() in parse_input");
					free(args);
					return NULL;
				}
			}

		} while ((arg = strtok(NULL, delim)) != NULL);
	}

	args[counter] = NULL;
	input		  = save;

	return args;
}
