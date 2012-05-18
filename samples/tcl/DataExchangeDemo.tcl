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
if { [info exists env(CASROOT)] } {
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
