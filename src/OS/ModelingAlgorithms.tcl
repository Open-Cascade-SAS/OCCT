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
    return ALGO
}
proc ModelingAlgorithms:depends { } {
    return [list ModelingData]
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
