puts "========"
puts "OCC27773"
puts "========"
puts ""
#################################################
# Empty result of section operation between line and offset of a circle
#################################################

circle c 1 0 0 20
offset c1 c 10 0 0 1
mkedge e1 c1
vertex v1 0 35 0
vertex v2 0 -35 0
edge e2 v1 v2
bsection result e1 e2
checknbshapes result -vertex 2
