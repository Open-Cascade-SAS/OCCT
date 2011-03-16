@ECHO OFF

call env.bat

IF NOT DEFINED QTDIR GOTO ERR_QTDIR

REM *** To start build on MSVC6 ***
rem msdev.exe Graphic3dDemo.dsw

REM *** To start build on MSVC7 ***
start "%VS71COMNTOOLS%\..\IDE\devenv.exe" Graphic3dDemo.sln

GOTO END

:ERR_QTDIR
ECHO Environment variable "QTDIR" has not been defined. Abort.
GOTO END

:END
