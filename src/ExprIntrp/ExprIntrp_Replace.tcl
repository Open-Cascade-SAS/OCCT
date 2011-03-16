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



