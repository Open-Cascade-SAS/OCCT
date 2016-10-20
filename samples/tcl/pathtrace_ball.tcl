# Script demonstrating Global illumination materials
# using path tracing rendering engine in 3D view

#Category: Visualization
#Title: Path tracing - Ball

set aBallPath [locate_data_file occ/Ball.brep]

pload MODELING VISUALIZATION

# Setup 3D viewer
vclear
vinit name=View1 w=512 h=512
vglinfo
vvbo 0
vsetdispmode 1

# Setup view parameters
vcamera -persp
vviewparams -scale 18 -eye 44.49 -0.15 33.93 -at -14.20 -0.15 1.87 -up -0.48 0.00 0.88

# Load the model from disk
puts "Importing shape..."
restore $aBallPath ball

# Tessellate the model
incmesh ball 0.01

# Display the model and assign material
vdisplay -noupdate ball
vsetmaterial -noupdate ball glass

# Create a sphere inside the model
psphere s 8
incmesh s 0.01
vdisplay -noupdate s
vsetlocation -noupdate s 0 0 13
vsetmaterial -noupdate s plaster

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

# Configure light sources
vlight del 1
vlight change 0 head 0
vlight change 0 direction -0.25 -1 -1
vlight change 0 sm 0.3
vlight change 0 int 10

# Load environment map
vtextureenv on 1

puts "Trying path tracing mode..."
vrenderparams -ray -gi -rayDepth 10

# Start progressive refinement mode
#vprogressive

puts "Make several path tracing iterations to refine the picture, please wait..."
vfps 100
puts "Done. To improve the image further, or after view manipulations, give command:"
puts "vfps \[nb_iteratons\]" 
