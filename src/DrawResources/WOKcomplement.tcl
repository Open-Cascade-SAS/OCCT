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
