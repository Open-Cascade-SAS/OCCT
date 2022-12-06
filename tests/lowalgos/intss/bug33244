
puts "============"
puts "0033244: Modeling Algorithms - Surface-surface intersection produces the double curves"
puts "============"
puts ""


set PI180 0.017453292519943295

set x -1.11630646267172
set y -4.54487349779333
set z 13.2493435203532
set dx -1.05794851588922e-07
set dy -1.39278337794573e-08
set dz  0.999999999999994
set ux 0.999999999999994
set uy 5.91645678915759e-31
set uz 1.05794851588922e-07
set semi-angle [expr 0.785398163360967 / ${PI180}]
set radius 0.560000000061149

cone s1 ${x} ${y} ${z} ${dx} ${dy} ${dz} ${semi-angle} ${radius}

set x -2.08647872350287e-07
set y -5.78732475509323
set z 13.2493436211
set dx -1.05794850062242e-07
set dy -1.39278350756825e-08
set dz  0.999999999999994
set ux 0.999999999999995
set uy 0
set uz 1.05794850062242e-07
set semi-angle [expr 0.785398163396248 / ${PI180}]
set radius 0.785398163396248

cone s2 ${x} ${y} ${z} ${dx} ${dy} ${dz} ${semi-angle} ${radius}

intersect ii s1 s2
if { ![isdraw ii_1] || ![isdraw ii_2] || [isdraw ii_3] } {
   puts "ERROR. Intersection is wrong"
}

intersect jj s2 s1
if { ![isdraw jj_1] || ![isdraw jj_2] || [isdraw jj_3] } {
   puts "ERROR. Intersection is wrong"
}


