@echo off

rem Auxiliary script for semi-automated building of OCCT using cmake.
rem cmake_custom.bat should be configured with VS version and path to 3rd-parties.
rem OCCT3RDPARTY must be specified as mandatory dependency.

setlocal

set "SrcRoot=%~dp0..\.."

set VS=14
set VSDATA=2015
set VSPLATFORM=Win64
set "BUILD_DIR=build-vs%VS%-%VSPLATFORM%"
set "INSTALL_DIR=%SrcRoot%\install"

set BUILD_ADDITIONAL_TOOLKITS=
set BUILD_DOC_Overview=OFF
set BUILD_Inspector=OFF
set BUILD_LIBRARY_TYPE=Shared
set BUILD_PATCH=
set BUILD_RELEASE_DISABLE_EXCEPTIONS=ON
set BUILD_WITH_DEBUG=OFF
set BUILD_ENABLE_FPE_SIGNAL_HANDLER=ON
set BUILD_USE_PCH=OFF

set BUILD_MODULE_ApplicationFramework=ON
set BUILD_MODULE_DataExchange=ON
set BUILD_MODULE_Draw=ON
set BUILD_MODULE_ModelingAlgorithms=ON
set BUILD_MODULE_ModelingData=ON
set BUILD_MODULE_Visualization=ON

set USE_D3D=OFF
set USE_FFMPEG=OFF
set USE_FREEIMAGE=OFF
set USE_GLES2=OFF
set USE_RAPIDJSON=OFF
set USE_TBB=OFF
set USE_VTK=OFF

if exist "%~dp0cmake_custom.bat" call "%~dp0cmake_custom.bat"

if not "%VSPLATFORM%"=="" set "arch_compile=Visual Studio %VS% %VSDATA% %VSPLATFORM%"
if     "%VSPLATFORM%"=="" set "arch_compile=Visual Studio %VS% %VSDATA%"

set "INSTALL_DIR=%INSTALL_DIR:\=/%"
set "OCCT3RDPARTY=%OCCT3RDPARTY:\=/%"

set "BUILD_DIR=%SrcRoot%\%BUILD_DIR%"
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
pushd "%BUILD_DIR%"
 
cmake -G "%arch_compile%" ^
  -D 3RDPARTY_DIR:STRING="%OCCT3RDPARTY%" ^
  -D BUILD_ADDITIONAL_TOOLKITS:STRING="%BUILD_ADDITIONAL_TOOLKITS%" ^
  -D BUILD_DOC_Overview:BOOL=%BUILD_DOC_Overview% ^
  -D BUILD_Inspector:BOOL=%BUILD_Inspector% ^
  -D BUILD_LIBRARY_TYPE:STRING=%BUILD_LIBRARY_TYPE% ^
  -D BUILD_MODULE_ApplicationFramework:BOOL=%BUILD_MODULE_ApplicationFramework% ^
  -D BUILD_MODULE_DataExchange:BOOL=%BUILD_MODULE_DataExchange% ^
  -D BUILD_MODULE_Draw:BOOL=%BUILD_MODULE_Draw% ^
  -D BUILD_MODULE_FoundationClasses:BOOL=ON ^
  -D BUILD_MODULE_ModelingAlgorithms:BOOL=%BUILD_MODULE_ModelingAlgorithms% ^
  -D BUILD_MODULE_ModelingData:BOOL=%BUILD_MODULE_ModelingData% ^
  -D BUILD_MODULE_Visualization:BOOL=%BUILD_MODULE_Visualization% ^
  -D BUILD_PATCH:PATH="%BUILD_PATCH%" ^
  -D BUILD_RELEASE_DISABLE_EXCEPTIONS:BOOL=%BUILD_RELEASE_DISABLE_EXCEPTIONS% ^
  -D BUILD_WITH_DEBUG:BOOL=%BUILD_WITH_DEBUG% ^
  -D BUILD_ENABLE_FPE_SIGNAL_HANDLER:BOOL=%BUILD_ENABLE_FPE_SIGNAL_HANDLER% ^
  -D BUILD_USE_PCH:BOOL=%BUILD_USE_PCH% ^
  -D INSTALL_DIR:PATH="%INSTALL_DIR%" ^
  -D USE_D3D:BOOL=%USE_D3D% ^
  -D USE_FFMPEG:BOOL=%USE_FFMPEG% ^
  -D USE_FREEIMAGE:BOOL=%USE_FREEIMAGE% ^
  -D USE_GLES2:BOOL=%USE_GLES2% ^
  -D USE_RAPIDJSON:BOOL=%USE_RAPIDJSON% ^
  -D USE_TBB:BOOL=%USE_TBB% ^
  -D USE_VTK:BOOL=%USE_VTK% ^
  "%SrcRoot%"

popd
endlocal
