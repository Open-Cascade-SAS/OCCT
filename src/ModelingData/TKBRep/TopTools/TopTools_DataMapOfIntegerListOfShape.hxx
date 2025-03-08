// Created on: 1993-01-14
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef TopTools_DataMapOfIntegerListOfShape_HeaderFile
#define TopTools_DataMapOfIntegerListOfShape_HeaderFile

#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<Standard_Integer, TopTools_ListOfShape>
  TopTools_DataMapOfIntegerListOfShape;
typedef NCollection_DataMap<Standard_Integer, TopTools_ListOfShape>::Iterator
  TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape;

#endif
