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
#include <signal.h>
#include <ctype.h>
#include <dirent.h>
//#include <sys/stat.h>
//#include <fcntl.h>


#define WHITESPACE " \t\n"
#define MAX_COMMAND_SIZE 255
#define MAX_NUM_ARGUMENTS 32



int main( int argc, char * argv[] )
{

  char * command_string=(char*)malloc(MAX_COMMAND_SIZE);

  char *dir=" ";
  int ret;


 


  while(1)
  {
    printf("msh> ");
    


    //exits

    if(command_string[0]=='e'&& command_string[1]=='x'&&command_string[2]=='i'&&command_string[3]=='t'&&command_string[4]=='\n')
    {
      exit(0);
    }  

    //cd 
    if(command_string[0]=='c'&& command_string[1]=='d'&& command_string[2]=='\n')
    {
       // int chdir(const char *path);
       ret=chdir(dir);

      
    }
  




    while(!fgets(command_string,MAX_COMMAND_SIZE,stdin));

    char *token [MAX_NUM_ARGUMENTS];

    int token_count=0;

    char *argument_pointer;
    
    char *working_string=strdup(command_string);

    char *head_ptr=working_string;

    while(((argument_pointer=strsep(&working_string,WHITESPACE))!=NULL)&& (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count]=strndup(argument_pointer,MAX_COMMAND_SIZE);
      if(strlen(token[token_count])==0)
      {
        token[token_count]=NULL;

      }
      token_count++;

    }

    
    
    pid_t pid=fork();

    if(pid<0)//fork failed
    {
      perror("failed");
      exit(0);

    }
    else if(pid==0) //child (new process)
    {
      
      execv(token[0],token);
      exit(0);

    }
    else //parent
    {
      wait(NULL); //waits until child processes
    }
    

    int token_index=0;
    for(token_index=0;token_index<token_count;token_index++)
    {
      printf("token[%d]=%s\n",token_index,token[token_index]);

    }
    free(head_ptr);


    
  }
  return 0;
}

