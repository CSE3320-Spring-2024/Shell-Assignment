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

#include <stdio.h> // for input output (printing)
#include <unistd.h> // for exec()
#include <sys/wait.h>
#include <stdlib.h> // for malloc()
#include <errno.h>
#include <string.h> // for string stuff

#define DEBUG 1

#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 32
#define WHITESPACE_DEL " \t\n"
#define EXIT_COMMAND "exit\n"

int main( int argc, char * argv[] )
{
  char* command_input = (char*) malloc(MAX_COMMAND_SIZE);
  char** arg_tokens;
  int valid_args; 

  // while loop only continues if stdin is NOT EOF and if the entered string is NOT "exit"
  while(!feof(stdin) && strcmp(command_input, EXIT_COMMAND))
  {
    valid_args = 0;

    printf("msh> ");

    // repeat until the user inputs a valid string or input is an EOF
    while(!feof(stdin) && !fgets(command_input, MAX_COMMAND_SIZE, stdin));

    if(!feof(stdin))
    {
      char* working_string = strdup(command_input);
      char* original_working_str = working_string;

      // get tokens from user input and store in arg_tokens
      // update value of the # of valid arguments detected
      arg_tokens = tokenize_whitespace(MAX_NUM_ARGUMENTS, MAX_COMMAND_SIZE, &valid_args, &working_string, WHITESPACE_DEL, DEBUG);

      if(DEBUG) printf("\nDEBUG: You typed: %s", command_input);

      // freeing memory
      free_all_tokens(arg_tokens, valid_args, DEBUG);
      free(original_working_str);
    }
  }

  free(command_input);

  return 0;
}

