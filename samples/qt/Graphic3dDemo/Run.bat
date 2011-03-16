@ECHO OFF

call env.bat

IF NOT EXIST win32\obj goto QTSET 

IF NOT EXIST "%QTDIR%" goto QTSET

goto QT

:QTSET

SET QTDIR=%ROOT%\3rdparty\win32\qt
SET PATH=%QTDIR%;%PATH%

:QT

if not exist %BIN_DIR%\Graphic3dDemo.exe goto ERR_EXE

@echo Starting Graphic3dDemo .....
start .\%BIN_DIR%\Graphic3dDemo.exe

GOTO END

:ERR_EXE
ECHO Executable %BIN_DIR%\Graphic3dDemo.exe not found."
ECHO Probably you didn't compile the application.
GOTO END

:END
