#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/builtins.h"
#include "../include/prompt.h"
#include "../include/utils.h"

extern struct USER_INFO currUser;

char *builtinsStr[]			  = {"cd", "help", "exit", "trash", "whoami"};
int (*builtinsFnc[])(char **) = {indus_cd, indus_help, indus_exit, indus_trash,
								 indus_whoami};

int num_builtins() { return sizeof(builtinsStr) / sizeof(builtinsStr[0]); }

int indus_cd(char **args) {
	char *dir;

	if (args[1] == NULL)
		dir = copy_string(currUser.home);
	else
		dir = expand_tilde(args[1]);

	if (dir == NULL)
		return 1;

	if (chdir(dir) != 0) {
		switch (errno) {
		case ENOENT:
			fprintf(stderr, "No such directory\n");
			break;
		case ENOTDIR:
			fprintf(stderr, "Not a directory\n");
			break;
		case EACCES:
			fprintf(stderr, "Permission denied\n");
			break;
		default:
			perror("chdir()");
			break;
		}
		return 1;
	}

	free(dir);
	return 0;
}

int indus_trash(char **args) {
	if (args[1] == NULL) {
		fprintf(stderr, "indus_trash(): Missing operand\n");
		return 1;
	}

	for (int i = 1; args[i] != NULL; i++) {

		int n;
		char trashPath[PATH_MAX];
		char *fileName	= expand_tilde(args[i]);
		char *printName = fileName;

		if (fileName == NULL)
			return 1;

		if (args[i][0] == '~')
			printName = args[i] + 2;

		n = snprintf(trashPath, PATH_MAX, "%s/%s", currUser.trashDir,
					 printName);
		if (n < 0) {
			perror("snprintf()");
			free(fileName);
			return 1;
		}

		if (rename(fileName, trashPath) == -1) {
			if (errno == EXDEV)
				/* TODO(?): Copy files over manually */
				/* https://stackoverflow.com/a/17440097 */
				fprintf(stderr, "File to trash is on a different FS!\n");
			else
				perror("indus_trash()");

			free(fileName);
			return 1;
		}

		free(fileName);
	}

	return 0;
}

int indus_help(char **args) {
	if (args[1] != NULL) {
		puts("Invalid input format!");
		return 0;
	}

	printf(BOLD_MAGENTA "Available commands:\n" RESET GREEN);
	for (int i = 0; i < num_builtins(); i++)
		puts(builtinsStr[i]);
	printf(RESET);

	return 0;
}

int indus_exit(char **args) {
	int status = 0;

	if (args[1] != NULL)
		status = atoi(args[1]);

	exit(status);
}

int indus_whoami(char **args __attribute__((unused))) {
	puts(currUser.name);
	return 0;
}
