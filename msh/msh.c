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

#include "tokenize_helpers.c"
#include "shell_helpers.c"
#include "built_in_commands.c"

#include <stdio.h>          // for input output (printing)
#include <unistd.h>         // for exec()
#include <stdlib.h>         // for malloc()
#include <string.h>         // for string stuff
#include <errno.h>          
#include <sys/wait.h>       // wait() for the child process
#include <ctype.h>
#include <signal.h>

#define DEBUG 0
#define PWD 0

#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 32
#define WHITESPACE_DEL " \t\n"
#define EXIT_COMMAND "exit\n"
#define BIN_PATH "/bin/"
#define USR_BIN_PATH "/usr/bin/"
#define USR_LOCAL_BIN "/usr/local/bin/"
#define ERROR_MESSAGE "An error has occured\n"

int main( int argc, char * argv[] )
{
  char* command_input = (char*) malloc(MAX_COMMAND_SIZE + 1);
  char** arg_tokens;
  char cwd[257];
  int valid_args; 
  int child_status_value;
  pid_t current_pid;

  // while loop only continues if stdin is NOT EOF and if the entered string is NOT "exit"
  while(!feof(stdin) && strcmp(command_input, EXIT_COMMAND))
  {
    valid_args = 0;

    if(PWD) getcwd(cwd, sizeof(cwd));
    if(PWD) printf("\n\n~%s\n", cwd);
    printf("msh> ");

    // repeat until the user inputs a valid string or input is an EOF
    while(!feof(stdin) && !fgets(command_input, MAX_COMMAND_SIZE, stdin));

    if(!feof(stdin) && strcmp(command_input, EXIT_COMMAND))
    {
      // deep copying string from command_input into working_string
      char* working_string = strdup(command_input);
      char* original_working_str = working_string;

      // get tokens from user input and store in arg_tokens
      // update value of the # of valid arguments detected
      arg_tokens = tokenize_whitespace(MAX_NUM_ARGUMENTS, MAX_COMMAND_SIZE, &valid_args, &working_string, WHITESPACE_DEL, DEBUG);

      if(DEBUG) printf("\nDEBUG: You typed: %s", command_input);

      if(valid_args >= 1)
      {

        // if the command entered is NOT cd, exit, or a command found in bin, usr/bin, usr/local/bin/, or the local path
        // print the command is not found and repeat prompt
        if(!strcmp(arg_tokens[0], "cd"))
        {
          cd(arg_tokens, valid_args, DEBUG, ERROR_MESSAGE);
        }
        else if(!strcmp(arg_tokens[0], "exit"))
        {
          exit_command(arg_tokens, valid_args, &command_input, DEBUG, ERROR_MESSAGE);
        }
        else if(command_found(&arg_tokens[0], BIN_PATH, USR_BIN_PATH, USR_LOCAL_BIN, MAX_COMMAND_SIZE, DEBUG))
        {
          current_pid = fork();

          if(current_pid < 0)
          { 
            if(DEBUG) printf("msh: fork failed");
            write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)); 
          }
          else if(current_pid == 0)
          {
            if(DEBUG) printf("\nDEBUG: -----\nCHILD\n-----\n");
            execv(arg_tokens[0], arg_tokens);
            strcpy(command_input, "exit\n");
          }
          else
          {
            waitpid(0, &child_status_value, WCONTINUED);
            if(DEBUG) printf("\nDEBUG: -----\nPARENT\n-----\n");
          }
        }
        else 
        {
          if(DEBUG) printf("msh: command not found: %s\n\n", arg_tokens[0]);
          write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)); 
        }
      }

      // freeing memory
      free_all_tokens(arg_tokens, valid_args, DEBUG);
      free(original_working_str);
    }
  }

  free(command_input);

  return 0;
}

