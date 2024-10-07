#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <readline/readline.h>

#include "../include/prompt.h"
#include "../include/utils.h"

extern struct USER_INFO currentUser;

char *get_line(char *prompt) {
	char display[strlen(prompt) + 3];
	sprintf(display, "%s " ACCENT PROMPT_CHAR COL_RESET " ", prompt);
	return readline(display);
}

char *shorten_home_in_pwd(char *pwd) {
	if (!SHORTEN_HOME)
		return pwd;

	size_t sizeOfHome = strlen(currentUser.home);
	size_t sizeOfPwd  = strlen(pwd);

	if (sizeOfPwd < sizeOfHome)
		return pwd;

	if (!strncmp(currentUser.home, pwd, sizeOfHome)) {

		size_t sizeLeft = sizeOfPwd - sizeOfHome + 1; /* '~' */
		char *ret		= malloc(sizeLeft + 1);

		if (ret == NULL) {
			perror("malloc()");
			return pwd;
		}

		ret[0] = '~';

		/* TODO: convert to memcpy */
		if (sizeLeft > 0)
			strcpy(ret + 1, pwd + sizeOfHome);

		return ret;
	}

	return pwd;
}

char *shorten_path_in_pwd(char *pwd) {
	if (!SHORTEN_PWD || strlen(pwd) == 1)
		return pwd;

	int counter		= 0;
	char *word		= NULL;
	char *prev		= NULL;
	char *promptBkp = copy_string(pwd);

	if (promptBkp == NULL)
		return pwd;

	char *save = promptBkp;

	if (promptBkp[0] == '/') {
		promptBkp++;
		counter++;
	}

	if ((word = strtok(promptBkp, "/")) != NULL) {
		while (word) {
			pwd[counter++] = word[0];
			prev		   = word;

			if ((word = strtok(NULL, "/")) == NULL)
				strcpy(pwd + counter, prev + 1);
			else
				pwd[counter++] = '/';
		}
	}

	free(save);
	return pwd;
}

char *generate_prompt(char *pwd) {
	char *ret;
	ret = shorten_home_in_pwd(pwd);
	ret = shorten_path_in_pwd(ret);
	return ret;
}
