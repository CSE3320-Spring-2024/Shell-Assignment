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

int main( int argc, char * argv[] )
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  // error message to print 
  char error_message[30] = "An error has occurred\n";         

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandi line.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something.
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_pointer;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    
    char *head_ptr = working_string;
    
    // Tokenize the input with(out) whitespace used as the delimiter

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
    //token[token_count++] = NULL;

        //-------------------------------------------------------------------------------------------------------------------------

        if (token_count > 0)
        {

          if (token[0] && strcmp("exit", token[0]) == 0)        // if exit, close the program
          {
            exit(0);
          }
 
          else if (token[0] && strcmp("cd", token[0]) == 0)
          {
            if (token_count == 1)
            {
              if (chdir("") == -1) 
              {
                // print error if token failed
                write(STDERR_FILENO, error_message, strlen(error_message)); 
              }
            }
            else if (token_count != 2) 
            {
              // print error if theres more than one arg attached to cd
              write(STDERR_FILENO, error_message, strlen(error_message));             
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
          
        }
  // /*
    // Code to print out each individual token
    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }

//*/
    free( head_ptr );

  }
  return 0;
  // e2520ca2-76f3-90d6-0242ac1210022
}