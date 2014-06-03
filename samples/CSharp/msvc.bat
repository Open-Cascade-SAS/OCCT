call "%~dp0..\..\env.bat" %1 %2 %3

if ["%CASDEB%"] == [""] (
  call "%~dp0..\..\msvc.bat" %VCVER% win%ARCH% Release %~dp0\CSharp.sln
) else (
  call "%~dp0..\..\msvc.bat" %VCVER% win%ARCH% Debug %~dp0\CSharp.sln
)