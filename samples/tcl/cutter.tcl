# Sample: creation of milling cutter
#Category: Modeling
#Title: Milling cutter

pload MODELING VISUALIZATION

# parameters
dset R  10.    ;# outer radius
dset H  20.    ;# height of the spiral part
dset a  0.5*pi ;# total angle of spiral rotation

# make profile

circle outer 0 0 0 0 0 1 R
circle inner 0 0 0 0 0 1 R-2
circle round 3 6 0 0 0 1 5
polyline cut -2 0 0  -2 R+1 0  0.5 R+1 0  -1 0 0  -2 0 0

proc _curvetoface {name} {
  uplevel #0 mkedge $name $name
  uplevel #0 wire $name $name
  uplevel #0 mkface $name p0 $name
}

plane p0
_curvetoface outer
_curvetoface inner
_curvetoface round
mkface cut p0 cut

bcommon teeth cut round
bcommon teeth teeth outer

tcopy inner profile
set nbteeths 25
for {set i 0} {$i < $nbteeths} {incr i} {
  fuse profile profile teeth
  trotate teeth 0 0 0 0 0 1 360./$nbteeths
}

# sweep profile along curved shape

polyline sp 0 0 0  0 0 H
cylinder ss 0 0 0  0 0 1  1 0 0  R
vertex v1 R 0 0
vertex v2 R 0 H
line ll 0 0 a H
trim ll ll 0 sqrt(a*a+H*H)

trotate v2 0 0 0 0 0 1 180.*a/pi
mkedge ee ll ss v1 v2
wire gg ee
 
donly profile sp gg
explode profile w
tcopy profile_1 profile_2 
tcopy profile_1 profile_3
tscale profile_1  0 0 0  1./(R-2) 
ttranslate profile_2 0 0 H/2
tscale profile_2  0 0 H/2  4./(R-2) 
ttranslate profile_3 0 0 H
tscale profile_3  0 0 H  3./(R-2) 

mksweep sp
setsweep -G gg 0 0
addsweep profile_1
addsweep profile_2
addsweep profile_3
vclear

buildsweep base -S

# make a shank
plane p1 0 0 H 0 0 1
pcylinder shank p1 1.4 H
plane p2 0 0 H+1 0 0 1
pcylinder sh2 p2 1.5 H-1
bfuse shank shank sh2
bfuse cutter base shank

# check result
checkshape cutter

# show result
incmesh cutter 0.01
vdisplay cutter
vsetdispmode cutter 1
vrenderparams -msaa 8
incmesh profile_1 0.01; vdisplay profile_1
vfit
