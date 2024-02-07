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


int main(int argc, char *argv[])
{
  char respond[50] = "Not a Command";
  char *line = "not exit";
  size_t size = 0;
  ssize_t chars = -1;
  char del[2] = " ";
  char *ptr;
  char argv1 [256][30];

  Start:
  while(strcmp(line, "exit"))
  {
    printf("msh> ");
    chars = getline(&line, &size, stdin); // returned value is length of string + 1 for line break
    line[chars - 1] = '\0';

    int argc1 = 1;
    for(int i = 0;i<chars -1;i++)
    {
      if(line[i] == ' ')
      {
        if(line[i-1] == ' ')
        {
          printf("Please enter only one space between commands\n");
          goto Start;
        }
        argc1++;
      }
    }
    printf("argc = %d\n", argc1);


char argv1[argc1][40];
char temp[256];
strcpy(temp, line);
strcpy(argv1[0],strtok(temp, del)); //assigns first command
if(argc1 > 1)
{
  
  for(int i = 1;i < argc1;i++)     //assigns each additional command
  {
    strcpy(argv1[i],strtok(NULL, del));
  }
  for(int i = 0;i < argc1;i++)
  {
    printf("%s\n", argv1[i]);
  }
         //At this point, Custom Argc and Argv are functional
}

    if (chars < 0)
    {
      puts("Input not valid");
    }
    else if(strcmp(argv1[0], "exit") == 0)
    {
      exit(0);
    }
    else if(strcmp(argv1[0], "cd") == 0)
    {
      printf("Moving to new directory %s\n", argv1[1]);
    }
    else
    {
      printf("%s is not a working command\n", line);
    }
  }
  free(line);
  return 0;
}

