@echo off

call "%~dp0..\..\..\env.bat" %1 %2 %3

call "custom.bat" %1 %2 %3

set "PATH=%QTDIR%/bin;%PATH%"