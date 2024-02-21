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

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports four arguments



///////NOT WORKING?????
///// > Insert code work on later fill out oof 
///////RESUBMIT TO MAKE SURE - Wasn't working??? 
//Try again Github s

int main(int argc, char *argv[])
{
  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  char error_message[30] = "An error has occurred\n"; 
  char * env; 
  char path[256]; 
  FILE *file_name = NULL; 
  int inter_mode = 1; 
  int start = 0; 

  ////////////////////////////////////////////////////////////////////////////

  if (argc == 2) {  
    file_name = fopen(argv[1], "r");

    if (file_name == NULL ) {
      
    write(STDERR_FILENO, error_message, strlen(error_message)); 
    exit(1); 

    }

    inter_mode = 0;  

    printf("prompt> ");
    
  } else if (argc > 2){

    write(STDERR_FILENO, error_message, strlen(error_message)); 
    exit(1); 
  }

  ///////////////////////////////////////////////////////////////////////

  while( 1 )
  {
    if (inter_mode){  //true
    printf ("msh> ");
  
    }

    ////////////////////////////////////////////////////////////////////////////
  
    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
   
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;  
    int   comm_count = 0;                               
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      } else {

        comm_count++; 

      }

        token_count++;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    if ((token[0] == NULL) && (comm_count >= 1)){ 

      for (int i = 0; i < token_count; i++) {

        if (token[i] != NULL){
          start = i; 
        }
      }
    }
    /////////////////////////////////////////////////////////////////////////

      // int strncmp(const char *str1, const char *str2, size_t n)
    if ((token[0]) && (!strncmp(token[0], "exit", 4))){

      if (token[1] != NULL) {

        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit(0); 

      }

      exit(0); //exit(0) = success

       // int strncmp(const char *str1, const char *str2, size_t n)
    }  else if ((token[0]) && (!strncmp(token[0], "cd", 2))){

      // DIR *opendir(const char *dirname);
      if (!opendir(token[1])) {

        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit(0); 
      }
////////////////////////////////////////////////////////////

      if (token[1] == NULL){
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit(0); 
      } 
      else if (token_count > 3){
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit(0); 
      }
      
      chdir(token[1]); //claim

      } 
      else {

        pid_t pid = fork();
        int status;
  //        if( pid == 0 )
  //  {
  //     // Iterate over the provided command and see if there is a redirect
  //     // operator in it.  If there is then open a pipe between 
  //     int i;
  //     for( i=1; i<argc; i++ )
  //     {
  //        if( strcmp( argv[i], ">" ) == 0 )

         for (int i=1; i< token_count; i++){

            if (strcmp(token[i], ">") == 0) {
               {
  //           int fd = open( argv[i+1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
  //           if( fd < 0 )
  //           {
  //               perror( "Can't open output file." );
  //               exit( 0 );                    
  //           }
  //           dup2( fd, 1 );
  //           close( fd );

              int file_dir = open(token[i+1], O_RDWR|O_CREAT| O_TRUNC, O_IRUSR| S_IWSR);

              if ( file_dir < 0) {

                write(STDERR_FILENO, error_message, strlen(error_message)); 
                exit(0);
              }

              dup2(file_dir, 1); //1 -
              close(file_dir);

              token[i] = NULL; 
            }
          }
          ////////

           if (access(path, X_OK) ==0) {
              // int ret = execvp( arguments[0], &arguments[0] )
              execvp(path, &token[start]);
              exit(0); 
            }
  //       
            
  //           // Trim off the > output part of the command
  //           argv[i] = NULL;
  //        }
  //     }
  //     execvp( argv[1], &argv[1] );
  //  }
  // else if( pid > 0 )
  // {
  //   wait( NULL );
  // }
  // else
  // {
  //   perror( "Fork failed." );
  // }
        if (pid < 0) {
          write(STDERR_FILENO, error_message, strlen(error_message)); 
          exit(0); 

        } else if (pid == 0) { 

  //          if( child_pid == 0 )
  // {
  //   execl("/bin/ls", "ls", NULL );
  //   exit( EXIT_SUCCESS );
  // }

          char *path_token = NULL; 
          // char *getenv(const char *varname);
          env = getenv("PATH");

          if (env == NULL) {
            write(STDERR_FILENO, error_message, strlen(error_message)); 
            exit(0); 
          }

          for (int i = 0; i < token_count; i++) {

            if (strcmp(token[i], ">") == 0) {

             write(STDERR_FILENO, error_message, strlen(error_message)); 
             exit(0); 
            }
          }
  
        
       //////////////////////////////////////////////

          while ((path_token = strcmp(&env, ":")) != NULL) { 

            strncpy(path, path_token);
            strcat(path, "/");
            strcat(path, token[start]);
           
          }
        
        ////////////////////////////////////////////////////
         write(STDERR_FILENO, error_message, strlen(error_message)); 
           exit(0);

      }
      waitpid(pid, &status,0); 
       
    }

    //////////////////////////////////////////////////////////////////////////////////////

    free( head_ptr );
     
  }

   return 0;
}
