#include <dirent.h>
#include <errno.h>
#include <linux/stat.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/bool.h"
#include "../include/builtins.h"
#include "../include/utils.h"

extern struct USER_INFO currentUser;

char *builtinsStr[]			  = {"cd",	  "ls",	   "pwd",	"help",	 "exit",
								 "mkdir", "clear", "trash", "whoami"};
int (*builtinsFnc[])(char **) = {indus_cd,	  indus_ls,	   indus_pwd,
								 indus_help,  indus_exit,  indus_mkdir,
								 indus_clear, indus_trash, indus_whoami};

int num_builtins() { return sizeof(builtinsStr) / sizeof(builtinsStr[0]); }

/* FIXME: Conditional jump or move depends on uninitialised value(s) */
int indus_ls(char **args) {

	bool argGiven;
	char *dir;
	DIR *d;
	struct dirent *entry;
	struct stat entryStat;

	if (args[1] == NULL) {
		dir = get_pwd();
		if (dir == NULL) {
			perror("get_pwd()");
			return 1;
		}
	} else {
		dir		 = args[1];
		argGiven = true;
	}

	d = opendir(dir);
	if (d == NULL) {
		perror("opendir()");
		return 1;
	}

	char **dirs	 = NULL;
	char **files = NULL;
	int dCount	 = 0;
	int fCount	 = 0;

	while ((entry = readdir(d)) != NULL) {
		char path[1024];
		snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

		if (stat(path, &entryStat) == -1) {
			perror("stat()");
			return 1;
		}

		if (S_ISDIR(entryStat.st_mode)) {
			dirs		 = realloc(dirs, sizeof(char *) * (dCount + 1));
			dirs[dCount] = strdup(entry->d_name);
			dCount++;
		} else {
			files		  = realloc(files, sizeof(char *) * (fCount + 1));
			files[fCount] = strdup(entry->d_name);
			fCount++;
		}
	}

	for (int i = 0; i < dCount; i++) {
		printf("\033[1;34m%s\033[0m\n", dirs[i]);
		free(dirs[i]);
	}

	free(dirs);

	for (int i = 0; i < fCount; i++) {
		printf("%s\n", files[i]);
		free(files[i]);
	}

	free(files);

	if (!argGiven)
		free(dir);

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
		dir = expand_tilde(args[1]);

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

/* CHECK: doesn't work on ~/abc? */
int indus_trash(char **args) {
	if (args[1] == NULL) {
		fputs("See `help trash` for usage information\n", stderr);
		return 1;
	}

	char *path = expand_tilde(args[1]);
	char trashDir[1024];
	char trashFilePath[1024];

	snprintf(trashDir, sizeof(trashDir), "%s/.trash", currentUser.home);

	struct stat st = {0};
	if (stat(trashDir, &st) == -1) {
		if (mkdir(trashDir, 0755) != 0) {
			perror("mkdir ~/.trash error");
			return 1;
		}
	}

	snprintf(trashFilePath, sizeof(trashFilePath), "%s/%s", trashDir, path);

	if (rename(path, trashFilePath) != 0) {
		switch (errno) {
		case ENOENT:
			fprintf(stderr,
					"trash: cannot remove '%s': No such file or directory\n",
					path);
			break;
		case EACCES:
			fprintf(stderr, "trash: cannot remove '%s': Permission denied\n",
					path);
			break;
		default:
			perror("indus_trash()");
			break;
		}
		return 1;
	}

	printf("Moved '%s' to trash\n", path);
	return 0;
}

int indus_help(char **args) {
	if (args[1] == NULL) {
		printf("Available commands:\n");
		for (int i = 0; i < num_builtins(); i++)
			printf("  %s\n", builtinsStr[i]);
	} else {
	}

	return 0;
}

// CHECK: exit some other way...?
int indus_exit(char **args) {
	int status = 0;

	if (args[1] != NULL)
		status = atoi(args[1]);

	exit(status);
}

int indus_mkdir(char **args) {
	if (args[1] == NULL) {
		fputs("See `help mkdir` for usage information\n", stderr);
		return 1;
	}

	char *dir = expand_tilde(args[1]);

	if (mkdir(dir, 0700) != 0) {
		perror("mkdir()");
		return 1;
	}

	return 0;
}

int indus_clear(char **args __attribute__((unused))) {
	printf("\033[H\033[J");
	return 0;
}

int indus_whoami(char **args __attribute__((unused))) {
	puts(currentUser.name);
	return 0;
}

int indus_pwd(char **args __attribute__((unused))) {
	char *pwd = get_pwd();

	if (pwd == NULL) {
		perror("get_pwd()");
		return 1;
	}

	puts(pwd);
	free(pwd);
	return 0;
}
