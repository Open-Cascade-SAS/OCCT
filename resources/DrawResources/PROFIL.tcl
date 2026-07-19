# Copyright (c) 1999-2014 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

# File:	BRepOffset.cxx
# Created:	Wed Oct 25 10:39:23 1995
# Author:	Bruno DUMORTIER
#		<dub@fuegox>


addmenu Sketch "Sketch" { vprofil2d }

proc mkprofil2d {last} {
    global [.top.p.eobj.name get]
    global [.top.p.eobj.face get]
    global DX 
    global DY
    global DX0
    global DY0

    append cmd " F "
    append cmd [dval DX] " " [dval DY]
    
    for {set i 0} {$i < [.top.l.lb.cm size]} {incr i} {
	append cmd " [.top.l.lb.cm get $i]"
    }
    if [info exist cmd] {
	uplevel #0 eval 2dprofile [.top.p.eobj.name get] $cmd $last
    }
    .top.p.eobj.com delete 0 end
    set DX0 [dval DX]
    set DY0 [dval DY]
    update
    repaint
}

proc mkprofil3d {last} {
    global [.top.p.eobj.name get]
    global [.top.p.eobj.face get]
    global DX 
    global DY

    append cmd " F "
    append cmd [dval DX] " " [dval DY]
    
    if [info exist [.top.p.eobj.face get]] {
	append cmd " S"
	append cmd " [.top.p.eobj.face get]";
    }
    for {set i 0} {$i < [.top.l.lb.cm size]} {incr i} {
	append cmd " [.top.l.lb.cm get $i]"
    }
    if [info exist cmd] {
	puts $cmd
	eval profile [.top.p.eobj.name get] $cmd $last
    }
    .top.p.eobj.com delete 0 end
    repaint
}

proc bougex {} {
    global DX
    global DY

    dset x0 DX
    dset y0 DY

    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset DX x0+x2-x1 DY y0
	mkprofil2d WW
    }
    if {[dval MOUSEbutton] == 1} { mkprofil2d WW; return; }
    dset DX x0 DY y0
    mkprofil2d WW
}

proc bougey {} {
    global DX
    global DY

    dset x0 DX
    dset y0 DY

    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset DX x0 DY y0+y2-y1
	mkprofil2d WW
    }
    if {[dval MOUSEbutton] == 1} { mkprofil2d WW; return; }
    dset DX x0 DY y0
    mkprofil2d WW
}

proc bouge {} {
    global DX
    global DY

    dset x0 DX
    dset y0 DY

    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset DX x0+x2-x1 DY y0+y2-y1
	mkprofil2d WW
    }
    if {[dval MOUSEbutton] == 1} { mkprofil2d WW; return; }
    dset DX x0 DY y0
    mkprofil2d WW
}

proc bougefp {} {
    global DX
    global DY

    dset x0 DX
    dset y0 DY

    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	pick 29 x2 y2 z2 MOUSEbutton nowait
	dset DX x2 DY y2
	mkprofil2d WW
    }
    if {[dval MOUSEbutton] == 1} { mkprofil2d WW; return; }
    dset DX x0 DY y0
    mkprofil2d WW
}

proc movex {} {
    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	set MO ""
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dx x2-x1 
	if { [dval dx] != 0 } {
	    append MO "x "
	    append MO [dval dx]
	}
	append MO " WW"
	mkprofil2d $MO
    }
    if {[dval MOUSEbutton] == 1} { 
	set MO ""
	if { [dval dx] != 0 } {
	    append MO "x "
	    append MO [dval dx]
	}
	.top.l.lb.cm insert end $MO
	mkprofil2d WW
	return
    }
    2dclear
    if [info exist [.top.p.eobj.face get]] {
	eval pcurve [.top.p.eobj.face get];
    }
    mkprofil2d WW
}

proc movey {} {
    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	set MO ""
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dy y2-y1 
	if { [dval dy] != 0 } {
	    append MO "y "
	    append MO [dval dy]
	}
	append MO " WW"
	mkprofil2d $MO
    }
    if {[dval MOUSEbutton] == 1} { 
	set MO ""
	if { [dval dy] != 0 } {
	    append MO "y "
	    append MO [dval dy]
	}
	.top.l.lb.cm insert end $MO
	mkprofil2d WW
	return
    }
    2dclear
    if [info exist [.top.p.eobj.face get]] {
	eval pcurve [.top.p.eobj.face get];
    }
    mkprofil2d WW
}

proc movec {} {
    autodisplay
    upvar #0 pi    PI
    upvar #0 CurX  CX
    upvar #0 CurY  CY
    upvar #0 CurDX CDX
    upvar #0 CurDY CDY

    line dummyline CX CY CDX CDY

    point p1 CX CY
    repaint
    pick id x1 y1 z1 MOUSEbutton
    dset sign 1
    if {[dval MOUSEbutton] == 2} { dset sign -1}
    dset MOUSEbutton 0
    autodisplay

    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	autodisplay
	point p2 x2 y2
	cirtang dummy dummyline p1 p2
	dset PS sign*(CDX*(y2-CY)-CDY*(x2-CX))
	if { [dval PS] < 0 } { reverse dummy_1}
	parameters dummy_1 CX CY U1
	parameters dummy_1 x2 y2 U2
	autodisplay
	eval trim dummy_1 dummy_1 U1 U2
	repaint
    }

    if {[dval MOUSEbutton] == 1} { 
	erase dummy_1
	set MO ""
	if { [dval U2] < [dval U1]} { dset U2 U2+2*PI}
	dset da sign*180*(U2-U1)/PI
	2dcvalue dummy_1 0 X Y DX DY
	dset dr sqrt(DX*DX+DY*DY)
	dset PS CDX*(y2-CY)-CDY*(x2-CX)
	if { [dval PS] < 0 } {dset dr -dr}
	if { [dval dr] != 0 } {
	    if { [dval da] != 0 } {
		append MO "c "
		append MO [format "%.3f" [dval dr]] " " 
		append MO [format "%.3f" [dval da]]
		.top.l.lb.cm insert end $MO
	    }
	}
	mkprofil2d WW
	return
    }
    2dclear
    if [info exist [.top.p.eobj.face get]] {
	eval pcurve [.top.p.eobj.face get];
    }
    mkprofil2d WW
}

proc movel {} {
    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	set MO ""
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dr sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))
	if { [dval dr] != 0 } {
	    append MO "l "
	    append MO [dval dr]
	}
	append MO " WW"
	mkprofil2d $MO
    }
    if {[dval MOUSEbutton] == 1} { 
	set MO ""
	if { [dval dr] != 0 } {
	    append MO "l "
	    append MO [dval dr]
	}
	.top.l.lb.cm insert end $MO
	mkprofil2d WW
	return
    }
    2dclear
    if [info exist [.top.p.eobj.face get]] {
	eval pcurve [.top.p.eobj.face get];
    }
    mkprofil2d WW
}

proc movet {} {
    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	set MO ""
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dx x2-x1 dy y2-y1
	if { [dval dx] != 0 || [dval dy] != 0 } {
	    append MO "t "
	    append MO [dval dx] " " [dval dy]
	}
	append MO " WW"
	mkprofil2d $MO
    }
    if {[dval MOUSEbutton] == 1} { 
	set MO ""
	if { [dval dx] != 0 || [dval dy] != 0 } {
	    append MO "t "
	    append MO [dval dx] " " [dval dy]
	}
	.top.l.lb.cm insert end $MO
	mkprofil2d WW
	return
    }
    2dclear
    if [info exist [.top.p.eobj.face get]] {
	eval pcurve [.top.p.eobj.face get];
    }
    mkprofil2d WW
}

proc movexx {} {
    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	set MO ""
	pick id x2 y2 z2 MOUSEbutton nowait
	append MO "xx "
	append MO [dval x2]
	append MO " WW"
	mkprofil2d $MO
    }
    if {[dval MOUSEbutton] == 1} { 
	set MO ""
	append MO "xx "
	append MO [dval x2]
	.top.l.lb.cm insert end $MO
	mkprofil2d WW
	return
    }
    2dclear
    if [info exist [.top.p.eobj.face get]] {
	eval pcurve [.top.p.eobj.face get];
    }
    mkprofil2d WW
}

proc moveyy {} {
    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	set MO ""
	pick id x2 y2 z2 MOUSEbutton nowait
	append MO "yy "
	append MO [dval y2]
	append MO " WW"
	mkprofil2d $MO
    }
    if {[dval MOUSEbutton] == 1} { 
	set MO ""
	append MO "yy "
	append MO [dval y2]
	.top.l.lb.cm insert end $MO
	mkprofil2d WW
	return
    }
    2dclear
    if [info exist [.top.p.eobj.face get]] {
	eval pcurve [.top.p.eobj.face get];
    }
    mkprofil2d WW
}

proc moveix {} {
    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	set MO ""
	pick id x2 y2 z2 MOUSEbutton nowait
	append MO "ix "
	append MO [dval x2]
	append MO " WW"
	mkprofil2d $MO
    }
    if {[dval MOUSEbutton] == 1} { 
	set MO ""
	append MO "ix "
	append MO [dval x2]
	.top.l.lb.cm insert end $MO
	mkprofil2d WW
	return
    }
    2dclear
    if [info exist [.top.p.eobj.face get]] {
	eval pcurve [.top.p.eobj.face get];
    }
    mkprofil2d WW
}

proc moveiy {} {
    pick id x1 y1 z1 MOUSEbutton
    dset MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	set MO ""
	pick id x2 y2 z2 MOUSEbutton nowait
	append MO "iy "
	append MO [dval y2]
	append MO " WW"
	mkprofil2d $MO
    }
    if {[dval MOUSEbutton] == 1} { 
	set MO ""
	append MO "iy "
	append MO [dval y2]
	.top.l.lb.cm insert end $MO
	mkprofil2d WW
	return
    }
    2dclear
    if [info exist [.top.p.eobj.face get]] {
	eval pcurve [.top.p.eobj.face get];
    }
    mkprofil2d WW
}

proc vprofil2d {} {

    toplevel .top -bg cornsilk
    wm geometry .top +10+10
    wm title .top "SKETCHER"
    
    frame .top.l -bg cornsilk
    
    frame .top.l.lb -bg azure1 -relief ridge -bd 4
    listbox .top.l.lb.cm -bg azure1 -yscrollcommand ".top.l.lb.sc set"
#    scrollbar .top.l.lb.sc -bg azure2 -fg grey80 -activeforeground grey90\
#	    -relief sunken -command ".top.l.lb.cm yview"
    scrollbar .top.l.lb.sc -bg azure2 \
	    -relief sunken -command ".top.l.lb.cm yview"
    bind .top.l.lb.cm <Double-Button-1> {
	.top.p.eobj.com delete 0 end
	.top.p.eobj.com insert end\
		[.top.l.lb.cm get [.top.l.lb.cm curselection]]
    }
    pack .top.l.lb.cm .top.l.lb.sc -side left -fill both -padx 2m
    
    frame .top.l.t -bg azure2 -relief ridge -bd 4
    radiobutton .top.l.t.a -bg azure3 -activebackground azure2\
	    -text "face" -variable proftype -value F
    radiobutton .top.l.t.b -bg azure3 -activebackground azure2\
	    -text "closed" -variable proftype -value W
    radiobutton .top.l.t.c -bg azure3 -activebackground azure2\
	    -text "wire" -variable proftype -value " "
    
    frame .top.l.f -bg azure2 -relief ridge -bd 4
    button .top.l.f.f -bg deepskyblue -activebackground lightskyblue1\
	    -text "  FIT  " -command {
	if [info exist [.top.p.eobj.face get]] {
	    eval pcurve [.top.p.eobj.face get];
	}
	mkprofil2d WW
	2dfit; repaint
    }
    button .top.l.f.u -bg deepskyblue -activebackground lightskyblue1\
	    -text " ZOOM + " -command {
	2dmu; repaint
    }
    button .top.l.f.d -bg deepskyblue -activebackground lightskyblue1\
	    -text " ZOOM - " -command {2dmd; repaint}

    pack .top.l.f.f .top.l.f.u .top.l.f.d -side right -padx 1m -pady 1m
    
    frame .top.l.d -bg azure2 -relief ridge -bd 4
    button .top.l.d.u -bg deepskyblue -activebackground lightskyblue1\
	    -text " UP" -command 2dpu
    button .top.l.d.d -bg deepskyblue -activebackground lightskyblue1\
	    -text "DOWN" -command 2dpd
    button .top.l.d.l -bg deepskyblue -activebackground lightskyblue1\
	    -text "LEFT" -command 2dpl
    button .top.l.d.r -bg deepskyblue -activebackground lightskyblue1\
	    -text "RIGHT" -command 2dpr
    pack .top.l.d.u .top.l.d.d .top.l.d.l .top.l.d.r -side left\
	    -padx 1m -pady 1m

    frame .top.l.q -bg azure2 -relief ridge -bd 4
    button .top.l.q.ok -bg deepskyblue -activebackground lightskyblue1\
	    -text " VALI " -command {
	delete 29
	if { $proftype == "F"} {
	    set proftype " "
	} elseif { $proftype != "W" } {
	    set proftype "WW"
	}
	mkprofil3d $proftype
	destroy .top
	if [info exist PickedFace] { erase PickedFace}
	repaint;
    }
    button .top.l.q.ko -bg deepskyblue -activebackground lightskyblue1\
	    -text " CANCEL " -command {
	delete 29
	if [info exist [.top.p.eobj.name get]] {
	    eval unset [.top.p.eobj.name get];
	}
	destroy .top
	if [info exist PickedFace] { unset PickedFace}
	repaint;
    }
    button .top.l.q.h  -bg deepskyblue -activebackground lightskyblue1\
	    -text " HELP " -command helpme
    pack .top.l.q.ok .top.l.q.ko .top.l.q.h -side left -fill both\
	    -padx 2m -pady 1m
    
    pack .top.l.t.a .top.l.t.b .top.l.t.c -side left -padx 1m -pady 1m
    
    pack .top.l.lb .top.l.t -side top -pady 1m -fill both
    pack .top.l.f .top.l.d -side top -pady 1m -fill both
    pack .top.l.q -side bottom -fill both -pady 1m
    
    pack .top.l -side left -fill both -padx 1m
    
    frame .top.p -bg azure2 -relief ridge -bd 4
    
    frame .top.p.obj -bg azure2 
    frame .top.p.eobj -bg azure2 
    label .top.p.obj.name -bg azure2 -text "Name:"
    entry .top.p.eobj.name  -bg azure1 -relief sunken
    .top.p.eobj.name insert end "prof"
    
    label .top.p.obj.com -bg azure2 -text "Command:"
    entry .top.p.eobj.com  -bg azure1 -relief sunken
    bind .top.p.eobj.com <Return> {
	.top.l.lb.cm insert end [.top.p.eobj.com get]
	mkprofil2d WW
    } 
    
    label .top.p.obj.face  -bg azure2 -text "Face:"
    entry .top.p.eobj.face -bg azure1 -relief sunken -textvariable CURFACE
    bind .top.p.eobj.face <Return> {
	2dclear
	if [info exist [.top.p.eobj.face get]] {
	    eval pcurve [.top.p.eobj.face get];
	} elseif { [.top.p.eobj.face get] == "."} {
	    uplevel #0 pickface
	    set CURFACE PickedFace
	    uplevel #0 eval pcurve [.top.p.eobj.face get];
	}
	mkprofil2d WW
	2dfit
	repaint
    } 
    
    label .top.p.obj.x0 -bg azure2 -text "X0:"
    entry .top.p.eobj.x0  -bg azure1 -relief sunken -textvariable DX0
    bind .top.p.eobj.x0 <Return> {
	dset DX [expr $DX0]
	mkprofil2d WW
    } 
    
    label .top.p.obj.y0 -bg azure2 -text "Y0:"
    entry .top.p.eobj.y0  -bg azure1 -relief sunken -textvariable DY0
    bind .top.p.eobj.y0 <Return> {
	dset DY [expr $DY0]
	mkprofil2d WW
    } 
    
    frame .top.c -bg azure2 -relief ridge -bd 4
    button .top.c.add -bg deepskyblue -activebackground lightskyblue1\
	    -text " Add " -command {
	.top.l.lb.cm insert end [.top.p.eobj.com get]
	mkprofil2d WW
    }
    button .top.c.rem -bg deepskyblue -activebackground lightskyblue1\
	    -text "Remove" -command {
	if {[.top.l.lb.cm curselection] != ""} {
	    .top.l.lb.cm delete [.top.l.lb.cm curselection]
	} else {
	    .top.l.lb.cm delete end
	}
	2dclear
	if [info exist [.top.p.eobj.face get]] {
	    eval pcurve [.top.p.eobj.face get];
	}
	mkprofil2d WW
    }
    button .top.c.set -bg deepskyblue -activebackground lightskyblue1\
	    -text " Set " -command {
	if {[.top.l.lb.cm curselection] != ""} {
	    .top.l.lb.cm insert [.top.l.lb.cm curselection]\
		    [.top.p.eobj.com get]
	    .top.l.lb.cm delete [.top.l.lb.cm curselection]
	} else {
	    .top.l.lb.cm insert end [.top.l.lb.cm get]
	}
	mkprofil2d WW
    }
    pack .top.c.add .top.c.rem .top.c.set -side left -fill both\
	    -padx 4m -pady 1m
    
    frame .top.m -bg azure2 -relief ridge -bd 4
    
    frame .top.m.m1 -bg azure2 
    button .top.m.m1.x -bg deepskyblue -activebackground lightskyblue1\
	    -text " X " -command movex
    button .top.m.m1.xx -bg deepskyblue -activebackground lightskyblue1\
	    -text " XX " -command movexx
    
    frame .top.m.m2 -bg azure2 
    button .top.m.m2.y -bg deepskyblue -activebackground lightskyblue1\
	    -text " Y " -command movey
    button .top.m.m2.yy -bg deepskyblue -activebackground lightskyblue1\
	    -text " YY " -command moveyy
    
    frame .top.m.m3 -bg azure2 
    button .top.m.m3.c -bg deepskyblue -activebackground lightskyblue1\
	    -text " C " -command movec
    button .top.m.m3.ix -bg deepskyblue -activebackground lightskyblue1\
	    -text " IX " -command moveix
    
    frame .top.m.m4 -bg azure2 
    button .top.m.m4.l -bg deepskyblue -activebackground lightskyblue1\
	    -text " L " -command movel
    button .top.m.m4.iy -bg deepskyblue -activebackground lightskyblue1\
	    -text " IY " -command moveiy
    
    frame .top.m.m5 -bg azure2 
    button .top.m.m5.t -bg deepskyblue -activebackground lightskyblue1\
	    -text " T " -command movet
    button .top.m.m5.tt -bg deepskyblue -activebackground lightskyblue1\
	    -text " TT " -command movett
    
    frame .top.sc -bg azure2 -relief ridge -bd 4
    frame .top.sc.sc1 -bg azure2
    button .top.sc.sc1.d -bg deepskyblue -activebackground lightskyblue1\
	    -text "SCAN" -command bouge
    button .top.sc.sc1.f -bg deepskyblue -activebackground lightskyblue1\
	    -text " First Point " -command bougefp
    
    frame .top.sc.sc2 -bg azure2
    button .top.sc.sc2.x -bg deepskyblue -activebackground lightskyblue1\
	    -text " SCAN X " -command bougex
    button .top.sc.sc2.y -bg deepskyblue -activebackground lightskyblue1\
	    -text " SCAN Y " -command  bougey 
    
    pack .top.p.obj.name .top.p.obj.com .top.p.obj.face .top.p.obj.x0\
	    .top.p.obj.y0 -side top -fill x -pady 1m
    pack .top.p.eobj.name .top.p.eobj.com .top.p.eobj.face .top.p.eobj.x0\
	    .top.p.eobj.y0 -side top -fill x -pady 1m
    pack .top.p.obj .top.p.eobj -side left 
    
    pack .top.m.m1.x .top.m.m1.xx -side top -fill x -pady 1m
    pack .top.m.m2.y .top.m.m2.yy -side top -fill x -pady 1m
    pack .top.m.m3.c .top.m.m3.ix -side top -fill x -pady 1m
    pack .top.m.m4.l .top.m.m4.iy -side top -fill x -pady 1m
    pack .top.m.m5.t .top.m.m5.tt -side top -fill x -pady 1m
    pack .top.m.m1 .top.m.m2 .top.m.m3 .top.m.m4 .top.m.m5 -side left -padx 2m
    
    pack .top.sc.sc1.d .top.sc.sc1.f -side top -fill x -pady 1m
    pack .top.sc.sc2.x .top.sc.sc2.y -side top -fill x -pady 1m
    pack .top.sc.sc1 .top.sc.sc2 -side left -padx 6m 
    
    pack .top.p  -side top -fill both -padx 1m -pady 1m
    pack .top.c  -side top -fill both -padx 1m -pady 1m
    pack .top.m  -side top -fill both -padx 1m -pady 1m
    pack .top.sc -side top -fill both -padx 1m -pady 1m

    #global variables and init them
    global proftype
    global CURFACE
    global DX
    global DY
    global DX0
    global DY0

    set  proftype " "
    set  CURFACE ""
    set  DX0 ""
    set  DY0 ""
    dset DX 0 
    dset DY 0
    
    view 29 -2D- 465 10 664 410
    2dclear
    # call 2dprofile to initialize CurX CurY CurDX CurDY
    mkprofil2d WW
}

proc helpme {} {
    toplevel .h -bg azure3
    wm title .h "HELP"
    wm geometry .h +40+90
    message .h.m -justify left -bg azure2 -width 13c -relief ridge -bd 4 -text\
     "Build a profile in the UV plane from a moving point and direction.\n \
      The original point and direction are 0 0 and 1 0.\n \
      Codes and values describe the point or direction change.\n \
      When the point change the direction becomes the tangent.\n \
      All angles are in degree (may be negative).\n \
      By default the profile is closed.\n \
      \n \
      Instruction\tParameters\tAction\n \
      F\t\tX Y\t\tSet the first point\n \
      X\t\tDX\t\tTranslate point along X\n \
      Y\t\tDY\t\tTranslate point along Y\n \
      L\t\tDL\t\tTranslate point along direction\n \
      XX\t\tX\t\tSet point X coordinate\n \
      YY\t\tY\t\tSet point Y coordinate\n \
      T\t\tDX DY\t\tTranslate point\n \
      TT\t\tX Y\t\tSet point\n \
      R\t\tAngle\t\tRotate direction\n \
      RR\t\tAngle\t\tSet direction\n \
      D\t\tDX DY\t\tSet direction\n \
      IX\t\tX\t\tIntersect with vertical\n \
      IY\t\tY\t\tIntersect with horizontal\n \
      C\t\tRadius Angle\tArc of circle tangent to direction"

    frame .h.q -relief ridge -bd 4 -bg azure3
    button .h.q.q -bg deepskyblue -activebackground lightskyblue1 -text "QUIT" -command {destroy .h}
    pack .h.q.q -padx 1m -pady 1m
    pack .h.m .h.q -side top -pady 2m
}
