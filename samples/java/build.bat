@ECHO OFF

call %~dp0..\..\ros\env_build.bat vc8 win32

IF NOT EXIST win32 mkdir win32
cd win32
IF NOT EXIST vc8 mkdir vc8
cd vc8

SET ROOT=%CASROOT%\..
SET SAMPLEHOME=%ROOT%\samples\java

IF NOT EXIST obj mkdir obj
IF NOT EXIST classes mkdir classes

del  CASCADESamples.jar

%JAVAHOME%\bin\javac  -d classes %CASROOT%\src\jcas\*.java %SAMPLEHOME%\java\CASCADESamplesJni\*.java %SAMPLEHOME%\java\SampleGeometryJni\*.java %SAMPLEHOME%\java\SamplesTopologyJni\*.java %SAMPLEHOME%\java\SampleViewer3DJni\*.java %SAMPLEHOME%\java\SampleAISBasicJni\*.java %SAMPLEHOME%\java\SampleAISDisplayModeJni\*.java %SAMPLEHOME%\java\SampleAISSelectJni\*.java %SAMPLEHOME%\java\SampleDisplayAnimationJni\*.java %SAMPLEHOME%\java\SampleImportExportJni\*.java %SAMPLEHOME%\java\SampleHLRJni\*.java %SAMPLEHOME%\java\util\*.java  %SAMPLEHOME%\java\*.java


cd classes
%JAVAHOME%\bin\jar -cf CASCADESamples.jar -C  . jcas\*.class CASCADESamplesJni\*.class SampleGeometryJni\*.class SamplesTopologyJni\*.class SampleViewer3DJni\*.class SampleAISBasicJni\*.class SampleAISDisplayModeJni\*.class SampleAISSelectJni\*.class SampleDisplayAnimationJni\*.class SampleImportExportJni\*.class SampleHLRJni\*.class util\*.class  *.class
copy CASCADESamples.jar %SAMPLEHOME%\win32

cd ..
del /q /s /f classes\*
rmdir /q /s classes

cd ..
