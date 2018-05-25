puts "========"
puts "0029807: Impossible to cut cone from prism"
puts "========"
puts ""

# Faces f2 and f6 are taken from the test case "boolean volumemaker E7"

cone con_f1 0 -60.919306349999999 0 0 -1 1.1102230246251565e-016 28.800000000062262 0
mkface f1 con_f1 0 6.2831853071795862 0 1000000

cone con_f5 0 -309.47272469000001 0 0 -1 1.1102230246251565e-016 43.999999999485127 0
mkface f5 con_f5 0 6.2831853071795862 0 1000000

bsection result f1 f5

smallview
don result
fit
disp f1 f5

checksection result -r 0

checkview -screenshot -2d -path ${imagedir}/${test_image}_3d.png

checkmaxtol result -ref 6.6226289034767669e-007
checkprops result -l 1993.34
checknbshapes result -vertex 1 -edge 1

checkshape result

if {[regexp "Faulties" [bopargcheck result]]} {
  puts "Error: bopargcheck has found some faulties in result"
}