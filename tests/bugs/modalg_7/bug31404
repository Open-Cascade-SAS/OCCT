puts "======================================================="
puts "0031404: Modeling Algorithms - BOP Fuse produces a self-interfering or a good shape depending on the order of arguments"
puts "======================================================="
puts ""

psphere sph1 25
plane pln2 32, 68, -27
psphere sph2 pln2 75
bfuse f1 sph1 sph2
savehistory h1
bfuse f2 sph2 sph1
savehistory h2

explode sph1 f
modified ff1 h1 sph1_1
modified ff2 h2 sph1_1

foreach r {f1 f2} {
  checkshape $r
  if {![regexp "OK" [bopcheck $r]]} {
    puts "Error: the $r is a self-interfering shape"
  }
  if { [checkmaxtol $r] > 2.e-7 } {
    puts "Error: $r has bad tolerance"
  }
}

checknbshapes f1 -wire 3 -face 2 -edge 9 -vertex 6
checknbshapes f2 -wire 3 -face 2 -edge 9 -vertex 6

checkprops f1 -s 74192.6
checkprops f2 -s 74192.6

view 1 -2D-  0 200 500 300

pcurve ff1
pcurve ff2 
2dtranslate ff2_6 pi 0
donly ff1_6 ff2_6
2dfit

checklength ff1_6 -l 7.167
checklength ff2_6 -l 7.167

checkview -screenshot -2d -path ${imagedir}/${test_image}.png

