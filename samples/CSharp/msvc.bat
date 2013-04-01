call "%~dp0..\..\env.bat" %1 %2 %3

if ["%CASDEB%"] == [""] (
  call "%~dp0..\..\msvc.bat" %VCVER% win%ARCH% "" "%~dp0IE\IE.sln"
) else (
  call "%~dp0..\..\msvc.bat" %VCVER% win%ARCH% %CASDEB% "%~dp0IE\IE.sln"
)