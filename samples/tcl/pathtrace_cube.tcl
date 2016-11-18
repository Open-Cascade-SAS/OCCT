# Script demonstrating Global illumination effects using non-interactive 
# path tracing rendering engine in 3d view

#Category: Visualization
#Title: Path tracing - Cube

pload MODELING VISUALIZATION

# setup 3D viewer content
vclear
vinit name=View1 w=512 h=512
vglinfo

# setup light sources
vlight del 0
vlight del 1
vlight add positional head 0 pos 0.5 0.5 0.85
vlight change 0 sm 0.06
vlight change 0 int 30.0

vvbo 0
vsetdispmode 1
vcamera -persp

# setup outer box
box b 1 1 1 
explode b FACE 
vdisplay -noupdate b_1 b_2 b_3 b_5 b_6
vlocation -noupdate b_1 -setLocation  1  0  0
vlocation -noupdate b_2 -setLocation -1  0  0
vlocation -noupdate b_3 -setLocation  0  1  0
vlocation -noupdate b_5 -setLocation  0  0  1
vlocation -noupdate b_6 -setLocation  0  0 -1

vsetmaterial -noupdate b_1 plastic
vsetmaterial -noupdate b_2 plastic
vsetmaterial -noupdate b_3 plastic
vsetmaterial -noupdate b_5 plastic
vsetmaterial -noupdate b_6 plastic
vbsdf b_1 -kd 1 0.3 0.3 -ks 0
vbsdf b_2 -kd 0.3 0.5 1 -ks 0
vbsdf b_3 -kd 1 -ks 0
vbsdf b_5 -kd 1 -ks 0
vbsdf b_6 -kd 1 -ks 0

vfront
vfit

# setup first inner sphere
psphere s 0.2
vdisplay     -noupdate s
vlocation    -noupdate s -setLocation 0.21 0.3 0.2
vsetmaterial -noupdate s glass
vbsdf s -absorpColor 0.8 0.8 1.0
vbsdf s -absorpCoeff 6

# setup first inner box
box c 0.3 0.3 0.2
vdisplay     -noupdate c
vlocation    -noupdate c -setLocation 0.55 0.3 0.0
vlocation    -noupdate c -rotate 0 0 0 0 0 1 -30
vsetmaterial -noupdate c plastic
vbsdf c -kd 1.0 0.8 0.2 -ks 0.3 -n

# setup second inner box
box g 0.15 0.15 0.3
vdisplay     -noupdate g
vlocation    -noupdate g -setLocation 0.7 0.25 0.2
vlocation    -noupdate g -rotate 0 0 0 0 0 1 10
vsetmaterial -noupdate g glass
vbsdf g -absorpColor 0.8 1.0 0.8
vbsdf g -absorpCoeff 6

# setup second inner sphere
psphere r 0.1
vdisplay -noupdate r
vsetmaterial -noupdate r plastic
vbsdf r -kd 0.5 0.9 0.3 -ks 0.3 -baseRoughness 0.0 -n
vbsdf r -baseFresnel Constant 1.0
vlocation r -setLocation 0.5 0.65 0.1

puts "Trying path tracing mode..."
vrenderparams -ray -gi -rayDepth 8

puts "Make several path tracing iterations to refine the picture, please wait..."
vfps 100
puts "Done. To improve the image further, or after view manipulations, give command:"
puts "vfps \[nb_iteratons\]"
