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
#include <stdbool.h>

#define WHITESPACE " \t\n"
#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 32

int main()
{
  char* command_string = (char*) malloc(MAX_COMMAND_SIZE);
  char* path[] = {"/bin", "/usr/bin", "/usr/local/bin", "./", NULL};
  
  while(1) {
    printf("msh> ");

    while(!fgets(command_string, MAX_COMMAND_SIZE, stdin));

    /* Parse */
    char* token[MAX_NUM_ARGUMENTS];
    char* argument_pointer;                                         
    char* working_string  = strdup(command_string);                
    char* head_ptr = working_string;
    char error_message[30] = "An error has occurred\n";
    int token_count = 0;                                 
    
    // Tokenize the input with whitespace used as the delimiter
    while(((argument_pointer = strsep(&working_string, WHITESPACE)) != NULL) &&
         (token_count<MAX_NUM_ARGUMENTS)) {
      token[token_count] = strndup(argument_pointer, MAX_COMMAND_SIZE);
      if(strlen(token[token_count]) == 0) {
        token[token_count] = NULL;
      }

      token_count++;
    }

    // Now print the tokenized input as a debug check --REMOVE WHEN SUBMITTING--
    int token_index  = 0;
    for(token_index = 0; token_index < token_count; token_index++) 
    {
      printf("token[%d] = %s\n", token_index, token[token_index]);  
    }

    // Check for exit
    if(strcmp(token[0], "exit") == 0) {
      if(token[1] != NULL) {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        continue;
      }
      exit(0);
    }

    // Check for cds
    if(strcmp(token[0], "cd") == 0) {
      if(token[1] == NULL) {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        continue;
      } else if(token[2] != NULL) {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        continue;
      }
      if(chdir(token[1]) != 0) {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
      }

      continue;
    }

    // Check if command exists
    bool command_exists = false;
    char command_path[MAX_COMMAND_SIZE];

    for(int i = 0; path[i] != NULL; i++) {
      snprintf(command_path, MAX_COMMAND_SIZE, "%s/%s", path[i], token[0]);
      if(access(command_path, X_OK) == 0) {
        command_exists = true;
        break;
      }
    }
    if(!command_exists) {
      write(STDERR_FILENO, error_message, strlen(error_message)); 
      continue;
    }

    // The forkening...
    pid_t pid = fork();

    if(pid < 0) {
      write(STDERR_FILENO, error_message, strlen(error_message)); 
      exit(1);
    } else if(pid == 0) {
      if (execvp(token[0], token) < 0) {
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit(1);
      }
    } else {
      wait(NULL);
    }

    free( head_ptr );
  }

  return 0;
  
  // e2520ca2-76f3-90d6-0242ac1210022
}

