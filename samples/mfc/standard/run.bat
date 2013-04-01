@echo off

if ["%4"] == [""] (
  echo Launch selected sample as follows: 
  echo Run.bat [^vc8^|^vc9^|^vc10^|^vc11^] [^win32^|^win64^] [^Release^|^Debug^] [^SampleName^]
  echo Use option -d to run Debug mode
  exit /B
)

call "%~dp0..\..\..\env.bat" %1 %2 %3
call "%~dp0env.bat"

if not exist "%~dp0%BIN_DIR%\%4.exe" goto err_exe

"%~dp0%BIN_DIR%\%4.exe"

goto eof

:err_exe
echo Executable %~dp0%BIN_DIR%\%4.exe not found.
echo Probably you didn't compile the application.
exit /B

:eof
