# Script displays properties of different materials available in OCCT

#Category: Visualization
#Title: Material properties in viewer

set THE_MATERIALS {brass bronze copper gold jade neon_phc pewter obsidian plaster plastic satin silver steel stone chrome aluminium water glass diamond charcoal}
set THE_COLORS    {default red green blue1}
set THE_ROW_DIST  35

proc drawLabels {} {
  set x 20
  set y 15
  set r 0.098
  set g 0.098
  set b 0.098
  foreach aMatIter $::THE_MATERIALS {
    vdrawtext "$aMatIter" "$aMatIter" -pos $x $y 0 -color $r $g $b -halign right -valign center -angle 000 -zoom 0 -height 14 -aspect regular -font Arial
    incr y 10
  }
  set x 40
  set y  5
  foreach aColIter $::THE_COLORS {
    if { $aColIter == "red" } {
      set r 1.0
      set g 0.0
      set b 0
    } elseif { $aColIter == "green" } {
      set r 0.0
      set g 1.0
      set b 0.0
    } elseif { $aColIter == "blue1" } {
      set r 0.0
      set g 0.0
      set b 1.0
    }
    vdrawtext "$aColIter" "$aColIter" -pos $x $y 0 -color $r $g $b -halign center -valign center -angle 000 -zoom 0 -height 14 -aspect regular -font Arial
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
    uplevel #0 vdisplay     -noredraw $aSph $aBox
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

catch { vclose View1 }
vinit View1 w=768 h=768
vclear
vtop
vglinfo
vsetgradientbg 180 200 255 180 180 180 2
vlight change 0 pos -1 1 1
vsetdispmode 1
vrenderparams -msaa 8

# adjust scene bounding box
box bnd 0 0 0 180 210 1
vdisplay -noredraw bnd
vsetdispmode       bnd 0
vfit
vremove -noredraw  bnd

# draw spheres and boxes with different materials
drawLabels
drawObjects 0 ""
drawObjects 1 red
drawObjects 2 green
drawObjects 3 blue1
#vfit
vzfit
