@echo OFF

rem Extract version info from version.cmake
for /f tokens^=2^ delims^=^" %%i in ('findstr OCC_VERSION_DEVELOPMENT "%~dp0\..\cmake\version.cmake"') do ( set "anOcctVerSuffix=%%i" )
for /f tokens^=3 %%i in ('findstr OCC_VERSION_MAJOR "%~dp0\..\cmake\version.cmake"') do ( set "OCC_VERSION_MAJOR=%%i" )
for /f tokens^=3 %%i in ('findstr OCC_VERSION_MINOR "%~dp0\..\cmake\version.cmake"') do ( set "OCC_VERSION_MINOR=%%i" )
for /f tokens^=3 %%i in ('findstr OCC_VERSION_MAINTENANCE "%~dp0\..\cmake\version.cmake"') do ( set "OCC_VERSION_MAINTENANCE=%%i" )
set "anOcctVersion=%OCC_VERSION_MAJOR%.%OCC_VERSION_MINOR%.%OCC_VERSION_MAINTENANCE%"
