#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../utils/utils.h"
#include "prompt.h"

char *populate_pwd() {
	char *pwd = getcwd(NULL, 0);

	if (pwd == NULL) {
		perror("getcwd");
		return NULL;
	}

	return pwd;
}

char *shorten_home_in_prompt(char *pwd, char *username) {
	if (!SHORTEN_HOME) {
		return pwd;
	}

	size_t sizeLongHome = strlen(username) + 6;
	size_t sizeOfPwd	= strlen(pwd);

	if (sizeOfPwd < sizeLongHome) {
		return pwd;
	}

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

	} else {
		fprintf(stderr, "PWD that brought me here: ... %s\n", pwd);
		return pwd;
	}
}

char *shorten_path_in_prompt(char *prompt) {
	if (!SHORTEN_PWD || strlen(prompt) == 1) {
		return prompt;
	}

	char *promptBkp	 = copy_string(prompt);
	char *backupCopy = promptBkp;
	int counter		 = 0;

	if (promptBkp[0] == '/') {
		promptBkp++;
		counter++;
	}

	char *word = NULL, *prev = NULL;

	if ((word = strtok(promptBkp, "/")) != NULL) {
		while (word) {
			prompt[counter++] = word[0];
			prev			  = word;

			if ((word = strtok(NULL, "/")) == NULL) {
				char *destPtr = prompt + counter;
				strcpy(destPtr, prev + 1);
			} else {
				prompt[counter++] = '/';
			}
		}
	}

	/*
	 * If preserving memory were a concern,
	 * we could reallocate `prompt` to strlen(prompt) to not 'waste' space.
	 *
	 * But since this function will run at every iteration of the program's
	 * main command loop, it's better to not do this.
	 */

	free(backupCopy);
	return prompt;
}

/*
 * Check Parent directories for git branch
 * If found, simply strcat it to the end for now
 * TODO: formatting options?
 */
char *show_git_branch(char *ret) {
	if (!ADD_GIT_BRANCH) {
		return ret;
	}

	/* char branchName[GIT_BRANCH_BUFFER_LEN + 1]; */

	return ret;
}

char *generate_prompt(char *pwd, char *username) {

	char *pwdCopy = copy_string(pwd);

	char *ret;

	ret = shorten_home_in_prompt(pwdCopy, username);

	ret = shorten_path_in_prompt(ret);

	ret = show_git_branch(ret);

	return ret;
}
