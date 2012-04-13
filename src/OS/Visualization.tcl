# Copyright (c) 1999-2012 OPEN CASCADE SAS
#
# The content of this file is subject to the Open CASCADE Technology Public
# License Version 6.5 (the "License"). You may not use the content of this file
# except in compliance with the License. Please obtain a copy of the License
# at http://www.opencascade.org and read it completely before using this file.
#
# The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
# main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
#
# The Original Code and all software distributed under the License is
# distributed on an "AS IS" basis, without warranty of any kind, and the
# Initial Developer hereby disclaims all such warranties, including without
# limitation, any warranties of merchantability, fitness for a particular
# purpose or non-infringement. Please see the License for the specific terms
# and conditions governing the rights and limitations under the License.

;#
;# Liste des toolkits WOK sous forme de full path
;# 
proc Visualization:toolkits { } {
    return [list TKService \
		 TKV2d \
		 TKV3d \
		 TKOpenGl \
                 TKMeshVS \
                 TKNIS \
                 TKVoxel \
	    ]
}
;#
;# Autres UDs a prendre.
;#
proc Visualization:ressources { } {
    return [list \
	   [list both r FontMFT {}] \
	   [list both r Textures {}] \
	   [list both n VoxelClient {}] \
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
    return [list X11 GL FREETYPE FTGL]
}

;#
;# Pre-requis pour la compilation ( -I ... )
;# Returns a list of directory that should be used in -I directives
;# while compiling c or c++ files.
;#
proc Visualization:CompileWith {} {
    
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
	    lappend l "\$OPENWINHOME/include"
	    lappend l "/usr/openwin/include/X11"
	    lappend l "/usr/dt/include"
	    lappend l "[lindex [wokparam -v %CSF_CXX_INCLUDE] 0]"
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include"
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include/solaris"
	    lappend l "[lindex [wokparam -v %CSF_TCL_HOME] 0]/include"
	}
	IRIX {
	    lappend l /usr/include/CC
	}

    }
    return $l
}
;#
;# Pre-requis pour la compilation ( -L ... )
;# Returns a list of directory that should be used in -L directives
;# while creating shareable.
;#
proc Visualization:LinksoWith { } {
    
    set l {}
    switch -- [OS:os] {
	HP-UX {
	}
	Linux {
	    lappend l /usr/X11R6/lib
	}
	SunOS {
	    lappend l "-L[wokparam -v %CSF_TCL_HOME]/lib -R[wokparam -v %CSF_TCL_HOME]/lib -ltcl"
	    lappend l "-L[wokparam -v %CSF_TCL_HOME]/lib -R[wokparam -v %CSF_TCL_HOME]/lib -ltk"
	    lappend l /usr/openwin/lib
	}
	IRIX {
	    lappend l /usr/lib32 
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
proc Visualization:Export { } {
    return [list source runtime wokadm api]
}
