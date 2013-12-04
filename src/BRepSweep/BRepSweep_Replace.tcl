# Copyright (c) 1999-2014 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and / or modify it
# under the terms of the GNU Lesser General Public version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

proc BRepSweep_Replace:AdmFileType {} {
    return "dbadmfile";
}

proc BRepSweep_Replace:OutputDirTypeName {} {
    return "dbtmpfile";
}


proc BRepSweep_Replace:HandleInputFile { ID } { 

    scan $ID "%\[^:\]:%\[^:\]:%\[^:\]"  unit type name

    switch $name {
	BRepSweep_NumLinearRegularSweep.hxx {return 1;}
	default {
	    return 0;
	}
    }
}

proc BRepSweep_Replace:Execute { unit args } {
    
    global tcl_interactive

    set tcl_interactive 1
    package require Wokutils

    msgprint -i -c "BRepSweep_Replace:Execute" "Copying of BRepSweep includes"

    if { [wokparam -e %Station $unit] != "wnt" } {
	set copycmd "cp -p "
	set replstr "/"
    } {
	set copycmd "cmd /c copy"
	set replstr "\\\\\\\\"
    }
    
    foreach file  $args {
	scan $file "%\[^:\]:%\[^:\]:%\[^:\]"  Unit type name
	
	regsub ".hxx" $name ".hxx" sourcename

	set source    [woklocate -p BRepSweep:source:$sourcename     [wokinfo -N $unit]]
	set vistarget [woklocate -p BRepSweep:pubinclude:$name [wokinfo -N $unit]]
	set target    [wokinfo   -p pubinclude:$name          $unit]

	regsub -all "/" " $source $target" $replstr  TheArgs

	msgprint -i -c "BRepSweep_Replace:Execute" "Copy $source to $target"
	if { [file exist $target] && [wokparam -e %Station $unit] != "wnt" } {
		eval exec "chmod u+w $target"
	}
	eval exec "$copycmd $TheArgs"

    }
    return 0;
}
