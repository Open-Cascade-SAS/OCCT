puts "========"
puts "OCC29972: Intersection curve has a weird gap in the middle of it"
puts "========"
puts ""

set GoodNbCurves 6

foreach a [directory res*] {unset $a}

torus s1 -127 84 100 0 0 -1 1 0 0 23 10
cylinder s2 -132 61.5500556793564 131 0 0 -1 1 0 0 10

intersect res s1 s2 1.0e-4

if { [info exists res] } {
  #Only variable "res" exists
  renamevar res res_1
}

bclearobjects
bcleartools

set ic 1
set AllowRepeat 1
while { $AllowRepeat != 0 } {
  if { ![info exists res_$ic] } {
    set AllowRepeat 0
  } else {
    bounds res_$ic U1 U2
    
    if {[dval U2-U1] < 1.0e-9} {
      puts "Error: Wrong curve's range!"
    }
    
    xdistcs res_$ic s1 U1 U2 100 2.0e-7
    xdistcs res_$ic s2 U1 U2 100 2.0e-7
    
    mkedge ee res_$ic
    baddobjects ee
    
    incr ic
  }
}

# Check of gaps between intersection curves
bfillds
bbuild rs

checknbshapes rs -edge $GoodNbCurves

regexp {nb alone Vertices : ([-0-9.+eE]+)} [ checksection rs -r 4 ] full nbv
if { $nbv < 2 } {
  puts "Error : The section is closed. The result is possible to better than on MASTER version. Please check carefully."
} else {
  for {set i 1} {$i < $nbv} {incr i} {
    for {set j [expr $i +1] } {$j <= $nbv} {incr j} {
      distmini dd alone_$i alone_$j;
      
      if { [dval dd_val] > 3.0e-6 } {
        puts "Error: The distance between alone_$i and alone_$j is too big"
      }
    }
  }
}

smallview
don res_*

fit
don s1 s2
disp res_*

checkview -screenshot -2d -path ${imagedir}/${test_image}.png