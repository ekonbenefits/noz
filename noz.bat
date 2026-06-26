@echo off
rem Sample launcher for noz.
rem Usage: noz.bat file1 [file2] ...

rem Windows cmd/powershell shells set OS=Windows_NT.
rem DOS, DOSBox, and DOSBox-X typically do not.
if "%OS%"=="Windows_NT" goto run_win32
goto run_dos16

:run_dos16
noz_16.exe %1 %2 %3 %4 %5 %6 %7 %8 %9
goto end

:run_win32
noz.exe %1 %2 %3 %4 %5 %6 %7 %8 %9

:end
