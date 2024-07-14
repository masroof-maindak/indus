#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include <readline/readline.h>
#include <readline/history.h>

/* COLOURS */
#define COL_RED     "\x1b[31m"
#define COL_GREEN   "\x1b[32m"
#define COL_YELLOW  "\x1b[33m"
#define COL_BLUE    "\x1b[34m"
#define COL_MAGENTA "\x1b[35m"
#define COL_CYAN    "\x1b[36m"
#define COL_RESET   "\x1b[0m"

#define INPUT_BUFFER 1024

// TODO: read these from config
#define ACCENT 		COL_YELLOW
#define PROMPT_CHAR ";"
#define SHORTEN_HOME 1
#define SHORTEN_PWD 1

void parse() {}

void run() {}

void loop() {
	char* username;
	username = getlogin();

	char pwd[PATH_MAX];

	char* input;
	char** arg;
	int status;

	printf("Greetings, %s. Welcome to Indus.\n", username);
	printf("Type " ACCENT "help" COL_RESET " to get started.\n");
	puts("Press Ctrl+c to exit.\n");

	while (1) {
		if (getcwd(pwd, sizeof(pwd)) == NULL) {
			perror("getcwd() error");
			return;
		}

		if (SHORTEN_HOME) {
			// TODO: shorten `/home/maindak` to `~`
		}

		if (SHORTEN_PWD) {
			// TODO: shorten `~/Documents/xyz` to `~/D/xyz`
		}

		printf("%s" ACCENT PROMPT_CHAR COL_RESET " ", pwd);
		input = readline("");
		add_history(input);

		arg = parse(input);
		status = run(arg);

		printf("No you're a %s\n", input);

		free(input);
		free(args);
	}
}

int main(int argc, char** argv) {
	system("clear");

	// TODO: Parse command line options?

	// TODO: Parse config

	loop();

	// TODO: Perform shutdown/cleanup

	return 0;
}
