@ECHO OFF

call "%~dp0env.bat"

IF NOT EXIST win32\obj goto QTSET 

IF NOT EXIST "%QTDIR%" goto QTSET

goto QT

:QTSET

SET QTDIR=%ROOT%\3rdparty\win32\qt
SET PATH=%QTDIR%;%PATH%

:QT

if not exist "%BIN_DIR%\Tutorial.exe" goto ERR_EXE

echo %QTDIR%
echo Starting IESample .....
start /D "%BIN_DIR%" Tutorial.exe



GOTO END

:ERR_EXE
ECHO Executable %BIN_DIR%\Tutorial.exe not found."
ECHO Probably you didn't compile the application.
pause
GOTO END

:END

