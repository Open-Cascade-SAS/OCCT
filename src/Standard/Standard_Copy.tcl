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

proc Standard_Copy:AdmFileType {} {
    return "dbadmfile";
}

proc Standard_Copy:OutputDirTypeName {} {
    return "dbtmpfile";
}


proc Standard_Copy:HandleInputFile { ID } { 

    scan $ID "%\[^:\]:%\[^:\]:%\[^:\]"  unit type name
	
    switch $name {
	Standard_Transient.hxx         {return 1;} 
	Standard_Persistent.hxx        {return 1;} 
	default {
	    return 0;
	}
    }
}

proc Standard_Copy:Execute { unit args } {
    
    msgprint -i -c "Standard_Copy::Execute" "Copy of Standard includes"

    foreach file  $args {
	scan $file "%\[^:\]:%\[^:\]:%\[^:\]"  Unit type name

	set source    [woklocate -p Standard:source:$name     [wokinfo -N $unit]]

        set vistarget [woklocate -p Standard:pubinclude:$name [wokinfo -N $unit]]
        set target    [wokinfo   -p pubinclude:$name          $unit]

	if { [catch {eval exec "cmp $source $vistarget"} ] } {
	    msgprint -i -c "Standard_Copy::Execute" "Copy $source to $target"
	    if { [file exist $target] && [wokparam -e %Station ] != "wnt" } {
		eval exec "chmod u+w $target" }
	    eval exec "cp -p $source $target"
	    if { [wokprofile -s] == "wnt" } {
		eval exec "attrib -r $target"
	    }
	} else {
	    msgprint -i -c "Standard_Copy::Execute" "No change in $source"
	}
    }
    return 0;
}
