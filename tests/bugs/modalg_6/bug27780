puts "============"
puts "OCC27780"
puts "============"
puts ""
######################################################
# Face-face intersection produces 2D curve that has reversed derivative at its end
######################################################

restore [locate_data_file buc60532a.brep] p 
restore [locate_data_file buc60532b.brep] t1
explode t1 f

bopcurves p t1_2 -2d -p 6.2406621764215551 0.23999999463558200 0.00034444887595448459 -5.0019657458625186

pcurve p
trim c c2d1_1 0 1
2dcvalue c 0 u0 v0
2dcvalue c 1 u1 v1 du dv
dset u01 u1-u0
dset v01 v1-v0
dset dot u01*du+v01*dv

if {[dval dot] < 0} {
  puts "Error: p-curve has reversed direction at its end"
} else {
  puts "OK: p-curve has correct direction at its end"
}

view 1 -2D- 728 450 400 400
don p_* c
2dfit

checkview -screenshot -2d -path ${imagedir}/${test_image}.png
