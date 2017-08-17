@echo off

call "%~dp0..\..\..\env.bat" %1 %2 %3

call "custom.bat" %1 %2 %3

set "PATH=%QTDIR%/bin;%PATH%"
set "QT_QPA_PLATFORM_PLUGIN_PATH=%QTDIR%\plugins\platforms"