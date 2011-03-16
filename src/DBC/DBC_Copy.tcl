
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
