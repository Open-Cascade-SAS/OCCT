@echo off

rem Setup environment
call "%~dp0env.bat" %1 %2 %3

rem Define path to project file
set "PRJFILE=%~dp0\adm\msvc\%VCVER%\OCCT.sln"
if not exist "%PRJFILE%" set "PRJFILE=%~dp0\adm\msvc\%VCVER%\Products.sln"
if not "%4" == "" (
  set "PRJFILE=%4"
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
