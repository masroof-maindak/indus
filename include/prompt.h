#ifndef PROMPT_H
#define PROMPT_H

#define SHORTEN_HOME 1
#define SHORTEN_PWD	 0

/* COLOURS */
#define RESET		 "\x1b[0m"
#define RED			 "\x1b[31m"
#define GREEN		 "\x1b[32m"
#define YELLOW		 "\x1b[33m"
#define BLUE		 "\x1b[34m"
#define MAGENTA		 "\x1b[35m"
#define CYAN		 "\x1b[36m"
#define WHITE		 "\x1b[37m"
#define BOLD_RED	 "\x1b[1;31m"
#define BOLD_GREEN	 "\x1b[1;32m"
#define BOLD_YELLOW	 "\x1b[1;33m"
#define BOLD_BLUE	 "\x1b[1;34m"
#define BOLD_MAGENTA "\x1b[1;35m"
#define BOLD_CYAN	 "\x1b[1;36m"
#define BOLD_WHITE	 "\x1b[1;37m"

#define FILE_COLOR	RESET
#define ERROR_COLOR RED
#define ACCENT		YELLOW

#define PROMPT_CHAR "*"

/* FUNCTIONS */
char *get_line(char *prompt);
char *generate_prompt(char *pwd);

#endif // PROMPT_H
