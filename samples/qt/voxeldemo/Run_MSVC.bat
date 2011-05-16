@echo off
rem Launch MS VC with environment prepared for building OCCT qt Voxel demo

rem Set build environment 

call %~dp0..\..\..\ros\env_build.bat vc8 win32


SET BIN_DIR=Release
SET CSF_GraphicShr=OpenGl\Release\VoxelOpenGl.dll

SET PATH=OpenGl\Release;%QTDIR%\bin;%QTDIR%;%PATH%

SET CSF_TEX_FONT=2
SET MMGT_REENTRANT=1


rem Define path to project file
set PRJFILE=%~dp0VoxelDemo.sln

rem Launch Visual Studio - either professional (devenv) or Express, as available
if exist "%DevEnvDir%\devenv.exe" (
    start devenv.exe "%PRJFILE%" /useenv
) else if exist "%DevEnvDir%\VCExpress.exe"  (
    start VCExpress.exe "%PRJFILE%" /useenv
) else (
    echo Error: Could not find MS Visual Studio ^(%VCVER%^)
    echo Check relevant environment variable ^(e.g. VS80COMNTOOLS for vc8^)
)

