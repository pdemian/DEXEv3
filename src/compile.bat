@echo off

REM compile icon
windres "icon.rc" -O coff -o "icon.res"

if NOT %ERRORLEVEL% EQU 0 (
	pause
)

REM compile project
gcc -O3 -Wdisabled-optimization -Wall  -Wextra -Wno-unused -Wno-int-to-pointer-cast -Wunreachable-code -Winline -Wuninitialized -pedantic-errors -Wfloat-equal -Wcast-qual -Wcast-align -std=c99 "dexe_main.c" "dexe_utils.c" "dexe_stack.c" "dexe_parser.c" "dexe_executer.c" "icon.res" -o "dexe" 

if NOT %ERRORLEVEL% EQU 0 (
	pause
)

REM cleanup
del icon.res