// Created on: 2000-08-08
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef XCAFDoc_DataMapOfShapeLabel_HeaderFile
#define XCAFDoc_DataMapOfShapeLabel_HeaderFile

#include <TDF_Label.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>
  XCAFDoc_DataMapOfShapeLabel;
typedef NCollection_DataMap<TopoDS_Shape, TDF_Label, TopTools_ShapeMapHasher>::Iterator
  XCAFDoc_DataMapIteratorOfDataMapOfShapeLabel;

#endif
