puts "================================"
puts "0033615: Modeling Algorithms - Partition algorithm creates unexpected vertices"
puts "================================"
puts ""

plane plane -5 0 4 -1 0 0
pcone cone plane 3 1 10 
pcylinder cylinder 10 20 
explode cylinder f 
explode cone f 

don cylinder_1 cone_1 
axo;fit 
bclearobjects
bcleartools
baddobjects cone_1 
baddtools cylinder_1 
bfillds 
bbuild result

checknbshapes result -vertex 5 -edge 8 -wire 5 -face 4
checkview -display result -2d -path ${imagedir}/${test_image}.png
