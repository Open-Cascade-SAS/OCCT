call "%~dp0..\..\..\env.bat" %1 %2 %3

if ["%CASDEB%"] == [""] (
  call "%~dp0..\..\..\msvc.bat" %VCVER% win%ARCH% "" "%~dp0VoxelDemo-%VCVER%.sln"
) else (
  call "%~dp0..\..\..\msvc.bat" %VCVER% win%ARCH% %CASDEB% "%~dp0VoxelDemo-%VCVER%.sln"
)