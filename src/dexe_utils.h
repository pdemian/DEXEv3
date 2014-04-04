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

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include "dexe_executable.h"
#include "dexe_opcodes.h"


//defines
#define DEXE_MAJOR_VERSION    0x00
#define DEXE_MINOR_VERSION    0x01
#define DEXE_REVISION_VERSION 0x00

#define COMMANDLINE_HELP      0x01
#define COMMANDLINE_VERSION   0x02
#define COMMANDLINE_DEBUG     0x04
#define COMMANDLINE_DUMP      0x08
#define COMMANDLINE_DECOMPILE 0x10
#define COMMANDLINE_SILENT    0x20
#define COMMANDLINE_VERBOSE   0x40

#define DEXE_FLAGS_DEBUG      0x01
#define DEXE_FLAGS_EXECUTABLE 0x02
#define DEXE_FLAGS_LIBRARY    0x04

//structs
struct Dexe_Info_struct
{
	int commandline;
	char* filename;
	FILE* file;
};
typedef struct Dexe_Info_struct Dexe_Info;


//enums
enum DEXE_ERROR
{
	//success
	OK = 0, 
	
	//I/O errors or memory errors
	FILE_ERROR, 
	INVALID_DEXE_FILE, 
	ALLOCATION_ERROR_IN_MAIN,
	ALLOCATION_ERROR_IN_READER,
	ALLOCATION_ERROR_IN_STACK,
	ALLOCATION_ERROR_IN_EXECUTER,
	
	//Corrupt file errors
	CORRUPT_DEXE_FILE,
	VERSION_MISMATCH,
	INVALID_OPCODE,
	VARIABLE_INDEX_OUT_OF_RANGE,
	FUNCTION_DOES_NOT_EXIST,
	INVALID_JUMP_POSITION,
	
	//Programmer's code errors
	ABRUPT_END_OF_FUNCTION,
	MANIPULATED_EMPTY_STACK,
	DIVISION_BY_ZERO,
	NOT_ENOUGH_ARGUMENTS,
	
	//unknown
	UNKNOWN_ERROR
};


//prototypes
extern int bytes_to_int(char*);

extern Opcode get_opcode_from_instruction(char);

extern void free_memory(Executable*);

//this declares that the function won't return, so the compiler won't give out warnings
#ifdef _MSC_VER
	__declspec(noreturn) extern void error(Executable*, enum DEXE_ERROR, char*, ...);
#elif __GNUC__
	extern void error(Executable*, enum DEXE_ERROR, char*, ...) __attribute__((noreturn));
#else
	extern void error(Executable*, enum DEXE_ERROR, char*, ...);
#endif
