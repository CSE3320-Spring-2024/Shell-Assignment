/*
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


int main( int argc, char * argv[] )
{
  return 0;
}
*/

//-----------------MSH EXAMPLE CODE----------------------------------------------

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

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the command line.  The
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
    
    // Tokenize the input with whitespace used as the delimiter
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

    





    if (token_count > 0) {
    // Handle built-in commands
    if (strcmp(token[0], "exit") == 0) {
        // Free allocated memory before exiting
        free(command_string);
        free(working_string);
        exit(0);
    }
    // Add more built-in commands here (e.g., cd)

    // Fork a child process to execute external commands
    pid_t pid = fork();

    if (pid < 0) {
        // Error forking
        perror("fork");
    } else if (pid == 0) {
        // Child process
        execvp(token[0], token);
        // execvp only returns if an error occurs
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        // Wait for the child to finish
        waitpid(pid, NULL, 0);
    }
}

// Free allocated memory
free(working_string);
for (int i = 0; i < token_count; i++) {
    free(token[i]);
}


    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
/*
    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }

    free( head_ptr );
*/
  }
  return 0;
  // e2520ca2-76f3-90d6-0242ac1210022
}

