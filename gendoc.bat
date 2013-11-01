@echo off

rem Helper script to run generation of OCCT documentation on Windows.
rem Running it requires that Tcl, Doxygen, and MikTex (for PDF generation)
rem should be in the PATH

if exist "%~dp0env.bat" (
  call "%~dp0env.bat"
)

if not ["%1"] == ["-h"] (
  tclsh86.exe %~dp0dox/start.tcl  %*
) else (
  echo.
  echo  gen.bat options:
  echo  -html                : To generate HTML files ^(cannot be used with -pdf^)
  echo  -pdf                 : To generate PDF files ^(cannot be used with -html^)
  echo  -m^=^<modules_list^> : Specifies list of articles to generate. If it is not specified, all files, mentioned in FILES.txt are processed
  echo  "-l=<document_name>  : Specifies the article caption for a single document"
  echo  -h                   : Prints help message
  echo  -v                   : Specifies the Verbose mode ^(info on all script actions is shown^)
  echo.
)
