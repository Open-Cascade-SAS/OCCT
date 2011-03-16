;#
;# Liste des toolkits WOK sous forme de full path
;# 
proc ModelingAlgorithms:toolkits { } {
    return [list TKGeomAlgo \
		TKTopAlgo \
		TKPrim \
		TKBO \
		TKBool \
		TKHLR \
		TKFillet \
		TKOffset \
		TKFeat \
		TKMesh \
		TKXMesh \
		TKShHealing \
	       ]
}
;#
;# Autres UDs a prendre.
;#
proc ModelingAlgorithms:ressources { } {
    
}
;#
;# Nom du module 
;#
proc ModelingAlgorithms:name { } {
    return ModelingAlgorithms
}
proc ModelingAlgorithms:alias { } {
    return MLG
}
proc ModelingAlgorithms:depends { } {
    return [list ModelingData]
}
;#
;# Pre-requis pour la compilation ( -I ... )
;# Returns a list of directory that should be used in -I directives
;# while compiling c or c++ files.
;#
proc ModelingAlgorithms:CompileWith { } {
    
    set l {}
    switch -- [OS:os] {
	HP-UX {
	}
	Linux {
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include"
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include/solaris"
	    lappend l "[lindex [wokparam -v %CSF_TCL_HOME] 0]/include"
           lappend l "[lindex [lindex [wokparam -v %STLPortInclude] 0] 0]" 
	}
	SunOS {
	    lappend l "/usr/openwin/include"
	    lappend l "/usr/dt/include"
	    lappend l "[lindex [wokparam -v %CSF_CXX_INCLUDE] 0]"
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include"
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include/solaris"
	    lappend l "[lindex [wokparam -v %CSF_TCL_HOME] 0]/include"
	}
	IRIX {
	    lappend l "/usr/include/CC"
	}

    }
    return $l
}
;#
;# Pre-requis pour la compilation ( -L ... )
;# Returns a list of directory that should be used in -L directives
;# while creating shareable.
;#
proc ModelingAlgorithms:LinksoWith { } {
    
    set l {}
    switch -- [OS:os] {
	HP-UX {
	}
	Linux {
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
proc ModelingAlgorithms:Export { } {
    return [list source runtime wokadm api]
}
