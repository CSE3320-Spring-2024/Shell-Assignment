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

#include "built_in_commands.c"

#include <stdio.h> // for input output (printing)
#include <stdlib.h> // for malloc()
#include <string.h> // for string stuff
#include <unistd.h> // for exec()
#include <dirent.h> // for accessing directories
#include <sys/stat.h> // for getting file stats
#include <fcntl.h> // for pipes

void search_directory(char* search_directory, char** input_command, int* command_found, 
											int max_command_size, int debug);

int command_found(char** input_command, char* bin_path, char* usr_bin_path, 
									char* usr_local_bin, int max_command_size, int debug);

void fork_process(char** arg_tokens, char** input_command, int valid_arg_count, int write_to_file, int debug, char* error_message);

/*

run_commands() -

runs the commands provided by arg_tokens[]

*/

void run_commands(int batch_mode, char** input_command, char** arg_tokens, int valid_arg_count, int write_to_file, int max_command_size, 
									int max_num_arguments, char* bin_path, char* usr_bin_path, char* usr_local_bin, int debug, char* error_message)
{
  if(valid_arg_count >= 1)
  {
    // if the command entered is NOT cd or exit. Check user bin files for the command
    // if the command is found in the user's bin, for a process and execute the command
    if(!strcmp(arg_tokens[0], "cd"))
    {
      cd(arg_tokens, valid_arg_count, debug, error_message);
    }
    else if(!strcmp(arg_tokens[0], "exit"))
    {
      exit_command(valid_arg_count, input_command, debug, error_message);
    }
    else if(command_found(&arg_tokens[0], bin_path, usr_bin_path, usr_local_bin, max_command_size, debug))
    {
    	fork_process(arg_tokens, input_command, valid_arg_count, write_to_file, debug, error_message);
    }	
    else 
    {
      if(debug) printf("msh: command not found: %s\n\n", arg_tokens[0]);
      if(strstr(arg_tokens[0], ".") || !strcmp(arg_tokens[0], ">")) write(STDERR_FILENO, error_message, strlen(error_message)); 
    }
  }

  // freeing memory
  free_all_tokens(arg_tokens, valid_arg_count, debug);
}

/*

fork_process() -

forks a process

*/

void fork_process(char** arg_tokens, char** input_command, int valid_arg_count, int write_to_file, int debug, char* error_message)
{
	int output_fd;
	int child_status_value;
	pid_t current_pid;

  current_pid = fork();

  if(current_pid < 0)
  { 
    if(debug) printf("msh: fork failed");
    write(STDERR_FILENO, error_message, strlen(error_message)); 
  }
  else if(current_pid == 0)
  {
    if(debug) printf("\nDEBUG: -----\nCHILD\n-----\n");

    if(write_to_file == -1)
    {
    	execv(arg_tokens[0], arg_tokens);
    }
    else
    {
    	output_fd = open(arg_tokens[write_to_file + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    	if(output_fd >= 0 && ((valid_arg_count - 1) - write_to_file) == 1)
    	{
    		dup2(output_fd, 1);
    		free(arg_tokens[write_to_file]);
    		arg_tokens[write_to_file] = NULL;
    		execv(arg_tokens[0], arg_tokens);
    	}
    	else
    	{
    		write(STDERR_FILENO, error_message, strlen(error_message)); 
    	}
    	close(output_fd);
    }
    strcpy((*input_command), "exit\n");
  }
  else
  {
    waitpid(0, &child_status_value, WCONTINUED);
    if(debug) printf("\nDEBUG: -----\nPARENT\n-----\n");
  }
}

/*

command_found() -

returns a 1 if the command that the user input is found
returns a 0 if the command was not found

*/

int command_found(char** input_command, char* bin_path, char* usr_bin_path, char* usr_local_bin, int max_command_size, int debug)
{
	int command_found = 0;

	if(!command_found) search_directory(bin_path, input_command, &command_found, max_command_size, debug);
	if(!command_found) search_directory(usr_bin_path, input_command, &command_found, max_command_size, debug);
	if(!command_found) search_directory(usr_local_bin, input_command, &command_found, max_command_size, debug);
	if(!command_found) search_directory("./", input_command, &command_found, max_command_size, debug);

	return command_found;
}

/*

search_directory()

searches the directory the user provided for executables
if an executable is found that matches the user input
update the value of command_found

*/

void search_directory(char* search_directory, char** input_command, int* command_found, int max_command_size, int debug)
{
	DIR* current_directory;
	struct dirent* dir_stream;
	struct stat file_stat;

	char current_file_path[max_command_size + strlen(search_directory) + 1];

	current_directory = opendir(search_directory);

	if(current_directory)
	{
		while(((dir_stream = readdir(current_directory)) != NULL) && !(*command_found))
		{
			if(!(strcmp(dir_stream->d_name, ".") == 0 || strcmp(dir_stream->d_name, "..") == 0))
			{
				strcpy(current_file_path, search_directory);
				strcat(current_file_path, dir_stream->d_name);

				stat(current_file_path, &file_stat);

				if(S_ISREG(file_stat.st_mode) && (file_stat.st_mode & S_IXUSR) && strcmp(dir_stream->d_name, (*input_command)) == 0)
				{
					if(debug) printf("\nDEBUG: COMMAND WAS FOUND. PATH: %s\n", current_file_path);
					(*command_found) = 1;
					strcpy((*input_command), current_file_path);
				}
			}
		}
	}
	else
	{
		if(debug) printf("\n--- ERROR: %s DIRECTORY COULD NOT BE OPENED ---\n", search_directory);
	}

	closedir(current_directory);
}
