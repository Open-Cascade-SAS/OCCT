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
