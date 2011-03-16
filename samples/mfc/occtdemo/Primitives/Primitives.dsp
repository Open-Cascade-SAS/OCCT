# Microsoft Developer Studio Project File - Name="Primitives" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Primitives - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Primitives.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Primitives.mak" CFG="Primitives - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Primitives - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Primitives - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Primitives - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../Release"
# PROP Intermediate_Dir "../Release/obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "../" /I "../Common" /I "../Common/WNT" /I "../Common/ISession" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "WNT" /D "_AFXDLL" /YX"StdAfx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 FWOSPlugin.lib PTKernel.lib TKBool.lib TKCAF.lib TKCDF.lib TKDraw.lib TKernel.lib TKFeat.lib TKFillet.lib TKGeomAlgo.lib TKHLR.lib TKMath.lib TKOffset.lib TKPCAF.lib TKPrim.lib TKPShape.lib TKService.lib TKTopAlgo.lib TKV2d.lib TKV3d.lib TKBRep.lib TKG2d.lib TKG3d.lib TKGeomBase.lib TKBO.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Primitives - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../Debug"
# PROP Intermediate_Dir "../Debug/objd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "../" /I "../Common" /I "../Common/WNT" /I "../Common/ISession" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "WNT" /D "_AFXDLL" /YX"StdAfx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 FWOSPlugin.lib PTKernel.lib TKBool.lib TKCAF.lib TKCDF.lib TKDraw.lib TKernel.lib TKFeat.lib TKFillet.lib TKGeomAlgo.lib TKHLR.lib TKMath.lib TKOffset.lib TKPCAF.lib TKPrim.lib TKPShape.lib TKService.lib TKTopAlgo.lib TKV2d.lib TKV3d.lib TKBRep.lib TKG2d.lib TKG3d.lib TKGeomBase.lib TKBO.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Primitives - Win32 Release"
# Name "Primitives - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Primitives_Presentation.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Primitives_Presentation.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Group "Source Files No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\ISession\ISession_Curve.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\OCCDemo.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\OCCDemo.rc
# End Source File
# Begin Source File

SOURCE=..\Common\OCCDemo_Presentation.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\OCCDemoDoc.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\OCCDemoView.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\ResultDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\ISession\ISession_Curve.h
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\OCCDemo.h
# End Source File
# Begin Source File

SOURCE=..\Common\OCCDemo_Presentation.h
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\OCCDemoDoc.h
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\OCCDemoView.h
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\resource.h
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\ResultDialog.h
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\WNT\res\OCCDemo.ico
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\res\OCCDemo.rc2
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\Common\WNT\res\toolbar1.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=..\env.bat
# End Source File
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# Begin Source File

SOURCE=..\run.bat
# End Source File
# Begin Source File

SOURCE=..\vc.bat
# End Source File
# End Group
# End Target
# End Project
