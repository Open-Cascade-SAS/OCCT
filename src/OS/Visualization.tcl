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
  set aResult [list TKService TKV3d TKMeshVS]

  lappend aResult "TKOpenGl"
  if { [info exists ::env(HAVE_GLES2)] && "$::env(HAVE_GLES2)" == "true" } {
    lappend aResult "TKOpenGles"
  }

  if { [info exists ::env(HAVE_D3D)] } {
    if { "$::env(HAVE_D3D)" == "true" } {
      lappend aResult "TKD3DHost"
    }
  }

  if { [info exists ::env(HAVE_VTK)] && "$::env(HAVE_VTK)" == "true" } {
    lappend aResult "TKIVtk"
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
         [list both r XRResources {}] \
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
;#
proc Visualization:Export { } {
    return [list source runtime wokadm api]
}
