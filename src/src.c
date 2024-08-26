#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/bool.h"
#include "../include/builtins.h"
#include "../include/prompt.h"
#include "../include/utils.h"

struct USER_INFO currentUser = {-1, NULL, NULL, NULL};

int execute(char **args) {
	if (args == NULL)
		return 0;

	for (int i = 0; i < num_builtins(); i++)
		if (!strcmp(args[0], builtinsStr[i]))
			return builtinsFnc[i](args);

	// TODO: fork() & exec()

	return 0;
}

int run() { return 0; }

void loop() {
	char *input = NULL;
	char **args = NULL;
	int status	= 0;

	printf("Greetings, %s. Welcome to Indus.\n", currentUser.name);
	printf("Type " ACCENT "help" COL_RESET " to get started.\n");
	puts("Press Ctrl+c to exit.\n");

	while (1) {
		char *prompt = NULL;
		char *pwd	 = get_pwd();

		if (pwd == NULL)
			return;

		prompt = generate_prompt(pwd);
		input  = get_line(prompt);
		args   = parse_input(input);
		status = execute(args);

		free(pwd);
		free(prompt);
		free(input);
		free(args);
	}
}

int cleanup() { return 0; }

bool init() {
	/* signal(SIGINT, SIG_IGN); */
	init_user_info(&currentUser);
	ensure_trash_dir_exists();
	if (currentUser.trashDir == NULL)
		return false;
	return true;
}

int main(/* int argc, char **argv, char **envp */) {
	if (init())
		loop();
	return cleanup();
}
