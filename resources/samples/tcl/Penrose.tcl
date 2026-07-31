# Generate set of boxes resembling OCC logo and arranged in the shape of 
# Penrose triangle on perspective view.
# The sample shows how the viewer can be manipulated to produce required 
# visual effect

#Category: Visualization
#Title: Penrose triangle on perspective view

pload MODELING VISUALIZATION
# Ray-Tracing doesn't work with Compatible Profile on macOS
if { $::tcl_platform(os) == "Darwin" } { vcaps -core }

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
  set dim [bounding b$i -save xmin ymin zmin xmax ymax zmax]
  set dx [dval xmax-xmin]
  set x1 [dval xmin+0.1*$dx]
  set x2 [dval ymin+0.1*$dx]
  set x3 [dval zmin+0.1*$dx]
  box bc $x1 $x2 $x3 0.8*$dx 0.8*$dx $dx
  bcut bb b$i bc
  tcopy bb b$i
}

# prepare a view
vinit Penrose w=1024 h=512
vbackground -color WHITE
vrenderparams -rayTrace -fsaa on -reflections off -shadows off

# set camera position and adjust lights
vcamera -persp -fovy 25
vviewparams -eye 14 -14 6.8 -up 0 0 1 -at 4 -4 0 -scale 70
vsetdispmode 1
vlight -defaults
vlight dirlight1 -type DIRECTIONAL -direction 1 -2 -10 -head 1 -color WHITE
vlight dirlight2 -type DIRECTIONAL -direction 0 -10 0  -head 1 -color WHITE

# display boxes
vdisplay b1 b2 b3 b4 b5 b6 b7 b8 b9 b10

# set colors like in boxes of on OCC logo
vsetcolor b1  DD0029
vsetcolor b2  F6DD00
vsetcolor b3  98F918
vsetcolor b4  E9007A
vsetcolor b5  007ABC
vsetcolor b6  93007A
vsetcolor b7  EE9800
vsetcolor b8  00B489
vsetcolor b9  00A47A
vsetcolor b10 007ABC

# set material to plastic for better look
for {set i 1} {$i <= 10} {incr i} {vsetmaterial b$i plastic}

vdrawtext label "Which\nbox\nis\ncloser\nto\nyou?" -pos 0 -6 -2 -color BLACK -halign left -valign bottom -angle 0 -zoom 0 -height 40
