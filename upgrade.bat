@echo off

rem Helper script to run procedure of automatic upgrade of application code
rem on newer version of OCCT on Windows.
rem Running it requires that Tcl should be in the PATH

SET "OLD_PATH=%PATH%"

if exist "%~dp0env.bat" (
  call "%~dp0env.bat"
)

set "TCL_EXEC=tclsh.exe"

for %%X in (%TCL_EXEC%) do (set TCL_FOUND=%%~$PATH:X)

if defined TCL_FOUND (
  %TCL_EXEC% %~dp0adm/start.tcl upgrade %*
) else (
  echo "Error. %TCL_EXEC% is not found. Please update PATH variable"
)

SET "PATH=%OLD_PATH%"
