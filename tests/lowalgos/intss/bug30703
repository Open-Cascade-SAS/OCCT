puts "============"
puts "0030703: Modeling Algorithms - Intersector returns overlapping curves"
puts "============"
puts ""

restore [locate_data_file bug30703s1] s1
restore [locate_data_file bug30703s2] s2

set info [intersect res s1 s2]
# Number of solutions check. 
# There should be only 2 solution.
if {[llength $info] != 1} {
  Error: Incorrect number of solutions.
} 

checklength res -l 22.9782037

donly s2 res
clpoles s2
clknots s2
clpoles res
clknots res

smallview

fit

checkview -screenshot -2d -path ${imagedir}/${test_image}.png
