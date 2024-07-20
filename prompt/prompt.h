#ifndef PROMPT_H
#define PROMPT_H

#define SHORTEN_HOME 1
#define SHORTEN_PWD 1
#define ADD_GIT_BRANCH 1

char* populate_pwd(char* pwd);
char* generate_prompt(char* pwd, char* username);

#endif // PROMPT_H
