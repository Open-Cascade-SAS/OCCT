call "%~dp0..\..\..\env.bat" %1 %2 %3
call "%~dp0env.bat"

set "BIN_DIR=win%ARCH%\%VCVER%\bind"

IF ["%CASDEB%"] == [""] (
  set "BIN_DIR=win%ARCH%\%VCVER%\bin"
)

if not exist "%~dp0%BIN_DIR%\IESample.exe" goto ERR_EXE

echo Starting IESample .....
"%~dp0%BIN_DIR%\IESample.exe"

goto END

:ERR_EXE
echo Executable %~dp0%BIN_DIR%\IESample.exe not found.
echo Probably you didn't compile the application.
pause
goto END

:END