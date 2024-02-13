#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 32

int main() {
    // Example command line input (tokenized)
    char *command_line = "ls -l /tmp";
    
    // Parse the command line input into tokens
    char *token;
    char *tokens[MAX_TOKENS + 1]; // Add 1 for NULL terminator
    int token_count = 0;
    
    token = strtok(command_line, " ");
    while (token != NULL && token_count < MAX_TOKENS) {
        tokens[token_count++] = token;
        token = strtok(NULL, " ");
    }
    tokens[token_count] = NULL; // Set the last element to NULL
    
    // Create the args array
    char *args[token_count + 1]; // Add 1 for NULL terminator
    for (int i = 0; i < token_count; i++) {
        args[i] = tokens[i];
    }
    args[token_count] = NULL; // NULL terminate the array
    
    // Print the args array for demonstration
    printf("Args array:\n");
    for (int i = 0; args[i] != NULL; i++) {
        printf("args[%d] = %s\n", i, args[i]);
    }

    return 0;
}
