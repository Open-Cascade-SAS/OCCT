puts "======================================================"
puts "OCC32721: BOP wrong results on a cone and an extrusion"
puts "======================================================"
puts ""

restore [locate_data_file bug32721.brep] prism
pcone cone 6 0 10

bop cone prism
bopfuse r1
bopcommon r2
bopcut r3
boptuc r4

checkshape r1
checknbshapes r1 -t -vertex 8 -edge 17 -wire 8 -face 8 -shell 1 -solid 1
checkshape r2
checknbshapes r2 -t -vertex 3 -edge 7 -wire 4 -face 4 -shell 1 -solid 1
checkshape r3
checknbshapes r3 -t -vertex 4 -edge 10 -wire 4 -face 4 -shell 1 -solid 1
checkshape r4
checknbshapes r4 -t -vertex 7 -edge 14 -wire 8 -face 8 -shell 2 -solid 2

set tolres [checkmaxtol r1]

if { ${tolres} > 0.0002} {
   puts "Error: bad tolerance of result"
}

checkprops r1 -s 388.634 -v 406.357
checkprops r2 -s 57.8605 -v 22.8116
checkprops r3 -s 358.735 -v 354.179
checkprops r4 -s 87.7592 -v 29.3659

checkview -display r1 -2d -path ${imagedir}/${test_image}.png