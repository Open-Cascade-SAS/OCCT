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

proc SelectLoop { DF shape T} {

#
# loop to select all the sub-shapes of type <T> in the context <shape>
#

    global   $DF
    global   $shape
    display  $shape
    fit
    clear
    set L [Label $DF 0:1000]
    isos $shape 0

    set ter _*   
    global $ter

    foreach S [directory [concat $shape$ter]] {
	global $S
	unset $S
    }

    uplevel #0 explode $shape $T
       
    foreach S [directory [concat $shape$ter]] {
	clear
	puts $S
	global $S
	display $shape
	display $S
	#wclick
	#SelectShape $DF $L $S $shape
	SelectGeometry $DF $L $S $shape
	DumpSelection $DF $L 1
	wclick
	SolveSelection $DF $L

	pick ID x y z b
	if {[dval b] == 3} {
	    foreach OS [directory [concat $shape$ter]] {
		global $OS
		unqset $OS
	    }
	    return
	}
    }
}





