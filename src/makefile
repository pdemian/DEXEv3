CC = gcc
OUTPUT = -o dexe

CFLAGS = -c -Wall -Wextra -Wno-unused -Wunreachable-code -Winline -Wuninitialized -pedantic-errors -Wfloat-equal -Wcast-qual -Wcast-align -std=c99

OPTIMIZEFLAGS = -O3 -Wdisabled-optimization
DEBUGFLAGS = -g -ggdb

#if you want to have a release, change to $(OPTIMIZEFLAGS), else leave as $(DEBUGFLAGS)
EXTRAFLAGS = $(DEBUGFLAGS)

all: dexe

dexe: dexe_main.o dexe_utils.o dexe_stack.o dexe_parser.o dexe_executer.o
	$(CC) dexe_main.o dexe_utils.o dexe_stack.o dexe_parser.o dexe_executer.o $(OUTPUT)


dexe_main.o: dexe_main.c
	$(CC) $(EXTRAFLAGS) $(CFLAGS) dexe_main.c
	
dexe_utils.o: dexe_utils.c
	$(CC) $(EXTRAFLAGS) $(CFLAGS) dexe_utils.c
	
dexe_stack.o: dexe_stack.c
	$(CC) $(EXTRAFLAGS) $(CFLAGS) dexe_stack.c
	
dexe_parser.o: dexe_parser.c
	$(CC) $(EXTRAFLAGS) $(CFLAGS) dexe_parser.c
	
dexe_executer.o: dexe_executer.c
	$(CC) $(EXTRAFLAGS) $(CFLAGS) dexe_executer.c

clean:
	rm -rf *o dexe
