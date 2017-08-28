@echo off

rem Use:
rem - first argument specifies version of Visual Studio (vc8, vc9, or vc10),
rem - second argument specifies architecture (win32 or win64),
rem - third argument specifies build mode (Debug or Release)
rem Default options are:
rem   vc8 win32 Release

set "SCRIPTROOT=%~dp0"
set "SCRIPTROOT=%SCRIPTROOT:~0,-1%"

rem ----- Reset values to defaults -----
set "CASDEB="
set "VCVER=vc10"
set "ARCH=64"
set "VCVARS="
set "HAVE_TBB=false"
set "HAVE_OPENCL=false"
set "HAVE_FREEIMAGE=false"
set "HAVE_FFMPEG=false"
set "HAVE_GL2PS=false"
set "HAVE_VTK=false"
set "HAVE_GLES2=false"
set "HAVE_D3D=false"
set "HAVE_ZLIB=false"
set "HAVE_LIBLZMA=false"
set "CSF_OPT_INC="
set "CSF_OPT_LIB32="
set "CSF_OPT_LIB64="
set "CSF_OPT_BIN32="
set "CSF_OPT_BIN64="

if not ["%CASROOT%"] == [""] if exist "%SCRIPTROOT%\%CASROOT%" set "CASROOT=%SCRIPTROOT%\%CASROOT%"
if     ["%CASROOT%"] == [""] set "CASROOT=%SCRIPTROOT%"

rem ----- Load local settings -----
if exist "%CASROOT%\custom.bat" (
  call "%CASROOT%\custom.bat" %1 %2 %3 %4 %5
)

rem ----- Read script arguments (override local settings) -----
if not ["%1"]    == [""]      set "VCVER=%1"
if not ["%2"]    == [""]      set "ARCH=%2"
if /I ["%ARCH%"] == ["win32"] set "ARCH=32"
if /I ["%ARCH%"] == ["win64"] set "ARCH=64"
if /I ["%3"]     == ["debug"] set "CASDEB=d"
if /I ["%3"]     == ["d"]     set "CASDEB=d"
if /I ["%3"]     == ["i"]     set "CASDEB=i"
if /I ["%3"]     == ["relwithdeb"] set "CASDEB=i"

rem Decode VCVER variable and define related ones:
rem
rem VCFMT - "vc" followed by full version number of Visual Studio toolset
rem         (same as VCVER without optional suffix "-uwp")
rem VCLIB - name of folder contining binaries
rem         (same as VCVER except without third version in number)
rem VCPROP - name of required Visual Studion Workload (starting with VS 2017)
rem
rem Note that for VS before 2015 (vc14) always
rem VCFMT=VCLIB=VCVER and VCPROP=NativeDesktop

rem Since VS 2017, environment variables like VS100COMNTOOLS are not defined 
rem any more, we can only use vswhere.exe tool to find Visual Studio.
rem Add path to vswhere.exe
set "PATH=%PATH%;%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"

rem for vc10-12, interpretation is trivial
set VCFMT=%VCVER%
set VCLIB=%VCVER:~0,4%
set VCPROP=NativeDesktop
rem vc14 and later can have optional suffix "-uwp"
if "%VCVER:~-4%" == "-uwp" (
  set VCFMT=%VCVER:~0,-4%
  set VCLIB=%VCLIB%-uwp
  set VCPROP=Universal
)
rem echo VCVER=%VCVER% VCFMT=%VCFMT% VCLIB=%VCLIB% VCPROP=%VCPROP%

rem ----- Parsing of Visual Studio platform -----
set "VisualStudioExpressName=VCExpress"

if not "%DevEnvDir%" == "" (
  rem If DevEnvDir is already defined (e.g. in custom.bat), use that value
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
    set "DevEnvDir=%%i\Common7\IDE\"
  )
) else if /I "%VCFMT%" == "gcc" (
  rem MinGW
) else (
  echo Error: wrong VS identifier
  exit /B
)

rem ----- Parsing vcvarsall for qt samples and define PlatformToolset -----
if /I "%VCFMT%" == "vc10" (
  set "VCVARS=%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"
  set "VCPlatformToolSet=v100"
) else if /I "%VCFMT%" == "vc11" (
  set "VCVARS=%VS110COMNTOOLS%..\..\VC\vcvarsall.bat"
  set "VCPlatformToolSet=v110"
) else if /I "%VCFMT%" == "vc12" (
  set "VCVARS=%VS120COMNTOOLS%..\..\VC\vcvarsall.bat"
  set "VCPlatformToolSet=v120"
) else if /I "%VCFMT%" == "vc14" (
  set "VCVARS=%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"
  set "VCPlatformToolSet=v140"
) else if /I "%VCFMT%" == "vc141" (
  for /f "usebackq delims=" %%i in (`vswhere.exe -version "[15.0,15.99]" -requires Microsoft.VisualStudio.Workload.%VCPROP% -property installationPath`) do (
    set "VCVARS=%%i\VC\Auxiliary\Build\vcvarsall.bat"
  )
  set "VCPlatformToolSet=v141"
) else if /I "%VCFMT%" == "gcc" (
  rem MinGW
) else (
  echo Error: first argument ^(%VCVER%^) should specify supported version of Visual C++,
  echo one of: vc10 ^(VS 2010 SP3^), vc11 ^(VS 2012 SP3^), vc12 ^(VS 2013^) or vc14 ^(VS 2015^)
  exit
)

set "CSF_OPT_LIB32D=%CSF_OPT_LIB32%"
set "CSF_OPT_LIB64D=%CSF_OPT_LIB64%"
set "CSF_OPT_BIN32D=%CSF_OPT_BIN32%"
set "CSF_OPT_BIN64D=%CSF_OPT_BIN64%"
set "CSF_OPT_LIB32I=%CSF_OPT_LIB32%"
set "CSF_OPT_LIB64I=%CSF_OPT_LIB64%"
set "CSF_OPT_BIN32I=%CSF_OPT_BIN32%"
set "CSF_OPT_BIN64I=%CSF_OPT_BIN64%"

rem ----- Optional 3rd-parties should be enabled by HAVE macros -----
set "CSF_OPT_CMPL="
set "PRODUCTS_DEFINES="
if ["%HAVE_TBB%"]       == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_TBB"       & set "CSF_DEFINES=HAVE_TBB;%CSF_DEFINES%"
if ["%HAVE_OPENCL%"]    == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_OPENCL"    & set "CSF_DEFINES=HAVE_OPENCL;%CSF_DEFINES%"
if ["%HAVE_GL2PS%"]     == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_GL2PS"     & set "CSF_DEFINES=HAVE_GL2PS;%CSF_DEFINES%"
if ["%HAVE_FREEIMAGE%"] == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_FREEIMAGE" & set "CSF_DEFINES=HAVE_FREEIMAGE;%CSF_DEFINES%"
if ["%HAVE_FFMPEG%"]    == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_FFMPEG"    & set "CSF_DEFINES=HAVE_FFMPEG;%CSF_DEFINES%"
if ["%HAVE_VTK%"]       == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_VTK"       & set "CSF_DEFINES=HAVE_VTK;%CSF_DEFINES%"
if ["%HAVE_GLES2%"]     == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_GLES2"     & set "CSF_DEFINES=HAVE_GLES2;%CSF_DEFINES%"
if ["%HAVE_D3D%"]       == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_D3D"       & set "CSF_DEFINES=HAVE_D3D;%CSF_DEFINES%"
if ["%HAVE_ZLIB%"]      == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_ZLIB"      & set "CSF_DEFINES=HAVE_ZLIB;%CSF_DEFINES%"
if ["%HAVE_LIBLZMA%"]   == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_LIBLZMA"   & set "CSF_DEFINES=HAVE_LIBLZMA;%CSF_DEFINES%"

rem Eliminate VS warning
if ["%CSF_DEFINES%"]  == [""] set "CSF_DEFINES=;"

rem ----- Optional 3rd-parties should be enabled by HAVE macros -----
if not ["%PRODUCTS_DEFINES%"] == [""] set "CSF_OPT_CMPL=%CSF_OPT_CMPL% %PRODUCTS_DEFINES%"

rem ----- Colect 3rd-parties additional include paths into compiler options -----
for %%a in ("%CSF_OPT_INC:;=";"%") do (
  set "anItem=%%~a"
  if not ["%%~a"] == [""] call :concatCmplInc %%~a
)

rem ----- Colect 3rd-parties additional library paths (32-bit) into linker options -----
set "OPT_LIB32="
for %%a in ("%CSF_OPT_LIB32:;=";"%") do (
  set "anItem=%%~a"
  if not ["%%~a"] == [""] call :concatLib32 %%~a
)

rem ----- Colect 3rd-parties additional library paths (64-bit) into linker options -----
set "OPT_LIB64="
for %%a in ("%CSF_OPT_LIB64:;=";"%") do (
  set "anItem=%%~a"
  if not ["%%~a"] == [""] call :concatLib64 %%~a
)

set "CSF_OPT_LNK32=%CSF_OPT_LNK32% %OPT_LIB32%"
set "CSF_OPT_LNK64=%CSF_OPT_LNK64% %OPT_LIB64%"
set "CSF_OPT_LNK32D=%CSF_OPT_LNK32D% %OPT_LIB32%"
set "CSF_OPT_LNK64D=%CSF_OPT_LNK64D% %OPT_LIB64%"
set "CSF_OPT_LNK32I=%CSF_OPT_LNK32I% %OPT_LIB32%"
set "CSF_OPT_LNK64I=%CSF_OPT_LNK64I% %OPT_LIB64%"

rem ----- Default paths to sub-folders (can be different in install env) -----
if "%CSF_OCCTIncludePath%" == "" set "CSF_OCCTIncludePath=%CASROOT%\inc"
if "%CSF_OCCTResourcePath%" == "" set "CSF_OCCTResourcePath=%CASROOT%\src"
if "%CSF_OCCTSamplesPath%" == "" set "CSF_OCCTSamplesPath=%CASROOT%\samples"
if "%CSF_OCCTDataPath%" == "" set "CSF_OCCTDataPath=%CASROOT%\data"
if "%CSF_OCCTTestsPath%" == "" set "CSF_OCCTTestsPath=%CASROOT%\tests"
if "%CSF_OCCTBinPath%" == "" set "CSF_OCCTBinPath=%CASROOT%\win%ARCH%\%VCLIB%\bin%CASDEB%"
if "%CSF_OCCTLibPath%" == "" set "CSF_OCCTLibPath=%CASROOT%\win%ARCH%\%VCLIB%\lib%CASDEB%"

rem ----- Set path to 3rd party and OCCT libraries -----
set "PATH=%CSF_OCCTBinPath%;%PATH%"
if ["%CASDEB%"] == [""] if ["%ARCH%"] == ["32"] set "PATH=%CSF_OPT_BIN32%;%PATH%"
if ["%CASDEB%"] == [""] if ["%ARCH%"] == ["64"] set "PATH=%CSF_OPT_BIN64%;%PATH%"
if ["%CASDEB%"] == ["d"] if ["%ARCH%"] == ["32"] set "PATH=%CSF_OPT_BIN32D%;%PATH%"
if ["%CASDEB%"] == ["d"] if ["%ARCH%"] == ["64"] set "PATH=%CSF_OPT_BIN64D%;%PATH%"
if ["%CASDEB%"] == ["i"] if ["%ARCH%"] == ["32"] set "PATH=%CSF_OPT_BIN32I%;%PATH%"
if ["%CASDEB%"] == ["i"] if ["%ARCH%"] == ["64"] set "PATH=%CSF_OPT_BIN64I%;%PATH%"

rem ----- Set envoronment variables used by OCCT -----
set CSF_LANGUAGE=us
set MMGT_CLEAR=1
set "CSF_SHMessage=%CSF_OCCTResourcePath%\SHMessage"
set "CSF_MDTVTexturesDirectory=%CSF_OCCTResourcePath%\Textures"
set "CSF_ShadersDirectory=%CSF_OCCTResourcePath%\Shaders"
set "CSF_XSMessage=%CSF_OCCTResourcePath%\XSMessage"
set "CSF_TObjMessage=%CSF_OCCTResourcePath%\TObj"
set "CSF_StandardDefaults=%CSF_OCCTResourcePath%\StdResource"
set "CSF_PluginDefaults=%CSF_OCCTResourcePath%\StdResource"
set "CSF_XCAFDefaults=%CSF_OCCTResourcePath%\StdResource"
set "CSF_TObjDefaults=%CSF_OCCTResourcePath%\StdResource"
set "CSF_StandardLiteDefaults=%CSF_OCCTResourcePath%\StdResource"
set "CSF_IGESDefaults=%CSF_OCCTResourcePath%\XSTEPResource"
set "CSF_STEPDefaults=%CSF_OCCTResourcePath%\XSTEPResource"
set "CSF_XmlOcafResource=%CSF_OCCTResourcePath%\XmlOcafResource"
set "CSF_MIGRATION_TYPES=%CSF_OCCTResourcePath%\StdResource\MigrationSheet.txt"

rem Draw Harness special stuff
if exist "%CSF_OCCTResourcePath%\DrawResources\DrawDefault" (
  set "DRAWDEFAULT=%CSF_OCCTResourcePath%\DrawResources\DrawDefault"
)
if exist "%CSF_OCCTResourcePath%\DrawResources" (
  set "DRAWHOME=%CSF_OCCTResourcePath%\DrawResources"
  set "CSF_DrawPluginDefaults=%DRAWHOME%"
)
goto :eof

:concatCmplInc
set "CSF_OPT_CMPL=%CSF_OPT_CMPL% -I%1"
goto :eof

:concatLib32
rem Compiler options for Code::Blocks: -L for gcc/mingw and /LIBPATH for msvc
rem set "OPT_LIB32=%OPT_LIB32% /LIBPATH:%1"
set "OPT_LIB32=%OPT_LIB32% -L%1"
goto :eof

:concatLib64
rem Compiler options for Code::Blocks: -L for gcc/mingw and /LIBPATH for msvc
rem set "OPT_LIB64=%OPT_LIB64% /LIBPATH:%1"
set "OPT_LIB64=%OPT_LIB64% -L%1"
goto :eof
