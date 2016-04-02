# Script reproducing creation of bottle model as described in OCCT Tutorial
#Category: Modeling
#Title: OCCT Tutorial bottle shape

pload MODELING VISUALIZATION

puts "Constructing bottle body..."

# set basic dimensions
dset height 70
dset width 50
dset thickness 30

# construct base profile (half)
vertex v1 -width/2 0 0
vertex v2 -width/2 -thickness/4 0
edge e1 v1 v2

point p2 -width/2 -thickness/4 0
point p3 0 -thickness/2 0
point p4 width/2 -thickness/4 0
catch {gcarc arc cir p2 p3 p4}
mkedge e2 arc

vertex v4 width/2 -thickness/4 0
vertex v5 width/2 0 0
edge e3 v4 v5

wire w1 e1 e2 e3

# complete profile by mirror and make a prism
copy w1 w2
tmirror w2 0 0 0 0 1 0

wire w3 w1 w2
mkplane f w3

prism p f 0 0 height

# fillet all edges
explode p e
blend b p thickness/12 p_1 thickness/12 p_2 thickness/12 p_3 thickness/12 p_4 thickness/12 p_5 thickness/12 p_6 thickness/12 p_7 thickness/12 p_8 thickness/12 p_9 thickness/12 p_10 thickness/12 p_11 thickness/12 p_12 thickness/12 p_13 thickness/12 p_14 thickness/12 p_15 thickness/12 p_16 thickness/12 p_17 thickness/12 p_18

# neck dimensions
dset neckradius thickness/4
dset neckheight height/10

# add neck
pcylinder c neckradius neckheight
ttranslate c 0 0 height

bfuse f b c

# make body hollow
explode c f
offsetshape body f -thickness/50 1.e-3 c_2

puts "Constructing threading..."

# make two cylinders
cylinder c1 0 0 height 0 0 1 neckradius*0.99
cylinder c2 0 0 height 0 0 1 neckradius*1.05

# define threading dimensions in parametric space
dset major 2*pi
dset minor neckheight/10

# make parametric curves for threading
ellipse el1 2*pi neckheight/2 2*pi neckheight/4 major minor
ellipse el2 2*pi neckheight/2 2*pi neckheight/4 major minor/4

trim arc1 el1 0 pi
trim arc2 el2 0 pi

2dcvalue el1 0 x1 y1
2dcvalue el1 pi x2 y2

line l x1 y1 x2-x1 y2-y1
parameters l x2 y2 1.e-9 U
trim s l 0 U

# construct 3d edges and wires
mkedge E1OnS1 arc1 c1 0 pi
mkedge E2OnS1 s c1 0 U
mkedge E1OnS2 arc2 c2 0 pi
mkedge E2OnS2 s c2 0 U

wire tw1 E1OnS1 E2OnS1
wire tw2 E1OnS2 E2OnS2
mkedgecurve tw1 1.e-5
mkedgecurve tw2 1.e-5

# build threading as solid
thrusections -N thread 1 0 tw1 tw2

puts "Putting together and writing \"Open CASCADE\"..."

# define text
text2brep text2d OpenCASCADE -font Times-Roman -height 8 -aspect bold -composite off
prism text text2d 0 0 2
trotate    text 0 0 0 0 1 0 90
ttranslate text 24.75 -2 65

# cut operation
bcut bodytext body text
bop bodytext thread
bopfuse bottle

puts "Showing result..."

# display result
vdisplay bottle
vfit
vsetdispmode 1
vaspects -isoontriangulation 1
