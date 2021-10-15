#!/usr/bin/tclsh

# =======================================================================
# Created on: 2012-01-26
# Created by: Kirill GAVRILOV
# Copyright (c) 2012 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

# =======================================================================
# GUI procedure for search of third-party tools and generation of environment
# customization script
# =======================================================================

# load tools
source [file join [file dirname [info script]] genconfdeps.tcl]

# proxy variable for implicit file path normalization
set PRODUCTS_PATH_INPUT "$::PRODUCTS_PATH"

package require Tk

set aRowIter 0
set aCheckRowIter 0
frame .myFrame -padx 5 -pady 5
pack  .myFrame -fill both -expand 1
frame .myFrame.myPrjFrame
frame .myFrame.myVsFrame
frame .myFrame.myHxxChecks
frame .myFrame.myChecks

# project file format
set SYS_PRJFMT_LIST {}
set SYS_PRJNAME_LIST {}
if { "$::tcl_platform(platform)" == "windows" } {
  lappend ::SYS_PRJFMT_LIST "vcxproj"
  lappend ::SYS_PRJNAME_LIST "Visual Studio (.vcxproj)"
}
if { "$tcl_platform(os)" == "Darwin" } {
  lappend ::SYS_PRJFMT_LIST "xcd"
  lappend ::SYS_PRJNAME_LIST "XCode (.xcd)"
}
lappend ::SYS_PRJFMT_LIST "cbp"
lappend ::SYS_PRJNAME_LIST "Code Blocks (.cbp)"
lappend ::SYS_PRJFMT_LIST "pro"
lappend ::SYS_PRJNAME_LIST "Qt Creator (.pro)"

set aPrjIndex [lsearch $::SYS_PRJFMT_LIST $::PRJFMT]
set ::PRJNAME [lindex $::SYS_PRJNAME_LIST $aPrjIndex]
set ::CONFIG "Release"

set SYS_VS_LIST {}
set SYS_VC_LIST {}
set SYS_VCVARS_LIST {}

# detect installed Visual Studio 2017+ instances by running vswhere.exe
if { ! [catch {exec vswhere.exe -version "\[15.0,15.99\]" -latest -requires Microsoft.VisualStudio.Workload.NativeDesktop -property installationPath} res] && "$res" != "" } {
  lappend ::SYS_VS_LIST "Visual Studio 2017 (15, toolset v141)"
  lappend ::SYS_VC_LIST "vc141"
  lappend ::SYS_VCVARS_LIST "$res\\VC\\Auxiliary\\Build\\vcvarsall.bat"
}
if { ! [catch {exec vswhere.exe -version "\[15.0,15.99\]" -latest -requires Microsoft.VisualStudio.Workload.Universal -property installationPath} res] && "$res" != "" } {
  lappend ::SYS_VS_LIST "Visual Studio 2017 (15, toolset v141) UWP"
  lappend ::SYS_VC_LIST "vc141-uwp"
  lappend ::SYS_VCVARS_LIST "$res\\VC\\Auxiliary\\Build\\vcvarsall.bat"
}
if { ! [catch {exec vswhere.exe -version "\[16.0,16.99\]" -latest -requires Microsoft.VisualStudio.Workload.NativeDesktop -property installationPath} res] && "$res" != "" } {
  lappend ::SYS_VS_LIST "Visual Studio 2019 (16, toolset v142)"
  lappend ::SYS_VC_LIST "vc142"
  lappend ::SYS_VCVARS_LIST "$res\\VC\\Auxiliary\\Build\\vcvarsall.bat"
}
if { ! [catch {exec vswhere.exe -version "\[16.0,16.99\]" -latest -requires Microsoft.VisualStudio.Workload.Universal -property installationPath} res] && "$res" != "" } {
  lappend ::SYS_VS_LIST "Visual Studio 2019 (16, toolset v142) UWP"
  lappend ::SYS_VC_LIST "vc142-uwp"
  lappend ::SYS_VCVARS_LIST "$res\\VC\\Auxiliary\\Build\\vcvarsall.bat"
}
if { ! [catch {exec vswhere.exe -version "\[16.0,16.99\]" -latest -requires Microsoft.VisualStudio.Component.VC.ClangCL -property installationPath} res] && "$res" != "" } {
  lappend ::SYS_VS_LIST "Visual Studio 2019 (16, toolset ClangCL)"
  lappend ::SYS_VC_LIST "vclang"
  lappend ::SYS_VCVARS_LIST "$res\\VC\Auxiliary\\Build\\vcvarsall.bat"
}
if { ! [catch {exec vswhere.exe -version "\[17.0,17.99\]" -latest -requires Microsoft.VisualStudio.Workload.NativeDesktop -property installationPath} res] && "$res" != "" } {
  lappend ::SYS_VS_LIST "Visual Studio 2022 (17, toolset v143)"
  lappend ::SYS_VC_LIST "vc143"
  lappend ::SYS_VCVARS_LIST "$res\\VC\\Auxiliary\\Build\\vcvarsall.bat"
}
if { ! [catch {exec vswhere.exe -version "\[17.0,17.99\]" -latest -requires Microsoft.VisualStudio.Workload.Universal -property installationPath} res] && "$res" != "" } {
  lappend ::SYS_VS_LIST "Visual Studio 2022 (17, toolset v143) UWP"
  lappend ::SYS_VC_LIST "vc143-uwp"
  lappend ::SYS_VCVARS_LIST "$res\\VC\\Auxiliary\\Build\\vcvarsall.bat"
}
if { ! [catch {exec vswhere.exe -version "\[17.0,17.99\]" -latest -requires Microsoft.VisualStudio.Component.VC.ClangCL -property installationPath} res] && "$res" != "" } {
  lappend ::SYS_VS_LIST "Visual Studio 2022 (17, toolset ClangCL)"
  lappend ::SYS_VC_LIST "vclang"
  lappend ::SYS_VCVARS_LIST "$res\\VC\\Auxiliary\\Build\\vcvarsall.bat"
}

# detect installed Visual Studio instances from global environment
if { [info exists ::env(VS140COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2015 (14, toolset v140)"
  lappend ::SYS_VC_LIST "vc14"
  lappend ::SYS_VCVARS_LIST "%VS140COMNTOOLS%..\\..\\VC\\vcvarsall.bat"

  lappend ::SYS_VS_LIST "Visual Studio 2015 (14, toolset v140) UWP"
  lappend ::SYS_VC_LIST "vc14-uwp"
  lappend ::SYS_VCVARS_LIST "%VS140COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS120COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2013 (12, toolset v120)"
  lappend ::SYS_VC_LIST "vc12"
  lappend ::SYS_VCVARS_LIST "%VS120COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS110COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2012 (11, toolset v110)"
  lappend ::SYS_VC_LIST "vc11"
  lappend ::SYS_VCVARS_LIST "%VS110COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS100COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2010 (10, toolset v100)"
  lappend ::SYS_VC_LIST "vc10"
  lappend ::SYS_VCVARS_LIST "%VS100COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS90COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2008 (9, toolset v90)"
  lappend ::SYS_VC_LIST "vc9"
  lappend ::SYS_VCVARS_LIST "%VS90COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS80COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2005 (8, toolset v80)"
  lappend ::SYS_VC_LIST "vc8"
  lappend ::SYS_VCVARS_LIST "%VS80COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
lappend ::SYS_VS_LIST "Custom"
lappend ::SYS_VC_LIST "vcX"
lappend ::SYS_VCVARS_LIST "%VSXXCOMNTOOLS%..\\..\\VC\\vcvarsall.bat"

set aVcVerIndex [lsearch $::SYS_VC_LIST $::VCVER]
set ::VSVER  [lindex $::SYS_VS_LIST     $aVcVerIndex]
if { "$::VCVARS" == "" } {
  set ::VCVARS [lindex $::SYS_VCVARS_LIST $aVcVerIndex]
}

proc wokdep:gui:Close {} {
  # if changed ask
  exit
}

proc wokdep:gui:configSuffix {} {
  if { "$::CONFIG" == "Debug" } {
    return "D"
  }
  return ""
}

proc wokdep:gui:SwitchConfig {} {
  set ::PRJFMT [lindex $::SYS_PRJFMT_LIST [.myFrame.myPrjFrame.myPrjCombo current]]
  set ::VCVER  [lindex $::SYS_VC_LIST     [.myFrame.myVsFrame.myVsCombo current]]
  set ::VCVARS [lindex $::SYS_VCVARS_LIST [.myFrame.myVsFrame.myVsCombo current]]

  set ::CSF_OPT_INC {}
  set ::CSF_OPT_LIB32 {}
  set ::CSF_OPT_LIB64 {}
  set ::CSF_OPT_BIN32 {}
  set ::CSF_OPT_BIN64 {}
  set ::CSF_OPT_LIB32D {}
  set ::CSF_OPT_LIB64D {}
  set ::CSF_OPT_BIN32D {}
  set ::CSF_OPT_BIN64D {}
  wokdep:gui:UpdateList
}

proc wokdep:gui:SwitchArch {} {
  wokdep:gui:Show3264Bitness ::aRowIter

  if { [llength [grid info .myFrame.mySave]] != 0 } {
    grid forget .myFrame.mySave .myFrame.myClose
  }

  # Bottom section
  grid .myFrame.mySave  -row $::aRowIter -column 4 -columnspan 2
  grid .myFrame.myClose -row $::aRowIter -column 6 -columnspan 2
}

# update label text and visibility
font create wokdep:gui:EmptyFont -size -1
proc wokdep:gui:SetLabelText {theLabel theText} {
  set aFont TkDefaultFont
  if { $theText == "" } {
    set aFont wokdep:gui:EmptyFont
  }
  $theLabel configure -text $theText -font $aFont
}

proc wokdep:gui:UpdateList {} {
  set anIncErrs   {}
  set anLib32Errs {}
  set anLib64Errs {}
  set anBin32Errs {}
  set anBin64Errs {}
  wokdep:SearchTclTk     anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  if { "$::HAVE_FREETYPE" == "true" } {
    wokdep:SearchFreeType anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  wokdep:SearchX11       anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  if { "$::HAVE_GLES2" == "true" } {
    wokdep:SearchEGL     anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
    wokdep:SearchGLES    anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::HAVE_FREEIMAGE" == "true" } {
    wokdep:SearchFreeImage anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::HAVE_FFMPEG" == "true" } {
    wokdep:SearchFFmpeg  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::HAVE_OPENVR" == "true" } {
    wokdep:SearchOpenVR  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::HAVE_TBB" == "true" } {
    wokdep:SearchTBB     anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
#  if { "$::HAVE_OPENCL" == "true" } {
#    wokdep:SearchOpenCL  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
#  }
  if { "$::HAVE_VTK" == "true" } {
    wokdep:SearchVTK  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }

  if { "$::HAVE_ZLIB" == "true" } {
    set aCheckLib "z"
    if { "$::tcl_platform(platform)" == "windows" } {
      set aCheckLib "zlib"
    }
    wokdep:SearchStandardLibrary  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs "zlib" "zlib.h" "$aCheckLib" {"zlib"}
  }
  if { "$::HAVE_LIBLZMA" == "true" } {
    set aCheckLib "lzma"
    if { "$::tcl_platform(platform)" == "windows" } {
      set aCheckLib "liblzma"
    }
    wokdep:SearchStandardLibrary  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs "liblzma" "lzma.h" "$aCheckLib" {"lzma" "xz"}
  }
  if { "$::HAVE_E57" == "true" } {
    wokdep:SearchStandardLibrary  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs "e57" "e57/E57Foundation.h" "E57RefImpl" {"e57"}
    set aCheckLib "xerces-c"
    if { "$::tcl_platform(platform)" == "windows" } {
      set aCheckLib "xerces-c_3"
    }
    wokdep:SearchStandardLibrary  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs "xerces-c" "xercesc/sax2/XMLReaderFactory.hpp" "$aCheckLib" {"xerces"}
  }
  if { "$::HAVE_RAPIDJSON" == "true" } {
    wokdep:SearchRapidJson anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::HAVE_DRACO" == "true" } {
    set aDummy {}
    wokdep:SearchStandardLibrary  anIncErrs anLib32Errs anLib64Errs aDummy aDummy "draco" "draco/compression/decode.h" "draco" {"draco"}
  }

  if {"$::BUILD_Inspector" == "true" } {
    set ::CHECK_QT "true"
  }

  if { "$::CHECK_QT" == "true" } {
    wokdep:SearchQt     anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }

  if { "$::CHECK_JDK" == "true" } {
    wokdep:SearchJDK     anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }

  wokdep:gui:SetLabelText .myFrame.myIncErrLbl [join $anIncErrs   "\n"]

  wokdep:gui:SetLabelText .myFrame.myIncErrLbl     [join $anIncErrs   "\n"]
  wokdep:gui:SetLabelText .myFrame.myLib32_ErrLbl  [join $anLib32Errs "\n"]
  wokdep:gui:SetLabelText .myFrame.myLib64_ErrLbl  [join $anLib64Errs "\n"]
  wokdep:gui:SetLabelText .myFrame.myBin32_ErrLbl  [join $anBin32Errs "\n"]
  wokdep:gui:SetLabelText .myFrame.myBin64_ErrLbl  [join $anBin64Errs "\n"]

  wokdep:gui:SetLabelText .myFrame.myLib32D_ErrLbl [join $anLib32Errs "\n"]
  wokdep:gui:SetLabelText .myFrame.myLib64D_ErrLbl [join $anLib64Errs "\n"]
  wokdep:gui:SetLabelText .myFrame.myBin32D_ErrLbl [join $anBin32Errs "\n"]
  wokdep:gui:SetLabelText .myFrame.myBin64D_ErrLbl [join $anBin64Errs "\n"]

  # merge duplicates
  set ::CSF_OPT_LIB32  [lsort -unique $::CSF_OPT_LIB32]
  set ::CSF_OPT_LIB64  [lsort -unique $::CSF_OPT_LIB64]
  set ::CSF_OPT_BIN32  [lsort -unique $::CSF_OPT_BIN32]
  set ::CSF_OPT_BIN64  [lsort -unique $::CSF_OPT_BIN64]
  set ::CSF_OPT_LIB32D [lsort -unique $::CSF_OPT_LIB32D]
  set ::CSF_OPT_LIB64D [lsort -unique $::CSF_OPT_LIB64D]
  set ::CSF_OPT_BIN32D [lsort -unique $::CSF_OPT_BIN32D]
  set ::CSF_OPT_BIN64D [lsort -unique $::CSF_OPT_BIN64D]
}

proc wokdep:gui:BrowseVcVars {} {
  set aResult [tk_chooseDirectory -initialdir $::VCVARS -title "Choose a directory"]
  if { "$aResult" != "" } {
    set ::VCVARS $aResult
  }
}

proc wokdep:gui:BrowsePartiesRoot {} {
  set aResult [tk_chooseDirectory -initialdir $::PRODUCTS_PATH_INPUT -title "Choose a directory"]
  if { "$aResult" != "" } {
    set ::PRODUCTS_PATH_INPUT $aResult
    wokdep:gui:UpdateList
  }
}

proc wokdep:gui:AddIncPath {} {
  set aResult [tk_chooseDirectory -title "Choose a directory"]
  if { "$aResult" != "" } {
    lappend ::CSF_OPT_INC "$aResult"
    wokdep:gui:UpdateList
  }
}

proc wokdep:gui:AddLibPath {} {
  set aCfg [wokdep:gui:configSuffix]
  set aResult [tk_chooseDirectory -title "Choose a directory"]
  if { "$aResult" != "" } {
    lappend ::CSF_OPT_LIB${::ARCH}${aCfg} "$aResult"
    wokdep:gui:UpdateList
  }
}

proc wokdep:gui:AddBinPath {} {
  set aCfg [wokdep:gui:configSuffix]
  set aResult [tk_chooseDirectory -title "Choose a directory"]
  if { "$aResult" != "" } {
    lappend ::CSF_OPT_BIN${::ARCH}${aCfg} "$aResult"
    wokdep:gui:UpdateList
  }
}

proc wokdep:gui:RemoveIncPath {} {
  set aSelIndices [.myFrame.myIncList curselection]
  if { [llength $aSelIndices] != 0 } {
    .myFrame.myIncList delete [lindex $aSelIndices 0]
  }
  wokdep:gui:UpdateList
}

proc wokdep:gui:RemoveLibPath {} {
  set aCfg [wokdep:gui:configSuffix]
  set aSelIndices [.myFrame.myLib${::ARCH}${aCfg}_List curselection]
  if { [llength $aSelIndices] != 0 } {
    .myFrame.myLib${::ARCH}${aCfg}_List delete [lindex $aSelIndices 0]
  }
  wokdep:gui:UpdateList
}

proc wokdep:gui:RemoveBinPath {} {
  set aCfg [wokdep:gui:configSuffix]
  set aSelIndices [.myFrame.myBin${::ARCH}${aCfg}_List curselection]
  if { [llength $aSelIndices] != 0 } {
    .myFrame.myBin${::ARCH}${aCfg}_List delete [lindex $aSelIndices 0]
  }
  wokdep:gui:UpdateList
}

proc wokdep:gui:ResetIncPath {} {
  set ::CSF_OPT_INC {}
  wokdep:gui:UpdateList
}

proc wokdep:gui:ResetLibPath {} {
  set ::CSF_OPT_LIB${::ARCH}  {}
  set ::CSF_OPT_LIB${::ARCH}D {}
  set ::CSF_OPT_BIN${::ARCH}  {}
  set ::CSF_OPT_BIN${::ARCH}D {}
  wokdep:gui:UpdateList
}

proc wokdep:gui:Show3264Bitness { theRowIter } {
  upvar $theRowIter aRowIter

  set aArchOld ""
  set aCfg [wokdep:gui:configSuffix]
  if { "$::ARCH" == "32" } {
    set aArchOld "64"
  } else {
    set aArchOld "32"
  }

  set aCfgOld "D"
  if { "$::CONFIG" == "Debug" } { set aCfgOld "" }
  set aDelArch ${aArchOld}${aCfg}
  if { [llength [grid info .myFrame.myLib${aDelArch}_Lbl]] != 0 } {
    grid forget .myFrame.myLib${aDelArch}_Lbl .myFrame.myLib${aDelArch}_List   .myFrame.myLib${aDelArch}_Scrl
    grid forget .myFrame.myLib${aDelArch}_Add .myFrame.myLib${aDelArch}_Remove .myFrame.myLib${aDelArch}_Clear .myFrame.myLib${aDelArch}_ErrLbl
    grid forget .myFrame.myBin${aDelArch}_Lbl .myFrame.myBin${aDelArch}_List   .myFrame.myBin${aDelArch}_Scrl
    grid forget .myFrame.myBin${aDelArch}_Add .myFrame.myBin${aDelArch}_Remove .myFrame.myBin${aDelArch}_Clear .myFrame.myBin${aDelArch}_ErrLbl
  }
  set aDelCfg ${::ARCH}${aCfgOld}
  if { [llength [grid info .myFrame.myLib${aDelCfg}_Lbl]] != 0 } {
    grid forget .myFrame.myLib${aDelCfg}_Lbl .myFrame.myLib${aDelCfg}_List   .myFrame.myLib${aDelCfg}_Scrl
    grid forget .myFrame.myLib${aDelCfg}_Add .myFrame.myLib${aDelCfg}_Remove .myFrame.myLib${aDelCfg}_Clear .myFrame.myLib${aDelCfg}_ErrLbl
    grid forget .myFrame.myBin${aDelCfg}_Lbl .myFrame.myBin${aDelCfg}_List   .myFrame.myBin${aDelCfg}_Scrl
    grid forget .myFrame.myBin${aDelCfg}_Add .myFrame.myBin${aDelCfg}_Remove .myFrame.myBin${aDelCfg}_Clear .myFrame.myBin${aDelCfg}_ErrLbl
  }

  set aNewCfg ${::ARCH}${aCfg}
  # Additional libraries search paths
  grid .myFrame.myLib${aNewCfg}_Lbl    -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
  grid .myFrame.myLib${aNewCfg}_List   -row $aRowIter -column 0 -rowspan 4 -columnspan 5
  grid .myFrame.myLib${aNewCfg}_Scrl   -row $aRowIter -column 5 -rowspan 4
  grid .myFrame.myLib${aNewCfg}_Add    -row $aRowIter -column 6
  incr aRowIter
  #grid .myFrame.myLib${aNewCfg}_Edit   -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myLib${aNewCfg}_Remove -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myLib${aNewCfg}_Clear  -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myLib${aNewCfg}_ErrLbl -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter

  # Additional executables search paths
  grid .myFrame.myBin${aNewCfg}_Lbl    -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
  grid .myFrame.myBin${aNewCfg}_List   -row $aRowIter -column 0 -rowspan 4 -columnspan 5
  grid .myFrame.myBin${aNewCfg}_Scrl   -row $aRowIter -column 5 -rowspan 4
  grid .myFrame.myBin${aNewCfg}_Add    -row $aRowIter -column 6
  incr aRowIter
  #grid .myFrame.myBin${aNewCfg}_Edit   -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myBin${aNewCfg}_Remove -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myBin${aNewCfg}_Clear  -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myBin${aNewCfg}_ErrLbl -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
}

# Header
ttk::label    .myFrame.myPrjFrame.myPrjLbl     -text "Project format:" -padding {5 5 20 5}
ttk::combobox .myFrame.myPrjFrame.myPrjCombo   -values $SYS_PRJNAME_LIST -state readonly -textvariable PRJNAME -width 40
ttk::label    .myFrame.myVsFrame.myVsLbl       -text "Visual Studio configuration:" -padding {5 5 20 5}
ttk::combobox .myFrame.myVsFrame.myVsCombo     -values $SYS_VS_LIST -state readonly -textvariable VSVER -width 40
ttk::combobox .myFrame.myVsFrame.myArchCombo   -values { {32} {64} } -textvariable ARCH -state readonly -width 6
ttk::combobox .myFrame.myVsFrame.myConfigCombo -values { {Release} {Debug} } -textvariable CONFIG -state readonly -width 6
entry         .myFrame.myVcEntry     -textvariable VCVER  -width 10
entry         .myFrame.myVcVarsEntry -textvariable VCVARS -width 70
ttk::button   .myFrame.myVcBrowseBtn -text "Browse" -command wokdep:gui:BrowseVcVars
ttk::label    .myFrame.myHxxChecks.myRelDebInfoLbl   -text "Release with Debug info"
checkbutton   .myFrame.myHxxChecks.myRelDebInfoCheck -offvalue "false" -onvalue "true" -variable HAVE_RelWithDebInfo

#
ttk::combobox .myFrame.myHxxChecks.myScutsCombo   -values { {ShortCut} {Copy} {HardLink} } -textvariable SHORTCUT_HEADERS -state readonly -width 12
ttk::label    .myFrame.myHxxChecks.myScutsLbl     -text "Strategy for filling headers folder inc:"

#
ttk::label    .myFrame.mySrchLbl       -text "3rd-parties search path:" -padding {5 5 80 5}
entry         .myFrame.mySrchEntry     -textvariable PRODUCTS_PATH_INPUT -width 80
ttk::button   .myFrame.mySrchBrowseBtn -text "Browse" -command wokdep:gui:BrowsePartiesRoot
checkbutton   .myFrame.myChecks.myFreeTypeCheck -offvalue "false" -onvalue "true" -variable HAVE_FREETYPE  -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myFreeTypeLbl   -text "Use FreeType"
checkbutton   .myFrame.myChecks.myFImageCheck   -offvalue "false" -onvalue "true" -variable HAVE_FREEIMAGE -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myFImageLbl     -text "Use FreeImage"
checkbutton   .myFrame.myChecks.myTbbCheck      -offvalue "false" -onvalue "true" -variable HAVE_TBB       -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myTbbLbl        -text "Use Intel TBB"
checkbutton   .myFrame.myChecks.myOpenVrCheck   -offvalue "false" -onvalue "true" -variable HAVE_OPENVR    -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myOpenVrLbl     -text "Use OpenVR"
if { "$::tcl_platform(os)" != "Darwin" } {
  checkbutton .myFrame.myChecks.myGlesCheck     -offvalue "false" -onvalue "true" -variable HAVE_GLES2     -command wokdep:gui:UpdateList
  ttk::label  .myFrame.myChecks.myGlesLbl       -text "Use OpenGL ES"
}
if { "$::tcl_platform(platform)" == "windows" } {
  checkbutton .myFrame.myChecks.myD3dCheck      -offvalue "false" -onvalue "true" -variable HAVE_D3D       -command wokdep:gui:UpdateList
  ttk::label  .myFrame.myChecks.myD3dLbl        -text "Use Direct3D"
}
checkbutton   .myFrame.myChecks.myFFmpegCheck   -offvalue "false" -onvalue "true" -variable HAVE_FFMPEG    -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myFFmpegLbl     -text "Use FFmpeg"
#checkbutton   .myFrame.myChecks.myOpenClCheck   -offvalue "false" -onvalue "true" -variable HAVE_OPENCL    -command wokdep:gui:UpdateList
#ttk::label    .myFrame.myChecks.myOpenClLbl     -text "Use OpenCL"
checkbutton   .myFrame.myChecks.myRapidJsonCheck -offvalue "false" -onvalue "true" -variable HAVE_RAPIDJSON -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myRapidJsonLbl   -text "Use RapidJSON"
checkbutton   .myFrame.myChecks.myDracoCheck    -offvalue "false" -onvalue "true" -variable HAVE_DRACO     -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myDracoLbl      -text "Use Draco"

checkbutton   .myFrame.myChecks.myXLibCheck     -offvalue "false" -onvalue "true" -variable HAVE_XLIB
ttk::label    .myFrame.myChecks.myXLibLbl       -text "Use X11 for windows drawing"
ttk::label    .myFrame.myChecks.myVtkLbl        -text "Use VTK"
checkbutton   .myFrame.myChecks.myVtkCheck      -offvalue "false" -onvalue "true" -variable HAVE_VTK       -command wokdep:gui:UpdateList

checkbutton   .myFrame.myChecks.myZLibCheck     -offvalue "false" -onvalue "true" -variable HAVE_ZLIB      -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myZLibLbl       -text "Use zlib"
checkbutton   .myFrame.myChecks.myLzmaCheck     -offvalue "false" -onvalue "true" -variable HAVE_LIBLZMA   -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myLzmaLbl       -text "Use liblzma"
checkbutton   .myFrame.myChecks.myE57Check      -offvalue "false" -onvalue "true" -variable HAVE_E57       -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myE57Lbl        -text "Use E57"

checkbutton   .myFrame.myChecks.myQtCheck       -offvalue "false" -onvalue "true" -variable CHECK_QT       -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myQtLbl         -text "Search Qt"
checkbutton   .myFrame.myChecks.myJDKCheck      -offvalue "false" -onvalue "true" -variable CHECK_JDK      -command wokdep:gui:UpdateList
ttk::label    .myFrame.myChecks.myJDKLbl        -text "Search JDK"

if { "$::tcl_platform(platform)" == "windows" } {
  checkbutton   .myFrame.myChecks.myInspectorBuild -offvalue "false" -onvalue "true" -variable BUILD_Inspector      -command wokdep:gui:UpdateList
  ttk::label    .myFrame.myChecks.myInspectorLbl   -text "Build Inspector"
}

# Additional headers search paths
ttk::label    .myFrame.myIncLbl    -text "Additional headers search paths:" -padding {5 5 80 5}
scrollbar     .myFrame.myIncScrl   -command ".myFrame.myIncList yview"
listbox       .myFrame.myIncList   -listvariable CSF_OPT_INC -width 80 -height 5 -yscrollcommand ".myFrame.myIncScrl set"
ttk::button   .myFrame.myIncAdd    -text "Add"     -command wokdep:gui:AddIncPath
ttk::button   .myFrame.myIncEdit   -text "Edit"
ttk::button   .myFrame.myIncRemove -text "Remove"  -command wokdep:gui:RemoveIncPath
ttk::button   .myFrame.myIncClear  -text "Reset"   -command wokdep:gui:ResetIncPath
ttk::label    .myFrame.myIncErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional libraries (32-bit) search paths
ttk::label    .myFrame.myLib32_Lbl    -text "Additional libraries (32-bit) search paths:" -padding {5 5 80 5}
scrollbar     .myFrame.myLib32_Scrl   -command ".myFrame.myLib32_List yview"
listbox       .myFrame.myLib32_List   -listvariable CSF_OPT_LIB32 -width 80 -height 5 -yscrollcommand ".myFrame.myLib32_Scrl set"
ttk::button   .myFrame.myLib32_Add    -text "Add"     -command wokdep:gui:AddLibPath
ttk::button   .myFrame.myLib32_Edit   -text "Edit"
ttk::button   .myFrame.myLib32_Remove -text "Remove"  -command wokdep:gui:RemoveLibPath
ttk::button   .myFrame.myLib32_Clear  -text "Reset"   -command wokdep:gui:ResetLibPath
ttk::label    .myFrame.myLib32_ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional debug libraries (32-bit) search paths
ttk::label    .myFrame.myLib32D_Lbl    -text "Additional debug libraries (32-bit) search paths:" -padding {5 5 80 5}
scrollbar     .myFrame.myLib32D_Scrl   -command ".myFrame.myLib32D_List yview"
listbox       .myFrame.myLib32D_List   -listvariable CSF_OPT_LIB32D -width 80 -height 5 -yscrollcommand ".myFrame.myLib32D_Scrl set"
ttk::button   .myFrame.myLib32D_Add    -text "Add"     -command wokdep:gui:AddLibPath
ttk::button   .myFrame.myLib32D_Edit   -text "Edit"
ttk::button   .myFrame.myLib32D_Remove -text "Remove"  -command wokdep:gui:RemoveLibPath
ttk::button   .myFrame.myLib32D_Clear  -text "Reset"   -command wokdep:gui:ResetLibPath
ttk::label    .myFrame.myLib32D_ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional libraries (64-bit) search paths
ttk::label    .myFrame.myLib64_Lbl    -text "Additional libraries (64-bit) search paths:" -padding {5 5 80 5}
scrollbar     .myFrame.myLib64_Scrl   -command ".myFrame.myLib64_List yview"
listbox       .myFrame.myLib64_List   -listvariable CSF_OPT_LIB64 -width 80 -height 5 -yscrollcommand ".myFrame.myLib64_Scrl set"
ttk::button   .myFrame.myLib64_Add    -text "Add"     -command wokdep:gui:AddLibPath
ttk::button   .myFrame.myLib64_Edit   -text "Edit"
ttk::button   .myFrame.myLib64_Remove -text "Remove"  -command wokdep:gui:RemoveLibPath
ttk::button   .myFrame.myLib64_Clear  -text "Reset"   -command wokdep:gui:ResetLibPath
ttk::label    .myFrame.myLib64_ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional debug libraries (64-bit) search paths
ttk::label    .myFrame.myLib64D_Lbl    -text "Additional debug libraries (64-bit) search paths:" -padding {5 5 80 5}
scrollbar     .myFrame.myLib64D_Scrl   -command ".myFrame.myLib64D_List yview"
listbox       .myFrame.myLib64D_List   -listvariable CSF_OPT_LIB64D -width 80 -height 5 -yscrollcommand ".myFrame.myLib64D_Scrl set"
ttk::button   .myFrame.myLib64D_Add    -text "Add"     -command wokdep:gui:AddLibPath
ttk::button   .myFrame.myLib64D_Edit   -text "Edit"
ttk::button   .myFrame.myLib64D_Remove -text "Remove"  -command wokdep:gui:RemoveLibPath
ttk::button   .myFrame.myLib64D_Clear  -text "Reset"   -command wokdep:gui:ResetLibPath
ttk::label    .myFrame.myLib64D_ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional executables (32-bit) search paths
ttk::label    .myFrame.myBin32_Lbl    -text "Additional executables (32-bit) search paths:" -padding {5 5 80 5}
scrollbar     .myFrame.myBin32_Scrl   -command ".myFrame.myBin32_List yview"
listbox       .myFrame.myBin32_List   -listvariable CSF_OPT_BIN32 -width 80 -height 5 -yscrollcommand ".myFrame.myBin32_Scrl set"
ttk::button   .myFrame.myBin32_Add    -text "Add"     -command wokdep:gui:AddBinPath
ttk::button   .myFrame.myBin32_Edit   -text "Edit"
ttk::button   .myFrame.myBin32_Remove -text "Remove"  -command wokdep:gui:RemoveBinPath
ttk::button   .myFrame.myBin32_Clear  -text "Reset"   -command wokdep:gui:ResetLibPath
ttk::label    .myFrame.myBin32_ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional debug executables (32-bit) search paths
ttk::label    .myFrame.myBin32D_Lbl    -text "Additional debug executables (32-bit) search paths:" -padding {5 5 80 5}
scrollbar     .myFrame.myBin32D_Scrl   -command ".myFrame.myBin32D_List yview"
listbox       .myFrame.myBin32D_List   -listvariable CSF_OPT_BIN32D -width 80 -height 5 -yscrollcommand ".myFrame.myBin32D_Scrl set"
ttk::button   .myFrame.myBin32D_Add    -text "Add"     -command wokdep:gui:AddBinPath
ttk::button   .myFrame.myBin32D_Edit   -text "Edit"
ttk::button   .myFrame.myBin32D_Remove -text "Remove"  -command wokdep:gui:RemoveBinPath
ttk::button   .myFrame.myBin32D_Clear  -text "Reset"   -command wokdep:gui:ResetLibPath
ttk::label    .myFrame.myBin32D_ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional executables (64-bit) search paths
ttk::label    .myFrame.myBin64_Lbl    -text "Additional executables (64-bit) search paths:" -padding {5 5 80 5}
scrollbar     .myFrame.myBin64_Scrl   -command ".myFrame.myBin64_List yview"
listbox       .myFrame.myBin64_List   -listvariable CSF_OPT_BIN64 -width 80 -height 5 -yscrollcommand ".myFrame.myBin64_Scrl set"
ttk::button   .myFrame.myBin64_Add    -text "Add"     -command wokdep:gui:AddBinPath
ttk::button   .myFrame.myBin64_Edit   -text "Edit"
ttk::button   .myFrame.myBin64_Remove -text "Remove"  -command wokdep:gui:RemoveBinPath
ttk::button   .myFrame.myBin64_Clear  -text "Reset"   -command wokdep:gui:ResetLibPath
ttk::label    .myFrame.myBin64_ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional debug executables (64-bit) search paths
ttk::label    .myFrame.myBin64D_Lbl    -text "Additional debug executables (64-bit) search paths:" -padding {5 5 80 5}
scrollbar     .myFrame.myBin64D_Scrl   -command ".myFrame.myBin64D_List yview"
listbox       .myFrame.myBin64D_List   -listvariable CSF_OPT_BIN64D -width 80 -height 5 -yscrollcommand ".myFrame.myBin64D_Scrl set"
ttk::button   .myFrame.myBin64D_Add    -text "Add"     -command wokdep:gui:AddBinPath
ttk::button   .myFrame.myBin64D_Edit   -text "Edit"
ttk::button   .myFrame.myBin64D_Remove -text "Remove"  -command wokdep:gui:RemoveBinPath
ttk::button   .myFrame.myBin64D_Clear  -text "Reset"   -command wokdep:gui:ResetLibPath
ttk::label    .myFrame.myBin64D_ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Bottom
ttk::button   .myFrame.mySave  -text "Save"  -command wokdep:SaveCustom
ttk::button   .myFrame.myClose -text "Close" -command wokdep:gui:Close

# Create grid
# Header
grid .myFrame.myPrjFrame            -row $aRowIter -column 0 -columnspan 10 -sticky w
grid .myFrame.myPrjFrame.myPrjLbl   -row 0 -column 0
grid .myFrame.myPrjFrame.myPrjCombo -row 0 -column 1
if { "$tcl_platform(platform)" == "windows" } {
  incr aRowIter
  grid .myFrame.myVsFrame               -row $aRowIter -column 0 -columnspan 10 -sticky w
  grid .myFrame.myVsFrame.myVsLbl       -row 0 -column 0
  grid .myFrame.myVsFrame.myVsCombo     -row 0 -column 1 -padx 5
  grid .myFrame.myVsFrame.myArchCombo   -row 0 -column 2
  grid .myFrame.myVsFrame.myConfigCombo -row 0 -column 3
  incr aRowIter
  grid .myFrame.myVcEntry     -row $aRowIter -column 0
  grid .myFrame.myVcVarsEntry -row $aRowIter -column 1 -columnspan 4 -sticky w
  grid .myFrame.myVcBrowseBtn -row $aRowIter -column 6
  incr aRowIter
} else {
  grid .myFrame.myVsFrame               -row $aRowIter -column 4 -sticky w
  grid .myFrame.myVsFrame.myConfigCombo -row 0 -column 0
  incr aRowIter
}

#
grid .myFrame.myHxxChecks -row $aRowIter -column 0 -columnspan 10 -sticky w
grid .myFrame.myHxxChecks.myScutsLbl     -row 0 -column 0
grid .myFrame.myHxxChecks.myScutsCombo   -row 0 -column 1
if { "$tcl_platform(platform)" == "windows" } {
  grid .myFrame.myHxxChecks.myRelDebInfoCheck -row 0 -column 2
  grid .myFrame.myHxxChecks.myRelDebInfoLbl   -row 0 -column 3
}
incr aRowIter
#
grid .myFrame.mySrchLbl       -row $aRowIter -column 0 -columnspan 10 -sticky w
incr aRowIter
grid .myFrame.mySrchEntry     -row $aRowIter -column 0 -columnspan 5
grid .myFrame.mySrchBrowseBtn -row $aRowIter -column 6
incr aRowIter

grid .myFrame.myChecks        -row $aRowIter -column 0 -columnspan 10 -sticky w
incr aRowIter
grid .myFrame.myChecks.myFreeTypeCheck  -row $aCheckRowIter -column 0 -sticky e
grid .myFrame.myChecks.myFreeTypeLbl    -row $aCheckRowIter -column 1 -sticky w
grid .myFrame.myChecks.myRapidJsonCheck -row $aCheckRowIter -column 2 -sticky e
grid .myFrame.myChecks.myRapidJsonLbl   -row $aCheckRowIter -column 3 -sticky w
if { "$::tcl_platform(os)" != "Darwin" } {
  grid .myFrame.myChecks.myGlesCheck     -row $aCheckRowIter -column 4 -sticky e
  grid .myFrame.myChecks.myGlesLbl       -row $aCheckRowIter -column 5 -sticky w
}
#grid .myFrame.myChecks.myOpenClCheck   -row $aCheckRowIter -column 6 -sticky e
#grid .myFrame.myChecks.myOpenClLbl     -row $aCheckRowIter -column 7 -sticky w
grid .myFrame.myChecks.myZLibCheck     -row $aCheckRowIter -column 6 -sticky e
grid .myFrame.myChecks.myZLibLbl       -row $aCheckRowIter -column 7 -sticky w

grid .myFrame.myChecks.myQtCheck      -row $aCheckRowIter -column 12 -sticky e
grid .myFrame.myChecks.myQtLbl        -row $aCheckRowIter -column 13 -sticky w

incr aCheckRowIter
grid .myFrame.myChecks.myFImageCheck   -row $aCheckRowIter -column 0 -sticky e
grid .myFrame.myChecks.myFImageLbl     -row $aCheckRowIter -column 1 -sticky w
grid .myFrame.myChecks.myDracoCheck    -row $aCheckRowIter -column 2 -sticky e
grid .myFrame.myChecks.myDracoLbl      -row $aCheckRowIter -column 3 -sticky w

if { "$::tcl_platform(platform)" == "windows" } {
  grid .myFrame.myChecks.myD3dCheck    -row $aCheckRowIter -column 4 -sticky e
  grid .myFrame.myChecks.myD3dLbl      -row $aCheckRowIter -column 5 -sticky w
} else {
  grid .myFrame.myChecks.myXLibCheck   -row $aCheckRowIter -column 4 -sticky e
  grid .myFrame.myChecks.myXLibLbl     -row $aCheckRowIter -column 5 -sticky w
}
grid .myFrame.myChecks.myLzmaCheck     -row $aCheckRowIter -column 6 -sticky e
grid .myFrame.myChecks.myLzmaLbl       -row $aCheckRowIter -column 7 -sticky w
grid .myFrame.myChecks.myJDKCheck      -row $aCheckRowIter -column 12 -sticky e
grid .myFrame.myChecks.myJDKLbl        -row $aCheckRowIter -column 13 -sticky w

incr aCheckRowIter
grid .myFrame.myChecks.myFFmpegCheck   -row $aCheckRowIter -column 0 -sticky e
grid .myFrame.myChecks.myFFmpegLbl     -row $aCheckRowIter -column 1 -sticky w
grid .myFrame.myChecks.myVtkCheck      -row $aCheckRowIter -column 2 -sticky e
grid .myFrame.myChecks.myVtkLbl        -row $aCheckRowIter -column 3 -sticky w
grid .myFrame.myChecks.myOpenVrCheck   -row $aCheckRowIter -column 4 -sticky e
grid .myFrame.myChecks.myOpenVrLbl     -row $aCheckRowIter -column 5 -sticky w
grid .myFrame.myChecks.myE57Check      -row $aCheckRowIter -column 6 -sticky e
grid .myFrame.myChecks.myE57Lbl        -row $aCheckRowIter -column 7 -sticky w

if { "$::tcl_platform(platform)" == "windows" } {
  grid .myFrame.myChecks.myInspectorBuild      -row $aCheckRowIter -column 12 -sticky e
  grid .myFrame.myChecks.myInspectorLbl        -row $aCheckRowIter -column 13 -sticky w
}

incr aCheckRowIter

grid .myFrame.myChecks.myTbbCheck      -row $aCheckRowIter -column 12 -sticky e
grid .myFrame.myChecks.myTbbLbl        -row $aCheckRowIter -column 13 -sticky w

incr aCheckRowIter

# Additional headers search paths
grid .myFrame.myIncLbl    -row $aRowIter -column 0 -columnspan 10 -sticky w
incr aRowIter
grid .myFrame.myIncList   -row $aRowIter -column 0 -rowspan 4 -columnspan 5
grid .myFrame.myIncScrl   -row $aRowIter -column 5 -rowspan 4
grid .myFrame.myIncAdd    -row $aRowIter -column 6
incr aRowIter
#grid .myFrame.myIncEdit   -row $aRowIter -column 6
incr aRowIter
grid .myFrame.myIncRemove -row $aRowIter -column 6
incr aRowIter
grid .myFrame.myIncClear  -row $aRowIter -column 6
incr aRowIter
grid .myFrame.myIncErrLbl -row $aRowIter -column 0 -columnspan 10 -sticky w
incr aRowIter

# Additional search paths
wokdep:gui:Show3264Bitness aRowIter

# Bottom section
grid .myFrame.mySave  -row $aRowIter -column 4 -columnspan 2
grid .myFrame.myClose -row $aRowIter -column 6 -columnspan 2

# Bind events
bind .myFrame.myPrjFrame.myPrjCombo <<ComboboxSelected>> {
  wokdep:gui:SwitchConfig
}
bind .myFrame.myVsFrame.myVsCombo <<ComboboxSelected>> {
  wokdep:gui:SwitchConfig
}
bind .myFrame.myVsFrame.myArchCombo <<ComboboxSelected>> {
  wokdep:gui:SwitchArch
}
bind .myFrame.myVsFrame.myConfigCombo <<ComboboxSelected>> {
  wokdep:gui:SwitchArch
}

.myFrame.mySrchEntry configure -validate all -validatecommand {
  set ::PRODUCTS_PATH [file normalize "$::PRODUCTS_PATH_INPUT"]
  #return [file exists "$::PRODUCTS_PATH"]
  wokdep:gui:UpdateList
  return 1
}

wokdep:gui:UpdateList
