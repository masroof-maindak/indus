#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

struct USER_INFO {
	uid_t uid;
	char *name;
	char *home;
};

char **parse_input(char *input);
char *copy_string(char *str);
void init_user_info(struct USER_INFO *);

#endif // UTILS_H
