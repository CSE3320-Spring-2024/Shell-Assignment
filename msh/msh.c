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
  char *line = NULL;
  size_t size = 0;
  ssize_t chars = -1;
  char del[2] = " ";
  pid_t pid;

  Start:
  while(1) //runs until exit is commanded by user
  {
    printf("msh> ");
    chars = getline(&line, &size, stdin); // returned value is length of string + 1 for line break
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
        if(line[i-1] == ' ') //two spaces in a row is a bad command
        {
          printf("Please enter only one space between commands\n");
          goto Start;     //ignores bad command and returns to Start:
        }
        argc1++;
      }
    }
    //printf("argc = %d\n", argc1); //For testing


char argv1[argc1][40];
strcpy(argv1[0]," ");
char temp[256];
strcpy(temp, line); // keeps original copy safe from strtok
strcpy(argv1[0],strtok(temp, del)); //assigns first command
if(argc1 > 1)
{
  for(int i = 1;i < argc1;i++)     //assigns each additional command
  {
    strcpy(argv1[i],strtok(NULL, del));
  }
  for(int i = 0;i < argc1;i++) //prints all argv1 entries seperately
  {
    printf("{%s}\n", argv1[i]);
  }
  //At this point, Custom Argc and Argv are functional
}

    if (chars < 0) //Something went terribly wrong here.
    {
      puts("An error has occured"); 
    }
    else if(strcmp(line, "exit") == 0) //user input exit command
    {
      exit(0);
    }
    else if(strcmp(argv1[0], "cd") == 0) //user is changing directory
    {
      //printf("Moving to new directory %s\n", argv1[1]);
      chdir(argv1[1]);
    }
    else //User entered an external command
    {
      
        //This section taken from popen.c in examples
      pid = fork();
      if(pid == 0)
      {
          //child
          printf("Hello Child\n");  //testing fork

          //execl("/bin/ls", "ls", NULL );
          //exit( EXIT_SUCCESS );
          char cat[40] = "/bin/";
          for(int i = 5;i< strlen(argv1[0]) + 5;i++)
            {
              cat[i] = argv1[0][i-5];
            }
            printf("{%s}\n", cat);
          
          execl(cat, argv1[0], NULL );

          //exit( EXIT_SUCCESS );
          for(int i = 1;i < argc1;i++)
          {
            if(strcmp(argv1[i], ">") == 0)  //allows > as a command to pipe result into file
            {
            //open file called "entered command". if it doesn't exist, create it.
            int fd = open(argv[i + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); 
            //O_WRONLY | O_APPEND //from geeksforgeeks.org dup2 tutorial
            //O_RDWR | O_CREAT, S_IRUSR | S_IWUSR  //From popen.c
            if(fd < 0)
            {
              perror( "Can't open output file." );  //prints error if file can not be created or found
              exit(0);                    
            }
            dup2(fd, 1);
            close(fd);
            printf("File Updated\n");
            }
          
         }

         return 0;
      }
      else if(pid > 0)
      {
        //parent
        wait(NULL);
        printf("Parent is awake\n");
      }
      else
      {
        perror( "Fork failed." );
      }
      
      printf("%s is not recognized in MSH\n", line);
    }
  }
  
  free(line);
  return 0;
}
//fork()
//implement exec() execvp() or execv()
// /bin or /users/bin
//Ctrl d to exit bash