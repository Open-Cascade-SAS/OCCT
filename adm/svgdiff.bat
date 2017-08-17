@echo off
rem Script to diff SVG images visually (as PNG) in TortoiseGit client
rem 
rem It assumes that Inkscape and TortoiseGitIDiff executables are either 
rem installed in default locations in Program Files, or are accessible by PATH
rem 
rem To use this script for diffing SVG images, open TortoiseGit settings
rem (Start -> Programs -> TortoiseGit -> Settings), select "Diff Viewer",
rem click button "Advanced..." on the right tab and then add new record:
rem - Extension: .svg
rem - External program: <path to OCCT>\adm\svgdiff.bat %base %mine %bname %yname

rem Remove double quotes around arguments
set "f1=%1"
set "f2=%2"
set "f1=%f1:~1,-1%.png"
set "f2=%f2:~1,-1%.png"

rem Check if Inkscape and TortoiseGit are installed in default locations in 
rem ProgramFiles; if not, assume they still may be accessible by PATH
set "inkscape=%ProgramFiles%\Inkscape\inkscape.exe"
if not exist "%inkscape%" set inkscape=inkscape.exe
set "tgitidiff=%ProgramFiles%\TortoiseGit\bin\TortoiseGitIDiff.exe"
if not exist "%tgitidiff%" set tgitidiff=TortoiseGitIDiff.exe

rem Convert SVG to PNG using Inkscape
"%inkscape%" -e "%f1%" %1
"%inkscape%" -e "%f2%" %2

rem Call Tortoise differ
"%tgitidiff%" /left:"%f1%" /right:"%f2%" /lefttitle:%3 /righttitle:%4
