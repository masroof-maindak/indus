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

// Enum to represent the different types of operators.
typedef enum {
    NO_OP,      // No operator
    AND,        // &&
    OR,         // ||
    PIPE,       // |
    OVERWRITE,  // >
    APPEND      // >>
} Operator;

// Struct to hold an array of tokens.
typedef struct {
    char **tokens;    // Array of token strings
    int count;        // Number of tokens currently in the array
    int capacity;     // Capacity of the token array
} TokenArray;

// Function declarations

// Initializes a TokenArray struct.
void init_token_array(TokenArray *tokenArr);

// Adds a token to a TokenArray struct.
void add_token(TokenArray *tokenArr, const char *token);

// Frees the memory allocated for a TokenArray struct.
void free_token_array(TokenArray *tokenArr);

// Tokenizes the input string into an array of tokens.
void tokenize_input(const char *input, TokenArray *tokenArr);

// Parses and executes the commands stored in the TokenArray struct.
void parse_and_execute(TokenArray *tokenArr);

// Executes a command based on the array of tokens.
int execute_command(char **tokens, int count);

// Identifies the type of operator given a token string.
Operator identify_operator(const char *token);

// Checks if a token is an operator.
bool is_operator(const char *token);

// Utility function to print the tokens in a TokenArray struct (for debugging purposes).
void print_command_tokens(const TokenArray *cmdTokens);

#endif // PARSE_H
