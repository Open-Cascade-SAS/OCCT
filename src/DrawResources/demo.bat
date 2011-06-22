echo ON

echo " Open CASCADE "

if not DEFINED CASROOT     echo CASROOT is mandatory

if %PROCESSOR_ARCHITECTURE% EQU x86 Set STATION=Windows_NT



rem =========================================
rem
rem    LD_LIBRARY_PATH definition 
rem
rem =========================================

if NOT DEFINED OLDPATH set OLDPATH=%PATH%



set PATH=%PATH%;%CASROOT%\%STATION%\dll;

rem the Path where Tcl is installed

set PATH=D:/DevTools/Tcltk/bin/;%PATH%;

rem ======================================


Set DrawExe=%CASROOT%\%STATION%\bin\TTOPOLOGY.exe

# POP : Since C31 Version ; this Varaible are not Mandatory
rem if not DEFINED DRAWHOME    Set DRAWHOME=%CASROOT%/src/DrawResources
rem if not DEFINED DRAWDEFAULT set DRAWDEFAULT=%DRAWHOME%\\DrawDefault

Set MMGT_CLEAR=1
%DRAWEXE% 



