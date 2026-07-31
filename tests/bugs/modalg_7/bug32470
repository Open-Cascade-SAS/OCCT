puts "============================================"
puts "OCC32470: BOP wrong result on sphere and box"
puts "============================================"
puts ""

psphere sph 10
box b 20 100 100
ttranslate b 0 -50 -50
trotate b 0 0 10 0 -1 0 45
bcut result sph b

checkshape result

checknbshapes result -t -vertex 3 -edge 5 -wire 2 -face 2 -shell 1 -solid 1

set tolres [checkmaxtol result]

if { ${tolres} > 1.e-5} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 1229.69 -v 3945.6
