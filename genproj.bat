@echo off

rem Helper script to run generation of VS projects on Windows.
rem Running it requires that Tcl should be in the PATH

SET "OLD_PATH=%PATH%"

rem run GUI configurator if custom.bat is missing
if not exist "%~dp0custom.bat" (
  call %~dp0genconf.bat
)

if not exist "%~dp0custom.bat" (
  echo Error: custom.bat is not present.
  echo Run the script again to generate custom.bat, or create it manually
  goto :eof
)

if exist "%~dp0env.bat" (
  call "%~dp0env.bat"
)

rem  find Tcl
set "TCL_EXEC=tclsh.exe"
for %%X in (%TCL_EXEC%) do (set TCL_FOUND=%%~$PATH:X)

set "TCL_EXEC2=tclsh86.exe"
if not defined TCL_FOUND (
  for %%X in (%TCL_EXEC2%) do (
    set TCL_FOUND=%%~$PATH:X
    set TCL_EXEC=%TCL_EXEC2%
  )
)

rem fail if Tcl is not found
if not defined TCL_FOUND (
  echo Error: "%TCL_EXEC%" is not found. Please update PATH variable ^(use custom.bat^)
  goto :eof
) 

cd %~dp0
%TCL_EXEC% %~dp0adm/start.tcl genproj -path=. -target=%VCVER%
SET "PATH=%OLD_PATH%"
