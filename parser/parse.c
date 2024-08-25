#include "parse.h"
#include "../include/utils.h"

void init_token_array(TokenArray *tArr) {
	tArr->tokens   = malloc(10 * sizeof(char *));
	tArr->count	   = 0;
	tArr->capacity = 10;
}

void add_token(TokenArray *tArr, const char *token) {
	if (tArr->count >= tArr->capacity) {
		tArr->capacity *= 2;
		tArr->tokens = realloc(tArr->tokens, tArr->capacity * sizeof(char *));
	}

	tArr->tokens[tArr->count] = strdup(token);
	tArr->count++;
}

void free_token_array(TokenArray *tArr) {
	for (int i = 0; i < tArr->count; i++)
		free(tArr->tokens[i]);

	free(tArr->tokens);
}

void tokenize_input(const char *input, TokenArray *tArr) {
	const char *ptr = input;
	char buffer[MAX_TOKEN_LENGTH];
	int charIdx = 0;

	while (*ptr != '\0') {
		if (isspace(*ptr) || *ptr == '&' || *ptr == '|' || *ptr == '>') {
			if (charIdx > 0) {
				buffer[charIdx] = '\0';
				add_token(tArr, buffer);
				charIdx = 0;
			}

			if (*ptr == '&' && *(ptr + 1) == '&') {
				add_token(tArr, "&&");
				ptr++;
			} else if (*ptr == '|' && *(ptr + 1) == '|') {
				add_token(tArr, "||");
				ptr++;
			} else if (*ptr == '|') {
				add_token(tArr, "|");
			} else if (*ptr == '>') {
				if (*(ptr + 1) != '>')
					add_token(tArr, ">");
				else
					add_token(tArr, ">>");
			}
		} else {
			buffer[charIdx] = *ptr;
			charIdx++;
		}
		ptr++;
	}

	if (charIdx > 0) {
		buffer[charIdx] = '\0';
		add_token(tArr, buffer);
	}
}

int execute_command(char **tokens, int count) {
	// Simulated command execution
	printf("Executing command: ");
	for (int i = 0; i < count; i++)
		printf("%s ", tokens[i]);
	printf("\n");
	return SUCCESS_CODE;
}

void print_command_tokens(const TokenArray *cmdTokens) {
	puts("Command Tokens:");
	for (int i = 0; i < cmdTokens->count; i++)
		printf("Token[%d]: %s\n", i, cmdTokens->tokens[i]);
}

Operator get_operator_type(const char *token) {
	if (strcmp(token, "&&") == 0)
		return AND;
	if (strcmp(token, "||") == 0)
		return OR;
	if (strcmp(token, "|") == 0)
		return PIPE;
	if (strcmp(token, ">") == 0)
		return OVERWRITE;
	if (strcmp(token, ">>") == 0)
		return APPEND;
	return NO_OP;
}

bool is_first_operator_redirect(TokenArray *tArr) {
	for (int i = 0; i < tArr->count; i++) {
		Operator opType = get_operator_type(tArr->tokens[i]);

		if (opType != NO_OP) {
			if (opType == OVERWRITE || opType == APPEND)
				return true;
			return false;
		}
	}

	return false;
}

void parse_and_execute(TokenArray *tArr) {
	int status		= SUCCESS_CODE;
	int redirectIdx = -1;
	Operator lastOp = NO_OP;
	bool opCheck	= false;
	bool redirCheck = is_first_operator_redirect(tArr);

	for (int i = 0; i < tArr->count; i++) {
		const char *token = tArr->tokens[i];

		// Check for operators
		if (strcmp(token, "&&") == 0) {
			lastOp	= AND;
			opCheck = true;
		} else if (strcmp(token, "||") == 0) {
			lastOp	= OR;
			opCheck = true;
		} else if (strcmp(token, "|") == 0) {
			lastOp	= PIPE;
			opCheck = true;
		} else if (strcmp(token, ">") == 0) {
			lastOp	= OVERWRITE;
			opCheck = true;
		} else if (strcmp(token, ">>") == 0) {
			lastOp	= APPEND;
			opCheck = true;
		} else {

			// Collect tokens for the command
			TokenArray cmdTokens;
			init_token_array(&cmdTokens);
			bool flag = false;
			int idx	  = 0;

			if (opCheck) { // check if next operator is redirection
				opCheck = false;
				if (lastOp != APPEND && lastOp != OVERWRITE) {
					for (int k = i; k < tArr->count; k++) {
						const char *tk = tArr->tokens[k];
						if (strcmp(tk, "&&") != 0 && strcmp(tk, "||") != 0 &&
							strcmp(tk, "|") != 0) {
							if (strcmp(tk, ">") == 0 || strcmp(tk, ">>") == 0) {
								flag = true;
								while (k < tArr->count &&
									   strcmp(tArr->tokens[k], "&&") != 0 &&
									   strcmp(tArr->tokens[k], "||") != 0 &&
									   strcmp(tArr->tokens[k], "|") != 0) {
									add_token(&cmdTokens, tArr->tokens[k]);
									k++;
									idx = k;
								}
								break;

							} else {
								add_token(&cmdTokens, tk);
							}
						} else {
							break; // if it is any other operator ls -l && cd
								   // /usr || echo 'done' | then break.
						}
					}
				} else { // this is the case if the very first operator in our
						 // string is either > or >>
					int k	   = i; // since i == one token ahead of > or >>
					flag	   = true;
					redirCheck = false;

					while (k > 0 && strcmp(tArr->tokens[k], "&&") != 0 &&
						   strcmp(tArr->tokens[k], "||") != 0 &&
						   strcmp(tArr->tokens[k], "|") != 0) {

						k--; // make k point to where the command starts
					}

					while (k < tArr->count &&
						   strcmp(tArr->tokens[k], "&&") != 0 &&
						   strcmp(tArr->tokens[k], "||") != 0 &&
						   strcmp(tArr->tokens[k], "|") != 0) {

						add_token(&cmdTokens, tArr->tokens[k]);
						k++;
						idx = k;
					}
				}
			}

			int j = i;

			if (flag) {
				i	   = idx;
				status = execute_command(cmdTokens.tokens, cmdTokens.count);
				free_token_array(&cmdTokens);
			} else if (!redirCheck) {
				free_token_array(&cmdTokens);
				init_token_array(&cmdTokens);
				while (j < tArr->count && strcmp(tArr->tokens[j], "&&") != 0 &&
					   strcmp(tArr->tokens[j], "||") != 0 &&
					   strcmp(tArr->tokens[j], "|") != 0 &&
					   strcmp(tArr->tokens[j], ">") != 0 &&
					   strcmp(tArr->tokens[j], ">>") != 0) {
					add_token(&cmdTokens, tArr->tokens[j]);
					j++;
				}

				i = j - 1; // Adjust i to the last token processed

				// Execute the command based on the last operator
				if ((lastOp == AND && status != SUCCESS_CODE) ||
					(lastOp == OR && status == SUCCESS_CODE)) {
					puts("Skipping command due to short-circuiting");
				} else {
					if (lastOp == PIPE) {
						// Handle piping (this is a simplified example; actual
						// piping is more complex)
						puts("Piping not fully implemented in this example.");
						status =
							execute_command(cmdTokens.tokens, cmdTokens.count);
					} else {
						// Simple command execution
						status =
							execute_command(cmdTokens.tokens, cmdTokens.count);
					}
				}

				free_token_array(&cmdTokens);
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

void run_test(const char *input, const char *test_name) {
	TokenArray tArr;
	init_token_array(&tArr);
	tokenize_input(input, &tArr);
	printf("%s:\n", test_name);
	parse_and_execute(&tArr);
	free_token_array(&tArr);
}

int main() {
	run_test("grep 'done' > output.txt", "Test Case 1: grep with redirection");
	run_test("echo \"hello\" >> file1.txt > file2.txt",
			 "Test Case 2: Double redirection");
	run_test("echo \"hello\" > file1.txt && cd usr/",
			 "Test Case 3: Redirection and AND");
	run_test("ls -l | grep 'file' > output.txt",
			 "Test Case 4: Pipe and redirection");
	run_test("cat file.txt | sort | uniq > sorted.txt",
			 "Test Case 5: Multiple pipes with redirection");

	return 0;
}
