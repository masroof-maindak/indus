#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../include/bool.h"
#include "../include/builtins.h"
#include "../include/prompt.h"
#include "../include/utils.h"

struct USER_INFO currUser = {0, NULL, NULL, NULL};

int run(char **args);
int cleanup() { return 0; }
void loop();
bool init();

int main(/* int argc, char **argv, char **envp */) {
	if (init())
		loop();
	return cleanup();
}

int execute(char **args) {
	if (!args)
		return 0;

	/* TODO: proper parsing */

	for (int i = 0; i < num_builtins(); i++)
		if (!strcmp(args[0], builtinsStr[i]))
			return builtinsFnc[i](args);

	return run(args);
}

int run(char **args) {
	int wstatus;
	pid_t pid = fork();

	switch (pid) {
	case -1:
		perror("fork()");
		return -1;
	case 0: /* CHILD */
		execvp(args[0], args);
		switch (errno) {
		case EACCES:
			fprintf(stderr, "indus: %s: Permission denied\n", args[0]);
			break;
		case ENOENT:
			fprintf(stderr, "indus: %s: command not found\n", args[0]);
			break;
		default:
			perror("execvp()");
		}
		return -2;
	default: /* PARENT */
		if (waitpid(pid, &wstatus, 0) != pid) {
			perror("waitpid()");
			return -3;
		}

		if (WIFEXITED(wstatus))
			return 0;
	}

	return 0;
}

void loop() {
	char *input = NULL;
	char **args = NULL;
	int status	= 0;

	printf("Greetings, %s. Welcome to Indus.\n", currUser.name);
	printf("Type " ACCENT "help" COL_RESET " to get started.\n");
	printf("Press Ctrl+c to exit.\n\n");

	while (status == 0) {
		char *prompt = NULL;
		char *pwd	 = get_pwd();

		if (pwd == NULL)
			return;

		prompt = generate_prompt(pwd);
		input  = get_line(prompt);
		args   = parse_input(input);

		if (args == NULL) {
			free(pwd);
			if (prompt != pwd)
				free(prompt);
			free(input);
		}

		status = execute(args);

		free(pwd);
		if (prompt != pwd)
			free(prompt);
		free(input);
		free(args);
	}
}

bool init() {
	signal(SIGINT, SIG_IGN);
	init_user_info(&currUser);
	ensure_trash_dir_exists();
	if (currUser.trashDir == NULL)
		return false;
	return true;
}
