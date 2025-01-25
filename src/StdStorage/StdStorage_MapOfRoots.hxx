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

#ifndef StdStorage_MapOfRoots_HeaderFile
#define StdStorage_MapOfRoots_HeaderFile

#include <TCollection_AsciiString.hxx>
#include <StdStorage_Root.hxx>
#include <NCollection_IndexedDataMap.hxx>

typedef NCollection_IndexedDataMap<TCollection_AsciiString, Handle(StdStorage_Root)>
  StdStorage_MapOfRoots;
typedef NCollection_IndexedDataMap<TCollection_AsciiString, Handle(StdStorage_Root)>::Iterator
  StdStorage_DataMapIteratorOfMapOfRoots;

#endif // StdStorage_MapOfRoots_HeaderFile
