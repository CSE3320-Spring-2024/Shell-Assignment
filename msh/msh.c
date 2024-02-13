#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <dirent.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 32  


// Function to parse input line into constituent pieces
int parse_input(char *input, char **args) {
    int count = 0;
    char *token;

    // Tokenize the input using space as delimiter
    token = strtok(input, " \t\n");
    while (token != NULL && count < MAX_NUM_ARGUMENTS - 1) {
        args[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[count] = NULL; // Null-terminate the argument list
    return count;
}

int main(int argc, char *argv[]) {

    char error_message[30] = "An error has occurred\n";           // error message to print 

    char *input = NULL;
    size_t input_size = 0;
    ssize_t input_length;
    char *args[MAX_NUM_ARGUMENTS];
    int arg_count;

    // Check if batch mode is enabled
    FILE *input_file = NULL;
    if (argc > 1) {
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
          write(STDERR_FILENO, error_message, strlen(error_message)); 
          exit(0);
        }
    }

    while (1) 
    {
        if (input_file)     // Read commands from file in batch mode
        {    
            input_length = getline(&input, &input_size, input_file);
            if (input_length == -1) {
                break; // Exit loop when end of file is reached
            }
            // Remove newline character from input
            input[input_length - 1] = '\0';
        } 
        
        else                 // Prompt for input in interactive mode
        {
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
        if (strcmp(args[0], "exit") == 0)          // Exit command
        {
            exit(0);
        } 

        else if (strcmp(args[0], "cd") == 0)        // CD command
        {
            if (arg_count != 2) 
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
            } 
            else
            { 
              if (chdir(args[1]) == -1) 
              {
                write(STDERR_FILENO, error_message, strlen(error_message));
              }
            } 
        }



        pid_t child_pid = fork();         // Fork a child process to execute the command
        int status;
        if (child_pid == -1)              // Checks if child process fails
        {
           write(STDERR_FILENO, error_message, strlen(error_message)); 
           exit(0);
        } 
        else if (child_pid == 0)          // Child process
        {
           if ( execvp(args[0], args) == -1 )        
           {
              write(STDERR_FILENO, error_message, strlen(error_message)); 
              exit(0);
           }
        } 
        else                              // Parent process waits for child to complete
        {
            waitpid(child_pid, &status, 0 );   
            fflush( NULL ); 
        }
    }

    // Free allocated memory
    free(input);
    if (input_file) 
    {
        fclose(input_file);
    }

    return 0;
}
