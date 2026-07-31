@echo OFF

rem Auxiliary script for semi-automated building of WebGL sample.
rem wasm_custom.bat should be configured with paths to CMake, 3rd-parties and Emscripten SDK.
rem FreeType should be specified as mandatory dependency.

set "aCasSrc=%~dp0..\.."
set "aBuildRoot=%aCasSrc%\work"

set aNbJobs=%NUMBER_OF_PROCESSORS%

rem Paths to 3rd-party tools and libraries
set "aCmakeBin="
set "aFreeType="

rem Build stages to perform
set "toCMake=1"
set "toClean=0"
set "toMake=1"
set "toInstall=1"
set "toDebug=0"
set "sourceMapBase="

set "USE_PTHREADS=OFF"

rem Configuration file
if exist "%~dp0wasm_custom.bat" call "%~dp0wasm_custom.bat"

call "%EMSDK_ROOT%\emsdk_env.bat"
set "aToolchain=%EMSDK%/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
if not ["%aCmakeBin%"] == [""] ( set "PATH=%aCmakeBin%;%PATH%" )

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
if not ["%1"] == ["-nopause"] (
  pause
)

goto :eof

:cmakeGenerate
set "aPlatformAndCompiler=wasm32%aBuildTypePrefix%"
set "aDestDirOcct=%aBuildRoot%\occt-%aPlatformAndCompiler%"
set "aSrcRootSmpl=%aCasSrc%\samples\webgl"
set "aWorkDirSmpl=%aBuildRoot%\sample-%aPlatformAndCompiler%-make"
set "aDestDirSmpl=%aBuildRoot%\sample-%aPlatformAndCompiler%"
set "aLogFileSmpl=%aBuildRoot%\sample-%aPlatformAndCompiler%-build.log"
if ["%toCMake%"] == ["1"] (
  if ["%toClean%"] == ["1"] (
    rmdir /S /Q %aWorkDirSmpl%"
    rmdir /S /Q %aDestDirSmpl%"
  )
)
if not exist "%aWorkDirSmpl%" ( mkdir "%aWorkDirSmpl%" )
if     exist "%aLogFileSmpl%" ( del   "%aLogFileSmpl%" )

pushd "%aWorkDirSmpl%"

if ["%toCMake%"] == ["1"] (
  cmake -G "MinGW Makefiles" ^
 -D CMAKE_TOOLCHAIN_FILE:FILEPATH="%aToolchain%" ^
 -D CMAKE_BUILD_TYPE:STRING="%aBuildType%" ^
 -D CMAKE_CXX_FLAGS="%anExtraCxxFlags%" ^
 -D CMAKE_INSTALL_PREFIX:PATH="%aDestDirSmpl%" ^
 -D SOURCE_MAP_BASE:STRING="%sourceMapBase%" ^
 -D OpenCASCADE_DIR:PATH="%aDestDirOcct%/lib/cmake/opencascade" ^
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
