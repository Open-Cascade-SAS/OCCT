@echo off

rem Helper script to run generation of VS projects on Windows.
rem Running it requires that Tcl should be in the PATH

SET "OLD_PATH=%PATH%"

if exist "%~dp0env.bat" (
  call "%~dp0env.bat"
)

set "TCL_EXEC=tclsh.exe"

for %%X in (%TCL_EXEC%) do (set TCL_FOUND=%%~$PATH:X)

if not defined TCL_FOUND (
  echo "Error. %TCL_EXEC% is not found. Please update PATH variable"
  goto :eof
)

if not exist "%~dp0custom.bat" (
  %TCL_EXEC% %~dp0adm/genconf.tcl
)

if not exist "%~dp0custom.bat" (
  echo custom.bat is not created. Run the script again or create custom.bat manually
  goto :eof
)

if exist "%~dp0env.bat" (
  call "%~dp0env.bat"
)

cd %~dp0
%TCL_EXEC% %~dp0adm/start.tcl genproj -path=. -target=%VCVER%
SET "PATH=%OLD_PATH%"
