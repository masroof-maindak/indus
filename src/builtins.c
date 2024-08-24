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

char *builtins_str[]		  = {"cd",	 "ls",	  "trash", "pwd",	"help",
								 "exit", "mkdir", "clear", "whoami"};
int (*builtins_fn[])(char **) = {indus_cd,	  indus_ls,	   indus_pwd,
								 indus_help,  indus_exit,  indus_mkdir,
								 indus_clear, indus_trash, indus_whoami};

int num_builtins() { return sizeof(builtins_str) / sizeof(builtins_str[0]); }

/* FIXME: Conditional jump or move depends on uninitialised value(s) */
int indus_ls(char **args) {

	bool argGiven;
	char *dir;
	DIR *d;
	struct dirent *entry;
	struct stat entry_stat;

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

	char **dirs		  = NULL;
	char **files	  = NULL;
	size_t dir_count  = 0;
	size_t file_count = 0;

	while ((entry = readdir(d)) != NULL) {
		char path[1024];
		snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

		if (stat(path, &entry_stat) == -1) {
			perror("stat()");
			continue;
		}

		if (S_ISDIR(entry_stat.st_mode)) {
			dirs			= realloc(dirs, sizeof(char *) * (dir_count + 1));
			dirs[dir_count] = strdup(entry->d_name);
			dir_count++;
		} else {
			files = realloc(files, sizeof(char *) * (file_count + 1));
			files[file_count] = strdup(entry->d_name);
			file_count++;
		}
	}

	for (size_t i = 0; i < dir_count; i++) {
		printf("\033[1;34m%s\033[0m\n", dirs[i]);
		free(dirs[i]);
	}

	free(dirs);

	for (size_t i = 0; i < file_count; i++) {
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
	char trash_dir[1024];
	char trash_file_path[1024];

	snprintf(trash_dir, sizeof(trash_dir), "%s/.trash", currentUser.home);

	struct stat st = {0};
	if (stat(trash_dir, &st) == -1) {
		if (mkdir(trash_dir, 0700) != 0) {
			perror("mkdir ~/.trash error");
			return 1;
		}
	}

	snprintf(trash_file_path, sizeof(trash_file_path), "%s/%s", trash_dir,
			 path);

	if (rename(path, trash_file_path) != 0) {
		switch (errno) {
		case ENOENT:
			fprintf(stderr,
					"rm: cannot remove '%s': No such file or directory\n",
					path);
			break;
		case EACCES:
			fprintf(stderr, "rm: cannot remove '%s': Permission denied\n",
					path);
			break;
		default:
			perror("rm error");
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
			printf("  %s\n", builtins_str[i]);
	} else {
	}

	return 0;
}

// CHECK: exit some other way...?
int indus_exit(char **args) {
	int exit_status = 0;

	if (args[1] != NULL)
		exit_status = atoi(args[1]);

	exit(exit_status);
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
