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
#define ERROR_MESSAGE "An error has occurred\n"

int main( int argc, char * argv[] )
{
  FILE* fp;
  char** arg_tokens;
  char* input_command = (char*) malloc(MAX_COMMAND_SIZE + 1);
  char file_line[MAX_COMMAND_SIZE + 1];
  char cwd[257];
  int valid_arg_count; 
  int write_to_file;
  int rc = 0;
  int batch_mode = 0;

  // if there is no arguments enter interactive mode
  // if there is one argument enter batch mode
  if(argc == 1)
  {
    // while loop only continues if stdin is NOT EOF and if the entered string is NOT "exit"
    while(!feof(stdin) && strcmp(input_command, EXIT_COMMAND))
    {
      valid_arg_count = 0;
      write_to_file = -1;

      if(PWD) getcwd(cwd, sizeof(cwd));
      if(PWD) printf("\n\n~%s\n", cwd);
      printf("msh> ");

      // repeat until the user inputs a valid string or input is an EOF
      while(!feof(stdin) && !fgets(input_command, MAX_COMMAND_SIZE, stdin));

      if(!feof(stdin) && strcmp(input_command, EXIT_COMMAND))
      {
        // deep copying string from input_command into working_string
        char* working_string = strdup(input_command);
        char* original_working_str = working_string;

        // get tokens from user input and store in arg_tokens
        // update value of the # of valid arguments detected
        arg_tokens = tokenize_whitespace(MAX_NUM_ARGUMENTS, MAX_COMMAND_SIZE, &valid_arg_count, &write_to_file, &working_string, WHITESPACE_DEL, DEBUG);

        if(DEBUG) printf("\nDEBUG: You typed: %s", input_command);

        // if there are an appropriate # of arguments, attempt to run the commands the user input
        run_commands(batch_mode, &input_command, arg_tokens, valid_arg_count, write_to_file, MAX_COMMAND_SIZE, 
                     MAX_NUM_ARGUMENTS, BIN_PATH, USR_BIN_PATH, USR_LOCAL_BIN, DEBUG, ERROR_MESSAGE);

        free(original_working_str);
      }
    }
  }
  else if(argc == 2)
  {
    batch_mode = 1;
    fp = fopen(argv[1], "r");

    if(fp != NULL)
    {
      while(fgets(file_line, MAX_COMMAND_SIZE + 1, fp) != NULL)
      {
        valid_arg_count = 0;
        write_to_file = -1;
        // deep copying string from input_command into working_string
        char* working_string = strdup(file_line);
        char* original_working_str = working_string;

        // get tokens from user input and store in arg_tokens
        // update value of the # of valid arguments detected
        arg_tokens = tokenize_whitespace(MAX_NUM_ARGUMENTS, MAX_COMMAND_SIZE, &valid_arg_count, &write_to_file, &working_string, WHITESPACE_DEL, DEBUG);

        if(DEBUG) printf("\nDEBUG: File line read: %s", file_line);

        // if there are an appropriate # of arguments, attempt to run the commands the user input
        run_commands(batch_mode, &input_command, arg_tokens, valid_arg_count, write_to_file, MAX_COMMAND_SIZE, 
                     MAX_NUM_ARGUMENTS, BIN_PATH, USR_BIN_PATH, USR_LOCAL_BIN, DEBUG, ERROR_MESSAGE);

        free(original_working_str);
      } 
    }
    else
    {
      write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)); 
      rc = 1;
    }
    fclose(fp);
  }
  else
  {
    write(STDERR_FILENO, ERROR_MESSAGE, strlen(ERROR_MESSAGE)); 
    rc = 1;
  }

  free(input_command);

  return rc;
}

