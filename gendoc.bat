@echo off

rem Helper script to run generation of OCCT documentation on Windows.
rem Running it requires that Tcl, Doxygen, and MikTex (for PDF generation)
rem should be in the PATH

SET "OLD_PATH=%PATH%"

if exist "%~dp0env.bat" (
  call "%~dp0env.bat"
)

tclsh.exe %~dp0dox/start.tcl  %*
SET "PATH=%OLD_PATH%"
