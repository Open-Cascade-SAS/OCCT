# Script demonstrating PBR metallic-roughness material properties
#Category: Visualization
#Title: PBR metallic-rougness spheres

pload XDE OCAF MODELING VISUALIZATION
psphere s 0.35

catch { Close D }
XNewDoc D

# grid of spheres
set THE_UPPER 6
foreach i [list 0 3] {
  set aPrefix ""
  set aColor "GRAY80"
  if { $i != 0 } {
    set aPrefix "g_";
    set aColor "CCB11D"
  }
  set aColShapes {}
  for { set m 0 } { $m <= $THE_UPPER } { incr m } {
    set aRowShapes {}
    for { set r 0 } { $r <= $THE_UPPER } { incr r } {
      set aName ${aPrefix}m${m}r${r}
      copy s $aName
      lappend aRowShapes $aName
      ttranslate $aName ${r} ${i} ${m}
    }
    set aName ${aPrefix}m${m}
    compound {*}$aRowShapes $aName
    lappend aColShapes $aName
  }
  set aName ${aPrefix}spheres
  compound {*}$aColShapes $aName
  set aLabName "Gray Spheres"
  if { $i != 0 } { set aLabName "Golden Spheres" }
  set aLabComp [XAddShape D $aName 0]
  SetName D $aLabComp $aLabName

  for { set m 0 } { $m <= $THE_UPPER } { incr m } {
    set aMet [expr 100 * ${m}/$THE_UPPER]
    set aName ${aPrefix}m${m}
    XAddComponent D $aLabComp $aName
    set aLabCompCol [XFindShape D $aName]
    SetName D $aLabCompCol "${aPrefix}m${aMet}%"
    SetName D {*}[XFindComponent D $aName] "${aPrefix}m${aMet}%"
    for { set r 0 } { $r <= $THE_UPPER } { incr r } {
      set aRoug [expr 100 * ${r}/$THE_UPPER]
      set aName ${aPrefix}m${m}r${r}
      XAddComponent D $aLabCompCol $aName
      set aLab [XFindComponent D $aName]
      SetName D {*}$aLab "${aPrefix}m${aMet}%_r${aRoug}%"
      XAddVisMaterial D $aName -baseColor $aColor -metallic ${m}/$THE_UPPER -roughness ${r}/$THE_UPPER
      XSetVisMaterial D {*}$aLab $aName
    }
  }
}
set aLab [XFindShape D s]
SetName D {*}$aLab "Sphere"

XGetAllVisMaterials D

# labels
text2brep tm  "Metal"     -plane 0 -1 0 0 0 -1 -height 0.5 -pos -0.5 0  6.5 -halign left  -valign top -font monospace
text2brep tnm "Non-metal" -plane 0 -1 0 0 0 -1 -height 0.5 -pos -0.5 0 -0.5 -halign right -valign top -font monospace
text2brep ts  "Smooth"    -plane 0 -1 0 1 0  0 -height 0.5 -pos -0.5 0 -0.5 -halign left  -valign top -font monospace
text2brep tr  "Rough"     -plane 0 -1 0 1 0  0 -height 0.5 -pos  6.5 0 -0.5 -halign right -valign top -font monospace
compound tm tnm ts tr labs
set aLab [XAddShape D labs 0]
SetName D $aLab "Labels"
XAddComponent D $aLab tm
XAddComponent D $aLab tnm
XAddComponent D $aLab ts
XAddComponent D $aLab tr
SetName D {*}[XFindComponent D tm]  "Metal"
SetName D    [XFindShape     D tm]  "Metal"
SetName D {*}[XFindComponent D tnm] "Non-metal"
SetName D    [XFindShape     D tnm] "Non-metal"
SetName D {*}[XFindComponent D ts]  "Smooth"
SetName D    [XFindShape     D ts]  "Smooth"
SetName D {*}[XFindComponent D tr]  "Rough"
SetName D    [XFindShape     D tr]  "Rough"

vclear
vinit View1 -width 768 -height 768
vfront
vrenderparams -shadingModel PBR
vlight -change 0 -intensity 2.5
XDisplay -dispMode 1 D
vcamera -ortho
vfit
