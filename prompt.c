#include <stdio.h>

static char input[2048];

int main(int argc, char** argv) {

	puts("Indus version 0.0.0.0.1");
	puts("Press Ctrl+c to exit\n");

	// Using 'true' here doesn't work because C doesn't have bools
	while (1) {

		// file put string
		// Doesn't append a newline char 
		fputs("indus> ", stdout);

		// file get string
		// Stores 2048 bytes in input buffer
		fgets(input, 2048, stdin);

		printf("No you're a %s", input);
	}

	return 0;
}
