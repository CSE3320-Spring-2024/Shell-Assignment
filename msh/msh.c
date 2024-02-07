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

  while(strcmp(line, "exit"))
  {
    printf("msh> ");
    chars = getline(&line, &size, stdin); // returned value is length of string + 1 for line break
    line[chars - 1] = '\0';

/*. //Add this later after exit is working
  int i = 0;
  while ((ptr = strsep(&line, del)) != NULL)   
  {  
     strcpy(argv1[i],ptr);
     i++;
  }
*/

  for(int i = 0;i < chars + 1;i++)
  {
      if(line[i] == ' ')
        line[i] == '\0';
  }

    if (chars < 0)
    {
      puts("Input not valid");
    }
    if(strcmp(line, "exit") == 0)
    {
      exit;
    }
    else
    {
      printf("%s is not a working command\n", line);
    }
  }
  free(line);
  return 0;
}

