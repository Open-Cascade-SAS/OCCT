# Simple sample demonstrating work with assemblies in XDE, and assignment of 
# names and colors to components vs. instances

#Category: XDE
#Title: Work with assemblies, colors etc. in XDE

pload MODELING
pload OCAF
pload XDE

puts "Make a link as assembly made of a pin and two instances of the same nut"
pcylinder pin 1 10
pcylinder nut 2 2
compound nut nut nuts
explode nuts
ttranslate nuts_1 0 0 7
ttranslate nuts_2 0 0 1
compound pin nuts_1 nuts_2 link

puts "Add link assembly in XCAF document, and add names and colors:"
NewDocument D XCAF
XAddShape D link
SetName D [XFindShape D pin] "Pin"
SetName D [XFindShape D nut] "Nut"
SetName D [XFindShape D link] "Link"
SetName D [XFindShape D link]:1 "Pin instance"
SetName D [XFindShape D link]:2 "Nut instance 1"
SetName D [XFindShape D link]:3 "Nut instance 2"

puts "- Pin will be white"
XSetColor D [XFindShape D pin] 1 1 1
puts "- Nut itself will be dark gray"
XSetColor D [XFindShape D nut] 0.1 0.1 0.1
puts "- Nut instance #1 will be red"
XSetColor D [XFindShape D link]:2 1 0 0
puts "- Nut instance #2 will be green"
XSetColor D [XFindShape D link]:3 0 1 0

puts "Starting DF browser..."
DFBrowse D
puts "Expand the document tree to see its structure and assigned names"

puts "Showing assembly in 3d view..."
XShow D
vfit
vsetdispmode 1
