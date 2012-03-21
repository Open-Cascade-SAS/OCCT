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


set stationname $tcl_platform(platform)
if { ${stationname} == "windows" } {
   proc winfo { aTest aWindow } { return False }
}


proc sage { a} {
    global stationname 
    if { ${stationname} != "windows" } {
	
	if { ![winfo exists .h ] } {
	    toplevel .h -bg azure3
	    wm title .h "INFO AISViewer"
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

if { [winfo exists .h ] } {
    destroy .h
}

sage " Creating the V3D Viewer"
sage "    vinit"
sage " " 
vinit
vclear
clear

sage " Creating the Topological Viewer"
sage "    smallview"
sage " " 
smallview
sage " Creating a box"
sage "    box  b -10 -10 -10 20 20 20"
sage " "
box b -10 -10 -10 20 20 20
fit
sage " Display the box in the V3D Viewer"
sage "    vdisplay b"
sage " "
vdisplay b
vfit
nexplode b f
erase b
sage " Draft of two lateral faces "
sage "    nexplode b f"
sage "    depouille r b  0 0  -1 b_6 -15 10 -10 10  0 0 -1 "
sage "    nexplode r f"
sage "    depouille rr r  0 0 -1   r_1 -15 -10 -10 10  0 0 -1 "
sage " "
depouille r b  0 0  -1 b_6 -15 10 -10 10  0 0 -1 
clear
nexplode r f
depouille rr r  0 0 -1   r_1 -15 -10 -10 10  0 0 -1 
clear
nexplode rr e
verase b
vdisplay rr
vfit
sage " Fillet on four lateral edges, then on the top and bottom edges  "
sage "    nexplode rr e"
sage "    blend result rr 3 rr_2 3 rr_3 3 rr_10 3 rr_11"
sage "    nexplode result e"
sage "    blend result result 2 result_11 3 result_12"
sage " "
blend result rr 3 rr_2 3 rr_3 3 rr_10 3 rr_11
erase rr
erase result
nexplode result e
blend result result 2 result_11 3 result_12 
clear
nexplode result f
verase rr
vdisplay result
vfit
sage " Creating a profile on the top face "
sage "    nexplode result f"
sage "    profile p  S result_16 F 10 4 D 1 0 C 2 90. Y 8 C 2 90. X -2 C 2 90. Y -8 C 2 90. X 2
"
sage " "
profile p  S result_16 F 10 4 D 1 0 C 2 90. Y 8 C 2 90. X -2 C 2 90. Y -8 C 2 90. X 2
sage " Creating a prism"
sage "    prism rr p 0 0 20"
sage " "
prism rr p 0 0 20
sage " Change some attributs : MATERIAL , TRANSPARENCY"
sage "    vsetmaterial rr PLASTIC"
sage "    vsetcolor rr RED"
sage "    vsettransparency result 0.5"
sage "    vsetdispmode 1"
sage " "
vdisplay rr
vsetmaterial rr PLASTIC
vsettransparency rr 0.5
vsetcolor rr RED
vsettransparency result 0.5
vsetdispmode 1
vfit
fit
sage " Fusion of this prism with the original part "
sage "    fuse result rr result"
sage " "
fuse result rr result
donl result
vdisplay result
vdonly result
vfit
nexplode result f
erase result
fit

sage " Opening the top face"
sage "    offsetshape r result -1 0.0001 result_17"
sage " "
nexplode result f

offsetshape r result -1 0.0001 result_17
vdisplay r
vdonly r
vfit
vsetcolor r MATRABLUE
vsettransparency r 0

sage " Creating a cylinder and positionning it"
sage "    pcylinder cyl 2 300"
sage "    trotate cyl cyl 0 0 0  1 0 0 45"
sage "    ttranslate cyl cyl 0 7.5 0"
sage " "
pcylinder cyl 2 30
vdisplay cyl 
vsetcolor cyl RED
vfit
trotate cyl cyl 0 0 0  1 0 0 45
ttranslate cyl cyl 0 7.5 0
vdisplay cyl 
vsetcolor cyl RED
vfit
vdisplay cyl 
vsetcolor cyl RED
vfit
sage "Demo completed"
