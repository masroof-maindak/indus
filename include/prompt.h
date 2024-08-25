#ifndef PROMPT_H
#define PROMPT_H

#define SHORTEN_HOME 1
#define SHORTEN_PWD	 0
/* #define ADD_GIT_BRANCH		  1 */
/* #define GIT_BRANCH_BUFFER_LEN 256 */

/* COLOURS */
#define COL_RED		"\x1b[31m"
#define COL_GREEN	"\x1b[32m"
#define COL_YELLOW	"\x1b[33m"
#define COL_BLUE	"\x1b[34m"
#define COL_MAGENTA "\x1b[35m"
#define COL_CYAN	"\x1b[36m"
#define COL_RESET	"\x1b[0m"
#define COL_WHITE    "\x1b[37m"

#define BOLD_COL_RED      "\x1b[1;31m"
#define BOLD_COL_GREEN    "\x1b[1;32m"
#define BOLD_COL_YELLOW   "\x1b[1;33m"
#define BOLD_COL_BLUE     "\x1b[1;34m"
#define BOLD_COL_MAGENTA  "\x1b[1;35m"
#define BOLD_COL_CYAN     "\x1b[1;36m"
#define BOLD_COL_WHITE    "\x1b[1;37m"

#define DIR_COLOR   BOLD_COL_BLUE 
#define FILE_COLOR  COL_WHITE     
#define ERROR_COLOR COL_RED       
#define RESET_COLOR COL_RESET 

#define ACCENT		COL_YELLOW
#define PROMPT_CHAR "*"

/* FUNCTIONS */
char *get_line(char *prompt);
char *generate_prompt(char *pwd);

#endif // PROMPT_H
