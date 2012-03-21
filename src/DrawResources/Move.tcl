# Copyright (c) 1999-2012 OPEN CASCADE SAS
#
# The content of this file is subject to the Open CASCADE Technology Public
# License Version 6.5 (the "License"). You may not use the content of this file
# except in compliance with the License. Please obtain a copy of the License
# at http://www.opencascade.org and read it completely before using this file.
#
# The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
# main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
#
# The Original Code and all software distributed under the License is
# distributed on an "AS IS" basis, without warranty of any kind, and the
# Initial Developer hereby disclaims all such warranties, including without
# limitation, any warranties of merchantability, fitness for a particular
# purpose or non-infringement. Please see the License for the specific terms
# and conditions governing the rights and limitations under the License.

#
#  02/02/1996 : pbo : creation
#  25/10/1996 : pbo : add 2d view
#
#  rotation/panning/zoom with buttons
#

frame .move -relief groove -borderwidth 1 
pack .move -pady 1 -padx 1 -anchor w -side left
#toplevel .move

frame .move.rotate -borderwidth 1 
label .move.rotate.title -text " Rotation "
button .move.rotate.l   -text " < " -command {l ; repaint}
button .move.rotate.r   -text " > " -command {r ; repaint}
button .move.rotate.u   -text " ^ " -command {u ; repaint}
button .move.rotate.d   -text " v " -command {d ; repaint}
pack .move.rotate.title -side top
pack .move.rotate.l -side left
pack .move.rotate.r -side right
pack .move.rotate.u -side top
pack .move.rotate.d -side bottom
pack .move.rotate

frame .move.panning -borderwidth 1 
label .move.panning.title -text " Panning "
button .move.panning.l   -text " < " -command {pl ; 2dpl ; repaint}
button .move.panning.r   -text " > " -command {pr ; 2dpr ; repaint}
button .move.panning.u   -text " ^ " -command {pu ; 2dpu ; repaint}
button .move.panning.d   -text " v " -command {pd ; 2dpd ; repaint}
pack .move.panning.title -side top
pack .move.panning.l -side left
pack .move.panning.r -side right
pack .move.panning.u -side top
pack .move.panning.d -side bottom
pack .move.panning

frame .move.zoom -borderwidth 1 
label .move.zoom.title -text " Zoom "
button .move.zoom.mu   -text " + " -command {mu    ; 2dmu ; repaint}
button .move.zoom.md   -text " - " -command {md    ; 2dmd ; repaint}
button .move.zoom.fit  -text "max" -command {fit   ; 2dfit; repaint}
button .move.zoom.w    -text "win" -command {wzoom ;        repaint}
pack .move.zoom.title -side top
pack .move.zoom.w -side left
pack .move.zoom.fit -side right
pack .move.zoom.mu -side top
pack .move.zoom.md -side bottom
pack .move.zoom

