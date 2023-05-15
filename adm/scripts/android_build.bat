@echo OFF

rem Auxiliary script for semi-automated building of OCCT for Android platform.
rem android_custom.bat should be configured with paths
rem to CMake, 3rd-parties, Android NDK and MinGW make tool.

set "aCasSrc=%~dp0..\.."
set "aBuildRoot=%aCasSrc%\work"

set aNbJobs=%NUMBER_OF_PROCESSORS%

rem Paths to 3rd-party tools and libraries
set "anNdkPath="
set "aFreeType="
set "aRapidJson="
set "aDraco="

rem Build stages to perform
set "toCMake=1"
set "toClean=0"
set "toMake=1"
set "toInstall=1"
set "toPack=1"
set "isStatic=0"

rem Minimal Android platform and CPU architectures
set "anNdkApiLevel=21"
set "anNdkAbiList=armeabi-v7a x86 arm64-v8a x86_64"

rem OCCT Modules to build
set "BUILD_ModelingData=ON"
set "BUILD_ModelingAlgorithms=ON"
set "BUILD_Visualization=ON"
set "BUILD_ApplicationFramework=ON"
set "BUILD_DataExchange=ON"
set "BUILD_DETools=OFF"

rem Optional 3rd-party libraries to enable
set "USE_FREETYPE=ON"
set "USE_RAPIDJSON=OFF"
set "USE_DRACO=OFF"

rem Archive tool
set "THE_7Z_PARAMS=-t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on"
set "THE_7Z_PATH=%ProgramW6432%\7-Zip\7z.exe"

rem Configuration file
if exist "%~dp0android_custom.bat" call "%~dp0android_custom.bat"

set "aCompiler=gcc"
set "aCppLib=gnustl_shared"
if not exist "%anNdkPath%/sources/cxx-stl/gnu-libstdc++" (
  if exist "%anNdkPath%/sources/cxx-stl/llvm-libc++" (
    set "aCompiler=clang"
    set "aCppLib=c++_shared"
  )
)
set "aLibType=Shared"
if ["%isStatic%"] == ["1"] set "aLibType=Static"
set "aDestDir=%aBuildRoot%\android-%aCompiler%"
if ["%toCMake%"] == ["1"] (
  if ["%toClean%"] == ["1"] (
    rmdir /S /Q %aDestDir%"
  )
)

set "anOcctVerSuffix="
set "anOcctVersion=0.0.0"
set "aGitBranch="
for /f tokens^=2^ delims^=^" %%i in ('findstr /b /c:"#define OCC_VERSION_DEVELOPMENT" "%aCasSrc%\src\Standard\Standard_Version.hxx"') do ( set "anOcctVerSuffix=%%i" )
for /f tokens^=2^ delims^=^" %%i in ('findstr /b /c:"#define OCC_VERSION_COMPLETE" "%aCasSrc%\src\Standard\Standard_Version.hxx"') do ( set "anOcctVersion=%%i" )
for /f %%i in ('git symbolic-ref --short HEAD') do ( set "aGitBranch=%%i" )

for %%s in (%anNdkAbiList%) do (
  call :cmakeGenerate "%anNdkApiLevel%" "%%s"
  if errorlevel 1 (
    if not ["%1"] == ["-nopause"] (
      pause
    )
    exit /B 1
    goto :eof
  )
)

for /F "skip=1 delims=" %%F in ('
  wmic PATH Win32_LocalTime GET Day^,Month^,Year /FORMAT:TABLE
') do (
  for /F "tokens=1-3" %%L in ("%%F") do (
    set DAY00=0%%L
    set MONTH00=0%%M
    set YEAR=%%N
  )
)
set DAY00=%DAY00:~-2%
set MONTH00=%MONTH00:~-2%
set "aRevision=-%YEAR%-%MONTH00%-%DAY00%"
rem set "aRevision=-%aGitBranch%"
set "anArchName=occt-%anOcctVersion%%anOcctVerSuffix%%aRevision%-android"
set "aTarget=%aBuildRoot%\%anArchName%"
if ["%toPack%"] == ["1"] (
  echo Creating archive %anArchName%.7z
  rmdir /S /Q "%aTarget%"
  if not exist "%aTarget%" ( mkdir "%aTarget%" )
  if exist "%aBuildRoot%/%anArchName%.7z" del "%aBuildRoot%/%anArchName%.7z"
  xcopy /S /Y "%aDestDir%\*" "%aTarget%\"

  "%THE_7Z_PATH%" a -r %THE_7Z_PARAMS% "%aBuildRoot%/%anArchName%.7z" "%aTarget%"
)
if not ["%1"] == ["-nopause"] (
  pause
)

goto :eof

:cmakeGenerate
set "anApi=%~1"
set "anAbi=%~2"
if ["%anApi%"] == [""] (
  set "anApi=21"
  if ["%anAbi%"] == ["armeabi-v7a"] ( set "anApi=16" )
  if ["%anAbi%"] == ["x86"]         ( set "anApi=16" )
)
set "aPlatformAndCompiler=android-%anAbi%-%aCompiler%"
set "aWorkDir=%aBuildRoot%\%aPlatformAndCompiler%-make"
set "aLogFile=%aBuildRoot%\build-%aPlatformAndCompiler%.log"
if ["%toCMake%"] == ["1"] (
  if ["%toClean%"] == ["1"] (
    rmdir /S /Q %aWorkDir%"
  )
)
if not exist "%aWorkDir%" ( mkdir "%aWorkDir%" )
if     exist "%aLogFile%" ( del   "%aLogFile%" )

rem include some information about OCCT into archive
echo ^<pre^>> "%aWorkDir%\VERSION.html"
git status >> "%aWorkDir%\VERSION.html"
git log -n 100 >> "%aWorkDir%\VERSION.html"
echo ^</pre^>>> "%aWorkDir%\VERSION.html"

echo Start building OCCT for %aPlatformAndCompiler%
echo Start building OCCT for %aPlatformAndCompiler%, API level %anApi%>> %aLogFile%

pushd "%aWorkDir%"

set "aFreeTypeLibName=libfreetype.so"
if exist "%aFreeType%/libs/%anAbi%/libfreetype.a" ( set "aFreeTypeLibName=libfreetype.a" )

set "aTimeZERO=%TIME%"
if ["%toCMake%"] == ["1"] (
  echo Configuring OCCT for Android %anAbi%, API level %anApi%...
  cmake -G "MinGW Makefiles" ^
 -D CMAKE_SYSTEM_NAME:STRING="Android" ^
 -D CMAKE_ANDROID_NDK="%anNdkPath%" ^
 -D CMAKE_BUILD_TYPE:STRING="Release" ^
 -D CMAKE_ANDROID_ARCH_ABI:STRING="%anAbi%" ^
 -D CMAKE_SYSTEM_VERSION:STRING="%anApi%" ^
 -D CMAKE_ANDROID_STL_TYPE="%aCppLib%" ^
 -D BUILD_LIBRARY_TYPE:STRING="%aLibType%" ^
 -D INSTALL_DIR:PATH="%aDestDir%" ^
 -D INSTALL_DIR_INCLUDE:STRING="inc" ^
 -D INSTALL_DIR_LIB:STRING="libs/%anAbi%" ^
 -D INSTALL_DIR_CMAKE:STRING="libs/%anAbi%/cmake/opencascade" ^
 -D INSTALL_DIR_RESOURCE:STRING="src" ^
 -D BUILD_MODULE_FoundationClasses:BOOL="ON" ^
 -D BUILD_MODULE_ModelingData:BOOL="%BUILD_ModelingData%" ^
 -D BUILD_MODULE_ModelingAlgorithms:BOOL="%BUILD_ModelingAlgorithms%" ^
 -D BUILD_MODULE_Visualization:BOOL="%BUILD_Visualization%" ^
 -D BUILD_MODULE_ApplicationFramework:BOOL="%BUILD_ApplicationFramework%" ^
 -D BUILD_MODULE_DataExchange:BOOL="%BUILD_DataExchange%" ^
 -D BUILD_MODULE_DETools:BOOL="OFF" ^
 -D BUILD_MODULE_Draw:BOOL="OFF" ^
 -D BUILD_DOC_Overview:BOOL="OFF" ^
 -D USE_FREETYPE:BOOL="%USE_FREETYPE%" ^
 -D 3RDPARTY_FREETYPE_DIR:PATH="%aFreeType%" ^
 -D 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2:FILEPATH="%aFreeType%/include" ^
 -D 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build:FILEPATH="%aFreeType%/include" ^
 -D 3RDPARTY_FREETYPE_LIBRARY_DIR:PATH="%aFreeType%/libs/%anAbi%" ^
 -D 3RDPARTY_FREETYPE_LIBRARY:FILEPATH="%aFreeType%/libs/%anAbi%/%aFreeTypeLibName%" ^
 -D USE_RAPIDJSON:BOOL="%USE_RAPIDJSON%" ^
 -D 3RDPARTY_RAPIDJSON_DIR:PATH="%aRapidJson%" ^
 -D 3RDPARTY_RAPIDJSON_INCLUDE_DIR:PATH="%aRapidJson%/include" ^
 -D USE_DRACO:BOOL="%USE_DRACO%" ^
 -D 3RDPARTY_DRACO_DIR:PATH="%aDraco%" ^
 -D 3RDPARTY_DRACO_INCLUDE_DIR:FILEPATH="%aDraco%/include" ^
 -D 3RDPARTY_DRACO_LIBRARY_DIR_draco:PATH="%aDraco%/libs/%anAbi%" ^
 "%aCasSrc%"

  if errorlevel 1 (
    popd
    exit /B 1
    goto :eof
  )
)
set "aTimeGEN=%TIME%"
call :computeDuration %aTimeZERO% %aTimeGEN%
if ["%toCMake%"] == ["1"] (
  echo Generation time: %DURATION%
  echo Generation time: %DURATION%>> "%aLogFile%"
)

if ["%toClean%"] == ["1"] (
  mingw32-make clean
)

if ["%toMake%"] == ["1"] (
  echo Building...
  mingw32-make -j %aNbJobs% 2>> "%aLogFile%"
  if errorlevel 1 (
    type "%aLogFile%"
    popd
    exit /B 1
    goto :eof
  )
  type "%aLogFile%"
)
set "aTimeBUILD=%TIME%"
call :computeDuration %aTimeGEN% %aTimeBUILD%
if ["%toMake%"] == ["1"] (
  echo Building time: %DURATION%
  echo Building time: %DURATION%>> "%aLogFile%"
)
call :computeDuration %aTimeZERO% %aTimeBUILD%
if ["%toMake%"] == ["1"] (
  echo Total building time: %DURATION%
  echo Total building time: %DURATION%>> "%aLogFile%"
)

if ["%toInstall%"] == ["1"] (
  echo Installing into %aDestDir%...
  mingw32-make install 2>> "%aLogFile%"
  copy /Y "%aWorkDir%\VERSION.html" "%aDestDir%\VERSION.html"
)
set "aTimeINSTALL=%TIME%"
call :computeDuration "%aTimeBUILD%" "%aTimeINSTALL%"
if ["%toInstall%"] == ["1"] (
  echo Install time: %DURATION%
  echo Install time: %DURATION%>> "%aLogFile%"
)

call :computeDuration "%aTimeZERO%" "%aTimeINSTALL%"
echo Total time: %DURATION%
echo Total time: %DURATION%>> "%aLogFile%"

popd
goto :eof

:computeDuration
set "aTimeFrom=%~1"
set "aTimeEnd=%~2"
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
