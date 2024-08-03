#ifndef PROMPT_H
#define PROMPT_H

#define SHORTEN_HOME		  0
#define SHORTEN_PWD			  1
#define ADD_GIT_BRANCH		  1
#define GIT_BRANCH_BUFFER_LEN 256

/* COLOURS */
#define COL_RED		"\x1b[31m"
#define COL_GREEN	"\x1b[32m"
#define COL_YELLOW	"\x1b[33m"
#define COL_BLUE	"\x1b[34m"
#define COL_MAGENTA "\x1b[35m"
#define COL_CYAN	"\x1b[36m"
#define COL_RESET	"\x1b[0m"

#define ACCENT		COL_YELLOW
#define PROMPT_CHAR "*"

/* FUNCTIONS */
char *populate_pwd();
char *generate_prompt(char *pwd, char *username);

#endif // PROMPT_H
