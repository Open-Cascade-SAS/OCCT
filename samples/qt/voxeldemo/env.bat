@ECHO OFF

call %~dp0..\..\..\ros\env.bat

SET BIN_DIR=Release
SET CSF_GraphicShr=OpenGl\Release\VoxelOpenGl.dll

SET PATH=OpenGl\Release;%QTDIR%\bin;%QTDIR%;%PATH%

SET CSF_TEX_FONT=2
SET MMGT_REENTRANT=1
