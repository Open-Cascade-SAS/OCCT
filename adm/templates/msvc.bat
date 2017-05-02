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

if not "%DevEnvDir%" == "" (
  rem If DevEnvDir is already defined (e.g. in custom.bat), use that value
) else if /I "%VCFMT%" == "vc8" (
  set "DevEnvDir=%VS80COMNTOOLS%..\IDE"
) else if /I "%VCFMT%" == "vc9" (
  set "DevEnvDir=%VS90COMNTOOLS%..\IDE"
) else if /I "%VCFMT%" == "vc10" (
  set "DevEnvDir=%VS100COMNTOOLS%..\IDE"
) else if /I "%VCFMT%" == "vc11" (
  set "DevEnvDir=%VS110COMNTOOLS%..\IDE"
  rem Visual Studio Express starting from VS 2012 is called "for Windows Desktop"
  rem and has a new name for executable - WDExpress
  set "VisualStudioExpressName=WDExpress"
) else if /I "%VCFMT%" == "vc12" (
  set "DevEnvDir=%VS120COMNTOOLS%..\IDE"
  set "VisualStudioExpressName=WDExpress"
) else if /I "%VCFMT%" == "vc14" (
  set "DevEnvDir=%VS140COMNTOOLS%..\IDE"
) else if /I "%VCFMT%" == "vc141" (
  for /f "usebackq delims=" %%i in (`vswhere.exe -version "[15.0,15.99]" -requires Microsoft.VisualStudio.Workload.%VCPROP% -property installationPath`) do (
    set DevEnvDir=%%i\Common7\IDE
  )
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
  echo For VS 2010-2015, check relevant environment variable ^(e.g. VS100COMNTOOLS^)
)
