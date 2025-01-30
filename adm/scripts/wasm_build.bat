@echo OFF

rem Auxiliary script for semi-automated building of OCCT for WASM platform.
rem wasm_custom.bat should be configured with paths to CMake, 3rd-parties and Emscripten SDK.
rem FreeType should be specified as mandatory dependency.

set "aCasSrc=%~dp0..\.."
set "aBuildRoot=%aCasSrc%\work"

set aNbJobs=%NUMBER_OF_PROCESSORS%

rem Paths to 3rd-party tools and libraries
set "aCmakeBin="
set "aFreeType="
set "aRapidJson="
set "aDraco="
set "aTcl="

rem Build stages to perform
set "toCMake=1"
set "toClean=0"
set "toMake=1"
set "toInstall=1"
set "toPack=0"
set "toDebug=0"
set "toBuildSample=0"
set "sourceMapBase="

rem OCCT Modules to build
set "BUILD_ModelingData=ON"
set "BUILD_ModelingAlgorithms=ON"
set "BUILD_Visualization=ON"
set "BUILD_ApplicationFramework=ON"
set "BUILD_DataExchange=ON"
set "BUILD_DETools=OFF"
set "BUILD_Draw=OFF"

rem Optional 3rd-party libraries to enable
set "USE_FREETYPE=ON"
set "USE_RAPIDJSON=OFF"
set "USE_DRACO=OFF"
set "USE_PTHREADS=OFF"

rem Archive tool
set "THE_7Z_PARAMS=-t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on"
set "THE_7Z_PATH=%ProgramW6432%\7-Zip\7z.exe"

rem Configuration file
if exist "%~dp0wasm_custom.bat" call "%~dp0wasm_custom.bat"

call "%EMSDK_ROOT%\emsdk_env.bat"
set "aToolchain=%EMSDK%/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
if not ["%aCmakeBin%"] == [""] ( set "PATH=%aCmakeBin%;%PATH%" )

set "anOcctVerSuffix="
set "anOcctVersion=0.0.0"
set "aGitBranch="
call "%~dp0build_common.bat"
set "aGitBranch="
for /f %%i in ('git symbolic-ref --short HEAD') do ( set "aGitBranch=%%i" )

set "aBuildType=Release"
set "aBuildTypePrefix="
set "anExtraCxxFlags="
if /I ["%USE_PTHREADS%"] == ["ON"] (
  set "anExtraCxxFlags=-pthread"
  set "aBuildTypePrefix=%aBuildTypePrefix%-pthread"
)
if ["%toDebug%"] == ["1"] (
  set "aBuildType=Debug"
  set "aBuildTypePrefix=%aBuildTypePrefix%-debug"
)

call :cmakeGenerate
if errorlevel 1 (
  if not ["%1"] == ["-nopause"] (
    pause
  )
  exit /B 1
  goto :eof
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
set "anArchName=occt-%anOcctVersion%%anOcctVerSuffix%%aRevision%-wasm32%aBuildTypePrefix%"
set "aTarget=%aBuildRoot%\%anArchName%"
if ["%toPack%"] == ["1"] (
  echo Creating archive %anArchName%.7z
  rmdir /S /Q "%aTarget%"
  if not exist "%aTarget%" ( mkdir "%aTarget%" )
  if exist "%aBuildRoot%/%anArchName%.7z" del "%aBuildRoot%/%anArchName%.7z"
  xcopy /S /Y "%aDestDir%\*" "%aTarget%\"

  if ["%toBuildSample%"] == ["1"] (
    if not exist "%aTarget%\sample" ( mkdir "%aTarget%\sample" )
    xcopy /S /Y "%aDestDirSmpl%\*" "%aTarget%\sample\"
  )

  "%THE_7Z_PATH%" a -r %THE_7Z_PARAMS% "%aBuildRoot%/%anArchName%.7z" "%aTarget%"
)
if not ["%1"] == ["-nopause"] (
  pause
)

goto :eof

:cmakeGenerate
set "aPlatformAndCompiler=wasm32%aBuildTypePrefix%"
set "aWorkDir=%aBuildRoot%\occt-%aPlatformAndCompiler%-make"
set "aDestDir=%aBuildRoot%\occt-%aPlatformAndCompiler%"
set "aLogFile=%aBuildRoot%\occt-%aPlatformAndCompiler%-build.log"
if ["%toCMake%"] == ["1"] (
  if ["%toClean%"] == ["1"] (
    rmdir /S /Q %aWorkDir%"
    rmdir /S /Q %aDestDir%"
  )
)
if not exist "%aWorkDir%" ( mkdir "%aWorkDir%" )
if     exist "%aLogFile%" ( del   "%aLogFile%" )

set "aSrcRootSmpl=%aCasSrc%\samples\webgl"
set "aWorkDirSmpl=%aBuildRoot%\sample-%aPlatformAndCompiler%-make"
set "aDestDirSmpl=%aBuildRoot%\sample-%aPlatformAndCompiler%"
set "aLogFileSmpl=%aBuildRoot%\sample-%aPlatformAndCompiler%-build.log"
if ["%toBuildSample%"] == ["1"] (
  if ["%toCMake%"] == ["1"] (
    if ["%toClean%"] == ["1"] (
      rmdir /S /Q %aWorkDirSmpl%"
      rmdir /S /Q %aDestDirSmpl%"
    )
  )
  if not exist "%aWorkDirSmpl%" ( mkdir "%aWorkDirSmpl%" )
  if     exist "%aLogFileSmpl%" ( del   "%aLogFileSmpl%" )
)

rem include some information about OCCT into archive
echo ^<pre^>> "%aWorkDir%\VERSION.html"
git status >> "%aWorkDir%\VERSION.html"
git log -n 100 >> "%aWorkDir%\VERSION.html"
echo ^</pre^>>> "%aWorkDir%\VERSION.html"

echo Start building OCCT for %aPlatformAndCompiler%
echo Start building OCCT for %aPlatformAndCompiler%>> %aLogFile%

echo --->> %aLogFile%
call emcc --version >> %aLogFile%
echo --->> %aLogFile%

pushd "%aWorkDir%"

set "aTimeZERO=%TIME%"
if ["%toCMake%"] == ["1"] (
  echo Configuring OCCT for WASM...
  cmake -G "MinGW Makefiles" ^
 -D CMAKE_TOOLCHAIN_FILE:FILEPATH="%aToolchain%" ^
 -D CMAKE_BUILD_TYPE:STRING="%aBuildType%" ^
 -D BUILD_LIBRARY_TYPE:STRING="Static" ^
 -D CMAKE_CXX_FLAGS="%anExtraCxxFlags%" ^
 -D INSTALL_DIR:PATH="%aDestDir%" ^
 -D INSTALL_DIR_INCLUDE:STRING="inc" ^
 -D INSTALL_DIR_RESOURCE:STRING="src" ^
 -D BUILD_MODULE_FoundationClasses:BOOL="ON" ^
 -D BUILD_MODULE_ModelingData:BOOL="%BUILD_ModelingData%" ^
 -D BUILD_MODULE_ModelingAlgorithms:BOOL="%BUILD_ModelingAlgorithms%" ^
 -D BUILD_MODULE_Visualization:BOOL="%BUILD_Visualization%" ^
 -D BUILD_MODULE_ApplicationFramework:BOOL="%BUILD_ApplicationFramework%" ^
 -D BUILD_MODULE_DataExchange:BOOL="%BUILD_DataExchange%" ^
 -D BUILD_MODULE_DETools:BOOL="%BUILD_DETools%" ^
 -D BUILD_MODULE_Draw:BOOL="%BUILD_Draw%" ^
 -D BUILD_DOC_Overview:BOOL="OFF" ^
 -D USE_FREETYPE:BOOL="%USE_FREETYPE%" ^
 -D 3RDPARTY_FREETYPE_DIR:PATH="%aFreeType%" ^
 -D 3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2:FILEPATH="%aFreeType%/include" ^
 -D 3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build:FILEPATH="%aFreeType%/include" ^
 -D USE_RAPIDJSON:BOOL="%USE_RAPIDJSON%" ^
 -D 3RDPARTY_RAPIDJSON_DIR:PATH="%aRapidJson%" ^
 -D 3RDPARTY_RAPIDJSON_INCLUDE_DIR:PATH="%aRapidJson%/include" ^
 -D USE_DRACO:BOOL="%USE_DRACO%" ^
 -D 3RDPARTY_DRACO_DIR:PATH="%aDraco%" ^
 -D 3RDPARTY_DRACO_INCLUDE_DIR:FILEPATH="%aDraco%/include" ^
 -D 3RDPARTY_DRACO_LIBRARY_DIR:PATH="%aDraco%/lib" ^
 -D 3RDPARTY_DRACO_LIBRARY:FILEPATH="%aDraco%/lib/libdraco.a" ^
 -D USE_TK:BOOL="OFF" ^
 -D 3RDPARTY_TCL_DIR:PATH="%aTcl%" ^
 -D 3RDPARTY_TCL_INCLUDE_DIR:PATH="%aTcl%/include" ^
 -D 3RDPARTY_TCL_LIBRARY_DIR:PATH="%aTcl%/lib" ^
 -D 3RDPARTY_TCL_LIBRARY:FILEPATH="%aTcl%/lib/libtcl.a" ^
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
if ["%toBuildSample%"] == ["0"] (
  goto :eof
)

pushd "%aWorkDirSmpl%"

if ["%toCMake%"] == ["1"] (
  cmake -G "MinGW Makefiles" ^
 -D CMAKE_TOOLCHAIN_FILE:FILEPATH="%aToolchain%" ^
 -D CMAKE_BUILD_TYPE:STRING="%aBuildType%" ^
 -D CMAKE_CXX_FLAGS="%anExtraCxxFlags%" ^
 -D CMAKE_INSTALL_PREFIX:PATH="%aDestDirSmpl%" ^
 -D SOURCE_MAP_BASE:STRING="%sourceMapBase%" ^
 -D OpenCASCADE_DIR:PATH="%aDestDir%/lib/cmake/opencascade" ^
 -D freetype_DIR:PATH="%aFreeType%/lib/cmake/freetype" ^
 "%aSrcRootSmpl%"

  if errorlevel 1 (
    popd
    pause
    exit /B
    goto :eof
  )
)
if ["%toClean%"] == ["1"] (
  mingw32-make clean
)

if ["%toMake%"] == ["1"] (
  echo Building...
  mingw32-make -j %aNbJobs% 2>> "%aLogFileSmpl%"
  if errorlevel 1 (
    type "%aLogFileSmpl%"
    popd
    pause
    exit /B
    goto :eof
  )
  type "%aLogFileSmpl%"
)

if ["%toInstall%"] == ["1"] (
  mingw32-make install 2>> "%aLogFileSmpl%"
  if errorlevel 1 (
    type "%aLogFileSmpl%"
    popd
    pause
    exit /B
    goto :eof
  )
)
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
