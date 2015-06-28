// Created on: 1995-12-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef BRepCheck_DataMapOfShapeResult_HeaderFile
#define BRepCheck_DataMapOfShapeResult_HeaderFile

#include <TopoDS_Shape.hxx>
#include <BRepCheck_Result.hxx>
#include <TopTools_OrientedShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<TopoDS_Shape,Handle(BRepCheck_Result),TopTools_OrientedShapeMapHasher> BRepCheck_DataMapOfShapeResult;
typedef NCollection_DataMap<TopoDS_Shape,Handle(BRepCheck_Result),TopTools_OrientedShapeMapHasher>::Iterator BRepCheck_DataMapIteratorOfDataMapOfShapeResult;


#endif
