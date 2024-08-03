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

char *shorten_path_in_prompt(char *prompt) {
	if (!SHORTEN_PWD || strlen(prompt) == 1) {
		return prompt;
	}

	char* final_string = copy_string(ret);
	printf("%s\n", final_string);

	if (!SHORTEN_HOME) {
		/* 1. /home/maindak 	=>	/h/maindak 	*/
		/* 2. /home		 		=>	/home 		*/

	} else {
		/* 3. ~/Documents	 	=>	~/Documents */
		/* 4. ~/Documents/a 	=>	~/D/a 		*/
		/* 5. ~			 		=>	~ 			*/

	}

	return ret;
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
