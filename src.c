#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "prompt/prompt.h"
#include "utils/utils.h"

#define USERNAME_BUFFER 32

char *parse_input(char *input) { return NULL; }

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

		free(pwd);
		free(input);
		free(prompt);
		/* free(arg); */

		pwd	   = NULL;
		input  = NULL;
		prompt = NULL;
		/* arg = null; */
	}

	free(username);
}

void parse_flags(int argc, char **argv) {
	/* TODO */
	char *first = copy_string(argv[1]);
	printf("%s\n", first);
}

void parse_config() {
	/* TODO: read config vars from XDG_CONFIG_DIR/indus/config(?) */
	/* TODO: mkdir -p if it doesn't exist */
	/* TODO: live reload(?) - watch for changes to file in another thread */
}

int cleanup() {
	/* TODO: sigint handler? */
	/* TODO: dump history to file */
	return 0;
}

void init() {
	/* load_history(); from file? */
	parse_config();
}

int main(int argc, char **argv) {

	if (argc > 1)
		parse_flags(argc, argv);

	init();

	loop();

	return cleanup();
}
