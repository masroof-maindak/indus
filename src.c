#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtins/builtins.h"
#include "prompt/prompt.h"
#include "utils/utils.h"

#define USERNAME_BUFFER 32

int execute(char **args) {
	if (args == NULL)
		return 0;

	for (int i = 0; i < num_builtins(); i++)
		if (!strcmp(args[0], builtins_str[i]))
			return builtins_fn[i](args);

	// TODO: fork() & exec()

	return 0;
}

int run() { return 0; }

void loop() {
	char *username = malloc(USERNAME_BUFFER + 1);
	username	   = get_username();

	if (username == NULL) {
		fputs("Failed to fetch username\n", stderr);
		return;
	}

	char *input = NULL;
	char **args = NULL;
	int status	= 0;

	printf("Greetings, %s. Welcome to Indus.\n", username);
	printf("Type " ACCENT "help" COL_RESET " to get started.\n");
	puts("Press Ctrl+c to exit.\n");

	while (1) {
		char *prompt = NULL;
		char *pwd	 = get_pwd();

		if (pwd == NULL) {
			free(username);
			return;
		}

		prompt = generate_prompt(pwd, username);
		input  = get_line(prompt);
		args   = parse_input(input);
		status = execute(args);

		free(pwd);
		free(prompt);
		free(input);
		free(args);
	}

	free(username);
}

int cleanup() { return 0; }

void init() { /* signal(SIGINT, SIG_IGN); */ }

int main(/* int argc, char **argv, char **envp */) {
	init();
	loop();
	return cleanup();
}
