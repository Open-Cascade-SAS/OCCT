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
# Tools for search of third-party libraries and generation on environment
# customization script
# =======================================================================

set ARCH "64"

if { "$tcl_platform(platform)" == "unix" } {
  set SYS_PATH_SPLITTER ":"
  set SYS_LIB_PREFIX    "lib"
  set SYS_EXE_SUFFIX    ""
  if { "$tcl_platform(os)" == "Darwin" } {
    set SYS_LIB_SUFFIX "dylib"
    set PRJFMT "xcd"
  } else {
    set SYS_LIB_SUFFIX "so"
    set PRJFMT "cbp"
  }
  set VCVER "gcc"
  set VCVARS ""
} elseif { "$tcl_platform(platform)" == "windows" } {
  set SYS_PATH_SPLITTER ";"
  set SYS_LIB_PREFIX    ""
  set SYS_LIB_SUFFIX    "lib"
  set SYS_EXE_SUFFIX    ".exe"
  set VCVER  "vc10"
  set VCVARS ""
  set PRJFMT "vcxproj"
}

set SHORTCUT_HEADERS "ShortCut"

set PRODUCTS_PATH ""
set CSF_OPT_INC   [list]
set CSF_OPT_LIB32 [list]
set CSF_OPT_LIB64 [list]
set CSF_OPT_BIN32 [list]
set CSF_OPT_BIN64 [list]
set CSF_OPT_LIB32D [list]
set CSF_OPT_LIB64D [list]
set CSF_OPT_BIN32D [list]
set CSF_OPT_BIN64D [list]

if { "$tcl_platform(pointerSize)" == "4" } {
  set ARCH "32"
}
if { [info exists ::env(ARCH)] } {
  set ARCH "$::env(ARCH)"
}

if { [info exists ::env(SHORTCUT_HEADERS)] } {
  set SHORTCUT_HEADERS "$::env(SHORTCUT_HEADERS)"
  if { $SHORTCUT_HEADERS == "true" } {
    set SHORTCUT_HEADERS "ShortCut"
  }
}

# fetch environment variables (e.g. set by custom.sh or custom.bat) and set them as tcl variables with the same name
set THE_ENV_VARIABLES { HAVE_TK HAVE_FREETYPE HAVE_FREEIMAGE HAVE_FFMPEG HAVE_TBB HAVE_GLES2 HAVE_D3D HAVE_VTK \
  HAVE_ZLIB HAVE_LIBLZMA HAVE_E57 HAVE_RAPIDJSON HAVE_DRACO HAVE_OPENVR HAVE_OPENCL \
  CHECK_QT4 CHECK_JDK HAVE_XLIB \
  HAVE_RelWithDebInfo BUILD_Inspector }
foreach anEnvIter $THE_ENV_VARIABLES { set ${anEnvIter} "false" }
set HAVE_TK       "true"
set HAVE_FREETYPE "true"
if { "$tcl_platform(os)" != "Darwin" } { set HAVE_XLIB "true" }
foreach anEnvIter $THE_ENV_VARIABLES {
  if { [info exists ::env(${anEnvIter})] } {
    set ${anEnvIter} "$::env(${anEnvIter})"
  }
}
# do not export platform-specific variables
if { "$::tcl_platform(os)" == "Darwin" } {
  set HAVE_GLES2 ""
}
if { "$tcl_platform(platform)" != "windows" } {
  set HAVE_D3D ""
  set HAVE_RelWithDebInfo ""
} else {
  set HAVE_XLIB ""
}
foreach anEnvIter {ARCH VCVER VCVARS PRJFMT } {
  if { [info exists ::env(${anEnvIter})] } {
    set ${anEnvIter} "$::env(${anEnvIter})"
  }
}
if { [info exists ::env(PRODUCTS_PATH)] } {
  set PRODUCTS_PATH [file normalize "$::env(PRODUCTS_PATH)"]
}

if { [info exists ::env(CSF_OPT_INC)] } {
  set CSF_OPT_INC [split "$::env(CSF_OPT_INC)" $::SYS_PATH_SPLITTER]
}
if { [info exists ::env(CSF_OPT_LIB32)] } {
  set CSF_OPT_LIB32 [split "$::env(CSF_OPT_LIB32)" $::SYS_PATH_SPLITTER]
}
if { [info exists ::env(CSF_OPT_LIB64)] } {
  set CSF_OPT_LIB64 [split "$::env(CSF_OPT_LIB64)" $::SYS_PATH_SPLITTER]
}
if { [info exists ::env(CSF_OPT_BIN32)] } {
  set CSF_OPT_BIN32 [split "$::env(CSF_OPT_BIN32)" $::SYS_PATH_SPLITTER]
}
if { [info exists ::env(CSF_OPT_BIN64)] } {
  set CSF_OPT_BIN64 [split "$::env(CSF_OPT_BIN64)" $::SYS_PATH_SPLITTER]
}

if { [info exists ::env(CSF_OPT_LIB32D)] } {
  set CSF_OPT_LIB32D [split "$::env(CSF_OPT_LIB32D)" $::SYS_PATH_SPLITTER]
  foreach aLibIter $::CSF_OPT_LIB32 {
    set aPos [lsearch -exact $::CSF_OPT_LIB32D $aLibIter]
    set ::CSF_OPT_LIB32D [lreplace $::CSF_OPT_LIB32D $aPos $aPos]
  }
}
if { [info exists ::env(CSF_OPT_LIB64D)] } {
  set CSF_OPT_LIB64D [split "$::env(CSF_OPT_LIB64D)" $::SYS_PATH_SPLITTER]
  foreach aLibIter $::CSF_OPT_LIB64 {
    set aPos [lsearch -exact $::CSF_OPT_LIB64D $aLibIter]
    set ::CSF_OPT_LIB64D [lreplace $::CSF_OPT_LIB64D $aPos $aPos]
  }
}
if { [info exists ::env(CSF_OPT_BIN32D)] } {
  set CSF_OPT_BIN32D [split "$::env(CSF_OPT_BIN32D)" $::SYS_PATH_SPLITTER]
  foreach aLibIter $::CSF_OPT_BIN32 {
    set aPos [lsearch -exact $::CSF_OPT_BIN32D $aLibIter]
    set ::CSF_OPT_BIN32D [lreplace $::CSF_OPT_BIN32D $aPos $aPos]
  }
}
if { [info exists ::env(CSF_OPT_BIN64D)] } {
  set CSF_OPT_BIN64D [split "$::env(CSF_OPT_BIN64D)" $::SYS_PATH_SPLITTER]
  foreach aLibIter $::CSF_OPT_BIN64 {
    set aPos [lsearch -exact $::CSF_OPT_BIN64D $aLibIter]
    set ::CSF_OPT_BIN64D [lreplace $::CSF_OPT_BIN64D $aPos $aPos]
  }
}

# Search header file in $::CSF_OPT_INC and standard paths
proc wokdep:SearchHeader {theHeader} {
  # search in custom paths
  foreach anIncPath $::CSF_OPT_INC {
    set aPath "${anIncPath}/${theHeader}"
    if { [file exists "$aPath"] } {
      return "$aPath"
    }
  }

  # search in system
  set aPath "/usr/include/${theHeader}"
  if { [file exists "$aPath"] } {
    return "$aPath"
  }

  if { "$::tcl_platform(os)" == "Linux" } {
    if { "$::ARCH" == "64" } {
      set aPath "/usr/include/x86_64-linux-gnu/${theHeader}"
      if { [file exists "$aPath"] } {
        return "$aPath"
      }
    } else {
      set aPath "/usr/include/i386-linux-gnu/${theHeader}"
      if { [file exists "$aPath"] } {
        return "$aPath"
      }
    }
  }

  return ""
}

# Search library file in $::CSF_OPT_LIB* and standard paths
proc wokdep:SearchLib {theLib theBitness {theSearchPath ""}} {
  if { "$theSearchPath" != "" } {
    set aPath  "${theSearchPath}/${::SYS_LIB_PREFIX}${theLib}.${::SYS_LIB_SUFFIX}"
    set aPath2 "${theSearchPath}/${::SYS_LIB_PREFIX}${theLib}.a"
    if { [file exists "$aPath"] } {
      return "$aPath"
    } elseif { "$::tcl_platform(platform)" != "windows" && [file exists "$aPath2"] } {
      return "$aPath2"
    } else {
      return ""
    }
  }

  # search in custom paths
  foreach aLibPath [set ::CSF_OPT_LIB$theBitness] {
    set aPath  "${aLibPath}/${::SYS_LIB_PREFIX}${theLib}.${::SYS_LIB_SUFFIX}"
    set aPath2 "${aLibPath}/${::SYS_LIB_PREFIX}${theLib}.a"
    if { [file exists "$aPath"] } {
      return "$aPath"
    } elseif { "$::tcl_platform(platform)" != "windows" && [file exists "$aPath2"] } {
      return "$aPath2"
    }
  }

  # search in system
  if { "$::ARCH" == "$theBitness"} {
    set aPath  "/usr/lib/${::SYS_LIB_PREFIX}${theLib}.${::SYS_LIB_SUFFIX}"
    set aPath2 "/usr/lib/${::SYS_LIB_PREFIX}${theLib}.a"
    if { [file exists "$aPath"] } {
      return "$aPath"
    } elseif { [file exists "$aPath2"] } {
      return "$aPath2"
    }
  }

  if { "$::tcl_platform(os)" == "Linux" } {
    if { "$theBitness" == "64" } {
      set aPath  "/usr/lib/x86_64-linux-gnu/lib${theLib}.so"
      set aPath2 "/usr/lib/x86_64-linux-gnu/lib${theLib}.a"
      if { [file exists "$aPath"] } {
        return "$aPath"
      } elseif { [file exists "$aPath2"] } {
        return "$aPath2"
      }
    } else {
      set aPath  "/usr/lib/i386-linux-gnu/lib${theLib}.so"
      set aPath2 "/usr/lib/i386-linux-gnu/lib${theLib}.a"
      if { [file exists "$aPath"] } {
        return "$aPath"
      } elseif { [file exists "$aPath2"] } {
        return "$aPath2"
      }
    }
  }

  return ""
}

# Search file in $::CSF_OPT_BIN* and standard paths
proc wokdep:SearchBin {theBin theBitness {theSearchPath ""}} {
  if { "$theSearchPath" != "" } {
    set aPath "${theSearchPath}/${theBin}"
    if { [file exists "$aPath"] } {
      return "$aPath"
    } else {
      return ""
    }
  }

  # search in custom paths
  foreach aBinPath [set ::CSF_OPT_BIN$theBitness] {
    set aPath "${aBinPath}/${theBin}"
    if { [file exists "$aPath"] } {
      return "$aPath"
    }
  }

  # search in system
  if { "$::ARCH" == "$theBitness"} {
    set aPath "/usr/bin/${theBin}"
    if { [file exists "$aPath"] } {
      return "$aPath"
    }
  }

  return ""
}

# Detect compiler C-runtime version 'vc*' and architecture '32'/'64'
# to determine preferred path.
proc wokdep:Preferred {theList theCmpl theArch} {
  if { [llength $theList] == 0 } {
    return ""
  }

  # keep only two first digits in "vc141"
  if { ! [regexp {^vc[0-9][0-9]} $theCmpl aCmpl] } {
    if { [regexp {^vclang} $theCmpl] } { 
      set aCmpl vc14
    } else {
      set aCmpl $theCmpl
    }
  }

  set aShortList {}
  foreach aPath $theList {
    if { [string first "$aCmpl" "$aPath"] != "-1" } {
      lappend aShortList "$aPath"
    }
  }

  if { [llength $aShortList] == 0 } {
    #return [lindex $theList 0]
    set aShortList $theList
  }

  set aVeryShortList {}
  foreach aPath $aShortList {
    if { [string first "$theArch" "$aPath"] != "-1" } {
      lappend aVeryShortList "$aPath"
    }
  }
  if { [llength $aVeryShortList] == 0 } {
    return [lindex [lsort -decreasing $aShortList] 0]
  }

  return [lindex [lsort -decreasing $aVeryShortList] 0]
}

# Search library placement
proc wokdep:SearchStandardLibrary {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64 theName theCheckHeader theCheckLib theCheckFolders} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  set aHeaderPath [wokdep:SearchHeader "$theCheckHeader"]
  if { "$aHeaderPath"  == "" } {
    set hasHeader false
    foreach aFolderIter $theCheckFolders {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{$aFolderIter}*] "$::VCVER" "$::ARCH" ]
      if { "$aPath" != "" && [file exists "$aPath/include/$theCheckHeader"] } {
        lappend ::CSF_OPT_INC "$aPath/include"
        set hasHeader true
        break
      }
    }
    if { !$hasHeader } {
      lappend anErrInc "Error: '$theCheckHeader' not found ($theName)"
      set isFound "false"
    }
  }

  foreach anArchIter {64 32} {
    set aLibPath [wokdep:SearchLib "$theCheckLib" "$anArchIter"]
    if { "$aLibPath" == "" } {
      set hasLib false
      foreach aFolderIter $theCheckFolders {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{$aFolderIter}*] "$::VCVER" "$anArchIter" ]
        set aLibPath [wokdep:SearchLib "$theCheckLib" "$anArchIter" "$aPath/lib"]
        if { "$aLibPath" != "" } {
          lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
          lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
          set hasLib true

          set aLibDPath [wokdep:SearchLib "$theCheckLib" "$anArchIter" "$aPath/libd"]
          if { "$aLibDPath" != "" } {
            lappend ::CSF_OPT_LIB${anArchIter}D "$aPath/libd"
            lappend ::CSF_OPT_BIN${anArchIter}D "$aPath/bind"
          }
          break
        }
      }
      if { !$hasLib } {
        lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}$theCheckLib.${::SYS_LIB_SUFFIX}' not found ($theName)"
        if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
      }
    }

    if { "$::tcl_platform(platform)" == "windows" } {
      set aDllPath [wokdep:SearchBin "$theCheckLib.dll" "$anArchIter"]
      if { "$aDllPath" == "" } {
        set hasDll false
        foreach aFolderIter $theCheckFolders {
          set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{$aFolderIter}*] "$::VCVER" "$anArchIter" ]
          set aDllPath [wokdep:SearchBin "$theCheckLib.dll" "$anArchIter" "$aPath/bin"]
          if { "$aDllPath" != "" } {
            lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
            set hasDll true
            break
          } else {
            set aDllPath [wokdep:SearchBin "$theCheckLib.dll" "$anArchIter" "$aPath/lib"]
            if { "$aDllPath" != "" } {
              lappend ::CSF_OPT_BIN$anArchIter "$aPath/lib"
              set hasDll true
              break
            }
          }
        }
        if { !$hasDll } {
          lappend anErrBin$anArchIter "Error: '$theCheckLib.dll' not found ($theName)"
          if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
        }
      }
    }
  }

  return "$isFound"
}

# Search Tcl/Tk libraries placement
proc wokdep:SearchTclTk {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set tclver_maj 8
  set tclver_min 6
  
  set isFound "true"
  set aTclHPath [wokdep:SearchHeader "tcl.h"]
  set aTkHPath  [wokdep:SearchHeader "tk.h"]
  if { "$aTclHPath" == "" || "$aTkHPath" == "" } {
    if { [file exists "/usr/include/tcl8.6/tcl.h"]
      && [file exists "/usr/include/tcl8.6/tk.h" ] } {
      lappend ::CSF_OPT_INC "/usr/include/tcl8.6"
      set aTclHPath "/usr/include/tcl8.6/tcl.h"
    } else {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{tcl}*] "$::VCVER" "$::ARCH" ]
      if { "$aPath" != "" && [file exists "$aPath/include/tcl.h"] && [file exists "$aPath/include/tk.h"] } {
        lappend ::CSF_OPT_INC "$aPath/include"
        set aTclHPath "$aPath/include/tcl.h"
      } else {
        lappend anErrInc "Error: 'tcl.h' or 'tk.h' not found (Tcl/Tk)"
        set isFound "false"
      }
    }
  }

  # detect tcl version by parsing header file
  if { $isFound } {
    set fh [open $aTclHPath]
    set tcl_h [read $fh]
    close $fh
    regexp {define\s+TCL_MAJOR_VERSION\s+([0-9]+)} $tcl_h dummy tclver_maj
    regexp {define\s+TCL_MINOR_VERSION\s+([0-9]+)} $tcl_h dummy tclver_min
  }

  if { "$::tcl_platform(platform)" == "windows" } {
    set aTclLibName "tcl${tclver_maj}${tclver_min}"
    set aTkLibName  "tk${tclver_maj}${tclver_min}"
  } else {
    set aTclLibName "tcl${tclver_maj}.${tclver_min}"
    set aTkLibName  "tk${tclver_maj}.${tclver_min}"
  }

  foreach anArchIter {64 32} {
    set aTclLibPath [wokdep:SearchLib "$aTclLibName" "$anArchIter"]
    set aTkLibPath  [wokdep:SearchLib "$aTkLibName"  "$anArchIter"]
    if { "$aTclLibPath" == "" || "$aTkLibPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{tcl}*] "$::VCVER" "$anArchIter" ]
      set aTclLibPath [wokdep:SearchLib "$aTclLibName" "$anArchIter" "$aPath/lib"]
      set aTkLibPath  [wokdep:SearchLib "$aTkLibName"  "$anArchIter" "$aPath/lib"]
      if { "$aTclLibPath" != "" && "$aTkLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
      } else {
        lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}${aTclLibName}.${::SYS_LIB_SUFFIX}' or '${::SYS_LIB_PREFIX}${aTkLibName}.${::SYS_LIB_SUFFIX}' not found (Tcl/Tk)"
        if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
      }
    }

    if { "$::tcl_platform(platform)" == "windows" } {
      set aTclDllPath [wokdep:SearchBin "${aTclLibName}.dll" "$anArchIter"]
      set aTkDllPath  [wokdep:SearchBin "${aTkLibName}.dll"  "$anArchIter"]
      if { "$aTclDllPath" == "" || "$aTkDllPath" == "" } {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{tcl}*] "$::VCVER" "$anArchIter" ]
        set aTclDllPath [wokdep:SearchBin "${aTclLibName}.dll" "$anArchIter" "$aPath/bin"]
        set aTkDllPath  [wokdep:SearchBin "${aTkLibName}.dll"  "$anArchIter" "$aPath/bin"]
        if { "$aTclDllPath" != "" && "$aTkDllPath" != "" } {
          lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
        } else {
          lappend anErrBin$anArchIter "Error: '${aTclLibName}.dll' or '${aTkLibName}.dll' not found (Tcl/Tk)"
          if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
        }
      }
    }
  }

  return "$isFound"
}

# Search FreeType library placement
proc wokdep:SearchFreeType {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  set aFtBuildPath [wokdep:SearchHeader "ft2build.h"]

  if { "$aFtBuildPath" == "" } {
    # TODO - use `freetype-config --cflags` instead
    set aSysFreeType "/usr/include/freetype2"
    if { [file exists "$aSysFreeType/ft2build.h"] } {
      lappend ::CSF_OPT_INC "$aSysFreeType"
    } elseif { [file exists "$aSysFreeType/freetype2/ft2build.h"] } {
      lappend ::CSF_OPT_INC "$aSysFreeType/freetype2"
    } else {
      set aSysFreeType "/usr/X11/include/freetype2"
      if { [file exists "$aSysFreeType/ft2build.h"] } {
        lappend ::CSF_OPT_INC "/usr/X11/include"
        lappend ::CSF_OPT_INC "$aSysFreeType"
      } else {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{freetype}*] "$::VCVER" "$::ARCH" ]
        if {"$aPath" != ""} {
          if {[file exists "$aPath/include/ft2build.h"]} {
            lappend ::CSF_OPT_INC "$aPath/include"
          } elseif {[file exists "$aPath/include/freetype2/ft2build.h"]} {
            lappend ::CSF_OPT_INC "$aPath/include/freetype2"
          }
        } else {
          lappend anErrInc "Error: 'freetype.h' not found (FreeType2)"
          set isFound "false"
        }
      }
    }
  }

  # parse 'freetype-config --libs'
  set aConfLibPath ""
  if { [catch { set aConfLibs [exec freetype-config --libs] } ] == 0 } {
    foreach aPath [split $aConfLibs " "] {
      if { [string first "-L" "$aPath"] == 0 } {
        set aConfLibPath [string range "$aPath" 2 [string length "$aPath"]]
      }
    }
  }

  foreach anArchIter {64 32} {
    set aFtLibPath [wokdep:SearchLib "freetype" "$anArchIter"]
    if { "$aFtLibPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{freetype}*] "$::VCVER" "$anArchIter" ]
      set aFtLibPath [wokdep:SearchLib "freetype" "$anArchIter" "$aPath/lib"]
      if { "$aFtLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
      } else {
        set aFtLibPath [wokdep:SearchLib "freetype" "$anArchIter" "$aConfLibPath"]
        if { "$aFtLibPath" != "" } {
          lappend ::CSF_OPT_LIB$anArchIter "$aConfLibPath"
        } else {
          lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}freetype.${::SYS_LIB_SUFFIX}' not found (FreeType2)"
          if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
        }
      }
    }
    if { "$::tcl_platform(platform)" == "windows" } {
      set aFtDllPath [wokdep:SearchBin "freetype.dll" "$anArchIter"]
      if { "$aFtDllPath" == "" } {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{freetype}*] "$::VCVER" "$anArchIter" ]
        set aFtDllPath [wokdep:SearchBin "freetype.dll" "$anArchIter" "$aPath/bin"]
        if { "$aFtDllPath" != "" } {
          lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
        } else {
          set aFtDllPath [wokdep:SearchBin "freetype.dll" "$anArchIter" "$aPath/lib"]
          if { "$aFtDllPath" != "" } {
            lappend ::CSF_OPT_BIN$anArchIter "$aPath/lib"
          } else {
            lappend anErrBin$anArchIter "Error: 'freetype.dll' not found (FreeType2)"
            if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
          }
        }
      }
    }
  }

  return "$isFound"
}

# Search FreeImage library placement
proc wokdep:SearchFreeImage {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  # binary distribution has another layout
  set aFImageDist     "Dist"

  set isFound "true"
  set aFImageHPath [wokdep:SearchHeader "FreeImage.h"]
  if { "$aFImageHPath" == "" } {
    set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{freeimage}*] "$::VCVER" "$::ARCH" ]
    if { "$aPath" != "" && [file exists "$aPath/include/FreeImage.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
    } elseif { "$aPath" != "" && [file exists "$aPath/$aFImageDist/FreeImage.h"] } {
      lappend ::CSF_OPT_INC "$aPath/$aFImageDist"
    } else {
      lappend anErrInc "Error: 'FreeImage.h' not found (FreeImage)"
      set isFound "false"
    }
  }

  foreach anArchIter {64 32} {
    set aFImageLibPath [wokdep:SearchLib "freeimage"     "$anArchIter"]
    if { "$aFImageLibPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{freeimage}*] "$::VCVER" "$anArchIter" ]
      set aFImageLibPath [wokdep:SearchLib "freeimage" "$anArchIter" "$aPath/lib"]
      if { "$aFImageLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
      } else {
        set aFImageLibPath [wokdep:SearchLib "freeimage" "$anArchIter" "$aPath/$aFImageDist"]
        if { "$aFImageLibPath" != "" } {
          lappend ::CSF_OPT_LIB$anArchIter "$aPath/$aFImageDist"
        } else {
          lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}freeimage.${::SYS_LIB_SUFFIX}' not found (FreeImage)"
          if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
        }
      }
    }
    if { "$::tcl_platform(platform)" == "windows" } {
      set aFImageDllPath [wokdep:SearchBin "freeimage.dll" "$anArchIter"]
      if { "$aFImageDllPath" == "" } {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{freeimage}*] "$::VCVER" "$anArchIter" ]
        set aFImageDllPath [wokdep:SearchBin "freeimage.dll" "$anArchIter" "$aPath/bin"]
        if { "$aFImageDllPath" != "" } {
          lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
        } else {
          set aFImageDllPath [wokdep:SearchBin "freeimage.dll" "$anArchIter" "$aPath/$aFImageDist"]
          if { "$aFImageDllPath" != "" } {
            lappend ::CSF_OPT_BIN$anArchIter "$aPath/$aFImageDist"
          } else {
            lappend anErrBin$anArchIter "Error: 'freeimage.dll' is not found (FreeImage)"
            if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
          }
        }
      }
    }
  }

  return "$isFound"
}

# Search FFmpeg framework placement
proc wokdep:SearchFFmpeg {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  set aFFmpegHPath [wokdep:SearchHeader "libavutil/avutil.h"]
  if { "$aFFmpegHPath"  == "" } {
    set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{ffmpeg}*] "$::VCVER" "$::ARCH" ]
    if { "$aPath" != "" && [file exists "$aPath/include/libavutil/avutil.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
    } else {
      lappend anErrInc "Error: 'libavutil/avutil.h' not found (FFmpeg)"
      set isFound "false"
    }
  }

  foreach anArchIter {64 32} {
    set aFFmpegLibPath [wokdep:SearchLib "avutil" "$anArchIter"]
    if { "$aFFmpegLibPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{ffmpeg}*] "$::VCVER" "$anArchIter" ]
      set aFFmpegLibPath [wokdep:SearchLib "avutil" "$anArchIter" "$aPath/lib"]
      if { "$aFFmpegLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
        lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
      } else {
        lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}avutil.${::SYS_LIB_SUFFIX}' not found (FFmpeg)"
        if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
      }
    }
  }

  return "$isFound"
}

# Search OpenVR SDK placement
proc wokdep:SearchOpenVR {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  set anOpenVrHPath [wokdep:SearchHeader "openvr.h"]
  if { "$anOpenVrHPath"  == "" } {
    set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{openvr}*] "$::VCVER" "$::ARCH" ]
    if { "$aPath" != "" && [file exists "$aPath/include/openvr.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
    } elseif { "$aPath" != "" && [file exists "$aPath/headers/openvr.h"] } {
      lappend ::CSF_OPT_INC "$aPath/headers"
    } else {
      lappend anErrInc "Error: 'openvr.h' not found (OpenVR)"
      set isFound "false"
    }
  }

  set aPlatform "unknown"
  if { "$::tcl_platform(platform)" == "windows" } {
    set aPlatform "win"
  } elseif { "$::tcl_platform(os)" == "Darwin" } {
    set aPlatform "osx"
  } elseif { "$::tcl_platform(os)" == "Linux" } {
    set aPlatform "linux"
  }

  foreach anArchIter {64 32} {
    set anOpenVrLibPath [wokdep:SearchLib "openvr_api" "$anArchIter"]
    if { "$anOpenVrLibPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{openvr}*] "$::VCVER" "$anArchIter" ]
      set anOpenVrLibPath  [wokdep:SearchLib "openvr_api" "$anArchIter" "$aPath/lib/${aPlatform}${anArchIter}"]
      set anOpenVrLibPath2 [wokdep:SearchLib "openvr_api" "$anArchIter" "$aPath/lib"]
      if { "$anOpenVrLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib/${aPlatform}${anArchIter}"
        lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin/${aPlatform}${anArchIter}"
      } elseif { "$anOpenVrLibPath2" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
        lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
      } else {
        lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}openvr_api.${::SYS_LIB_SUFFIX}' not found (OpenVR)"
        if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
      }
    }
  }

  return "$isFound"
}

# Search TBB library placement
proc wokdep:SearchTBB {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  # keep only two first digits in "vc141"
  if { ! [regexp {^vc[0-9][0-9]} ${::VCVER} aVcLib] } {
    if { [regexp {^vclang} ${::VCVER}] } {
      set aVcLib vc14
    } else {
      set aVcLib ${::VCVER}
    }
  }

  set isFound "true"
  set aTbbHPath [wokdep:SearchHeader "tbb/scalable_allocator.h"]
  if { "$aTbbHPath"  == "" } {
    set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{tbb}*] "$::VCVER" "$::ARCH" ]
    if { "$aPath" != "" && [file exists "$aPath/include/tbb/scalable_allocator.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
    } else {
      lappend anErrInc "Error: 'tbb/scalable_allocator.h' not found (Intel TBB)"
      set isFound "false"
    }
  }

  foreach anArchIter {64 32} {
    set aSubDir "ia32"
    if { "$anArchIter" == "64"} {
      set aSubDir "intel64"
    }

    set aTbbLibPath [wokdep:SearchLib "tbb" "$anArchIter"]
    if { "$aTbbLibPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{tbb}*] $aVcLib "$anArchIter" ]
      set aTbbLibPath [wokdep:SearchLib "tbb" "$anArchIter" "$aPath/lib/$aSubDir/$aVcLib"]
      if { "$aTbbLibPath" == "" } {
        # Set the path to the TBB library for Linux
        if { "$::tcl_platform(platform)" != "windows" } {
          set aSubDir "$aSubDir/cc4.1.0_libc2.4_kernel2.6.16.21"
        }
        set aTbbLibPath [wokdep:SearchLib "tbb" "$anArchIter" "$aPath/lib/$aSubDir"]
        if { "$aTbbLibPath" != "" } {
          lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib/$aSubDir"
        }
      } else {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib/$aSubDir/$aVcLib"
      }
      if { "$aTbbLibPath" == "" } {
        lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}tbb.${::SYS_LIB_SUFFIX}' not found (Intel TBB)"
        if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
      }
    }
    if { "$::tcl_platform(platform)" == "windows" } {
      set aTbbDllPath [wokdep:SearchBin "tbb12.dll" "$anArchIter"]
      if { "$aTbbDllPath" == "" } {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{tbb}*] $aVcLib "$anArchIter" ]
        set aTbbDllPath [wokdep:SearchBin "tbb12.dll" "$anArchIter" "$aPath/bin/$aSubDir/$aVcLib"]
        if { "$aTbbDllPath" != "" } {
          lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin/$aSubDir/$aVcLib"
        } else {
          lappend anErrBin$anArchIter "Error: 'tbb12.dll' not found (Intel TBB)"
          if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
        }
      }
    }
  }

  return "$isFound"
}

# Search OpenCL library placement
proc wokdep:SearchOpenCL {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  if { "$::tcl_platform(os)" == "Darwin" } {
    # OpenCL framework available since Mac OS X 16
    return "$isFound"
  }

  set aCLHPath [wokdep:SearchHeader "CL/cl_gl.h"]
  if { "$aCLHPath"  == "" } {
    set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{OpenCL}*] "$::VCVER" "$::ARCH" ]
    if { "$aPath" != "" && [file exists "$aPath/include/CL/cl_gl.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
    } else {
      lappend anErrInc "Error: 'CL/cl_gl.h' not found (OpenCL)"
      set isFound "false"
    }
  }

  foreach anArchIter {64 32} {
    set aCLLibPath [wokdep:SearchLib "OpenCL" "$anArchIter"]
    if { "$aCLLibPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{OpenCL}*] "$::VCVER" "$anArchIter" ]
      set aCLLibPath [wokdep:SearchLib "OpenCL" "$anArchIter" "$aPath/lib"]
      if { "$aCLLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
      } else {
        lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}OpenCL.${::SYS_LIB_SUFFIX}' not found (OpenCL)"
        if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
      }
    }
  }

  return "$isFound"
}

# Search EGL library placement
proc wokdep:SearchEGL {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  set aHeaderPath [wokdep:SearchHeader "EGL/egl.h"]
  if { "$aHeaderPath"  == "" } {
    set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{EGL}*] "$::VCVER" "$::ARCH" ]
    if { "$aPath" == "" || ![file exists "$aPath/include/EGL/egl.h"] } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{angle}*] "$::VCVER" "$::ARCH" ]
    }

    if { "$aPath" != "" && [file exists "$aPath/include/EGL/egl.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
    } else {
      lappend anErrInc "Error: 'EGL/egl.h' not found (EGL)"
      set isFound "false"
    }
  }

  set aLibName "EGL"
  if { "$::tcl_platform(platform)" == "windows" } {
    # awkward exception
    set aLibName "libEGL"
  }

  foreach anArchIter {64 32} {
    set aLibPath [wokdep:SearchLib "$aLibName" "$anArchIter"]
    if { "$aLibPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{EGL}*] "$::VCVER" "$anArchIter" ]
      set aLibPath [wokdep:SearchLib "$aLibName" "$anArchIter" "$aPath/lib"]
      if { "$aLibPath" == "" } {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{angle}*] "$::VCVER" "$anArchIter" ]
        set aLibPath [wokdep:SearchLib "$aLibName" "$anArchIter" "$aPath/lib"]
      }

      if { "$aLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
      } else {
        lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}${aLibName}.${::SYS_LIB_SUFFIX}' not found (EGL)"
        if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
      }
    }

    if { "$::tcl_platform(platform)" == "windows" } {
      set aDllPath [wokdep:SearchBin "libEGL.dll" "$anArchIter"]
      if { "$aDllPath" == "" } {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{EGL}*] "$::VCVER" "$anArchIter" ]
        set aDllPath [wokdep:SearchBin "libEGL.dll" "$anArchIter" "$aPath/bin"]
        if { "$aDllPath" == "" } {
          set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{angle}*] "$::VCVER" "$anArchIter" ]
          set aDllPath [wokdep:SearchBin "libEGL.dll" "$anArchIter" "$aPath/bin"]
        }

        if { "$aDllPath" != "" } {
          lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
        } else {
          lappend anErrBin$anArchIter "Error: 'libEGL.dll' not found (EGL)"
          if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
        }
      }
    }
  }

  return "$isFound"
}

# Search OpenGL ES 2.0 library placement
proc wokdep:SearchGLES {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  set aHeaderPath [wokdep:SearchHeader "GLES2/gl2.h"]
  if { "$aHeaderPath"  == "" } {
    set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{GLES}*] "$::VCVER" "$::ARCH" ]
    if { "$aPath" == "" || ![file exists "$aPath/include/GLES2/gl2.h"] } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{angle}*] "$::VCVER" "$::ARCH" ]
    }

    if { "$aPath" != "" && [file exists "$aPath/include/GLES2/gl2.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
    } else {
      lappend anErrInc "Error: 'GLES2/gl2.h' not found (OpenGL ES 2.0)"
      set isFound "false"
    }
  }

  set aLibName "GLESv2"
  if { "$::tcl_platform(platform)" == "windows" } {
    # awkward exception
    set aLibName "libGLESv2"
  }

  foreach anArchIter {64 32} {
    set aLibPath [wokdep:SearchLib "$aLibName" "$anArchIter"]
    if { "$aLibPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{GLES}*] "$::VCVER" "$anArchIter" ]
      set aLibPath [wokdep:SearchLib "$aLibName" "$anArchIter" "$aPath/lib"]
      if { "$aLibPath" == "" } {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{angle}*] "$::VCVER" "$anArchIter" ]
        set aLibPath [wokdep:SearchLib "$aLibName" "$anArchIter" "$aPath/lib"]
      }

      if { "$aLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
      } else {
        lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}${aLibName}.${::SYS_LIB_SUFFIX}' not found (OpenGL ES 2.0)"
        if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
      }
    }

    if { "$::tcl_platform(platform)" == "windows" } {
      set aDllPath [wokdep:SearchBin "libGLESv2.dll" "$anArchIter"]
      if { "$aDllPath" == "" } {
        set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{EGL}*] "$::VCVER" "$anArchIter" ]
        set aDllPath [wokdep:SearchBin "libGLESv2.dll" "$anArchIter" "$aPath/bin"]
        if { "$aDllPath" == "" } {
          set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{angle}*] "$::VCVER" "$anArchIter" ]
          set aDllPath [wokdep:SearchBin "libGLESv2.dll" "$anArchIter" "$aPath/bin"]
        }

        if { "$aDllPath" != "" } {
          lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
        } else {
          lappend anErrBin$anArchIter "Error: 'libGLESv2.dll' not found (OpenGL ES 2.0)"
          if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
        }
      }
    }
  }

  return "$isFound"
}

# Search RapidJSON headers
proc wokdep:SearchRapidJson {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc anErrInc

  set isFound "true"
  set aRJHPath [wokdep:SearchHeader "rapidjson/rapidjson.h"]
  if { "$aRJHPath"  == "" } {
    set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{rapidjson}*] "$::VCVER" "$::ARCH" ]
    if { "$aPath" != "" && [file exists "$aPath/include/rapidjson/rapidjson.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
    } else {
      lappend anErrInc "Error: 'rapidjson/rapidjson.h' not found (RapidJSON)"
      set isFound "false"
    }
  }

  return "$isFound"
}

# Auxiliary function, gets VTK version to set default search directory
proc wokdep:VtkVersion { thePath } {
  set aResult "6.1"

  set aVtkRoot [lindex [regexp -all -inline {[0-9.]*} [file tail $thePath]] 0]
  if { "$aVtkRoot" != "" } {
    set aVtkRoot [regexp -inline {[0-9]*.[0-9]*} $aVtkRoot]
    if { "$aVtkRoot" != "" } {
    set aResult $aVtkRoot
    }
  }

  return $aResult
}

# Search VTK library placement
proc wokdep:SearchVTK {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  
  set aVtkPath ""
  set aVtkIncPath [wokdep:SearchHeader "vtkConfigure.h"]
  set aVtkVer [wokdep:VtkVersion $aVtkIncPath]
  if { "$aVtkIncPath" == ""} {
    set aPathList [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{VTK}*]
    set aVtkPath [wokdep:Preferred "$aPathList" "$::VCVER" "$::ARCH" ]
    if { "$aVtkPath" != "" && [file exists "$aVtkPath/include/vtk-[wokdep:VtkVersion $aVtkPath]/vtkConfigure.h"]} { 
      set aVtkVer [wokdep:VtkVersion $aVtkPath]
      lappend ::CSF_OPT_INC "$aVtkPath/include/vtk-[wokdep:VtkVersion $aVtkPath]"
    } else { # try to search in all found paths
      set isFound "false"
      foreach anIt $aPathList {
        if { [file exists "$anIt/include/vtk-[wokdep:VtkVersion $anIt]/vtkConfigure.h"] } {
          set aVtkPath $anIt
          set aVtkVer [wokdep:VtkVersion $aVtkPath]
          lappend ::CSF_OPT_INC "$anIt/include/vtk-[wokdep:VtkVersion $anIt]"
          set isFound "true"
          break
        }
      }

      # Bad case: we do not found vtkConfigure.h in all paths.
      if { "$isFound" == "false"} {
        lappend anErrInc "Error: 'vtkConfigure.h' not found (VTK)"
        set isFound "false"
      }
    }
  }

  set aVtkLibPath ""
  foreach anArchIter {64 32} {
    set aVtkLibPath [wokdep:SearchLib "vtkCommonCore-$aVtkVer" "$anArchIter"]
    if { "$aVtkLibPath" == "" } {
      set aPathList [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{VTK}*]
      set aPath [wokdep:Preferred $aPathList "$::VCVER" "$anArchIter" ]
      set aVtkLibPath [wokdep:SearchLib "vtkCommonCore-$aVtkVer" "$anArchIter" "$aPath/lib"]
      if { "$aVtkLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
      } else {
        set aPath [wokdep:SearchLib "vtkCommonCore-$aVtkVer" "$anArchIter" "$aVtkPath/lib"]
        if { "$aPath" != "" } {
          set aLibPath $aVtkIncPath
          lappend ::CSF_OPT_LIB$anArchIter "$aLibPath/lib"
        } else {
          # The last chance: search /lib directory in all found paths
          foreach anIt $aPathList {
            set aVtkLibPath [wokdep:SearchLib "vtkCommonCore-$aVtkVer" "$anArchIter" "$anIt/lib"]
            if { "$aVtkLibPath" != ""} {
              lappend ::CSF_OPT_LIB$anArchIter "$anIt/lib"
              break
            }
          }
          if { "$aVtkLibPath" == "" } {
            lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}vtkCommonCore-${aVtkVer}\.${::SYS_LIB_SUFFIX}' not found (VTK)"
            if { "$::ARCH" == "$anArchIter" } {
              set isFound "false"
            }
          }
        }
      }
    }
  
    # Search binary path
    if { "$::tcl_platform(platform)" == "windows" } {
      set aVtkBinPath [wokdep:SearchBin "vtkCommonCore-${aVtkVer}.dll" "$anArchIter"]
      if { "$aVtkBinPath" == "" } {
        set aPathList [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{VTK}*]
        set aPath [wokdep:Preferred $aPathList "$::VCVER" "$anArchIter" ]
        set aVtkBinPath [wokdep:SearchBin "vtkCommonCore-${aVtkVer}.dll" "$anArchIter" "$aPath/bin"]
        if { "$aVtkBinPath" != "" } {
          lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
        } else {
          # Try to find in lib path
          set aVtkBinPath [wokdep:SearchBin "vtkCommonCore-${aVtkVer}.dll" "$anArchIter" "$aPath/lib"]
          if { "$aVtkBinPath" != "" } {
            lappend ::CSF_OPT_BIN$anArchIter "$aPath/lib"
          } else {
            # We didn't find preferred binary path => search through all available VTK directories
            foreach anIt $aPathList {
              set aVtkBinPath [wokdep:SearchBin "vtkCommonCore-${aVtkVer}.dll" "$anArchIter" "$anIt/bin"]
              if { "$aVtkBinPath" != "" } {
                lappend ::CSF_OPT_BIN$anArchIter "$anIt/bin"
                break
              } else {
                # Try to find in lib path
                set aVtkBinPath [wokdep:SearchBin "vtkCommonCore-${aVtkVer}.dll" "$anArchIter" "$anIt/lib"]
                if { "$aVtkBinPath" != "" } {
                  lappend ::CSF_OPT_BIN$anArchIter "$anIt/lib"
                }
              }
            }
            if { "$aVtkBinPath" == "" } {
              lappend anErrBin$anArchIter "Error: 'vtkCommonCore-${aVtkVer}.dll' not found (VTK)"
              set isFound "false"
            }
          }
        }
      }
    }
  }

  return "$isFound"
}

# Search Qt libraries placement
proc wokdep:SearchQt {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{qt}*] "$::VCVER" "$::ARCH" ] 
  set aQMsgBoxHPath [wokdep:SearchHeader "QtGui/qguiapplication.h"]
  if { "$aQMsgBoxHPath" == "" } {
    if { "$aPath" != "" && [file exists "$aPath/include/QtGui/qguiapplication.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
      lappend ::CSF_OPT_INC "$aPath/include/Qt"
      lappend ::CSF_OPT_INC "$aPath/include/QtGui"
      lappend ::CSF_OPT_INC "$aPath/include/QtCore"
      lappend ::CSF_OPT_INC "$aPath/include/QtWidgets"
      lappend ::CSF_OPT_INC "$aPath/include/QtXml"
    } else {
      lappend anErrInc "Error: 'QtGui/qguiapplication.h' not found"
        set isFound "false"
    }
  }

  set aQtGuiLibName "QtGui"
  if { "$::tcl_platform(platform)" == "windows" } {
    set aQtGuiLibName "Qt5Gui"
  }

  foreach anArchIter {64 32} {
    set aQMsgBoxLibPath [wokdep:SearchLib "${aQtGuiLibName}" "$anArchIter"]
    if { "$aQMsgBoxLibPath" == "" } {
      set aQMsgBoxLibPath [wokdep:SearchLib "${aQtGuiLibName}" "$anArchIter" "$aPath/lib"]
      if { "$aQMsgBoxLibPath" != "" } {
        lappend ::CSF_OPT_LIB$anArchIter "$aPath/lib"
      } else {
        lappend anErrLib$anArchIter "Error: '${::SYS_LIB_PREFIX}${aQtGuiLibName}.${::SYS_LIB_SUFFIX}' not found (Qt)"
        if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
      }
    }
    if { "$::tcl_platform(platform)" == "windows" } {
      set aQMsgBoxDllPath [wokdep:SearchBin "${aQtGuiLibName}.dll" "$anArchIter"]
      if { "$aQMsgBoxDllPath" == "" } {
        set aQMsgBoxDllPath [wokdep:SearchBin "${aQtGuiLibName}.dll" "$anArchIter" "$aPath/bin"]
        if { "$aQMsgBoxDllPath" != "" } {
          lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
        } else {
          lappend anErrBin$anArchIter "Error: '${aQtGuiLibName}.dll' not found (Qt)"
          if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
        }
      }
    }
  }

  return "$isFound"
}

# Search JDK placement
proc wokdep:SearchJDK {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  set aJniHPath   [wokdep:SearchHeader "jni.h"]
  set aJniMdHPath [wokdep:SearchHeader "jni_md.h"]
  if { "$aJniHPath" == "" || "$aJniMdHPath" == "" } {
    set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{jdk,java}*] "$::VCVER" "$::ARCH" ]
    if { "$aPath" != "" && [file exists "$aPath/include/jni.h"] } {
      lappend ::CSF_OPT_INC "$aPath/include"
      if { "$::tcl_platform(platform)" == "windows" } {
        lappend ::CSF_OPT_INC "$aPath/include/win32"
      } elseif { [file exists "$aPath/include/linux"] } {
        lappend ::CSF_OPT_INC "$aPath/include/linux"
      }
    } else {
      if { [file exists "/System/Library/Frameworks/JavaVM.framework/Home/include/jni.h"] } {
        lappend ::CSF_OPT_INC "/System/Library/Frameworks/JavaVM.framework/Home/include"
      } else {
        lappend anErrInc "Error: 'jni.h' or 'jni_md.h' not found (JDK)"
        set isFound "false"
      }
    }
  }

  foreach anArchIter {64 32} {
    set aJavacPath [wokdep:SearchBin "javac${::SYS_EXE_SUFFIX}" "$anArchIter"]
    if { "$aJavacPath" == "" } {
      set aPath [wokdep:Preferred [glob -nocomplain -directory "$::PRODUCTS_PATH" -type d *{jdk,java}*] "$::VCVER" "$anArchIter" ]
      set aJavacPath [wokdep:SearchBin "javac${::SYS_EXE_SUFFIX}" "$anArchIter" "$aPath/bin"]
      if { "$aJavacPath" != "" } {
        lappend ::CSF_OPT_BIN$anArchIter "$aPath/bin"
      } else {
        if { "$::ARCH" == "$anArchIter" && [file exists "/System/Library/Frameworks/JavaVM.framework/Home/bin/javac${::SYS_EXE_SUFFIX}"] } {
          lappend ::CSF_OPT_BIN$anArchIter "/System/Library/Frameworks/JavaVM.framework/Home/bin"
        } else {
          lappend anErrBin$anArchIter "Error: 'javac${::SYS_EXE_SUFFIX}' not found (JDK)"
          if { "$::ARCH" == "$anArchIter"} { set isFound "false" }
        }
      }
    }
  }

  return "$isFound"
}

# Search X11 libraries placement
proc wokdep:SearchX11 {theErrInc theErrLib32 theErrLib64 theErrBin32 theErrBin64} {
  upvar $theErrInc   anErrInc
  upvar $theErrLib32 anErrLib32
  upvar $theErrLib64 anErrLib64
  upvar $theErrBin32 anErrBin32
  upvar $theErrBin64 anErrBin64

  set isFound "true"
  if { "$::tcl_platform(platform)" == "windows" || ( "$::tcl_platform(os)" == "Darwin" && "$::HAVE_XLIB" != "true" ) } {
    return "$isFound"
  }

  set aX11LibPath [wokdep:SearchLib "X11" "$::ARCH"]
  if { "$aX11LibPath" == "" } {
    set aX11LibPath [wokdep:SearchLib "X11" "$::ARCH" "/usr/X11/lib"]
    if { "$aX11LibPath" != "" } {
      #lappend ::CSF_OPT_LIB$::ARCH "/usr/X11/lib"
    } else {
      lappend anErrLib$::ARCH "Error: '${::SYS_LIB_PREFIX}X11.${::SYS_LIB_SUFFIX}' not found (X11)"
      set isFound "false"
    }
  }

  return "$isFound"
}

# Returns OCCT version string from file Standard_Version.hxx (if available)
proc wokdep:DetectCasVersion {} {
  set occt_ver 7.0.0
  set aCasRoot [file normalize [file dirname [info script]]]
  set filename "${aCasRoot}/src/Standard/Standard_Version.hxx"
  if { [file exists $filename] } {
    set fh [open $filename "r"]
    set fh_loaded [read $fh]
    close $fh
    regexp {[^/]\s*#\s*define\s+OCC_VERSION_COMPLETE\s+\"([^\s]*)\"} $fh_loaded dummy occt_ver
  } else {
    puts "Error: file '$filename' not found"
  }
  return $occt_ver
}

# Generate (override) custom environment file
proc wokdep:SaveCustom {} {
  set aGenInfo "This environment file was generated by genconf.tcl script at [clock format [clock seconds] -format "%Y.%m.%d %H:%M"]"
  if { "$::tcl_platform(platform)" == "windows" } {
    set aCustomFilePath "./custom.bat"
    set aFile [open $aCustomFilePath "w"]
    puts $aFile "@echo off"
    puts $aFile "rem $aGenInfo"

    puts $aFile ""
    puts $aFile "set PRJFMT=$::PRJFMT"
    puts $aFile "set VCVER=$::VCVER"
    puts $aFile "set ARCH=$::ARCH"
    puts $aFile "set VCVARS=$::VCVARS"
    puts $aFile "set SHORTCUT_HEADERS=$::SHORTCUT_HEADERS"

    puts $aFile ""
    puts $aFile "set \"PRODUCTS_PATH=$::PRODUCTS_PATH\""

    puts $aFile ""
    puts $aFile "rem Optional 3rd-parties switches"
    foreach anEnvIter $::THE_ENV_VARIABLES {
      set aName ${anEnvIter}
      set aValue [set ::${anEnvIter}]
      if { "$aValue" != "" } {
        puts $aFile "set ${aName}=$aValue"
      }
    }

    set aStringInc [join $::CSF_OPT_INC $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringInc [regsub -all "$::PRODUCTS_PATH" $aStringInc "%PRODUCTS_PATH%"]
    }
    puts $aFile ""
    puts $aFile "rem Additional headers search paths"
    puts $aFile "set \"CSF_OPT_INC=$aStringInc\""

    set aStringLib32 [join $::CSF_OPT_LIB32 $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringLib32 [regsub -all "$::PRODUCTS_PATH" $aStringLib32 "%PRODUCTS_PATH%"]
    }
    puts $aFile ""
    puts $aFile "rem Additional libraries (32-bit) search paths"
    puts $aFile "set \"CSF_OPT_LIB32=$aStringLib32\""

    set aStringLib32d [join $::CSF_OPT_LIB32D $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringLib32d [regsub -all "$::PRODUCTS_PATH" $aStringLib32d "%PRODUCTS_PATH%"]
    }
    puts $aFile ""
    puts $aFile "rem Additional debug libraries (32-bit) search paths"
    if { "$aStringLib32d" != "" && "$aStringLib32" != "" } {
      puts $aFile "set \"CSF_OPT_LIB32D=$aStringLib32d;%CSF_OPT_LIB32%\""
    } else {
      puts $aFile "set \"CSF_OPT_LIB32D=$aStringLib32d\""
    }

    set aStringLib64 [join $::CSF_OPT_LIB64 $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringLib64 [regsub -all "$::PRODUCTS_PATH" $aStringLib64 "%PRODUCTS_PATH%"]
    }
    puts $aFile ""
    puts $aFile "rem Additional libraries (64-bit) search paths"
    puts $aFile "set \"CSF_OPT_LIB64=$aStringLib64\""

    set aStringLib64d [join $::CSF_OPT_LIB64D $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringLib64d [regsub -all "$::PRODUCTS_PATH" $aStringLib64d "%PRODUCTS_PATH%"]
    }
    puts $aFile ""
    puts $aFile "rem Additional debug libraries (64-bit) search paths"
    if { "$aStringLib64d" != "" && "$aStringLib64" != "" } {
      puts $aFile "set \"CSF_OPT_LIB64D=$aStringLib64d;%CSF_OPT_LIB64%\""
    } else {
      puts $aFile "set \"CSF_OPT_LIB64D=$aStringLib64d\""
    }

    set aStringBin32 [join $::CSF_OPT_BIN32 $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringBin32 [regsub -all "$::PRODUCTS_PATH" $aStringBin32 "%PRODUCTS_PATH%"]
    }
    puts $aFile ""
    puts $aFile "rem Additional (32-bit) search paths"
    puts $aFile "set \"CSF_OPT_BIN32=$aStringBin32\""

    set aStringBin32d [join $::CSF_OPT_BIN32D $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringBin32d [regsub -all "$::PRODUCTS_PATH" $aStringBin32d "%PRODUCTS_PATH%"]
    }
    puts $aFile ""
    puts $aFile "rem Additional debug (32-bit) search paths"
    if { "$aStringBin32d" != "" && "$aStringBin32" != "" } {
      puts $aFile "set \"CSF_OPT_BIN32D=$aStringBin32d;%CSF_OPT_BIN32%\""
    } else {
      puts $aFile "set \"CSF_OPT_BIN32D=$aStringBin32d\""
    }

    set aStringBin64 [join $::CSF_OPT_BIN64 $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringBin64 [regsub -all "$::PRODUCTS_PATH" $aStringBin64 "%PRODUCTS_PATH%"]
    }
    puts $aFile ""
    puts $aFile "rem Additional (64-bit) search paths"
    puts $aFile "set \"CSF_OPT_BIN64=$aStringBin64\""

    set aStringBin64d [join $::CSF_OPT_BIN64D $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringBin64d [regsub -all "$::PRODUCTS_PATH" $aStringBin64d "%PRODUCTS_PATH%"]
    }
    puts $aFile ""
    puts $aFile "rem Additional debug (64-bit) search paths"
    if { "$aStringBin64d" != "" && "$aStringBin64" != "" } {
      puts $aFile "set \"CSF_OPT_BIN64D=$aStringBin64d;%CSF_OPT_BIN64%\""
    } else {
      puts $aFile "set \"CSF_OPT_BIN64D=$aStringBin64d\""
    }

    close $aFile
  } else {
    set aCustomFilePath "./custom.sh"
    set aFile [open $aCustomFilePath "w"]
    puts $aFile "#!/bin/bash"
    puts $aFile "# $aGenInfo"

    puts $aFile ""
    puts $aFile "export PRJFMT=$::PRJFMT"
    puts $aFile "export ARCH=$::ARCH"
    puts $aFile "export SHORTCUT_HEADERS=$::SHORTCUT_HEADERS"

    puts $aFile ""
    puts $aFile "export PRODUCTS_PATH=\"$::PRODUCTS_PATH\""

    puts $aFile ""
    puts $aFile "# Optional 3rd-parties switches"
    foreach anEnvIter $::THE_ENV_VARIABLES {
      set aName ${anEnvIter}
      set aValue [set ::${anEnvIter}]
      if { "$aValue" != "" } {
        puts $aFile "export ${aName}=${aValue}"
      }
    }

    set aStringInc [join $::CSF_OPT_INC $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringInc [regsub -all "$::PRODUCTS_PATH" $aStringInc "\${PRODUCTS_PATH}"]
    }
    puts $aFile ""
    puts $aFile "# Additional headers search paths"
    puts $aFile "export CSF_OPT_INC=\"$aStringInc\""

    set aStringLib [join [set ::CSF_OPT_LIB$::ARCH] $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringLib [regsub -all "$::PRODUCTS_PATH" $aStringLib "\${PRODUCTS_PATH}"]
    }
    puts $aFile ""
    puts $aFile "# Additional libraries ($::ARCH-bit) search paths"
    puts $aFile "export CSF_OPT_LIB$::ARCH=\"[set aStringLib]\""

    set aStringLibD [join [set ::CSF_OPT_LIB${::ARCH}D] $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringLibD [regsub -all "$::PRODUCTS_PATH" $aStringLibD "\${PRODUCTS_PATH}"]
    }
    puts $aFile ""
    puts $aFile "# Additional debug libraries ($::ARCH-bit) search paths"
    if { "$aStringLibD" != "" && "$aStringLib" != "" } {
      puts $aFile "export CSF_OPT_LIB${::ARCH}D=\"[set aStringLibD]:\$CSF_OPT_LIB${::ARCH}\""
    } else {
      puts $aFile "export CSF_OPT_LIB${::ARCH}D=\"[set aStringLibD]\""
    }

    set aStringBin [join [set ::CSF_OPT_BIN$::ARCH] $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringBin [regsub -all "$::PRODUCTS_PATH" $aStringBin "\${PRODUCTS_PATH}"]
    }
    puts $aFile ""
    puts $aFile "# Additional ($::ARCH-bit) search paths"
    puts $aFile "export CSF_OPT_BIN$::ARCH=\"[set aStringBin]\""

    set aStringBinD [join [set ::CSF_OPT_BIN${::ARCH}D] $::SYS_PATH_SPLITTER]
    if { "$::PRODUCTS_PATH" != "" } {
      set aStringBinD [regsub -all "$::PRODUCTS_PATH" $aStringBinD "\${PRODUCTS_PATH}"]
    }
    puts $aFile ""
    puts $aFile "# Additional debug ($::ARCH-bit) search paths"
    if { "$aStringBinD" != "" && "$aStringBin" != "" } {
      puts $aFile "export CSF_OPT_BIN${::ARCH}D=\"[set aStringBinD]:\$CSF_OPT_BIN${::ARCH}\""
    } else {
      puts $aFile "export CSF_OPT_BIN${::ARCH}D=\"[set aStringBinD]\""
    }

    close $aFile
  }
  puts "Configuration saved to file '$aCustomFilePath'"

  # generate custom.auto.pri
  set toExportCustomPri 1
  if { $toExportCustomPri == 1 } {
    set aCasVer [wokdep:DetectCasVersion]
    set aCustomFilePath "./adm/qmake/custom.auto.pri"
    set aFile [open $aCustomFilePath "w"]
    puts $aFile "# $aGenInfo"

    puts $aFile ""
    puts $aFile "VERSION=$aCasVer"
    puts $aFile "PRODUCTS_PATH=\"$::PRODUCTS_PATH\""

    puts $aFile ""
    puts $aFile "# Optional 3rd-parties switches"
    foreach anEnvIter $::THE_ENV_VARIABLES {
      set aName ${anEnvIter}
      set aValue [set ::${anEnvIter}]
      if { "$aValue" == "true" } {
        puts $aFile "CONFIG += ${aName}"
      } else {
        #puts $aFile "CONFIG -= ${aName}"
      }
    }

    puts $aFile ""
    puts $aFile "# Additional headers search paths"
    foreach anIncPath $::CSF_OPT_INC {
      if { "$::PRODUCTS_PATH" != "" } {
        set anIncPath [regsub -all "$::PRODUCTS_PATH" $anIncPath "\$\$\{PRODUCTS_PATH\}"]
      }
      puts $aFile "INCLUDEPATH += \"${anIncPath}\""
    }

    puts $aFile ""
    puts $aFile "CONFIG(debug, debug|release) {"
    puts $aFile "  # Additional debug libraries search paths"
    foreach aLibPath [set ::CSF_OPT_LIB${::ARCH}D] {
      if { "$::PRODUCTS_PATH" != "" } {
        set aLibPath [regsub -all "$::PRODUCTS_PATH" $aLibPath "\$\$\{PRODUCTS_PATH\}"]
      }
      puts $aFile "  LIBS += -L\"${aLibPath}\""
    }
    if { "$::tcl_platform(platform)" == "windows" } {
      puts $aFile ""
      puts $aFile "  # Additional debug DLLs search paths"
      foreach aDllPath [set ::CSF_OPT_BIN${::ARCH}D] {
        if { "$::PRODUCTS_PATH" != "" } {
          set aDllPath [regsub -all "$::PRODUCTS_PATH" $aDllPath "\$\$\{PRODUCTS_PATH\}"]
        }
        puts $aFile "  LIBS += -L\"${aDllPath}\""
      }
    }
    puts $aFile "}"

    puts $aFile ""
    puts $aFile "# Additional libraries search paths"
    foreach aLibPath [set ::CSF_OPT_LIB$::ARCH] {
      if { "$::PRODUCTS_PATH" != "" } {
        set aLibPath [regsub -all "$::PRODUCTS_PATH" $aLibPath "\$\$\{PRODUCTS_PATH\}"]
      }
      puts $aFile "LIBS += -L\"${aLibPath}\""
    }

    if { "$::tcl_platform(platform)" == "windows" } {
      puts $aFile ""
      puts $aFile "# Additional DLLs search paths"
      foreach aDllPath [set ::CSF_OPT_BIN$::ARCH] {
        if { "$::PRODUCTS_PATH" != "" } {
          set aDllPath [regsub -all "$::PRODUCTS_PATH" $aDllPath "\$\$\{PRODUCTS_PATH\}"]
        }
        puts $aFile "LIBS += -L\"${aDllPath}\""
      }
    }

    puts $aFile ""
    close $aFile
    puts "Configuration saved to file '$aCustomFilePath'"
  }
}
