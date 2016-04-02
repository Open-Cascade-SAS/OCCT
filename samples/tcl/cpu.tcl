# Sample model of Intel i7-4790 CPU
# Dimensions are taken from specs and foto found in Internet

#Category: XDE
#Title: Intel i7-4790 CPU

pload MODELING VISUALIZATION XDE

puts "Making board..."

# board is rectangle 37.5 x 37.5 mm with semi-round holes on two sides;
# assumed board thickness is 1 mm
dset L 37.5
dset t 1.
polyline pboard 0 0 0 L 0 0 L L 0 0 L 0 0 0 0 
mkplane fboard pboard
circle cslot -0.2 32.2 0 1
mkedge eslot cslot
wire wslot eslot
mkplane fslot wslot
bcut fboard fboard fslot
ttranslate fslot L+0.4 0 0
bcut fboard fboard fslot
prism board fboard 0 0 t

# make faces representing dard-green parts of the board sides
mkoffset dgbot fboard 1 -0.5
mkplane dgbot dgbot_1
tcopy dgbot dgtop
ttranslate dgtop 0 0 0.5*t
box aux 36.6 17.7 0 10 8.4 2*t
bcut dgtop dgtop aux
prism pbot dgbot 0 0 0.5*t
prism ptop dgtop 0 0 0.5*t
bfuse board board pbot
bfuse board board ptop

# add triangular faces indicating base corner of the plate
polyline btri 0.5 0.5 0 1.5 0.5 0 0.5 1.5 0 0.5 0.5 0
polyline ttri 0.5 0.5 t 2.5 0.5 t 0.5 2.5 t 0.5 0.5 t
thrusections stri 1 1 btri ttri
bfuse board board stri

explode board so
renamevar board_1 board

puts "Making case..."

# case is made of two filleted prisms, base and top
polyline lbase 3.4 1.8 t L-3.4 1.8 t L-3.4 11.4 t L-1.8 11.4 t L-1.8 25.2 t \
               L-3.4 25.2 t L-3.4 L-3 t 3.4 L-3 t 3.4 25.2 t \
               1.8 25.2 t 1.8 11.4 t 3.4 11.4 t 3.4 1.8 t
mkplane f lbase
explode f e
chfi2d fbase f f_1 f_2 F 1.3 f_2 f_3 F 0.7 f_3 f_4 F 0.7 f_4 f_5 F 0.7 \
               f_5 f_6 F 0.7 f_6 f_7 F 1.3 f_7 f_8 F 1.3 f_8 f_9 F 0.7 \
               f_9 f_10 F 0.7 f_10 f_11 F 0.7 f_11 f_12 F 0.7 f_12 f_1 F 1.3

polyline ltop 4. 3.4 t L-4 3.4 t L-4 L-4.8 t 4 L-4.8 t 4. 3.4 t
mkplane f ltop
explode f e
chfi2d ftop f f_1 f_2 F 1.6 f_2 f_3 F 1.6 f_3 f_4 F 1.6 f_4 f_1 F 1.6

# make case, assume height of base 1 mm and top additional 2.5 mm
prism pbase fbase 0 0 1
prism ptop ftop 0 0 3.5
bfuse case pbase ptop

explode case so
renamevar case_1 case

# write text on top of the case
# note that font is chosen by availability of Unicode symbols,
# it is different from actual font found on processor
set font "Arial Unicode MS"
#set text "i\u24c2\u00a911\nINTEL\u00ae CORE\u2122 i7-4790\nSR1QF 3.60GHZ\nMALAY\nL411B540 \u24d4"
#text2brep title $text "Arial Unicode MS" 1.7 x=10 y=24 z=4.51
# alternative variant to work-around issue #25852
set text "i\u20dd\u20dd11\nINTEL\u20dd CORE\u2122 i7-4790\nSR1QF 3.60GHZ\nMALAY\nL411B540 \u20dd"
text2brep title0 $text -font $font -height 1.7 -pos 10 24 4.51 -valign topfirstline
text2brep title1 "    M    C" -font $font -height 0.77 -pos 10 24.2 4.51
text2brep title2 "R" -font $font -height 0.77 -pos 15.3 21.9 4.51
text2brep title3 "e4" -font $font -height 0.7 -pos 18.6 15.1 4.51
compound title0 title1 title2 title3 title

puts "Adding contact pads..."

# contact pads on top side (round); we need 42 with R=0.3 and 1 with R=0.6
pcylinder rpad 0.27 0.1
eval compound [lrepeat 42 rpad] cpad
set lpad [explode cpad]
for {set i 1} {$i <= 20} {incr i} {
  ttranslate cpad_[expr 2*$i  ] [expr 4.5  + $i * 0.7] L-0.7 t
  ttranslate cpad_[expr 2*$i-1] [expr 4.85 + $i * 0.7] L-1.3 t
}
ttranslate cpad_41 L-0.7 L-0.7 t
ttranslate cpad_42 L-0.7   0.7 t
pcylinder Rpad 0.5 0.1
ttranslate Rpad 0.9 L-0.9 t
eval compound $lpad Rpad rpads

# contact pads at the bottom
box pad -0.45 -0.27 -0.1 0.9 0.54 0.1
trotate pad 0 0 0 0 0 1 60
ellipse c 0 0 -0.1 0.5 0.4
mkedge e c
wire w e
mkplane f w
prism b f 0 0 0.1
bcommon bpad pad b
explode bpad so
renamevar bpad_1 bpad
#donly bpad; boundings bpad

# pattern of presence of bottom pads, on XY plane (top view)
set pattern [join {
..ooooooooooooooo...ooooooooooooooooo... 
.oooooooooooooooo...ooooooooooooooooooo. 
.oooooooooooooooooooooooooooooooooooooo. 
.oooooooooooooooooooooooooooooooooooooo. 
.oooooooooooooooooooooooooooooooooooooo. 
.oooooooooooooooooooooooooooooooooooooo. 
.oooooooooooooooooooooooooooooooooooooo. 
.oooooooooooooooooooooooooooooooooooooo. 
.ooooooooooo................ooooooooooo. 
.oooooooooo.................ooooooooooo. 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
..oooooooooo................oooooooooo.. 
..oooooooooo................oooooooooo.. 
..oooooooooo................oooooooooo.. 
..oooooooooo................oooooooooo.. 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
oooooooooooo................oooooooooooo 
.ooooooooooo.................ooooooooooo 
.ooooooooooo................oooooooooooo 
.ooooooooooooooooooooooooooooooooooooooo 
oooooooooooooooooooooooooooooooooooooooo 
oooooooooooooooooooooooooooooooooooooooo 
oooooooooooooooooooooooooooooooooooooooo 
oooooooooooooooooooooooooooooooooooooooo 
.ooooooooooooooooooooooooooooooooooooooo 
..oooooooooooooooo...oooooooooooooooooo. 
...ooooooooooooooo...ooooooooooooooooo.. 
} ""]                                 

set nbpads 0
for {set i 0} {$i < 1600} {incr i} { 
  if { [string index $pattern $i] == "o" } { incr nbpads }
}
eval compound [lrepeat $nbpads bpad] cpad
set lpad [explode cpad]
for {set ipad 1; set iplace 0} {$ipad <= $nbpads && $iplace < 1600} {incr ipad; incr iplace} {
  while { [string index $pattern $iplace] == "." } { incr iplace }
  set icol [expr $iplace % 40]
  set irow [expr $iplace / 40]
  ttranslate cpad_$ipad [expr 1 + 0.91 * $icol] [expr [dval L] - 1 - 0.91 * $irow] 0
}

# round and square contact pads on top side
# note re-use of rpad object used for bootom round pads
eval compound [lrepeat 8 rpad] crpads
set lrpad [explode crpads]
ttranslate crpads_1 25.3  8.4  -0.1
ttranslate crpads_2 12.2 29.2  -0.1
ttranslate crpads_3 12.5 15.   -0.1
ttranslate crpads_4 12.5 18.75 -0.1
ttranslate crpads_5 12.5 19.5  -0.1
ttranslate crpads_6 12.5 20.25 -0.1
ttranslate crpads_7 12.5 21.   -0.1
ttranslate crpads_8 12.5 22.5  -0.1
box spad_1 12.21 13.75 -0.1 0.58 0.58 0.1
box spad_2 12.21 23.2  -0.1 0.58 0.58 0.1

# final compound for all bottom pads
eval compound $lpad $lrpad spad_1 spad_2 bpads

# resistor-like packages at the bottom
box rpk1 -0.6 -0.25 -0.5 0.3 0.5 0.5
box rpk2 -0.3 -0.25 -0.5 0.6 0.5 0.5
box rpk3  0.3 -0.25 -0.5 0.3 0.5 0.5
compound rpk1 rpk2 rpk3 rpk

eval compound [lrepeat 47 rpk] crpk
set lrpk [explode crpk]
# rotate first 26 packages vertically
for {set i 1} {$i <= 26} {incr i} {trotate crpk_$i 0 0 0 0 0 1 90}
# first 9 are vertical column on the right side of the bottom view
for {set i 1} {$i <=  9} {incr i} {
  ttranslate crpk_$i 13.4 [expr 9.8 + 1.6 * $i] 0 
}
# next 8 are 2x4 grid in top left corner
for {set i 1} {$i <=  4} {incr i} {
  ttranslate crpk_[expr  9 + $i] 23. [expr 21.5 + 1.6 * $i] 0 
  ttranslate crpk_[expr 13 + $i] 24. [expr 21.5 + 1.6 * $i] 0 
}
# others are translated individually, vertical first, bottom to top
ttranslate crpk_18 21.5  9.4 0
ttranslate crpk_19 21.5 11.0 0
ttranslate crpk_20 21.5 12.6 0
ttranslate crpk_21 22.5  9.8 0
ttranslate crpk_22 20.0 12.2 0
ttranslate crpk_23 24.0 13.6 0
ttranslate crpk_24 24.0 15.2 0
ttranslate crpk_25 19.5 16.0 0
ttranslate crpk_26 20.5 16.0 0
# now horizontal, bottom to top
ttranslate crpk_27 23.7  9.5 0
ttranslate crpk_28 23.7 10.5 0
ttranslate crpk_29 22.8 11.5 0
ttranslate crpk_30 22.8 12.5 0
ttranslate crpk_31 22.7 14.3 0
ttranslate crpk_32 22.7 16.0 0
ttranslate crpk_33 22.8 17.0 0
ttranslate crpk_34 22.8 19.1 0
ttranslate crpk_35 22.7 20.0 0
ttranslate crpk_36 23.0 20.9 0
ttranslate crpk_37 23.3 21.8 0

ttranslate crpk_38 19.8 21.6 0
ttranslate crpk_39 19.8 22.6 0
ttranslate crpk_40 19.8 23.6 0
ttranslate crpk_41 21.6 22.2 0
ttranslate crpk_42 21.6 23.2 0
ttranslate crpk_43 21.6 24.2 0

ttranslate crpk_44 18.0 24.6 0
ttranslate crpk_45 18.0 25.6 0
ttranslate crpk_46 18.0 26.6 0
ttranslate crpk_47 18.0 27.6 0

eval compound $lrpk brpk

# show result in plain 3d viewer
if [info exists i7_show_3dview] {
  vinit Driver1/Viewer1/View1
  vclear 
  vsetdispmode 1
  vrenderparams -msaa 8

  vlight clear
  vlight add amb
  vlight add directional direction 1 -1 -2 head 1

  vdisplay case
  vsetcolor case 0.7 0.7 0.7

  vdisplay title
  vsetcolor title 0.1 0.1 0.1

  # board is mostly yellow (edges, triangle markers)
  foreach f [explode board f] { vdisplay $f; vsetcolor $f 0.7 0.5 0.3 }
  # top and bottom faces are light-green (outside) and dark-green (inside)
  vsetcolor board_4 0 0.6 0.55
  vsetcolor board_5 0 0.6 0.55
  vsetcolor board_12 0 0.3 0.33
  vsetcolor board_14 0 0.3 0.33

  vdisplay rpads
  vsetcolor rpads 0.7 0.6 0.4

  vdisplay bpads
  vsetcolor bpads 0.7 0.6 0.4

  vdisplay brpk
  vsetcolor brpk 0.5 0.4 0.3

  donly board case rpads brpk; fit
}

# make XDE document
catch {Close D}
pload OCAF XDE

NewDocument D MDTV-XCAF

SetName D [XAddShape D board 0] "Board"
foreach f [explode board f] { XSetColor D $f 0.7 0.5 0.3 }
XSetColor D board_4 0 0.6 0.55
XSetColor D board_5 0 0.6 0.55
XSetColor D board_12 0 0.3 0.33
XSetColor D board_14 0 0.3 0.33

SetName D [XAddShape D case 0] "Case"
XSetColor D case 0.7 0.7 0.7

SetName D [XAddShape D title 0] "Case title"
XSetColor D title 0.1 0.1 0.1

SetName D [XAddShape D rpads 1] "Top side contact pads"
SetName D [XAddShape D bpads 1] "Bottom contact pads"
SetName D [XFindShape D bpad] "Contact pad"
SetName D [XFindShape D rpad] "Round pad"
SetName D [XFindShape D Rpad] "Big round pad"
SetName D [XFindShape D spad_1] "Square pad 1"
SetName D [XFindShape D spad_2] "Square pad 2"
XSetColor D rpad 0.7 0.6 0.4
XSetColor D Rpad 0.7 0.6 0.4
XSetColor D bpad 0.7 0.6 0.4
XSetColor D spad_1 0.7 0.6 0.4
XSetColor D spad_2 0.7 0.6 0.4

SetName D [XAddShape D brpk 1] "Bottom packages"
SetName D [XFindShape D rpk] "Bottom package"
XSetColor D rpk1 0.7 0.7 0.7
XSetColor D rpk2 0.5 0.4 0.3
XSetColor D rpk3 0.7 0.7 0.7

XShow D
vlight clear
vlight add amb
vlight add directional direction 1 -1 -2 head 1
vsetdispmode 1
vrenderparams -msaa 8
vfit

# save to STEP if variable i7_save_xde is defined (specifies file name)
if [info exists i7_save_xde] {
  param write.surfacecurve.mode 0
  WriteStep D $i7_save_xde
}
