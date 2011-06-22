@ECHO OFF

call env.bat

IF NOT DEFINED QTDIR GOTO ERR_QTDIR

REM *** To start build on MSVC6 ***
REM msdev.exe Interface.dsw

REM *** To start build on MSVC7 ***
start "%VS71COMNTOOLS%\..\IDE\devenv.exe" Interface.sln

GOTO END

:ERR_QTDIR
ECHO Environment variable "QTDIR" not defined. Abort.
GOTO END

:END
