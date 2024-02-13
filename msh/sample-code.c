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

#define WHITESPACE " \t\n"
#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 32
#define ERROR_MESSAGE "An error has occurred\n"
#define MAX_PATH_LENGTH 1024

void execute_exit() {
    exit(0);
}

void execute_cd(char *dir) {
    if (dir == NULL) {
        write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE));
        return;
    }

    if (chdir(dir) != 0) {
        perror("cd");
    }
}

void execute_path(char *new_path) {
    if (new_path == NULL) {
        char cwd[MAX_PATH_LENGTH];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Current directory: %s\n", cwd);
        } else {
            perror("getcwd");
        }
        return;
    }

    if (setenv("PATH", new_path, 1) != 0) {
        perror("path");
    }
}

void execute_command(char **args) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        if (execvp(args[0], args) < 0) {
            perror("execvp");
            exit(1);
        }
    } else {
        waitpid(pid, NULL, 0);
    }
}

void execute_command_with_redirection(char **args, char *output_file) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0) {
            perror("open");
            exit(1);
        }

        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2");
            exit(1);
        }

        if (execvp(args[0], args) < 0) {
            perror("execvp");
            exit(1);
        }

        close(fd);
    } else {
        waitpid(pid, NULL, 0);
    }
}

int main(int argc, char *argv[]) {
    char *command_string = (char *)malloc(MAX_COMMAND_SIZE);

    while (1) {
        printf("msh> ");
        while (!fgets(command_string, MAX_COMMAND_SIZE, stdin));

        char *token[MAX_NUM_ARGUMENTS];
        int token_count = 0;
        char *argument_pointer;
        char *working_string = strdup(command_string);
        char *head_ptr = working_string;

    while ( ( (argument_pointer = strsep(&working_string, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
        // grabs only the non-zero values, tokenizes each command seperately
        if (strlen(argument_pointer) > 0)                                                 
        {
            token[token_count] = strndup( argument_pointer, MAX_COMMAND_SIZE );
            token_count++;
        }
    }

        if (token_count > 0) {
            if (strcmp(token[0], "exit") == 0) {
                execute_exit();
            } else if (strcmp(token[0], "cd") == 0) {
                execute_cd(token[1]);
            } else if (strcmp(token[0], "path") == 0) {
                execute_path(token[1]);
            } else {
                // Check for redirection
                int i;
                char *output_file = NULL;
                for (i = 0; i < token_count; i++) {
                    if (strcmp(token[i], ">") == 0) {
                        // Redirection detected
                        if (i + 1 < token_count) {
                            output_file = token[i + 1];
                            token[i] = NULL; // Remove redirection operator from command
                            break;
                        } else {
                            write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE));
                            break;
                        }
                    }
                }

                if (output_file != NULL) {
                    execute_command_with_redirection(token, output_file);
                } else {
                    execute_command(token);
                }
            }
        }

        for (int i = 0; i < token_count; i++) {
            free(token[i]);
        }

        free(head_ptr);
    }

    free(command_string);
    return 0;
}
