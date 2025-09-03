#ifndef UTILS_H
#define UTILS_H

#include <linux/limits.h>
#include <sys/types.h>

struct USER_INFO {
	uid_t uid;
	char *name;
	char *home;
	char *trashDir;
};

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

char *get_pwd();
char *expand_tilde(char *dir);
char **tokenise_input(char *input);
char *copy_string(const char *str);

void ensure_trash_dir_exists();
void init_user_info(struct USER_INFO *);

#endif // UTILS_H
