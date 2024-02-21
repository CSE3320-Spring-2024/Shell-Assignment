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

//creating a boolean variable to check the input required 
bool checkTheInput = true;

//creating a function to print the error as defined in the requirements 
void toPrintTheError()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

//creating a function to manage the built-in commands given
void toHandleTheBuiltIns(char *tokenWeHave[])
{
    //checking if the token is "exit"
    if (strcmp("exit", tokenWeHave[0]) == 0)
    {
        //checking if the second token is not NULL
        if (tokenWeHave[1] != NULL)
        {
            //printing the required error to show
            toPrintTheError();
            return;
        }
        //exiting the program gracefully
        exit(0);
    }
    //checking if the token is "cd"
    else if (strcmp("cd", tokenWeHave[0]) == 0)
    {
        if (tokenWeHave[2] != NULL)
        {
            toPrintTheError();
        }
        else
        {
            if (tokenWeHave[1] != NULL)
            {
                if (chdir(tokenWeHave[1]) == -1)
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
        //creating a child process using fork() call
        pid_t pid = fork();
        if (pid == -1)
        {
            //handling the print error
            toPrintTheError();
            exit(1);
        }
        else if (pid == 0)
        {
            for (int i = 0; tokenWeHave[i] != NULL; ++i)
            {
                if (strcmp(tokenWeHave[i], ">") == 0)
                {
                     if (i == 0 || tokenWeHave[i + 1] == NULL)
                    {
                        toPrintTheError();
                        exit(1);
                    }
                    if (tokenWeHave[i + 2] != NULL)
                    {
                        toPrintTheError();
                        exit(1);
                    }
                    int fd = open(tokenWeHave[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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
                    for (int j = i; tokenWeHave[j] != NULL; ++j)
                    {
                        tokenWeHave[j] = tokenWeHave[j + 2];
                    }
                    break;
                }
            }
            execvp(tokenWeHave[0], tokenWeHave);
            toPrintTheError();
            exit(1);
        }
        else
        {
            int statusOfProcess;
            waitpid(pid, &statusOfProcess, 0);
        }
    }
}

//creating the main function passing cli arguments 
int main(int argc, char *argv[])
{
    //allocating memory dynamically
    char *allocate_cmdStr = (char *)malloc(SIZE_OF_COMMAND_MAX);
    //declaring file pointer input mode 
    FILE *batchIndicateFile = NULL;

    if (argc == 2)
    {
        batchIndicateFile = fopen(argv[1], "r");
        //batchIndicateFile is NULL (file opening failed)
        if (!batchIndicateFile)
        {
            toPrintTheError();
            exit(1);
        }
        checkTheInput = false;
    }
    //number of command-line arguments is not equal to 1
    else if (argc != 1)
    {
        toPrintTheError();
        exit(1);
    }
//infinite loop for the main shell 
    while (1)
    {
        if (argc != 2)
        {
            printf("msh> ");
            //waits until a command is entered
            while (!fgets(allocate_cmdStr, SIZE_OF_COMMAND_MAX, stdin));
        }
        else
        {
            //reading commands until end 
            while (!fgets(allocate_cmdStr, SIZE_OF_COMMAND_MAX, batchIndicateFile))
            {
                //if it reached the end of file 
                if (feof(batchIndicateFile))
                    exit(0);
            }
        }
        //creating an array to store tokens
        char *tokenWeHave[NUM_OF_ARGUMENTS_MAX];
        int numOfTokens = 0;
        //setting pointers for tokenization
        char *pointerForArgument, *stringWorks = strdup(allocate_cmdStr), *rootWorks = stringWorks;
        checkTheInput = true;

        //tokenizing the input string using WHITESPACE as delimeter, limited num of tokens 
        while (((pointerForArgument = strsep(&stringWorks, WHITESPACE)) != NULL) && (numOfTokens < NUM_OF_ARGUMENTS_MAX))
        {
            //copying the token into tokenWeHave array, limited length
            tokenWeHave[numOfTokens] = strndup(pointerForArgument, SIZE_OF_COMMAND_MAX);
            //if the empty string is 0
            if (strlen(tokenWeHave[numOfTokens]) == 0)
            {
                //setting the tokens to NULL
                tokenWeHave[numOfTokens] = NULL;
            }
            //incrementing the num of tokens
            else
            {
                numOfTokens++;
            }
        }
        //if the command token is NULL
        if (tokenWeHave[0] == NULL)
        {
            //continue to next iteration 
            continue;
        }
        //handling the built-in commands on given tokens
        toHandleTheBuiltIns(tokenWeHave);
        //freeing the memory allocated for rootWorks pointer
        free(rootWorks);
    }
    return 0;
}