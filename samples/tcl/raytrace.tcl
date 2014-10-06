# Script demonstrating ray tracing in 3d view

#Category: Visualization
#Title: Ray tracing

# make bottle by calling another script
source [file join [file dirname [info script]] bottle.tcl]

# make table and a glass
box table -50 -50 -10 100 100 10
pcone glass_out 7 9 25
pcone glass_in 7 9 25
ttranslate glass_in 0 0 0.2
bcut glass glass_out glass_in
ttranslate glass -30 -30 0

# show table and glass
vinit w=1024 h=1024
vsetmaterial bottle aluminium
vdisplay table
vsetmaterial table bronze
vsetmaterial table plastic
vsetcolor table coral2
vdisplay glass
vsetmaterial glass plastic
vsetcolor glass brown
vsettransparency glass 0.6

# add light source for shadows
vlight new spot pos -100 -100 300

# set white background and fit view
vsetcolorbg 255 255 255
vfit

# set ray tracing
puts "Trying raytrace mode..."
if { ! [catch {vrenderparams -raytrace -shadows -reflections -fsaa -rayDepth 5}] } {
  vtextureenv on 1
}
