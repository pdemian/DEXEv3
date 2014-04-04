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

int bytes_to_int(char* ptr)
{
	return 	((int)ptr[0] << 24) |
			((int)ptr[1] << 16) |
			((int)ptr[2] << 8) |
			((int)ptr[3] << 0);
}

Opcode get_opcode_from_instruction(char instruction)
{
	switch(instruction)
	{
		case Nop:
			return NOP;
		case Break:
			return BREAK;
		case Load:
			return LOAD;
		case Push:
			return PUSH;
		case Store:
			return STORE;
		case Dup: 
			return DUP;
		case Pop:
			return POP;
		case Inc:
			return INC;
		case Dec:
			return DEC;
		case Add:
			return ADD;
		case Sub:
			return SUB;
		case Mul:
			return MUL;
		case Div:
			return DIV;
		case Rem:
			return REM;
		case And:
			return AND;
		case Or:
			return OR;
		case Xor:
			return XOR;
		case Not:
			return NOT;
		case Neg:
			return NEG;
		case Shl:
			return SHL;
		case Shr:
			return SHR;
		case Cmp:
			return CMP;
		case Jmp:
			return JMP;
		case Je:
			return JE;
		case Jne:
			return JNE;
		case Jg:
			return JG;
		case Jge:
			return JGE;
		case Jl:
			return JL;
		case Jle:
			return JLE;
		case In:
			return IN;
		case Out:
			return OUT;
		case Call:
			return CALL;
		case Ret:
			return RET;
		default:
			return NOP;
	}
}

/*
	free_memory -> otherwise known as SEGFAULT village
	You don't really know how much you love Window's C lib until it's gone
	On Windows, this program never SEGFAULTed. Linux on the other hand, was
	a nonstop show of SEGFAULTS. I've so far spent over 4 hours debugging this
	function. Some of it was my fault (like trying to free stack memory), while
	others just fail for no explicit reason (eg, checking if something is NULL)
*/
void free_memory(Executable* exe)
{
	if(exe == NULL) 
		return;

	//free the functions struct
	for(int i = 0; i < exe->number_of_functions; i++)
	{
		free(exe->functions[i].instructions);
		if(exe->info->commandline & COMMANDLINE_DEBUG)
		{
			free(exe->functions[i].function_name);
			
			for(int k = 0; k < exe->functions[i].arg_count; k++)
				free(exe->functions[i].arg_names[k]);
			free(exe->functions[i].arg_names);
			
			for(int k = 0; k < exe->functions[i].local_count; k++)
				free(exe->functions[i].local_names[k]);
			free(exe->functions[i].local_names);
		}
	}
	
	
	if(exe->functions)
		free(exe->functions);
	//free the callstack
	if(exe->call_stack.stack_elements != NULL)
	{
		for(int i = exe->call_stack.stack_pointer - 1; i >= 0; i--)
		{
			Stack_Frame* sf = (Stack_Frame*)exe->call_stack.stack_elements[i];
			stack_free(&sf->stack);
			free(sf->local_memory);
		}
		//stack_free(&exe->call_stack); //SEGFAULT 1
	}
	
	//free the info struct
	if(exe->info != NULL)
	{
		//if(exe->info->file != NULL)
		//	fclose(exe->info->file); //SEGFAULT 2
		free(exe->info);
	}
}

void error(Executable* exe, enum DEXE_ERROR error, char* format, ...)
{
	if(exe->info->commandline & COMMANDLINE_SILENT)
	{
		free_memory(exe);
		exit(error);
	}
	
	puts("\n\nError\n\nThe execution of this DEXE file has been terminated for the following reason:");
	
	switch(error)
	{
		case OK:
			puts("Execution successful");
			break;
		case FILE_ERROR:
			puts("The file specified does not exist, cannot be accessed, or permissions exclude this file from being read.");
			break;
		case INVALID_DEXE_FILE:
			puts("The file specified is not a valid DEXE file. The file may be corrupt, or not a DEXE file at all.");
			break;
		case ALLOCATION_ERROR_IN_MAIN:
			puts("Could not allocate enough memory while setting up the DEXE file.");
			break;
		case ALLOCATION_ERROR_IN_READER:
			puts("Could not allocate enough memory while reading the DEXE file.");
			break;
		case ALLOCATION_ERROR_IN_STACK:
			puts("Could not allocate enough memory while allocating memory for the stack.");
			break;
		case ALLOCATION_ERROR_IN_EXECUTER:
			puts("Could not allocate enough memory while executing the DEXE file.");
			break;
		case CORRUPT_DEXE_FILE:
			puts("The DEXE file is corrupt.");
			break;
		case VERSION_MISMATCH:
			puts("There is a version mismatch between the DEXE file and this program. Either the DEXE file is depreciated, or this program is a depreciated.");
			break;
		case INVALID_OPCODE:
			puts("The DEXE file contains an invalid OPCODE. The DEXE file is possibly corrupt.");
			break;
		case VARIABLE_INDEX_OUT_OF_RANGE:
			puts("An error has occured where a local variable was called upon that did not exist within the current stack frame, causing an index out of range exception to be raised. The DEXE file is possibly corrupt.");
			break;
		case FUNCTION_DOES_NOT_EXIST:
			puts("The DEXE file contains a call to a function which does not exist within the executable file's function table. The DEXE file is possibly corrupt.");
			break;
		case INVALID_JUMP_POSITION:
			puts("A jump was made to an invalid position.");
			break;
		case ABRUPT_END_OF_FUNCTION:
			puts("A function has terminated without an explicit return statement. Either the DEXE file is possibly corrupt, or the programmer forgot to include a return statement.");
			break;
		case MANIPULATED_EMPTY_STACK:
			puts("A manipulation occured on an empty stack.");
			break;
		case DIVISION_BY_ZERO:
			puts("A division by zero has occured.");
			break;
		case NOT_ENOUGH_ARGUMENTS:
			puts("A function call was made without sufficient arguments on the stack.");
			break;
		default:
			puts("An unknown error has occured and caused the termination of this program.");
			break;
	}
	
	//get verbose arguments (char* format, ...) if verbose
	if(exe->info->commandline & COMMANDLINE_VERBOSE)
	{
		puts("");
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
		puts("");
		
		puts("Unwinding the call stack:");
		for(int i = exe->call_stack.stack_pointer -1; i >= 0; i--)
		{
			if(exe->info->commandline & COMMANDLINE_DEBUG)
				printf("  %s @ %d\n", exe->functions[((Stack_Frame*)exe->call_stack.stack_elements[i])->function_id].function_name, ((Stack_Frame*)exe->call_stack.stack_elements[i])->pc);
			else
				printf("  %d @ %d\n", ((Stack_Frame*)exe->call_stack.stack_elements[i])->function_id, ((Stack_Frame*)exe->call_stack.stack_elements[i])->pc);
		}
	}

	free_memory(exe);
	exit(error);
}