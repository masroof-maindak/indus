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
char *expand_tilde(char *dir);
char **parse_input(char *input);
char *copy_string(const char *str);

void ensure_trash_dir_exists();
void init_user_info(struct USER_INFO *);

#endif // UTILS_H
