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

set Draw_GEOMETRY 1

if {[array names ::env CSF_OCCTResourcePath] != "" && "$::env(CSF_OCCTResourcePath)" != "" && [file exists $::env(CSF_OCCTResourcePath)/DrawResources/CURVES.tcl]} {
  source $env(CSF_OCCTResourcePath)/DrawResources/CURVES.tcl
} else {
  source $env(CASROOT)/src/DrawResources/CURVES.tcl
}

if {[array names ::env CSF_OCCTResourcePath] != "" && "$::env(CSF_OCCTResourcePath)" != "" && [file exists $::env(CSF_OCCTResourcePath)/DrawResources/SURFACES.tcl]} {
  source $env(CSF_OCCTResourcePath)/DrawResources/SURFACES.tcl
} else {
  source $env(CASROOT)/src/DrawResources/SURFACES.tcl
}

