#########################################################################
# 26437: Visualization - Improve path tracing rendering engine
#########################################################################

pload ALL

# setup 3D viewer content
vinit name=View1 w=512 h=512
vglinfo

vvbo 0
vsetdispmode 1
vcamera -persp

box b 1 1 1 
explode b FACE 
vdisplay b_1 b_2 b_3 b_5 b_6

vfront
vfit

vsetmaterial b_1 plastic
vsetmaterial b_2 plastic
vsetmaterial b_3 plastic
vsetmaterial b_5 plastic
vsetmaterial b_6 plastic

vbsdf b_1 -kd 1 -ks 0
vbsdf b_2 -kd 1 -ks 0
vbsdf b_3 -kd 1 -ks 0
vbsdf b_5 -kd 1 -ks 0
vbsdf b_6 -kd 1 -ks 0

vbsdf b_2 -kd 0.3 0.5 1
vbsdf b_1 -kd 1 0.3 0.3

vsetlocation b_1  1  0  0
vsetlocation b_2 -1  0  0
vsetlocation b_5  0  0  1
vsetlocation b_6  0  0 -1
vsetlocation b_3  0  1  0

vlight del 0
vlight del 1
vlight add positional head 0 pos 0.5 0.5 0.85
vlight change 0 sm 0.06
vlight change 0 int 60.0

psphere s 0.2
vdisplay s
vsetlocation s 0.21 0.3 0.2
vsetmaterial s glass
vbsdf s -absorpcolor 0.8 0.8 1.0
vbsdf s -absorpcoeff 6

box c 0.3 0.3 0.2
vdisplay c
vsetlocation c 0.55 0.3 0.0
vlocrotate c 0 0 0 0 0 1 -30
vsetmaterial c plastic
vbsdf c -kd 1.0 0.8 0.2 -ks 0.3 -n

box g 0.15 0.15 0.3
vdisplay g
vsetlocation g 0.7 0.25 0.2
vlocrotate g 0 0 0 0 0 1 10
vsetmaterial g glass
vbsdf g -absorpcolor 0.8 1.0 0.8
vbsdf g -absorpcoeff 6

psphere r 0.1
vdisplay r
vsetmaterial r plastic
vbsdf r -kd 0.5 0.9 0.3 -ks 0.0 -kr 0.3 -n
vbsdf r -fresnel Constant 1.0
vsetlocation r 0.5 0.65 0.1

vrenderparams -ray -gi -rayDepth 8
