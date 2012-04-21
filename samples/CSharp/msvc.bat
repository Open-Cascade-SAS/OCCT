call ../../env.bat %1 %2 %3
if not ["%CASDEB%"] == [""] call ../../msvc.bat %VCVER% win%ARCH% %CASDEB% %~dp0IE\IE.sln
if ["%CASDEB%"] == [""] call ../../msvc.bat %VCVER% win%ARCH% "" %~dp0IE\IE.sln