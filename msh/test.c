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

int main(int argc, char *argv[])
{
    FILE *batch_file;
    int batch_found = 0;
    char *command_string = (char*) malloc(MAX_COMMAND_SIZE);
    char *head_ptr;
    char error_message[30] = "An error has occurred\n";

    if (argc == 2)
    {
        batch_file = fopen(argv[1], "r");
        if (batch_file == NULL)
        {
            perror("Error opening batch file");
            return EXIT_FAILURE;
        }
        batch_found = 1;
    }
    else if (argc > 2)
    {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    while (1)
    {
        int i = 0;
        int j = 0;
        int location_num = 0;
        int redirect_found = 0;
        char *token[MAX_NUM_ARGUMENTS];
        char *token_2[MAX_NUM_ARGUMENTS];
        int token_count = 0;
        char *argument_pointer;

        if (batch_found == 1)
        {
            if (fgets(command_string, MAX_COMMAND_SIZE, batch_file) == NULL)
            {
                fclose(batch_file);
                return EXIT_SUCCESS;
            }
        }
        else
        {
            printf("msh> ");
            if (fgets(command_string, MAX_COMMAND_SIZE, stdin) == NULL)
            {
                perror("Error reading command");
                return EXIT_FAILURE;
            }
        }

        char *working_string = strdup(command_string);
        head_ptr = working_string;

        while (((argument_pointer = strsep(&working_string, WHITESPACE)) != NULL) && (token_count < MAX_NUM_ARGUMENTS))
        {
            if (strlen(argument_pointer) > 0)
            {
                token[token_count] = strndup(argument_pointer, MAX_COMMAND_SIZE);
                token_count++;
            }
        }

        for (i = 0; i < token_count; i++)
        {
            if (strcmp(token[i], ">") == 0)
            {
                redirect_found = 1;
                location_num = i;
            }
            else
            {
                token_2[j] = token[i];
                j++;
            }
        }
        token_2[j] = NULL;

        if (token_count > 0)
        {
            if (token[0] && strcmp("exit", token[0]) == 0)
            {
                free(head_ptr);
                return EXIT_SUCCESS;
            }
            else if (token[0] && strcmp("cd", token[0]) == 0)
            {
                            if (token_count == 1)
            {
              if (chdir("/workspaces/shell-assignment-JoshuaM4818") == -1) 
              {
                write(STDERR_FILENO, error_message, strlen(error_message));                 // print error if token failed
              }
            }
            else if (token_count != 2) 
            {     
              write(STDERR_FILENO, error_message, strlen(error_message));             // print error if theres more than one arg attached to cd
            } 
            else 
            {
              if (chdir(token[1]) == -1) 
              {
                // print error if second token failed
                write(STDERR_FILENO, error_message, strlen(error_message)); 
              }
            }
            }

            else if (token[0] && strcmp("ls", token[0]) == 0)
            {
                              token[token_count++] = NULL;
              int status;

              pid_t child_pid = fork();         // Fork a child process to execute the command
      
              if( child_pid == 0 )
              {
                  if (redirect_found == 1 )
                  {
                      int fd = open( token[location_num + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
                      if( fd < 0 )
                      {
                          write(STDERR_FILENO, error_message, strlen(error_message));
                          exit( 0 );                    
                      }
                      dup2( fd, 1 );

                      if (execv("/bin/ls", token_2) == -1)
                      { 
                          write(STDERR_FILENO, error_message, strlen(error_message));
                          exit(0);
                      }
                      
                      close( fd );  
                  }
                  else
                  {
                      if (execv("/bin/ls", token_2) == -1)
                      {
                          write(STDERR_FILENO, error_message, strlen(error_message));
                          exit(0);
                      }
                  }
              }

              else if (child_pid > 0)                             // Parent process waits for child to complete
              {
                  waitpid(child_pid, &status, 0 );   
                  fflush( NULL ); 
              }
              else
              {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(0);
              }
            }
        }

        free(head_ptr);
    }

    return 0;
}
