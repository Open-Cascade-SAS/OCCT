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



proc Draw_Replace:AdmFileType {} {
    return "dbadmfile";
}

proc Draw_Replace:OutputDirTypeName {} {
    return "dbtmpfile";
}


proc Draw_Replace:HandleInputFile { ID } { 

    scan $ID "%\[^:\]:%\[^:\]:%\[^:\]"  unit type name

    switch $name {
	Draw_Interpretor.hxx {return 1;}
	default {
	    return 0;
	}
    }
}

proc Draw_Replace:Execute { unit args } {
    
    global tcl_interactive

    set tcl_interactive 1
    package require Wokutils

    msgprint -i -c "Draw_Replace:Execute" "Copying of Draw includes"

    if { [wokparam -e %Station $unit] != "wnt" } {
	set copycmd "cp -p "
	set replstr "/"
    } {
	set copycmd "cmd /c copy"
	set replstr "\\\\\\\\"
    }
    
    foreach file  $args {
	scan $file "%\[^:\]:%\[^:\]:%\[^:\]"  Unit type name
	
	regsub ".hxx" $name "_proto.hxx" sourcename

	set source    [woklocate -p Draw:source:$sourcename     [wokinfo -N $unit]]
	set vistarget [woklocate -p Draw:pubinclude:$name [wokinfo -N $unit]]
	set target    [wokinfo   -p pubinclude:$name          $unit]

	regsub -all "/" " $source $target" $replstr  TheArgs

	msgprint -i -c "Draw_Replace:Execute" "Copy $source to $target"
	if { [file exist $target] && [wokparam -e %Station] != "wnt" } {
		eval exec "chmod u+w $target"
	}
	eval exec "$copycmd $TheArgs"

    }
    return 0;
}
