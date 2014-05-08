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

// Purpose:     An indexed map is used to  store  keys and to bind
//              an index to them.  Each new key stored in  the map
//              gets an index.  Index are incremented  as keys are
//              stored in the map. A key can be found by the index
//              and an index by the  key. No key  but the last can
//              be removed so the indices are in the range 1..Extent.
//              See  the  class   Map   from NCollection   for   a
//              discussion about the number of buckets.

#ifndef NCollection_DefineIndexedMap_HeaderFile
#define NCollection_DefineIndexedMap_HeaderFile

#include <NCollection_IndexedMap.hxx>

// *********************************************** Class IndexedMap ***********

#define DEFINE_INDEXEDMAP(_ClassName_, _BaseCollection_, TheKeyType)           \
typedef NCollection_IndexedMap <TheKeyType > _ClassName_;

#endif
