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
#include <fcntl.h>

// void process_command(char* command_str);

int main( int argc, char * argv[] )
{

  //char *command_str = ()


// if ( == "exit") {}



   pid_t pid = fork( );

   if( pid == 0 )
   {
      // Iterate over the provided command and see if there is a redirect
      // operator in it.  If there is then open a pipe between 
      int i;
      for( i=1; i<argc; i++ )
      {
         if( token[0] && strcmp( argv[i], ">" ) == 0 )
         {
            int fd = open( argv[i+1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
            if( fd < 0 )
            {
                perror( "Can't open output file." );
                exit( 0 );                    
            }
            dup2( fd, 1 );
            close( fd );
            
            // Trim off the > output part of the command
            argv[i] = NULL;
         }
      }
      execvp( argv[1], &argv[1] );
   }
  else if( pid > 0 )
  {
    wait( NULL );
  }
  else
  {
    perror( "Fork failed." );
  }

//REDIRECTION OUTPUT - forks then string compares. parent just waits
  // pid_t pid = fork(); 

  // if (pid == 0){

  //   if (token[0] && strcmp (token[0], ">") == 0) {
  //     printf("Found one\n");
  //   else {
  //     printf("Didn't find one\n");
  //   }

  //   printf("Done searching\n");
    
  //   }

  //   else 
  //   {
  //     int status;
  //     wait (&status);


  //   }

  // }



  return 0;
}

