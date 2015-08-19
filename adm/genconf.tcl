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

package require Tk

set aRowIter 0
frame .myFrame -padx 5 -pady 5
pack  .myFrame -fill both -expand 1

set SYS_VS_LIST {}
set SYS_VC_LIST {}
set SYS_VCVARS_LIST {}

# detect installed Visual Studio instances from global environment
if { [info exists ::env(VS150COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 201x (vc15)"
  lappend ::SYS_VC_LIST "vc15"
  lappend ::SYS_VCVARS_LIST "%VS150COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS140COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2015 (vc14)"
  lappend ::SYS_VC_LIST "vc14"
  lappend ::SYS_VCVARS_LIST "%VS140COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS120COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2013 (vc12)"
  lappend ::SYS_VC_LIST "vc12"
  lappend ::SYS_VCVARS_LIST "%VS120COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS110COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2012 (vc11)"
  lappend ::SYS_VC_LIST "vc11"
  lappend ::SYS_VCVARS_LIST "%VS110COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS100COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2010 (vc10)"
  lappend ::SYS_VC_LIST "vc10"
  lappend ::SYS_VCVARS_LIST "%VS100COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS90COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2008 (vc9)"
  lappend ::SYS_VC_LIST "vc9"
  lappend ::SYS_VCVARS_LIST "%VS90COMNTOOLS%..\\..\\VC\\vcvarsall.bat"
}
if { [info exists ::env(VS80COMNTOOLS)] } {
  lappend ::SYS_VS_LIST "Visual Studio 2005 (vc8)"
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

proc wokdep:gui:SwitchConfig {} {
  set ::VCVER  [lindex $::SYS_VC_LIST     [.myFrame.myVsCombo current]]
  set ::VCVARS [lindex $::SYS_VCVARS_LIST [.myFrame.myVsCombo current]]

  set ::CSF_OPT_INC {}
  set ::CSF_OPT_LIB32 {}
  set ::CSF_OPT_LIB64 {}
  set ::CSF_OPT_BIN32 {}
  set ::CSF_OPT_BIN64 {}
  wokdep:gui:UpdateList
}

proc wokdep:gui:SwitchArch {} {
  if { "$::ARCH" == "32" } {
    wokdep:gui:Show32Bitness ::aRowIter
  }
  if { "$::ARCH" == "64" } {
    wokdep:gui:Show64Bitness ::aRowIter
  }

  if { [llength [grid info .myFrame.mySave]] != 0 } {
    grid forget .myFrame.mySave .myFrame.myClose
  }

  # Bottom section
  grid .myFrame.mySave  -row $::aRowIter -column 4 -columnspan 2
  grid .myFrame.myClose -row $::aRowIter -column 6 -columnspan 2
}

proc wokdep:gui:UpdateList {} {
  set anIncErrs   {}
  set anLib32Errs {}
  set anLib64Errs {}
  set anBin32Errs {}
  set anBin64Errs {}
  wokdep:SearchTclTk     anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  wokdep:SearchFreeType  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  wokdep:SearchX11       anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  if { "$::HAVE_FREEIMAGE" == "true" } {
    wokdep:SearchFreeImage anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::HAVE_GL2PS" == "true" } {
    wokdep:SearchGL2PS   anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::HAVE_TBB" == "true" } {
    wokdep:SearchTBB     anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::HAVE_OPENCL" == "true" } {
    wokdep:SearchOpenCL  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::HAVE_VTK" == "true" } {
    wokdep:SearchVTK  anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::CHECK_QT4" == "true" } {
    wokdep:SearchQt4     anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }
  if { "$::CHECK_JDK" == "true" } {
    wokdep:SearchJDK     anIncErrs anLib32Errs anLib64Errs anBin32Errs anBin64Errs
  }

  .myFrame.myIncErrLbl   configure -text [join $anIncErrs   "\n"]
  .myFrame.myLib32ErrLbl configure -text [join $anLib32Errs "\n"]
  .myFrame.myLib64ErrLbl configure -text [join $anLib64Errs "\n"]
  .myFrame.myBin32ErrLbl configure -text [join $anBin32Errs "\n"]
  .myFrame.myBin64ErrLbl configure -text [join $anBin64Errs "\n"]
}

proc wokdep:gui:BrowseVcVars {} {
  set aResult [tk_chooseDirectory -initialdir $::VCVARS -title "Choose a directory"]
  if { "$aResult" != "" } {
    set ::VCVARS $aResult
  }
}

proc wokdep:gui:BrowsePartiesRoot {} {
  set aResult [tk_chooseDirectory -initialdir $::PRODUCTS_PATH -title "Choose a directory"]
  if { "$aResult" != "" } {
    set ::PRODUCTS_PATH $aResult
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

proc wokdep:gui:AddLib32Path {} {
  set aResult [tk_chooseDirectory -title "Choose a directory"]
  if { "$aResult" != "" } {
    lappend ::CSF_OPT_LIB32 "$aResult"
    wokdep:gui:UpdateList
  }
}

proc wokdep:gui:AddLib64Path {} {
  set aResult [tk_chooseDirectory -title "Choose a directory"]
  if { "$aResult" != "" } {
    lappend ::CSF_OPT_LIB64 "$aResult"
    wokdep:gui:UpdateList
  }
}

proc wokdep:gui:AddBin32Path {} {
  set aResult [tk_chooseDirectory -title "Choose a directory"]
  if { "$aResult" != "" } {
    lappend ::CSF_OPT_BIN32 "$aResult"
    wokdep:gui:UpdateList
  }
}

proc wokdep:gui:AddBin64Path {} {
  set aResult [tk_chooseDirectory -title "Choose a directory"]
  if { "$aResult" != "" } {
    lappend ::CSF_OPT_BIN64 "$aResult"
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

proc wokdep:gui:RemoveLib32Path {} {
  set aSelIndices [.myFrame.myLib32List curselection]
  if { [llength $aSelIndices] != 0 } {
    .myFrame.myLib32List delete [lindex $aSelIndices 0]
  }
  wokdep:gui:UpdateList
}

proc wokdep:gui:RemoveLib64Path {} {
  set aSelIndices [.myFrame.myLib64List curselection]
  if { [llength $aSelIndices] != 0 } {
    .myFrame.myLib64List delete [lindex $aSelIndices 0]
  }
  wokdep:gui:UpdateList
}

proc wokdep:gui:RemoveBin32Path {} {
  set aSelIndices [.myFrame.myBin32List curselection]
  if { [llength $aSelIndices] != 0 } {
    .myFrame.myBin32List delete [lindex $aSelIndices 0]
  }
  wokdep:gui:UpdateList
}

proc wokdep:gui:RemoveBin64Path {} {
  set aSelIndices [.myFrame.myBin64List curselection]
  if { [llength $aSelIndices] != 0 } {
    .myFrame.myBin64List delete [lindex $aSelIndices 0]
  }
  wokdep:gui:UpdateList
}

proc wokdep:gui:ResetIncPath {} {
  set ::CSF_OPT_INC {}
  wokdep:gui:UpdateList
}

proc wokdep:gui:ResetLib32Path {} {
  set ::CSF_OPT_LIB32 {}
  wokdep:gui:UpdateList
}

proc wokdep:gui:ResetLib64Path {} {
  set ::CSF_OPT_LIB64 {}
  wokdep:gui:UpdateList
}

proc wokdep:gui:ResetBin32Path {} {
  set ::CSF_OPT_BIN32 {}
  wokdep:gui:UpdateList
}

proc wokdep:gui:ResetBin64Path {} {
  set ::CSF_OPT_BIN64 {}
  wokdep:gui:UpdateList
}

proc wokdep:gui:Show32Bitness { theRowIter } {
  upvar $theRowIter aRowIter

  if { [llength [grid info .myFrame.myLib64Lbl]] != 0 } {
    grid forget .myFrame.myLib64Lbl .myFrame.myLib64List   .myFrame.myLib64Scrl
    grid forget .myFrame.myLib64Add .myFrame.myLib64Remove .myFrame.myLib64Clear .myFrame.myLib64ErrLbl
    grid forget .myFrame.myBin64Lbl .myFrame.myBin64List   .myFrame.myBin64Scrl
    grid forget .myFrame.myBin64Add .myFrame.myBin64Remove .myFrame.myBin64Clear .myFrame.myBin64ErrLbl
  }

  # Additional libraries (32-bit) search paths
  grid .myFrame.myLib32Lbl    -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
  grid .myFrame.myLib32List   -row $aRowIter -column 0 -rowspan 4 -columnspan 5
  grid .myFrame.myLib32Scrl   -row $aRowIter -column 5 -rowspan 4
  grid .myFrame.myLib32Add    -row $aRowIter -column 6
  incr aRowIter
  #grid .myFrame.myLib32Edit   -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myLib32Remove -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myLib32Clear  -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myLib32ErrLbl -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter

  # Additional executables (32-bit) search paths
  grid .myFrame.myBin32Lbl    -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
  grid .myFrame.myBin32List   -row $aRowIter -column 0 -rowspan 4 -columnspan 5
  grid .myFrame.myBin32Scrl   -row $aRowIter -column 5 -rowspan 4
  grid .myFrame.myBin32Add    -row $aRowIter -column 6
  incr aRowIter
  #grid .myFrame.myBin32Edit   -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myBin32Remove -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myBin32Clear  -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myBin32ErrLbl -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
}

proc wokdep:gui:Show64Bitness { theRowIter } {
  upvar $theRowIter aRowIter

  if { [llength [grid info .myFrame.myLib32Lbl]] != 0 } {
    grid forget .myFrame.myLib32Lbl .myFrame.myLib32List   .myFrame.myLib32Scrl
    grid forget .myFrame.myLib32Add .myFrame.myLib32Remove .myFrame.myLib32Clear .myFrame.myLib32ErrLbl
    grid forget .myFrame.myBin32Lbl .myFrame.myBin32List   .myFrame.myBin32Scrl
    grid forget .myFrame.myBin32Add .myFrame.myBin32Remove .myFrame.myBin32Clear .myFrame.myBin32ErrLbl
  }

  # Additional libraries (64-bit) search paths
  grid .myFrame.myLib64Lbl    -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
  grid .myFrame.myLib64List   -row $aRowIter -column 0 -rowspan 4 -columnspan 5
  grid .myFrame.myLib64Scrl   -row $aRowIter -column 5 -rowspan 4
  grid .myFrame.myLib64Add    -row $aRowIter -column 6
  incr aRowIter
  #grid .myFrame.myLib64Edit   -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myLib64Remove -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myLib64Clear  -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myLib64ErrLbl -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter

  # Additional executables (64-bit) search paths
  grid .myFrame.myBin64Lbl    -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
  grid .myFrame.myBin64List   -row $aRowIter -column 0 -rowspan 4 -columnspan 5
  grid .myFrame.myBin64Scrl   -row $aRowIter -column 5 -rowspan 4
  grid .myFrame.myBin64Add    -row $aRowIter -column 6
  incr aRowIter
  #grid .myFrame.myBin64Edit   -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myBin64Remove -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myBin64Clear  -row $aRowIter -column 6
  incr aRowIter
  grid .myFrame.myBin64ErrLbl -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
}

# Header
ttk::label    .myFrame.myVsLbl       -text "Visual Studio configuration" -relief solid -padding {5 5 80 5}
ttk::combobox .myFrame.myVsCombo     -values $SYS_VS_LIST -state readonly -textvariable VSVER -width 30
ttk::combobox .myFrame.myArchCombo   -values { {32} {64} } -textvariable ARCH -state readonly -width 6
entry         .myFrame.myVcEntry     -textvariable VCVER  -width 6
entry         .myFrame.myVcVarsEntry -textvariable VCVARS -width 70
ttk::button   .myFrame.myVcBrowseBtn -text "Browse" -command wokdep:gui:BrowseVcVars

#
ttk::label    .myFrame.myOcctBuildLbl -text "OCCT headers configuration" -relief solid -padding {5 5 80 5}
checkbutton   .myFrame.myScutsCheck   -offvalue "false" -onvalue "true" -variable SHORTCUT_HEADERS
ttk::label    .myFrame.myScutsLbl     -text "Short-cut headers instead original ones in inc"

#
ttk::label    .myFrame.mySrchLbl       -text "3rd-parties search path" -relief solid -padding {5 5 80 5}
entry         .myFrame.mySrchEntry     -textvariable PRODUCTS_PATH -width 80
ttk::button   .myFrame.mySrchBrowseBtn -text "Browse" -command wokdep:gui:BrowsePartiesRoot
checkbutton   .myFrame.myFImageCheck   -offvalue "false" -onvalue "true" -variable HAVE_FREEIMAGE -command wokdep:gui:UpdateList
ttk::label    .myFrame.myFImageLbl     -text "Use FreeImage"
checkbutton   .myFrame.myGl2psCheck    -offvalue "false" -onvalue "true" -variable HAVE_GL2PS     -command wokdep:gui:UpdateList
ttk::label    .myFrame.myGl2psLbl      -text "Use GL2PS"
checkbutton   .myFrame.myTbbCheck      -offvalue "false" -onvalue "true" -variable HAVE_TBB       -command wokdep:gui:UpdateList
ttk::label    .myFrame.myTbbLbl        -text "Use Intel TBB"
checkbutton   .myFrame.myOpenClCheck   -offvalue "false" -onvalue "true" -variable HAVE_OPENCL    -command wokdep:gui:UpdateList
ttk::label    .myFrame.myOpenClLbl     -text "Use OpenCL"
checkbutton   .myFrame.myMacGLXCheck   -offvalue "false" -onvalue "true" -variable MACOSX_USE_GLX
ttk::label    .myFrame.myMacGLXLbl     -text "Use X11 for windows drawing"
ttk::label    .myFrame.myVtkLbl        -text "Use VTK"
checkbutton   .myFrame.myVtkCheck      -offvalue "false" -onvalue "true" -variable HAVE_VTK       -command wokdep:gui:UpdateList
checkbutton   .myFrame.myQt4Check      -offvalue "false" -onvalue "true" -variable CHECK_QT4      -command wokdep:gui:UpdateList
ttk::label    .myFrame.myQt4Lbl        -text "Search Qt4"
checkbutton   .myFrame.myJDKCheck      -offvalue "false" -onvalue "true" -variable CHECK_JDK      -command wokdep:gui:UpdateList
ttk::label    .myFrame.myJDKLbl        -text "Search JDK"

# Additional headers search paths
ttk::label    .myFrame.myIncLbl    -text "Additional headers search paths" -relief solid -padding {5 5 80 5}
scrollbar     .myFrame.myIncScrl   -command ".myFrame.myIncList yview"
listbox       .myFrame.myIncList   -listvariable CSF_OPT_INC -width 80 -height 5 -yscrollcommand ".myFrame.myIncScrl set"
ttk::button   .myFrame.myIncAdd    -text "Add"     -command wokdep:gui:AddIncPath
ttk::button   .myFrame.myIncEdit   -text "Edit"
ttk::button   .myFrame.myIncRemove -text "Remove"  -command wokdep:gui:RemoveIncPath
ttk::button   .myFrame.myIncClear  -text "Reset"   -command wokdep:gui:ResetIncPath
ttk::label    .myFrame.myIncErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional libraries (32-bit) search paths
ttk::label    .myFrame.myLib32Lbl    -text "Additional libraries (32-bit) search paths" -relief solid -padding {5 5 80 5}
scrollbar     .myFrame.myLib32Scrl   -command ".myFrame.myLib32List yview"
listbox       .myFrame.myLib32List   -listvariable CSF_OPT_LIB32 -width 80 -height 5 -yscrollcommand ".myFrame.myLib32Scrl set"
ttk::button   .myFrame.myLib32Add    -text "Add"     -command wokdep:gui:AddLib32Path
ttk::button   .myFrame.myLib32Edit   -text "Edit"
ttk::button   .myFrame.myLib32Remove -text "Remove"  -command wokdep:gui:RemoveLib32Path
ttk::button   .myFrame.myLib32Clear  -text "Reset"   -command wokdep:gui:ResetLib32Path
ttk::label    .myFrame.myLib32ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional libraries (64-bit) search paths
ttk::label    .myFrame.myLib64Lbl    -text "Additional libraries (64-bit) search paths" -relief solid -padding {5 5 80 5}
scrollbar     .myFrame.myLib64Scrl   -command ".myFrame.myLib64List yview"
listbox       .myFrame.myLib64List   -listvariable CSF_OPT_LIB64 -width 80 -height 5 -yscrollcommand ".myFrame.myLib64Scrl set"
ttk::button   .myFrame.myLib64Add    -text "Add"     -command wokdep:gui:AddLib64Path
ttk::button   .myFrame.myLib64Edit   -text "Edit"
ttk::button   .myFrame.myLib64Remove -text "Remove"  -command wokdep:gui:RemoveLib64Path
ttk::button   .myFrame.myLib64Clear  -text "Reset"   -command wokdep:gui:ResetLib64Path
ttk::label    .myFrame.myLib64ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional executables (32-bit) search paths
ttk::label    .myFrame.myBin32Lbl    -text "Additional executables (32-bit) search paths" -relief solid -padding {5 5 80 5}
scrollbar     .myFrame.myBin32Scrl   -command ".myFrame.myBin32List yview"
listbox       .myFrame.myBin32List   -listvariable CSF_OPT_BIN32 -width 80 -height 5 -yscrollcommand ".myFrame.myBin32Scrl set"
ttk::button   .myFrame.myBin32Add    -text "Add"     -command wokdep:gui:AddBin32Path
ttk::button   .myFrame.myBin32Edit   -text "Edit"
ttk::button   .myFrame.myBin32Remove -text "Remove"  -command wokdep:gui:RemoveBin32Path
ttk::button   .myFrame.myBin32Clear  -text "Reset"   -command wokdep:gui:ResetBin32Path
ttk::label    .myFrame.myBin32ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Additional executables (64-bit) search paths
ttk::label    .myFrame.myBin64Lbl    -text "Additional executables (64-bit) search paths" -relief solid -padding {5 5 80 5}
scrollbar     .myFrame.myBin64Scrl   -command ".myFrame.myBin64List yview"
listbox       .myFrame.myBin64List   -listvariable CSF_OPT_BIN64 -width 80 -height 5 -yscrollcommand ".myFrame.myBin64Scrl set"
ttk::button   .myFrame.myBin64Add    -text "Add"     -command wokdep:gui:AddBin64Path
ttk::button   .myFrame.myBin64Edit   -text "Edit"
ttk::button   .myFrame.myBin64Remove -text "Remove"  -command wokdep:gui:RemoveBin64Path
ttk::button   .myFrame.myBin64Clear  -text "Reset"   -command wokdep:gui:ResetBin64Path
ttk::label    .myFrame.myBin64ErrLbl -text "Error: " -foreground red -padding {5 5 5 5}

# Bottom
ttk::button   .myFrame.mySave  -text "Save"  -command wokdep:SaveCustom
ttk::button   .myFrame.myClose -text "Close" -command wokdep:gui:Close

# Create grid
# Header
if { "$tcl_platform(platform)" == "windows" } {
  grid .myFrame.myVsLbl       -row $aRowIter -column 0 -columnspan 10 -sticky w
  incr aRowIter
  grid .myFrame.myVsCombo     -row $aRowIter -column 0 -columnspan 2 -sticky w
  grid .myFrame.myArchCombo   -row $aRowIter -column 2 -sticky w
  incr aRowIter
  grid .myFrame.myVcEntry     -row $aRowIter -column 0
  grid .myFrame.myVcVarsEntry -row $aRowIter -column 1 -columnspan 4 -sticky w
  grid .myFrame.myVcBrowseBtn -row $aRowIter -column 6
  incr aRowIter
}

#
grid .myFrame.myOcctBuildLbl -row $aRowIter -column 0 -columnspan 10 -sticky w
incr aRowIter
grid .myFrame.myScutsCheck   -row $aRowIter -column 0 -sticky e
grid .myFrame.myScutsLbl     -row $aRowIter -column 1 -columnspan 2 -sticky w
incr aRowIter
#
grid .myFrame.mySrchLbl       -row $aRowIter -column 0 -columnspan 10 -sticky w
incr aRowIter
grid .myFrame.mySrchEntry     -row $aRowIter -column 0 -columnspan 5
grid .myFrame.mySrchBrowseBtn -row $aRowIter -column 6
incr aRowIter
grid .myFrame.myFImageCheck   -row $aRowIter -column 0 -sticky e
grid .myFrame.myFImageLbl     -row $aRowIter -column 1 -sticky w
grid .myFrame.myQt4Check      -row $aRowIter -column 2 -sticky e
grid .myFrame.myQt4Lbl        -row $aRowIter -column 3 -sticky w
incr aRowIter
grid .myFrame.myGl2psCheck    -row $aRowIter -column 0 -sticky e
grid .myFrame.myGl2psLbl      -row $aRowIter -column 1 -sticky w
grid .myFrame.myJDKCheck      -row $aRowIter -column 2 -sticky e
grid .myFrame.myJDKLbl        -row $aRowIter -column 3 -sticky w
incr aRowIter
grid .myFrame.myTbbCheck      -row $aRowIter -column 0 -sticky e
grid .myFrame.myTbbLbl        -row $aRowIter -column 1 -sticky w
incr aRowIter
grid .myFrame.myOpenClCheck   -row $aRowIter -column 0 -sticky e
grid .myFrame.myOpenClLbl     -row $aRowIter -column 1 -sticky w
incr aRowIter
grid .myFrame.myVtkCheck      -row $aRowIter -column 0 -sticky e
grid .myFrame.myVtkLbl        -row $aRowIter -column 1 -sticky w
incr aRowIter
if { "$::tcl_platform(os)" == "Darwin" } {
  grid .myFrame.myMacGLXCheck -row $aRowIter -column 0 -sticky e
  grid .myFrame.myMacGLXLbl   -row $aRowIter -column 1 -sticky w
  incr aRowIter
}

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

# Additional 32-bit search paths
if { "$ARCH" == "32" } {
  wokdep:gui:Show32Bitness aRowIter
}

# Additional 64-bit search paths
if { "$ARCH" == "64" } {
  wokdep:gui:Show64Bitness aRowIter
}

# Bottom section
grid .myFrame.mySave  -row $aRowIter -column 4 -columnspan 2
grid .myFrame.myClose -row $aRowIter -column 6 -columnspan 2

# Bind events
bind .myFrame.myVsCombo <<ComboboxSelected>> {
  wokdep:gui:SwitchConfig
}
bind .myFrame.myArchCombo <<ComboboxSelected>> {
  wokdep:gui:SwitchArch
}

.myFrame.mySrchEntry configure -validate all -validatecommand {
  #return [file exists "$::PRODUCTS_PATH"]
  wokdep:gui:UpdateList
  return 1
}

wokdep:gui:UpdateList
