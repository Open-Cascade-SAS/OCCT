@echo off

if NOT DEFINED DXSDK_DIR (
  echo ERROR: DirectX SDK is required in order to build the sample but it is not found in your system. Please install DirectX SDK and retry.
  exit /B
)

call "%~dp0..\..\env.bat" %1 %2 %3

if ["%CASDEB%"] == [""] (
  call "%~dp0..\..\msvc.bat" %VCVER% win%ARCH% Release %~dp0\CSharp_D3D.sln
) else (
  call "%~dp0..\..\msvc.bat" %VCVER% win%ARCH% Debug %~dp0\CSharp_D3D.sln
)