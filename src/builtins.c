#include <dirent.h>
#include <errno.h>
#include <linux/limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/bool.h"
#include "../include/builtins.h"
#include "../include/prompt.h"
#include "../include/utils.h"

extern struct USER_INFO currentUser;

char *builtinsStr[]			  = {"cd",	  "ls",	   "pwd",	"help",	 "exit",
								 "mkdir", "clear", "trash", "whoami"};
int (*builtinsFnc[])(char **) = {indus_cd,	  indus_ls,	   indus_pwd,
								 indus_help,  indus_exit,  indus_mkdir,
								 indus_clear, indus_trash, indus_whoami};

int num_builtins() { return sizeof(builtinsStr) / sizeof(builtinsStr[0]); }

int compare(const void *a, const void *b) {
	return strcmp(*(char **)a, *(char **)b);
}

/* TODO: Refactor */
int indus_ls(char **args) {

	bool argGiven = false;
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

	int dAlloc = 8, fAlloc = 8, sAlloc = 8;
	int dCount = 0, fCount = 0, sCount = 0;

	char **dirs		= malloc(dAlloc * sizeof(char *));
	char **files	= malloc(fAlloc * sizeof(char *));
	char **symlinks = malloc(sAlloc * sizeof(char *));

	if (!dirs || !files || !symlinks) {
		perror("malloc()");
		closedir(d);
		return 1;
	}

	while ((entry = readdir(d)) != NULL) {
		if (entry->d_name[0] == '.' && (strcmp(entry->d_name, ".") == 0 ||
										strcmp(entry->d_name, "..") == 0)) {
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
				dAlloc *= 2;
				dirs = realloc(dirs, dAlloc * sizeof(char *));
				if (!dirs) {
					perror("realloc()");
					closedir(d);
					return 1;
				}
			}

			dirs[dCount++] = copy_string(entry->d_name);
		} else if (S_ISLNK(entryStat.st_mode)) {
			if (sCount >= sAlloc) {
				sAlloc *= 2;
				symlinks = realloc(symlinks, sAlloc * sizeof(char *));
				if (!symlinks) {
					perror("realloc()");
					closedir(d);
					return 1;
				}
			}

			symlinks[sCount++] = copy_string(entry->d_name);
		} else {
			if (fCount >= fAlloc) {
				fAlloc *= 2;
				files = realloc(files, fAlloc * sizeof(char *));
				if (!files) {
					perror("realloc()");
					closedir(d);
					return 1;
				}
			}

			files[fCount++] = copy_string(entry->d_name);
		}
	}

	qsort(dirs, dCount, sizeof(char *), compare);
	qsort(files, fCount, sizeof(char *), compare);
	qsort(symlinks, sCount, sizeof(char *), compare);

	for (int i = 0; i < dCount; i++) {
		printf(BOLD_COL_BLUE "%s\n", dirs[i]);
		free(dirs[i]);
	}

	free(dirs);

	int totalCount = fCount + sCount;
	char **merged  = malloc(totalCount * sizeof(char *));
	char **colors  = malloc(totalCount * sizeof(char *));

	if (!merged || !colors) {
		perror("malloc()");
		closedir(d);
		return 1;
	}

	int fIdx = 0, sIdx = 0, mIdx = 0;

	while (fIdx < fCount && sIdx < sCount) {
		if (strcmp(files[fIdx], symlinks[sIdx]) < 0) {
			merged[mIdx]   = files[fIdx];
			colors[mIdx++] = FILE_COLOR;
			fIdx++;
		} else {
			merged[mIdx]   = symlinks[sIdx];
			colors[mIdx++] = ACCENT;
			sIdx++;
		}
	}

	while (fIdx < fCount) {
		merged[mIdx]   = files[fIdx];
		colors[mIdx++] = FILE_COLOR;
		fIdx++;
	}

	while (sIdx < sCount) {
		merged[mIdx]   = symlinks[sIdx];
		colors[mIdx++] = ACCENT;
		sIdx++;
	}

	for (int i = 0; i < totalCount; i++) {
		printf("%s%s\n" COL_RESET, colors[i], merged[i]);
		free(merged[i]);
	}

	free(merged);
	free(colors);
	free(files);
	free(symlinks);

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

	if (dir == NULL)
		return 1;

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

int indus_trash(char **args) {

	if (args[1] == NULL) {
		fputs("Missing operand\n", stderr);
		return 1;
	}

	for (int i = 1; args[i] != NULL; i++) {
		char trashPath[PATH_MAXL];

		int ret;
		char *fileName	= expand_tilde(args[i]);
		char *printName = fileName;

		if (fileName == NULL)
			return 1;

		if (args[i][0] == '~')
			printName = args[i] + 2;

		ret = snprintf(trashPath, PATH_MAXL, "%s/%s", currentUser.trashDir,
					   printName);

		if (ret < 0) {
			fprintf(stderr,
					"sprintf encountered an error while formatting %s: %s",
					fileName, strerror(errno));
			free(fileName);
			return 1;
		}

		if (rename(fileName, trashPath) == -1) {
			if (errno == EXDEV)
				/* TODO?: Copy files over manually */
				/* https://stackoverflow.com/a/17440097 */
				fputs("File to trash is on a different FS!", stderr);
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
	if (args[1] == NULL) {
		printf(BOLD_COL_MAGENTA "Available commands:\n" COL_RESET COL_GREEN);
		for (int i = 0; i < num_builtins(); i++)
			printf("  %s\n", builtinsStr[i]);
		printf(COL_RESET);
	} else {
		puts("Invalid input format!");
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

	if (dir == NULL)
		return 1;

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
