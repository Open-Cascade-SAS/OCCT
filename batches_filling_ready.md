# OCCT Test Migration Batches

Below are batches of Tcl scripts. Paste one batch at a time to the AI.

--- BATCH 1 START ---

I am converting these tests to direct C++ API Gtest cases.

### Test: bugs/filling/bug13904
- **Target Suite**: OCCT_Bugs_filling
- **Target Case**: bug13904
- **Suggested C++**: BRepOffsetAPI_MakeFilling
```tcl
puts "==========="
puts "OCC13904"
puts "==========="
######################################################
# Exception during "filling" operation
######################################################

set BugNumber OCC13904

circle c 0 100 100 0 1 1 50
ellipse el 0 0 0 0 1 1 100 50

set  mistake 0
if [catch { appsurf result c el } res] {
  set  mistake 1
  puts "Faulty ${BugNumber}: filling problem"
} else {
  puts "Filling ${BugNumber}  OK"
  set What [whatis result]
  if { [regexp "surface" $What] != 1 } {
    set  mistake 1
  } else {
    set  mistake 0
  }
}

# Resume
puts ""
if { ${mistake} != 0 } {
   puts "Faulty ${BugNumber}"
} else {
   puts "OK ${BugNumber}"
}
```

### Test: bugs/filling/bug16119
- **Target Suite**: OCCT_Bugs_filling
- **Target Case**: bug16119
- **Data Files**: `OCC16119-c1.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16119-c1.draw`), `OCC16119-c2.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16119-c2.draw`), `OCC16119-c31.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16119-c31.draw`), `OCC16119-c32.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16119-c32.draw`), `OCC16119-c41.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16119-c41.draw`), `OCC16119-c42.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16119-c42.draw`), `OCC16119-cv.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16119-cv.draw`)
- **Suggested C++**: GeomFill_BSplineCurves / GeomFill_ConstrainedFilling
```tcl
puts "============"
puts "OCC16119"
puts "============"
puts ""
#######################################################################
# Bug in GeomFill_Coons algorithm
#######################################################################

set BugNumber OCC16119

restore [locate_data_file OCC16119-c1.draw] c1
restore [locate_data_file OCC16119-c2.draw] c2
restore [locate_data_file OCC16119-c31.draw] c31
restore [locate_data_file OCC16119-c32.draw] c32
restore [locate_data_file OCC16119-c41.draw] c41
restore [locate_data_file OCC16119-c42.draw] c42
restore [locate_data_file OCC16119-cv.draw] cv

fillcurves s11 c1 c41 cv c31
fillcurves s12 cv c42 c2 c32

svalue s11 .5 .5 x1 y1 z1 dux1 duy1 duz1 dvx1 dvy1 dvz1
svalue s12 .5 .5 x2 y2 z2 dux2 duy2 duz2 dvx2 dvy2 dvz2

set deltaX [dval dvx1-dvx2]
set deltaY [dval dvy1-dvy2]
set deltaZ [dval dvz1-dvz2]

checkreal "deltaX" ${deltaX} -3.5527136788005009e-015 0 0.001
checkreal "deltaY" ${deltaY} -3.5527136788005009e-015 0 0.001
checkreal "deltaZ" ${deltaZ} 8.8817841970012523e-016  0 0.001
```

### Test: bugs/filling/bug16833
- **Target Suite**: OCCT_Bugs_filling
- **Target Case**: bug16833
- **Data Files**: `OCC16833-c1.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16833-c1.draw`), `OCC16833-c2.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16833-c2.draw`), `OCC16833-c3.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16833-c3.draw`), `OCC16833-c4.draw` (Path: `E:\code\occtSrc\OCCT\data\geom\OCC16833-c4.draw`)
- **Suggested C++**: BRepOffsetAPI_MakeFilling, GeomFill_BSplineCurves / GeomFill_ConstrainedFilling
```tcl
puts "============"
puts "OCC16833"
puts "============"
puts ""
#######################################################################
# Error in Coons algorithm
#######################################################################

set BugNumber OCC16833

restore [locate_data_file OCC16833-c1.draw] c1
restore [locate_data_file OCC16833-c2.draw] c2
restore [locate_data_file OCC16833-c3.draw] c3
restore [locate_data_file OCC16833-c4.draw] c4

set  mistake 0
if [catch { fillcurves result c1 c2 c3 c4 } res] {
  set  mistake 1
  puts "Faulty ${BugNumber}: filling problem"
} else {
  puts "Filling ${BugNumber}  OK"
  set What [whatis result]
  if { [regexp "surface" $What] != 1 } {
    set  mistake 1
  } else {
    set  mistake 0
  }
}

# Resume
puts ""
if { ${mistake} != 0 } {
   puts "Faulty ${BugNumber}"
} else {
   puts "OK ${BugNumber}"
}
```

### Test: bugs/filling/bug23343
- **Target Suite**: OCCT_Bugs_filling
- **Target Case**: bug23343
- **Data Files**: `bug23343_initFace.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\bug23343_initFace.brep`), `bug23343_edge_constraint.brep` (Path: `E:\code\occtSrc\OCCT\data\brep\bug23343_edge_constraint.brep`)
```tcl
puts "TODO OCC23343 ALL: An exception was caught"
puts "TODO OCC23343 ALL: Faulty OCC23343"

puts "============"
puts "0023343: Crash (A C C E S S  V I O L A T I O N) in GeomPlate_BuildPlateSurface / MakeApprox if init Surface is set"
puts "============"
puts ""

restore [locate_data_file bug23343_initFace.brep] initFace
restore [locate_data_file bug23343_edge_constraint.brep] edge_constraint

point p1 30 -33.4729635533385 49.7661550602442
point p2 30 -49.6961550602442 33.3929635533386
point p3 23.3333333333333 -50 30.07
point p4 -30 -33.4729635533386 49.6161550602442

if [catch {gplate result 1 4 initFace edge_constraint 0 p1 p2 p3 p4} catch_result] {
   puts "Faulty OCC23343: there is crash (ACCESS VIOLATION) in GeomPlate_BuildPlateSurface"
} else {
   puts "OK OCC23343: there is not crash (ACCESS VIOLATION) in GeomPlate_BuildPlateSurface"
}

checkview -display result -2d -path ${imagedir}/${test_image}.png
```

### Test: bugs/filling/bug27775
- **Target Suite**: OCCT_Bugs_filling
- **Target Case**: bug27775
- **Suggested C++**: BRepOffsetAPI_MakeFilling, GeomFill_BSplineCurves / GeomFill_ConstrainedFilling, Geom_BSplineCurve
```tcl
puts "REQUIRED All: Standard_ConstructionError\: GeomFill_BSplineCurves\: invalid filling style"

puts "========"
puts "OCC27704"
puts "========"
puts ""
#################################################
# Different behavior of GeomFill_BSplineCurves algorithm in DEBUG and RELEASE mode
#################################################

#Indeed, the input data are invalid here:
#1. Algorithm requires B-Spline with 4+ poles. Source curves contains two pole only.
#2. Four curves must comprise closed region. But it is not here.
#Therefore, the normal behavior is to throw an exception.

bsplinecurve c1 1 2 0 2 100.000001513789 2 -24033.3957701043 -6337.90755953146 -16577.8188547128 1 -23933.3957701044 -6337.90755953146 -16577.8362547128 1
bsplinecurve c2 1 2 0 2 100.000001513789 2 -23933.3957701044 -6337.90755953146 -16577.8362547128 1 -24033.3957701043 -6337.90755953146 -16577.8188547128 1
bsplinecurve c3 1 2 0 2 33.1099999999979 2 -24033.3957701043 -6371.01755953146 -16577.8188547128 1 -24033.3957701043 -6337.90755953146 -16577.8188547128 1
bsplinecurve c4 1 2 0 2 100.000001513789 2 -24033.3957701043 -6371.01755953146 -16577.8188547128 1 -23933.3957701044 -6371.01755953146 -16577.8362547128 1

if {[catch {fillcurves res c1 c2 c3 c4 2}]} {
  puts "OK: The incorrect input data were processed correctly!"  
} else {
  puts "Error: The output result must be invalid because the input data are wrong!"
}
```

--- BATCH 1 END ---

