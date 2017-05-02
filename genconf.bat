@echo off

rem Helper script to configure environment for building with genproj tool.
rem Running it requires that Tcl should be in the PATH

SET "OLD_PATH=%PATH%"

rem create env.bat if it does not exist yet
if not exist "%~dp0env.bat" (
  type "%~dp0adm\templates\env.bat" | findstr /i /v "__CASROOT__" > "%~dp0env.bat"
)
call "%~dp0env.bat"

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

rem Prompt the user to specify location of Tcl if not found in PATH
if not defined TCL_FOUND (
  set /P TCL_PATH=This script requires Tcl, but tclsh.exe cannot be found in PATH.^

Please enter path to folder containing tclsh.exe^

^: 
  rem NOTE: KEEP LINE AFTER "set" ABOVE EMPTY !
)

if exist %TCL_PATH%\tclsh.exe (
  set "TCL_FOUND=%TCL_PATH%\tclsh.exe"
) else if exist %TCL_PATH%\tclsh86.exe (
  set "TCL_FOUND=%TCL_PATH%\tclsh86.exe"
) else (
  set "TCL_EXEC=%TCL_PATH%\tclsh.exe"
)

rem Initialize custom.bat if it does not exist yet
rem if not exist %%dp0custom.bat (
rem   echo set "PATH=%%PATH%%;%TCL_PATH%" >%~dp0custom.bat
rem )

rem fail if Tcl is not found
if not defined TCL_FOUND (
  echo Error: "%TCL_EXEC%" is not found. Please update PATH variable ^(use custom.bat^)
  goto :eof
) 

rem run GUI tool
"%TCL_FOUND%" %~dp0adm/genconf.tcl

SET "PATH=%OLD_PATH%"
