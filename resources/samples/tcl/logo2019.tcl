# The following example constructs a 3D object looking like new OCC logo on top view.
#
#Category: Modeling
#Title: OCC Logo 2019

pload MODELING VISUALIZATION XDE OCAF

# spheric body
psphere s 1
box b 0 0 -2 1.5 1.5 4
bcut sb s b

# toroidal handle
ptorus t 1 0.5
trotate t 0 0 0 0 0 1 -90
ttranslate t 1.5 1.5 0
bcut tb t b

pcylinder p1 0.5 0.5
trotate p1 0 0 0 0 0 1 180
trotate p1 0 0 0 1 0 0 90
ttranslate p1 0.5 1.5 0

pcylinder p2 0.5 0.5
trotate p2 0 0 0 0 0 1 180
trotate p2 0 0 0 1 0 0 90
trotate p2 0 0 0 0 0 1 90
ttranslate p2 1 0.5 0

bfuse tp tb p1
bfuse tp tp p2

# intermediate part

# - get surfaces and edges on half of spheric and toroidal parts for filling
box b -2 -2 -2 4 4 2
bcut sbh sb b
bcut tph tp b
unset b

explode sbh f
renamevar sbh_1 sbf
explode sbf e

explode tph f
renamevar tph_2 tpf1
renamevar tph_13 tpf2
explode tpf1 e
explode tpf2 e

# - make curved surface by plate
#plate r 0 4 tpf1_1 tpf1 1 tpf2_3 tpf2 1 sbf_2 sbf 1 sbf_4 sbf 1
approxplate r1 0 4 tpf1_1 tpf1 0 tpf2_1 tpf2 0 sbf_2 sbf 0 sbf_4 sbf 0 0.00001 100 3 0

# - make solid
tcopy r1 r2
tmirror r2 0 0 0 0 0 1
explode sb f
explode tp f
sewing rr 0.001 r1 r2 sb_2 sb_3 tp_2 tp_5
ssolid rr rs

# rotate all solids by 45 deg to have standard orientation of the logo on top view
trotate sb 0 0 0 0 0 1 -45
trotate tp 0 0 0 0 0 1 -45
trotate rs 0 0 0 0 0 1 -45

# create XDE document
catch {Close D}
XNewDoc D
set main [XNewShape D]
XAddComponent D $main sb
XAddComponent D $main tp
XAddComponent D $main rs
XUpdateAssemblies D
SetName D $main "OCC Logo 2019"
SetName D [XFindShape D sb] "Core"
SetName D [XFindShape D tp] "Loop"
SetName D [XFindShape D rs] "Connector"
XSetColor D sb FF3652
XSetColor D tp 00AADA
XSetColor D rs 0073B0

# display
vinit View1
vbackground -color WHITE
XDisplay -dispMode 1 D
vtop
vfit
