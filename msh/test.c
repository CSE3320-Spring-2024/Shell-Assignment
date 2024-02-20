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
#include <fcntl.h>

#define MAX_PATH_LENGTH 256
#define MAX_COMMAND_LENGTH 255
#define MAX_NUM_ARGUMENTS 32

char *PATH[] = {"/bin", "/usr/bin", "/usr/local/bin", "./"};

int main()
{
    char input[MAX_COMMAND_LENGTH];
    char *command;
    char *arguments[MAX_NUM_ARGUMENTS];

    while (1)
    {
        printf("msh> ");
        fgets(input, sizeof(input), stdin);

        // Remove newline character from input
        input[strcspn(input, "\n")] = 0;

        // Tokenize input to separate command and arguments
        command = strtok(input, " ");
        if (command == NULL)
            continue; // Empty command line

        arguments[0] = command;
        int arg_count = 1;
        while (arg_count < MAX_NUM_ARGUMENTS)
        {
            arguments[arg_count] = strtok(NULL, " ");
            if (arguments[arg_count] == NULL)
                break;
            arg_count++;
        }
        arguments[arg_count] = NULL; // Null-terminate argument list







        // Iterate over each directory in PATH
        int i;
        for (i = 0; i < sizeof(PATH) / sizeof(PATH[0]); i++)
        {
            // Construct full path to command
            char full_path[MAX_PATH_LENGTH];////////////////////////////////////
            snprintf(full_path, sizeof(full_path), "%s/%s", PATH[i], command);/////////

            // Check if the command is executable
            if (access(full_path, X_OK) == 0)
            {
                // Execute command
                int pid = fork();
                if (pid == 0)
                {
                    execv(full_path, arguments);
                    // If execv returns, an error occurred
                    perror("execv");
                    exit(EXIT_FAILURE);
                }
//-------------------------------------------------------------------------------    
                else if (pid > 0)// Parent process waits for child to complete
                {
                    
                    wait(NULL);
                    break;
                }
                else// Error forking child process
                {
                    
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
            }
        }
//---------------------------------------------------------------------------------
        if (i == sizeof(PATH) / sizeof(PATH[0]))
        {
            // Command not found
            fprintf(stderr, "Command not found: %s\n", command);
        }
    }







    return 0;
}
