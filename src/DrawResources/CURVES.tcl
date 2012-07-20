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

if { [info commands addmenu] == "" } { return }

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
