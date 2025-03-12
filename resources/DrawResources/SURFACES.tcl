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
if [info exists theMenus(Surfaces)] {
  destroy [string trimright $theMenus(Surfaces) ".menu"]
  unset theMenus(Surfaces)
}

proc dialanasurf {command sname args} {
    set com "dialbox $command name $sname origin {0 0 0} normal {0 0 1} xdir {1 0 0} "
    foreach l $args {append com " $l"}
    eval $com
}

addmenu Surfaces "Plane"      {dialanasurf plane p {}}
addmenu Surfaces "Cylinder"   {dialanasurf cylinder c {radius 1}}
addmenu Surfaces "Cone"       {dialanasurf cone c {angle 30 radius 0}}
addmenu Surfaces "Sphere"     {dialanasurf sphere s {radius 1}}
addmenu Surfaces "Torus"      {dialanasurf torus t {radii {1 0.8}}}
addmenu Surfaces "Revolution" {dialbox revsur name r basis . origin {0 0 0} axis {0 0 1}}
addmenu Surfaces "Extrusion"  {dialbox extsurf name e basis . direction {0 0 1}}
redrawhelp
