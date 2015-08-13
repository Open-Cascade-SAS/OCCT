puts "================"
puts "OCC21564"
puts "================"
puts ""
#######################################################################
# Intersection of two planar faces produces curve with too many poles
#######################################################################

puts "Construct horizontal plane, convert it to b-spline, and create a face on it"
plane p1 0 0 0 0 0 1
trim p1t p1 -10 10 -10 10
convert bs1 p1t
mkface f1 bs1

puts "Construct vertical plane and face on it"
plane p2 0 0 0 1 0 0 
mkface f2 p2 -10 10 -10 10

puts "\nBuild intersection of two faces"
bsection sec f1 f2

puts "Check number of points in resulting curve"
if { ! [regexp {([0-9]+) Poles} [dump sec] str nbp] } {
    puts "Error: Could not check number of poles in resulting curve!" 
} elseif { $nbp != 2 } {
    puts "Error: Intersection curve has too many poles ($nbp while 2 is expected)"
}

puts "\nBuild intersection with approximation of resulting curve"
bsection sec f1 f2 -a

puts "Check number of points in resulting curve"
if { ! [regexp {([0-9]+) Poles} [dump sec] str nbp] } {
    puts "Error: Could not check number of poles in resulting curve!" 
} elseif { $nbp != 2 } {
    puts "Error: Intersection curve has too many poles ($nbp while 2 is expected)"
}

puts "\nNow trying with inclined plane"
plane p2 0 0 0 1 2 0 
mkface f2 p2 -15 15 -15 15

puts "\nBuild intersection of two faces"
bsection sec f1 f2

puts "Check number of points in resulting curve"
if { ! [regexp {([0-9]+) Poles} [dump sec] str nbp] } {
    puts "Error: Could not check number of poles in resulting curve!" 
} elseif { $nbp != 2 } {
    puts "Error: Intersection curve has too many poles ($nbp while 2 is expected)"
}

puts "\nBuild intersection with approximation of resulting curve"
bsection sec f1 f2 -a

puts "Check number of points in resulting curve"
if { ! [regexp {([0-9]+) Poles} [dump sec] str nbp] } {
    puts "Error: Could not check number of poles in resulting curve!" 
} elseif { $nbp != 2 } {
    puts "Error: Intersection curve has too many poles ($nbp while 2 is expected)"
}
