puts "=========="
puts "OCC25715"
puts "=========="
puts ""
#############################################################
# Intersection between cylinders produces excess vertices
#############################################################
# don't change nbshapes values according to 25715

pcylinder s1 0.069 0.6
pcylinder s2 0.024, 0.4
trotate s1 0 0 0 0 0 1 90
trotate s2 0 0 0 0 1 0 90
trotate s2 0 0 0 1 0 0 -45
ttranslate s2 -0.2 0 0.48
bcut result s1 s2

set nb_v_good 4
set nb_e_good 6
set nb_w_good 6
set nb_f_good 4
set nb_sh_good 1
set nb_sol_good 1
set nb_compsol_good 0
set nb_compound_good 1
set nb_shape_good 23

set 2dviewer 1
