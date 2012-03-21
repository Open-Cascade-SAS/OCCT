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

proc ExprIntrp_Replace:AdmFileType {} {
    return "dbadmfile";
}

proc ExprIntrp_Replace:OutputDirTypeName {} {
    return "dbtmpfile";
}


proc ExprIntrp_Replace:HandleInputFile { ID } { 

    scan $ID "%\[^:\]:%\[^:\]:%\[^:\]"  unit type name
    
    switch $name {
         ExprIntrp.tab.c  {return 1;} 
	default {
	    return 0;
	}
    }
}

proc ExprIntrp_Replace:Execute { unit args } {
    
    msgprint -i -c "ExprIntrp_Replace::Execute" "Copying of ExprIntrp includes  $unit $args "

    global tcl_interactive

    set tcl_interactive 1
    package require Wokutils


    if { [wokparam -e %Station $unit] != "wnt" } {
	set copycmd "cp -p "
	set replstr "/"
    } else {
	set copycmd "cmd /c copy"
	set replstr "\\\\\\\\"
    }
    

	set sourcename ExprIntrp.tab.c
	set name       ExprIntrp.tab.c

	set source    [woklocate -p ExprIntrp:source:$sourcename     [wokinfo -N $unit]]
	set vistarget [woklocate -p ExprIntrp:privinclude [wokinfo -N $unit]]$name
#	set target    [wokinfo   -p ExprIntrp:privinclude:$name [wokinfo -N $unit]]
msgprint -i -c "$source "
	regsub -all "/" " $source $vistarget" $replstr  TheArgs

	msgprint -i -c "ExprIntrp_Replace::Execute" "Copy $source to $vistarget"
	if { [file exist $vistarget] && [wokparam -e %Station ] != "wnt" } {
		eval exec "chmod u+w $vistarget"
	}
	eval exec "$copycmd $TheArgs"

    return 0;
}



