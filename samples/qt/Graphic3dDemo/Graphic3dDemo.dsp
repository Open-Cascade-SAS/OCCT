# Microsoft Developer Studio Project File - Name="Graphic3dDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Graphic3dDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Graphic3dDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Graphic3dDemo.mak" CFG="Graphic3dDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Graphic3dDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Graphic3dDemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "win32/bin"
# PROP Intermediate_Dir "win32/obj"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "inc" /I "$(CASROOT)\inc" /I "$(QTDIR)\include" /I "$(QTDIR)\include\Qt" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "WNT" /D "QT_DLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 TKernel.lib TKV3d.lib TKService.lib PTKernel.lib TKG3d.lib TKSTL.lib TKBrep.lib TKPrim.lib TKTopAlgo.lib TKG2d.lib TKMath.lib QtCore4.lib QtGui4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Graph.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /machine:I386 /libpath:"$(CASROOT)\win32\lib" /libpath:"$(QTDIR)\lib" /libpath:"win32\bin"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Graphic3dDemo___Win32_Debug"
# PROP BASE Intermediate_Dir "Graphic3dDemo___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "win32/bind"
# PROP Intermediate_Dir "win32/objd"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "inc" /I "$(CASROOT)\inc" /I "$(QTDIR)\include" /I "$(QTDIR)\include\Qt" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "WNT" /D "QT_DLL" /D "GRAPH_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 TKernel.lib TKV3d.lib TKService.lib PTKernel.lib TKG3d.lib TKSTL.lib TKBrep.lib TKPrim.lib TKTopAlgo.lib TKG2d.lib TKMath.lib QtCore4.lib QtGui4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Graph.lib /nologo /entry:"mainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"$(CASROOT)\win32\libd" /libpath:"$(QTDIR)\lib" /libpath:"win32\bind"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Graphic3dDemo - Win32 Release"
# Name "Graphic3dDemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\Application.cxx
# End Source File
# Begin Source File

SOURCE=.\src\AutoTestDlg.cxx
# End Source File
# Begin Source File

SOURCE=.\src\Document.cxx
# End Source File
# Begin Source File

SOURCE=.\src\Graphic3dDemo.cxx
# End Source File
# Begin Source File

SOURCE=.\src\MDIWindow.cxx
# End Source File
# Begin Source File

SOURCE=.\src\ObjectDlg.cxx
# End Source File
# Begin Source File

SOURCE=.\src\ShapeDlg.cxx
# End Source File
# Begin Source File

SOURCE=.\src\Sphere_BasicShape.cxx
# End Source File
# Begin Source File

SOURCE=.\src\Sphere_Sphere.cxx
# End Source File
# Begin Source File

SOURCE=.\src\Translate.cxx
# End Source File
# Begin Source File

SOURCE=.\src\View.cxx
# End Source File
# Begin Source File

SOURCE=.\src\ViewDlg.cxx
# End Source File
# Begin Source File

SOURCE=.\src\ViewOperations.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\inc\Application.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Application.h
InputName=Application

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Application.h
InputName=Application

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\AutoTestDlg.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\AutoTestDlg.h
InputName=AutoTestDlg

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\AutoTestDlg.h
InputName=AutoTestDlg

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\Document.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Document.h
InputName=Document

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Document.h
InputName=Document

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\global.h
# End Source File
# Begin Source File

SOURCE=.\inc\Handle_Sphere_BasicShape.hxx
# End Source File
# Begin Source File

SOURCE=.\inc\Handle_Sphere_Sphere.hxx
# End Source File
# Begin Source File

SOURCE=.\inc\MDIWindow.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\MDIWindow.h
InputName=MDIWindow

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\MDIWindow.h
InputName=MDIWindow

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\ObjectDlg.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\ObjectDlg.h
InputName=ObjectDlg

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\ObjectDlg.h
InputName=ObjectDlg

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\ShapeDlg.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\ShapeDlg.h
InputName=ShapeDlg

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\ShapeDlg.h
InputName=ShapeDlg

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\Sphere_BasicShape.hxx
# End Source File
# Begin Source File

SOURCE=.\inc\Sphere_BasicShape.ixx
# End Source File
# Begin Source File

SOURCE=.\inc\Sphere_BasicShape.jxx
# End Source File
# Begin Source File

SOURCE=.\inc\Sphere_Sphere.hxx
# End Source File
# Begin Source File

SOURCE=.\inc\Sphere_Sphere.ixx
# End Source File
# Begin Source File

SOURCE=.\inc\Sphere_Sphere.jxx
# End Source File
# Begin Source File

SOURCE=.\inc\Translate.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Translate.h
InputName=Translate

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\Translate.h
InputName=Translate

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\View.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\View.h
InputName=View

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\View.h
InputName=View

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\ViewDlg.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\ViewDlg.h
InputName=ViewDlg

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\ViewDlg.h
InputName=ViewDlg

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\inc\ViewOperations.h

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\ViewOperations.h
InputName=ViewOperations

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputName).h ...
InputDir=.\inc
InputPath=.\inc\ViewOperations.h
InputName=ViewOperations

".\win32\inc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputDir)\$(InputName).h -o .\win32\inc\moc_$(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Sample_icons.ts

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build
InputPath=.\res\Sample_icons.ts
InputName=Sample_icons

".\win32\res\$(InputName).qm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)/bin/lrelease $(InputPath) -qm .\win32\res\$(InputName).qm

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build
InputPath=.\res\Sample_icons.ts
InputName=Sample_icons

".\win32\res\$(InputName).qm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)/bin/lrelease $(InputPath) -qm .\win32\res\$(InputName).qm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\Sample_msg.ts

!IF  "$(CFG)" == "Graphic3dDemo - Win32 Release"

# Begin Custom Build
InputPath=.\res\Sample_msg.ts
InputName=Sample_msg

".\win32\res\$(InputName).qm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)/bin/lrelease $(InputPath) -qm .\win32\res\$(InputName).qm

# End Custom Build

!ELSEIF  "$(CFG)" == "Graphic3dDemo - Win32 Debug"

# Begin Custom Build
InputPath=.\res\Sample_msg.ts
InputName=Sample_msg

".\win32\res\$(InputName).qm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)/bin/lrelease $(InputPath) -qm .\win32\res\$(InputName).qm

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "moc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\win32\inc\moc_Application.cxx
# End Source File
# Begin Source File

SOURCE=.\win32\inc\moc_AutoTestDlg.cxx
# End Source File
# Begin Source File

SOURCE=.\win32\inc\moc_Document.cxx
# End Source File
# Begin Source File

SOURCE=.\win32\inc\moc_MDIWindow.cxx
# End Source File
# Begin Source File

SOURCE=.\win32\inc\moc_ObjectDlg.cxx
# End Source File
# Begin Source File

SOURCE=.\win32\inc\moc_ShapeDlg.cxx
# End Source File
# Begin Source File

SOURCE=.\win32\inc\moc_Translate.cxx
# End Source File
# Begin Source File

SOURCE=.\win32\inc\moc_View.cxx
# End Source File
# Begin Source File

SOURCE=.\win32\inc\moc_ViewDlg.cxx
# End Source File
# Begin Source File

SOURCE=.\win32\inc\moc_ViewOperations.cxx
# End Source File
# End Group
# End Target
# End Project
