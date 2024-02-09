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

#include <stdio.h> // for input output (printing)
#include <stdlib.h> // for malloc()
#include <string.h> // for string stuff

/*

tokenize_whitespace() -

Tokenizes the valid input from user
Returns a pointer to each seperated input
Modifies an integer value to the # of valid arguments the funciton detects

*/

char** tokenize_whitespace(int max_num_arguments, int max_command_size, int* valid_arg_num, char** working_string, char* whitespace_del, int debug)
{
	char** arg_tokens = (char**) calloc(max_num_arguments, sizeof(char**));
	char* argument_token;

  if(debug) printf("\nDEBUG: TOKENIZING: \n");

  while(((argument_token = strsep(working_string, whitespace_del)) != NULL) &&
        ((*valid_arg_num) < max_num_arguments))
  {
    if(debug) printf("DEBUG: %s\n", argument_token);

    // duplicating tokenized input into arg_tokens[] to access arguments individually later
    if(strlen(argument_token) >= 1)
    {
      arg_tokens[(*valid_arg_num)] = strdup(argument_token);
      (*valid_arg_num)++;
    }
  }

  return arg_tokens;
}

/*

free_all_tokens() -

Frees each pointer that allocated space to hold an argument
Frees the head pointer

*/

void free_all_tokens(char** arg_tokens, int valid_arguments, int debug)
{
  if(debug) printf("\nDEBUG: Your tokens:\n");

  for(int token_ind = 0; token_ind < valid_arguments; token_ind++)
  {
    if(debug && (strlen(arg_tokens[token_ind]) >= 1)) printf("DEBUG: token %d: %s\n", token_ind, arg_tokens[token_ind]);
    free(arg_tokens[token_ind]);
  }
  free(arg_tokens);
}
