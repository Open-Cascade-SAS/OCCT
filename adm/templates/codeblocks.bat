@echo off

rem Setup environment
call "%~dp0env.bat" %1 %2 %3

rem Define path to project file
set "PRJFILE=%~dp0\adm\wnt\cbp\OCCT.workspace"
if not exist "%PRJFILE%" set "PRJFILE=%~dp0\adm\wnt\cbp\Products.workspace"
if not "%4" == "" (
  set "PRJFILE=%4"
)

if "%CB_PATH%"=="" if exist "%PROGRAMFILES%\CodeBlocks" set "CB_PATH=%PROGRAMFILES%\CodeBlocks\codeblocks.exe"
if "%CB_PATH%"=="" if exist "%PROGRAMFILES(x86)%\CodeBlocks" set "CB_PATH=%PROGRAMFILES(x86)%\CodeBlocks\codeblocks.exe"
if "%CB_PATH%"=="" set "CB_PATH=codeblocks.exe"

set "CASBIN=wnt\cbp"
set "PATH=%~dp0%CASBIN%\bin%CASDEB%;%PATH%"

start "" "%CB_PATH%" "%PRJFILE%"
