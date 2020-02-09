# Copyright (c) 2020 OPEN CASCADE SAS
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

# List of toolkits 
proc TModelingData:toolkits { } {
  return [list TKShapeView]
}

# List of non-toolkits (resource units, executables etc., with associated info)
proc TModelingData:ressources { } {
}

# Module name 
proc TModelingData:name { } {
  return TModelingData
}

# And short alias
proc TModelingData:alias { } {
  return TModelingData
}

# Dependency on other products
proc TModelingData:depends { } {
  return [list FoundationClasses ModelingData TVisualization]
}

# Returns a list of exported features.
proc TModelingData:Export { } {
  return [list source runtime wokadm api]
}
