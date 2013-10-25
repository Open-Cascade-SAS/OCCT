@echo off

if "%1" == "-h" (
  echo Launch WinForms sample as follows: 
  echo %~n0 ^[vc9^] ^[win32^|win64^] ^[Debug^|Release^]
  echo By default configuration set in ..\..\custom.bat is used
  echo Run %~n0 -h to get this help
  exit /B
)

call "%~dp0..\..\env.bat" %1 %2 %3

set "EXEC=%~dp0\win%ARCH%\bin%CASDEB%\IE_WinForms.exe"

if not exist "%EXEC%" goto err_exe

"%EXEC%"

goto eof

:err_exe
echo Executable %EXEC% not found.
echo Check that OCCT and sample are built with the selected configuration:
echo compiler=%VCVER% platform=win%ARCH% %3
exit /B

:eof