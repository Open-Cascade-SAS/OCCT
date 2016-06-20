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
proc ApplicationFramework:toolkits { } {
    return [list \
                TKCDF \
                TKLCAF \
                TKVCAF \
                TKCAF \
                TKBinL \
                TKXmlL \
                TKBin \
                TKXml \
                TKStdL \
                TKStd \
                TKTObj \
                TKBinTObj \
                TKXmlTObj \
           ]
}
;#
;# Autres UDs a prendre.
;#
proc ApplicationFramework:ressources { } {
    return [list \
                [list both r StdResource {}] \
                [list both r XmlOcafResource {}] \
           ]
}
;#
;# Nom du module 
;#
proc ApplicationFramework:name { } {
    return ApplicationFramework
}
;#
;# Short Nom du module  ( 3 lettres )
;#
proc ApplicationFramework:alias { } {
    return CAF
}
proc ApplicationFramework:depends { } {
    return [list Visualization]
}

;#
;# Returns a list of exported features.
;# source : Source files
;# runtime: Shareables
;# wokadm : WOK admin files
;# api    : Public include files
;#
proc ApplicationFramework:Export { } {
    return [list source runtime wokadm api]
}
