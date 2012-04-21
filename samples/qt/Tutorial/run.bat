call ../../../env.bat %1 %2 %3
call %~dp0env.bat

IF not "%CASDEB%" EQU "" (
SET BIN_DIR=win%ARCH%\%VCVER%\bind
) ELSE (
SET BIN_DIR=win%ARCH%\%VCVER%\bin
)

if not exist "%BIN_DIR%\Tutorial.exe" goto ERR_EXE

echo Starting Tutorial .....
start /D "%BIN_DIR%" Tutorial.exe

goto END

:ERR_EXE
echo Executable %BIN_DIR%\Tutorial.exe not found."
echo Probably you didn't compile the application.
pause
goto END

:END