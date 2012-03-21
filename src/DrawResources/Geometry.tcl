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

proc cmp {} {
    dtyp .
    set name [lastrep id x y b]
    global $name
    lastrep id x1 y1 z1 b
    cfindp $name id x y pole
    if {[dval pole] == 0} return
    dset x0 x1 y0 y1 z0 z1 b 0
    draw id 6 $name
    while {[dval b] == 0} {
	pick id x2 y2 z2 b nowait
	dset dx x2-x1 dy y2-y1 dz z2-z1  x1 x2  y1 y2  z1 z2
	draw id 6 $name
	cmovep $name pole dx dy dz
	draw id 6 $name
    }
    draw id 6 $name
    if {[dval b] == 1} return;
    dset dx x0-x2 dy y0-y2 dz z0-z2
    cmovep $name pole dx dy dz
}

proc smp {} {
    dtyp .
    set name [lastrep id x y b]
    global $name
    lastrep id x1 y1 z1 b
    sfindp $name id x y upole vpole
    if {[dval upole] == 0} return
    dset x0 x1 y0 y1 z0 z1 b 0
    draw id 6 $name
    while { [dval b] == 0} {
	pick id x2 y2 z2 b nowait
	dset dx x2-x1  dy y2-y1  dz z2-z1 x1 x2 y1 y2 z1 z2
	draw id 6 $name
	movep $name upole vpole dx dy dz
	draw id 6 $name
    }
    draw id 6 $name
    if {[dval b] == 1} return
    dset dx x0-x2  dy y0-y2  dz z0-z2
    movep $name upole vpole dx dy dz
}

#################################################
# smooth
#################################################
proc smooth {name tol {file ""}} {
    if {$file == ""} {
	uplevel #0 "bsmooth $name $tol"
    } else {
	global Draw_DataDir
	uplevel #0 "bsmooth $name $tol $Draw_DataDir/$file"
    }
    return $name
}

help smooth {smooth  cname tol [filename] } "DRAW Variables management"

#################################################
# beziersmooth
#################################################
proc beziersmooth {name tol deg option {file ""}} {
    if {$file == ""} {
	uplevel #0 "bzsmooth $name $tol $deg $option"
    } else {
	global Draw_DataDir
	uplevel #0 "bzsmooth $name $tol  $deg $option $Draw_DataDir/$file"
    }
    return $name
}

help beziersmooth { beziersmooth  cname tol deg [-GR -VA -PR] [filename] } "DRAW Variables management"


