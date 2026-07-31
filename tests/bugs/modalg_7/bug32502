puts "================================================="
puts "OCC32502: BOP wrong result on sphere and cylinder"
puts "================================================="
puts ""

psphere sph 10
plane pln 0 0 7 0 1 0 0 0 1
pcylinder cyl pln 3 40
ttranslate cyl 0 -20 0

bop sph cyl
bopfuse result

checkshape result

checknbshapes result -t -vertex 4 -edge 11 -wire 5 -face 5 -shell 1 -solid 1

set tolres [checkmaxtol result]

if { ${tolres} > 3.e-5} {
   puts "Error: bad tolerance of result"
}

checkprops result -s 1750.1 -v 4947.2
