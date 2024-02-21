/*
Name : Prakhyat Chaube 
ID : 100206072
*/
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

//header files we will need for this project
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>

#define WHITESPACE " \t\n"
#define SIZE_OF_COMMAND_MAX 255
#define NUM_OF_ARGUMENTS_MAX 10

bool checkTheInput = true;

void toPrintTheError()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void toHandleTheBuiltIns(char *token[])
{
    if (strcmp("exit", token[0]) == 0)
    {
        if (token[1] != NULL)
        {
            toPrintTheError();
            return;
        }
        exit(0);
    }
    else if (strcmp("cd", token[0]) == 0)
    {
        if (token[2] != NULL)
        {
            toPrintTheError();
        }
        else
        {
            if (token[1] != NULL)
            {
                if (chdir(token[1]) == -1)
                {
                    toPrintTheError();
                }
            }
            else
            {
                toPrintTheError();
            }
        }
    }
    else
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            toPrintTheError();
            exit(1);
        }
        else if (pid == 0)
        {
            for (int i = 0; token[i] != NULL; ++i)
            {
                if (strcmp(token[i], ">") == 0)
                {
                     if (i == 0 || token[i + 1] == NULL)
                    {
                        toPrintTheError();
                        exit(1);
                    }
                    if (token[i + 2] != NULL)
                    {
                        toPrintTheError();
                        exit(1);
                    }
                    int fd = open(token[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                    if (fd == -1)
                    {
                        toPrintTheError();
                        exit(1);
                    }
                    if (dup2(fd, STDOUT_FILENO) == -1)
                    {
                        toPrintTheError();
                        close(fd);
                        exit(1);
                    }
                    close(fd);
                    for (int j = i; token[j] != NULL; ++j)
                    {
                        token[j] = token[j + 2];
                    }
                    break;
                }
            }
            execvp(token[0], token);
            toPrintTheError();
            exit(1);
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}


int main(int argc, char *argv[])
{
    char *allocate_cmdStr = (char *)malloc(SIZE_OF_COMMAND_MAX);
    FILE *batchFile = NULL;

    if (argc == 2)
    {
        batchFile = fopen(argv[1], "r");
        if (!batchFile)
        {
            toPrintTheError();
            exit(1);
        }
        checkTheInput = false;
    }
    else if (argc != 1)
    {
        toPrintTheError();
        exit(1);
    }

    while (1)
    {
        if (argc != 2)
        {
            printf("msh> ");
            while (!fgets(allocate_cmdStr, SIZE_OF_COMMAND_MAX, stdin));
        }
        else
        {
            while (!fgets(allocate_cmdStr, SIZE_OF_COMMAND_MAX, batchFile))
            {
                if (feof(batchFile))
                    exit(0);
            }
        }

        char *tokenWeHave[NUM_OF_ARGUMENTS_MAX];
        int numOfTokens = 0;
        char *pointerForArgument;
        char *stringWorks = strdup(allocate_cmdStr);
        char *rootWorks = stringWorks;
        checkTheInput = true;

        while (((pointerForArgument = strsep(&stringWorks, WHITESPACE)) != NULL) && (numOfTokens < NUM_OF_ARGUMENTS_MAX))
        {
            tokenWeHave[numOfTokens] = strndup(pointerForArgument, SIZE_OF_COMMAND_MAX);
            if (strlen(tokenWeHave[numOfTokens]) == 0)
            {
                tokenWeHave[numOfTokens] = NULL;
            }
            else
            {
                numOfTokens++;
            }
        }
        if (tokenWeHave[0] == NULL)
        {
            continue;
        }
        toHandleTheBuiltIns(tokenWeHave);
        free(rootWorks);
    }
    return 0;
}