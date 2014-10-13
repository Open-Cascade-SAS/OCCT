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

if { [info commands addmenu] == "" } { return }

global theMenus
if [info exists theMenus(Curves)] {
  destroy [string trimright $theMenus(Curves) ".menu"]
  unset theMenus(Curves)
}

addmenu Curves "Line"    {
    dialbox line name l origin {0 0 0} direction {1 0 0}
}
addmenu Curves "Circle"  {
    dialbox circle name c center {0 0 0} normal {0 0 1} xdir {1 0 0} radius 1
}
addmenu Curves "Ellipse"  {
    dialbox ellipse name e center {0 0 0} normal {0 0 1} xdir {1 0 0} radii {1 0.5}
}
addmenu Curves "Hyperbola"  {
    dialbox hyperbola name h center {0 0 0} normal {0 0 1} xdir {1 0 0} radii {1 0.5}
}

addmenu Curves "Parabola"  {
    dialbox parabola name b center {0 0 0} normal {0 0 1} xdir {1 0 0} focus 1
}
redrawhelp
