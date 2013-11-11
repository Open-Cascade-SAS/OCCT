@echo off

if ["%1"] == [""] (
  echo Launch selected sample as follows:
  echo sample.bat SampleName
  echo available samples:
  echo    Geometry
  echo    Modeling
  echo    Viewer2d
  echo    Viewer3d
  echo    ImportExport
  echo    Ocaf
  echo    Triangulation
  echo    HLR
  echo    Animation
  echo    Convert
  exit /B
)

if not exist "%~dp0/bin/%1.exe" (
  echo Executable %~dp0/bin/%4.exe not found.
  echo Probably you didn't compile the application.
  exit /B
)

call "%~dp0/env.bat"

"%~dp0/bin/%1.exe"
