// Created on: 2002-04-24
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Purpose:     The DataMap is a Map to store keys with associated
//              Items. See Map  from NCollection for  a discussion
//              about the number of buckets.
//              The DataMap can be seen as an extended array where
//              the Keys  are the   indices.  For this reason  the
//              operator () is defined on DataMap to fetch an Item
//              from a Key. So the following syntax can be used :
//              anItem = aMap(aKey);
//              aMap(aKey) = anItem;
//              This analogy has its  limit.   aMap(aKey) = anItem
//              can  be done only  if aKey was previously bound to
//              an item in the map.

#ifndef NCollection_DefineDataMap_HeaderFile
#define NCollection_DefineDataMap_HeaderFile

#include <NCollection_DataMap.hxx>

// *********************************************** Class DataMap *************

#define DEFINE_DATAMAP(_ClassName_, _BaseCollection_, TheKeyType, TheItemType) \
typedef NCollection_DataMap<TheKeyType, TheItemType > _ClassName_;

#endif
