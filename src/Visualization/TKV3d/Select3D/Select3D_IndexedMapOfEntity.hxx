// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _Select3D_IndexedMapOfEntity_Header
#define _Select3D_IndexedMapOfEntity_Header

#include <NCollection_IndexedMap.hxx>

class Select3D_SensitiveEntity;
typedef NCollection_IndexedMap<Handle(Select3D_SensitiveEntity)> Select3D_IndexedMapOfEntity;

#endif // _Select3D_IndexedMapOfEntity_Header
