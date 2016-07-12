@echo off

Setlocal EnableDelayedExpansion

if "%1" == "-h" (
  goto err_bat
)

if not ["%4"] == [""] (
  set "SampleName=%4"
  call "%~dp0env.bat" %1 %2 %3
) else if not ["%1"] == [""] (
  set "SampleName=%1"
  call "%~dp0env.bat"
) else (
  goto err_bat
)

if not exist "%~dp0%BIN_DIR%\%SampleName%.exe" goto err_exe

"%~dp0%BIN_DIR%\%SampleName%.exe"

goto eof

:err_bat
echo Launch selected sample as follows:
echo   %~n0.bat [^vc10^|^vc11^|^vc12^|^vc14^] [^win32^|^win64^] [^Release^|^Debug^] [^SampleName^]
echo or
echo   %~n0.bat [^SampleName^]
echo Run %~n0.bat -h to get this help
exit /B

:err_exe
echo Executable %~dp0%BIN_DIR%\%SampleName%.exe not found.
echo Probably you didn't compile the application.
exit /B

:eof
