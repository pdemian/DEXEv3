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

//includes
#include "dexe_utils.h"
#include "dexe_parser.h"
#include "dexe_executer.h"

//prototypes
void dump(Executable*);
void print_help();
void print_version();
char* get_commandline(int*,int,char**);
void handle_commandline(Executable*);
void decompile(Executable*);

//functions
int main(int argc, char** argv)
{
	Executable exe;

	//set up executable
	exe.info = (Dexe_Info*)malloc(sizeof(Dexe_Info));
	if(exe.info == NULL)
		error(&exe, ALLOCATION_ERROR_IN_MAIN, "The info struct (containing the filename, commandline args, and file pointer) could not be allocated.");
	exe.info->commandline = 0;
	exe.info->file = NULL;
	exe.call_stack = (stack){0,0,0};
	exe.functions = NULL;

	//get command line arguments
	exe.info->filename = get_commandline(&exe.info->commandline, argc, argv);

	//handle command line
	if(exe.info->filename == NULL && !(exe.info->commandline & COMMANDLINE_VERSION)) 
		exe.info->commandline |= COMMANDLINE_HELP;
	
	handle_commandline(&exe);
	
	//read file into memory
	dexe_read(&exe);
	
	//execute
	int ret_value = dexe_execute(&exe);
	
	//free resources
	free_memory(&exe);
	
	//debug exit
	if(exe.info->commandline & COMMANDLINE_DEBUG)
		error(&exe, OK, "The program executed without error");
		
	//normal exit
	return ret_value;
}

void print_help()
{
	puts("Usage: dexe [options] file");
	puts("Options:");
	puts("  -h,  -help           Display this information");
	puts("  -v,  -version        Display version information");
	puts("  -db, -debug          Turn on debug. Allows for breakpoints. Implies -v");
	puts("  -dp, -dump           Dump file information");
	puts("  -dc, -decompile      Decompile the file. Implies -dump");
	puts("  -s,  -silent         Silent errors (exit immediately on error)");
	puts("  -vb, -verbose        Verbose errors (print additional information on error)");
	puts("");
	puts("Note, Unix style double dash specifiers (eg, --help) are also accepted.");
	exit(EXIT_SUCCESS);
}
void print_version()
{
	printf("dexe (DEXE interpreter) Version %d.%d.%d\n\n", DEXE_MAJOR_VERSION, DEXE_MINOR_VERSION, DEXE_REVISION_VERSION);
	puts("Copyright (C) 2014 Patrick Demian");
	puts("Permission is hereby granted, free of charge, to any person obtaining a copy of");
	puts("this software and associated documentation files (the \"Software\"), to deal in");
	puts("the Software without restriction, including without limitation the rights to");
	puts("use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies");
	puts("of the Software, and to permit persons to whom the Software is furnished to do");
	puts("so, subject to the following conditions:");
	puts("");
	puts("The above copyright notice and this permission notice shall be included in all");
	puts("copies or substantial portions of the Software.");
	puts("");
	puts("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
	puts("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	puts("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE");
	puts("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	puts("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	puts("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN ");
	puts("THE SOFTWARE.");
	exit(EXIT_SUCCESS);
}


char* get_commandline(int* commandline, int argc, char** argv)
{
	char* ptr = NULL;

	for(int i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			//get command line options
			if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "-help") || !strcmp(argv[i], "-h"))
			{
				*commandline |= COMMANDLINE_HELP;
			}
			else if(!strcmp(argv[i], "--version") || !strcmp(argv[i], "-version") || !strcmp(argv[i], "-v"))
			{
				*commandline |= COMMANDLINE_VERSION;
			}
			else if(!strcmp(argv[i], "--debug") || !strcmp(argv[i], "-debug") || !strcmp(argv[i], "-db"))
			{
				*commandline |= COMMANDLINE_DEBUG | COMMANDLINE_VERBOSE;
			}
			else if(!strcmp(argv[i], "-dump") ||  !strcmp(argv[i], "-dump") || !strcmp(argv[i], "-dp"))
			{
				*commandline |= COMMANDLINE_DUMP;
			}
			else if(!strcmp(argv[i], "--decompile") || !strcmp(argv[i], "-decompile") || !strcmp(argv[i], "-dc"))
			{
				*commandline |= COMMANDLINE_DECOMPILE | COMMANDLINE_DUMP;
			}
			else if(!strcmp(argv[i], "--silent") ||  !strcmp(argv[i], "-silent") || !strcmp(argv[i], "-s"))
			{
				*commandline |= COMMANDLINE_SILENT;
			}
			else if(!strcmp(argv[i], "--verbose") || !strcmp(argv[i], "-verbose") || !strcmp(argv[i], "-vb"))
			{
				*commandline |= COMMANDLINE_VERBOSE;
			}
			else
			{
				printf("%s warning: ignoring unrecognized option '%s'\n\n", argv[0], argv[i]);
			}
		}
		else
		{
			//what's left is (probably) the file location/file name
			ptr = argv[i];
		}
	}
	
	return ptr;
}

void handle_commandline(Executable* exe)
{

	if(exe->info->commandline & COMMANDLINE_HELP)
	{
		print_help();
	}
	else if(exe->info->commandline & COMMANDLINE_VERSION)
	{
		print_version();
	}
	else if(exe->info->commandline & COMMANDLINE_DUMP)
	{
		dexe_read(exe);
		dump(exe);
		if(exe->info->commandline & COMMANDLINE_DECOMPILE)
		{
			decompile(exe);
		}
		free_memory(exe);
		exit(EXIT_SUCCESS);
	}
}

void dump(Executable* exe)
{
	puts("Dexe dump\n");
	
	printf("Filename: %s\n", exe->info->filename);

	printf("Version:  %u.%u.%u\n", (exe->version >> 16) & 0xFF, (exe->version >> 8) & 0xFF, exe->version & 0xFF);
	printf("Entry:    Function %d\n", exe->entry);
	printf("Flags:    (0x%X)\n", exe->flags);
	if(exe->flags & DEXE_FLAGS_DEBUG)
		puts("  Debug symbols included");
	else
		puts("  Debug symbols stripped");
	if(exe->flags & DEXE_FLAGS_EXECUTABLE)
		puts("  Executable file");
	else
		puts("  Code library file");
	
	printf("\nNumber of functions: %d\n\n", exe->number_of_functions);
	
	for(int i = 0; i < exe->number_of_functions; i++)
	{
		printf("  [Function %d]\n", i);
	
		if(exe->flags & DEXE_FLAGS_DEBUG)
			printf("    Function name: %s\n", exe->functions[i].function_name);
		printf("    Argument count: %d\n", exe->functions[i].arg_count);
		
		for(int k = 0; k < exe->functions[i].arg_count && exe->flags & DEXE_FLAGS_DEBUG; k++)
			printf("      %d) %s\n",k,exe->functions[i].arg_names[k]);
			
		printf("    Locals count: %d\n", exe->functions[i].local_count);
		
		for(int k = 0; k < exe->functions[i].local_count && exe->flags & DEXE_FLAGS_DEBUG; k++)
			printf("      %d) %s\n", k, exe->functions[i].local_names[k]);
		
		printf("    Size of code: %d\n\n", exe->functions[i].size_of_instructions);
	}
	puts("\nEnd dump");
}
void decompile(Executable* exe)
{
	puts("\nDecompiled code:\n");

	int has_debug_symbols = exe->flags & DEXE_FLAGS_DEBUG;
	
	for(int i = 0; i < exe->number_of_functions; i++)
	{
		if(has_debug_symbols)
		{
			//maximum possible string length is 66,560, but I doubt anyone will have 255 
			//arguments with a length of 255 characters. This buffer can handle a maximum of 15 arguments
			
			//I guess making this a call to malloc instead would be better.
			char buffer[4096] = "";
			
			if(exe->functions[i].arg_count > 0)
			{
				sprintf(buffer, "int %s", exe->functions[i].arg_names[0]);
				
				for(int x = 1; x < exe->functions[i].arg_count; x++)
					sprintf(buffer, "%s, int %s", buffer, exe->functions[i].arg_names[x]);
			}
			printf("function %s (%s):\n", exe->functions[i].function_name, buffer);
		}
		else
			printf("function %d (int[%d]):\n", i, exe->functions[i].arg_count);
			
		for(int k = 0; k < exe->functions[i].size_of_instructions; k++)
		{
			char opcode = exe->functions[i].instructions[k];
			
			if(opcode < 0 || opcode > Ret)
			{
				puts("UNKNOWN");
				continue;
			}
		
			Opcode instruct = get_opcode_from_instruction(opcode);
			
			switch(instruct.opcode)
			{
				case Nop:
				case Break:
				case Dup:
				case Pop:
				case Inc:
				case Dec:
				case Add:
				case Sub:
				case Mul:
				case Div:
				case Rem:
				case And:
				case Or:
				case Xor:
				case Not:
				case Neg:
				case Shl:
				case Shr:
				case Cmp:
				case In:
				case Out:
				case Ret:
					printf("  %s\n", instruct.mnemonic);
					break;
					
				case Jmp:
				case Je:
				case Jne:
				case Jg:
				case Jge:
				case Jl:
				case Jle:
					printf("  %s %d\n", instruct.mnemonic, bytes_to_int(exe->functions[i].instructions + k + 1));
					k += 4;
					break;
					
				case Call:
					if(has_debug_symbols)
					{
						int id = bytes_to_int(exe->functions[i].instructions + k + 1);
						char* name;
						
						if(id < 0 || id > exe->number_of_functions)
							name = "UNKNOWN";
						else
							name = exe->functions[i].function_name;

						printf("  %s %d [%s]\n", instruct.mnemonic, id, name);
					}
					else
						printf("  %s %d\n", instruct.mnemonic, bytes_to_int(exe->functions[i].instructions + k + 1));
					
					k += 4;
					break;
					
				case Push:
					printf("  %s 0x%X\n", instruct.mnemonic, bytes_to_int(exe->functions[i].instructions + k + 1));
					k += 4;
					break;
					
				case Load:
				case Store:
					if(has_debug_symbols)
					{
						int id = exe->functions[i].instructions[++k];
						char* name = (id < 0 || id > exe->functions[i].local_count ? "UNKNOWN" : exe->functions[i].local_names[id]);
						
						printf("  %s 0x%X [%s]\n", instruct.mnemonic, id, name);
					}
					else
						printf("  %s 0x%X\n", instruct.mnemonic, exe->functions[i].instructions[++k]);
					break;
			}
		}
		puts("end\n");
	}
	puts("\nEnd decompile");
}