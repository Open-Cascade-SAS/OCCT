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
proc Draw:toolkits { } {
    set aResult [list TKDraw TKTopTest TKViewerTest TKXSDRAW TKDCAF TKXDEDRAW TKTObjDRAW TKQADraw]

    if { [info exists ::env(HAVE_VTK)] && $::env(HAVE_VTK) == "true" } {
      lappend aResult "TKIVtkDraw"
    }

    return $aResult
}

;#
;# Autres UDs a prendre. Listes de triplets
;# { ar typ UD str } Tous les types de UD vont dans un sous directory nomme root/str
;# Ils seront dans CAS3.0/str de l'archive de type ar (source/runtime)
;# { ar typ UD {}  } Tous les types de UD vont dans root/UD/src => CAS3.0/src
;#
proc Draw:ressources { } {
    return [list \
	    [list both r DrawResources {}] \
	    [list both x DRAWEXE {}] \
	    ]
}
proc Draw:freefiles { } {
    return {}
}
;#
;# Nom du module 
;#
proc Draw:name { } {
    return Draw
}
proc Draw:alias { } {
    return DRAW
}
proc Draw:depends { } {
    return [list DataExchange]
}

proc Draw:acdepends { } {
    return [list TCLTK]
}

;#
;# Returns a list of exported features.
;# source : Source files
;# runtime: Shareables
;# wokadm : WOK admin files
;# api    : Public include files
;#
proc Draw:Export { } {
    return [list source runtime wokadm api]
}
