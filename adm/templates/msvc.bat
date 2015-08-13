@echo off

rem Setup environment
call "%~dp0env.bat" %1 %2 %3

rem Define path to project file
set "PRJFILE=%~dp0\adm\msvc\%VCVER%\OCCT.sln"
if not exist "%PRJFILE%" set "PRJFILE=%~dp0\adm\msvc\%VCVER%\Products.sln"
if not "%4" == "" (
  set "PRJFILE=%4"
)

set "VisualStudioExpressName=VCExpress"

if "%VCVER%" == "vc8" (
  set "DevEnvDir=%VS80COMNTOOLS%..\IDE"
) else if "%VCVER%" == "vc9" (
  set "DevEnvDir=%VS90COMNTOOLS%..\IDE"
) else if "%VCVER%" == "vc10" (
  set "DevEnvDir=%VS100COMNTOOLS%..\IDE"
) else if "%VCVER%" == "vc11" (
  set "DevEnvDir=%VS110COMNTOOLS%..\IDE"
  rem Visual Studio Express starting from VS 2012 is called "for Windows Desktop"
  rem and has a new name for executable - WDExpress
  set "VisualStudioExpressName=WDExpress"
) else if "%VCVER%" == "vc12" (
  set "DevEnvDir=%VS120COMNTOOLS%..\IDE"
  set "VisualStudioExpressName=WDExpress"
) else if "%VCVER%" == "vc14" (
  set "DevEnvDir=%VS140COMNTOOLS%..\IDE"
) else (
  echo Error: wrong VS identifier
  exit /B
)

rem Launch Visual Studio - either professional (devenv) or Express, as available
if exist "%DevEnvDir%\devenv.exe"  (
  start "" "%DevEnvDir%\devenv.exe" "%PRJFILE%"
) else if exist "%DevEnvDir%\%VisualStudioExpressName%.exe"  (
  start "" "%DevEnvDir%\%VisualStudioExpressName%.exe" "%PRJFILE%"
) else (
  echo Error: Could not find MS Visual Studio ^(%VCVER%^)
  echo Check relevant environment variable ^(e.g. VS80COMNTOOLS for vc8^)
)
