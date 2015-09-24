// Created on: 2015-07-13
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StepRepr_Array1OfShapeAspect.hxx>

#include <Standard_RangeError.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_OutOfMemory.hxx>
 

#define Array1Item Handle(StepRepr_ShapeAspect)
#define Array1Item_hxx <StepRepr_ShapeAspect.hxx>
#define TCollection_Array1 StepRepr_Array1OfShapeAspect
#define TCollection_Array1_hxx <StepRepr_Array1OfShapeAspect.hxx>
#include <TCollection_Array1.gxx>

