#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

int main(int argc, char** argv) {

	puts("Indus version 0.0.0.0.1");
	puts("Press Ctrl+c to exit\n");

	while (1) {
		char* input = readline("lisp> ");

		// add to history
		add_history(input);

		// Print back
		printf("No you're a %s\n", input);
		// have to manually print a newline, because readline strips it out

		// Need `libedit-dev` on linux to compile
		// histedit.h on arch

		// free buffer
		free(input);
	}

	return 0;
}
