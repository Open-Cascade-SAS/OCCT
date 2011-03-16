@ECHO OFF

call "%~dp0..\..\..\ros\env.bat"

IF NOT DEFINED CASROOT GOTO ERR_CASROOT

IF NOT EXIST "%CASROOT%\win32\vc8\bin\TKernel.dll" GOTO ERR_CASCADE

SET CASLIBS=%CASROOT%\win32\vc8\bin

SET ROOT=%CASROOT%\..

set RES_DIR=%~dp0res
set BIN_DIR=%~dp0win32\bin
SET CSF_ResourcesDefaults=%RES_DIR%
SET CSF_IEResourcesDefaults=%RES_DIR%

SET PATH=%CASLIBS%;%QTDIR%\bin;%PATH%

GOTO END

:ERR_CASROOT
ECHO Environment variable "CASROOT" has not been defined. Abort.
pause
exit /B

:ERR_CASCADE
ECHO Environment variable "CASROOT" defined incorrectly. Abort.
pause
exit /B

:END
