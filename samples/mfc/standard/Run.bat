@ECHO OFF

call %~dp0..\..\ros\env.bat

if "%4" EQU "" (
  echo Launch selected sample as follows: 
  echo Run.bat [^vc8^|^vc9^|^vc10^] [^win32^|^win64^] [^Release^|^Debug^] [^SampleName^]
  echo Use option -d to run Debug mode
  EXIT /B
)

SET VCVER=%1
SET OS_TYPE=%2

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

IF not "%CASDEB%" EQU "" (
	SET EXEDIR=%~dp0%OS_TYPE%\%VCVER%\bind
	SET LIBDIR=%~dp0%OS_TYPE%\%VCVER%\libd
	SET CSF_GraphicShr=%CASROOT%\%OS_TYPE%\%VCVER%\bind\TKOpenGl.dll
) else (
	SET EXEDIR=%~dp0%OS_TYPE%\%VCVER%\bin
	SET LIBDIR=%~dp0%OS_TYPE%\%VCVER%\lib
	SET CSF_GraphicShr=%CASROOT%\%OS_TYPE%\%VCVER%\bin\TKOpenGl.dll
)

SET PATH=%LIBDIR%;%BIN_DIR%;%PATH%

SET RES_DIR=.\res

set CSF_ResourcesDefaults=%CASROOT%\..\samples\mfc\06_Ocaf\res
set CSF_PluginDefaults=%CASROOT%\..\samples\mfc\06_Ocaf\res

IF NOT EXIST %EXEDIR%\%4.exe goto ERR_EXE

start %EXEDIR%\%4.exe

GOTO END

:ERR_EXE
ECHO Executable %EXEDIR%\%~n1.exe not found."
ECHO Probably you didn't compile the application.
EXIT /B

:END
