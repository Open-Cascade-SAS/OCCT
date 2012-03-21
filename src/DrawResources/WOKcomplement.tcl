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

##########################################################
#
#  find the name of the station
#
proc wokstation {} {
    set LINE_FROM_UNAME [ exec uname -a ] ;
    if { [ regexp SunOS $LINE_FROM_UNAME ] } {
	return "sun"
    } 
    elseif { [ regexp IRIX $LINE_FROM_UNAME ] } {
	return "sil"
    }
    elseif { [ regexp OSF $LINE_FROM_UNAME ] } {
	return "ao1" 
    }
    elseif { [ regexp HP-UX $LINE_FROM_UNAME ] } {
	return "hp"
    }
    elseif { [ regexp FreeBSD $LINE_FROM_UNAME ] } {
	return "bsd"
    }
    elseif { [ regexp Darwin $LINE_FROM_UNAME ] } {
	return "mac"
    }
    else {
	return "unknown"
    }
    
}
