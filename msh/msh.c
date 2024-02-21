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

#define MAX_NUM_ARGUMENTS 32     

int main(int argc, char *argv[])
{

    FILE *bf;
    int batch_found = 0;
    char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
    char error_message[30] = "An error has occurred\n";   // error message to print 

    if (argc == 2)
    {
        bf = fopen(argv[1], "r");
        if (bf == NULL)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        batch_found = 1;
    }
    else if (argc > 2)
    {
       write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

    while( 1 )
    {
        int i = 0;
        int j = 0;  
        int token_count = 0;
        int location_num = 0;
        int redirect_found = 0;

        /* Parse input */
        char *token[MAX_NUM_ARGUMENTS];
        char *token_2[MAX_NUM_ARGUMENTS];
        char *argument_pointer;             // Pointer to point to the token, parsed by strsep 

        if (batch_found == 1)
        {
            if (fgets(command_string, MAX_COMMAND_SIZE, bf) == NULL)
            {
                fclose(bf);
                exit(0);
            }
        }
    
        else
        {
            printf ("msh> ");    // Print out the msh prompt

            // Read the command from the command line.  The
            // maximum command that will be read is MAX_COMMAND_SIZE
            // This while command will wait here until the user
            // inputs something.
            while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );                                      
                                                                
        }

        char *working_string  = strdup( command_string );                
        
          // we are going to move the working_string pointer so
          // keep track of its original value so we can deallocate
          // the correct amount at the end  
        char *head_ptr = working_string;
          
        // Tokenize the input with(out) whitespace used as the delimiter

        while ( ( (argument_pointer = strsep(&working_string, WHITESPACE ) ) != NULL) &&
                    (token_count<MAX_NUM_ARGUMENTS))
        {
            // grabs only the non-zero values, tokenizes each command seperately
            if (strlen(argument_pointer) > 0)                                                 
            {
                token[token_count] = strndup( argument_pointer, MAX_COMMAND_SIZE );
                token_count++;
            }
        }
                  
        for( i = 0; i < token_count; i++ )            // if ls -l > output ------->        token0 = ls        token1 = -l       token2 = >      token3 = output 
        {       
            if( strcmp( token[i], ">" ) == 0 )        // copy in token_2 ---------<        token2_0 = ls      token2_1 = -l    
            {
                redirect_found++;
                location_num = i; 
            }
            else
            {
                token_2[j] = token[i];
                j++;
            }
        }
        
        token_2[j] =  NULL;

        //-------------------------------------------------------------------------------------------------------------------------

        if (token_count > 0)
        {
            if (token[0] && strcmp("exit", token[0]) == 0)        // if exit, close the program ------ Completed (So far)
            {
                if (token_count != 1)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
                else
                {
                    exit(0);
                }     
            }
 
            else if (token[0] && strcmp("cd", token[0]) == 0)                           // Change directory ----- Completed (So far)
            {
                if (token_count != 2) 
                {     
                    write(STDERR_FILENO, error_message, strlen(error_message));        // print error if theres more than one arg attached to cd
                }    
                else 
                {
                    if (chdir(token[1]) == -1) 
                    {
                        // print error if second token failed
                        write(STDERR_FILENO, error_message, strlen(error_message)); 
                    }
                }
            }

            else if (token[0] && strcmp("ls", token[0]) == 0)      // LS function ------ Completed (so far)  Does need other PATH methods
            {
                int status;

                pid_t child_pid = fork();         // Fork a child process to execute the command
      
                if( child_pid == 0 )
                {
                    if (redirect_found == 1 )
                    {
                        if(location_num + 2 != token_count)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(-1); 
                        }
                        int fd = open( token[location_num+1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
                        if( fd < 0 )
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit( -1 );                    
                        }
                        dup2( fd, 1 );
                        close( fd );

                        if (token_count >= 2) 
                        {
                            token_count -= 2;
                            token_2[token_count] = NULL;
                        }
                        if (access("/bin/ls", X_OK) == 0)
                        {
                            if (execv("/bin/ls", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/bin/ls", X_OK) == 0)
                        {
                            if (execv("/usr/bin/ls", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/local/bin/ls", X_OK) == 0)
                        {
                            if (execv("/usr/local/bin/ls", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("./ls", X_OK) == 0)
                        {
                            if (execv("./ls", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }                            
                        }
                      
                        
                    }
                    else if ( redirect_found > 0)
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        exit(-1);
                    }
                    else
                    {
                        if (access("/bin/ls", X_OK) == 0)
                        {
                            if (execv("/bin/ls", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/bin/ls", X_OK) == 0)
                        {
                            if (execv("/usr/bin/ls", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/local/bin/ls", X_OK) == 0)
                        {
                            if (execv("/usr/local/bin/ls", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("./ls", X_OK) == 0)
                        {
                            if (execv("./ls", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }                            
                        }
                        else 
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(0);
                        }
                    }
                }

                else if (child_pid > 0)                             // Parent process waits for child to complete
                {
                    waitpid(child_pid, &status, 0 );   
                    fflush( NULL ); 
                }
                else
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(0);
                }
            }
        
            else if (token[0] && strcmp("cat", token[0]) == 0)      // cat function ------ Completed (so far)  Does need other PATH methods
            {
                token[token_count++] = NULL;
                int status;

                pid_t child_pid = fork();         // Fork a child process to execute the command
      
                if( child_pid == 0 )
                {
                    if (redirect_found == 1 )
                    {
                        if(location_num + 1 != token_count)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit( 0 ); 
                        }
                        int fd = open( token[location_num + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
                        if( fd < 0 )
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit( 0 );                    
                        }
                        dup2( fd, 1 );

                        if (access("/bin/cat", X_OK) == 0)
                        {
                            if (execv("/bin/cat", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/bin/cat", X_OK) == 0)
                        {
                            if (execv("/usr/bin/cat", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/local/bin/cat", X_OK) == 0)
                        {
                            if (execv("/usr/local/bin/cat", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("./cat", X_OK) == 0)
                        {
                            if (execv("./cat", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }                            
                        }

                        close( fd );  
                    }
                    else
                    {
                        if (access("/bin/cat", X_OK) == 0)
                        {
                            if (execv("/bin/cat", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/bin/cat", X_OK) == 0)
                        {
                            if (execv("/usr/bin/cat", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/local/bin/cat", X_OK) == 0)
                        {
                            if (execv("/usr/local/bin/cat", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("./cat", X_OK) == 0)
                        {
                            if (execv("./cat", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }                            
                        }
                        else 
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(0);
                        }
                    }
                }

                else if (child_pid > 0)                             // Parent process waits for child to complete
                {
                    waitpid(child_pid, &status, 0 );   
                    fflush( NULL ); 
                }
                else
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(0);
                }
            }

            else if (token[0] && strcmp("rm", token[0]) == 0)      // LS function ------ Completed (so far)  Does need other PATH methods
            {
                token[token_count++] = NULL;
                int status;

                pid_t child_pid = fork();         // Fork a child process to execute the command
      
                if( child_pid == 0 )
                {
                    if (redirect_found == 1 )
                    {
                        if(location_num + 1 != token_count)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(-1); 
                        }
                        int fd = open( token[location_num + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
                        if( fd < 0 )
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(-1);                    
                        }
                        dup2( fd, 1 );
                        close( fd ); 

                        if (access("/bin/rm", X_OK) == 0)
                        {
                            if (execv("/bin/rm", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/bin/rm", X_OK) == 0)
                        {
                            if (execv("/usr/bin/rm", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/local/bin/rm", X_OK) == 0)
                        {
                            if (execv("/usr/local/bin/rm", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("./rm", X_OK) == 0)
                        {
                            if (execv("./rm", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }                            
                        }
                        else
                        {
                            exit(-1);
                        }
                      
                         
                    }
                    else
                    {
                        if (access("/bin/rm", X_OK) == 0)
                        {
                            if (execv("/bin/rm", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/bin/rm", X_OK) == 0)
                        {
                            if (execv("/usr/bin/rm", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/local/bin/rm", X_OK) == 0)
                        {
                            if (execv("/usr/local/bin/rm", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("./rm", X_OK) == 0)
                        {
                            if (execv("./rm", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }                            
                        }
                        else 
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(0);
                        }
                    }
                }

                else if (child_pid > 0)                             // Parent process waits for child to complete
                {
                    waitpid(child_pid, &status, 0 );   
                    fflush( NULL ); 
                }
                else
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(0);
                }
            }

            else if (token[0] && strcmp("echo", token[0]) == 0)      // LS function ------ Completed (so far)  Does need other PATH methods
            {
                token[token_count++] = NULL;
                int status;

                pid_t child_pid = fork();         // Fork a child process to execute the command
      
                if( child_pid == 0 )
                {
                    if (redirect_found == 1 )
                    {
                        if(location_num + 1 != token_count)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit( 0 ); 
                        }
                        int fd = open( token[location_num + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
                        if( fd < 0 )
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit( 0 );                    
                        }
                        dup2( fd, 1 );

                        if (access("/bin/echo", X_OK) == 0)
                        {
                            if (execv("/bin/echo", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/bin/echo", X_OK) == 0)
                        {
                            if (execv("/usr/bin/echo", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/local/bin/echo", X_OK) == 0)
                        {
                            if (execv("/usr/local/bin/echo", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("./echo", X_OK) == 0)
                        {
                            if (execv("./echo", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }                            
                        }
                      
                        close( fd );  
                    }
                    else
                    {
                        if (access("/bin/echo", X_OK) == 0)
                        {
                            if (execv("/bin/echo", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/bin/echo", X_OK) == 0)
                        {
                            if (execv("/usr/bin/echo", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("/usr/local/bin/echo", X_OK) == 0)
                        {
                            if (execv("/usr/local/bin/echo", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }
                        }

                        else if (access("./echo", X_OK) == 0)
                        {
                            if (execv("./echo", token_2) == -1)
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                                exit(0);
                            }                            
                        }
                        else 
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(0);
                        }
                    }
                }

                else if (child_pid > 0)                             // Parent process waits for child to complete
                {
                    waitpid(child_pid, &status, 0 );   
                    fflush( NULL ); 
                }
                else
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(0);
                }
            }
            

            else if (token[0] && strcmp("&", token[0]) != 0)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        }

        free( head_ptr );

    }

    if (bf != NULL) 
    {
        fclose(bf);
    }

    return 0;
    // e2520ca2-76f3-90d6-0242ac1210022
}