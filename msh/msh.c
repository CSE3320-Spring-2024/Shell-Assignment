/*
define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define WHITESPACE " \t\n"
#define SIZE_OF_COMMAND_MAX 1024
#define NUM_OF_ARGUMENTS_MAX 10


bool checkTheInput = true;


void toPrintTheError()
{
    char error_message[] = "An error has occurred\n";
    fprintf(stderr, "%s", error_message);
}

void toHandleTheBuiltIns(char *token[])
{
    if (strcmp("exit", token[0]) == 0)
    {
        exit(0);
    }
    else if (strcmp("cd", token[0]) == 0)
    {
        if (token[1] == NULL)
        {
            const char *home_dir = getenv("HOME");
            if (home_dir != NULL)
            {
                if (chdir(home_dir) == -1)
                {
                    perror("cd");
                }
            }
            else
            {
                toPrintTheError();
            }
        }
        else if (token[2] != NULL)
        {
            toPrintTheError();
        }
        else
        {
            if (chdir(token[1]) == -1)
            {
                perror("cd");
            }
        }
    }
    else
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("An error has occurred\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
{
    execvp(token[0], token);
    perror(token[0]); // Print specific error message
    exit(EXIT_FAILURE);
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
    char *bufferInputWeHave = NULL;
    size_t bufferInputSizeWeHave = 0;
    ssize_t bufferInputLengthWeHave;
    FILE *batchFile;

    if (argc == 2)
    {
        batchFile = fopen(argv[1], "r");
    }

    while (1)
    {
        if (argc != 2)
        {
            printf("msh> ");
            fflush(stdout);
            bufferInputLengthWeHave = getline(&bufferInputWeHave, &bufferInputSizeWeHave, stdin);
        }
        else
        {
            bufferInputLengthWeHave = getline(&bufferInputWeHave, &bufferInputSizeWeHave, batchFile);
            if(feof(batchFile)) exit(0);
        }
        if(bufferInputLengthWeHave == -1)
        {
            if(feof(stdin))
            {
                printf("\n");
                break;
            }
            else
            {
                toPrintTheError();
                continue;
            }
        }

        if (bufferInputWeHave[bufferInputLengthWeHave - 1] == '\n')
            bufferInputWeHave[bufferInputLengthWeHave - 1] = '\0';

        if (strcmp(bufferInputWeHave, "exit") == 0) {
            break;
        }

        char *tokenWeHave[NUM_OF_ARGUMENTS_MAX];
        int numOfTokens = 0;
        char *pointerForArgument;
        char *stringWorks = strdup(bufferInputWeHave);
        char *rootWorks = stringWorks;
        checkTheInput = true;

        while (((pointerForArgument = strsep(&rootWorks, WHITESPACE)) != NULL) && (numOfTokens < NUM_OF_ARGUMENTS_MAX))
        {
            tokenWeHave[numOfTokens] = strndup(pointerForArgument, SIZE_OF_COMMAND_MAX);
            if (strlen(tokenWeHave[numOfTokens]) == 0)
            {
                tokenWeHave[numOfTokens] = NULL;
            }
            numOfTokens++;
        }

        if (tokenWeHave[0] != NULL && bufferInputWeHave[0] != '!')
    {

        if (tokenWeHave[0] == NULL)
        {
            continue;
        }
        toHandleTheBuiltIns(tokenWeHave);
        free(rootWorks);
    }

    return 0;
}
}
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define WHITESPACE " \t\n"
#define SIZE_OF_COMMAND_MAX 255
#define NUM_OF_ARGUMENTS_MAX 10
#define PID_OF_COMMAND_MAX 15

int toTrackTheCmds = 0;
int toTrackThePids = 0;
bool checkTheInput = true;

void toPrintTheError()
{
    const char *error_message = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void toHandleTheBuiltIns(char *token[])
{
    if (strcmp("exit", token[0]) == 0)
    {
        exit(0);
    }
    else if (strcmp("cd", token[0]) == 0)
    {
        if (token[1] == NULL)
        {
            const char *home_dir = getenv("HOME");
            if (home_dir != NULL)
            {
                if (chdir(home_dir) == -1)
                {
                    toPrintTheError();
                }
            }
            else
            {
                toPrintTheError();
            }
        }
        else if (token[2] != NULL)
        {
            toPrintTheError();
        }
        else
        {
            if (chdir(token[1]) == -1)
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
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            execvp(token[0], token);
            toPrintTheError();
            exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
        }
        checkTheInput = false; // No prompt for batch mode
    }
    else if (argc != 1)
    {
        toPrintTheError();
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if (argc != 2)
        {
            printf("msh> ");
            while (!fgets(allocate_cmdStr, SIZE_OF_COMMAND_MAX, stdin))
                ;
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

        while (((pointerForArgument = strsep(&stringWorks, WHITESPACE)) != NULL) &&
               (numOfTokens < NUM_OF_ARGUMENTS_MAX))
        {
            tokenWeHave[numOfTokens] = strndup(pointerForArgument, SIZE_OF_COMMAND_MAX);
            if (strlen(tokenWeHave[numOfTokens]) == 0)
            {
                tokenWeHave[numOfTokens] = NULL;
            }
            numOfTokens++;
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
