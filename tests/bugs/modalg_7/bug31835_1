puts "============================================================================================="
puts "0031835: Modeling Algorithms - step by step Boolean common produces bad shape on given three cylinders"
puts "============================================================================================="
puts ""

plane p1 -200 2.22044604925033e-14 12.6935294289015 1 -1.11022302462516e-16 0
pcylinder c1 p1 100 400
plane p2 0 0 -187.306470571099 0 0 1
pcylinder c2 p2 100 400
plane p3 0 200 12.6935294289015 0 -1 1.11022302462516e-16
pcylinder c3 p3 100 400

bcommon r c1 c2
checkshape r
if {![regexp "OK" [bopcheck r]]} {
  puts "Error: r is self-intersecting shape"
}
checkmaxtol r -ref 2.e-7
checknbshapes r -wire 5 -face 5 -shell 1 -solid 1
checkprops r -s 160000 -v 5.33333e+06

bcommon result r c3
checkshape result
if {![regexp "OK" [bopcheck result]]} {
  puts "Error: result is self-intersecting shape"
}
checkmaxtol result -ref 2.e-7
checknbshapes result -wire 12 -face 12 -shell 1 -solid 1
checkprops result -s 140589 -v 4.68629e+06

bclearobjects
bcleartools
baddobjects c1 c2 c3
bfillds
bcbuild r

bcremoveall
bcadd result1 c1 1 c2 1 c3 1

checkprops result1 -equal result
checknbshapes result1 -ref [nbshapes result]
checkmaxtol result1 -ref 2.e-7

checkview -display result -2d -path ${imagedir}/${test_image}.png
