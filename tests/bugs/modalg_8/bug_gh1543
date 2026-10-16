# This test checks the fix for GitHub issue #1543: a Boolean Cut with a helical
# swept tool that leaves the cylinder through its top face returned the cylinder
# untouched (IsDone, no error) for some exit angles. IntPatch lost the vertex at
# the face boundary when it decomposed the walking line at the seam, and the
# piece straddling the boundary was then classified by its midpoint and dropped.

puts "========"
puts "GH1543: Modeling Algorithms - helical cut silently removes nothing"
puts "========"
puts ""

# cylinder r = 8, z = 0..30
pcylinder c 8 30

# helix: 2D line u = t, v = t * pitch / (2 pi) on a cylindrical surface r = 8,
# pitch 2, 7.5 turns (height 15), as one edge with its 3D curve built
cylinder cs 0 0 0 0 0 1 8
2dline l 0 0 1 0.3183098861837907
mkedge e l cs 0 49.4536246421342
mkwire spine e
build3d spine

# profile: circle r = 0.6 at the spine start, normal along the spine tangent
circle pc 8 0 0 0 0.9528905 0.3033145 0.6
mkedge pe pc
mkwire prof pe

# sweep with a fixed +Z binormal, round-corner transitions, as a solid
mksweep spine
setsweep -CN 0 0 1
addsweep prof
buildsweep tool -R -S

# DZ = 16.2: the tool starts inside the cylinder and exits the top face at
# theta = 324 degrees from the seam. Before the fix nothing was removed here
# (volume stayed 6031.86); DZ = 16.0 worked.
ttranslate tool 0 0 16.2

bcut result c tool

checkshape result
checknbshapes result -solid 1
checkprops result -v 5841.80 -eps 1e-3
