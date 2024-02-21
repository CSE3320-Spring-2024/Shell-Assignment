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
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 32

int main(int argc, char *argv[])
{
  FILE *input_stream = stdin;
  char *command_string = (char*) malloc(MAX_COMMAND_SIZE);
  char error_message[30] = "An error has occurred\n";

  if(argc > 2)
  {
    write(STDERR_FILENO, error_message, strlen(error_message));
    free(command_string);
    exit(1);
  }

  if(argc == 2)
  {
    input_stream = fopen(argv[1], "r");
    if(input_stream == NULL)
    {
      write(STDERR_FILENO, error_message, strlen(error_message));
      free(command_string);
      exit(1);
    }
  }

  while(1)
  {
    
    if(input_stream == stdin)
    {
        printf("msh> ");
    }

    if(fgets(command_string, MAX_COMMAND_SIZE, input_stream) == NULL)
    {
        if(feof(input_stream) && input_stream != stdin)
        {
          fclose(input_stream);// End of file
          free(command_string);
          exit(0);
          break;
        }
        else if(ferror(input_stream))
        {
            //Error reading from input_stream
            write(STDERR_FILENO, error_message, strlen(error_message));
            break;
        }
    }

    command_string[strcspn(command_string, "\n")] = 0;
    
    char *token[MAX_NUM_ARGUMENTS];
    int token_count = 0;                                 
    char *argument_pointer;                                          
    char *working_string = strdup(command_string);
    char *head_ptr = working_string;


    while(((argument_pointer = strsep(&working_string, WHITESPACE)) != NULL) && (token_count < MAX_NUM_ARGUMENTS))
    {
      if(strlen(argument_pointer) > 0)
      {
        token[token_count] = strndup(argument_pointer, MAX_COMMAND_SIZE);
        if(token[token_count] == NULL)
        {
          for(int i = 0; i < token_count; i++)
          {
            free(token[i]);
          }
            break;//Break out of the while loop
          }
            token_count++;
          }
      }
      if(token_count == 0)
      {
        free(head_ptr);
        continue;
      }

      token[token_count] = NULL;//NULL-terminate token array

      if(strcmp(token[0], "exit") == 0)
      {
        if(token_count > 1)
        {
          write(STDERR_FILENO, error_message, strlen(error_message));
        }
        else
        {
          break;//Break out of the while loop
        }
      }
      else if(strcmp(token[0], "cd") == 0)
      {
        if(token_count > 2)
        {
          //If more than one arg print error
          write(STDERR_FILENO, error_message, strlen(error_message));
          break;
        }
        else if(token_count == 2)
        {
          //Change directory
          if(chdir(token[1]) != 0)
          {
            //Changing directory fails error
            write(STDERR_FILENO, error_message, strlen(error_message));
            break;
          }
        }
        else
        {
          //Change directory no arg error
          write(STDERR_FILENO, error_message, strlen(error_message));
          break;
      
        }
      }
      else
      {
        pid_t pid = fork();
        if(pid == 0)
        {
          //Child process
          int redirectIndex = -1;
          for(int i = 0; i < token_count; i++)
          {
            if(token[i] && strcmp(token[i], ">") == 0)
            {
              redirectIndex = i;
              break;
            }
          }

          if(redirectIndex != -1)
          {
              if(redirectIndex == 0 || token[redirectIndex + 1] == NULL || token[redirectIndex + 2] != NULL)
              {
                //Redirection symbol is used but no file specified
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(EXIT_FAILURE); //Prevents the child process from continuing
              }
              else
              {
                int fd = open(token[redirectIndex + 1], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
                 if(fd < 0)
                {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                dup2(fd, STDERR_FILENO);
                close(fd);
                token[redirectIndex] = NULL; //Terminate command before the redirection symbol
              }
            
          }

          if(execvp(token[0], token) == -1)
          {
              if(getenv("PATH") != NULL)
              {
                //PATH is set but the command wasn't found
                if(errno == ENOENT)
                {
                    //Command not found
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
                else
                {
                  perror("execvp error");
                }
              }
              else
              {
                //If PATH is not set, error
                write(STDERR_FILENO, error_message, strlen(error_message));
              }
            exit(EXIT_FAILURE);
          }
          } 
          else if(pid > 0)
          {
            //Parent process, wait for the child to complete
             wait(NULL);
          }
          else
          {
            //Fork failed
            perror("fork error");
            break;
          }
      }
      free(head_ptr);
      for (int i = 0; i < token_count; i++)
      {
        free(token[i]);
      }
  }
  free(command_string);
  if(input_stream != stdin)
  {
    fclose(input_stream);
  }
  return 0;
}
