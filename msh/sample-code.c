#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_COMMAND_SIZE 1024
#define MAX_ARGUMENTS 64

// Function to parse input line into constituent pieces
int parse_input(char *input, char **args) {
    int count = 0;
    char *token;

    // Tokenize the input using space as delimiter
    token = strtok(input, " \t\n");
    while (token != NULL && count < MAX_ARGUMENTS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[count] = NULL; // Null-terminate the argument list
    return count;
}

// Built-in exit command
void exit_shell() {
    exit(0);
}

// Built-in cd command
void change_directory(char *directory) {
    if (directory == NULL) {
        fprintf(stderr, "cd: missing directory\n");
    } else if (chdir(directory) != 0) {
        perror("cd");
    }
}

int main(int argc, char *argv[]) {
    char *input = NULL;
    size_t input_size = 0;
    ssize_t input_length;
    char *args[MAX_ARGUMENTS];
    int arg_count;

    // Check if batch mode is enabled
    FILE *input_file = NULL;
    if (argc > 1) {
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
    }

    while (1) {
        if (input_file) {
            // Read commands from file in batch mode
            input_length = getline(&input, &input_size, input_file);
            if (input_length == -1) {
                break; // Exit loop when end of file is reached
            }
            // Remove newline character from input
            input[input_length - 1] = '\0';
        } else {
            // Prompt for input in interactive mode
            printf("msh> ");
            input_length = getline(&input, &input_size, stdin);
            if (input_length == -1) {
                break; // Exit loop when end of file or error occurs
            }
            // Remove newline character from input
            input[input_length - 1] = '\0';
        }

        // Parse input into arguments
        arg_count = parse_input(input, args);
        if (arg_count == 0) {
            continue; // Skip empty input
        }

        // Check for built-in commands
        if (strcmp(args[0], "exit") == 0) {
            exit_shell(); // Exit the shell
        } else if (strcmp(args[0], "cd") == 0) {
            change_directory(args[1]); // Change directory
            continue; // Skip executing external commands
        }

        // Fork a child process to execute the command
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process
            wait(NULL); // Wait for child to complete
        }
    }

    // Free allocated memory
    free(input);
    if (input_file) {
        fclose(input_file);
    }

    return 0;
}
