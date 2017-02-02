# Copyright (c) 1999-2014 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

#
#  02/02/1996 : pbo : creation
#  25/10/1996 : pbo : add 2d view
#
#  rotation/panning/zoom with buttons
#

frame .move -relief groove -borderwidth 1 
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

frame .pick -borderwidth 1
label .pick.title -text " Pick "
button .pick.coords -text "Coords" -command {catch {puts [mpick]}}
button .pick.dist -text "Dist" -command {catch {puts [mdist]}}
button .pick.whatis -text "What is" -command {catch {puts [whatis .]}}
button .pick.erase -text "Erase" -command {catch {puts [erase .]}}
pack .pick.title
pack .pick.coords -pady 2 -padx 10
pack .pick.dist -pady 2 -padx 10
pack .pick.whatis -pady 2 -padx 10
pack .pick.erase -pady 2 -padx 10

set ShowExtCommands 0

proc ShowHideExtCommands {} {
    global ShowExtCommands
    if $ShowExtCommands {
        pack .move -pady 1 -padx 1 -side left
        pack .pick -pady 1 -padx 1
    } else {
        pack forget .move .pick
    }
}
