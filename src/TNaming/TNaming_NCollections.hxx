// Created on: 2010-03-25
// Created by: Sergey ZARITCHNY
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#ifndef TNaming_NCollections_HeaderFile
#define TNaming_NCollections_HeaderFile
#include <NCollection_Map.hxx>   
#include <NCollection_DataMap.hxx>
typedef NCollection_Map<TopoDS_Shape> TNaming_MapOfShape; 
typedef TNaming_MapOfShape::Iterator TNaming_MapIteratorOfMapOfShape;
typedef NCollection_DataMap<TopoDS_Shape, TNaming_MapOfShape> TNaming_DataMapOfShapeMapOfShape; 
typedef TNaming_DataMapOfShapeMapOfShape::Iterator TNaming_DataMapIteratorOfDataMapOfShapeMapOfShape; 
//=======================================================================
//function : NCollection => IsEqual
//=======================================================================
Standard_Boolean IsEqual (const TopoDS_Shape& S1, const TopoDS_Shape& S2) 
{
  return S1.IsEqual(S2);
}

#endif
