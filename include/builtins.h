#ifndef BUILTINS_H
#define BUILTINS_H

int num_builtins();

extern char *builtins_str[];
extern int (*builtins_fn[])(char **args);

int indus_ls(char **args);
int indus_cd(char **args);
int indus_rm(char **args);
int indus_help(char **args);
int indus_exit(char **args);
int indus_mkdir(char **args);
int indus_clear(char **args);
int indus_whoami(char **args);

#endif // BUILTINS_H
