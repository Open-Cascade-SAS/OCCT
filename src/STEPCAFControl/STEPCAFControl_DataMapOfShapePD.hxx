// Created on: 2000-04-09
// Created by: Sergey MOZOKHIN
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

#ifndef STEPCAFControl_DataMapOfShapePD_HeaderFile
#define STEPCAFControl_DataMapOfShapePD_HeaderFile

#include <TopoDS_Shape.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<TopoDS_Shape,
                            Handle(StepBasic_ProductDefinition),
                            TopTools_ShapeMapHasher>
  STEPCAFControl_DataMapOfShapePD;
typedef NCollection_DataMap<TopoDS_Shape,
                            Handle(StepBasic_ProductDefinition),
                            TopTools_ShapeMapHasher>::Iterator
  STEPCAFControl_DataMapIteratorOfDataMapOfShapePD;

#endif
