#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "prompt/prompt.h"
#include "utils/utils.h"

#define INPUT_BUFFER 1024
#define USERNAME_BUFFER 32

// NOTE: Should arg be a char***? For catering to pipes, `&&`, and `||`
char* parse_input(char* input) {

	return NULL;
}

int run() {return 0;}

void loop() {
	char* username = malloc(USERNAME_BUFFER + 1);
	username = getlogin();

	if (username == NULL) {
		perror("getlogin() error");
		return;
	}

	char *pwd = malloc(PATH_MAX);

	char* input;
	/* char** arg; */
	/* int status; */

	printf("Greetings, %s. Welcome to Indus.\n", username);
	printf("Type " ACCENT "help" COL_RESET " to get started.\n");
	puts("Press Ctrl+c to exit.\n");

	while (1) {

		char *prompt = generate_prompt(pwd, username);

		printf("%s" ACCENT PROMPT_CHAR COL_RESET " ", prompt);
		input = readline(NULL);

		/* arg = parse_input(input); */
		/*status = run(arg);*/

		printf("No you're a %s\n", input);

		free(input);
		free(prompt);
		/* free(arg); */

		input = NULL;
		prompt = NULL;
	}

	free(pwd);
	free(username);
}

void parse_flags(int argc, char** argv) {
	/* TODO */
	char* first = copy_string(argv[1]);
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

int main(int argc, char** argv) {

	if (argc > 1) {
		parse_flags(argc, argv);
	}

	init();

	loop();

	return cleanup();
}
