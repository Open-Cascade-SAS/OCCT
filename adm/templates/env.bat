@echo off

rem Use:
rem - first argument specifies version of Visual Studio (vc8, vc9, or vc10),
rem - second argument specifies architecture (win32 or win64),
rem - third argument specifies build mode (Debug or Release)
rem Default options are:
rem   vc8 win32 Release

set "SCRIPTROOT=%~dp0"
set "SCRIPTROOT=%SCRIPTROOT:~0,-1%"
set "CASROOT=__CASROOT__"
if not ["%CASROOT%"] == [""] if exist "%SCRIPTROOT%\%CASROOT%" set "CASROOT=%SCRIPTROOT%\%CASROOT%"
if     ["%CASROOT%"] == [""] set "CASROOT=%SCRIPTROOT%"

rem ----- Reset values to defaults -----
set "CASDEB="
set "VCVER=vc8"
set "ARCH=32"
set "VCVARS="
set "HAVE_TBB=false"
set "HAVE_OPENCL=false"
set "HAVE_FREEIMAGE=false"
set "HAVE_GL2PS=false"
set "HAVE_OPENCL=false"
set "HAVE_VTK=false"
set "CSF_OPT_INC="
set "CSF_OPT_LIB32="
set "CSF_OPT_LIB64="
set "CSF_OPT_BIN32="
set "CSF_OPT_BIN64="

rem ----- Load local settings -----
if exist "%~dp0custom.bat" (
  call "%~dp0custom.bat" %1 %2 %3 %4 %5
)

rem ----- Read script arguments (override local settings) -----
if not ["%1"]    == [""]      set "VCVER=%1"
if not ["%2"]    == [""]      set "ARCH=%2"
if /I ["%ARCH%"] == ["win32"] set "ARCH=32"
if /I ["%ARCH%"] == ["win64"] set "ARCH=64"
if /I ["%3"]     == ["debug"] set "CASDEB=d"
if /I ["%3"]     == ["d"]     set "CASDEB=d"

set "CSF_OPT_INC=%CSF_OPT_INC%;%CASROOT%\inc"
set "CSF_OPT_LIB32D=%CSF_OPT_LIB32%;%CASROOT%\win32\%VCVER%\libd"
set "CSF_OPT_LIB64D=%CSF_OPT_LIB64%;%CASROOT%\win64\%VCVER%\libd"
set "CSF_OPT_BIN32D=%CSF_OPT_BIN32%;%CASROOT%\win32\%VCVER%\bind"
set "CSF_OPT_BIN64D=%CSF_OPT_BIN64%;%CASROOT%\win64\%VCVER%\bind"
set "CSF_OPT_LIB32=%CSF_OPT_LIB32%;%CASROOT%\win32\%VCVER%\lib"
set "CSF_OPT_LIB64=%CSF_OPT_LIB64%;%CASROOT%\win64\%VCVER%\lib"
set "CSF_OPT_BIN32=%CSF_OPT_BIN32%;%CASROOT%\win32\%VCVER%\bin"
set "CSF_OPT_BIN64=%CSF_OPT_BIN64%;%CASROOT%\win64\%VCVER%\bin"

rem ----- Optional 3rd-parties should be enabled by HAVE macros -----
set "CSF_OPT_CMPL="
set "PRODUCTS_DEFINES="
if ["%HAVE_TBB%"]       == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_TBB"
if ["%HAVE_OPENCL%"]    == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_OPENCL"
if ["%HAVE_GL2PS%"]     == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_GL2PS"
if ["%HAVE_FREEIMAGE%"] == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_FREEIMAGE"
if ["%HAVE_VTK%"]       == ["true"] set "PRODUCTS_DEFINES=%PRODUCTS_DEFINES% -DHAVE_VTK"
if ["%HAVE_TBB%"]       == ["true"] set "CSF_DEFINES=HAVE_TBB;%CSF_DEFINES%"
if ["%HAVE_OPENCL%"]    == ["true"] set "CSF_DEFINES=HAVE_OPENCL;%CSF_DEFINES%"
if ["%HAVE_GL2PS%"]     == ["true"] set "CSF_DEFINES=HAVE_GL2PS;%CSF_DEFINES%"
if ["%HAVE_FREEIMAGE%"] == ["true"] set "CSF_DEFINES=HAVE_FREEIMAGE;%CSF_DEFINES%"
if ["%HAVE_VTK%"]       == ["true"] set "CSF_DEFINES=HAVE_VTK;%CSF_DEFINES%"

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

set "CSF_OPT_LNK32="
set "CSF_OPT_LNK64="
set "CSF_OPT_LNK32D="
set "CSF_OPT_LNK64D="
set "CSF_OPT_LNK32=%CSF_OPT_LNK32% %OPT_LIB32%"
set "CSF_OPT_LNK32D=%CSF_OPT_LNK32D% %OPT_LIB32%"
set "CSF_OPT_LNK64=%CSF_OPT_LNK64% %OPT_LIB64%"
set "CSF_OPT_LNK64D=%CSF_OPT_LNK64D% %OPT_LIB64%"

set "CASBIN=win%ARCH%\%VCVER%"

rem ----- Set path to 3rd party and OCCT libraries -----
set "PATH=%SCRIPTROOT%\%CASBIN%\bin%CASDEB%;%PATH%"
if     ["%CASDEB%"] == [""] if ["%ARCH%"] == ["32"] set "PATH=%CSF_OPT_BIN32%;%PATH%"
if     ["%CASDEB%"] == [""] if ["%ARCH%"] == ["64"] set "PATH=%CSF_OPT_BIN64%;%PATH%"
if not ["%CASDEB%"] == [""] if ["%ARCH%"] == ["32"] set "PATH=%CSF_OPT_BIN32D%;%PATH%"
if not ["%CASDEB%"] == [""] if ["%ARCH%"] == ["64"] set "PATH=%CSF_OPT_BIN64D%;%PATH%"

rem ----- Set envoronment variables used by OCCT -----
set CSF_LANGUAGE=us
set MMGT_CLEAR=1
set CSF_EXCEPTION_PROMPT=1
set "CSF_SHMessage=%CASROOT%\src\SHMessage"
set "CSF_MDTVTexturesDirectory=%CASROOT%\src\Textures"
set "CSF_ShadersDirectory=%CASROOT%\src\Shaders"
set "CSF_XSMessage=%CASROOT%\src\XSMessage"
set "CSF_TObjMessage=%CASROOT%\src\TObj"
set "CSF_StandardDefaults=%CASROOT%\src\StdResource"
set "CSF_PluginDefaults=%CASROOT%\src\StdResource"
set "CSF_XCAFDefaults=%CASROOT%\src\StdResource"
set "CSF_TObjDefaults=%CASROOT%\src\StdResource"
set "CSF_StandardLiteDefaults=%CASROOT%\src\StdResource"
set "CSF_UnitsLexicon=%CASROOT%\src\UnitsAPI\Lexi_Expr.dat"
set "CSF_UnitsDefinition=%CASROOT%\src\UnitsAPI\Units.dat"
set "CSF_IGESDefaults=%CASROOT%\src\XSTEPResource"
set "CSF_STEPDefaults=%CASROOT%\src\XSTEPResource"
set "CSF_XmlOcafResource=%CASROOT%\src\XmlOcafResource"
set "CSF_MIGRATION_TYPES=%CASROOT%\src\StdResource\MigrationSheet.txt"

rem Draw Harness special stuff
if exist "%CASROOT%\src\DrawResources" (
  set "DRAWHOME=%CASROOT%\src\DrawResources"
  set "CSF_DrawPluginDefaults=%DRAWHOME%"
)
if exist "%SCRIPTROOT%\src\DrawResourcesProducts" (
  set "CSF_DrawPluginProductsDefaults=%SCRIPTROOT%\src\DrawResourcesProducts"
)
goto :eof

:concatCmplInc
set "CSF_OPT_CMPL=%CSF_OPT_CMPL% -I%1"
goto :eof

:concatLib32
set "OPT_LIB32=%OPT_LIB32% /LIBPATH:%1"
goto :eof

:concatLib64
set "OPT_LIB64=%OPT_LIB64% /LIBPATH:%1"
goto :eof
