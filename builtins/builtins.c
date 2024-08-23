#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <pwd.h>

#include "builtins.h"

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
	if (args[1] == NULL) {
		// dir = pwd
	}

	return 0;
}

int indus_cd(char **args) {
	char* dir;
	if (args[1] == NULL) {
		// CD to home
		struct passwd *pw =getpwuid(getuid());
		if (pw==NULL){
			perror("getpwuid() error");
			return 1;
		}
		dir = pw->pw_dir;
	}
	else {
		dir = args[1];
	}

	//change to dir
	if (chdir(dir) != 0) {
        perror("chdir() error");
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
		// show all builtins
	}

	return 0;
}

int indus_exit(char **args) { return 0; }
