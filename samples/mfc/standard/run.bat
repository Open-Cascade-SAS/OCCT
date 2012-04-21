echo %4
if "%4" EQU "" (
  echo Launch selected sample as follows: 
  echo Run.bat [^vc8^|^vc9^|^vc10^] [^win32^|^win64^] [^Release^|^Debug^] [^SampleName^]
  echo Use option -d to run Debug mode
  EXIT /B
)

call ../../../env.bat %1 %2 %3
call %~dp0env.bat

IF NOT EXIST %BIN_DIR%\%4.exe goto ERR_EXE

start %BIN_DIR%\%4.exe

GOTO END

:ERR_EXE
ECHO Executable %BIN_DIR%\%~n1.exe not found."
ECHO Probably you didn't compile the application.
EXIT /B

:END
