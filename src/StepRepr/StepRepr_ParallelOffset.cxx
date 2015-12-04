// Created on: 2015-07-10
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

#include <StepRepr_ParallelOffset.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ParallelOffset,StepRepr_DerivedShapeAspect)

StepRepr_ParallelOffset::StepRepr_ParallelOffset ()    {  }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ParallelOffset::Init(
  const Handle(TCollection_HAsciiString)& theShapeAspect_Name,
  const Handle(TCollection_HAsciiString)& theShapeAspect_Description,
  const Handle(StepRepr_ProductDefinitionShape)& theShapeAspect_OfShape,
  const StepData_Logical theShapeAspect_ProductDefinitional,
  const Handle(StepBasic_MeasureWithUnit) &theOffset)
{
  StepRepr_ShapeAspect::Init(theShapeAspect_Name,
                             theShapeAspect_Description,
                             theShapeAspect_OfShape,
                             theShapeAspect_ProductDefinitional);

  offset = theOffset;
}
