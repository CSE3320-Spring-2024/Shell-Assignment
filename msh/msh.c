// The MIT License (MIT)
// 
// Copyright (c) 2024 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 1024
#define MAX_ARGS 64
#define DELIMITERS " \t\r\n\a"
#define ERROR_MESSAGE "An error has occurred\n"

// Function prototypes
void loop(FILE *stream);
char *read_line(FILE *stream);
char **split_line(char *line);
int execute(char **args);
int launch(char **args);
int msh_cd(char **args);
int msh_exit(char **args);
int redirect_output(char **args);
void print_error();

char *builtin_str[] = {
    "cd",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &msh_cd,
    &msh_exit
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int main(int argc, char *argv[]) {
    FILE *stream = stdin;

    if (argc > 2) {
        fprintf(stderr, "Usage: %s [batch file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        stream = fopen(argv[1], "r");
        if (!stream) {
            fprintf(stderr, "Error opening file: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    loop(stream);

    if (stream != stdin) {
        fclose(stream);
    }

    return EXIT_SUCCESS;
}

void loop(FILE *stream) {
    char *line;
    char **args;
    int status;

    do {
        if (stream == stdin) {
            printf("msh> ");
            fflush(stdout);
        }

        line = read_line(stream);
        args = split_line(line);
        status = execute(args);

        free(line);
        free(args);
    } while (status);
}

char *read_line(FILE *stream) {
    char *line = NULL;
    size_t bufsize = 0;
    getline(&line, &bufsize, stream);
    return line;
}

char **split_line(char *line) {
    int bufsize = MAX_ARGS, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        print_error();
        exit(EXIT_FAILURE);
    }

    token = strtok(line, DELIMITERS);
    while (token != NULL) {
        tokens[position++] = token;
        if (position >= bufsize) {
            bufsize += MAX_ARGS;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (!tokens) {
                print_error();
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, DELIMITERS);
    }
    tokens[position] = NULL;
    return tokens;
}

int execute(char **args) {
    if (args[0] == NULL) {
        return 1; 
    }

    for (int i = 0; i < num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return launch(args);
}

int launch(char **args) {
    pid_t pid, wpid;
    int status;

    if (redirect_output(args) != 0) {
        return 1; 
    }

    pid = fork();
    if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            print_error();
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        print_error();
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int msh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "msh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("msh");
        }
    }
    return 1;
}

int msh_exit(char **args) {
    return 0;
}

int redirect_output(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0 && args[i + 1] != NULL) {
            args[i] = NULL; // Terminate arguments before the redirection symbol.
            int fd = open(args[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd < 0) {
                print_error();
                return -1;
            }
            if (dup2(fd, STDOUT_FILENO) < 0) {
                print_error();
                close(fd);
                return -1;
            }
            close(fd);
            return 0; 
        }
    }
    return 0;
}

void print_error() {
    write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)); 
}
