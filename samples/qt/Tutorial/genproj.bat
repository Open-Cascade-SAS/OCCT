@echo off
REM Generation of vcproj files with qmake utilite
REM Variable QTDIR and PATH to qmake executable must be defined without fail

REM Use first argument to specify version of Visual Studio (vc10, vc11, vc12 or vc14),
REM second argument specifies architecture) (win32 or win64)
REM third argument specifies Debug or Release mode

call "%~dp0env.bat" %1 %2 %3

call "%VCVARS%" %VCARCH%

qmake -tp vc -r -o Tutorial.sln Tutorial0.pro
