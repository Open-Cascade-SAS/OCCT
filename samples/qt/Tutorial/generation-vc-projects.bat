REM Generation of vcproj files with qmake utilite
REM Variable QTDIR and PATH to qmake executable must be defined without fail

REM Use first argument to specify version of Visual Studio (vc8, vc9, or vc10),
REM second argument specifies architecture) (win32 or win64)

call "%~dp0..\..\..\ros\env_build.bat" %1 %2

IF NOT DEFINED QTDIR GOTO ERR_QTDIR

SET PATH=%QTDIR%\bin;%PATH%

qmake -tp vc -r Tutorial.pro

GOTO END
:ERR_QTDIR
ECHO Environment variable "QTDIR" has not been defined. Abort.

:END
