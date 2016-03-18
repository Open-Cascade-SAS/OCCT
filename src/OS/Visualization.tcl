# Copyright (c) 1999-2014 OPEN CASCADE SAS
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

;#
;# Liste des toolkits WOK sous forme de full path
;# 
proc Visualization:toolkits { } {
    set aResult [list TKService \
                     TKV3d \
                     TKOpenGl \
                     TKMeshVS]

    if { [info exists ::env(HAVE_VTK)] && "$::env(HAVE_VTK)" == "true" } {
      lappend aResult "TKIVtk"
    }

    if { "$::tcl_platform(platform)" == "windows" } {
      if { [info exists ::env(HAVE_D3D)] } {
        if { "$::env(HAVE_D3D)" == "true" } {
          lappend aResult "TKD3DHost"
        }
      } elseif { [info exists ::env(VCVER)] && "$::env(VCVER)" != "vc8" && "$::env(VCVER)" != "vc9" && "$::env(VCVER)" != "vc10" } {
        lappend aResult "TKD3DHost"
      }
    }

    return $aResult
}
;#
;# Autres UDs a prendre.
;#
proc Visualization:ressources { } {
    return [list \
	   [list both r Textures {}] \
	   [list both r Shaders {}] \
	    ]
}
;#
;# Nom du module 
;#
proc Visualization:name { } {
    return Visualization
}
proc Visualization:alias { } {
    return VIS
}
proc Visualization:depends { } {
    return [list ModelingAlgorithms]
}

proc Visualization:acdepends { } {
    set aList [list X11 GL FREETYPE]

    if { [info exists ::env(HAVE_VTK)] && "$::env(HAVE_VTK)" == "true" } {
      lappend aList "VTK"
    }

    return $aList
}

;#
;# Returns a list of exported features.
;# source : Source files
;# runtime: Shareables
;# wokadm : WOK admin files
;# api    : Public include files
;#
proc Visualization:Export { } {
    return [list source runtime wokadm api]
}
