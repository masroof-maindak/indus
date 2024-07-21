#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "prompt/prompt.h"

/* COLOURS */
#define COL_RED     "\x1b[31m"
#define COL_GREEN   "\x1b[32m"
#define COL_YELLOW  "\x1b[33m"
#define COL_BLUE    "\x1b[34m"
#define COL_MAGENTA "\x1b[35m"
#define COL_CYAN    "\x1b[36m"
#define COL_RESET   "\x1b[0m"

#define INPUT_BUFFER 1024
#define USERNAME_BUFFER 32

// TODO: read these from XDG_CONFIG_DIR/indus/config(?)
// TODO: mkdir -p if it doesn't exist
// TODO: live reload(?) - watch for changes to file in another thread
#define ACCENT 		COL_YELLOW
#define PROMPT_CHAR "*"

char pathSplit[] = "/";

// NOTE: Should arg be a char***? For catering to pipes, `&&`, and `||`
void parse() {}

void run() {}

char* copy_string(char* str) {
	char* copy = malloc(strlen(str) + 1);
	strcpy(copy, str);
	return copy;
}

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

		char* prompt = generate_prompt(pwd, username);

		printf("%s" ACCENT PROMPT_CHAR COL_RESET " ", prompt);
		input = readline("");
		add_history(input);

		/*arg = parse(input);*/
		/*status = run(arg);*/

		printf("No you're a %s\n", input);

		free(input);
		free(prompt);
		/*free(arg);*/

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
void parse_config() {/* TODO */}
int cleanup() {
	/* TODO: sigint handler? */
	/* TODO: dump history to file */
	return 0;
}

int main(int argc, char** argv) {

	if (argc > 1) {
		parse_flags(argc, argv);
	}

	parse_config();

	loop();

	return cleanup();
}
