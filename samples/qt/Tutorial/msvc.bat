call "%~dp0..\..\..\env.bat" %1 %2 %3
call "%~dp0env.bat"

if ["%CASDEB%"] == [""] (
  call "%~dp0..\..\..\msvc.bat" %VCVER% win%ARCH% "" "%~dp0Tutorial-%VCVER%.sln"
) else (
  call "%~dp0..\..\..\msvc.bat" %VCVER% win%ARCH% %CASDEB% "%~dp0Tutorial-%VCVER%.sln"
)