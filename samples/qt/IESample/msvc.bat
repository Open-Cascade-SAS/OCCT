call ../../../env.bat %1 %2 %3
call %~dp0env.bat
if not ["%CASDEB%"] == [""] call ../../../msvc.bat %VCVER% win%ARCH% %CASDEB% IESample-%VCVER%.sln
if ["%CASDEB%"] == [""] call ../../../msvc.bat %VCVER% win%ARCH% "" IESample-%VCVER%.sln