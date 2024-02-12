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

#define WHITESPACE " \t\n"     
#define MAX_COMMAND_SIZE 255   
#define MAX_NUM_ARGUMENTS 32     

int main()
{
  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  char error_message[30] = "An error has occurred\n";

  while( 1 )
  {
    printf ("msh> ");
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );
    char *token[MAX_NUM_ARGUMENTS];
    int token_count = 0;                                                                                 
    char *argument_pointer;                                                                                        
    char *working_string  = strdup( command_string );                
    char *head_ptr = working_string;
    while ( ( (argument_pointer = strsep(&working_string, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_pointer, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

char s[100];
//--------------------------Added code-----------------------------------------------------------------------------


        if (token_count > 0)
        {

        if (strcmp("exit", token[0]) == 0)        // if exit, close the program
        {
            exit(0);
        }

        else if (strcmp("ls", token[0]) == 0)
        {
            pid_t child_pid1 = fork();
            int status;
            if(child_pid1 == -1)
            {
                write(STDERR_FILENO, error_message, strlen(error_message)); 
                exit(-1);
            }
            else if (child_pid1 == 0)                 // Child process
            {
                execl("/bin/ls", "ls", NULL );
                exit(0);
            }
            else                                      // Parent waiting for child 
            {
                waitpid(child_pid1, &status, 0 );
                fflush( NULL );
            }
        }     
        
        else if (strcmp("cd", token[0]) == 0)
        {
     //       if (strcmp("..", token[1]) == 0)
    //        {
                chdir(".."); 
    //        }

    //    else 
    //    {
            chdir("..");
            wait( NULL );
    //    }
       }


      }
      else{
        wait(NULL);
      }



        free(head_ptr);

//-----------------------------End-------------------------------------------------------------------------------------
  }
  return 0;
}



