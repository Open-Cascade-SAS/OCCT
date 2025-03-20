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

help smooth {smooth  cname tol [filename] } "DRAW Variables management"

proc smooth {name tol {file ""}} {
    if {$file == ""} {
	uplevel #0 "bsmooth $name $tol"
    } else {
	global Draw_DataDir
	uplevel #0 "bsmooth $name $tol $Draw_DataDir/$file"
    }
    return $name
}

#################################################
# beziersmooth
#################################################

help beziersmooth { beziersmooth  cname tol deg [-GR -VA -PR] [filename] } "DRAW Variables management"

proc beziersmooth {name tol deg option {file ""}} {
    if {$file == ""} {
	uplevel #0 "bzsmooth $name $tol $deg $option"
    } else {
	global Draw_DataDir
	uplevel #0 "bzsmooth $name $tol  $deg $option $Draw_DataDir/$file"
    }
    return $name
}

help pickf {name : extract picked with mouse face as a new variable\
} {DRAW Variables management}

proc pickf {name} {
    global $name
    eval renamevar "[pickface]" $name
}
