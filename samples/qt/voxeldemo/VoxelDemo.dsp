# Microsoft Developer Studio Project File - Name="VoxelDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=VoxelDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "VoxelDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "VoxelDemo.mak" CFG="VoxelDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "VoxelDemo - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "VoxelDemo - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "VoxelDemo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\inc" /I "$(QTDIR)\include" /I "$(QTDIR)\include\QtCore" /I "$(QTDIR)\include\QtGui" /I "$(CASROOT)\..\wok\wok_entities\vx\inc" /I "$(CASROOT)\..\wok\wok_entities\vx\src\Voxel" /I "$(CASROOT)\inc" /I ".\res" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "WNT" /D "QT_THREAD_SUPPORT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 TKV3d.lib TKernel.lib TKBRep.lib TKG3d.lib TKTopAlgo.lib TKMath.lib TKService.lib TKPrim.lib QtCore4.lib QtGui4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"$(CASROOT)\..\wok\wok_entities\vx\wnt\lib" /libpath:"$(CASROOT)\win32\lib" /libpath:"$(QTDIR)\lib"

!ELSEIF  "$(CFG)" == "VoxelDemo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\inc" /I "$(CASROOT)\..\wok\wok_entities\vx\inc" /I "$(CASROOT)\..\wok\wok_entities\vx\src\Voxel" /I "$(CASROOT)\inc" /I "$(QTDIR)\include" /I "$(QTDIR)\include\QtCore" /I "$(QTDIR)\include\QtGui" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "WNT" /D "QT_THREAD_SUPPORT" /D "TEST" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 TKV3d.lib TKernel.lib TKBRep.lib TKG3d.lib TKTopAlgo.lib TKMath.lib TKService.lib TKPrim.lib QtCored4.lib QtGuid4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"$(CASROOT)\..\wok\wok_entities\vx\wnt\lib" /libpath:"$(CASROOT)\wnt\lib" /libpath:"$(QTDIR)\lib"

!ENDIF 

# Begin Target

# Name "VoxelDemo - Win32 Release"
# Name "VoxelDemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\Application.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConversionThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Viewer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\inc\Application.h

!IF  "$(CFG)" == "VoxelDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Application.h
InputName=Application

".\moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\moc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "VoxelDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Application.h
InputName=Application

"moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\moc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\ConversionThread.h
# End Source File
# Begin Source File

SOURCE=.\inc\Timer.h
# End Source File
# Begin Source File

SOURCE=.\inc\Viewer.h

!IF  "$(CFG)" == "VoxelDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Viewer.h
InputName=Viewer

".\moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\moc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "VoxelDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Viewer.h
InputName=Viewer

".\moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\moc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Moc files"

# PROP Default_Filter "cxx"
# Begin Source File

SOURCE=.\moc\moc_Application.cxx
# End Source File
# Begin Source File

SOURCE=.\moc\moc_Viewer.cxx
# End Source File
# End Group
# End Target
# End Project
