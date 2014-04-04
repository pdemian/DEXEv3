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

#pragma once
#include "dexe_stack.h"
#include "dexe_utils.h"

struct Executable_Function_struct
{
	char* function_name; //for debug
	
	int arg_count;
	char** arg_names; //for debug
	
	int local_count;
	char** local_names; //for debug
	
	int size_of_instructions;
	char* instructions;
};
typedef struct Executable_Function_struct Executable_Function;

struct Stack_Frame_struct
{
	int function_id;
	int pc;
	int flags;
	
	stack stack;
	int* local_memory;
};
typedef struct Stack_Frame_struct Stack_Frame;

struct Executable_struct
{
	struct Dexe_Info_struct* info;

	int version;
	int flags;
	int entry;
	
	int number_of_functions;
	Executable_Function* functions;
	
	stack call_stack;
};
typedef struct Executable_struct Executable;