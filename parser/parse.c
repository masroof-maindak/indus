#include "parse.h"
#include <stdbool.h>

void init_token_array(TokenArray *token_array) {
	token_array->tokens	  = malloc(10 * sizeof(char *)); // Initial capacity
	token_array->count	  = 0;
	token_array->capacity = 10;
}

void add_token(TokenArray *token_array, const char *token) {
	if (token_array->count >= token_array->capacity) {
		token_array->capacity *= 2;
		token_array->tokens = realloc(token_array->tokens,
									  token_array->capacity * sizeof(char *));
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
				if (*(ptr + 1) != '>') {
					add_token(token_array, ">");
				} else {
					add_token(token_array, ">>");
				}
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

void print_command_tokens(const TokenArray *command_tokens) {
	printf("Command Tokens:\n");
	for (int i = 0; i < command_tokens->count; i++) {
		printf("Token[%d]: %s\n", i, command_tokens->tokens[i]);
	}
}

Operator get_operator_type(const char *token) {
    if (strcmp(token, "&&") == 0) return AND_OPERATOR;
    if (strcmp(token, "||") == 0) return OR_OPERATOR;
    if (strcmp(token, "|") == 0) return PIPE_OPERATOR;
    if (strcmp(token, ">") == 0) return REDIRECT_OPERATOR_OVERWRITE;
    if (strcmp(token, ">>") == 0) return REDIRECT_OPERATOR_APPEND;
    return NO_OPERATOR;
}
bool is_first_operator_redirect(TokenArray *token_array) {
	for (int i = 0; i < token_array->count; i++) {
		Operator op_type = get_operator_type(token_array->tokens[i]);
		if (op_type != NO_OPERATOR) {
			// Check if the first encountered operator is either > or >>
			if (op_type == REDIRECT_OPERATOR_OVERWRITE ||
				op_type == REDIRECT_OPERATOR_APPEND) {
				return true;
			}
			// If the first encountered operator is not a redirection operator,
			// return false
			return false;
		}
	}
	// If no operators are found, return false
	return false;
}


void parse_and_execute(TokenArray *token_array) {
	int status_code = SUCCESS_CODE;
	int redirect_idx = -1;
	Operator last_operator = NO_OPERATOR;
	bool op_check = false;
	bool redir_check = is_first_operator_redirect(token_array);

	for (int i = 0; i < token_array->count; i++) {
		const char *token = token_array->tokens[i];

		// Check for operators
		if (strcmp(token, "&&") == 0) {
			last_operator = AND_OPERATOR;
			op_check = true;
		} else if (strcmp(token, "||") == 0) {
			last_operator = OR_OPERATOR;
			op_check = true;
		} else if (strcmp(token, "|") == 0) {
			last_operator = PIPE_OPERATOR;
			op_check = true;
		} else if (strcmp(token, ">") == 0) {
			last_operator = REDIRECT_OPERATOR_OVERWRITE;
			op_check = true;
		} else if (strcmp(token, ">>") == 0) {
			last_operator = REDIRECT_OPERATOR_APPEND;
			op_check = true;
		} else {

			// Collect tokens for the command
			TokenArray command_tokens;
			init_token_array(&command_tokens);
			bool flag = false;
			int idx = 0;

			if (op_check) { // check if next operator is redirection
				op_check = false;
				if (last_operator != REDIRECT_OPERATOR_APPEND &&
					last_operator != REDIRECT_OPERATOR_OVERWRITE) {
					for (int k = i; k < token_array->count; k++) {
						const char *tk = token_array->tokens[k];
						if (strcmp(tk, "&&") != 0 && strcmp(tk, "||") != 0 &&
							strcmp(tk, "|") != 0) {
							if (strcmp(tk, ">") == 0 ||
								strcmp(tk, ">>") == 0) { // if it is then handle it
								flag = true;
								while (k < token_array->count &&
									strcmp(token_array->tokens[k], "&&") != 0 &&
									strcmp(token_array->tokens[k], "||") != 0 &&
									strcmp(token_array->tokens[k], "|") != 0) {
									add_token(&command_tokens, token_array->tokens[k]);
									k++;
									idx = k;
								}
								break;

							} else {
								add_token(&command_tokens, tk);
							}
						} else {
							break; // if it is any other operator ls -l && cd
								   // /usr || echo 'done' | then break.
						}
					}
				} else { // this is the case if the very first operator in our
						 // string is either > or >>
					int k = i; // since i now points to one token ahead of > or >>
					flag = true;
					redir_check = false;
					while (k > 0 && strcmp(token_array->tokens[k], "&&") != 0 &&
						   strcmp(token_array->tokens[k], "||") != 0 &&
						   strcmp(token_array->tokens[k], "|") != 0) {
						k--; // make k point to where the command starts
					}
					while (k < token_array->count &&
						   strcmp(token_array->tokens[k], "&&") != 0 &&
						   strcmp(token_array->tokens[k], "||") != 0 &&
						   strcmp(token_array->tokens[k], "|") != 0) {
						add_token(&command_tokens, token_array->tokens[k]);
						k++;
						idx = k;
					}
				}
			}

			int j = i;

			if (flag) {
				i = idx;
				status_code = execute_command(command_tokens.tokens, command_tokens.count);
				free_token_array(&command_tokens);
			} else if (!redir_check) {
				free_token_array(&command_tokens);
				init_token_array(&command_tokens);
				while (j < token_array->count &&
					   strcmp(token_array->tokens[j], "&&") != 0 &&
					   strcmp(token_array->tokens[j], "||") != 0 &&
					   strcmp(token_array->tokens[j], "|") != 0 &&
					   strcmp(token_array->tokens[j], ">") != 0 &&
					   strcmp(token_array->tokens[j], ">>") != 0) {
					add_token(&command_tokens, token_array->tokens[j]);
					j++;
				}
				i = j - 1; // Adjust i to the last token processed

				// Execute the command based on the last operator
				if ((last_operator == AND_OPERATOR && status_code != SUCCESS_CODE) ||
					(last_operator == OR_OPERATOR && status_code == SUCCESS_CODE)) {
					printf("Skipping command due to short-circuiting\n");
				} else {
					if (last_operator == PIPE_OPERATOR) {
						// Handle piping (this is a simplified example; actual
						// piping is more complex)
						printf("Piping not fully implemented in this example.\n");
						status_code = execute_command(command_tokens.tokens,
													  command_tokens.count);
					} else {
						// Simple command execution
						status_code = execute_command(command_tokens.tokens,
													  command_tokens.count);
					}
				}

				free_token_array(&command_tokens);
			}
			
		}
	}
}


// int main() {
// 	// Test Case 1: Basic AND, OR, and PIPE operations
// 	const char *input1 = "grep 'done' > output.txt";
// 	TokenArray token_array1;
// 	init_token_array(&token_array1);
// 	tokenize_input(input1, &token_array1);
// 	printf("Test Case 1:\n");
// 	// for (int i = 0; i < token_array1.count; i++) {
// 	// 	printf("Token: %s\n", token_array1.tokens[i]);
// 	// }
// 	parse_and_execute(&token_array1);
// 	free_token_array(&token_array1);

// 	// Test Case 2: Multiple AND and OR operations
// 	const char *input2 = "mkdir test && cd test && touch file.txt || echo "
// 						 "'Failed to create file'";
// 	TokenArray token_array2;
// 	init_token_array(&token_array2);
// 	tokenize_input(input2, &token_array2);
// 	printf("\nTest Case 2:\n");
// 	// for (int i = 0; i < token_array2.count; i++) {
// 	// 	printf("Token: %s\n", token_array2.tokens[i]);
// 	// }
// 	parse_and_execute(&token_array2);
// 	free_token_array(&token_array2);

// 	// Test Case 3: Complex command with multiple pipes and redirection
// 	const char *input3 =
// 		"cat file.txt | grep 'search term' | sort > sorted.txt";
// 	TokenArray token_array3;
// 	init_token_array(&token_array3);
// 	tokenize_input(input3, &token_array3);
// 	printf("\nTest Case 3:\n");
// 	// for (int i = 0; i < token_array3.count; i++) {
// 	// 	printf("Token: %s\n", token_array3.tokens[i]);
// 	// }
// 	parse_and_execute(&token_array3);
// 	free_token_array(&token_array3);

// 	// Test Case 4: Command with special characters in arguments
// 	const char *input4 =
// 		"echo \"Hello, World!\" && echo 'Another Command' | grep 'Another'";
// 	TokenArray token_array4;
// 	init_token_array(&token_array4);
// 	tokenize_input(input4, &token_array4);
// 	printf("\nTest Case 4:\n");
// 	// for (int i = 0; i < token_array4.count; i++) {
// 	// 	printf("Token: %s\n", token_array4.tokens[i]);
// 	// }
// 	parse_and_execute(&token_array4);
// 	free_token_array(&token_array4);

// 	return 0;
// }


int main() {
    // Test Case 1
    const char *input1 = "grep 'done' > output.txt";
    TokenArray token_array1;
    init_token_array(&token_array1);
    tokenize_input(input1, &token_array1);
    printf("Test Case 1:\n");
    parse_and_execute(&token_array1);
    free_token_array(&token_array1);

    // Test Case 2: echo "hello" >> file1.txt > file2.txt
    const char *input2 = "echo \"hello\" >> file1.txt > file2.txt";
    TokenArray token_array2;
    init_token_array(&token_array2);
    tokenize_input(input2, &token_array2);
    printf("Test Case 2:\n");
    parse_and_execute(&token_array2);
    free_token_array(&token_array2);

    // Test Case 3: echo "hello" > file1.txt && cd usr/
    const char *input3 = "echo \"hello\" > file1.txt && cd usr/";
    TokenArray token_array3;
    init_token_array(&token_array3);
    tokenize_input(input3, &token_array3);
    printf("Test Case 3:\n");
    parse_and_execute(&token_array3);
    free_token_array(&token_array3);

    // Test Case 4: ls -l | grep 'file' > output.txt
    const char *input4 = "ls -l | grep 'file' > output.txt";
    TokenArray token_array4;
    init_token_array(&token_array4);
    tokenize_input(input4, &token_array4);
    printf("Test Case 4:\n");
    parse_and_execute(&token_array4);
    free_token_array(&token_array4);

    // Test Case 5: cat file.txt | sort | uniq > sorted.txt
    const char *input5 = "cat file.txt | sort | uniq > sorted.txt";
    TokenArray token_array5;
    init_token_array(&token_array5);
    tokenize_input(input5, &token_array5);
    printf("Test Case 5:\n");
    parse_and_execute(&token_array5);
    free_token_array(&token_array5);

    return 0;
}