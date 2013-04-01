REM Generation of vcproj files with qmake utilite
REM Variable QTDIR and PATH to qmake executable must be defined without fail

REM Use first argument to specify version of Visual Studio (vc8, vc9, or vc10),
REM second argument specifies architecture) (win32 or win64)
REM third argument specifies Debug or Release mode

call "%~dp0..\..\..\env.bat" %1 %2 %3
call "%~dp0env.bat"

if not "%1" == "" (
    if /I "%1" == "vc8" (
        set VCVER=vc8
        set "VCVARS=%VS80COMNTOOLS%..\..\VC\vcvarsall.bat"
    ) else if /I "%1" == "vc9" (
        set VCVER=vc9
        set "VCVARS=%VS90COMNTOOLS%..\..\VC\vcvarsall.bat"
    ) else if /I "%1" == "vc10" (
        set VCVER=vc10
        set "VCVARS=%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"
    ) else if /I "%1" == "vc11" (
        set VCVER=vc11
        set "VCVARS=%VS110COMNTOOLS%..\..\VC\vcvarsall.bat"
    ) else (
        echo Error: first argument ^(%1^) should specify supported version of Visual C++,
        echo one of: vc8 ^(VS 2005 SP1^), vc9 ^(VS 2008 SP1^), vc10 ^(VS 2010^) or vc11 ^(VS 2012^)
        exit
    )
)

if ["%ARCH%"] == ["32"] set VCARCH=x86
if ["%ARCH%"] == ["64"] set VCARCH=amd64

call "%VCVARS%" %VCARCH%

qmake -tp vc -r Tutorial.pro
