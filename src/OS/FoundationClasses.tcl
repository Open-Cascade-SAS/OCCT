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
proc FoundationClasses:toolkits { } {
    return [list TKernel TKMath]
}
;#
;# Autres UDs a prendre.
;#
proc FoundationClasses:ressources { } {
        return [list \
	       [list both r OS {} ] \
	       [list both r UnitsAPI {} ] \
	       ]
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
proc FoundationClasses:freefiles { } {}
;#
;# Nom du module 
;#
proc FoundationClasses:name { } {
    return FoundationClasses
}
proc FoundationClasses:alias { } {
    return FOC
}
proc FoundationClasses:depends { } {
    return {}
}

;#
;# Returns a list of exported features.
;# source : Source files
;# runtime: Shareables
;# wokadm : WOK admin files
;# api    : Public include files
;#
proc FoundationClasses:Export { } {
    return [list source runtime wokadm api]
}

proc FoundationClasses:install { } {
        return [list UnitsAPI]
}
