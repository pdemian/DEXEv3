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

enum Instruction_Enum
{
	Nop   = 0x00,
	Break = 0x01,
	Load  = 0x02,
	Push  = 0x03,
	Store = 0x04,
	Dup   = 0x05,
	Pop   = 0x06,
	Inc   = 0x07,
	Dec   = 0x08,
	Add   = 0x09,
	Sub   = 0x0A,
	Mul   = 0x0B,
	Div   = 0x0C,
	Rem   = 0x0D,
	And   = 0x0E,
	Or    = 0x0F,
	Xor   = 0x10,
	Not   = 0x11,
	Neg   = 0x12,
	Shl   = 0x13,
	Shr   = 0x14,
	Cmp   = 0x15,
	Jmp   = 0x16,
	Je    = 0x17,
	Jne   = 0x18,
	Jg    = 0x19,
	Jge   = 0x1A,
	Jl    = 0x1B,
	Jle   = 0x1C,
	In    = 0x1D,
	Out   = 0x1E,
	Call  = 0x1F,
	Ret   = 0x20
};
typedef enum Instruction_Enum Instruction;


struct Opcode_Struct
{
	Instruction opcode;
	char* mnemonic;
	int parameter_size;
	int required_stack_size;
};
typedef struct Opcode_Struct Opcode;


static Opcode NOP   = { Nop,   "nop",   0,0};
static Opcode BREAK = { Break, "break", 0,0};
static Opcode LOAD  = { Load,  "load",  1,0};
static Opcode PUSH  = { Push,  "push",  4,0};
static Opcode STORE = { Store, "store", 1,1};
static Opcode DUP   = { Dup,   "dup",   0,1};
static Opcode POP   = { Pop,   "pop",   0,1};
static Opcode INC   = { Inc,   "inc",   0,1};
static Opcode DEC   = { Dec,   "dec",   0,1};
static Opcode ADD   = { Add,   "add",   0,2};
static Opcode SUB   = { Sub,   "sub",   0,2};
static Opcode MUL   = { Mul,   "mul",   0,2};
static Opcode DIV   = { Div,   "div",   0,2};
static Opcode REM   = { Rem,   "rem",   0,2};
static Opcode AND   = { And,   "and",   0,2};
static Opcode OR    = { Or,    "or",    0,2};
static Opcode XOR   = { Xor,   "xor",   0,2};
static Opcode NOT   = { Not,   "not",   0,1};
static Opcode NEG   = { Neg,   "neg",   0,1};
static Opcode SHL   = { Shl,   "shl",   0,2};
static Opcode SHR   = { Shr,   "shr",   0,2};
static Opcode CMP   = { Cmp,   "cmp",   0,2};
static Opcode JMP   = { Jmp,   "jmp",   4,0};
static Opcode JE    = { Je,    "je",    4,0};
static Opcode JNE   = { Jne,   "jne",   4,0};
static Opcode JG    = { Jg,    "jg",    4,0};
static Opcode JGE   = { Jge,   "jge",   4,0};
static Opcode JL    = { Jl,    "jl",    4,0};
static Opcode JLE   = { Jle,   "jle",   4,0};
static Opcode IN    = { In,    "in",    0,0};
static Opcode OUT   = { Out,   "out",   0,1};
static Opcode CALL  = { Call,  "call",  4,0};
static Opcode RET   = { Ret,   "ret",   0,1};