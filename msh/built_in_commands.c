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
#include <unistd.h> // for getcwd

/*

exit() -

ensures the program exits when exit is used properly

*/

void exit_command(char** arg_tokens, int valid_args, char** command_input, int debug, char* error_message)
{
	if(valid_args >= 2)
	{
		printf("\n%s\n", error_message);
	}
	else if(valid_args == 1)
	{
		strcpy((*command_input), "exit\n");
	}
	else if(debug)
	{
		if(debug) printf("DEBUG: EXIT COMMAND: arg_tokens has 1 or 0 arguments. program should have broken out of loop and exitted sooner");
	}
}	

/*

cd() -

changes the directory based on the argument provided to cd

*/

void cd(char** arg_tokens, int valid_args, int debug, char* error_message)
{
	char path[257];

	if(valid_args == 2)
	{
		getcwd(path, sizeof(path));
		if(debug) printf("\nDEBUG: CD COMMAND: print working directory:%s\n", path);

		if(!chdir(arg_tokens[1]))
		{
			getcwd(path, sizeof(path));
			if(debug) printf("\nDEBUG: CD COMMAND: updated working directory:%s\n", path);
		}
		else
		{
			if(debug) printf("cd: No such file or directory\n\n");
			write(STDERR_FILENO, error_message, strlen(error_message)); 
		}
	}
	else if(valid_args > 2)
	{
		if(debug) printf("cd: too many arguemnts\n\n");
		write(STDERR_FILENO, error_message, strlen(error_message)); 
	}
	else if(valid_args == 1)
	{
		if(debug) printf("cd: needs one argument\n\n");
		write(STDERR_FILENO, error_message, strlen(error_message)); 
	}
}
