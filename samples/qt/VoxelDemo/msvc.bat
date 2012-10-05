set CASROOT=D:\OCC653\ros
set QTDIR=D:\qt\4.8.2
set PATH=%QTDIR%\bin;%PATH%

call %CASROOT%/env.bat %1 %2 %3
call %CASROOT%/msvc.bat %1 %2 %3 VoxelDemo.sln
