call "%~dp0env.bat" %1 %2 %3

if ["%CASDEB%"] == [""] (
  call "%~dp0..\..\..\msvc.bat" %VCVER% win%ARCH% "" "%~dp0IESample-%VCVER%.sln"
) else (
  call "%~dp0..\..\..\msvc.bat" %VCVER% win%ARCH% %CASDEB% "%~dp0IESample-%VCVER%.sln"
)