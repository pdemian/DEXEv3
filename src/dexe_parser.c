/*
	Copyright (C) 2014 Patrick Demian

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
	of the Software, and to permit persons to whom the Software is furnished to do
	so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
	THE SOFTWARE.
*/

#include "dexe_utils.h"
#include "dexe_executable.h"

int read_int(Executable* exe)
{
	char ch[4];
	if(fread(ch,1,4,exe->info->file) != 4)
		error(exe, CORRUPT_DEXE_FILE, "While reading the file, reached EOF before finished reading.");
	
	return bytes_to_int(ch);
}

char* read_string(Executable* exe, int size)
{
	unsigned int n = 0;
	if(!size)
	{
		n = (unsigned int)fgetc(exe->info->file);
	}
	else
	{
		n = (unsigned int)size;
	}
	
	char* str = (char*)malloc(n);
	
	if(str == NULL)
		error(exe, ALLOCATION_ERROR_IN_READER, "Unable to allocate a string long enough to store one of the strings in the file.");
	
	if(fread(str,1,n,exe->info->file) != n)
		error(exe, CORRUPT_DEXE_FILE, "While reading the file, reached EOF before finished reading.");
	
	return str;
}

void verify_valid_file(Executable* exe)
{
	char ch[5];
	
	if(fread(ch,1,5,exe->info->file) != 5)
		error(exe, INVALID_DEXE_FILE,"The file cannot be a DEXE file because it does not contain a DEXE header.");
		
	if(memcmp(ch,"DASM\xF0",5))
		error(exe, INVALID_DEXE_FILE, "The file cannot be a DEXE file because its headers do not match. Expected 'DASM\\xF0' read '%5s'",ch);
}
void parse_header(Executable* exe)
{
	exe->version = read_int(exe);
	exe->flags = read_int(exe);
	exe->entry = read_int(exe);
	
	exe->number_of_functions = read_int(exe);

	if((exe->version >> 16) > DEXE_MAJOR_VERSION)
		error(exe, VERSION_MISMATCH, "The major versions do not match. There is no guarantee that a newer version file will run on this interpreter. Suggestion: Update this interpreter. Interpreter version: [%u.%u.%u]. File version: [%u.%u.%u]", DEXE_MAJOR_VERSION, DEXE_MINOR_VERSION, DEXE_REVISION_VERSION, (exe->version >> 16) & 0xFF, (exe->version >> 8) & 0xFF, exe->version & 0xFF);
}
void parse_functions(Executable* exe)
{
	int ch;
	//expect '\xE0' byte
	if((ch = fgetc(exe->info->file)) != 0xE0)
		error(exe, CORRUPT_DEXE_FILE, "Expected function start byte (0xE0), but recieved byte 0x%X", ch);

	exe->functions = (Executable_Function*)malloc(sizeof(Executable_Function) * exe->number_of_functions);
	
	if(exe->functions == NULL)
		error(exe, ALLOCATION_ERROR_IN_READER, "Unable to allocate enough memory long enough to store an array of functions from the file.");

	for(int i = 0; i < exe->number_of_functions; i++)
	{
		//debug executables have additional items for debug purposes of course
		if(exe->flags & DEXE_FLAGS_DEBUG)
		{
			//read function name
			exe->functions[i].function_name = read_string(exe,0);
			
			//read arg_names
			exe->functions[i].arg_count = fgetc(exe->info->file);
			exe->functions[i].arg_names = (char**)malloc(sizeof(char*) * exe->functions[i].arg_count);
			if(exe->functions[i].arg_names == NULL)
				error(exe, ALLOCATION_ERROR_IN_READER, "Unable to allocate a string long enough to store an array of strings in the file.");
			
			for(int k = 0; k < exe->functions[i].arg_count; k++)
				exe->functions[i].arg_names[k] = read_string(exe,0);
			
			//read local_names
			exe->functions[i].local_count = fgetc(exe->info->file);
			exe->functions[i].local_names = (char**)malloc(sizeof(char*) * exe->functions[i].local_count);
			if(exe->functions[i].local_names == NULL)
				error(exe, ALLOCATION_ERROR_IN_READER, "Unable to allocate a string long enough to store an array of strings in the file.");

			for(int k = 0; k < exe->functions[i].local_count; k++)
				exe->functions[i].local_names[k] = read_string(exe,0);
				
		}
		else
		{
			//read arg_count, and local_count
			exe->functions[i].arg_count = fgetc(exe->info->file);
			exe->functions[i].local_count = fgetc(exe->info->file);
			
			//set everything else to NULL
			exe->functions[i].function_name = NULL;
			exe->functions[i].arg_names = NULL;
			exe->functions[i].local_names = NULL;
		}
		
		//read code
		exe->functions[i].size_of_instructions = read_int(exe);
		exe->functions[i].instructions = read_string(exe, exe->functions[i].size_of_instructions);

	}
	
	if((ch = fgetc(exe->info->file)) != 0xEF)
		error(exe, CORRUPT_DEXE_FILE, "Expected function end byte (0xEF), but recieved byte 0x%X", ch);
	
	if((ch = fgetc(exe->info->file)) != 0xFF)
		error(exe, CORRUPT_DEXE_FILE, "Expected DEXE end byte (0xFF), but recieved byte 0x%X", ch);
}

void dexe_read(Executable* exe)
{
	exe->info->file = fopen(exe->info->filename, "rb");
	
	if(exe->info->file == NULL)
	{
		error(exe, FILE_ERROR, "Exception occured while attempting to access '%s'", exe->info->filename);
	}
	
	verify_valid_file(exe);
	parse_header(exe);
	parse_functions(exe);
	
	fclose(exe->info->file);
	exe->info->file = NULL;
}