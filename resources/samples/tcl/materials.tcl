# Script displays properties of different materials available in OCCT

#Category: Visualization
#Title: Material properties in viewer

set THE_MATERIALS {brass bronze copper gold jade neon_phc pewter obsidian plaster plastic satin silver steel stone chrome aluminium water glass diamond charcoal}
set THE_COLORS    {default red green blue}
set THE_ROW_DIST  35

proc drawLabels {} {
  set x 20
  set y 15
  foreach aMatIter $::THE_MATERIALS {
    vdrawtext "$aMatIter" "$aMatIter" -pos $x $y 0 -color GRAY10 -halign right -valign center -angle 000 -zoom 0 -height 14 -aspect regular -font Arial
    incr y 10
  }
  set x 40
  set y  5
  foreach aColIter $::THE_COLORS {
    set aLabColor "$aColIter"
    if { "$aColIter" == "default" } { set aLabColor BLACK }
    vdrawtext "$aColIter" "$aColIter" -pos $x $y 0 -color "$aLabColor" -halign center -valign center -angle 000 -zoom 0 -height 14 -aspect regular -font Arial
    incr x $::THE_ROW_DIST
  }
}

proc drawObjects {theRow theColor} {
  set aSize    4
  set aCtr    -2
  set aCounter 0
  set x [expr 30 + $theRow * $::THE_ROW_DIST]
  set y 15
  foreach aMatIter $::THE_MATERIALS {
    set aSph s${theRow}_${aCounter}
    set aBox b${theRow}_${aCounter}
    uplevel #0 psphere $aSph $aSize
    uplevel #0 box     $aBox $aCtr $aCtr $aCtr $aSize $aSize $aSize
    uplevel #0 ttranslate   $aSph $x $y 0
    uplevel #0 ttranslate   $aBox [expr $x + 10] $y 0
    uplevel #0 vdisplay     -noredraw -dispMode 1 $aSph $aBox
    uplevel #0 vsetmaterial -noredraw $aSph $aBox $aMatIter
    if {$theColor != ""} {
      uplevel #0 vsetcolor  -noredraw $aSph $aBox $theColor
    }
    incr aCounter
    incr y 10
  }
}

# setup 3D viewer content
pload MODELING VISUALIZATION

vclear
vclose ALL
vinit View1 w=768 h=768
vtop
vglinfo
vbackground -gradient B4C8FF B4B4B4 -gradientMode VERTICAL

vlight -change 0 -dir 0.577 -0.577 -0.577
vrenderparams -msaa 8

# adjust scene bounding box
box bnd 0 0 0 180 210 1
vdisplay -noredraw -dispMode 0 bnd
vfit
vremove -noredraw  bnd

# draw spheres and boxes with different materials
drawLabels
drawObjects 0 ""
drawObjects 1 red
drawObjects 2 green
drawObjects 3 blue1
vrepaint
