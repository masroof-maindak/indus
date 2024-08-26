#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>

struct USER_INFO {
	uid_t uid;
	char *name;
	char *home;
	char *trashDir;
};

#if defined(PATH_MAX) && PATH_MAX > 1000
#define PATH_MAXL PATH_MAX
#else
#define PATH_MAXL 1024
#endif

char *get_pwd();
char *copy_string(char *str);
char *expand_tilde(char *dir);
char **parse_input(char *input);

void ensure_trash_dir_exists();
void init_user_info(struct USER_INFO *);

#endif // UTILS_H
