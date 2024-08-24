#include "builtins.h"
#include "../utils/utils.h"

#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern struct USER_INFO currentUser;

char *builtins_str[]		  = {"cd", "ls", "rm", "help", "exit"};
int (*builtins_fn[])(char **) = {indus_cd, indus_ls, indus_rm, indus_help,
								 indus_exit};

int num_builtins() { return sizeof(builtins_str) / sizeof(builtins_str[0]); }

int indus_ls(char **args) {

	char *dir;
	DIR *d;
	struct dirent *entry;

	if (args[1] == NULL) {
		dir = get_pwd();
		if (dir == NULL) {
			perror("get_pwd()");
			return 1;
		}
	} else {
		dir = args[1];
	}

	d = opendir(dir);
	if (d == NULL) {
		perror("opendir()");
		return 1;
	}

	while ((entry = readdir(d)) != NULL) {
		// TODO: bold/coloured for directories
		printf("%s\n", entry->d_name);
	}

	if (closedir(d) == -1) {
		perror("closedir()");
		return 1;
	}

	return 0;
}

int indus_cd(char **args) {
	char *dir;

	if (args[1] == NULL)
		dir = currentUser.home;
	else
		dir = args[1];

	if (chdir(dir) != 0) {
		switch (errno) {
		case ENOENT:
			fputs("No such directory\n", stderr);
			break;
		case ENOTDIR:
			fputs("Not a directory\n", stderr);
			break;
		case EACCES:
			fputs("Permission denied\n", stderr);
			break;
		default:
			perror("chdir()");
			break;
		}
		return 1;
	}

	return 0;
}

int indus_rm(char **args) {
	if (args[1] == NULL) {
		fputs("See `help rm` for usage information\n", stderr);
		return 1;
	}

	// Move the file being deleted to ~/.trash

	return 0;
}

int indus_help(char **args) {

	if (args[1] == NULL) {
		printf("Available commands:\n");
		for (int i = 0; i < num_builtins(); i++)
			printf("  %s\n", builtins_str[i]);
	} else {

	}

	return 0;
}

int indus_exit(char **args) { return 0; }
