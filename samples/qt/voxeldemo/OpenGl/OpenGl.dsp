# Microsoft Developer Studio Project File - Name="OpenGl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OpenGl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OpenGl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OpenGl.mak" CFG="OpenGl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OpenGl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OpenGl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OpenGl - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENGL_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(CASROOT)\..\wok\wok_entities\vx\inc" /I "$(CASROOT)\inc" /I "$(CASROOT)\drv\OpenGl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENGL_EXPORTS" /D "WNT" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 opengl32.lib glu32.lib TKernel.lib TKMath.lib TKGeomBase.lib TKV3d.lib TKService.lib TKG3d.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"Release/VoxelOpenGl.dll" /libpath:"$(CASROOT)\..\wok\wok_entities\vx\wnt\lib" /libpath:"$(CASROOT)\win32\lib"

!ELSEIF  "$(CFG)" == "OpenGl - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENGL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(CASROOT)\..\wok\wok_entities\vx\inc" /I "$(CASROOT)\inc" /I "$(CASROOT)\drv\OpenGl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "OPENGL_EXPORTS" /D "WNT" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opengl32.lib glu32.lib TKernel.lib TKMath.lib TKGeomBase.lib TKV3d.lib TKService.lib TKG3d.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/VoxelOpenGl.dll" /pdbtype:sept /libpath:"$(CASROOT)\..\wok\wok_entities\vx\wnt\lib" /libpath:"$(CASROOT)\wnt\lib"

!ENDIF 

# Begin Target

# Name "OpenGl - Win32 Release"
# Name "OpenGl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_addnames.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_antialias.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_applid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_attri.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_AVIWriter.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_bintcol.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_bintrefl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_bsurfprop.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_charexpan.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_charspace.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_cmn_htbl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_cmn_memory.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_cmn_stg_tbl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_curve.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_degmodel.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_depthcue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_dind.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_dotexturemap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_edgecol.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_edgeflag.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_edgetyp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_edgewid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_execstruct.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_Extension.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_facecull.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_facedmode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_filters.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_funcs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_gl2ps.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_1.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_2.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_3.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_4.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_5.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_6.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_7.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_703.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_705.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_707.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_709.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_710.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_711.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_712.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_713.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_8.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_9.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_Export.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_Layer.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_GraphicDriver_print.cxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_highlight.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_hlind.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_ImageBox.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_indexpolygons.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_initelem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_inquire.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_intcol.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_intrefl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_intshademtd.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_intstyle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_intstyleind.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_label.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_LightBox.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_lightstate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_localtran3.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_mrkr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_mrkrcol.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_mrkrset.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_mrkrsize.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_mrkrtype.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_pick.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_pickid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_polygon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_polygonholes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_polygonoffset.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_polyl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_polylcol.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_polyltyp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_polylwid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_PrimitiveArray.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_qstrip.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_remnames.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_subrs.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_subrvis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_surfprop.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem_util.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_texmappedfont.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_text.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_textalignment.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_textcol.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_textcolsubtitle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_textdisplaytype.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_textfont.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_textheight.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_textstyle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_texture.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_TextureBox.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_util.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_utilgr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tmesh.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_activateview.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_antialiasing.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_backfacing.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_background.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_begin.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_begin_animation.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_begin_immediat_mode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_begin_layer_mode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_bezier.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_bezier_weight.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_blink.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_boundarybox.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_cleargroup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_clearstructure.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_cliplimit.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_closegroup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_connect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_contextstructure.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_curve.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_deactivateview.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_degeneratestructure.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_depthcueing.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_depthtest.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_disconnect.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_displaystructure.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_element_exploration.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_end.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_erasestructure.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_facecontextgroup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_group.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_highlightcolor.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_init_pick.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_inquirefacilities.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_inquirelight.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_inquireplane.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_inquireview.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_light.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_light_exploration.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_linecontextgroup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_marker.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_marker_set.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_markercontextgroup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_namesetstructure.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_opengroup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_parray.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_pick.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_pickid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_plane.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_polygon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_polygon_holes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_polygon_indices.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_polygon_set.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_polyline.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_print.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_project_raster.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_quadrangle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_ratio_window.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_redraw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_removegroup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_removestructure.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_removeview.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_set_environment.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_setlight.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_setplane.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_setvisualisation.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_structure.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_structure_exploration.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_text.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_textcontextgroup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_texture.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_transformstructure.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_transparency.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_triangle.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_triedron.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_unproject_raster.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_update.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_userdraw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_view.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_view_exploration.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_viewmapping.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_togl_vieworientation.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tox.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_transform_persistence.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_triedron.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tsm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tXfm.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_txgl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_undefined.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_userdraw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_view.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_vind.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_ws.c
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wok\wok_entities\vx\src\VoxelClient\VoxelClient_VisDrawer.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_animation.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_AVIWriter.hxx
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_callback.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_cmn_htbl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_cmn_memory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_cmn_stg_tbl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_cmn_varargs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_context.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_degeneration.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_Extension.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_gl2ps.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_ImageBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_inquire.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_LightBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem_attri.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem_depthcue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem_filters.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem_highlight.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem_inquire.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem_pick.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem_util.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_telem_view.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_TextureBox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_all.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_elems.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_funcs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_pick.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_subrs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_subrvis.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_tox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_util.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_utilgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tgl_vis.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_traces.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_transform_persistence.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_triangulate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_triedron.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_trsf_stack.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tsm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tsm_ws.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_tXfm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\ros\src\OpenGl\OpenGl_txgl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wok\wok_entities\vx\src\Voxel\Voxel_VisData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wok\wok_entities\vx\src\VoxelClient\VoxelClient_VisDrawer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
