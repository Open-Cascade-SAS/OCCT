# Demo script for dimensions
puts "Dimensions demo: it shows capability of OCC to create different kinds "
puts "of dimensions (linear and angular) with 2D or 3D text."

set dispScriptFile [file normalize [info script]]
set scriptFolder   [file dirname $dispScriptFile]

set aTopLoc [locate_data_file occ/Top.brep]
set aBotLoc [locate_data_file occ/Bottom.brep]
set aHatch  [locate_data_file images/hatch_1.png]

if { ![file exist "$aTopLoc"] || ![file exist "$aBotLoc"] } {
  puts "No model file in current directory!"
  puts "Please put Bottom.brep and Top.brep in current directory and try again"
}

pload MODELING VISUALIZATION
restore $aTopLoc a
restore $aBotLoc b
vinit View1 w=768 h=768
vclear
vsetdispmode 0
vsetcolorbg 255 255 255
vbottom

# Get cut projection
box bb -150 -250 0 150 250 100
bsection bs b bb
bsection as a bb
vdisplay bs as
vfit

set anAEdges [explode as E]
set aBEbges  [explode bs E]

#foreach e [concat $anAEdges $aBEbges] { vdisplay $e }
set anArrAngle [expr 3.14 * 12.0 / 180.0]

set aList {das_7 dbs_27 dbs_6 dbs_19 das_25 das_26 dbs_22 das_43 das_12 das_41 dbs_39 dbs_59 das_3944 dbs_1826 das_4843}

vdimension dbs_19 -length -shapes bs_19 -plane xoy -color black -flyout -15
vdimension dbs_6 -length -shapes bs_6 -plane xoy -color black -flyout 15
vdimension dbs_27 -length -shapes bs_27 -plane xoy -color black -label right -flyout -27
vdimension das_7 -length -shapes as_7 -plane xoy -color black -flyout -20
vdimension das_25 -length -shapes as_25 -plane xoy -color black -flyout -15
vdimension das_26 -length -shapes as_26 -plane xoy -color black -flyout 30
vdimension dbs_22 -length -shapes bs_22 -plane xoy -color black -flyout -20
vdimension das_43 -length -shapes as_43 -plane xoy -color black -flyout 55 -label right
vdimension das_12 -length -shapes as_12 -plane xoy -color black -flyout 35 -label right
vdimension das_41 -length -shapes as_41 -plane xoy -color black -flyout 15

vdimension dbs_39 -radius -shapes bs_39 -color black -label right
vdimension dbs_59 -radius -shapes bs_59 -color black

vdimension das_3944 -angle -shapes as_39 as_44 -color black
vdimension dbs_1826 -angle -shapes bs_18 bs_26 -color black
vdimension das_4843 -angle -shapes as_48 as_43 -color black

foreach i $aList {
  vdimparam $i text=3d textsize=6 textmode=s arlength=4 arangle=$anArrAngle
}

# Final fit
foreach e [concat $anAEdges $aBEbges] {
  vremove $e
}
vfit

vdisplay a b
vsetdispmode a 1
vsetdispmode b 1
vaspects a -setmaterial steel
vaspects b -setmaterial bronze

# set clipping plane
vclipplane create pa
vclipplane change pa equation 0 0 1 0
vclipplane change pa capping on
vclipplane change pa capping color 0.9 0.9 0.9
#vclipplane change pa capping hatch on
vclipplane set    pa object a

vclipplane create pb
vclipplane change pb equation 0 0 1 0
vclipplane change pb capping on
vclipplane change pb capping color 1.0 0.8 0.0
#vclipplane change pb capping hatch on
vclipplane set    pb object b

vsettexturemode Driver1/Viewer1/View1 2
vclipplane change pa capping texname $aHatch
vclipplane change pa capping texscale 0.05 -0.05
vclipplane change pb capping texname $aHatch
vclipplane change pb capping texscale 0.05  0.05

# nice view
vbottom
vrotate -0.3 -0.3 0
vfit
vzoom 1.2
