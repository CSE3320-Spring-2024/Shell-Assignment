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
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
  if(argc > 2) {
    fprintf(stderr, "An error has occurred\n");
    exit(1);
  }
  char *line = malloc(255);
  size_t size = 0;
  ssize_t chars;
  char del[2] = " ";
  pid_t pid;
  FILE *file;
  int flip = 0;
  //int cnt = 0;

  //Start:
  while(1) //runs until exit is commanded by user
  {
    

    if(argc == 2)
    {
      //batchmode
      if (flip == 0)
      {
        flip = 1;
        file = fopen(argv[1], "r");
        if(file == NULL)
        {
          
          fprintf(stderr,"An error has occurred\n");
          exit(1);
        }
      }
      if (fgets(line, 255, file) != NULL)
      {
        chars = strlen(line);
        //cnt ++;
      }
      else
      {
        //printf("cnt: %d\n", cnt);
        /*if(cnt < 1)
          exit(1);
        else
          exit(0);*/
          exit(0);
      }
    }
    else
    {
      //user mode
      printf("msh> ");
      chars = getline(&line, &size, stdin); // returned value is length of string + 1 for line break
      if(chars <= 0) 
        continue;
    }

    while(chars > 0 && isspace(line[chars - 1])) //Removes extraneous spaces at end of last command
    {
      line[chars - 1] = '\0';
      chars--;
    }
    line[chars] = '\0'; //Ensures last character is a null terminator
    int argc1 = 1; //argc will always be at least 1
    for(int i = 0;i<chars -1;i++) //counts argc
    {
      if(line[i] == ' ') //space indicates a new command
      {
        if(line[i+1] != ' ') //two spaces in a row is a bad command
        {
          //fprintf(stderr, "An error has occurred\n");
          //goto Start;     //ignores bad command and returns to Start:
          //argc1++;
        }
        argc1++;
      }
    }

    char *argv1[argc1 + 1];
    char temp[256];
    if(chars <= 0) continue;
    strcpy(temp, line); // keeps original copy safe from strtok
    argv1[0] = strdup(strtok(temp, del)); //assigns first command
    argv1[1] = NULL;

    if(argc1 > 1)
    {
      for(int i = 1;i < argc1;i++)     //assigns each additional command
      {
        char *tok = strtok(NULL, del);
        if(tok == NULL) break;
        argv1[i] = strdup(tok);
        argv1[i + 1] = NULL;
      }
    }
    //At this point, Custom Argc and Argv are functional

    if (chars < 0) //Something went terribly wrong here.
    {
      fprintf(stderr, "An error has occurred\n"); 
    }
    else if(strcmp(argv1[0], "exit") == 0) //user input exit command
    {
      if (argc1 > 1)
        fprintf(stderr, "An error has occurred\n");
      else
        exit(0);
    }
    else if(strcmp(argv1[0], "cd") == 0) //user is changing directory
    {
      if(argc1 != 2)
        fprintf(stderr, "An error has occurred\n");
        //printf("An error has occurred");
      else
        chdir(argv1[1]);
    }
    else //User entered an external command
    {
      pid = fork();
      if(pid == 0)
      {
        //child
        //////////////////////////////////////redirect
        for(int i = 1;i < argc1;i++)
        {
          if(argv1[i] == NULL) break;
          if(strcmp(argv1[i], ">") == 0)  //allows > as a command to pipe result into file
          {
            if((argv1[i+1] == NULL) || (argv1[i+2] != NULL))
            {
              fprintf(stderr, "An error has occurred\n");
              exit(0);
            }
            else
            {
              //open file called "entered command". if it doesn't exist, create it.
              int fd = open(argv1[i + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); 
              if(fd < 0)
              {
              fprintf(stderr, "An error has occurred\n");
                exit(0);                    
              }
              dup2(fd, 1);
              close(fd);
              argv1[i] = NULL; 
              break;
            }
          }
        }
        /////////////////////////////////////////

        char cat[400];
        sprintf(cat, "%s/%s", "/bin/", argv1[0]);
        if(access(cat, X_OK) == 0) //checks if command exists in Bin. 
        {
        execv(cat, (char**)argv1);
        return 0;
        }
        else
        {
          fprintf(stderr, "An error has occurred\n");
          return 0;
        }
      }
      else if(pid > 0)
      {
        //parent
        wait(NULL);
      }
      else
      {
        perror("An error has occurred");
      }
    }
  }
  return 0;
}
