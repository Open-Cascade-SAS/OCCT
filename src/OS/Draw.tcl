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
;# Pre-requis pour la compilation ( -I ... )
;# Returns a list of directory that should be used in -I directives
;# while compiling c or c++ files.
;#
proc Draw:CompileWith {} {
    
    set l {}
    switch -- [OS:os] {
	HP-UX {
	}
	Linux {
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include"
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include/linux"
	    lappend l "[lindex [wokparam -v %CSF_TCL_HOME] 0]/include"
            lappend l "[lindex [lindex [wokparam -v %STLPortInclude] 0] 0]" 
	}
	SunOS {
	    lappend l "/usr/openwin/include"
	    lappend l "/usr/dt/include"
	    lappend l [lindex [wokparam -v %CSF_CXX_INCLUDE] 0]
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include" 
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include/solaris"
	    lappend l "[lindex [wokparam -v %CSF_TCL_HOME] 0]/include"
	}
	IRIX {
	    lappend l /usr/include/CC
	    lappend l /usr/tcltk/include/itcl
	    lappend l /opt/Orbix_2.2/include
	}

    }
    return $l
}
;#
;# Pre-requis pour la compilation ( -L ... )
;# Returns a list of directory that should be used in -L directives
;# while creating shareable.
;#
proc Draw:LinksoWith {} {
    
    set l {}
    switch -- [OS:os] {
	HP-UX {
	}
	Linux {
	    lappend l -L/usr/X11R6/lib
            lappend l "-L[wokparam -v %CSF_TCL_HOME]/lib"
            lappend l "-L[wokparam -v %STLPortHome]/lib"
	}
	SunOS {
	    lappend l /usr/openwin/lib
	    lappend l "[wokparam -v %CSF_TCL_HOME]/lib"
	    lappend l /opt/DEV5_1/SUNWspro/SC4.2/include/CC
	}
	IRIX {
	    lappend l /usr/lib32 
	    lappend l /usr/tcltk.64/lib/itcl
	}

    }
    return $l
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
