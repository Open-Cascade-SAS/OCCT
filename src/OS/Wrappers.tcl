;#
;# Liste des toolkits WOK sous forme de full path
;# 
proc Wrappers:toolkits { } {
    return [list TKjcas]
}
;#
;# Autres UDs a prendre.
;#
proc Wrappers:ressources { } {
    return {} \
}
;
;#
;# retourne une liste de triplets {type <full path1> <target directory>/name}
;# permet de faire : cp <full path> $CASROOT/<target directory>/name
;# On peut ainsi embarquer des fichiers d'un peu partout et les dispatcher sous 
;# la racine d'arrivee et ce avec un autre nom.
;# rien n'empeche de CALCULER automatiquement des paths dans cette proc.
;# type = source/runtime/both pour dire si le fichier va dans l'archive en question.
;# une deux (ou les deux) type d'archive fabriquees. 
;#
proc Wrappers:freefiles { } {}
;#
;# Nom du module 
;#
proc Wrappers:name { } {
    return Wrappers
}
proc Wrappers:alias { } {
    return FOC
}
proc Wrappers:depends { } {
    return {}
}
;#
;# Liste des includes utilises qui ne sont pas ceux des Wb.
;#
proc Wrappers:CompileWith { } {
    
    set l {}
    switch -- [OS:os] {
	HP-UX {
	}
	Linux {
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include"
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/inclide/linux"
	    lappend l "[lindex [wokparam -v %CSF_TCL_HOME] 0]/include"
            lappend l "[lindex [lindex [wokparam -v %STLPortInclude] 0] 0]"
	}
	SunOS {
	    lappend l "/usr/openwin/include"
	    lappend l "/usr/dt/include"
	    lappend l "[lindex [wokparam -v %CSF_CXX_INCLUDE] 0]"
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/include"
	    lappend l "-I[lindex [wokparam -v %CSF_JavaHome] 0]/inclide/solaris"
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
proc Wrappers:LinksoWith { } {
    
    set l {}
    switch -- [OS:os] {
	HP-UX {
	}
	Linux {
	}
	SunOS {
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
proc Wrappers:Export { } {
    return [list source runtime wokadm api]
}
