call ../../env.bat %1 %2 %3
if not ["%CASDEB%"] == [""] set "BinDir=Debug"
if ["%CASDEB%"] == [""] set "BinDir=Release"
%~dp0IE\bin\%BinDir%\IE.exe