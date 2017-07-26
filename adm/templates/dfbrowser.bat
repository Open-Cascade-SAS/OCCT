@echo off

rem Setup environment and launch TInspector
call "%~dp0env.bat" %1 %2 %3

TInspectorEXE.exe --dfbrowser

