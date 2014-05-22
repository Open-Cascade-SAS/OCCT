puts "========="
puts " OCC497 "
puts "(case 3)"
puts "========="
puts ""
########################################################
## Result of cut and fuse operation are faulty shapes although arguments are valid.
########################################################

restore [locate_data_file OCC497e.brep] a_1 
checkshape a_1

restore [locate_data_file OCC497f.brep] a_2
checkshape a_2

bcut result a_1 a_2

set square 1773.6
set 2dviewer 0
