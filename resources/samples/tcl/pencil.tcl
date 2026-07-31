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

for {set i 1} {$i <= 26} {incr i} {XSetColor D body_1_$i BLUE}
XSetColor D body_1_1 E68066
XSetColor D body_1_9 E68066
for {set i 10} {$i <= 22} {incr i} {XSetColor D body_1_$i 99B300}
XSetColor D core_1 1A1AFF
foreach ff [explode core_1 f] { XSetColor D $ff 1A1AFF ; puts "set color $ff" }

vclear
vinit View1
XDisplay -dispMode 1 D -explore
vfit
vrenderparams -msaa 8
vbackground -color WHITE

#param write.iges.brep.mode 1
#WriteIges D d:/pencil3.igs
