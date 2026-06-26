@echo off
rem Sample launcher for noz.
rem Run this from the directory that contains noz.exe and noz_16.exe.
rem Usage: noz.bat file1 [file2] ... [file9]

rem Reliable split:
rem - Windows cmd/powershell shells set OS=Windows_NT -> use Win32
rem - DOS, DOSBox, and DOSBox-X usually do not -> use DOS16
if "%OS%"=="Windows_NT" goto run_win32
goto run_dos16

:run_dos16
if exist noz_16.exe goto exec_dos16
if exist noz16.exe goto exec_dos16_alt
echo Could not find noz_16.exe or noz16.exe in this directory.
goto end

:exec_dos16
noz_16.exe %1 %2 %3 %4 %5 %6 %7 %8 %9
goto end

:exec_dos16_alt
noz16.exe %1 %2 %3 %4 %5 %6 %7 %8 %9
goto end

:run_win32
if exist noz.exe goto exec_win32
echo Could not find noz.exe in this directory.
goto end

:exec_win32
noz.exe %1 %2 %3 %4 %5 %6 %7 %8 %9

:end
