puts "================================================================="
puts "OCC31470: BOP common produces empty result (box and holed sphere)"
puts "================================================================="
puts ""

plane pln1 51.899912462 99.996691888 62.33204004
psphere s1 pln1 15
psphere s2 pln1 10
bcut ss s1 s2
box bb 100 100 100

bcommon result bb ss
savehistory hh

explode ss f
modified ff hh ss_1
generated ee hh ss_1

checkshape result

checknbshapes result -solid 1 -shell 1 -face 3 -wire 6 -edge 8 -vertex 6

set tolres [checkmaxtol result]

if { ${tolres} > 2.e-7} {
   puts "Error: bad tolerance of result"
}

checkprops result -v 4975.49

v2d
pcurve pc ee ff
trim pc pc
2dfit

checkview -screenshot -2d -path ${imagedir}/${test_image}.png
