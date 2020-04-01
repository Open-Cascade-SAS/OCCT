@echo off
setlocal

rem Setup environment
call "%~dp0env.bat" %1 %2 %3

rem Define path to project file
set "PRJFILE=%~dp0OCCT.sln"

if "%VCVER%" == "vc8" (
  call "%VS80COMNTOOLS%/vsvars32.bat" > nul
) else if "%VCVER%" == "vc9" (
  call "%VS90COMNTOOLS%/vsvars32.bat" > nul
) else if "%VCVER%" == "vc10" (
  call "%VS100COMNTOOLS%/vsvars32.bat" > nul
) else if "%VCVER%" == "vc11" (
  call "%VS110COMNTOOLS%/vsvars32.bat" > nul
) else if "%VCVER%" == "vc12" (
  call "%VS120COMNTOOLS%/vsvars32.bat" > nul
) else if "%VCVER%" == "vc14" (
  call "%VS140COMNTOOLS%/vsvars32.bat" > nul
) else if "%VCVER%" == "vc141" (
  call "%VS141COMNTOOLS%/vsvars32.bat" > nul
) else if "%VCVER%" == "vc142" (
  call "%VS142COMNTOOLS%/vsvars32.bat" > nul
) else (
  echo Error: wrong VS identifier
  exit /B
)

set BUILDCONFIG=Release
if "%CASDEB%"=="i" set BUILDCONFIG=RelWithDebInfo
if "%CASDEB%"=="d" set BUILDCONFIG=Debug
if "%ARCH%"=="32" set PLATFORM=win32
if "%ARCH%"=="64" set PLATFORM=x64

msbuild "%PRJFILE%" /m /fl /flp:LogFile="build_%BUILDCONFIG%.log" /p:Configuration=%BUILDCONFIG% /p:Platform=%PLATFORM% /p:BuildProjectReferences=false
endlocal
