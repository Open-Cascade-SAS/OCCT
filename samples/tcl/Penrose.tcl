# Generate set of boxes resembling OCC logo and arranged in the shape of 
# Penrose triangle on perspective view.
# The sample shows how the viewer can be manipulated to produce required 
# visual effect

#Category: Visualization
#Title: Penrose triangle on perspective view

pload MODELING VISUALIZATION 

# procedure to define box dimensions
set scale 1.
set ratio 0.94
proc defbox {} { 
  global scale ratio
  set scale [expr $scale * $ratio]
  return [list 1.8*$scale 1.8*$scale 1.3*$scale]
}

# make set of boxes
eval box b1 0 0 0 [defbox] 
eval box b2 2 0 0 [defbox]
eval box b3 4 0 0 [defbox] 
eval box b4 6 0 0 [defbox] 
eval box b5 6 -1.5 0 [defbox] 
eval box b6 6 -3 0 [defbox] 
eval box b7 6 -4.5 0 [defbox] 
eval box b8 6 -6 0 [defbox] 
eval box b9 6 -6 1 [defbox]
eval box b10 6 -6 2 [defbox]

# cut last box by prisms created from the first two to make impression
# that it is overlapped by these on selected view (see vviewparams below)
explode b1 f
explode b2 f
prism p0 b1_5 12.3 -14 6.8
bcut bx b10 p0
prism p1 b2_3 12 -14 6.8
bcut bxx bx p1
tcopy bxx b10

# make some boxes hollow
for {set i 1} {$i <= 1} {incr i} {
  set dim [boundingstr b$i]
  set dx [expr [lindex $dim 3] - [lindex $dim 0]]
  set x1 [expr [lindex $dim 0] + 0.1 * $dx]
  set x2 [expr [lindex $dim 1] + 0.1 * $dx]
  set x3 [expr [lindex $dim 2] + 0.1 * $dx]
  box bc $x1 $x2 $x3 0.8*$dx 0.8*$dx $dx
  bcut bb b$i bc
  tcopy bb b$i
}

# prepare a view
vinit Penrose w=1024 h=512
vsetcolorbg 255 255 255
vrenderparams -rayTrace -fsaa on -reflections off -shadows off

# set camera position and adjust lights
vcamera -persp -fovy 25
vviewparams -eye 14 -14 6.8 -up 0 0 1 -at 4 -4 0 -scale 70
vsetdispmode 1
vlight def
vlight add directional direction 1 -2 -10 head 1 color white
vlight add directional direction 0 -10 0 head 1 color white

# display boxes
vdisplay b1 b2 b3 b4 b5 b6 b7 b8 b9 b10

# set colors like in boxes of on OCC logo
vsetcolor b1 0.8671875 0 0.16015625
vsetcolor b2 0.96484375 0.8671875 0
vsetcolor b3 0.609375 0.97734375 0.09375
vsetcolor b4 0.90234375 0 0.48046875
vsetcolor b5 0 0.48046875 0.73828125
vsetcolor b6 0.578125 0 0.48046875
vsetcolor b7 0.93359375 0.609375 0
vsetcolor b8 0 0.70703125 0.9296875
vsetcolor b9 0 0.64453125 0.48046875
vsetcolor b10 0 0.48046875 0.73828125

# set material to plastic for better look
for {set i 1} {$i <= 10} {incr i} {vsetmaterial b$i plastic}

vdrawtext label "Which\nbox\nis\ncloser\nto\nyou?" -pos 0 -6 -2 -color 0 0 0 -halign left -valign bottom -angle 0 -zoom 0 -height 40
