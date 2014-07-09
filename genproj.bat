@echo off

rem Helper script to run generation of VS projects on Windows.
rem Running it requires that Tcl should be in the PATH

SET "OLD_PATH=%PATH%"

if not exist "%~dp0custom.bat" (
  tclsh.exe ./adm/genconf.tcl
)

if not exist "%~dp0custom.bat" (
  echo custom.bat is not created. Run the script again and generate custom.bat
  goto :eof
) else (
  call "%~dp0custom.bat"
)

if exist "%~dp0env.bat" (
  call "%~dp0env.bat"
)

cd %~dp0
tclsh.exe ./adm/genproj.tcl -path=. -target=%VCVER% 
SET "PATH=%OLD_PATH%"
