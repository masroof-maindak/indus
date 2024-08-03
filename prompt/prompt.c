#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>

#include "prompt.h"
#include "../utils/utils.h"

char *populate_pwd(char *pwd){
	if (getcwd(pwd, PATH_MAX) == NULL) {
		perror("getcwd() error");
		return NULL;
	}

	return pwd;
}

char *shorten_home_in_prompt(char *pwd, char *username, char *prompt) {
	if (!SHORTEN_HOME) {
		return pwd;
	}

	size_t size = strlen(username) + 6;

	if (strlen(pwd) >= size) {
		char expect[size];
		strcpy (expect, "/home/");
		strcat (expect, username);

		if (!strncmp (expect, pwd, size)) { // returns 0 on success
			prompt[0] = '~';
			size_t remainingLen = strlen(pwd) - size;
			if (remainingLen > 0) {
				char *srcPtr = pwd + size;
				strcpy(prompt + 1, srcPtr);
			}
			return prompt;

		} else {
			printf ("PWD that brought me here: ... %s\n", pwd);
			return pwd;
		}

	}

	return pwd;
}

char *shorten_path_in_prompt(char *prompt) {
	if (!SHORTEN_PWD || strlen(prompt) == 1) {
		return prompt;
	}

	char *promptBkp = copy_string(prompt);
	char *backupCopy = promptBkp;
	int counter = 0;

	if (promptBkp[0] == '/') {
		promptBkp++;
		counter++;
	}
	
	char *word = NULL, *prev = NULL;

	if ((word = strtok(promptBkp, "/")) != NULL) {
		while (word) {
			prompt[counter++] = word[0];
			prev = word;

			if ((word = strtok(NULL, "/")) == NULL) {
				char* destPtr = prompt + counter;
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
	char *ret = malloc(PATH_MAX + 1);
	if (ret == NULL) {
		perror("ret malloc() error");
		return NULL;
	}

	pwd = populate_pwd(pwd);
	if (pwd == NULL) {
		free(ret);
		return NULL;
	}

	ret = shorten_home_in_prompt(pwd, username, ret);

	ret = shorten_path_in_prompt(ret);

	ret = show_git_branch(ret);

	return ret;
}
