REM @ECHO OFF

CALL "%~dp0env.bat" %1 %2 %3

IF NOT DEFINED VCVER GOTO ERR_VCVER

IF NOT DEFINED OS_TYPE GOTO ERR_OSTYPE

IF %VCVER%==vc8 (
start devenv.exe All-%VCVER%.sln /useenv
) ELSE (
IF %VCVER%==vc9 ( 
start "MSDEV" "devenv.exe" All-%VCVER%.sln /useenv
) ELSE (
IF %VCVER%==vc10 (   
start "MSDEV" "devenv.exe" All-%VCVER%.sln /useenv
) ) )

GOTO END

:ERR_VCVER
ECHO Environment variable "VCVER" has not been defined. Abort.
GOTO END

:ERR_OSTYPE
ECHO Environment variable "OS_TYPE" has not been defined. Abort.
GOTO END

:END
