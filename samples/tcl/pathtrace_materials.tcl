# Script demonstrating Global illumination materials
# using path tracing rendering engine in 3D view

#Category: Visualization
#Title: Path tracing - Materials

set aBallPath [locate_data_file occ/Ball.brep]

pload MODELING VISUALIZATION

# Setup 3D viewer
vclear
vinit name=View1 w=512 h=512
vglinfo
vvbo 0
vsetdispmode 1

# Restore exported shapes
restore $aBallPath Ball0
restore $aBallPath Ball1
restore $aBallPath Ball2
restore $aBallPath Ball3
restore $aBallPath Ball4
restore $aBallPath Ball5
restore $aBallPath Ball6
restore $aBallPath Ball7
restore $aBallPath Ball8

# Create chessboard-style floor
box tile 10 10 0.1
eval compound [lrepeat 144 tile] tiles
explode tiles
for {set i 0} {$i < 12} {incr i} {
  for {set j 1} {$j <= 12} {incr j} {
    ttranslate tiles_[expr 12 * $i + $j] [expr $i * 10 - 90] [expr $j * 10 - 70] -0.15
    vdisplay -noupdate tiles_[expr 12 * $i + $j]

    vsetmaterial -noupdate tiles_[expr 12 * $i + $j] plaster

    if {($i + $j) % 2 == 0} {
      vbsdf tiles_[expr 12 * $i + $j] -kd 0.85
    } else {
      vbsdf tiles_[expr 12 * $i + $j] -kd 0.45
    }
  }
}

# Setup object 'Ball1'
vdisplay Ball1
vsetmaterial Ball1 Brass
vbsdf Ball1 -Kc 0 0 0
vbsdf Ball1 -Kd 0.272798 0.746262 0.104794
vbsdf Ball1 -Ks 0.253738 0.253738 0.253738
vbsdf Ball1 -Kt 0 0 0
vbsdf Ball1 -baseRoughness 0.045
vbsdf Ball1 -coatRoughness 0
vbsdf Ball1 -Le 0 0 0
vbsdf Ball1 -absorpColor 0 0 0
vbsdf Ball1 -absorpCoeff 0
vbsdf Ball1 -coatFresnel Constant 0
vbsdf Ball1 -baseFresnel Schlick 0.58 0.42 0.2
vlocation Ball1 -rotation 0 0 0 1
vlocation Ball1 -location 10 0 0

# Setup object 'Ball2'
vdisplay Ball2
vsetmaterial Ball2 Brass
vbsdf Ball2 -Kc 0 0 0
vbsdf Ball2 -Kd 0.8 0.8 0.8
vbsdf Ball2 -Ks 0 0 0
vbsdf Ball2 -Kt 0 0 0
vbsdf Ball2 -baseRoughness 0
vbsdf Ball2 -coatRoughness 0
vbsdf Ball2 -Le 2.02 0.171915 0.171915
vbsdf Ball2 -absorpColor 0 0 0
vbsdf Ball2 -absorpCoeff 0
vbsdf Ball2 -coatFresnel Constant 0
vbsdf Ball2 -baseFresnel Constant 1
vlocation Ball2 -rotation 0 0 0 1
vlocation Ball2 -location 10 40 0

# Setup object 'Ball3'
vdisplay Ball3
vsetmaterial Ball3 Glass
vbsdf Ball3 -Kc 1 1 1
vbsdf Ball3 -Kd 0 0 0
vbsdf Ball3 -Ks 0 0 0
vbsdf Ball3 -Kt 1 1 1
vbsdf Ball3 -baseRoughness 0
vbsdf Ball3 -coatRoughness 0
vbsdf Ball3 -Le 0 0 0
vbsdf Ball3 -absorpColor 0.75 0.95 0.9
vbsdf Ball3 -absorpCoeff 0.05
vbsdf Ball3 -coatFresnel Dielectric 1.62
vbsdf Ball3 -baseFresnel Constant 1
vlocation Ball3 -rotation 0 0 0 1
vlocation Ball3 -location -30 -40 0

# Setup object 'Ball4'
vdisplay Ball4
vsetmaterial Ball4 Brass
vbsdf Ball4 -Kc 0 0 0
vbsdf Ball4 -Kd 0 0 0
vbsdf Ball4 -Ks 0.985 0.985 0.985
vbsdf Ball4 -Kt 0 0 0
vbsdf Ball4 -baseRoughness 0
vbsdf Ball4 -coatRoughness 0
vbsdf Ball4 -Le 0 0 0
vbsdf Ball4 -absorpColor 0 0 0
vbsdf Ball4 -absorpCoeff 0
vbsdf Ball4 -coatFresnel Constant 0
vbsdf Ball4 -baseFresnel Schlick 0.58 0.42 0.2
vlocation Ball4 -rotation 0 0 0 1
vlocation Ball4 -location -70 -40 0

# Setup object 'Ball5'
vdisplay Ball5
vsetmaterial Ball5 Glass
vbsdf Ball5 -Kc 1 1 1
vbsdf Ball5 -Kd 0 0 0
vbsdf Ball5 -Ks 0 0 0
vbsdf Ball5 -Kt 1 1 1
vbsdf Ball5 -baseRoughness 0
vbsdf Ball5 -coatRoughness 0
vbsdf Ball5 -Le 0 0 0
vbsdf Ball5 -absorpColor 0 0.288061 0.825532
vbsdf Ball5 -absorpCoeff 0.3
vbsdf Ball5 -coatFresnel Dielectric 1.62
vbsdf Ball5 -baseFresnel Constant 1
vlocation Ball5 -rotation 0 0 0 1
vlocation Ball5 -location -30 0 0

# Setup object 'Ball6'
vdisplay Ball6
vsetmaterial Ball6 Brass
vbsdf Ball6 -Kc 1 1 1
vbsdf Ball6 -Kd 0 0.716033 0.884507
vbsdf Ball6 -Ks 0.115493 0.115493 0.115493
vbsdf Ball6 -Kt 0 0 0
vbsdf Ball6 -baseRoughness 0.045
vbsdf Ball6 -coatRoughness 0
vbsdf Ball6 -Le 0 0 0
vbsdf Ball6 -absorpColor 0 0 0
vbsdf Ball6 -absorpCoeff 0
vbsdf Ball6 -coatFresnel Dielectric 1.5
vbsdf Ball6 -baseFresnel Schlick 0.58 0.42 0.2
vlocation Ball6 -rotation 0 0 0 1
vlocation Ball6 -location -30 40 0

# Setup object 'Ball7'
vdisplay Ball7
vsetmaterial Ball7 Brass
vbsdf Ball7 -Kc 1 1 1
vbsdf Ball7 -Kd 1e-06 9.9999e-07 9.9999e-07
vbsdf Ball7 -Ks 0.0479573 0.804998 0
vbsdf Ball7 -Kt 0 0 0
vbsdf Ball7 -baseRoughness 0.447
vbsdf Ball7 -coatRoughness 0
vbsdf Ball7 -Le 0 0 0
vbsdf Ball7 -absorpColor 0 0 0
vbsdf Ball7 -absorpCoeff 0
vbsdf Ball7 -coatFresnel Dielectric 1.5
vbsdf Ball7 -baseFresnel Schlick 0.58 0.42 0.2
vlocation Ball7 -rotation 0 0 0 1
vlocation Ball7 -location -70 0 0

# Setup object 'Ball8'
vdisplay Ball8
vsetmaterial Ball8 Aluminium
vbsdf Ball8 -Kc 0 0 0
vbsdf Ball8 -Kd 0 0 0
vbsdf Ball8 -Ks 0.985 0.985 0.985
vbsdf Ball8 -Kt 0 0 0
vbsdf Ball8 -baseRoughness 0.026
vbsdf Ball8 -coatRoughness 0
vbsdf Ball8 -Le 0 0 0
vbsdf Ball8 -absorpColor 0 0 0
vbsdf Ball8 -absorpCoeff 0
vbsdf Ball8 -coatFresnel Constant 0
vbsdf Ball8 -baseFresnel Schlick 0.913183 0.921494 0.924524
vlocation Ball8 -rotation 0 0 0 1
vlocation Ball8 -location -70 40 0

# Setup object 'Ball0'
vdisplay Ball0
vsetmaterial Ball0 Glass
vbsdf Ball0 -Kc 0 0 0
vbsdf Ball0 -Kd 0.723404 0.166229 0.166229
vbsdf Ball0 -Ks 0 0 0
vbsdf Ball0 -Kt 0 0 0
vbsdf Ball0 -baseRoughness 0
vbsdf Ball0 -coatRoughness 0
vbsdf Ball0 -Le 0 0 0
vbsdf Ball0 -absorpColor 0 0 0
vbsdf Ball0 -absorpCoeff 0
vbsdf Ball0 -coatFresnel Constant 0
vbsdf Ball0 -baseFresnel Constant 1
vlocation Ball0 -rotation 0 0 0 1
vlocation Ball0 -location 10 -40 0

# Restore view parameters
vcamera -perspective -fovy 25
vcamera -distance 238.089
vviewparams -proj 0.679219 -0.00724546 0.7339
vviewparams -up -0.733931 -0.00311795 0.679217
vviewparams -at -22.3025 0.0986351 3.30327
vviewparams -eye 139.412 -1.62643 178.037
vviewparams -size 170.508

# Restore light source parameters
vlight clear
vlight add directional direction -0.303949 -0.434084 -0.848048 smoothness 0.3 intensity 12

# Load environment map
vtextureenv on 1

puts "Trying path tracing mode..."
vrenderparams -ray -gi -rayDepth 10

# Start progressive refinement mode
#vprogressive

puts "Make several path tracing iterations to refine the picture, please wait..."
vfps 512
puts "Done. To improve the image further, or after view manipulations, give command:"
puts "vfps \[nb_iteratons\]" 
