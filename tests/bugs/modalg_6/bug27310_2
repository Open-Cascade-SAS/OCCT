puts "========"
puts "OCC27310"
puts "========"
puts ""
#################################################
# Huge tolerance obtained in the result of intersection of two cylindrical faces
#################################################

set ExpTol 6.9230965382090094e-006
set GoodNbCurv 2

restore [locate_data_file OCC496a.brep] a 
restore [locate_data_file OCC496b.brep] b

explode a f
explode b f

set log [bopcurves a_10 b_4 -2d]

regexp {Tolerance Reached=+([-0-9.+eE]+)\n+([-0-9.+eE]+)} ${log} full Toler NbCurv

if {${NbCurv} != ${GoodNbCurv}} {
  puts "Error: Number of curves is bad!"
}

checkreal TolReached $Toler $ExpTol 0.0 0.1

smallview
don c_*
fit
disp a_10 b_4

checkview -screenshot -2d -path ${imagedir}/${test_image}.png
