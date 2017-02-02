# Copyright (c) 2016 OPEN CASCADE SAS
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
# Created by: M.Sazonov
#
# Working with vectors and various measurements
#
# [2d] point is represented by (two/three) coords
# [2d] vector is represented by (two/three) coords
# plane is represented by an origin point and a normal vector
# [2d] line is represented by an origin point and a vector

help vec {vec x1 y1 z1 x2 y2 z2
  returns coordinates of vector between two points\
} {Vector and measurement Commands}

proc vec {x1 y1 z1 x2 y2 z2} {
    uplevel list [dval ($x2)-($x1)] [dval ($y2)-($y1)] [dval ($z2)-($z1)]
}

help 2dvec {2dvec x1 y1 x2 y2
  returns coordinates of 2D vector between two 2D points\
} {Vector and measurement Commands}

proc 2dvec {x1 y1 x2 y2} {
    uplevel list [dval ($x2)-($x1)] [dval ($y2)-($y1)]
}

help pln {pln x1 y1 z1 x2 y2 z2 x3 y3 z3
  returns plane built on three points\
} {Vector and measurement Commands}

proc pln {x1 y1 z1 x2 y2 z2 x3 y3 z3} {
    set v12 [uplevel eval norm [vec $x1 $y1 $z1 $x2 $y2 $z2]]
    set v13 [uplevel eval norm [vec $x1 $y1 $z1 $x3 $y3 $z3]]
    set vn [eval cross $v12 $v13]
    set N [eval module $vn]
    if [expr $N < 1e-7] {
        puts "points are on a line"
        return
    }
    concat $x1 $y1 $z1 [eval norm $vn]
}

help module {module x y z
  returns module of a vector\
} {Vector and measurement Commands}

proc module {x y z} {
    uplevel dval sqrt(($x)*($x)+($y)*($y)+($z)*($z))
}

help 2dmodule {2dmodule x y
  returns module of a 2D vector\
} {Vector and measurement Commands}

proc 2dmodule {x y} {
    uplevel dval sqrt(($x)*($x)+($y)*($y))
}

help norm {norm x y z
  returns unified vector from a given vector\
} {Vector and measurement Commands}

proc norm {x y z} {
    set N [uplevel dval sqrt(($x)*($x)+($y)*($y)+($z)*($z))]
    list [uplevel dval ($x)/$N] [uplevel dval ($y)/$N] [uplevel dval ($z)/$N]
}

help 2dnorm {2dnorm x y
  returns unified vector from a given 2D vector\
} {Vector and measurement Commands}

proc 2dnorm {x y} {
    set N [uplevel dval sqrt(($x)*($x)+($y)*($y))]
    list [uplevel dval ($x)/$N] [uplevel dval ($y)/$N]
}

help inverse {inverse x y z
  returns inversed vector\
} {Vector and measurement Commands}

proc inverse {x y z} {
    list [uplevel dval -$x] [uplevel dval -$y] [uplevel dval -$z]
}

help 2dinverse {2dinverse x y
  returns inversed 2D vector\
} {Vector and measurement Commands}

proc 2dinverse {x y} {
    list [uplevel dval -$x] [uplevel dval -$y]
}

help 2dort {2dort x y
  returns 2D vector rotated on 90 degrees\
} {Vector and measurement Commands}

proc 2dort {x y} {
    list [uplevel dval -$y] [uplevel dval $x]
}

help distpp {distpp x1 y1 z1 x2 y2 z2
  returns distance between two points\
} {Vector and measurement Commands}

proc distpp {x1 y1 z1 x2 y2 z2} {
    eval module [uplevel vec $x1 $y1 $z1 $x2 $y2 $z2]
}

help 2ddistpp {2ddistpp x1 y1 x2 y2
  returns distance between two 2D points\
} {Vector and measurement Commands}

proc 2ddistpp {x1 y1 x2 y2} {
    eval 2dmodule [uplevel 1 2dvec $x1 $y1 $x2 $y2]
}

help distplp {distplp xo yo zo dx dy dz xp yp zp
  returns distance between plane and point\
} {Vector and measurement Commands}

proc distplp {xo yo zo dx dy dz xp yp zp} {
    set vop [uplevel vec $xo $yo $zo $xp $yp $zp]
    set vn [uplevel norm $dx $dy $dz]
    eval dot $vop $vn
}

help distlp {distlp xo yo zo dx dy dz xp yp zp
  returns distance between line and point\
} {Vector and measurement Commands}

proc distlp {xo yo zo dx dy dz xp yp zp} {
    set vop [uplevel vec $xo $yo $zo $xp $yp $zp]
    set vl [uplevel norm $dx $dy $dz]
    eval module [eval cross $vl $vop]
}

help 2ddistlp {2ddistlp xo yo dx dy xp yp
  returns distance between 2D line and point\
} {Vector and measurement Commands}

proc 2ddistlp {xo yo dx dy xp yp} {
    set vop [uplevel 1 2dvec $xo $yo $xp $yp]
    set vl [uplevel 1 2dnorm $dx $dy]
    eval 2dcross $vl $vop
}

help distppp {distppp x1 y1 z1 x2 y2 z2 x3 y3 z3
  returns deviation of point p2 from segment p1-p3\
} {Vector and measurement Commands}

proc distppp {x1 y1 z1 x2 y2 z2 x3 y3 z3} {
    set vop [uplevel vec $x1 $y1 $z1 $x2 $y2 $z2]
    set vl [uplevel eval norm [vec $x1 $y1 $z1 $x3 $y3 $z3]]
    eval module [eval cross $vl $vop]
}

help 2ddistppp {2ddistppp x1 y1 x2 y2 x3 y3
  returns deviation of 2D point p2 from segment p1-p3 (sign shows the side)\
} {Vector and measurement Commands}

proc 2ddistppp {x1 y1 x2 y2 x3 y3} {
    set vop [uplevel 1 2dvec $x1 $y1 $x2 $y2]
    set vl [uplevel eval 2dnorm [2dvec $x1 $y1 $x3 $y3]]
    eval 2dcross $vl $vop
}

help barycen {barycen x1 y1 z1 x2 y2 z2 par
  returns point of a given parameter between two points\
} {Vector and measurement Commands}

proc barycen {x1 y1 z1 x2 y2 z2 par} {
    uplevel list [dval ($x1)*(1-($par))+($x2)*($par)]\
        [dval ($y1)*(1-($par))+($y2)*($par)]\
        [dval ($z1)*(1-($par))+($z2)*($par)]
}

help 2dbarycen {2dbarycen x1 y1 x2 y2 par
  returns 2D point of a given parameter between two points\
} {Vector and measurement Commands}

proc 2dbarycen {x1 y1 x2 y2 par} {
    uplevel list [dval ($x1)*(1-($par))+($x2)*($par)]\
        [dval ($y1)*(1-($par))+($y2)*($par)]
}

help cross {cross x1 y1 z1 x2 y2 z2
  returns cross product of two vectors\
} {Vector and measurement Commands}

proc cross {x1 y1 z1 x2 y2 z2} {
    set x [uplevel dval ($y1)*($z2)-($z1)*($y2)]
    set y [uplevel dval ($z1)*($x2)-($x1)*($z2)]
    set z [uplevel dval ($x1)*($y2)-($y1)*($x2)]
    list $x $y $z
}

help 2dcross {2dcross x1 y1 x2 y2
  returns cross product of two 2D vectors\
} {Vector and measurement Commands}

proc 2dcross {x1 y1 x2 y2} {
    uplevel dval ($x1)*($y2)-($y1)*($x2)
}

help dot {dot x1 y1 z1 x2 y2 z2
  returns scalar product of two vectors\
} {Vector and measurement Commands}

proc dot {x1 y1 z1 x2 y2 z2} {
    uplevel dval ($x1)*($x2)+($y1)*($y2)+($z1)*($z2)
}

help 2ddot {2ddot x1 y1 x2 y2
  returns scalar product of two 2D vectors\
} {Vector and measurement Commands}

proc 2ddot {x1 y1 x2 y2} {
    uplevel dval ($x1)*($x2)+($y1)*($y2)
}

help scale {scale x y z factor
  returns vector multiplied by scalar\
} {Vector and measurement Commands}

proc scale {x y z factor} {
    list [dval $x*$factor] [dval $y^$factor] [dval $z*$factor]
}

help 2dscale {2dscale x y factor
  returns 2D vector multiplied by scalar\
} {Vector and measurement Commands}

proc 2dscale {x y factor} {
    list [dval $x*$factor] [dval $y^$factor]
}

help pntc {pntc curve u
  returns coordinates of point on curve with given parameter\
} {Vector and measurement Commands}

proc pntc {curv u} {
    upvar \#0 $curv c
    cvalue c $u x y z
    return "[dval x] [dval y] [dval z]"
}

help 2dpntc {2dpntc curv2d u
  returns coordinates of 2D point on 2D curve with given parameter\
} {Vector and measurement Commands}

proc 2dpntc {curv2d u} {
    upvar \#0 $curv2d c
    2dcvalue c $u x y
    return "[dval x] [dval y]"
}

help pntsu {pntsu surf u v
  returns coordinates of point on surface with given parameters\
} {Vector and measurement Commands}

proc pntsu {surf u v} {
    upvar \#0 $surf s
    svalue s $u $v x y z
    return "[dval x] [dval y] [dval z]"
}

help pntcons {pntcons curv2d surf u
  returns coordinates of point on surface defined by 
  point on 2D curve with given parameter\
} {Vector and measurement Commands}

proc pntcons {curv2d surf u} {
    upvar \#0 $curv2d c $surf s
    2dcvalue c $u u0 v0
    svalue s u0 v0 x y z
    return "[dval x] [dval y] [dval z]"
}

help pnt {pnt point_or_vertex
  returns coordinates of point in the given Draw variable of type point or vertex\
} {Vector and measurement Commands}

proc pnt var {
    upvar \#0 $var v
    set type [dtyp v]
    set pp v
    if {[lindex $type 1] == "VERTEX"} {
        mkpoint p v
        set pp p
        set type "point"
    }
    if {$type == "point"} {
        if [catch {coord $pp x y z}] {
            if ![catch {coord $pp x y}] {
                return "[dval x] [dval y]"
            }
        } else {
            return "[dval x] [dval y] [dval z]"
        }
    }
}

help drseg {drseg name x1 y1 z1 x2 y2 z2
  creates a trimmed line between two points\
} {Vector and measurement Commands}

proc drseg {name x1 y1 z1 x2 y2 z2} {
    set x [uplevel dval $x1]
    set y [uplevel dval $y1]
    set z [uplevel dval $z1]
    set dx [uplevel dval ($x2)-($x1)]
    set dy [uplevel dval ($y2)-($y1)]
    set dz [uplevel dval ($z2)-($z1)]
    set len [module $dx $dy $dz]
    uplevel line $name $x $y $z $dx $dy $dz
    uplevel trim $name $name 0 $len
}

help 2ddrseg {2ddrseg name x1 y1 x2 y2
  creates a trimmed 2D line between two 2D points\
} {Vector and measurement Commands}

proc 2ddrseg {name x1 y1 x2 y2} {
    set x [uplevel dval $x1]
    set y [uplevel dval $y1]
    set dx [uplevel dval ($x2)-($x1)]
    set dy [uplevel dval ($y2)-($y1)]
    set len [2dmodule $dx $dy]
    uplevel line $name $x $y $dx $dy
    uplevel trim $name $name 0 $len
}

help mpick {show coordinates at mouse click\
} {Vector and measurement Commands}

proc mpick {} {
    puts "Pick position"
    pick id x1 y1 z1 b
    concat [dval x1] [dval y1] [dval z1]
}

help mdist {compute distance between two points of mouse clicks\
} {Vector and measurement Commands}

proc mdist {} {
    puts "Pick first position"
    pick id x1 y1 z1 b
    puts "Pick second position"
    pick id x2 y2 z2 b
    dval sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2))
}
