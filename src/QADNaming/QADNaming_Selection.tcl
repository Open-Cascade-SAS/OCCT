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





