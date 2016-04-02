# Sample demonstrating assignment of colors to faces in XDE

#Category: XDE
#Title: Assignment of colors to faces

pload MODELING VISUALIZATION OCAF XDE

box b 0 -20 -10 100 40 20
compound b b b a
explode a
trotate a_1 0 0 0 1 0 0 60
trotate a_2 0 0 0 1 0 0 -60
bcommon b a a_1
bcommon b b a_2

pcylinder c 4 100
trotate c 0 0 0 0 1 0 90

psphere s 1.4
ttranslate s 99.2 0 0
bfuse cx c s

pcone e 60 0.5 101
trotate e 0 0 0 0 1 0 90

bcommon body b e
bcut body body c
bcommon core cx e

text2brep text "CAD Assistant" -font Times -height 10
ttranslate text 10 -4 10
prism tr text 0 0 -1
bfuse body body tr

donly body core

#vdisplay body core
#vsetcolor body yellow
#vsetcolor core red

explode body so
explode body_1 f
explode core so

NewDocument D
XAddShape D body_1
XAddShape D core_1

#XSetColor D body_1   0. 0. 1.
for {set i 1} {$i <= 26} {incr i} {XSetColor D body_1_$i 0. 0. 1.}
XSetColor D body_1_1 0.9 0.5 0.4
XSetColor D body_1_9 0.9 0.5 0.4
for {set i 10} {$i <= 22} {incr i} {XSetColor D body_1_$i 0.6 0.7 0.0}
XSetColor D core_1 0.1 0.1 1.
foreach ff [explode core_1 f] { XSetColor D $ff 0.1 0.1 1. ; puts "set color $ff" }

XShow D

vfit
vsetdispmode 1
vrenderparams -msaa 8
vsetcolorbg 255 255 255

#param write.iges.brep.mode 1
#WriteIges D d:/pencil3.igs
