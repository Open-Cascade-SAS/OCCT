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

#
# view management scripts
#
#

proc mu4 {} {
    global tcl_platform
    set stationname $tcl_platform(platform)
    delete
#    if { ${stationname} == "windows" } {
#	view 1 +X+Z 20 20 300 300
#	view 2 +X+Y 20 350 300 300
#	view 3 -Y+Z 328 20 300 300
#	view 4 AXON 328 350 300 300
#    } else {
	view 1 +X+Z 320 20 400 400
	view 2 +X+Y 320 450 400 400
	view 3 -Y+Z 728 20 400 400
	view 4 AXON 728 450 400 400
#    }
}

help mu4 ", Four views layout" "DRAW Graphic Commands"

proc mu7 {} {
    delete
    view  1 +Y+Z 0   300 275 270
    view  2 +X-Y 285 0   275 270
    view  3 +X+Z 285 300 275 270
    view  4 +X+Y 285 600 275 270
    view  5 -Y+Z 570 300 275 270
    view  6 -X+Z 855 300 275 270
    view  7 AXON 855 600 275 270
}
help mu7 ", Seven views layout" "DRAW Graphic Commands"

proc mu8 {} {
    delete
    view  1 +Y+Z 0   300 275 270
    view  2 +X-Y 285 0   275 270
    view  3 +X+Z 285 300 275 270
    view  4 +X+Y 285 600 275 270
    view  5 -Y+Z 570 300 275 270
    view  6 PERS 855 0   275 270
    view  7 -X+Z 855 300 275 270
    view  8 AXON 855 600 275 270
}
help mu8 ", Seven views layout" "DRAW Graphic Commands"

proc mu24 {} {
    delete
    view  1 +X+Y 0   0   180 190
    view  2 -Y+X 0   220 180 190
    view  3 -X-Y 0   440 180 190
    view  4 +Y-X 0   660 180 190
    
    view  5 +Y+X 190 0   180 190
    view  6 -X+Y 190 220 180 190
    view  7 -Y-X 190 440 180 190
    view  8 +X-Y 190 660 180 190
    
    view  9 +X+Z 380 0   180 190
    view 10 -Z+X 380 220 180 190
    view 11 -X-Z 380 440 180 190
    view 12 +Z-X 380 660 180 190
    
    view 13 +Z+X 570 0   180 190
    view 14 -X+Z 570 220 180 190
    view 15 -Z-X 570 440 180 190
    view 16 +X-Z 570 660 180 190
    
    view 17 +Y+Z 760 0   180 190
    view 18 -Z+Y 760 220 180 190
    view 19 -Y-Z 760 440 180 190
    view 20 +Z-Y 760 660 180 190
    
    view 21 +Z+Y 950 0   180 190
    view 22 -Y+Z 950 220 180 190
    view 23 -Z-Y 950 440 180 190
    view 24 +Y-Z 950 660 180 190
    
}
help mu24 ", 24 views layout" "DRAW Graphic Commands"

proc axo {} {
    global tcl_platform
    set stationname $tcl_platform(platform)
    delete
#    if { ${stationname} == "windows" } {
#	view 1 AXON 10 120 600 600
#    } else {
	view 1 AXON 465 20 800 800
#    }
}
help axo ", One axonometric view. Orientation +X-Y+Z" "DRAW Graphic Commands"

proc haxo {} {
    delete
    view 1 AXON 465 20 800 800*20.4/29.1
}
help haxo ", One axonometric horizontal view. Orientation +X-Y+Z" "DRAW Graphic Commands"

proc vaxo {} {
    delete
    view 1 AXON 705 20 800*20.4/29.1 800
}
help vaxo ", One axonometric vertical view. Orientation +X-Y+Z" "DRAW Graphic Commands"

proc pers {} {
    delete
    view 1 PERS 465 20 800 800
}
help pers ", One perspective view" "DRAW Graphic Commands"

proc hpers {} {
    delete
    view 1 PERS 465 20 800 800*20.4/29.1
}
help hpers ", One perspective horizontal view" "DRAW Graphic Commands"

proc vpers {} {
    delete
    view 1 PERS 705 20 800*20.4/29.1 800
}
help vpers ", One perspective vertical view" "DRAW Graphic Commands"

proc front {} {
    delete
    view 1 +X+Z 465 20 800 800
}
help front ", One front view. Orientation +X+Z" "DRAW Graphic Commands"

proc hfront {} {
    delete
    view 1 +X+Z 465 20 800 800*20.4/29.1
}
help hfront ", One front horizontal view. Orientation +X+Z" "DRAW Graphic Commands"

proc vfront {} {
    delete
    view 1 +X+Z 705 20 800*20.4/29.1 800
}
help vfront ", One front vertical view. Orientation +X+Z" "DRAW Graphic Commands"

proc top {} {
    delete
    view 1 +X+Y 465 20 800 800
}
help top ", One top view. Orientation +X+Y" "DRAW Graphic Commands"

proc htop {} {
    delete
    view 1 +X+Y 465 20 800 800*20.4/29.1
}
help htop ", One top horizontal view. Orientation +X+Y" "DRAW Graphic Commands"

proc vtop {} {
    delete
    view 1 +X+Y 705 20 800*20.4/29.1 800
}
help vtop ", One top vertical view. Orientation +X+Y" "DRAW Graphic Commands"

proc left {} {
    delete
    view 1 -Y+Z 465 20 800 800
}
help left ", One left view. Orientation -Y+Z" "DRAW Graphic Commands"

proc hleft {} {
    delete
    view 1 -Y+Z 465 20 800 800*20.4/29.1
}
help hleft ", One left horizontal view. Orientation -Y+Z" "DRAW Graphic Commands"

proc vleft {} {
    delete
    view 1 -Y+Z 705 20 800*20.4/29.1 800
}
help vleft ", One left vertical view. Orientation -Y+Z" "DRAW Graphic Commands"

proc back {} {
    delete
    view 1 -X+Z 465 20 800 800
}
help back ", One back view. Orientation -X+Z" "DRAW Graphic Commands"

proc hback {} {
    delete
    view 1 -X+Z 465 20 800 800*20.4/29.1
}
help hback ", One back horizontal view. Orientation -X+Z" "DRAW Graphic Commands"

proc vback {} {
    delete
    view 1 -X+Z 705 20 800*20.4/29.1 800
}
help vback ", One back vertical view. Orientation -X+Z" "DRAW Graphic Commands"

proc right {} {
    delete
    view 1 +Y+Z 465 20 800 800
}
help right ", One right view. Orientation +Y+Z" "DRAW Graphic Commands"

proc hright {} {
    delete
    view 1 +Y+Z 465 20 800 800*20.4/29.1
}
help hright ", One right horizontal view. Orientation +Y+Z" "DRAW Graphic Commands"

proc vright {} {
    delete
    view 1 +Y+Z 705 20 800*20.4/29.1 800
}
help vright ", One right vertical view. Orientation +Y+Z" "DRAW Graphic Commands"

proc bottom {} {
    delete
    view 1 +X-Y 465 20 800 800
}
help bottom ", One bottom view. Orientation +X-Y" "DRAW Graphic Commands"

proc hbottom {} {
    delete
    view 1 +X-Y 465 20 800 800*20.4/29.1
}
help hbottom ", One bottom horizontal view. Orientation +X-Y" "DRAW Graphic Commands"

proc vbottom {} {
    delete
    view 1 +X-Y 705 20 800*20.4/29.1 800
}
help vbottom ", One bottom vertical view. Orientation +X-Y" "DRAW Graphic Commands"

proc v2d {} {
    delete
    view 1 -2D- 465 20 800 800
}
help v2d ", One 2d view" "DRAW Graphic Commands"

proc av2d {} {
    delete
    global tcl_platform
    set stationname $tcl_platform(platform)
#    if { ${stationname} == "windows" } {
#	view 2 -2D- 328  20 300 300
#	view 1 AXON 328 350 300 300
#    } else {
	view 2 -2D-  728 20 400 400
	view 1 AXON 728 450 400 400
#    }
}
help av2d ", axono and 2d view" "DRAW Graphic Commands"

proc v2d2 {} {
    view 2 -2D-  728 20 400 400
}
help v2d2 "2d view on number 2" "DRAW Graphic Commands"

proc smallview {{v AXON}} {
    global tcl_platform
    set stationname $tcl_platform(platform)
    delete
#    if { ${stationname} == "windows" } {
#	view 1 $v 328 350 300 300
#    } else {
	view 1 $v 728 450 400 400
#    }
}

help smallview " AXON PERS -2D- +X+Y ..." "DRAW Graphic Commands"
