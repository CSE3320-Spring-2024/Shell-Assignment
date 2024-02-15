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

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 32     

int main(int argc, char * argv[])
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
    

        //-------------------------------------------------------------------------------------------------------------------------

        if (token_count > 0)
        {

            if (token[0] && strcmp("exit", token[0]) == 0)        // if exit, close the program ------ Completed (So far)
          {
            exit(0);
          }
 
          else if (token[0] && strcmp("cd", token[0]) == 0)      // Chang directory ----- Completed (So far)
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

           else if (token[0] && strcmp("ls", token[0]) == 0)      // LS function ------ Completed (so far)  Does need other PATH methods
          {
              token[token_count++] = NULL;
              pid_t child_pid = fork();         // Fork a child process to execute the command
              int status;
              if (child_pid == -1)              // Checks if child process fails
              {
                  write(STDERR_FILENO, error_message, strlen(error_message)); 
                  exit(0);
              } 
              else if (child_pid == 0)  // Child process
              {
                  if (execvp("/bin/ls", token) == -1)        
                  {
                      write(STDERR_FILENO, error_message, strlen(error_message));
                      exit(0); // exit with failure status
                  }
              }
              else                              // Parent process waits for child to complete
              {
                  waitpid(child_pid, &status, 0 );   
                  fflush( NULL ); 
              }
          
          }




   pid_t pid = fork( );

   if( pid == 0 )
   {
      // Iterate over the provided command and see if there is a redirect
      // operator in it.  If there is then open a pipe between 
      int i;
      for( i=1; i<argc; i++ )
      {
         if( strcmp( argv[i], ">" ) == 0 )
         {
            int fd = open( argv[i+1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
            if( fd < 0 )
            {
                perror( "Can't open output file." );
                exit( 0 );                    
            }
            dup2( fd, 1 );
            close( fd );
            argv[i] = NULL;            // Trim off the > output part of the command
         }
      }
      execvp( argv[1], &argv[1] );
   }
  else if( pid > 0 )
  {
    wait( NULL );
  }
  else
  {
    perror( "Fork failed." );
  }








        }
   /*                               /// Print out string tokens
    // Code to print out each individual token
    int token_index  = 0;
    for( token_index = 0; token_index < token_count; token_index ++ ) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index] );  
    }

*/
    free( head_ptr );

  }
  /*                            // Free file information 
    if (input_file) 
    {
        fclose(input_file);
    }
*/
  return 0;
  // e2520ca2-76f3-90d6-0242ac1210022
}