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

proc 2dscan {} {
    dtyp .
    set name [lastrep id x y MOUSEbutton]
    global $name
    lastrep id x1 y1 z1 MOUSEbutton
    dset x0 x1 y0 y1 z0 z1 MOUSEbutton 0
    erase $name
    draw id 6 $name
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dx x2-x1 dy y2-y1 dz z2-z1  x1 x2  y1 y2  z1 z2
	draw id 6 $name
	2dtranslate $name dx dy
	draw id 6 $name
    }
    draw id 6 $name
    if {[dval MOUSEbutton] == 1} { display $name; return; }
    dset dx x0-x2 dy y0-y2 dz z0-z2
    2dtranslate $name dx dy
    display $name
}

proc scan {} {
    dtyp .
    set name [lastrep id x y MOUSEbutton]
    global $name
    lastrep id x1 y1 z1 MOUSEbutton
    dset x0 x1 y0 y1 z0 z1 MOUSEbutton 0
    erase $name
    draw id 6 $name
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dx x2-x1 dy y2-y1 dz z2-z1  x1 x2  y1 y2  z1 z2
	draw id 6 $name
	translate $name dx dy dz
	draw id 6 $name
    }
    draw id 6 $name
    if {[dval MOUSEbutton] == 1} { display $name; return; }
    dset dx x0-x2 dy y0-y2 dz z0-z2
    translate $name dx dy dz
    display $name
}

proc scanx {} {
    dtyp .
    set name [lastrep id x y MOUSEbutton]
    global $name
    lastrep id x1 y1 z1 MOUSEbutton
    dset x0 x1 y0 y1 z0 z1 MOUSEbutton 0
    erase $name
    draw id 6 $name
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dx x2-x1 x1 x2
	draw id 6 $name
	translate $name dx 0 0
	draw id 6 $name
    }
    draw id 6 $name
    if {[dval MOUSEbutton] == 1} {display $name; return;}
    dset dx x0-x2 
    translate $name dx 0 0
    display $name
}

proc scany {} {
    dtyp .
    set name [lastrep id x y MOUSEbutton]
    global $name
    lastrep id x1 y1 z1 MOUSEbutton
    dset x0 x1 y0 y1 z0 z1 MOUSEbutton 0
    erase $name
    draw id 6 $name
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dy y2-y1 y1 y2
	draw id 6 $name
	translate $name 0 dy 0
	draw id 6 $name
    }
    draw id 6 $name
    if {[dval MOUSEbutton] == 1} { display $name; return;}
    dset dy y0-y2
    translate $name 0 dy 0
    display $name
}

proc scanz {} {
    dtyp .
    set name [lastrep id x y MOUSEbutton]
    global $name
    lastrep id x1 y1 z1 MOUSEbutton
    dset x0 x1 y0 y1 z0 z1 MOUSEbutton 0
    erase $name
    draw id 6 $name
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dz z2-z1 z1 z2
	draw id 6 $name
	translate $name 0 0 dz
	draw id 6 $name
    }
    draw id 6 $name
    if {[dval MOUSEbutton] == 1} { display $name; return;}
    dset dz z0-z2
    translate $name 0 0 dz
    display $name
}

proc tscan {} {
    dtyp .
    set name [lastrep id x y MOUSEbutton]
    global $name
    lastrep id x1 y1 z1 MOUSEbutton
    dset x0 x1 y0 y1 z0 z1 MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dx x2-x0 dy y2-y0 dz z2-z0
	eval ttranslate [explode $name e] dx dy dz
	repaint
    }
    if {[dval MOUSEbutton] == 3} return;
    dset dx x2-x0 dy y2-y0 dz z2-z0
    ttranslate $name dx dy dz
}

proc tscanx {} {
    dtyp .
    set name [lastrep id x y MOUSEbutton]
    global $name
    lastrep id x1 y1 z1 MOUSEbutton
    dset x0 x1 y0 y1 z0 z1 MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dx x2-x0
	eval ttranslate [explode $name e] dx 0 0
	repaint
    }
    if {[dval MOUSEbutton] == 3} return;
    dset dx x2-x0 
    ttranslate $name dx 0 0
}

proc tscany {} {
    dtyp .
    set name [lastrep id x y MOUSEbutton]
    global $name
    lastrep id x1 y1 z1 MOUSEbutton
    dset x0 x1 y0 y1 z0 z1 MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dy y2-y0
	eval ttranslate [explode $name e] 0 dy 0
	repaint
    }
    if {[dval MOUSEbutton] == 3} return;
    dset dy y2-y0
    ttranslate $name 0 dy 0
}

proc tscanz {} {
    dtyp .
    set name [lastrep id x y MOUSEbutton]
    global $name
    lastrep id x1 y1 z1 MOUSEbutton
    dset x0 x1 y0 y1 z0 z1 MOUSEbutton 0
    while {[dval MOUSEbutton] == 0} {
	pick id x2 y2 z2 MOUSEbutton nowait
	dset dz z2-z0
	eval ttranslate [explode $name e] 0 0 dz
	repaint
    }
    if {[dval MOUSEbutton] == 3} return;
    dset dz z2-z0
    ttranslate $name 0 0 dz
}


