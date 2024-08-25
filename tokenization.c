#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 256
#define SUCCESS_CODE 0
#define FAILURE_CODE 1

typedef struct {
    char **tokens;
    int count;
    int capacity;
} TokenArray;

void init_token_array(TokenArray *token_array) {
    token_array->tokens = malloc(10 * sizeof(char*)); // Initial capacity
    token_array->count = 0;
    token_array->capacity = 10;
}

void add_token(TokenArray *token_array, const char *token) {
    if (token_array->count >= token_array->capacity) {
        token_array->capacity *= 2;
        token_array->tokens = realloc(token_array->tokens, token_array->capacity * sizeof(char*));
    }
    token_array->tokens[token_array->count] = strdup(token);
    token_array->count++;
}

void free_token_array(TokenArray *token_array) {
    for (int i = 0; i < token_array->count; i++) {
        free(token_array->tokens[i]);
    }
    free(token_array->tokens);
}

void tokenize_input(const char *input, TokenArray *token_array) {
    const char *ptr = input;
    char buffer[MAX_TOKEN_LENGTH];
    int char_idx = 0;

    while (*ptr != '\0') {
        if (isspace(*ptr) || *ptr == '&' || *ptr == '|' || *ptr == '>') {
            if (char_idx > 0) {
                buffer[char_idx] = '\0';
                add_token(token_array, buffer);
                char_idx = 0;
            }
            if (*ptr == '&' && *(ptr + 1) == '&') {
                add_token(token_array, "&&");
                ptr++;
            } else if (*ptr == '|' && *(ptr + 1) == '|') {
                add_token(token_array, "||");
                ptr++;
            } else if (*ptr == '|') {
                add_token(token_array, "|");
            } else if (*ptr == '>') {
                add_token(token_array, ">");
            }
        } else {
            buffer[char_idx] = *ptr;
            char_idx++;
        }
        ptr++;
    }

    if (char_idx > 0) {
        buffer[char_idx] = '\0';
        add_token(token_array, buffer);
    }
}

int execute_command(char **tokens, int count) {
    // Simulated command execution
    printf("Executing command: ");
    for (int i = 0; i < count; i++) {
        printf("%s ", tokens[i]);
    }
    printf("\n");
    return SUCCESS_CODE; // Simulate success
}

void parse_and_execute(TokenArray *token_array) {
    int status_code = SUCCESS_CODE;
    char last_operator = 0;

    for (int i = 0; i < token_array->count; i++) {
        const char *token = token_array->tokens[i];
        
        if (strcmp(token, "&&") == 0) {
            last_operator = '&';
        } else if (strcmp(token, "||") == 0) {
            last_operator = 'o';
        } else if (strcmp(token, "|") == 0) {
            last_operator = '|';
        } else if (strcmp(token, ">") == 0) {
            last_operator = '>';
        } else {
            // Collect tokens for the command
            TokenArray command_tokens;
            init_token_array(&command_tokens);
            int j = i;
            while (j < token_array->count && strcmp(token_array->tokens[j], "&&") != 0 &&
                   strcmp(token_array->tokens[j], "||") != 0 && strcmp(token_array->tokens[j], "|") != 0 &&
                   strcmp(token_array->tokens[j], ">") != 0) {
                add_token(&command_tokens, token_array->tokens[j]);
                j++;
            }
            i = j - 1; // Adjust i to the last token processed

            // Execute the command
            if ((last_operator == '&' && status_code != SUCCESS_CODE) ||
                (last_operator == 'o' && status_code == SUCCESS_CODE)) {
                printf("Skipping command due to short-circuiting\n");
            } else {
                status_code = execute_command(command_tokens.tokens, command_tokens.count);
            }

            free_token_array(&command_tokens);
        }
    }
}

int main() {
    // Test Case 1: Basic AND, OR, and PIPE operations
    const char *input1 = "ls -l && cd /usr || echo 'done' | grep 'done' > output.txt";
    TokenArray token_array1;
    init_token_array(&token_array1);
    tokenize_input(input1, &token_array1);
    printf("Test Case 1:\n");
    for (int i = 0; i < token_array1.count; i++) {
        printf("Token: %s\n", token_array1.tokens[i]);
    }
    parse_and_execute(&token_array1);
    free_token_array(&token_array1);

    // Test Case 2: Multiple AND and OR operations
    const char *input2 = "mkdir test && cd test && touch file.txt || echo 'Failed to create file'";
    TokenArray token_array2;
    init_token_array(&token_array2);
    tokenize_input(input2, &token_array2);
    printf("\nTest Case 2:\n");
    for (int i = 0; i < token_array2.count; i++) {
        printf("Token: %s\n", token_array2.tokens[i]);
    }
    parse_and_execute(&token_array2);
    free_token_array(&token_array2);

    // Test Case 3: Complex command with multiple pipes and redirection
    const char *input3 = "cat file.txt | grep 'search term' | sort > sorted.txt";
    TokenArray token_array3;
    init_token_array(&token_array3);
    tokenize_input(input3, &token_array3);
    printf("\nTest Case 3:\n");
    for (int i = 0; i < token_array3.count; i++) {
        printf("Token: %s\n", token_array3.tokens[i]);
    }
    parse_and_execute(&token_array3);
    free_token_array(&token_array3);

    // Test Case 4: Command with special characters in arguments
    const char *input4 = "echo \"Hello, World!\" && echo 'Another Command' | grep 'Another'";
    TokenArray token_array4;
    init_token_array(&token_array4);
    tokenize_input(input4, &token_array4);
    printf("\nTest Case 4:\n");
    for (int i = 0; i < token_array4.count; i++) {
        printf("Token: %s\n", token_array4.tokens[i]);
    }
    parse_and_execute(&token_array4);
    free_token_array(&token_array4);

    return 0;
}
