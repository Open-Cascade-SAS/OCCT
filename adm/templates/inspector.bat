@echo off

rem Use:
rem - first argument specifies version of Visual Studio (vc8, vc9, or vc10),
rem - second argument specifies architecture (win32 or win64),
rem - third argument specifies build mode (Debug or Release)
rem - fourth and next arguments specify loaded tool plugins
rem     The arguments are: dfbrowser shapeview vinspector
rem     If there are no tool plugins are specified, all plugins will be loaded
rem Default options are:
rem   vc8 win32 Release

rem Setup environment and launch TInspector
call "%~dp0env.bat" %1 %2 %3

TInspectorEXE.exe %*

