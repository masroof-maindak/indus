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
#include "../include/prompt.h"

#define INITIAL_SIZE 10
#define SCALE_FACTOR 2

extern struct USER_INFO currentUser;

char *builtinsStr[]			  = {"cd",	  "ls",	   "pwd",	"help",	 "exit",
								 "mkdir", "clear", "trash", "whoami"};
int (*builtinsFnc[])(char **) = {indus_cd,	  indus_ls,	   indus_pwd,
								 indus_help,  indus_exit,  indus_mkdir,
								 indus_clear, indus_trash, indus_whoami};

int num_builtins() { return sizeof(builtinsStr) / sizeof(builtinsStr[0]); }

/* FIXME: Conditional jump or move depends on uninitialised value(s) */

int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

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

	int dAlloc = INITIAL_SIZE, fAlloc = INITIAL_SIZE, sAlloc = INITIAL_SIZE;
    int dCount = 0, fCount = 0, sCount = 0;

    char **dirs = malloc(dAlloc * sizeof(char *));
    char **files = malloc(fAlloc * sizeof(char *));
    char **symlinks = malloc(sAlloc * sizeof(char *));

	if (!dirs || !files || !symlinks) {
        perror("malloc()");
        closedir(d);
        return 1;
    }

	while ((entry = readdir(d)) != NULL) {
		if (entry->d_name[0] == '.' && (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)) {
            continue; 
        }

		char path[1024];
		snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);

		if (lstat(path, &entryStat) == -1) {
			perror("lstat()");
			return 1;
		}

		if (S_ISDIR(entryStat.st_mode)) {
			if (dCount >= dAlloc) {
                dAlloc *= SCALE_FACTOR;
                dirs = realloc(dirs, dAlloc * sizeof(char *));
                if (!dirs) {
                    perror("realloc()");
                    closedir(d);
                    return 1;
                }
			}
			dirs[dCount++] = strdup(entry->d_name);
		} else if (S_ISLNK(entryStat.st_mode)) {
           if (sCount >= sAlloc) {
                sAlloc *= SCALE_FACTOR;
                symlinks = realloc(symlinks, sAlloc * sizeof(char *));
                if (!symlinks) {
                    perror("realloc()");
                    closedir(d);
                    return 1;
                }
            }
            symlinks[sCount++] = strdup(entry->d_name);
		} else {
			if (fCount >= fAlloc) {
                fAlloc *= SCALE_FACTOR;
                files = realloc(files, fAlloc * sizeof(char *));
                if (!files) {
                    perror("realloc()");
                    closedir(d);
                    return 1;
                }
            }
            files[fCount++] = strdup(entry->d_name);
        }
	}

	if (dCount > 0) {
        qsort(dirs, dCount, sizeof(char *), compare_strings);
    }
    if (fCount > 0) {
        qsort(files, fCount, sizeof(char *), compare_strings);
    }
	if (sCount > 0) {
        qsort(symlinks, sCount, sizeof(char *), compare_strings);
    }

	for (int i = 0; i < dCount; i++) {
		printf("%s%s%s  ", DIR_COLOR, dirs[i], RESET_COLOR);
		free(dirs[i]);
	}

	free(dirs);

	for (int i = 0; i < fCount; i++) {
		printf("%s%s%s  ", FILE_COLOR, files[i], RESET_COLOR);
		free(files[i]);
	}

	free(files);

	for (int i = 0; i < sCount; i++) {
        printf("%s%s%s  ", ACCENT, symlinks[i], RESET_COLOR);
        free(symlinks[i]);
    }
    free(symlinks);

	printf("\n");

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
		dir = copy_string(currentUser.home);
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

	free(dir);
	return 0;
}

/* TODO */
int indus_trash(char **args) {
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

	if (mkdir(dir, 0755) != 0) {
		if (errno == EACCES)
			fputs("Permission denied\n", stderr);
		else
			perror("mkdir()");
		return 1;
	}

	free(dir);
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
