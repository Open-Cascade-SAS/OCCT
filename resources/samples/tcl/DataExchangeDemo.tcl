# Copyright (c) 1999-2014 OPEN CASCADE SAS
#
#Category: Demos
#Title: Import and export
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

# Command to log a message to both command-line and dialog window
proc sage { a} {
    if { ![winfo exists .h ] } {
        toplevel .h -bg azure3
        wm title .h "INFO TEST HARNESS"
        wm geometry .h +320+20
    } 
    if { [winfo exists .h.m ] } {
        set astring [.h.m cget  -text]
        set newstring "${astring} \n $a"
        .h.m configure -text $newstring 
        puts $a
    } else {
        message .h.m -justify left -bg azure2 -width 13c -relief ridge -bd 4 -text $a
        puts $a
    } 
    pack .h.m
    update
}

pload DATAEXCHANGE

smallview
if { [winfo exists .h ] } {
    destroy .h
}

set ddir .
if { [info exists env(CSF_OCCTDataPath)] } {
    set ddir [file join $env(CSF_OCCTDataPath) occ]
} elseif { [info exists env(CASROOT)] } {
    set ddir [file join $env(CASROOT) data occ]
}

set tdir .
if { [info exist env(TEMP)] } {
    set tdir $env(TEMP)
}

sage " First, we retrieve a BREP File "
sage "    restore $ddir/wing.brep wing"
sage " "
datadir .
restore $ddir/wing.brep wing
disp wing
fit

sage "Generate the IGES File of this BREP"
sage "   brepiges wing $tdir/wing.igs"
sage " "
brepiges wing $tdir/wing.igs
wait 3

sage "we delete all DRAW data"
sage ""
dall
fit
wait 3

sage "Restore this IGES File we have created " 
sage "   igesbrep $tdir/wing.igs new *"
sage ""
igesbrep $tdir/wing.igs new *
disp new
fit

puts "End IGES Elementary Test " 
sage " "
file delete $tdir/wing.igs
