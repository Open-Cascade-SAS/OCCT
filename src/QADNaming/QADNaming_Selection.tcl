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





