# OCCT Test Migration Batches

Below are batches of Tcl scripts. Paste one batch at a time to the AI.

--- BATCH 1 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/buc60940
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: buc60940
- **Data Files**: `buc60940.rle` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"                                                                                      
puts "BUC60940"                                                                                                      
puts "========================"                                                                                      

restore [locate_data_file buc60940.rle] a 

fixshape result a 1e-7 1
checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/buc60950
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: buc60950
- **Data Files**: `buc60950.rle` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO CR23530 ALL: Faulty shapes in variables faulty_1 to faulty_"

puts "==========="
puts "  BUC60950"
puts "==========="

restore [locate_data_file buc60950.rle] a 
checkshape a

fixshape result a 1e-7

checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug14846
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug14846
- **Data Files**: `bug14846_Surf_Top.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC14846 ALL: Faulty shapes in variables faulty_1 to faulty_"

puts "========"
puts "OCC14846"
puts "========"
puts ""
###########################################################################
# ShapeFix_Wireframe::FixWireGaps() works wrong on a shape with seam edge
###########################################################################

restore [locate_data_file bug14846_Surf_Top.brep] a

checkshape a
fixwgaps result a
checkshape result

set bug_info_inp [string trim [lindex [tolerance a] 1]]
set bug_info_inp [string trim [string range $bug_info_inp [expr {[string last "=" $bug_info_inp] + 1}] [expr {[string length $bug_info_inp] - 1}]]]

set bug_info_out [string trim [lindex [tolerance result] 1]]
set bug_info_out [string trim [string range $bug_info_out [expr {[string last "=" $bug_info_out] + 1}] [expr {[string length $bug_info_out] - 1}]]]

set tmp_i [string trim [string range $bug_info_inp [expr {[string last "." $bug_info_inp] + 1}] [expr {[string length $bug_info_inp] - 1}]]]
set tmp_o [string trim [string range $bug_info_out [expr {[string last "=" $bug_info_out] + 1}] [expr {[string length $bug_info_out] - 1}]]]

set bug_info_value 0
if {[string length $bug_info_inp] < [string length $bug_info_out]} {
  set bug_info_value [string length $bug_info_inp]
} else {
  set bug_info_value [string length $bug_info_out]
}
if {$bug_info_value > 8} {
  set bug_info_value 8
}
set bug_info_inp [string trim [string range $bug_info_inp 0 [expr {[string last "." $bug_info_inp] + $bug_info_value}]]]
set bug_info_out [string trim [string range $bug_info_out 0 [expr {[string last "." $bug_info_out] + $bug_info_value}]]]

if {$bug_info_out > $bug_info_inp} {
  puts "OCC14846 is reproduced. Tolerance is increased after fixwgaps."
}
```

### Test: bugs/heal/bug1642
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug1642
- **Data Files**: `OCC1642.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"
puts " OCC1642 "
puts "========================"
puts ""
###########################################################
puts " ShapeFix_Wire::FixSelfIntersectingEdge does not remove a loop"
###########################################################

pload QAcommands
cpulimit 1000

restore [locate_data_file OCC1642.brep] shape

set list [OCC1642 result FinalFace InitWare InitFace shape FixReorder FixDegenerated FixConnected FixSelfIntersection]

set FixSelfIntersection [string last "FixSelfIntersection" $list]
set Intersectingpt [string last "Intersecting pt :" $list]

puts "FixSelfIntersection=${FixSelfIntersection}"
puts "Intersectingpt=${Intersectingpt}"
if { ${Intersectingpt} == -1} {
    puts "Warning: There are not intersecting points"
}

checkprops result -l 3880.52

if { ${Intersectingpt} > ${FixSelfIntersection} } {
    puts "OCC1642 Faulty"
} else {
    puts "OCC1642 OK"
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug17129
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug17129
- **Data Files**: `bug17129_ff_1285_proj.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC17129"
puts "============"
puts ""
#######################################################################
# ShapeFix projector makes 2d curves with oscillations
#######################################################################

restore [locate_data_file bug17129_ff_1285_proj.brep] a
fixshape r a

pcurve r

#v2d2
view 1 -2D-  728 20 400 400

2dfit
checkview -screenshot -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 1 END ---

--- BATCH 2 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug208
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug208
- **Data Files**: `OCC208.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC208"
puts "========"

##########################
## Incorrect work of ShapeFix
##########################

restore [locate_data_file OCC208.brep] a 

fixshape result a 0.01 0.01

checkshape result
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug209
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug209
- **Data Files**: `OCC209.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC209"
puts "========"

##########################################
## FixMissingSeam leads to lost pcurve on neighbouring face
##########################################

restore [locate_data_file OCC209.brep] a 
explode a f
checkshape a_4

fixshape result a 0.01 0.01
explode result f

checkshape result_4
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug210
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug210
- **Data Files**: `UKI60591.stp` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========="
puts " OCC210 "
puts "========="
puts ""
##################################################
# Improve FixShape to correct case of touching wires
##################################################
puts "TODO CR22598 ALL: Faulty shapes in variables faulty_1 to faulty_" 

pload XDE

cpulimit 1000

set BugNumber OCC210

set filepath [locate_data_file UKI60591.stp]
if [catch { stepread $filepath a * } result] {
    puts "Faulty ${BugNumber}: here is reading problem"
} else {
    puts "Reading ${BugNumber} OK"
    tpcompound r
    fixshape result r
    checkshape result
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug210_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug210_1
- **Data Files**: `OCC210a.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC210"
puts "(case 1)"
puts "========"

##########################################
## Improve FixShape to correct case of touching wires 
##########################################

restore [locate_data_file OCC210a.brep] a 

fixshape result a 0.01 0.01

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug210_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug210_2
- **Data Files**: `OCC210b.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC210"
puts "(case 2)"
puts "========"

##########################################
## Improve FixShape to correct case of touching wires 
##########################################

restore [locate_data_file OCC210b.brep] a 

fixshape result a 0.01 0.01

checkshape result
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 2 END ---

--- BATCH 3 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug21317
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug21317
- **Data Files**: `bug21317_faulty.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================"
puts "OCC21317"
puts "================"
puts ""
######################################################################################
# Face cannot be fixed by Shape Healing
######################################################################################

set BugNumber OCC21317

catch {pload XDE}

restore [locate_data_file bug21317_faulty.brep] a 
fixshape result a
set info [checkshape result]

# Resume
puts ""
if { [regexp {This shape seems to be valid} $info] } {
   puts "OK ${BugNumber}"
} else {
   puts "Faulty ${BugNumber}"
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug22
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug22
- **Data Files**: `OCC21.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======="
puts "OCC22"
puts "======="
puts ""
#####################################################################
##Locations are not correctly processed by ShapeBuild_ReShape (and base class BRepTools_ReShape)
#####################################################################

pload QAcommands

restore [locate_data_file OCC21.brep] shape
explode shape f
compound shape_1 shape_2 compoundToBeDivided

# Case A of the bug is reproduced. Resulting shape must have 5 faces instead of 3

if [catch { OCC22 result shape compoundToBeDivided 0 } ] {
    puts "OCC22 ERROR; (case 1)"
} else {
    puts "OCC22 OK; (case 1)"
}

regexp {([-0-9.+eE]+)\s FACE} [statshape result] full nbFaces
if {$nbFaces != 5} {
    puts "OCC22 FAULTY: Initial shape was not split"
}

# Case B of the bug is reproduced. Result must be valid
if [catch { OCC22 res shape compoundToBeDivided 1 } ] {
    puts "OCC22 ERROR; (case 2)"
} else {
    puts "OCC22 OK; (case 2)"
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug22587
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug22587
- **Data Files**: `OCC22587-face.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================"
puts "OCC22587"
puts "================"
puts ""
######################################################################################
# Problem with missing seam fixer
######################################################################################

set BugNumber OCC22587

catch {pload XDE}

restore [locate_data_file OCC22587-face.brep] f 
fixshape result f -l -o -h +m -d -s -i -n
set info [checkshape result]

# Resume
puts ""
if { [regexp {This shape seems to be valid} $info] } {
   puts "OK ${BugNumber}"
} else {
   puts "Faulty ${BugNumber}"
}
checkprops result -s 290.283
checkshape result
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug22602
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug22602
- **Data Files**: `bug22602_notched_simple.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC22602 ALL: Error : notched simple file is WRONG"

puts "========"
puts "OCC22602"
puts "========"
puts ""
#############################################
# Improvement in notched healing
#############################################

restore [locate_data_file bug22602_notched_simple.brep] a

checkview -display a -2d -path ${imagedir}/${test_image}_1.png

fixshape result a -l -o -h -m -d -s -i +n

checkshape result

set nbshapes_expected "
Number of shapes in shape
 VERTEX    : 6
 EDGE      : 7
"
checknbshapes result -ref ${nbshapes_expected} -t -m "notched simple file"

checkview -display result -2d -path ${imagedir}/${test_image}_2.png
```

### Test: bugs/heal/bug22642
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug22642
- **Data Files**: `bug22642_ss_47.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC22642 ALL: Faulty shapes in variables faulty_1 to faulty_"

puts "========"
puts "OCC22642"
puts "========"
puts ""
######################################################
# Refactoring of ShapeFix_Wire::FixIntersectingEdges
######################################################

restore [locate_data_file bug22642_ss_47.brep] f
fixshape result f -l -o -h -m -d -s +i -n

checkshape result
```

--- BATCH 3 END ---

--- BATCH 4 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug22919
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug22919
- **Data Files**: `bug22919_ss.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC22919"
puts "========"
puts ""
#############################################
# Shape Healing fails to correct a face with missing seam
#############################################

restore [locate_data_file bug22919_ss.brep] ss
explode ss

fixshape ok1 ss_1
whatis ok1
checkshape ok1

fixshape ok3 ss_3
whatis ok3
checkshape ok3

fixshape ok5 ss_5
whatis ok5
checkshape ok5

fixshape ko2 ss_2
whatis ko2
checkshape ko2

fixshape ko4 ss_4
whatis ko4
checkshape ko4
```

### Test: bugs/heal/bug22924
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug22924
- **Data Files**: `bug22924_3D_Sketcher.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================"
puts "OCC22924"
puts "================"
puts ""
######################################################################################
# We need more stable work of ShapeAnalysis_FreeBounds::ConnectEdgesToWires()
######################################################################################

set BugNumber OCC22924

pload XDE

set order {1 2 3 4 5 6 7 8 9 10}
restore [locate_data_file bug22924_3D_Sketcher.brep] a 
set err 0
set numcheck 100
set lord [split $order " "]
for {set i 1} {$i <= $numcheck} {incr i} {
    set val [connectedges r a]
    set lnum [lindex [split $val \n] 1]
    set llnum [split $lnum " "]
    set ll [llength $llnum]
    for {set j 0} {$j <= $ll} {incr j} {
        set ref_val [lindex $lord $j]
        set check_val [lindex $llnum $j]
	if {$ref_val == $check_val} {
            continue
	}
	incr err
    }
}

if $err  {
    puts "Error $err"
    puts "Faulty ${BugNumber}"
} else {
    puts "OK"
}
```

### Test: bugs/heal/bug23451
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug23451
- **Data Files**: `bug23451_testface_holes.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO All: Faulty shapes in variables faulty_1 to faulty_"

puts "================"
puts "OCC23451"
puts "================"
puts ""
######################################################################################
# FixMissingSeam constructs intersecting wires
######################################################################################

restore [locate_data_file bug23451_testface_holes.brep] f

fixshape result f

checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug23722
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug23722
- **Data Files**: `bug23722_face.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "CR23722"
puts "========"
puts ""
#############################
## stack overflow during reading IGES in Test Harness
#############################

pload IGES

restore [locate_data_file bug23722_face.brep]

brepiges bug23722_face ${imagedir}/face.igs

igesbrep ${imagedir}/face.igs a *
```

### Test: bugs/heal/bug23944
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug23944
- **Data Files**: `bug23944_s.draw` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC23944"
puts "============"
puts ""
####################################################################################
## Typo in ShapeCustom_RestrictionParameters leading to wrong approximation results
####################################################################################

restore [locate_data_file bug23944_s.draw] s

mkface f s
bsplres result f 1e-4 1e-9 15 100 C1 C2 0 1
mksurface rsurf result

set info [getsurfcontinuity rsurf]

if { ![regexp {rsurf has C0 continuity.} $info] } {
  puts "Faulty OCC23944"
}
```

--- BATCH 4 END ---

--- BATCH 5 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug24
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24
- **Data Files**: `OCC24.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
pload QAcommands

puts "======="
puts "OCC24"
puts "======="
puts ""
###########################################################################
## Recorded modifications are lost when converting from ShapeBuild_ReShape into  ShapeProcess_ShapeContext
## and back
###########################################################################

restore [locate_data_file OCC24.brep] s
checkshape s

explode s f

compound s_1 c
if [catch { OCC24 result s c OCC24 Seq } ] {
  puts "OCC24 ERROR"
} else {
  puts "OCC24 OK"
}

checkprops result -s 37.6991
checkshape result
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug24008_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24008_1
- **Data Files**: `bug24008_curv_6964.draw, bug24008_surf_6962.draw` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC24008"
puts "========"
puts ""
#################################################################
# ShapeAnalysis_Surface causes exception in Geom_OffsetSurface
#################################################################

pload QAcommands

restore [locate_data_file bug24008_curv_6964.draw] a
restore [locate_data_file bug24008_surf_6962.draw] b

set info [OCC24008 a b]
if { [regexp "Exception" $info] != 0 } {
    puts "Error : Exception was caught"
} else {
    puts "OK: Command wrong property"
}
```

### Test: bugs/heal/bug24008_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24008_2
- **Data Files**: `bug24008_curv_rbp.draw, bug24008_surf_rbls.draw` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC24008"
puts "========"
puts ""
#################################################################
# ShapeAnalysis_Surface causes exception in Geom_OffsetSurface
#################################################################

pload QAcommands

restore [locate_data_file bug24008_curv_rbp.draw] a
restore [locate_data_file bug24008_surf_rbls.draw] b

set info [OCC24008 a b]
if { [regexp "Exception" $info] != 0 } {
    puts "Error : Exception was caught"
} else {
    puts "OK: Command wrong property"
}
```

### Test: bugs/heal/bug24105_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24105_1
- **Data Files**: `bug24105_shape.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================"
puts "OCC24105"
puts "================"
puts ""
######################################################################################
# ShapeFix algorithm produces not correct result
# 1. geom/imps7/H6
######################################################################################

catch {pload XDE}

restore [locate_data_file bug24105_shape.brep] s

settolerance s 1.e-7

fixshape result s

checkprops result -s 313292
checkshape result

checknbshapes result -vertex 12 -edge 20 -wire 9 -face 9 -shell 1 -solid 1 -compsolid 0 -compound 0 -shape 52
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug24105_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24105_2
- **Data Files**: `bug24105_shape1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================"
puts "OCC24105"
puts "================"
puts ""
######################################################################################
# ShapeFix algorithm produces not correct result
# 2. geom/SWIG_scripts/C9
######################################################################################

catch {pload XDE}

restore [locate_data_file bug24105_shape1.brep] s

settolerance s 1.e-7

fixshape result s

checkprops result -s 123446
checkshape result

checknbshapes result -vertex 33 -edge 52 -wire 23 -face 21 -shell 1 -solid 1 -compsolid 0 -compound 1 -shape 132
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 5 END ---

--- BATCH 6 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug24111
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24111
- **Data Files**: `bug24111_shape.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================"
puts "OCC24111"
puts "================"
puts ""
######################################################################################
# Shape Healing produces invalid face
######################################################################################

catch {pload XDE}

restore [locate_data_file bug24111_shape.brep] a

fixshape result a

checkprops result -s 15902.8
checkshape result

checknbshapes result -vertex 14 -edge 15 -wire 1 -face 1 -shell 0 -solid 0 -compsolid 0 -compound 0 -shape 31
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug24126
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24126
- **Data Files**: `bug24126_Partition_1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================"
puts "OCC24126"
puts "================"
puts ""
######################################################################################
# Crash on fixing the attached shape
######################################################################################

catch {pload XDE}

restore [locate_data_file bug24126_Partition_1.brep] b

fixshape result b
```

### Test: bugs/heal/bug24218
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24218
- **Data Files**: `bug24218_f.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC24218"
puts "============"
puts ""
#####################################################################################################
## ShapeFix_Face requires double execution to produce valid shape when FixSplitFaceMode is in effect
#####################################################################################################

restore [locate_data_file bug24218_f.brep] f

fixshape r f +o
checkshape r

fixshape rr r +o
checkshape rr
```

### Test: bugs/heal/bug24249_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24249_1
- **Data Files**: `bug24249_ShapeFix_Fail.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC24249"
puts "============"
puts ""
######################################################
# Crash on ShapeFix_Shape
######################################################

pload XDE

restore [locate_data_file bug24249_ShapeFix_Fail.brep] a

fixshape result a
```

### Test: bugs/heal/bug24249_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24249_2
- **Data Files**: `bug24249_a_149.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC24249"
puts "============"
puts ""
######################################################
# Crash on ShapeFix_Shape
######################################################

pload XDE

restore [locate_data_file bug24249_a_149.brep] a_149

fixshape f a_149

fixshape result f
```

--- BATCH 6 END ---

--- BATCH 7 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug24249_3
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24249_3
- **Data Files**: `bug24249_a_149.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC24249"
puts "============"
puts ""
######################################################
# Crash on ShapeFix_Shape
######################################################

pload XDE

restore [locate_data_file bug24249_a_149.brep] a_149

mksurface surf a_149
mkface face surf

fixshape result face
```

### Test: bugs/heal/bug24370
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24370
- **Data Files**: `bug24370_edge.brep, bug24370_pcurve_draw, bug24370_surface_draw` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC24370"
puts "============"
puts ""
###################################################
## [Regression] 6.7.0beta ShapeFix_EdgeProjAux breaks conventions on using IsDone flag
###################################################

pload QAcommands

restore [locate_data_file bug24370_edge.brep] edge
restore [locate_data_file bug24370_pcurve_draw] pcurve
restore [locate_data_file bug24370_surface_draw] surface
set precision 0.001

set info [OCC24370 edge pcurve surface ${precision}]

set IsFirstDone [lindex ${info} 0]
set IsLastDone [lindex ${info} 1]
set First [lindex ${info} 2]
set Last [lindex ${info} 3]

set expected_First 0
set tol_abs_First 1.0e-03
set tol_rel_First 0.01
checkreal "First" ${First} ${expected_First} ${tol_abs_First} ${tol_rel_First}

set expected_Last 1
set tol_abs_Last 1.0e-03
set tol_rel_Last 0.01
checkreal "Last" ${Last} ${expected_Last} ${tol_abs_Last} ${tol_rel_Last}
```

### Test: bugs/heal/bug24549
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24549
- **Data Files**: `bug24549_Hull_B.igs` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC24549"
puts "============"
puts ""
#######################################################################
## Faulty shape after IGES translation with xstep.cascade.unit set to M
#######################################################################

pload DATAEXCHANGEKERNEL IGES

param xstep.cascade.unit M

igesread [locate_data_file bug24549_Hull_B.igs] result *

regexp {Tolerance +MAX=([-0-9.+eE]+)} [tolerance result] full MaxTol_1

puts "MaxTolerance = $MaxTol_1"

set MaxTol 0.001

if { ${MaxTol_1} > ${MaxTol} } {
    puts "Faulty : bad tolerance"
} else {
    puts "OK : good tolerance"
}
```

### Test: bugs/heal/bug24658
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24658
- **Data Files**: `bug24658.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC24658"
puts "============"
puts ""
#######################################################################
## fixshape changes source shape
#######################################################################

restore [locate_data_file bug24658.brep] a

puts "\nBefore shape healing"
regexp {Tolerance +MAX=([-0-9.+eE]+)} [tolerance a] full expected_MaxTolerance

fixshape af a

puts "\nAfter shape healing"
regexp {Tolerance +MAX=([-0-9.+eE]+)} [tolerance a] full MaxTol

set tol_abs_MaxTolerance 1.0e-06
set tol_rel_MaxTolerance 1.0e-06
checkreal "MaxTolerance" ${MaxTol} ${expected_MaxTolerance} ${tol_abs_MaxTolerance} ${tol_rel_MaxTolerance}
```

### Test: bugs/heal/bug24881
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24881
```tcl
puts "========"
puts "OCC24881"
puts "========"
puts ""
##################################################################
# Wrong status returned by ShapeFix_Wire::FixGaps3d () operation
##################################################################

pload QAcommands

box b 100 200 300

set bug_info [OCC24881 b]

if {[lindex $bug_info 0] != "Fix_FillGaps_NothingToDo"} {
  puts "ERROR: OCC24881 is reproduced. Status is [lindex $bug_info 0]"
}
```

--- BATCH 7 END ---

--- BATCH 8 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug24934
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24934
- **Data Files**: `bug24934_f1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC24934"
puts "============"
puts ""
######################################################
# Section: crash for two faces
######################################################

restore [locate_data_file bug24934_f1.brep] f1

set info [fixshape ff1 f1]
if { [regexp "Exception" $info] != 0 } {
    puts "Error : Exception in \"fixshape\" command was caught"
} else {
    puts "OK: Command \"fixshape\" work properly"
}
```

### Test: bugs/heal/bug24983
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug24983
- **Data Files**: `bug24983_111FACE.stp` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "0024983: For the incorrect seam edge in STEP file no fix is provided"
puts "========"

pload STEP

stepread [locate_data_file bug24983_111FACE.stp] a *

tpcompound result
checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug25013_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25013_1
- **Data Files**: `bug25013_25013faces.stp` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========="
puts " OCC25013"
puts "========="
puts ""
##################################################
# ShapeFix_Wire tweaks for better results
##################################################

pload STEP

stepread [locate_data_file bug25013_25013faces.stp] a *

tpcompound result
checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug25013_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25013_2
- **Data Files**: `bug25013_25013face.stp` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========="
puts " OCC25013"
puts "========="
puts ""
##################################################
# ShapeFix_Wire tweaks for better results
##################################################

pload STEP

stepread [locate_data_file bug25013_25013face.stp] a *

tpcompound result
checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug25014
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25014
```tcl
puts "============"
puts "OCC25014"
puts "============"
puts ""
#######################################################################
## ShapeAnalysis_WireOrder produces Standard_RangeError on empty wire (debug mode only)
#######################################################################

shape w w
stwire w r +r
```

--- BATCH 8 END ---

--- BATCH 9 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug25068
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25068
- **Data Files**: `bug24807_Compound.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC25068"
puts "============"
puts ""
###################################################
## ShapeAnalysis_FreeBounds::ConnectEdgesToWires returns wires with not valid Closed flag
###################################################

restore [locate_data_file bug24807_Compound.brep] a
connectedges r a 1.e-7 0
explode r

set info [whatis r_1]

if { [regexp "Closed" ${info}] == 1 } {
    puts "Error: Closed flag is not valid"
} else {
    puts "OK: Closed flag is valid"
}
```

### Test: bugs/heal/bug25333
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25333
- **Data Files**: `bug25318_TheShapeTotal.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC25333"
puts "========"
puts ""
##########################################################################################
# Result wires obtained after TestHarness command "connectedges" contains internal edges
##########################################################################################

restore [locate_data_file bug25318_TheShapeTotal.brep] a
connectedges r a

set WireSet [explode r]
set ii 0
set IsFixed "TRUE"
while {$ii != [llength $WireSet]} {
  set EdgeSet [explode [lindex $WireSet $ii]]
  set iii 0
  while {$iii != [llength $EdgeSet]} {
    set EdgeInfo [whatis [lindex $EdgeSet $iii]]
    if {[lindex $EdgeInfo 5] == "INTERNAL"} {
      set IsFixed "FALSE"
    }
    set iii [expr $iii + 1]
  }
  set ii [expr $ii + 1]
}

if {$IsFixed == "FALSE"} {
  puts "ERROR: OCC25333 is reproduced"
  puts "       Internal edge is detected"
}
```

### Test: bugs/heal/bug25455
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25455
- **Data Files**: `bug25455_rx.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC25455"
puts "============"
puts ""
######################################################
# fixshape works at the second attempt
######################################################

restore [locate_data_file bug25455_rx.brep] rx

fixshape rx rx

set info [checkshape rx]

# Resume
puts ""
if { [regexp {This shape seems to be valid} ${info}] } {
   puts "OK: fixshape works properly"
} else {
   puts "Error: fixshape works bad"
}
```

### Test: bugs/heal/bug25553_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25553_1
- **Data Files**: `bug25553_ii_73.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"                                                                                      
puts "Bug #25553"                                                                                                      
puts "========================"                                                                                      

brestore [locate_data_file bug25553_ii_73.brep] face

fixshape result face 1e-7 1
checkshape result
checknbshapes result -face 1

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug25553_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25553_2
- **Data Files**: `bug25553_f-cyl-no-seam-shifted-pcurves.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"                                                                                      
puts "Bug #25553"                                                                                                      
puts "========================"                                                                                      

brestore [locate_data_file bug25553_f-cyl-no-seam-shifted-pcurves.brep] face

fixshape result face 1e-7 1
checkshape result
checknbshapes result -face 1

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 9 END ---

--- BATCH 10 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug25553_3
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25553_3
- **Data Files**: `bug25553_f-cyl-no-seam-shifted-pcurves-2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"                                                                                      
puts "Bug #25553"                                                                                                      
puts "========================"                                                                                      

brestore [locate_data_file bug25553_f-cyl-no-seam-shifted-pcurves-2.brep] face

fixshape result face 1e-7 1
checkshape result
checknbshapes result -face 1

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug25634
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25634
- **Data Files**: `bug25634_shape3.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC25634"
puts "============"
puts ""
######################################################
# Checking of compliance of vertices and pcurve fails
######################################################

restore [locate_data_file bug25634_shape3.brep] s

explode s e

set info [checkedge s_1 s]

# Resume
puts ""
if { [regexp {Edge seems OK.} ${info}] } {
   puts "OK: Checking is good"
} else {
   puts "Error: Checking is bad"
}
```

### Test: bugs/heal/bug25712
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25712
- **Data Files**: `OCC25712_comp16.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC25712"
puts "========"
puts ""
################################################
# Non-deterministic behavior of ShapeFix_Solid
################################################

cpulimit 1000

set OK_shapes_1 177
set OK_shapes_2 9

for {set i 1} {$i <= 100} {incr i} {
  restore [locate_data_file OCC25712_comp16.brep] c
  fixshape rc c
  explode rc
  explode rc_2
  set bug_info_1 [numshapes rc_2_1]
  set bug_info_2 [numshapes rc_2_2]
  if {[lindex $bug_info_1 31] != $OK_shapes_1} {
    puts "ERROR: OCC25712 is reprodced. rc_2_1 should has $OK_shapes_1 shapes, but has [lindex $bug_info_1 31] shapes."
  }
  if {[lindex $bug_info_2 31] != $OK_shapes_2} {
    puts "ERROR: OCC25712 is reprodced. rc_2_2 should has $OK_shapes_2 shapes, but has [lindex $bug_info_2 31] shapes."
  }
}
```

### Test: bugs/heal/bug25923
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25923
- **Data Files**: `OCC25923_FixSmallWire_Orientation.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "0025923: Remove small wires on face read from STEP"
puts "========"
puts ""

restore [locate_data_file OCC25923_FixSmallWire_Orientation.brep] a

checkshape a

fixshape r a +s +o 1e-3

checkshape r
checknbshapes r -face 1 -wire 2 -edge 32 -vertex 32
```

### Test: bugs/heal/bug25967
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug25967
- **Data Files**: `bug25967_face-no-sh-fwd.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC25967 ALL: Error :  is WRONG because number of EDGE entities in shape"

puts "========"
puts "OCC25967"
puts "========"
puts ""
########################################################################
# Shape Healing fails to correct a face with missing poles and pcurves
########################################################################

restore [locate_data_file bug25967_face-no-sh-fwd.brep] f

fixshape result f
pcurve result
checknbshapes result -edge 4

v2d
2dfit
checkview -screenshot -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 10 END ---

--- BATCH 11 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug26052
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26052
```tcl
puts "TODO OCC26052 ALL: Faulty shapes in variables faulty_1 to faulty_"

puts "========"
puts "OCC26052"
puts "========"
puts ""
##############################################################################################
# Missing pcurves are computed incorrectly on two connected faces laying on the same surface
##############################################################################################

cylinder c 10
circle c1 0 0 0 10
circle c2 0 0 0 10
circle c3 0 0 10 10
circle c4 0 0 10 10
line l1 10 0 0 0 0 1
line l2 -10 0 0 0 0 1
trim c1 c1 0 pi
trim c2 c2 pi 2*pi
trim c3 c3 0 pi
trim c4 c4 pi 2*pi
trim l1 l1 0 10
trim l2 l2 0 10
vertex v1 10 0 0
vertex v2 -10 0 0
vertex v3 10 0 10
vertex v4 -10 0 10
mkedge ec1 c1 v1 v2
mkedge ec2 c2 v2 v1
mkedge ec3 c3 v3 v4
mkedge ec4 c4 v4 v3
mkedge el1 l1 v1 v3
mkedge el2 l2 v2 v4
shape w1 W
#orientation ec3 R
#orientation el1 R
add ec1 w1
add el2 w1
add ec3 w1
add el1 w1
shape w2 W
#orientation el1 F
#orientation el2 R
#orientation ec4 R
add ec2 w2
add el1 w2
add ec4 w2
add el2 w2
mkface f1 c w1
mkface f2 c w2
shape s Sh
add f1 s
add f2 s
fixshape result s

checkshape result
```

### Test: bugs/heal/bug26280
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26280
- **Data Files**: `bug26280_1kento13-mld-b-cv-qubit.stp` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC26280 ALL: Faulty shapes in variables faulty_1 to faulty_"

puts "========"
puts "OCC26280"
puts "========"
puts ""
##################################################
# Mechanism 'ShapeFix' is unable to heal a shape
##################################################

pload STEP

stepread [locate_data_file bug26280_1kento13-mld-b-cv-qubit.stp] r *
checkshape r_1
```

### Test: bugs/heal/bug26282
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26282
- **Data Files**: `bug26282_mld-m-cv-qubit-th.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC26282 ALL: Faulty shapes in variables faulty_1 to faulty_"

puts "========"
puts "OCC26282"
puts "========"
puts ""
####################################################################
# Mechanism 'ShapeFix' has errors revealed by wire tool 'FixTails'
####################################################################


restore [locate_data_file bug26282_mld-m-cv-qubit-th.brep] s

checkshape s
fixshape result s 1e-4 1 -maxtaila 6 -maxtailw 1e-4

checkshape result
```

### Test: bugs/heal/bug26408
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26408
- **Data Files**: `OCC26408-shape_1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC26408"
puts "========"
puts ""
#######################################
# Exception during fixshape procedure
#######################################

restore [locate_data_file OCC26408-shape_1.brep] a
fixshape r a 1.e-7 1.
```

### Test: bugs/heal/bug26466
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26466
- **Data Files**: `OCC26466-invalid_unifysamedom_input.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC26466"
puts "========"
puts ""
#######################################################################
# UnifySameDomain creates invalid result shape from valid input shape
#######################################################################

restore [locate_data_file OCC26466-invalid_unifysamedom_input.brep] s
checkshape s
unifysamedom r s
checkshape r
```

--- BATCH 11 END ---

--- BATCH 12 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug26620
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26620
- **Data Files**: `bug26620_ff.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "REQUIRED ALL: Faulty shapes in variables faulty_1 to"

puts "============"
puts "OCC26620"
puts "============"
puts ""
#######################################################################
## Shape healing unreasonably downgrades tolerance of a face
#######################################################################

restore [locate_data_file bug26620_ff.brep] ff

puts "\nBefore shape healing"
checkshape ff
tolerance ff

fixshape r ff

puts "\nAfter shape healing"
checkshape r
regexp {Tolerance +MAX=([-0-9.+eE]+)} [tolerance r] full MaxTol

set expected_MaxTolerance 0.1
set tol_abs_MaxTolerance 0.5
set tol_rel_MaxTolerance 0.5
checkreal "MaxTolerance" ${MaxTol} ${expected_MaxTolerance} ${tol_abs_MaxTolerance} ${tol_rel_MaxTolerance}
```

### Test: bugs/heal/bug26635
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26635
- **Data Files**: `OCC26635_t0.brep, OCC26635_t1.brep, OCC26635_t2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC26635"
puts "========"
puts ""
########################################
# UnifySameDomain loses internal edges
########################################

restore [locate_data_file OCC26635_t0.brep] t0
restore [locate_data_file OCC26635_t1.brep] t1
restore [locate_data_file OCC26635_t2.brep] t2

bclear
baddobjects t0 t1
baddtools t2
bfillds
bbop r 1

unifysamedom ru r
set bug_info [bopargcheck ru #F]

if {$bug_info != "Shape(s) seem(s) to be valid for BOP.\n"} {
  puts "ERROR: OCC26635 is reproduced."
}
```

### Test: bugs/heal/bug26642
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26642
- **Data Files**: `bug26656_unify.input.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC26642"
puts "========"
puts ""
############################################################################
# ShapeUpgrade_UnifySameDomain introduces extremely high vertex tolerances
############################################################################

restore [locate_data_file bug26656_unify.input.brep] i

set bug_info [tolerance i]
set value_1 [lindex $bug_info 1]
set value_1 [string trim [string range $value_1 [expr {[string first "=" $value_1] + 1}] [expr {[string length $value_1] - 1}]]]

unifysamedom r i

set bug_info [tolerance i]
set value_2 [lindex $bug_info 1]
set value_2 [string trim [string range $value_2 [expr {[string first "=" $value_2] + 1}] [expr {[string length $value_2] - 1}]]]
set bug_info [tolerance r]
set value_3 [lindex $bug_info 1]
set value_3 [string trim [string range $value_3 [expr {[string first "=" $value_3] + 1}] [expr {[string length $value_3] - 1}]]]

if {$value_1 > $value_2} {
  set diff_12 [expr {$value_1 - $value_2}]
} else {
  set diff_12 [expr {$value_2 - $value_1}]
}

if {$value_1 > $value_3} {
  set diff_13 [expr {$value_1 - $value_3}]
} else {
  set diff_13 [expr {$value_3 - $value_1}]
}

if {$diff_12 > 1.e-06 || $diff_13 > 1.e-06} {
  puts "ERROR: OCC26642 is reproduced. Command unifysamedom works wrongly."
}
```

### Test: bugs/heal/bug26656
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26656
- **Data Files**: `bug26656_unify.input.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC26656"
puts "========"
puts ""
#######################################################################
# ShapeFix_Face introduces extremely high vertex tolerance in the input shape
#######################################################################

restore [locate_data_file bug26656_unify.input.brep] i

puts "\nBefore ShapeFix_Face"
regexp {Tolerance +MAX=([-0-9.+eE]+)} [tolerance i] full MaxTolerance_1

set expected_MaxTolerance 2.0024548532087701e-07
set tol_abs_MaxTolerance 5.0e-7
set tol_rel_MaxTolerance 0.1

puts "MaxTolerance_1 = $MaxTolerance_1"
checkreal "MaxTolerance" ${MaxTolerance_1} ${expected_MaxTolerance} ${tol_abs_MaxTolerance} ${tol_rel_MaxTolerance}

unifysamedom r i

puts "\nAfter ShapeFix_Face"
regexp {Tolerance +MAX=([-0-9.+eE]+)} [tolerance i] full MaxTolerance_2

puts "MaxTolerance_2 = $MaxTolerance_2"
checkreal "MaxTolerance" ${MaxTolerance_2} ${expected_MaxTolerance} ${tol_abs_MaxTolerance} ${tol_rel_MaxTolerance}
```

### Test: bugs/heal/bug26671
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26671
- **Data Files**: `bug26671.igs` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"                                                                                      
puts "0026671: Infinite loop in ShapeFix_Wire::FixSelfIntersection()"
puts "========================"                                                                                      

pload IGES

cpulimit 20
igesbrep [locate_data_file bug26671.igs] result *

checkshape result
tolerance result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 12 END ---

--- BATCH 13 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug26708
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26708
- **Data Files**: `bug26708_a_205.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"                                                                                      
puts "Bug #26708"                                                                                                      
puts "========================"                                                                                      

brestore [locate_data_file bug26708_a_205.brep] face

fixshape result face 1e-7 1
checkshape result
checknbshapes result -face 1

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug26930_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26930_1
- **Data Files**: `bug26930_surf_1.draw, bug26930_curv_1.draw` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC26930"
puts "========"
puts ""
#################################################################################
# ShapeConstruct_ProjectCurveOnSurface returns a B-Spline instead of line (again)
#################################################################################

pload QAcommands

restore [locate_data_file bug26930_surf_1.draw] s
restore [locate_data_file bug26930_curv_1.draw] c

OCC26930 s c 0.0 14.3316
```

### Test: bugs/heal/bug26930_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26930_2
- **Data Files**: `bug26930_surf_2.draw, bug26930_curv_2.draw` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC26930"
puts "========"
puts ""
#################################################################################
# ShapeConstruct_ProjectCurveOnSurface returns a B-Spline instead of line (again)
#################################################################################

pload QAcommands

restore [locate_data_file bug26930_surf_2.draw] s
restore [locate_data_file bug26930_curv_2.draw] c

OCC26930 s c 0.0 12.41152967687705
```

### Test: bugs/heal/bug26943
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26943
- **Data Files**: `bug26943_face_helix.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC26943 All: Faulty shapes in variables faulty_1 to faulty_"

puts "================"
puts "OCC26943"
puts "================"
puts ""
################################################################################
# Wrong projection of helix on cylinder by ShapeConstruct_ProjectCurveOnSurface
################################################################################

restore [locate_data_file bug26943_face_helix.brep] ff

fixshape result ff

checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug26957
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug26957
- **Data Files**: `bug26957-w-700.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC26957"
puts "============"
puts ""
###########################################################
## Unify same domain operation is slow for multiple shells
###########################################################

restore [locate_data_file bug26957-w-700.brep] a1

nbshapes a1
regexp {([-0-9.+eE]+)} [time {unifysamedom r1 a1}] full t1
puts "t1=$t1"

sewing a2 0.0 a1

puts ""
nbshapes a2
regexp {([-0-9.+eE]+)} [time {unifysamedom r2 a2}] full t2
puts "t2=$t2"

set tol_rel 0.1
if { [expr (${t1}*1. - ${t2}) / ${t2}] > ${tol_rel}} {
   puts "Error: Unify same domain operation is slow for multiple shells"
}
```

--- BATCH 13 END ---

--- BATCH 14 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug27000_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27000_1
- **Data Files**: `bug27000_1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
restore [locate_data_file bug27000_1.brep] shape
unifysamedom result shape
checkshape result
checknbshapes result -vertex 60 -edge 87 -wire 35 -face 33 -shell 2 -solid 2 -compsolid 0 -compound 1 -shape 220
```

### Test: bugs/heal/bug27000_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27000_2
- **Data Files**: `bug27000_2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
restore [locate_data_file bug27000_2.brep] shape
unifysamedom result shape
checkshape result
checknbshapes result -vertex 46 -edge 68 -wire 26 -face 26 -shell 2 -solid 2 -compsolid 0 -compound 1 -shape 171
```

### Test: bugs/heal/bug27004
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27004
- **Data Files**: `bug27004_prim_0_1.brep, bug27004_prim_0_29.brep, bug27004_prim_0_30.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC27004"
puts "========"
puts ""
#####################################################
# ShapeUpgrade_UnifySameDomain produces invalid result
#####################################################

restore [locate_data_file bug27004_prim_0_1.brep]  s0
restore [locate_data_file bug27004_prim_0_29.brep] s1
restore [locate_data_file bug27004_prim_0_30.brep] s2

bclear
bfuzzyvalue 1e-5

baddobjects s0 s1 s2
baddtools s0 s1 s2

bfillds

bbop r 1

unifysamedom result r

checkshape result

checknbshapes result -vertex 12 -edge 18 -wire 8 -face 8 -solid 1
checkprops result -s 223704 -v 3.27888e+006

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug27078
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27078
- **Data Files**: `bug27078.igs` (Needs `BRepTools::Read` or path resolution)
```tcl
##################################################################
# OCC27078: Exception in ShapeFixIntersectionTool::UnionVertexes()
##################################################################

pload IGES

testreadiges [locate_data_file bug27078.igs] a

# fixshape should not throw an exception
fixshape result a
```

### Test: bugs/heal/bug27199
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27199
- **Data Files**: `bug27199_i1_i2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC27199"
puts "========"
puts ""
##############################################
# Unifysamedomain regression issue in OCCT 7
##############################################

restore [locate_data_file bug27199_i1_i2.brep] sh

explode sh
bop sh_1 sh_2
bopfuse r
unifysamedom result r
checkshape result

checknbshapes result -face 1

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 14 END ---

--- BATCH 15 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug27246
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27246
- **Data Files**: `bug27246_tmp.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC27246 ALL: Faulty shapes in variables faulty_1 to"

puts "============"
puts "OCC27246"
puts "============"
puts ""
################################
## ShapeFix_Shape destroys shape
################################

restore [locate_data_file bug27246_tmp.brep] tmp
checkshape tmp

fixshape result tmp
checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug27271
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27271
- **Data Files**: `bug27271_Shape.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC27271"
puts "============"
puts ""
###########################################################################################################
# Unifysamedomain invalid result
###########################################################################################################

restore [locate_data_file bug27271_Shape.brep] a
unifysamedom result a

checkshape result

checknbshapes result -vertex 44 -edge 81 -wire 45 -face 35 -shell 3 -solid 3

set tolres [checkmaxtol result]

if { ${tolres} > 0.003} {
   puts "Error: bad tolerance of result"
}

explode result
checkprops result_1 -v 1109.52
checkprops result_2 -v 7.50891
checkprops result_3 -v 7.51022
```

### Test: bugs/heal/bug27331
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27331
- **Data Files**: `OCC27331.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======="
puts "OCC27331"
puts "======="
puts ""
#####################################################################
## FixMissingSeam gives wrong result
#####################################################################

restore [locate_data_file OCC27331.brep] shape
fixshape result shape 1.e-7 1

checkshape result
```

### Test: bugs/heal/bug27729
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27729
- **Data Files**: `bug27729_a_1275.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC27729"
puts "========"
puts ""
#######################################################################
# UnifySameDomain: allow the user to specify linear and angular tolerances
#######################################################################

restore [locate_data_file bug27729_a_1275.brep] a

unifysamedom r a -t 0.1 -a 5

regexp {FACE *: *(\d*)} [nbshapes r] full nbfaces
if {$nbfaces > 1000} {
  puts "Error: number of faces in the result is too large"
}

checkview -display r -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug27781
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27781
- **Data Files**: `bug27781_face_with_small_wires.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC27781"
puts "========"
puts ""
#############################################
# Exception in ShapeFix_Shape algorithm with option FixSmallAreaWireMode
#############################################

restore [locate_data_file bug27781_face_with_small_wires.brep] a

fixshape result a +s

checkshape result
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 15 END ---

--- BATCH 16 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug27894
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug27894
- **Data Files**: `bug27894_usd_raises_Standard_NullObject.stp` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC27894"
puts "============"
puts ""
###############################
## Crash when calling ShapeUpgrade_UnifySameDomain
###############################

pload STEP

stepread [locate_data_file bug27894_usd_raises_Standard_NullObject.stp] a *
renamevar a_1 a
unifysamedom result a

checknbshapes result -m UnifySameDomain -face 12 -edge 30

checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug28207
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug28207
- **Data Files**: `bug28207_face.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC28207"
puts "========"
puts ""
#########################################################
##  Unexpected result of the unify same domain algorithm
#########################################################

restore [locate_data_file bug28207_face.brep] s1

unifysamedom result1 s1
checkshape result1
checknbshapes result1 -face 1 -wire 1 -edge 984
checkprops result1 -l 10402.1

#safeInputMode is true for USD
unifysamedom result2 s1 -a 0.06
checkshape result2
checknbshapes result2 -face 1 -wire 1 -edge 983
checkprops result2 -l 10402.1

unifysamedom result3 s1 -a 57
checkshape result3
checknbshapes result3 -face 1 -wire 1 -edge 983
checkprops result3 -l 10402.1

unifysamedom result4 s1 -t 0.01 -a 5.7
checkshape result4
checknbshapes result4 -face 1 -wire 1 -edge 510
checkprops result4 -l 10402.1

unifysamedom result5 s1 -t 0.1 -a 5.7
checkshape result5
checknbshapes result5 -face 1 -wire 1 -edge 198
checkprops result5 -l 10402.1

unifysamedom result6 s1 -t 1.0 -a 5.7
checkshape result6
checknbshapes result6 -face 1 -wire 1 -edge 65
checkprops result6 -l 10401.9

checkview -display result1 -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug28343_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug28343_1
- **Data Files**: `bug28343_shape1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC28343"
puts "========"
puts ""
#################################################
# Modeling: ShapeUpgrade_UnifySameDomain algorithm produces invalid shape
#################################################

restore [locate_data_file bug28343_shape1.brep] s

unifysamedom result s -a 0.005
checkshape result
bopargcheck result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug28343_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug28343_2
- **Data Files**: `bug28343_shape2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC28343"
puts "========"
puts ""
#################################################
# Modeling: ShapeUpgrade_UnifySameDomain algorithm produces invalid shape
#################################################

restore [locate_data_file bug28343_shape2.brep] s

unifysamedom result s -a 0.005
checkshape result
bopargcheck result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug28471
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug28471
- **Data Files**: `bug28471_faces_share_vertex.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======="
puts "OCC28471"
puts "======="
puts ""
##################################################
# ShapeUpgrade_RemoveLocations breaks sharing of sub-shapes
##################################################

restore [locate_data_file bug28471_faces_share_vertex.brep] a

set remove_level 0 ;# allow compound
removeloc r a $remove_level

checknbshapes r -ref [nbshapes a]
```

--- BATCH 16 END ---

--- BATCH 17 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug28523
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug28523
- **Data Files**: `bug28377_problemface.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC28523 ALL: Error: Bad area of face"

puts "========"
puts "OCC28523"
puts "========"
puts ""
################################################
## Fixshape reverses the orientation of the face
################################################

restore [locate_data_file bug28377_problemface.brep] f2

fixshape newface f2

checkshape newface ff1

set prop [ sprops newface ]
regexp {Mass\s*:\s*([0-9.e+-]+)} $prop prop_s Area

if { ${Area} <= 0. } {
   puts "Error: Bad area of face"
}
```

### Test: bugs/heal/bug28529
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug28529
- **Data Files**: `bug28529_face_to_unify.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "=========="
puts "OCC28529"
puts "=========="
puts ""
######################################################################################################################
# UnifySameDomain crashes on a shape with location
######################################################################################################################

restore [locate_data_file bug28529_face_to_unify.brep] a

unifysamedom result a +b
checknbshapes result -m UnifySameDomain -edge 143 -vertex 143
```

### Test: bugs/heal/bug28553
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug28553
- **Data Files**: `bug28553_sh.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======="
puts "OCC28553"
puts "======="
puts ""
##################################################
# Incorrect result of the ShapeUpgrade_ShapeDivideContinuity algorithm
##################################################

restore [locate_data_file bug28553_sh.brep] f

#Split shape
DT_ShapeDivide r f

explode r F
mksurface s2 r_1
don r_1 s2

set fbnd [xbounds r_1]
set v2f [lindex $fbnd 3]
bounds s2 u1 u2 v1 v2

if {$v2f > [dval v2]} {
  puts "Error: result face bounds are out of surface ($v2f > [dval v2])"
}
```

### Test: bugs/heal/bug28595
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug28595
- **Data Files**: `bug28595_face.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC28595 ALL: Faulty shapes in variables faulty_1"
#puts "TODO OCC28595 ALL: Error: bad tolerance of fixshape"

puts "========"
puts "OCC28595"
puts "========"
puts ""
##########################################################################
## Shifting 2d curves for non-periodic surfaces leads to incorrect result.
##########################################################################

restore [locate_data_file bug28595_face.brep] f

fixshape res f

checkshape res

regexp {Tolerance MAX=([-0-9.+eE]+)} [tolerance f]   full f_Tolerance
regexp {Tolerance MAX=([-0-9.+eE]+)} [tolerance res] full res_Tolerance

if { ${res_Tolerance} > ${f_Tolerance} } {
   puts "Error: bad tolerance of fixshape"
}
```

### Test: bugs/heal/bug28995
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug28995
- **Data Files**: `bug28995_s1.brep, bug28995_s2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======="
puts "0028995"
puts "======="
puts ""
##################################################
# UnifySameDomain produces invalid shape
##################################################

brestore [locate_data_file bug28995_s1.brep] s1
brestore [locate_data_file bug28995_s2.brep] s2

# perform fuse operation
bfuse rfuse s1 s2

# check the result of fuse
checkshape rfuse 

if {![regexp "This shape seems to be OK" [bopcheck rfuse]]} {
  puts "Error: The result of FUSE operation is a self-intersecting shape."
}

# unify faces and edges in the fused shape
unifysamedom result rfuse

# check unified result
checkshape result

if {![regexp "This shape seems to be OK" [bopcheck result]]} {
  puts "Error: The result of UnifySameDomain algorithm is a self-intersecting shape."
}

checknbshapes result -vertex 200 -edge 300 -wire 104 -face 103 -solid 1
checkprops result -s 1.59154e+006 -v 1.1497e+007

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 17 END ---

--- BATCH 18 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug29382_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug29382_1
- **Data Files**: `bug29382_Group_3.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================================================================================"
puts "OCC29382: ShapeUpgrade_UnifySameDomain algorithm incorrectly processes the shape"
puts "================================================================================"
puts ""

restore [locate_data_file bug29382_Group_3.brep] a

unifysamedom result a

checkshape result
checkshape a

checknbshapes result -solid 3 -shell 3 -face 18 -wire 18 -edge 36 -vertex 22

set tolres [checkmaxtol result]

if { ${tolres} > 1.8066863810061599e-05} {
   puts "Error: bad tolerance of result"
}

explode result
checkprops result_1 -v 4.41996e-06
checkprops result_2 -v 1.30453e-06
checkprops result_3 -v 1.16532e-06
```

### Test: bugs/heal/bug29382_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug29382_2
- **Data Files**: `bug29382_Group_3.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================================================================================"
puts "OCC29382: ShapeUpgrade_UnifySameDomain algorithm incorrectly processes the shape"
puts "================================================================================"
puts ""

restore [locate_data_file bug29382_Group_3.brep] a

unifysamedom result a +b

checkshape result
checkshape a

checknbshapes result -solid 3 -shell 3 -face 16 -wire 16 -edge 30 -vertex 18

set tolres [checkmaxtol result]

if { ${tolres} > 1.8066863810061599e-05} {
   puts "Error: bad tolerance of result"
}

explode result
checkprops result_1 -v 4.41996e-06
checkprops result_2 -v 1.30453e-06
checkprops result_3 -v 1.16532e-06
```

### Test: bugs/heal/bug29382_3
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug29382_3
```tcl
puts "================================================================================"
puts "OCC29382: ShapeUpgrade_UnifySameDomain algorithm incorrectly processes the shape"
puts "================================================================================"
puts ""

beziercurve a 5  0 0 0  1 0 0  2 2 0  0 0.5 0  0 0 0
mkedge a a
wire a a
mkplane a a
prism a a 0 0 1
box b -0.3 -0.2 0  1 0.4 1
bcommon shape a b

unifysamedom result shape +b

checkshape result

checknbshapes result -solid 1 -shell 1 -face 5 -wire 5 -edge 9 -vertex 5

set tolres [checkmaxtol result]

if { ${tolres} > 6.e-6} {
   puts "Error: bad tolerance of result"
}
```

### Test: bugs/heal/bug29502
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug29502
```tcl
puts "========"
puts "OCC29502"
puts "========"
puts ""
#################################################
# Improve performance of the ShapeUpgrade_UnifySameDomain::UnifyEdges() method
#################################################

# create cylinder
cylinder c 0 0 0 0 0 1 10
mkface f c 0 2*pi -10 10

# split seam edge by the vertex
vertex v 10 0 0
bclearobjects
bcleartools
baddobjects f
baddtools v
bfillds
bsplit r

checknbshapes r -vertex 3 -edge 4 -wire 1 -face 1

# perform unification of the seam edge:
# the split vertex should be removed
unifysamedom result r

checkshape result
checkprops result -equal f
checknbshapes result -ref [nbshapes f]
```

### Test: bugs/heal/bug29504_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug29504_1
- **Data Files**: `bug29504_cyl.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "===================================================================================================="
puts "OCC29504: ShapeUpgrade_UnifySameDomain produces invalid shape and ShapeFix_Shape is unable to fix it"
puts "===================================================================================================="
puts ""

restore [locate_data_file bug29504_cyl.brep] s
unifysamedom result s

checkshape result
checkshape s

checknbshapes result -face 1 -wire 1 -edge 4 -vertex 4

set tolres [checkmaxtol result]

if { ${tolres} > 1.000008e-07} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 104.72
```

--- BATCH 18 END ---

--- BATCH 19 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug29504_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug29504_2
- **Data Files**: `bug29504_sph.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "===================================================================================================="
puts "OCC29504: ShapeUpgrade_UnifySameDomain produces invalid shape and ShapeFix_Shape is unable to fix it"
puts "===================================================================================================="
puts ""

restore [locate_data_file bug29504_sph.brep] s
unifysamedom result s

checkshape result
checkshape s

checknbshapes result -face 1 -wire 1 -edge 4 -vertex 4

set tolres [checkmaxtol result]

if { ${tolres} > 1.000008e-07} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 55.1286
```

### Test: bugs/heal/bug29544_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug29544_1
- **Data Files**: `bug29544_edges.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC29544"
puts "========"
puts ""
#################################################
# Regression vs 7.2.0: ShapeUpgrade_UnifySameDomain fails to merge linear edges
#################################################

restore [locate_data_file bug29544_edges.brep] ce
unifysamedom result ce

checkshape result
checkprops result -equal ce
checknbshapes result -vertex 4 -edge 2
```

### Test: bugs/heal/bug29544_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug29544_2
```tcl
puts "========"
puts "OCC29544"
puts "========"
puts ""
#################################################
# Regression vs 7.2.0: ShapeUpgrade_UnifySameDomain fails to merge linear edges
#################################################

# create edge
line l 0 0 0 1 0 0
mkedge e l 0 10

# split edge by the vertex
vertex v 5 2.e-7 0
bclearobjects
bcleartools
baddobjects e
baddtools v
bfillds
bsplit sp

# unify the splits of the edge
unifysamedom result sp

checkshape result
checkprops result -l 10
checknbshapes result -vertex 2 -edge 1
```

### Test: bugs/heal/bug29695
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug29695
- **Data Files**: `bug29695_badface.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
restore [locate_data_file bug29695_badface.brep] a
fixshape result a

checkshape result
```

### Test: bugs/heal/bug30099
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30099
- **Data Files**: `bug30099.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "===================================================================================="
puts "OCC30099: Modeling Algorithms - ShapeUpgrade_UnifySameDomain produces invalid result"
puts "===================================================================================="
puts ""

restore [locate_data_file bug30099.brep] a

unifysamedom result a

checkshape result

checknbshapes result -solid 1 -shell 1 -face 12 -wire 14 -edge 36 -vertex 26

set tolres [checkmaxtol result]

if { ${tolres} > 1.2e-05} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 2.04221e+06
```

--- BATCH 19 END ---

--- BATCH 20 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug30100_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30100_1
- **Data Files**: `bug30100_usd.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "0030100: Modeling Algorithms - ShapeUpgrade_UnifySameDomain is unable to unify faces based on the same toroidal surface"
puts "========"
puts ""

restore [locate_data_file bug30100_usd.brep] s
unifysamedom result s
checkshape result
checknbshapes result -wire 1 -face 1
checkprops result -s 142506

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug30158_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30158_1
- **Data Files**: `bug30158_1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "==================================================================================="
puts "OCC30158: Shape Healing - Incorrect result produced by ShapeUpgrade_UnifySameDomain"
puts "==================================================================================="
puts ""

restore [locate_data_file bug30158_1.brep] s
unifysamedom result s

checkshape result
checkshape s

checknbshapes result -solid 1 -shell 1 -face 6 -wire 6 -edge 12 -vertex 8

set tolres [checkmaxtol result]

if { ${tolres} > 1.51e-07} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 48.3068
```

### Test: bugs/heal/bug30158_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30158_2
- **Data Files**: `bug30158_2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "==================================================================================="
puts "OCC30158: Shape Healing - Incorrect result produced by ShapeUpgrade_UnifySameDomain"
puts "==================================================================================="
puts ""

restore [locate_data_file bug30158_2.brep] s
unifysamedom result s

checkshape result
checkshape s

checknbshapes result -solid 1 -shell 1 -face 6 -wire 6 -edge 12 -vertex 8

set tolres [checkmaxtol result]

if { ${tolres} > 1.51e-07} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 48.3068
```

### Test: bugs/heal/bug30174
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30174
- **Data Files**: `bug30174_cylinder.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC30174"
puts "============"
puts ""
###########################################################################################################
# ShapeUpgrade_UnifySameDomain does not unify cylindrical faces
###########################################################################################################

restore [locate_data_file bug30174_cylinder.brep] a
unifysamedom result a

checkshape result

checknbshapes result -vertex 2 -edge 3 -wire 3 -face 3 -shell 1 -solid 1

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-5} {
   puts "Error: bad tolerance of result"
}
```

### Test: bugs/heal/bug30185
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30185
- **Data Files**: `bug30185.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "0030185: Unify same domain algorithm corrupts the initial shape"
puts "============"
puts ""

restore [locate_data_file bug30185.brep] s

unifysamedom r s +b
checkshape r
checknbshapes r -vertex 50 -edge 71 -face 21
checkmaxtol r -min_tol 2.49130e-03
checkmaxtol s -min_tol 2.49130e-03

checkview -display r -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 20 END ---

--- BATCH 21 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug30534
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30534
- **Data Files**: `bug30534_Werth_369.bin` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================================================================="
puts "OCC30534: Regression in the tool UnifySameDomain - the shape looses faces"
puts "========================================================================="
puts ""

binrestore [locate_data_file bug30534_Werth_369.bin] a

unifysamedom result a

checkshape result

checknbshapes result -solid 1 -shell 1 -face 423 -wire 431 -edge 1162 -vertex 736

set tolres [checkmaxtol result]

if { ${tolres} > 0.00969438844497875} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 18814.1
```

### Test: bugs/heal/bug30714
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30714
- **Data Files**: `bug30714.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================================================="
puts "OCC30714: UnifySameDomain corrupts original shape"
puts "================================================="
puts ""

restore [locate_data_file bug30714.brep] s
explode s
bcut r s_1 s_2
unifysamedom result r

checkshape result
checkshape s_1

checknbshapes result -solid 1 -shell 1 -face 10 -wire 12 -edge 24 -vertex 16

set tolres [checkmaxtol result]

if { ${tolres} > 1.000008e-07} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 1.20292
```

### Test: bugs/heal/bug30831
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30831
- **Data Files**: `bug30831.bin` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"                                                                                      
puts "0030831: Shape Healing - ShapeFix algorithm (creation of seam edge) takes too long time with thin faces"
puts "========================"                                                                                      

cpulimit 10
binrestore [locate_data_file bug30831.bin] a

fixshape r a 1.e-7 1.
checkshape r a
tolerance r

checkview -display r -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug30897
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30897
- **Data Files**: `bug30897_bleriot_16.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======================================================="
puts "0030897: Modeling Algorithms - Crash in UnifySameDomain"
puts "======================================================="
puts ""

restore [locate_data_file bug30897_bleriot_16.brep] a

unifysamedom result a

checkshape result

checknbshapes result -solid 1 -shell 1 -face 51 -wire 121 -edge 379 -vertex 324

set tolres [checkmaxtol result]

if { ${tolres} > 0.00385} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 403016
```

### Test: bugs/heal/bug30905
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30905
- **Data Files**: `bug30905.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======================================================="
puts "0030905: Modeling Algorithms - Invalid shapes in UnifySameDomain"
puts "======================================================="
puts ""

restore [locate_data_file bug30905.brep] a

unifysamedom result a

checkshape result

checknbshapes result -solid 1 -shell 1 -face 56 -wire 61 -edge 148 -vertex 93 -t

checkprops result -s 835.815 -v 339.409

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 21 END ---

--- BATCH 22 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug30927
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug30927
```tcl
puts "============================================================================"
puts "0030927: Modeling Algorithms - UnifySameDom looses the Closed flag for wires"
puts "============================================================================"
puts ""

box b1 10 10 10
box b2 5 0 -5 10 10 20
bfuse s b1 b2
unifysamedom result s

foreach w [explode result w] {
  if {![regexp "Closed" [whatis $w]]} {
    puts "Error: Wire $w is not closed"
  }
}
```

### Test: bugs/heal/bug31066
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31066
- **Data Files**: `bug31066.stp` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======================================================="
puts "OCC31066: Infinite loop in ShapeUpgrade_UnifySameDomain"
puts "======================================================="
puts ""

pload MODELING DATAEXCHANGE

stepread [locate_data_file bug31066.stp] a *

unifysamedom result a_1

checkshape result

checknbshapes result -solid 1 -shell 1 -face 140 -wire 174 -edge 352 -vertex 208

set tolres [checkmaxtol result]

if { ${tolres} > 0.001} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 139105
```

### Test: bugs/heal/bug31187
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31187
- **Data Files**: `bug31187_fuse.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============================================================="
puts "OCC31187: Unify same domain algorithm produces invalid shape."
puts "============================================================="
puts ""

restore [locate_data_file bug31187_fuse.brep] a

unifysamedom result a -t 1.e-4 -a 1.e-6

checkshape result

checknbshapes result -solid 1 -shell 1 -face 13 -wire 13 -edge 36 -vertex 25

set tolres [checkmaxtol result]

if { ${tolres} > 6.e-5} {
   puts "Error: bad tolerance of result"
}
```

### Test: bugs/heal/bug31202
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31202
- **Data Files**: `bug31202.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
restore [locate_data_file bug31202.brep] shape
fixshape result shape 0.000001137 0.0001137
checkshape result
checknbshapes result -edge 4
```

### Test: bugs/heal/bug31441
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31441
- **Data Files**: `bug31441.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============================================"
puts "OCC31441: UnifySameDomain corrupts the shape"
puts "============================================"
puts ""

brestore [locate_data_file bug31441.brep] a

unifysamedom result a

checkshape result

checknbshapes result -solid 1 -shell 2 -face 46 -wire 48 -edge 128 -vertex 84

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 1.10788
```

--- BATCH 22 END ---

--- BATCH 23 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug31524
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31524
- **Data Files**: `bug31524.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "TODO OCC31524 ALL: Faulty shapes in variables faulty_1"

puts "==============================================================================================="
puts "0031524: Modeling Algorithms - Unify same domain corrupts shape representing a cylindrical tube"
puts "==============================================================================================="
puts ""

restore [locate_data_file bug31524.brep] shape

unifysamedom res shape
checkshape shape
```

### Test: bugs/heal/bug31736_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31736_1
- **Data Files**: `bug31736_1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "=================================================================================="
puts "OCC31736: ShapeUpgrade_UnifySameDomain algorithm does not unify all possible faces"
puts "=================================================================================="
puts ""

restore [locate_data_file bug31736_1.brep] a
unifysamedom result a

checkshape result

checknbshapes result -vertex 18 -edge 26 -wire 9 -face 9 -shell 1 -solid 1

set tolres [checkmaxtol result]

if { ${tolres} > 0.5} {
   puts "Error: bad tolerance of result"
}
```

### Test: bugs/heal/bug31736_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31736_2
- **Data Files**: `bug31736_2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "=================================================================================="
puts "OCC31736: ShapeUpgrade_UnifySameDomain algorithm does not unify all possible faces"
puts "=================================================================================="
puts ""

restore [locate_data_file bug31736_2.brep] a
unifysamedom result a

checkshape result

checknbshapes result -vertex 17 -edge 24 -wire 9 -face 9 -shell 1 -solid 1

set tolres [checkmaxtol result]

if { ${tolres} > 0.5} {
   puts "Error: bad tolerance of result"
}
```

### Test: bugs/heal/bug31778
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31778
- **Data Files**: `bug31778.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============================================="
puts "OCC31778: UnifySameDomain fails in Debug mode"
puts "============================================="
puts ""

brestore [locate_data_file bug31778.brep] s
explode s
bclearobjects
bcleartools
baddobjects s_1
baddtools s_2 s_3
bfillds
bbop q 1
explode q

unifysamedom result q_1

checkshape result

checknbshapes result -solid 1 -shell 1 -face 19 -wire 21 -edge 51 -vertex 34

set tolres [checkmaxtol result]

if { ${tolres} > 5.e-5} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 15173.9
```

### Test: bugs/heal/bug31855_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31855_1
- **Data Files**: `bug31855_1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================================================="
puts "OCC31855: Regression of the unifysamedom command."
puts "================================================="
puts ""

restore [locate_data_file bug31855_1.brep] a
unifysamedom result a

checkshape result

checknbshapes result -vertex 89 -edge 134 -wire 47 -face 47 -shell 1 -solid 1

set tolres [checkmaxtol result]

if { ${tolres} > 0.0006} {
   puts "Error: bad tolerance of result"
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 23 END ---

--- BATCH 24 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug31855_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31855_2
- **Data Files**: `bug31855_2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================================================="
puts "OCC31855: Regression of the unifysamedom command."
puts "================================================="
puts ""

restore [locate_data_file bug31855_2.brep] a
unifysamedom result a

checkshape result

checknbshapes result -vertex 664 -edge 996 -wire 484 -face 406 -shell 1 -solid 1

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug31855_3
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug31855_3
- **Data Files**: `bug31855_3.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================================================="
puts "OCC31855: Regression of the unifysamedom command."
puts "================================================="
puts ""

restore [locate_data_file bug31855_3.brep] a
unifysamedom result a

checkshape result

checknbshapes result -vertex 272 -edge 408 -wire 150 -face 143 -shell 1 -solid 1

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug32140
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32140
- **Data Files**: `bug32140.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "=============================================================="
puts "OCC32140: unify same domain calls crossed for opposite vectors"
puts "=============================================================="
puts ""

restore [locate_data_file bug32140.brep] a

unifysamedom result a

checkshape result

checknbshapes result -solid 1 -shell 1 -face 26 -wire 32 -edge 69 -vertex 41

set tolres [checkmaxtol result]

if { ${tolres} > 6.e-6} {
   puts "Error: bad tolerance of result"
}
```

### Test: bugs/heal/bug32213
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32213
- **Data Files**: `bug32213.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "==========================================="
puts "OCC32213: Invalid result of UnifySameDomain"
puts "==========================================="
puts ""

restore [locate_data_file bug32213.brep] a

unifysamedom result a

checkshape result

checknbshapes result -solid 1 -shell 1 -face 21 -wire 22 -edge 57 -vertex 37

set tolres [checkmaxtol result]

if { ${tolres} > 6.e-6} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 81.9221
```

### Test: bugs/heal/bug32332_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32332_1
- **Data Files**: `bug32332_1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "=========================================================="
puts "OCC32332: Incorrect result of ShapeUpgrade_UnifySameDomain"
puts "=========================================================="
puts ""

restore [locate_data_file bug32332_1.brep] a

unifysamedom result a

checkshape result

checknbshapes result -wire 4 -edge 9 -vertex 6

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}
```

--- BATCH 24 END ---

--- BATCH 25 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug32332_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32332_2
- **Data Files**: `bug32332_2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "=========================================================="
puts "OCC32332: Incorrect result of ShapeUpgrade_UnifySameDomain"
puts "=========================================================="
puts ""

restore [locate_data_file bug32332_2.brep] a

unifysamedom result a

checkshape result

checknbshapes result -wire 4 -edge 12 -vertex 8

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}
```

### Test: bugs/heal/bug32561
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32561
- **Data Files**: `bug32561.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======================================================================================="
puts "OCC32561: UnifySameDomain does nothing while it is expected to union two pairs of faces"
puts "======================================================================================="
puts ""

restore [locate_data_file bug32561.brep] a

unifysamedom result a

checkshape result

checknbshapes result -t -solid 64 -shell 64 -face 233 -wire 233 -edge 284 -vertex 116

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}
```

### Test: bugs/heal/bug32581
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32581
- **Data Files**: `bug32581.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "================================================="
puts "OCC32581: UnifySameDomain produces invalid result"
puts "================================================="
puts ""

restore [locate_data_file bug32581.brep] a

unifysamedom result a

checkshape result

checknbshapes result -t -solid 3 -shell 3 -face 14 -wire 16 -edge 31 -vertex 20

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 63178.1 -v 1e+06
```

### Test: bugs/heal/bug32619
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32619
- **Data Files**: `bug32619.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============================================="
puts "OCC32619: UnifySameDomain ignores shared face"
puts "============================================="
puts ""

restore [locate_data_file bug32619.brep] a

unifysamedom result a

checkshape result

checknbshapes result -t -shell 2 -face 3 -wire 3 -edge 12 -vertex 10

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug32623
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32623
- **Data Files**: `bug32623.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============================================================="
puts "OCC32623: UnifySameDomain invalid result only in release mode"
puts "============================================================="
puts ""

restore [locate_data_file bug32623.brep] a

unifysamedom result a

checkshape result

checknbshapes result -t -solid 1 -shell 1 -face 58 -wire 68 -edge 163 -vertex 107

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-5} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 56934.4 -v 244645
```

--- BATCH 25 END ---

--- BATCH 26 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug32719
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32719
- **Data Files**: `bug32719.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============================================================"
puts "OCC32719: UnifySameDomain result has incorrect triangulation"
puts "============================================================"
puts ""

restore [locate_data_file bug32719.brep] a

unifysamedom result a

checkshape result

checknbshapes result -t -solid 4 -shell 4 -face 20 -wire 20 -edge 32 -vertex 16

set tolres [checkmaxtol result]

if { ${tolres} > 6.e-6} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 0.0222593 -v 5.17261e-05
```

### Test: bugs/heal/bug32814_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32814_1
- **Data Files**: `bug32814_1.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "=============================================="
puts "OCC32814: Unifysamedom produces invalid result"
puts "=============================================="
puts ""

restore [locate_data_file bug32814_1.brep] s

unifysamedom result s -a 1e-4

checkshape result
bopargcheck result

checknbshapes result -t -face 7 -wire 13 -edge 70 -vertex 64

set tolres [checkmaxtol result]

if { ${tolres} > 1.e-7} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 5.54082e+06

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug32814_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug32814_2
- **Data Files**: `bug32814_2.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "=============================================="
puts "OCC32814: Unifysamedom produces invalid result"
puts "=============================================="
puts ""

restore [locate_data_file bug32814_2.brep] s

unifysamedom result s

checkshape result
bopargcheck result

checknbshapes result -t -solid 1 -shell 1 -face 78 -wire 110 -edge 220 -vertex 136

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 1.61456e+06 -v 3e+07

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug329
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug329
- **Data Files**: `OCC329.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"
puts " OCC329 "
puts "========================"
puts ""
###########################################################
##fixshape command does not fix incorrect tolerance order. 
###########################################################

restore [locate_data_file OCC329.brep] a 

checkshape a
set tol1 [maxtolerance a]
regexp { +Face +: +Min +[-0-9.+eE]+ +Max +([-0-9.+eE]+)} $tol1 full face1
regexp { +Edge +: +Min +[-0-9.+eE]+ +Max +([-0-9.+eE]+)} $tol1 full edge1
regexp { +Vertex +: +Min +[-0-9.+eE]+ +Max +([-0-9.+eE]+)} $tol1 full vert1

fixshape result a 1.e-7 0.1

checkshape result
set tol2 [ maxtolerance result]
regexp { +Face +: +Min +[-0-9.+eE]+ +Max +([-0-9.+eE]+)} $tol2 full face2
regexp { +Edge +: +Min +[-0-9.+eE]+ +Max +([-0-9.+eE]+)} $tol2 full edge2
regexp { +Vertex +: +Min +[-0-9.+eE]+ +Max +([-0-9.+eE]+)} $tol2 full vert2

if { ${face2} > ${face1} || ${edge2} > ${edge1} || ${vert2} > ${vert1} } {
    puts "Error : Tolerance invalid. Function FixShape works WRONGLY"
} else {
    puts "Tolerance valed. Function FixShape works CORRECTLY"
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug33006
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug33006
- **Data Files**: `bug33006.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================================="
puts "OCC33006: UnifySameDomain raises exceptio"
puts "========================================="
puts ""

restore [locate_data_file bug33006.brep] a

unifysamedom result a

checkshape result

checknbshapes result -t -solid 1 -shell 1 -face 3 -wire 4 -edge 11 -vertex 4

set tolres [checkmaxtol result]

if { ${tolres} > 0.004416} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 77917.5 -v 195647

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 26 END ---

--- BATCH 27 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug33028
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug33028
- **Data Files**: `bug33028_kalip.stp` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"
puts " OCC33028: Standard_ConstructionError while using ShapeUpgrade_UnifySameDomain"
puts "========================"
puts ""

pload STEP

stepread [locate_data_file bug33028_kalip.stp] s *

set nbsBefore "
Number of shapes in shape
 VERTEX    : 543
 EDGE      : 819
 WIRE      : 359
 FACE      : 289
 SHELL     : 2
 SOLID     : 2
 COMPSOLID : 0
 COMPOUND  : 1
 SHAPE     : 2015
"
checknbshapes s_1 -ref ${nbsBefore} -t -m "result before attempt to simplify the model"

unifysamedom result s_1

set nbsAfter "
Number of shapes in shape
 VERTEX    : 515
 EDGE      : 805
 WIRE      : 359
 FACE      : 289
 SHELL     : 2
 SOLID     : 2
 COMPSOLID : 0
 COMPOUND  : 1
 SHAPE     : 1973
"
checknbshapes result -ref ${nbsAfter} -t -m "result after attempt to simplify the model"
```

### Test: bugs/heal/bug33171_1
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug33171_1
```tcl
puts "========================"
puts " 	0033171: Modeling Algorithms - Invalid result of faces unification"
puts "========================"
puts ""

# make outer prism
polyline p 0 0 0  10 0 0  10 10 0  0 10 0  0 0 0
mkplane f p
prism s f 0 0 5

# make section shells
polyline p1 3 10 0  3 7 0  6 7 0  6 3 0  10 3 0
polyline p2 6 7 0  10 7 0
polyline p3 8 7 0  8 10 0
polyline p4 0 5 0  10 5 0

prism sh1 p1 0 0 5
prism sh2 p2 0 0 5
prism sh3 p3 0 0 5
prism sh4 p4 0 0 5

# split the prism
bclearobjects
bcleartools
baddobjects s
baddtools sh1 sh2 sh3 sh4
bfillds
bsplit r

checkshape r
if {![regexp "This shape seems to be OK" [bopcheck r]]} {
    puts "Error: invalid shape after split"
}

# try to unify faces in the result compound
unifysamedom ru1 r
unifysamedom ru2 r +i

checkshape ru1
checkshape ru2

checknbshapes ru1 -ref [nbshapes r -t] -t
checknbshapes ru2 -ref [nbshapes r -t] -t

if {![regexp "This shape seems to be OK" [bopcheck ru1]]} {
    puts "Error: invalid shape after faces unification"
}
if {![regexp "This shape seems to be OK" [bopcheck ru2]]} {
    puts "Error: invalid shape after faces unification"
}

# make compound of shells
eval compound [explode r] shs

unifysamedom shsu1 r
unifysamedom shsu2 r +i

checkshape shsu1
checkshape shsu2

checknbshapes shsu1 -ref [nbshapes shs -t] -t
checknbshapes shsu2 -ref [nbshapes shs -t] -t

if {![regexp "This shape seems to be OK" [bopcheck shsu1]]} {
    puts "Error: invalid shape after faces unification"
}
if {![regexp "This shape seems to be OK" [bopcheck shsu2]]} {
    puts "Error: invalid shape after faces unification"
}

checkview -display ru2 -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug33171_2
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug33171_2
```tcl
puts "========================"
puts " 	0033171: Modeling Algorithms - Invalid result of faces unification"
puts "========================"
puts ""

# make two solids
box b1 10 10 5
box b2 10 0 0 5 5 5
# make shared
bclearobjects
bcleartools
baddobjects b1 b2
bfillds
bbuild r

checkshape r
if {![regexp "This shape seems to be OK" [bopcheck r]]} {
    puts "Error: invalid shape after fuse"
}

# try to unify faces in the result compound
unifysamedom ru1 r
unifysamedom ru2 r +i

checkshape ru1
checkshape ru2

checknbshapes ru1 -ref [nbshapes r -t] -t
checknbshapes ru2 -ref [nbshapes r -t] -t

if {![regexp "This shape seems to be OK" [bopcheck ru1]]} {
    puts "Error: invalid shape after faces unification"
}
if {![regexp "This shape seems to be OK" [bopcheck ru2]]} {
    puts "Error: invalid shape after faces unification"
}

# make compound of shells
eval compound [explode r] shs

unifysamedom shsu1 r
unifysamedom shsu2 r +i

checkshape shsu1
checkshape shsu2

checknbshapes shsu1 -ref [nbshapes shs -t] -t
checknbshapes shsu2 -ref [nbshapes shs -t] -t

if {![regexp "This shape seems to be OK" [bopcheck shsu1]]} {
    puts "Error: invalid shape after faces unification"
}
if {![regexp "This shape seems to be OK" [bopcheck shsu2]]} {
    puts "Error: invalid shape after faces unification"
}

checkview -display ru2 -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug33193
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug33193
- **Data Files**: `bug33193.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================================="
puts "0033193: Modeling Algorithms - Regression: UnifySameDomain raises SIGSEGV"
puts "========================================="
puts ""

restore [locate_data_file bug33193.brep] a

unifysamedom result a

checkshape result

checknbshapes result -t -solid 1 -shell 1 -face 152 -wire 202 -edge 411 -vertex 273

set tolres [checkmaxtol result]

if { ${tolres} > 1.26e-7} {
   puts "Error: bad tolerance of result"
}
```

### Test: bugs/heal/bug33398
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug33398
- **Data Files**: `bug33398.step` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================================="
puts "0033398: Modeling Algorithms - ShapeUpgrade_UnifySameDomain fails on specific STEP model"
puts "========================================="
puts ""

pload STEP

testreadstep [locate_data_file bug33398.step] s 
unifysamedom result s 

checknbshapes result -vertex 506 -edge 908 -wire 394 -face 382 -shell 4 -solid 4    
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

--- BATCH 27 END ---

--- BATCH 28 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/heal/bug33421
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug33421
```tcl
puts "TODO CR33439 ALL: Error :  is WRONG because number of EDGE entities in shape"
puts "TODO CR33439 ALL: Error :  is WRONG because number of WIRE entities in shape"
puts "TODO CR33439 ALL: Error :  is WRONG because number of FACE entities in shape"

puts "========================================="
puts "0033421: Modeling Algorithms - ShapeUpgrade_UnifySameDomain fails"
puts "========================================="
puts ""

pcylinder c1 10 10
copy c1 c2
tmirror c2 0 0 10 0 0 1
bop c1 c2
bopfuse c3

unifysamedom result c3

checknbshapes result -t -solid 1 -shell 1 -face 3 -wire 3 -edge 3 -vertex 3
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug482
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug482
- **Data Files**: `OCC482.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========================"
puts " OCC482 "
puts "========================"
puts ""
#################################################
## Regression in ShapeFix_Wireframe (caused by fix on OCC394)
#################################################

restore [locate_data_file OCC482.brep] a 

fixsmall result a 0.05

checkshape result
checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug518
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug518
- **Data Files**: `OCC518.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "========"
puts "OCC518"
puts "========"
puts ""
################################
## Checkshape reports problem in C40, none in DEV
################################

pload XDE

restore [locate_data_file OCC518.brep] a

fixshape result a .001 1.

checkshape result

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/heal/bug7570
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug7570
- **Data Files**: `bug7570_small.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "======="
puts "OCC7570"
puts "======="
puts ""
##################################################
# Exception in ShapeFix_Wireframe::FixSmallEdges
##################################################

pload QAcommands

restore [locate_data_file bug7570_small.brep] a

OCC7570 a
```

### Test: bugs/heal/bug884
- **Target Suite**: OCCT_Bugs_heal
- **Target Case**: bug884
- **Data Files**: `OCC884.brep` (Needs `BRepTools::Read` or path resolution)
```tcl
puts "============"
puts "OCC884"
puts "============"
puts ""
###################################################
## ShapeFix_Wire::FixSelfIntersection does not fix 3d curves on planes
###################################################

pload QAcommands

restore [locate_data_file OCC884.brep] a

OCC884 res a 1.1 1.1

explode res e
mkplane p res
pcurve c2d res_1 p
mkcurve c3d res_1

set list_3 [length $c3d]
set list_4 [length $c2d]

# len2 is the length of 3d curve
regexp {The length c3d is +([-0-9.+eE]+)} $list_3 full len2

# len1 is the length of 2d curve
regexp {The length c2d is +([-0-9.+eE]+)} $list_4 full len1

set percent_max 0.1
set percent [expr abs(${len2} - ${len1}) / double(${len1}) * 100.]

puts "len1 = ${len1}"
puts "len2 = ${len2}"
puts "percent = ${percent}"

if {${percent} > ${percent_max}} {
  puts "OCC884: Error"
} else {
  puts "OCC884: OK"
}
```

--- BATCH 28 END ---

