


proc Standard_Copy:AdmFileType {} {
    return "dbadmfile";
}

proc Standard_Copy:OutputDirTypeName {} {
    return "dbtmpfile";
}


proc Standard_Copy:HandleInputFile { ID } { 

    scan $ID "%\[^:\]:%\[^:\]:%\[^:\]"  unit type name
	
    switch $name {
	Handle_Standard_Transient.hxx  {return 1;} 
	Handle_Standard_Persistent.hxx {return 1;} 
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


	if { $name == "Standard_Persistent.hxx" || $name == "Standard_Persistent_objy.ddl"} {
	    set source    [woklocate -p Standard:source:Standard_Persistent_proto.hxx     [wokinfo -N $unit]]
	} else {
	    set source    [woklocate -p Standard:source:$name     [wokinfo -N $unit]]
	}

	if { $name == "lctlprt.dll" || $name == "lctlprt.lib" || $name == "lmgr32.dll" || $name == "rtk.lib" || $name == "rtk.dll" } {
	    set vistarget [woklocate -p Standard:library:$name [wokinfo -N $unit]]
	    set target    [wokinfo   -p library:$name          $unit]
        } else {
	    set vistarget [woklocate -p Standard:pubinclude:$name [wokinfo -N $unit]]
	    set target    [wokinfo   -p pubinclude:$name          $unit]
        }

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
