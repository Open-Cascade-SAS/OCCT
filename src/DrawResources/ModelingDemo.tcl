
set stationname $tcl_platform(platform)
if { ${stationname} == "windows" } {
   proc winfo { aTest aWindow } { return False }
}


proc sage { a} {
    global stationname 
    if { ${stationname} != "windows" } {
	
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
sage " Creating a box"
sage "    box  b -10 -10 -10 20 20 20"
sage " "
box b -10 -10 -10 20 20 20
fit
nexplode b f
erase b
sage " Draft of two lateral faces "
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
fit
sage " Fusion of this prism with the original part "
sage "    fuse result rr result"
sage " "
fuse result rr result
donl result
nexplode result f
erase result
fit

sage " Opening the top face"
sage "    offsetshape r result -1 0.0001 result_17"
sage " "

nexplode result f
offsetshape r result -1 0.0001 result_17
sage " Creating a cylinder and positionning it"
sage "    pcylinder cyl 2 300"
sage "    trotate cyl cyl 0 0 0  1 0 0 45"
sage "    ttranslate cyl cyl 0 7.5 0"
sage " "
pcylinder cyl 2 30
trotate cyl cyl 0 0 0  1 0 0 45
ttranslate cyl cyl 0 7.5 0
sage " Display the Shape on Hidden Line Mode "
sage "   hlr hid r"
sage ""
donl r
hlr hid r
sage " Display the Shape on HLR Mode "
sage "   hlr nohid r"
sage "   hlr hlr r"
sage ""
donl r
hlr nohid r
hlr hlr r
sage "Demo completed"

