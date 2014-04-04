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
#include "dexe_stack.h"


int stack_init(stack* st)
{
	st->stack_elements = (long*)malloc(DEFAULT_STACK_SIZE * sizeof(long));

	if(st->stack_elements == NULL)
		return 1;
	
	st->length = DEFAULT_STACK_SIZE;
	st->stack_pointer = 0;
	
	return 0;
}
void stack_free(stack* st)
{
	if(st != NULL)
	{
		free(st->stack_elements);
		
		st = NULL;
	}
}

void stack_empty(stack* st)
{
	st->stack_pointer = 0;
}

int stack_push(stack* st, long value)
{
	if(st->stack_pointer >= st->length)
	{
		//allocate more memory
	
		st->length += INCREASE_STACK_SIZE_BY;
		long* ptr = (long*)realloc(st->stack_elements, st->length * sizeof(long)); 
		
		if(ptr == NULL)
			return 1;
		
		st->stack_elements = ptr;
	}
	
	st->stack_elements[st->stack_pointer++] = value;
	
	return 0;
}

//Removed pushing Executable* and Opcode* and testing for range
//This now must be done via the calling method.
long stack_peek(stack* st)
{
	return st->stack_elements[st->stack_pointer-1];
}
long stack_pop(stack* st)
{
	return st->stack_elements[--st->stack_pointer];
}