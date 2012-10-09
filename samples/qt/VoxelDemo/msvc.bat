call ../../../env.bat %1 %2 %3

if not ["%CASDEB%"] == [""] call ../../../msvc.bat %VCVER% win%ARCH% %CASDEB% VoxelDemo-%VCVER%.sln
if ["%CASDEB%"] == [""] call ../../../msvc.bat %VCVER% win%ARCH% "" VoxelDemo-%VCVER%.sln