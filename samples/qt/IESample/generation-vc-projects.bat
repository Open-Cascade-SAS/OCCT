REM Generation of vcproj files with qmake utilite
REM Variable QTDIR and PATH to qmake executable must be defined without fail

REM Use first argument to specify version of Visual Studio (vc8, vc9, or vc10),
REM second argument specifies architecture) (win32 or win64)
REM third argument specifies Debug or Release mode

call ..\..\..\env.bat %1 %2 %3

qmake -tp vc -r IESample.pro
