@ECHO OFF

rem Set environment for building OCCT mfc samples
rem Use first argument to specify version of Visual Studio (vc8, vc9, or vc10),
rem second argument specifies architecture) (win32 or win64)
rem - third argument specifies build mode,

rem Set build environment 
call "%~dp0..\..\ros\env_build.bat" %1 %2

rem Set build mode
set CASDEB=d
if not "%3" EQU "" (
    if /I "%3" == "Release" (
        set CASDEB=
    ) else if /I "%3" == "Debug" (
        set CASDEB=d
    ) else (
        echo Error: third argument ^(%3^) should specify build mode,
        echo one of: Debug or Release
        exit /B
    )
)
)

rem Workaround for problems of building by VS2010 in 64 bit mode
if /I "%VCVER%" == "vc10" (
	if  "%ARCH%" == "64" (
		echo Attention: The error can occured during the compilation in Visual Studio 2010, 
		echo 		64 bit mode with embedding manifests ^(TRACKER : error TRK0002: Failed to execute command:^).
		echo 		It's an error of Visual Studio 2010 which will be corrected in future versions of Visual Studio. 
		echo		More details can be found here: 
		echo 		https://connect.microsoft.com/VisualStudio/feedback/details/505682/x64-c-c-projects-cannot-compile
	)
)

SET OS_TYPE=win%ARCH%

IF NOT DEFINED CASROOT GOTO ERR_CASROOT

IF NOT DEFINED OS_TYPE GOTO ERR_OSTYPE

IF NOT EXIST "%CASROOT%\%OS_TYPE%\%VCVER%\bin\TKernel.dll" GOTO ERR_CASCADE

IF not "%CASDEB%" EQU "" (
SET CASLIBS=%CASROOT%\%OS_TYPE%\%VCVER%\bind
SET BIN_DIR=%OS_TYPE%\%VCVER%\bind
SET LIB_DIR=%OS_TYPE%\%VCVER%\libd
SET CSF_GraphicShr=%CASROOT%\%OS_TYPE%\%VCVER%\bind\TKOpenGl.dll
) ELSE (
SET CASLIBS=%CASROOT%\%OS_TYPE%\%VCVER%\bin
SET BIN_DIR=%OS_TYPE%\%VCVER%\bin
SET LIB_DIR=%OS_TYPE%\%VCVER%\lib
SET CSF_GraphicShr=%CASROOT%\%OS_TYPE%\%VCVER%\bin\TKOpenGl.dll
)

SET PATH=%~dp0%LIB_DIR%;%~dp0%BIN_DIR%;%CASLIBS%;%PATH%

SET RES_DIR=.\res

set CSF_ResourcesDefaults=%CASROOT%\..\samples\mfc\06_Ocaf\res
set CSF_PluginDefaults=%CASROOT%\..\samples\mfc\06_Ocaf\res

GOTO END

:ERR_CASROOT
ECHO Environment variable "CASROOT" has not been defined. Abort.
EXIT /B

:ERR_CASCADE
ECHO Environment variable "CASROOT" defined incorrectly. Abort.
EXIT /B

:ERR_OSTYPE
ECHO Environment variable "OS_TYPE" has not been defined. Abort.
EXIT /B

:END

