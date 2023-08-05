// Created on: 1997-03-17
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef TNaming_MapOfNamedShape_HeaderFile
#define TNaming_MapOfNamedShape_HeaderFile

#include <TNaming_NamedShape.hxx>
#include <NCollection_Map.hxx>

typedef NCollection_Map<Handle(TNaming_NamedShape)> TNaming_MapOfNamedShape;
typedef NCollection_Map<Handle(TNaming_NamedShape)>::Iterator TNaming_MapIteratorOfMapOfNamedShape;


#endif
