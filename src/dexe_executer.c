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

#include "dexe_executer.h"
#include "dexe_stack.h"
#include "dexe_utils.h"
#include "dexe_opcodes.h"

#define JUMP_NOT_EQUAL 1
#define JUMP_EQUAL     2
#define JUMP_GREATER   4
#define JUMP_LESS      8

/*
	TODO: optimize this.
	Removed from stack_pop/stack_peek because I didn't want to push Executable* and Opcode* on every call.
	Maybe I can __fastcall this to make this faster? Or maybe inline? The optimizer will figure out whats the best thing to do
	That's the hope at least.

	Also, calling with Opcode* seems like it produces smaller and hopefully faster code. All it does is a 'lea' instruction
	while Opcode seems to copy all the contents of the struct into memory and then pushes it. Hopefully another speed improvement
*/
#if defined(_MSC_VER) || defined(__GNUC__)
	void /*__fastcall */ /* inline */ test_stack_size(Executable* exe, Opcode* op)
#else
	void test_stack_size(Executable* exe, Opcode* op)
#endif
{
	if(((Stack_Frame*)exe->call_stack.stack_elements[exe->call_stack.stack_pointer-1])->stack.stack_pointer < op->required_stack_size)
		error(exe, MANIPULATED_EMPTY_STACK, "A(n) '%s' instruction was encountered that requires at least %d item on the stack. Found %d items", op->mnemonic, op->required_stack_size, ((Stack_Frame*)stack_peek(&exe->call_stack))->stack.stack_pointer);
}


//prototype
void breakpoint(Executable* exe);

int dexe_execute(Executable* exe)
{
	if(exe->entry < 0 || exe->entry > exe->number_of_functions)
		error(exe, FUNCTION_DOES_NOT_EXIST, "The function specified by the entry point does not exist. There are %d functions. Valid function ids are 0-%d. The value specified by the entry point is: %d", exe->number_of_functions, exe->number_of_functions, exe->entry);

		
	stack_init(&exe->call_stack);
	
	Stack_Frame sf;
	sf.function_id = exe->entry;
	sf.pc = 0;
	sf.flags = 0;
	stack_init(&sf.stack);
	
	stack_push(&exe->call_stack, (long)&sf);
	
	int ret_val = dexe_run_function(exe);
		
	stack_pop(&exe->call_stack);
	
	stack_free(&exe->call_stack);

    return ret_val;
}

int dexe_run_function(Executable* exe)
{
	//set up the stack frame
	Stack_Frame* sf = (Stack_Frame*)stack_peek(&exe->call_stack);
	
	if (exe->functions[sf->function_id].local_count != 0)
	{
		sf->local_memory = (int*)malloc(exe->functions[sf->function_id].local_count * sizeof(int));

		if (!sf->local_memory)
			error(exe, ALLOCATION_ERROR_IN_EXECUTER, "Could not allocate %d bytes of memory for locals", exe->functions[sf->function_id].local_count * sizeof(int));
		
		memset(sf->local_memory,'\0',exe->functions[sf->function_id].local_count * sizeof(int));
	}
	else
		sf->local_memory = NULL;

	//additional variables for the sake of increasing readability (and hopefully, optimization purposes)
	char* code_ptr = exe->functions[sf->function_id].instructions;
	int size = exe->functions[sf->function_id].size_of_instructions;
	stack* stack_ptr = &sf->stack;

	
	for(sf->pc = 0; sf->pc < size; sf->pc++)
	{
		unsigned char opcode = code_ptr[sf->pc];
		
		switch(opcode)
		{
			case Nop:
			{
				//do nothing by definition
				break;
			}
			case Break:
			{
				if(exe->flags & DEXE_FLAGS_DEBUG && exe->info->commandline & COMMANDLINE_DEBUG)
					breakpoint(exe);
			}
			case Load:
			{
				sf->pc++;
				if(opcode > exe->functions[sf->function_id].local_count)
					error(exe, VARIABLE_INDEX_OUT_OF_RANGE, "Index recieved: %d. This occured in function %d.",opcode, sf->function_id);
					
				stack_push(stack_ptr, sf->local_memory[(unsigned char)opcode]);
				
				break;
			}
			case Push:
			{
				sf->pc++;
				if(sf->pc < size - 4)
					error(exe, ABRUPT_END_OF_FUNCTION, "Ran out of executable code while attempting to push a literal onto the stack.");
				
				stack_push(stack_ptr, bytes_to_int((char*)(code_ptr + sf->pc)));
				
				break;
			}
			case Store:
			{
				sf->pc++;
				if(opcode > exe->functions[sf->function_id].local_count)
					error(exe, VARIABLE_INDEX_OUT_OF_RANGE, "Index recieved: %d. This occured in function %d.",opcode, sf->function_id);
				if(stack_ptr->stack_pointer < 1)
					error(exe, MANIPULATED_EMPTY_STACK, "A store instruction was encountered that requires at least 1 item on the stack. Found %d items", stack_ptr->stack_pointer);
			
				sf->local_memory[opcode] = stack_pop(stack_ptr);
				
				break;
			}
			case Dup:
			{
				test_stack_size(exe, &DUP);

				stack_push(stack_ptr, stack_peek(stack_ptr));
				
				break;
			}
			case Pop:
			{
				test_stack_size(exe, &POP);

				stack_pop(stack_ptr);
				
				break;
			}
			case Inc:
			{
				test_stack_size(exe, &INC);
				
				stack_push(stack_ptr,stack_pop(stack_ptr)+1);
				
				break;
			}
			case Dec:
			{
				test_stack_size(exe, &DEC);
				
				stack_push(stack_ptr,stack_pop(stack_ptr)-1);
				
				break;
			}
			case Add:
			{
				test_stack_size(exe, &ADD);
				
				stack_push(stack_ptr, stack_pop(stack_ptr) + stack_pop(stack_ptr));
				
				break;
			}
			case Sub:
			{
				test_stack_size(exe, &SUB);
				
				register int value1 = stack_pop(stack_ptr);
				register int value2 = stack_pop(stack_ptr);
				stack_push(stack_ptr, value2 - value1);
				
				break;
			}
			case Mul:
			{
				test_stack_size(exe, &MUL);
				
				stack_push(stack_ptr, stack_pop(stack_ptr) * stack_pop(stack_ptr));
				
				break;
			}
			case Div:
			{
				test_stack_size(exe, &DIV);
				
				register int value1 = stack_pop(stack_ptr);
				register int value2 = stack_pop(stack_ptr);
				if(value2 == 0)
					error(exe,DIVISION_BY_ZERO, "A division by zero was encountered while trying to divide %d by %d", value2, value1);
				
				stack_push(stack_ptr, value2 / value1);
				
				break;
			}
			case Rem:
			{
				test_stack_size(exe, &REM);
				
				register int value1 = stack_pop(stack_ptr);
				register int value2 = stack_pop(stack_ptr);
				if(value2 == 0)
					error(exe,DIVISION_BY_ZERO, "A division by zero was encountered trying to divide %d by %d", value2, value1);
				
				stack_push(stack_ptr, value2 % value1);
				
				break;
			}
			case And:
			{
				test_stack_size(exe, &AND);
				
				stack_push(stack_ptr, stack_pop(stack_ptr) & stack_pop(stack_ptr));
				
				break;
			}
			case Or:
			{
				test_stack_size(exe, &OR);
				
				stack_push(stack_ptr, stack_pop(stack_ptr) | stack_pop(stack_ptr));
				
				break;
			}
			case Xor:
			{
				test_stack_size(exe, &XOR);
				
				stack_push(stack_ptr, stack_pop(stack_ptr) ^ stack_pop(stack_ptr));
				
				break;
			}
			case Not:
			{
				test_stack_size(exe, &NOT);
				
				stack_push(stack_ptr, ~stack_peek(stack_ptr));
				
				break;
			}
			case Neg:
			{
				test_stack_size(exe, &NEG);
				
				stack_push(stack_ptr, -stack_peek(stack_ptr));
				
				break;
			}
			case Shl:
			{
				test_stack_size(exe, &SHL);
				
				register int value1 = stack_pop(stack_ptr);
				register int value2 = stack_pop(stack_ptr);
				
				stack_push(stack_ptr, value2 << value1);
				
				break;
			}
			case Shr:
			{
				test_stack_size(exe, &SHR);
				
				register int value1 = stack_pop(stack_ptr);
				register int value2 = stack_pop(stack_ptr);
				
				stack_push(stack_ptr, value2 >> value1);
				
				break;
			}
			case Cmp:
			{
				test_stack_size(exe, &CMP);
				
				register int value1 = stack_pop(stack_ptr);
				register int value2 = stack_pop(stack_ptr);

				sf->flags = (value1 == value2 ? JUMP_EQUAL : JUMP_NOT_EQUAL) |
					(value2 > value1 ? JUMP_GREATER : 0) |
					(value2 < value1 ? JUMP_LESS : 0);
					
				break;
			}
			case Jmp:
			{
				sf->pc += bytes_to_int(code_ptr + sf->pc + 1);
				if(sf->pc < 0 || sf->pc > size)
					error(exe, INVALID_JUMP_POSITION, "Range expected: 0 - %d. Recieved %d", size, sf->pc);
				break;
			}
			case Je:
			{
				if (sf->flags & JUMP_EQUAL)
				{
					sf->pc += code_ptr[sf->pc + 1];
					if(sf->pc < 0 || sf->pc > size)
						error(exe, INVALID_JUMP_POSITION, "Range expected: 0 - %d. Recieved %d", size, sf->pc);
				}
				break;
			}
			case Jne:
			{
				if (sf->flags & JUMP_NOT_EQUAL)
				{
					sf->pc += code_ptr[sf->pc + 1];
					if(sf->pc < 0 || sf->pc > size)
						error(exe, INVALID_JUMP_POSITION, "Range expected: 0 - %d. Recieved %d", size, sf->pc);
				}
				break;
			}
			case Jg:
			{
				if(sf->flags & JUMP_GREATER)
				{
					sf->pc += code_ptr[sf->pc + 1];
					if(sf->pc < 0 || sf->pc > size)
						error(exe, INVALID_JUMP_POSITION, "Range expected: 0 - %d. Recieved %d", size, sf->pc);
				}
				break;
			}
			case Jge:
			{
				if(sf->flags & (JUMP_GREATER | JUMP_EQUAL))
				{
					sf->pc += code_ptr[sf->pc + 1];
					if(sf->pc < 0 || sf->pc > size)
						error(exe, INVALID_JUMP_POSITION, "Range expected: 0 - %d. Recieved %d", size, sf->pc);
				}
				break;
			}
			case Jl:
			{
				if(sf->flags & JUMP_LESS)
				{
					sf->pc += code_ptr[sf->pc + 1];
					if(sf->pc < 0 || sf->pc > size)
						error(exe, INVALID_JUMP_POSITION, "Range expected: 0 - %d. Recieved %d", size, sf->pc);
				}
				break;
			}
			case Jle:
			{
				if(sf->flags & (JUMP_LESS | JUMP_EQUAL))
				{
					sf->pc += code_ptr[sf->pc + 1];
					if(sf->pc < 0 || sf->pc > size)
						error(exe, INVALID_JUMP_POSITION, "Range expected: 0 - %d. Recieved %d", size, sf->pc);
				}
				break;
			}
			case In:
			{
				stack_push(stack_ptr, getc(stdin));
				
				break;
			}
			case Out:
			{
				test_stack_size(exe, &OUT);
				
				putc(stack_pop(stack_ptr), stdout);
				
				break;
			}
			case Call:
			{
				Stack_Frame frame;
				frame.function_id = bytes_to_int(code_ptr + sf->pc + 1);
				frame.pc = 0;
				frame.flags = 0;
				
				if(frame.function_id < 0 || frame.function_id > exe->number_of_functions)
					error(exe, FUNCTION_DOES_NOT_EXIST, "The function specified by a call does not exist. There are %d functions. Valid function ids are 0-%d. The value specified by the entry point is: %d", exe->number_of_functions, exe->number_of_functions, frame.function_id);

				if(stack_ptr->stack_pointer < exe->functions[frame.function_id].arg_count)
					error(exe, NOT_ENOUGH_ARGUMENTS, "Arguments required %d. Recieved %d", exe->functions[frame.function_id].arg_count, stack_ptr->stack_pointer + 1);
					
				stack_init(&frame.stack);
				for(int x = 0; x < exe->functions[frame.function_id].arg_count; x++)
					stack_push(&frame.stack, stack_pop(stack_ptr));
				
				stack_push(&exe->call_stack, (long)&frame);
				
				stack_push(stack_ptr, dexe_run_function(exe));
				
				stack_pop(&exe->call_stack);
							
				sf->pc += 4;
				
				break;
			}
			case Ret:
			{
				//Is there anything remaining on the stack? That's our return value. If not, maybe this is a void function? return 0
				int ret_value = stack_ptr->stack_pointer < 1 ? 0 : stack_pop(stack_ptr);
				stack_free(stack_ptr);
				free(sf->local_memory);
				
				return ret_value;
			}
			default:
				error(exe, INVALID_OPCODE, "Invalid opcode recieved: 0x%X", opcode);
		}
	}
	
	error(exe, ABRUPT_END_OF_FUNCTION, "This occured in function %d", sf->function_id);
}


void breakpoint(Executable* exe)
{
	Stack_Frame* sf = (Stack_Frame*)stack_peek(&exe->call_stack);
	int locals = exe->functions[sf->function_id].local_count;
	int exit = 0;
	int debug = exe->info->commandline & COMMANDLINE_DEBUG;
	
	puts("\nDebugger) [e - examine all memory] [s - stack] [d - call stack] [c - continue]");
	do
	{
		
		printf("\nDebugger) ");
		switch(getc(stdin))
		{
			case 'e':
			case 'E':
				if(debug)
					puts(" id     name      value");
				else
					puts(" id  value");
				for(int i = 0; i < locals; i++)
				{
					if(debug)
						printf("%3d) [%-10s] %-10d\n", i, exe->functions[sf->function_id].local_names[i],sf->local_memory[i]);
					else
						printf("%3d) %-10d\n",i, sf->local_memory[i]);
				}
				break;
			case 'c':
			case 'C':
				exit = 1;
				break;
			case 's':
			case 'S':
				puts("Unwinding the stack:");
				if(!sf->stack.stack_pointer)
					puts("  [Empty]");
				for(int i = sf->stack.stack_pointer-1; i >= 0; i--)
				{
					printf("  %ld\n", sf->stack.stack_elements[i]);
				}
				break;

			case 'd':
			case 'D':
				puts("Unwinding the call stack:");
				for(int i = exe->call_stack.stack_pointer -1; i >= 0; i--)
				{
					if(debug)
						printf("  %s @ %d\n", exe->functions[((Stack_Frame*)exe->call_stack.stack_elements[i])->function_id].function_name, ((Stack_Frame*)exe->call_stack.stack_elements[i])->pc);
					else
						printf("  %d @ %d\n", ((Stack_Frame*)exe->call_stack.stack_elements[i])->function_id, ((Stack_Frame*)exe->call_stack.stack_elements[i])->pc);
				}
				break;

			default:
				puts("Unknown option");
				break;
		}
		//flush the stream
		while(getc(stdin) != '\n');
	} while(!exit);
}