Setlocal EnableDelayedExpansion

call "%~dp0env.bat" %1 %2 %3 %4

set "BIN_DIR=win%ARCH%\%VCVER%\bind"
if ["%CASDEB%"] == [""] (
  set "BIN_DIR=win%ARCH%\%VCVER%\bin"
)

if not exist "%~dp0%BIN_DIR%\OCCTOverview.exe" goto ERR_EXE

echo Starting OCCTOverview .....
"%~dp0%BIN_DIR%\OCCTOverview.exe" %4

goto END

:ERR_EXE
echo Executable %~dp0%BIN_DIR%\OCCTOverview.exe not found.
echo Probably you didn't compile the application.
pause
goto END

:END