@echo off

call "%~dp0env.bat"

if not exist win32\obj goto QTSET 

if not exist "%QTDIR%" goto QTSET

goto QT

:QTSET

set QTDIR=%ROOT%\3rdparty\win32\qt
set PATH=%QTDIR%;%PATH%

:QT

if not exist "%BIN_DIR%\IESample.exe" goto ERR_EXE

echo Starting IESample .....
start /D "%BIN_DIR%" IESample.exe

goto END

:ERR_EXE
echo Executable %BIN_DIR%\IESample.exe not found."
echo Probably you didn't compile the application.
pause
goto END

:END
