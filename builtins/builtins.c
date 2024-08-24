#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "builtins.h"
extern struct USER_INFO currentUser;

char *builtins_str[]		  = {"cd", "ls", "rm", "help", "exit"};
int (*builtins_fn[])(char **) = {indus_cd, indus_ls, indus_rm, indus_help,
								 indus_exit};

/*
 * NOTE: Can I somehow make a global, dynamic struct for flag parsing that I can
 * use in both the shell's binary itself and all the builtins too?
 *
 * Hardcoding args[1] to be the dir for instance is bad design.
 * Both `ls -a dirName` and `ls dirName -a` should function identically.
 */

int num_builtins() { return sizeof(builtins_str) / sizeof(builtins_str[0]); }

int indus_ls(char **args) {

	char *dir;
    DIR *d;
    struct dirent *dir_entry;

	if (args[1] == NULL) {
		// dir = pwd
		 dir = get_pwd(); 
        if (dir == NULL) {
            perror("get_pwd() error");
            return 1;
        }
	} 
	else {
		 dir = args[1];
	}
	d = opendir(dir);
    if (d == NULL) {
        perror("opendir() error");
        return 1;
    }
	 while ((dir_entry = readdir(d)) != NULL) {
        // Skip the "." and ".." entries
        if (strcmp(dir_entry->d_name, ".") != 0 && strcmp(dir_entry->d_name, "..") != 0) {
            printf("%s\n", dir_entry->d_name);
        }
    }

    // Close the directory stream
    if (closedir(d) == -1) {
        perror("closedir() error");
        return 1;
    }

	return 0;
}

int	indus_cd(char **args) {
	char* dir;

	if (args[1] == NULL)
		dir = currentUser.home;
	else
		dir = args[1];

	if (chdir(dir) != 0) {
		switch (errno) {
			case ENOENT:
				fprintf(stderr, "cd: %s: No such file or directory\n", dir);
				break;
			case ENOTDIR:
				fprintf(stderr, "cd: %s: Not a directory\n", dir);
				break;
			case EACCES:
				fprintf(stderr, "cd: %s: Permission denied\n", dir);
				break;
			default:
				fprintf(stderr, "cd: Error occurred while changing directory to %s\n", dir);
				break;
		}
		return 1;
	}

	return 0;
}

int indus_rm(char **args) {
	// this function move the file being deleted to ~/.trash
	if (args[1] == NULL) {
		// Leave if no argument passed
	}

	return 0;
}

int indus_help(char **args) {
	 if (args[1] == NULL) {
        // Show all built-ins
        printf("Available commands:\n");
        for (int i = 0; i < num_builtins(); i++) {
            printf("  %s\n", builtins_str[i]);
        }
    } else {
        printf("No help available for '%s'.\n", args[1]);
    }
    return 0;
}

int indus_exit(char **args) { return 0; }
