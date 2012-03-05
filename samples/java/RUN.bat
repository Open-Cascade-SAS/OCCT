@ECHO OFF

call "%~dp0env.bat"

IF NOT DEFINED CASROOT GOTO ERR_CASROOT

SET ROOT=%CASROOT%\..

SET SAMPLEHOME=%ROOT%\samples\java

SET LIB_DIR=win32\vc8\lib
SET JAR_DIR=win32

IF NOT EXIST "%JAR_DIR%\CASCADESamples.jar" GOTO ERR_MAKE

SET OLD_PATH=%PATH%
set PATH=%~dp0;%SAMPLEHOME%\%LIB_DIR%;%PATH%

IF NOT EXIST "%SAMPLEHOME%\win32\vc8\obj" SET JAVAHOME=%ROOT%\3rdparty\win32\java

IF NOT DEFINED JAVAHOME GOTO ERR_JAVAHOME

SET PATH=%JAVAHOME%\bin;%JAVAHOME%\jre\bin;%JAVAHOME%\jre\bin\client;%PATH%

cd java
echo "javaw.exe -Xss1m -classpath .;%SAMPLEHOME%\%JAR_DIR%\CASCADESamples.jar SamplesStarter"
start javaw.exe -Xss1m -classpath .;%SAMPLEHOME%\%JAR_DIR%\CASCADESamples.jar SamplesStarter
pause
cd ..
SET PATH=%OLD_PATH%

GOTO END

:ERR_CASROOT
ECHO Environment variable "CASROOT" not defined. Abort.
pause
GOTO END

:ERR_JAVAHOME
ECHO Environment variable "JAVAHOME" not defined. Abort.
pause
GOTO END

:ERR_MAKE
ECHO File %JAR_DIR%\CASCADESamples.jar not found."
ECHO Probably you don't compile the application. Execute %JAR_DIR\build.bat
pause
GOTO END

:END
