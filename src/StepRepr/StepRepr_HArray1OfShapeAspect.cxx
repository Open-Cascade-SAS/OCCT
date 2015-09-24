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

#include <StepRepr_HArray1OfShapeAspect.hxx>

#include <Standard_Type.hxx>

#include <Standard_RangeError.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_OutOfMemory.hxx>
#include <StepRepr_Array1OfShapeAspect.hxx>
#include <StepRepr_ShapeAspect.hxx>

IMPLEMENT_STANDARD_TYPE(StepRepr_HArray1OfShapeAspect)
  IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
    STANDARD_TYPE(MMgt_TShared),
    STANDARD_TYPE(Standard_Transient),
  IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
IMPLEMENT_STANDARD_TYPE_END(StepRepr_HArray1OfShapeAspect)


IMPLEMENT_DOWNCAST(StepRepr_HArray1OfShapeAspect,Standard_Transient)
IMPLEMENT_STANDARD_RTTI(StepRepr_HArray1OfShapeAspect)

#define ItemHArray1 Handle(StepRepr_ShapeAspect)
#define ItemHArray1_hxx <StepRepr_ShapeAspect.hxx>
#define TheArray1 StepRepr_Array1OfShapeAspect
#define TheArray1_hxx <StepRepr_Array1OfShapeAspect.hxx>
#define TCollection_HArray1 StepRepr_HArray1OfShapeAspect
#define TCollection_HArray1_hxx <StepRepr_HArray1OfShapeAspect.hxx>
#define Handle_TCollection_HArray1 Handle_StepRepr_HArray1OfShapeAspect
#define TCollection_HArray1_Type_() StepRepr_HArray1OfShapeAspect_Type_()
#include <TCollection_HArray1.gxx>
