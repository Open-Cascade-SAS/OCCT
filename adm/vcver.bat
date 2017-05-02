@echo off
rem Decode VCVER variable and define related ones:
rem
rem VCFMT - "vc" followed by full version number of Visual Studio toolset
rem         (same as VCVER without optional suffix "-uwp")
rem VCLIB - name of folder contining binaries
rem         (same as VCVER except without third version in number)
rem VCPROP - name of required Visual Studion Workload (starting with VS 2017)
rem
rem Note that for VS before 2015 (vc14) always
rem VCFMT=VCLIB=VCVER and VCPROP=NativeDesktop

rem Since VS 2017, environment variables like VS100COMNTOOLS are not defined 
rem any more, we can only use vswhere.exe tool to find Visual Studio.
rem Add path to vswhere.exe
set "PATH=%PATH%;%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"

rem for vc10-12, interpretation is trivial
set VCFMT=%VCVER%
set VCLIB=%VCVER:~0,4%
set VCPROP=NativeDesktop

rem vc14 and later can have optional suffix "-uwp"
if "%VCVER:~-4%" == "-uwp" (
  set VCFMT=%VCVER:~0,-4%
  set VCLIB=%VCLIB%-uwp
  set VCPROP=Universal
)

rem echo VCVER=%VCVER% VCFMT=%VCFMT% VCLIB=%VCLIB% VCPROP=%VCPROP%
