@echo off

rem Setup environment and launch DRAWEXE
call "%~dp0env.bat"

echo Hint: use "pload ALL" command to load standard commands
DRAWEXE.exe %*

set "PATH=%ORIGIN_PATH%"

