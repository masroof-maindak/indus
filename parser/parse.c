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
					ptr++;
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

Operator identify_operator(const char *token) {
    if (strcmp(token, "&&") == 0) return AND;
    if (strcmp(token, "||") == 0) return OR;
    if (strcmp(token, "|") == 0) return PIPE;
    if (strcmp(token, ">") == 0) return OVERWRITE;
    if (strcmp(token, ">>") == 0) return APPEND;
    return NO_OP;
}

bool is_operator(const char *token) {
    return identify_operator(token) != NO_OP;
}

void parse_and_execute(TokenArray *tArr) {
    int status = SUCCESS_CODE;
    Operator lastOp = NO_OP;
    bool opCheck = false;

    for (int i = 0; i < tArr->count; i++) {
        const char *token = tArr->tokens[i];
        Operator currentOp = identify_operator(token);

        if (currentOp != NO_OP) {
            lastOp = currentOp;
            opCheck = (lastOp == OVERWRITE || lastOp == APPEND);
        } else {
            TokenArray cmdTokens;
            init_token_array(&cmdTokens);
            
            int j = i;
            if (opCheck) {
                add_token(&cmdTokens, tArr->tokens[j - 1]);
                opCheck = false;
            }
            
            while (j < tArr->count && !is_operator(tArr->tokens[j])) {
                add_token(&cmdTokens, tArr->tokens[j]);
                j++;
            }

            i = j - 1; // Adjust i to the last token processed

            if ((lastOp == AND && status != SUCCESS_CODE) ||
                (lastOp == OR && status == SUCCESS_CODE)) {
                puts("Skipping command due to short-circuiting");
            } else {
                if (lastOp == PIPE) {
                    // Handle piping (simplified example)
                    puts("Piping not fully implemented in this example.");
                    status = execute_command(cmdTokens.tokens, cmdTokens.count);
                } else {
                    status = execute_command(cmdTokens.tokens, cmdTokens.count);
                }
            }

            free_token_array(&cmdTokens);
        }
    }
}

void run_test(const char *input, const char *test_name) {
	TokenArray tArr;
	init_token_array(&tArr);
	tokenize_input(input, &tArr);
	printf("%s:\n", test_name);
	parse_and_execute(&tArr);
	free_token_array(&tArr);
}

int main() {
	run_test("grep 'done' >> output.txt", "Test Case 1: grep with redirection");
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
