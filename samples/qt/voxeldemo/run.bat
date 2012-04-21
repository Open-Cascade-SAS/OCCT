call ../../../env.bat %1 %2 %3

call %~dp0env.bat

if not exist "%BIN_DIR%\VoxelDemo.exe" goto ERR_EXE

echo Starting VoxelDemo .....
start /D "%BIN_DIR%" VoxelDemo.exe

goto END

:ERR_EXE
echo Executable %BIN_DIR%\VoxelDemo.exe not found."
echo Probably you didn't compile the application.
pause
goto END

:END