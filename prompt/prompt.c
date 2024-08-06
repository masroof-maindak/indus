#include <errno.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../utils/utils.h"
#include "prompt.h"

char *get_pwd() {
	long int path_max;
	size_t size;
	char *buf;
	char *ptr;

	path_max = pathconf(".", _PC_PATH_MAX);
	if (path_max == -1)
		size = 1024;
	else if (path_max > 10240)
		size = 10240;
	else
		size = path_max;

	for (buf = ptr = NULL; ptr == NULL; size *= 2) {
		if ((buf = realloc(buf, size)) == NULL) {
			perror("realloc()");
			return NULL;
		}

		ptr = getcwd(buf, size);
		if (ptr == NULL && errno != ERANGE) {
			perror("getcwd()");
			return NULL;
		}
	}

	return ptr;
}

char *shorten_home_in_pwd(char *pwd, char *username) {
	if (!SHORTEN_HOME)
		return pwd;

	size_t sizeLongHome = strlen(username) + 6;
	size_t sizeOfPwd	= strlen(pwd);

	if (sizeOfPwd < sizeLongHome)
		return pwd;

	char longHome[sizeLongHome];
	strcpy(longHome, "/home/");
	strcat(longHome, username);

	if (!strncmp(longHome, pwd, sizeLongHome)) {

		size_t sizeRet		= sizeOfPwd - sizeLongHome;
		char *prompt		= malloc(sizeRet);
		prompt[0]			= '~';
		size_t remainingLen = strlen(pwd) - sizeLongHome;

		if (remainingLen > 0) {
			char *srcPtr = pwd + sizeLongHome;
			strcpy(prompt + 1, srcPtr);
		}

		return prompt;
	}

	fprintf(stderr, "strncmp error in shorten_home_in_pwd: %s\n", pwd);
	return pwd;
}

char *shorten_path_in_pwd(char *pwd) {
	if (!SHORTEN_PWD || strlen(pwd) == 1)
		return pwd;

	char *promptBkp = copy_string(pwd);
	char *copySave	= promptBkp;
	int counter		= 0;

	if (promptBkp[0] == '/') {
		promptBkp++;
		counter++;
	}

	char *word = NULL;
	char *prev = NULL;

	if ((word = strtok(promptBkp, "/")) != NULL) {
		while (word) {
			pwd[counter++] = word[0];
			prev		   = word;

			if ((word = strtok(NULL, "/")) == NULL) {
				strcpy(pwd + counter, prev + 1);
			} else {
				pwd[counter++] = '/';
			}
		}
	}

	free(copySave);
	return pwd;
}

/*
 * Check Parent directories for git branch
 * If found, simply strcat it to the end for now
 * TODO: formatting options?
 */
char *show_git_branch(char *pwd) {
	if (!ADD_GIT_BRANCH)
		return pwd;

	/* char branchName[GIT_BRANCH_BUFFER_LEN + 1]; */

	return pwd;
}

char *generate_prompt(char *pwd, char *username) {

	char *ret;
	char *pwdCopy = copy_string(pwd);

	ret = shorten_home_in_pwd(pwdCopy, username);

	ret = shorten_path_in_pwd(ret);

	ret = show_git_branch(ret);

	return ret;
}
