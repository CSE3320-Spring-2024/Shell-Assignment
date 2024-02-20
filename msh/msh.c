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
#define HIST_OF_COMMAND_MAX 15
#define PID_OF_COMMAND_MAX 15

char historyOfCmds[HIST_OF_COMMAND_MAX][SIZE_OF_COMMAND_MAX];
int numOfPidsCmds[PID_OF_COMMAND_MAX];
int toTrackTheCmds = 0;
int toTrackThePids = 0;
bool checkTheInput = true;

void toPrintTheHistory(int trackNum)
{
    for (int i = 0; i < trackNum; i++)
    {
        printf("%d: %s", i + 1, historyOfCmds[i]);
    }
}

void toPrintThePids(int trackNum)
{
    for (int i = 0; i < trackNum; i++)
    {
        printf("%d: %d\n", i + 1, numOfPidsCmds[i]);
    }
}

void toPrintTheError()
{
    const char *error_message = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
    // exit(EXIT_FAILURE);
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
            toPrintTheError();
            exit(EXIT_FAILURE);
        }
        else
        {
            if (toTrackThePids < PID_OF_COMMAND_MAX)
            {
                numOfPidsCmds[toTrackThePids] = pid;
                toTrackThePids++;
            }
            else
            {
                for (int row = 0; row < PID_OF_COMMAND_MAX - 1; row++)
                {
                    numOfPidsCmds[row] = numOfPidsCmds[row + 1];
                }
                numOfPidsCmds[PID_OF_COMMAND_MAX - 1] = pid;
            }

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
            perror("Error opening batch file");
            exit(EXIT_FAILURE);
        }
        checkTheInput = false; // No prompt for batch mode
    }
    else if (argc != 1)
    {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        if (checkTheInput)
        {
            printf("msh> ");
            while (!fgets(allocate_cmdStr, SIZE_OF_COMMAND_MAX, stdin));
        }
        else if (fgets(allocate_cmdStr, SIZE_OF_COMMAND_MAX, batchFile) == NULL)
        {
            // End of batch file
            fclose(batchFile);
            exit(0);
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

        if (tokenWeHave[0] != NULL && allocate_cmdStr[0] != '!')
        {
            if (toTrackTheCmds < HIST_OF_COMMAND_MAX)
            {
                strcpy(historyOfCmds[toTrackTheCmds], allocate_cmdStr);
                toTrackTheCmds++;
            }
            else
            {
                for (int row = 0; row < HIST_OF_COMMAND_MAX - 1; row++)
                {
                    strcpy(historyOfCmds[row], historyOfCmds[row + 1]);
                }
                strcpy(historyOfCmds[HIST_OF_COMMAND_MAX - 1], allocate_cmdStr);
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
