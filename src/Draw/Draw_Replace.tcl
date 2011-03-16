

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
