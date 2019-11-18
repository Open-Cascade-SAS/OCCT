@echo OFF

rem Auxiliary script for semi-automated building of OCCT for WASM platform.
rem wasm_custom.bat should be configured with paths to CMake, 3rd-parties and Emscripten SDK.
rem FreeType should be specified as mandatory dependency.

set "aSrcRoot=%~dp0..\.."
set "aBuildRoot=work"

set aNbJobs=%NUMBER_OF_PROCESSORS%

set "toCMake=1"
set "toClean=0"
set "toMake=1"
set "toInstall=1"

set "BUILD_ModelingData=ON"
set "BUILD_ModelingAlgorithms=ON"
set "BUILD_Visualization=ON"
set "BUILD_ApplicationFramework=ON"
set "BUILD_DataExchange=ON"

rem Configuration file
if exist "%~dp0wasm_custom.bat" call "%~dp0wasm_custom.bat"

call "%EMSDK_ROOT%\emsdk_env.bat"
set "aToolchain=%EMSDK%/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"

set "anOcctVerSuffix="
set "anOcctVersion=0.0.0"
set "aGitBranch="
for /f tokens^=2^ delims^=^" %%i in ('findstr /b /c:"#define OCC_VERSION_DEVELOPMENT" "%aSrcRoot%\src\Standard\Standard_Version.hxx"') do ( set "anOcctVerSuffix=%%i" )
for /f tokens^=2^ delims^=^" %%i in ('findstr /b /c:"#define OCC_VERSION_COMPLETE" "%aSrcRoot%\src\Standard\Standard_Version.hxx"') do ( set "anOcctVersion=%%i" )
for /f %%i in ('git symbolic-ref --short HEAD') do ( set "aGitBranch=%%i" )

call :cmakeGenerate
if not ["%1"] == ["-nopause"] (
  pause
)

goto :eof

:cmakeGenerate
set "aPlatformAndCompiler=wasm"
set "aWorkDir=%aSrcRoot%\%aBuildRoot%\%aPlatformAndCompiler%-make"
set "aDestDir=%aSrcRoot%\%aBuildRoot%\%aPlatformAndCompiler%"
set "aLogFile=%aSrcRoot%\%aBuildRoot%\build-%aPlatformAndCompiler%.log"
if not exist "%aWorkDir%" ( mkdir "%aWorkDir%" )
if     exist "%aLogFile%" ( del   "%aLogFile%" )

rem include some information about OCCT into archive
echo ^<pre^>> "%aWorkDir%\VERSION.html"
git status >> "%aWorkDir%\VERSION.html"
git log -n 100 >> "%aWorkDir%\VERSION.html"
echo ^</pre^>>> "%aWorkDir%\VERSION.html"

echo Start building OCCT for %aPlatformAndCompiler%
echo Start building OCCT for %aPlatformAndCompiler%>> %aLogFile%

pushd "%aWorkDir%"

set aTimeZERO=%TIME%
if ["%toCMake%"] == ["1"] (
  echo "Configuring OCCT for WASM..."
  cmake -G "MinGW Makefiles" ^
 -D CMAKE_TOOLCHAIN_FILE:FILEPATH="%aToolchain%" ^
 -D CMAKE_BUILD_TYPE:STRING="Release" ^
 -D BUILD_LIBRARY_TYPE:STRING="Static" ^
 -D INSTALL_DIR:PATH="%aDestDir%" ^
 -D INSTALL_DIR_INCLUDE:STRING="inc" ^
 -D INSTALL_DIR_RESOURCE:STRING="src" ^
 -D 3RDPARTY_FREETYPE_DIR:PATH="%aFreeType%" ^
 -D 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2:FILEPATH="%aFreeType%/include" ^
 -D 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build:FILEPATH="%aFreeType%/include" ^
 -D BUILD_MODULE_FoundationClasses:BOOL="ON" ^
 -D BUILD_MODULE_ModelingData:BOOL="%BUILD_ModelingData%" ^
 -D BUILD_MODULE_ModelingAlgorithms:BOOL="%BUILD_ModelingAlgorithms%" ^
 -D BUILD_MODULE_Visualization:BOOL="%BUILD_Visualization%" ^
 -D BUILD_MODULE_ApplicationFramework:BOOL="%BUILD_ApplicationFramework%" ^
 -D BUILD_MODULE_DataExchange:BOOL="%BUILD_DataExchange%" ^
 -D BUILD_MODULE_Draw:BOOL="OFF" ^
 -D BUILD_DOC_Overview:BOOL="OFF" ^
 "%aSrcRoot%"

  if errorlevel 1 (
    popd
    exit /B 1
    goto :eof
  )
)
set aTimeGEN=%TIME%
call :computeDuration %aTimeZERO% %aTimeGEN%
if ["%toCMake%"] == ["1"] (
  echo Generation time: %DURATION%
  echo Generation time: %DURATION%>> "%aLogFile%"
)

if "%toClean%"=="1" (
  mingw32-make clean
)

if "%toMake%"=="1" (
  echo Building...
  mingw32-make -j %aNbJobs% 2>> "%aLogFile%"
  if errorlevel 1 (
    popd
    exit /B 1
    goto :eof
  )
  type "%aLogFile%"
)
set aTimeBUILD=%TIME%
call :computeDuration %aTimeGEN% %aTimeBUILD%
if "%toMake%"=="1" (
  echo Building time: %DURATION%
  echo Building time: %DURATION%>> "%aLogFile%"
)
call :computeDuration %aTimeZERO% %aTimeBUILD%
if "%toMake%"=="1" (
  echo Total building time: %DURATION%
  echo Total building time: %DURATION%>> "%aLogFile%"
)

if "%toInstall%"=="1" (
  echo Installing into %aDestDir%...
  mingw32-make install 2>> "%aLogFile%"
  copy /Y "%aWorkDir%\VERSION.html" "%aDestDir%\VERSION.html"
)
set aTimeINSTALL=%TIME%
call :computeDuration %aTimeBUILD% %aTimeINSTALL%
if "%toInstall%"=="1" (
  echo Install time: %DURATION%
  echo Install time: %DURATION%>> "%aLogFile%"
)

call :computeDuration %aTimeZERO% %aTimeINSTALL%
echo Total time: %DURATION%
echo Total time: %DURATION%>> "%aLogFile%"

popd
goto :eof

:computeDuration
set aTimeFrom=%1
set aTimeEnd=%2
rem handle time before 10AM (win10 - remove empty space at the beginning)
if "%aTimeFrom:~0,1%"==" " set "aTimeFrom=%aTimeFrom:~1%"
if  "%aTimeEnd:~0,1%"==" " set  "aTimeEnd=%aTimeEnd:~1%"
rem handle time before 10AM (win7 - add 0 at the beginning)
if "%aTimeFrom:~1,1%"==":" set "aTimeFrom=0%aTimeFrom%"
if  "%aTimeEnd:~1,1%"==":" set  "aTimeEnd=0%aTimeEnd%"
rem convert hours:minutes:seconds:ms into duration
set /A aTimeFrom=(1%aTimeFrom:~0,2%-100)*360000 + (1%aTimeFrom:~3,2%-100)*6000 + (1%aTimeFrom:~6,2%-100)*100 + (1%aTimeFrom:~9,2%-100)
set /A  aTimeEnd= (1%aTimeEnd:~0,2%-100)*360000 +  (1%aTimeEnd:~3,2%-100)*6000 +  (1%aTimeEnd:~6,2%-100)*100 +  (1%aTimeEnd:~9,2%-100)
set /A aDurTotalSec=%aTimeEnd%-%aTimeFrom%
if %aTimeEnd% LSS %aTimeFrom% set set /A aDurTotalSec=%aTimeFrom%-%aTimeEnd%
set /A aDurHH=%aDurTotalSec% / 360000
set /A aDurMM=(%aDurTotalSec% - %aDurHH%*360000) / 6000
set /A aDurSS=(%aDurTotalSec% - %aDurHH%*360000 - %aDurMM%*6000) / 100
if %aDurHH% LSS 10 set aDurHH=0%aDurHH%
if %aDurMM% LSS 10 set aDurMM=0%aDurMM%
if %aDurSS% LSS 10 set aDurSS=0%aDurSS%

set "DURATION=%aDurHH%:%aDurMM%:%aDurSS%"
goto :eof
