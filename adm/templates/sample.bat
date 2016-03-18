@echo off

if ["%1"] == [""] (
  echo Launch selected sample as follows:
  echo   sample.bat SampleName vc10 win32 d
  echo or to use last sample build configuration:
  echo   sample.bat SampleName
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
  PAUSE
  exit /B
)

call "%~dp0env.bat" %2 %3 %4
set "EXE_PATH=%CSF_OCCTBinPath%/%1.exe"

if not exist "%EXE_PATH%" (
  echo Executable %EXE_PATH% not found.
  echo Probably you didn't compile the application.
  PAUSE
  exit /B
)

"%EXE_PATH%"

