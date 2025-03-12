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

################################################
#
#  find the name of the station
#
################################################
proc wokstation {} {
    set LINE_FROM_UNAME [ exec uname -a ] ;
    if { [ regexp SunOS $LINE_FROM_UNAME ] } {
	return "sun"
    }  elseif { [ regexp IRIX $LINE_FROM_UNAME ] } {
	return "sil"
    } elseif { [ regexp OSF $LINE_FROM_UNAME ] } {
	return "ao1" 
    } elseif { [ regexp HP-UX $LINE_FROM_UNAME ] } {
	return "hp"
    } elseif { [ regexp Linux $LINE_FROM_UNAME ] } {
	return "lin"
    } elseif { [ regexp FreeBSD $LINE_FROM_UNAME ] } {
	return "bsd"
    } elseif { [ regexp Darwin $LINE_FROM_UNAME ] } {
	return "mac"
    } elseif { [ regexp AIX $LINE_FROM_UNAME ] } {
	return "aix"
    } else {
	return "wnt"
    }
    
}

set env(STATION)     [ wokstation ] 

# PMN LE 6/06/1997
# Ce type d'environnement n'a rien n'a faire dans les sources
# On doit le faire dans des ilots.tcl ou des ud.tcl ou bien
# dans les procedures de test externes a WOK
#set env(WBCONTAINER) /adv_20/BAG
#set env(WBROOT)     "/adv_21/MDL/k1deb/ref"
