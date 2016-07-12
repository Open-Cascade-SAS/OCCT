@echo off
REM Generation of vcproj files with qmake utilite
REM Variable QTDIR and PATH to qmake executable must be defined without fail

REM Use first argument to specify version of Visual Studio (vc10, vc11, vc12 or vc14),
REM second argument specifies architecture) (win32 or win64)
REM third argument specifies Debug or Release mode

call "%~dp0env.bat" %1 %2 %3

if not "%VCVER%" == "" (
    if /I "%VCVER%" == "vc10" (
        set VCVER=vc10
        set "VCVARS=%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"
    ) else if /I "%VCVER%" == "vc11" (
        set VCVER=vc11
        set "VCVARS=%VS110COMNTOOLS%..\..\VC\vcvarsall.bat"
    ) else if /I "%VCVER%" == "vc12" (
        set VCVER=vc12
        set "VCVARS=%VS120COMNTOOLS%..\..\VC\vcvarsall.bat"
    ) else if /I "%VCVER%" == "vc14" (
        set VCVER=vc14
        set "VCVARS=%VS140COMNTOOLS%..\..\VC\vcvarsall.bat"
    ) else (
        echo Error: first argument ^(%VCVER%^) should specify supported version of Visual C++,
        echo one of: vc10 ^(VS 2010 SP3^), vc11 ^(VS 2012 SP3^), vc12 ^(VS 2013^) or vc14 ^(VS 2015^)
        exit
    )
)

if ["%ARCH%"] == ["32"] set VCARCH=x86
if ["%ARCH%"] == ["64"] set VCARCH=amd64

call "%VCVARS%" %VCARCH%

qmake -tp vc -r -o Tutorial.sln Tutorial0.pro
