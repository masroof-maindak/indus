#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>

#include "prompt.h"

char* populate_pwd(char* pwd){
	if (getcwd(pwd, PATH_MAX) == NULL) {
		perror("getcwd() error");
		return NULL;
	}

	return pwd;
}

char* generate_prompt(char* pwd, char* username) {
	char* ret = malloc(PATH_MAX);

	pwd = populate_pwd(pwd);

	if (SHORTEN_HOME) {
		size_t size = strlen(username) + 6;

		if (strlen(pwd) < size) {
			strcpy(ret, pwd);
		} else {
			char expect[size];
			strcpy (expect, "/home/");
			strcat (expect, username);

			if (!strncmp (expect, pwd, size)) { // returns 0 on success
				ret[0] = '~';
				size_t remainingLen = strlen(pwd) - size;
				if (remainingLen > 0){
					char* srcPtr = pwd + size;
					memcpy(ret + 1, srcPtr, remainingLen);
				}
			}
		}
	}

	if (SHORTEN_PWD) {
		// TODO: shorten `~/Documents/xyz` to `~/D/xyz`
	}

	if (ADD_GIT_BRANCH) {
		// TODO: in format: `(main)`
	}

	free(pwd);
	pwd = NULL;

	return ret;
}
