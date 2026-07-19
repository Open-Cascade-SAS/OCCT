puts "========"
puts "0029807: Impossible to cut cone from prism"
puts "========"
puts ""

# Faces f2 and f6 are taken from the test case "boolean volumemaker C5 (C6)"

cone con_f2 0 518.47000000000003 0 0 -1 1.1102230246251565e-016 14.999999999912038 0
mkface f2 con_f2 0 6.2831853071795862 0 1000000

cone con_f6 0 -440.74363604000001 0 0 -1 1.1102230246251565e-016 45.110284878807235 0
mkface f6 con_f6 0 6.2831853071795862 0 1000000

bsection result f2 f6

smallview
don result
fit
disp f2 f6

checksection result -r 0

checkview -screenshot -2d -path ${imagedir}/${test_image}_3d.png

checkmaxtol result -ref 7.3189259943803184e-007
checkprops result -l 2202.91
checknbshapes result -vertex 1 -edge 1

checkshape result

if {[regexp "Faulties" [bopargcheck result]]} {
  puts "Error: bopargcheck has found some faulties in result"
}