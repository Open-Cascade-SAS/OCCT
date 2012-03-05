@ECHO OFF

call %~dp0env.bat

@echo Starting VoxelDemo .....
start .\%BIN_DIR%\VoxelDemo.exe
