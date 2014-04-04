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
#include "dexe_utils.h"

#define DEFAULT_STACK_SIZE 20
#define INCREASE_STACK_SIZE_BY 20



struct Stack_struct
{
	int length;
	int stack_pointer;
	long* stack_elements;
};
typedef struct Stack_struct stack;


extern int stack_init(stack*);
extern void stack_free(stack*);
extern void stack_empty(stack*);

extern int stack_push(stack*, long);
extern long stack_peek(stack*);
extern long stack_pop(stack*);