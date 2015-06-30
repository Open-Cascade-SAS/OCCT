// Created on: 2015-05-14
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _SelectMgr_IndexedMapOfOwner_HeaderFile
#define _SelectMgr_IndexedMapOfOwner_HeaderFile

#include <NCollection_IndexedMap.hxx>
#include <NCollection_Shared.hxx>

class SelectMgr_EntityOwner;

typedef NCollection_Shared< NCollection_IndexedMap<Handle(SelectMgr_EntityOwner)> > SelectMgr_IndexedMapOfOwner;

#endif // _SelectMgr_IndexedMapOfOwner_HeaderFile
