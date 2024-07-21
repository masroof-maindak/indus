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

char* shorten_home_dir(char* pwd, char* username, char* ret) {
	if (!SHORTEN_HOME) {
		return pwd;
	}

	size_t size = strlen(username) + 6;

	if (strlen(pwd) >= size) {
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
			ret[remainingLen + 1] = '\0'; // null terminator
			return ret;

		} else {
			printf ("PWD that brought me here: ... %s\n", pwd);
			return pwd;
		}

	}

	return pwd;
}

char* generate_prompt(char* pwd, char* username) {
	char* ret = malloc(PATH_MAX);
	if (ret == NULL) {
		perror("malloc() error");
		return NULL;
	}

	pwd = populate_pwd(pwd);
	if (pwd == NULL) {
		free(ret);
		return NULL;
	}

	ret = shorten_home_dir(pwd, username, ret);

	if (SHORTEN_PWD) {
		// TODO: shorten `~/Documents/xyz` to `~/D/xyz`
	}

	if (ADD_GIT_BRANCH) {
		// TODO: in format: `(main)`
	}

	return ret;
}
