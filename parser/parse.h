#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_TOKEN_LENGTH 256
#define SUCCESS_CODE 0
#define FAILURE_CODE 1

typedef enum {
    NO_OP,
    AND,
    OR,
    PIPE,
    OVERWRITE,
    APPEND
} Operator;

typedef struct {
    char **tokens;
    int count;
    int capacity;
} TokenArray;

int execute_command(char **tokens, int count);
void init_token_array(TokenArray *tokenArr);
void add_token(TokenArray *tokenArr, const char *token);
void free_token_array(TokenArray *tokenArr);
void tokenize_input(const char *input, TokenArray *tokenArr);
void parse_and_execute(TokenArray *tokenArr);

#endif // PARSE_H
