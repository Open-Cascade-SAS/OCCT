// Created on: 2000-08-11
// Created by: Andrey BETENEV
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

#ifndef XCAFPrs_DataMapOfShapeStyle_HeaderFile
#define XCAFPrs_DataMapOfShapeStyle_HeaderFile

#include <TopoDS_Shape.hxx>
#include <XCAFPrs_Style.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<TopoDS_Shape,XCAFPrs_Style,TopTools_ShapeMapHasher> XCAFPrs_DataMapOfShapeStyle;
typedef NCollection_DataMap<TopoDS_Shape,XCAFPrs_Style,TopTools_ShapeMapHasher>::Iterator XCAFPrs_DataMapIteratorOfDataMapOfShapeStyle;


#endif
