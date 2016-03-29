# Dimensions demo
#
# It shows capability of OCC to create different kinds
# of dimensions (linear and angular) with 2D or 3D text.

#Category: Visualization
#Title: Dimensions in 3d viewer

set aTopLoc [locate_data_file occ/Top.brep]
set aBotLoc [locate_data_file occ/Bottom.brep]
set aHatch  [locate_data_file images/hatch_1.png]

pload MODELING VISUALIZATION

puts "Importing shapes..."
restore $aTopLoc a
restore $aBotLoc b

vinit View1 w=768 h=768
vclear
vsetdispmode 0
vsetcolorbg 255 255 255
vbottom

puts "Getting cut projection..."
box bb -150 -250 0 150 250 100
bsection bs b bb
bsection as a bb
vdisplay bs as
vfit

set anAEdges [explode as E]
set aBEbges  [explode bs E]

set anArrAngle [expr 3.14 * 12.0 / 180.0]

# Form dimension names list to set parameters with vdimparam command
set aList {rd_1 rd_2 ad_1 ad_2 ad_3}
for {set i 1} {$i <= 10} {incr i} {
lappend aList ld_$i
}

puts "Creating dimensions..."
vdimension ld_1 -length -shapes bs_27 -plane xoy -color black -flyout -15
vdimension ld_2 -length -shapes bs_14 -plane xoy -color black -flyout 15
vdimension ld_3 -length -shapes bs_28 -plane xoy -color black -label right -flyout -27
vdimension ld_4 -length -shapes as_7 -plane xoy -color black -flyout -20
vdimension ld_5 -length -shapes as_28 -plane xoy -color black -flyout -15
vdimension ld_6 -length -shapes as_18 -plane xoy -color black -flyout 30
vdimension ld_7 -length -shapes bs_20 -plane xoy -color black -flyout -20
vdimension ld_8 -length -shapes as_43 -plane xoy -color black -flyout 55 -label right
vdimension ld_9 -length -shapes as_12 -plane xoy -color black -flyout 35 -label right
vdimension ld_10 -length -shapes as_40 -plane xoy -color black -flyout 15

vdimension rd_1 -radius -shapes bs_45 -color black -label right
vdimension rd_2 -radius -shapes bs_63 -color black

vdimension ad_1 -angle -shapes as_38 as_49 -color black
vdimension ad_2 -angle -shapes bs_24 bs_25 -color black
vdimension ad_3 -angle -shapes as_48 as_43 -color black

puts "Changing text and arrow paramaters of dimensions..."
foreach i $aList {
  vdimparam $i -text 3d sh 6 -arrowlength 4 -arrowangle $anArrAngle
}

vfit

puts "Displaying exported shapes..."
vdisplay a b
vsetdispmode a 1
vsetdispmode b 1
vaspects a -setmaterial steel
vaspects b -setmaterial bronze

puts "Clipping shapes for better view..."
vclipplane create pa
vclipplane change pa equation 0 0 1 0
vclipplane change pa capping on
vclipplane change pa capping color 0.9 0.9 0.9
vclipplane set    pa object a

vclipplane create pb
vclipplane change pb equation 0 0 1 0
vclipplane change pb capping on
vclipplane change pb capping color 1.0 0.8 0.0
vclipplane set    pb object b

vclipplane change pa capping texname $aHatch
vclipplane change pa capping texscale 0.05 -0.05
vclipplane change pb capping texname $aHatch
vclipplane change pb capping texscale 0.05  0.05

vbottom
vrotate -0.3 -0.3 0
vfit
vzoom 1.2
vrenderparams -msaa 8

puts "Done."