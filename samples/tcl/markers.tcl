# Markers demo
#
# It shows the various marker types supported by OCCT.

#Category: Visualization
#Title: Markers in 3d viewer

pload MODELING VISUALIZATION

# reflects Aspect_TypeOfMarker enumeration
set aMarkerTypeNames {
  Aspect_TOM_POINT
  Aspect_TOM_PLUS
  Aspect_TOM_STAR
  Aspect_TOM_X
  Aspect_TOM_O
  Aspect_TOM_O_POINT
  Aspect_TOM_O_PLUS
  Aspect_TOM_O_STAR
  Aspect_TOM_O_X
  Aspect_TOM_RING1
  Aspect_TOM_RING2
  Aspect_TOM_RING3
  Aspect_TOM_BALL
  Aspect_TOM_USERDEFINED
}

# custom marker
set aCustom1 [locate_data_file images/marker_box1.png]
set aCustom2 [locate_data_file images/marker_box2.png]
set aCustom3 [locate_data_file images/marker_kr.png]
set aCustom4 [locate_data_file images/marker_dot.png]

set aFontFile ""
catch { set aFontFile [locate_data_file DejaVuSans.ttf] }
set aLabelFont "Arial"
if { "$aFontFile" != "" } {
  vfont add "$aFontFile" SansFont
  set aLabelFont "SansFont"
}

# reset the viewer
vclear
vclose ALL
vinit name=View1 l=32 t=32 w=512 h=512

puts "Draw box in advance which should fit all our markers"
box b -8 -8 0 16 16 2
vbottom
vdisplay -noupdate -dispmode 0 b
vfit
vremove -noupdate b

puts "Draw markers of different type and size"
for { set aMarkerType 0 } { $aMarkerType <= 13 } { incr aMarkerType } {
  set aRow [expr $aMarkerType - 7]
  set aCol 5
  set aName [lindex $aMarkerTypeNames $aMarkerType]
  vdrawtext "$aName" "$aName" -pos 0 [expr $aRow + 0.5] 0 -color 0.5 1.0 1.0 -halign center -valign center -angle 000 -zoom 0 -height 12 -aspect bold -font $aLabelFont -noupdate
  vdisplay -top -noupdate "$aName"
  if { $aMarkerType == 13 } {
    vmarkerstest m${aMarkerType}_${aCol} $aCol $aRow 0 PointsOnSide=1 FileName=$aCustom1
    set aCol [expr $aCol - 1]
    vmarkerstest m${aMarkerType}_${aCol} $aCol $aRow 0 PointsOnSide=1 FileName=$aCustom2
    set aCol [expr $aCol - 1]
    vmarkerstest m${aMarkerType}_${aCol} $aCol $aRow 0 PointsOnSide=1 FileName=$aCustom3
    set aCol [expr $aCol - 1]
    vmarkerstest m${aMarkerType}_${aCol} $aCol $aRow 0 PointsOnSide=1 FileName=$aCustom4
  } else {
    for { set aMarkerScale 1.0 } { $aMarkerScale <= 7 } { set aMarkerScale [expr $aMarkerScale + 0.5] } {
      vmarkerstest m${aMarkerType}_${aCol} $aCol $aRow 0 MarkerType=$aMarkerType Scale=$aMarkerScale PointsOnSide=1
      set aCol [expr $aCol - 1]
    }
  }
}
puts "All markers have been displayed"
