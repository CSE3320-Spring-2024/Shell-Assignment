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

#include <stdio.h> // for input output (printing)
#include <stdlib.h> // for malloc()
#include <string.h> // for string stuff
#include <unistd.h> // for exec()
#include <dirent.h> // for accessing directories
#include <sys/stat.h> // for getting file stats

void search_directory(char* search_directory, char** input_command, int* command_found, int max_command_size, int debug);

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
