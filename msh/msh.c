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

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 32

int main( int argc, char * argv[] )
{
  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
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
    command_string[strcspn(command_string, "\n")] = 0;

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];
    int token_count = 0;                                 
    char *argument_pointer;      // Pointer to point to the token                                                                                             
    char *working_string  = strdup( command_string );  // parsed by strsep          
    char *head_ptr = working_string; //free this pointer later
    
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

    if(token_count == 0)
    {
      free(head_ptr);
      continue; //if no command skip to next iteration
    }

    if(token[0] && strcmp(token[0], "exit") == 0)
    {
      if(token_count > 1)
      {
        write(STDERR_FILENO, error_message, strlen(error_message));
      }
      else
      {
      free(command_string);
      free(head_ptr);
      exit(0);
      }
    }
    else if(token[0] && strcmp(token[0], "cd") == 0)
    {
      if(token_count == 2)
      {
        if(chdir(token[1]) != 0)
        {
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
      }
      else
      {
        printf("usage: cd directory\n");
      }
    }
    else
    {
      pid_t pid = fork();
      if(pid == 0)
      { // Child process
        // Attempt to execute the command
        if(execvp(token[0], token) == -1)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(EXIT_FAILURE); // Ensure to exit the child process if exec fails
        }
      } 
      else if(pid > 0)
      {   // Wait for the child process to finish
        int status;
        waitpid(pid, &status, 0);
      }
      else
      {  
        // Fork failed
        write(STDERR_FILENO, error_message, strlen(error_message));
      }
    }

    free( head_ptr );

  }

  return 0;
}

