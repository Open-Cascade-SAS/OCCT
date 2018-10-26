puts "============"
puts "OCC27252"
puts "============"
puts ""
#######################################################################
# Implicit-implicit intersection (Cylinder-Plane) loses intersection curve
#######################################################################

set GoodNbCurv 1
set MaxTol 1.0e-7

restore [locate_data_file bug27221.brep] a
explode a
mkplane f a_2
prism p f -25 0 0 
explode a_1 f
explode p f
don a_1_7 p_3

set log [bopcurves a_1_7 p_3 -2d]

regexp {Tolerance Reached=+([-0-9.+eE]+)\n+([-0-9.+eE]+)} ${log} full Toler NbCurv

if {${NbCurv} != ${GoodNbCurv}} {
  puts "Error: Curve Number is bad!"
}

if {${Toler} > ${MaxTol}} {
  puts "Error: Tolerance is too big!"
}

smallview
don c_*
fit
display a_1_7 p_3

checkview -screenshot -2d -path ${imagedir}/${test_image}.png
