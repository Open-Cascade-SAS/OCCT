
set stationname $tcl_platform(platform)
if { ${stationname} == "windows" } {
   proc winfo { aTest aWindow } { return False }
}


proc sage { a} {
    global stationname 
    if { ${stationname} != "windows" } {
	
	if { ![winfo exists .h ] } {
	    toplevel .h -bg azure3
	    wm title .h "INFO DATAEXCHANGE TEST HARNESS"
	    wm geometry .h +320+20
	    
	} 
	if { [winfo exists .h.m ] } {
	    set astring [.h.m cget  -text]
	    set newstring "${astring} \n $a"
	    .h.m configure -text $newstring 
	    puts $a
	} else {
	    message .h.m -justify left -bg azure2 -width 13c -relief ridge -bd 4\
		    -text $a
	    puts $a
	} 
	pack .h.m
	update
   }
}


smallview
if { [winfo exists .h ] } {
    destroy .h
}

if { [info exists env(CASROOT)] } {
    set thedir [file join $env(CASROOT) src DEResource]
    cd ${thedir}
}

datadir .
sage " First, we retrieve a BREP File "
sage "    datadir ."
sage "    restore wing.brep wing"
sage " "
datadir .
restore wing.brep wing
disp wing
fit
sage "Generate the IGES File of this BREP"
sage "   brepiges wing /tmp/wing.igs"
sage " "
brepiges wing /tmp/wing.igs
wait 3
sage "we delete all data"
sage ""
dall
sage "Restore this IGES File we have created " 
sage "   igesbrep wing.igs new *"
sage ""
cd /tmp
igesbrep wing.igs new *
disp new
fit
if { [winfo exists .h ] } {
    destroy .h
}
puts "End IGES Elementary Test " 
sage " "
unlink /tmp/wing.igs
