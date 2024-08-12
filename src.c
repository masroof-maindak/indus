#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "prompt/prompt.h"
#include "utils/utils.h"

#define USERNAME_BUFFER 32

char **parse_input(char *input) {
	char **args;
	char *arg;

	return NULL;
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
	char **args;
	/* int status; */

	printf("Greetings, %s. Welcome to Indus.\n", username);
	printf("Type " ACCENT "help" COL_RESET " to get started.\n");
	puts("Press Ctrl+c to exit.\n");

	while (1) {
		char *pwd = get_pwd();

		if (pwd == NULL) {
			fputs("Failed to fetch pwd\n", stderr);
			free(username);
			return;
		}

		char *prompt = generate_prompt(pwd, username);

		printf("%s " ACCENT PROMPT_CHAR COL_RESET " ", prompt);

		input = get_line();

		if (input == NULL) {
			free(pwd);
			free(prompt);
			continue;
		}

		printf("input: %s\n", input);

		args = parse_input(input);
		/* status = run(args); */

		free(pwd);
		free(input);
		free(prompt);
		free(args);
	}

	free(username);
}

int cleanup() {
	/* TODO: dump history to file */
	return 0;
}

void init() {
	/* signal(SIGINT, SIG_IGN); */
	/* load_history(); from file? */
}

int main(/* int argc, char **argv, char **envp */) {
	init();
	loop();
	return cleanup();
}
