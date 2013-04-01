call "%~dp0..\..\env.bat" %1 %2 %3

set "BinDir=Debug"

if ["%CASDEB%"] == [""] (
 set "BinDir=Release"
)

"%~dp0IE\bin\%BinDir%\IE.exe"