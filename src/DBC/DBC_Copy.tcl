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


proc DBC_Copy:AdmFileType {} {
    return "dbadmfile";
}

proc DBC_Copy:OutputDirTypeName {} {
    return "dbtmpfile";
}


proc DBC_Copy:HandleInputFile { ID } { 

    scan $ID "%\[^:\]:%\[^:\]:%\[^:\]"  unit type name

    switch $name {
	DBC_BaseArray.hxx  {return 1;} 
	default {
	    return 0;
	}
    }
}

proc DBC_Copy:Execute { unit args } {
    
    msgprint -i -c "DBC_Copy:Execute" "Copy of DBC_BaseArray include"

    foreach file  $args {
	scan $file "%\[^:\]:%\[^:\]:%\[^:\]"  Unit type name

	if { $name == "DBC_BaseArray.hxx" } {
	    set source    [woklocate -p DBC:source:DBC_BaseArray_objs.hxx     [wokinfo -N $unit]]
	} else {
	    set source    [woklocate -p DBC:source:$name     [wokinfo -N $unit]]
	}

	set vistarget [woklocate -p DBC:pubinclude:$name [wokinfo -N $unit]]
	set target    [wokinfo   -p pubinclude:$name     $unit]

	if { [catch {eval exec "cmp $source $vistarget"} ] } {
	    msgprint -i -c "DBC_Copy:Execute" "Copy $source to $target"
	    eval exec "cp -p $source $target"
	} else {
	    msgprint -i -c "DBC_Copy:Execute" "No change in $source"
	}
    }
    return 0;
}
