// Copyright (c) 2016 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _V3d_ListOfLight_HeaderFile
#define _V3d_ListOfLight_HeaderFile

#include <V3d_Light.hxx>

typedef NCollection_List<Handle(Graphic3d_CLight)> V3d_ListOfLight;
typedef V3d_ListOfLight::Iterator V3d_ListOfLightIterator;

#endif // _V3d_ListOfLight_HeaderFile
