#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Constants
#define MAX_TOKEN_LENGTH 256
#define SUCCESS_CODE 0
#define FAILURE_CODE 1


typedef enum {
    NO_OPERATOR,
    AND_OPERATOR,
    OR_OPERATOR,
    PIPE_OPERATOR,
    REDIRECT_OPERATOR_OVERWRITE,
    REDIRECT_OPERATOR_APPEND
} Operator;

typedef struct {
    char **tokens;
    int count;
    int capacity;
} TokenArray;


// Function declarations
void init_token_array(TokenArray *token_array);
void add_token(TokenArray *token_array, const char *token);
void free_token_array(TokenArray *token_array);
void tokenize_input(const char *input, TokenArray *token_array);
int execute_command(char **tokens, int count);
void parse_and_execute(TokenArray *token_array);

#endif // PARSE_H
