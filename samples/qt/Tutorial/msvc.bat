call ../../../env.bat %1 %2 %3
call %~dp0env.bat
if not ["%CASDEB%"] == [""] call ../../../msvc.bat %VCVER% win%ARCH% %CASDEB% Tutorial-%VCVER%.sln
if ["%CASDEB%"] == [""] call ../../../msvc.bat %VCVER% win%ARCH% "" Tutorial-%VCVER%.sln