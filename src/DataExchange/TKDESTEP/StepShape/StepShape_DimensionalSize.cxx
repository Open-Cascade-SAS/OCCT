// Created on: 2000-04-18
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepRepr_ShapeAspect.hxx>
#include <StepShape_DimensionalSize.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_DimensionalSize, Standard_Transient)

//=================================================================================================

StepShape_DimensionalSize::StepShape_DimensionalSize() {}

//=================================================================================================

void StepShape_DimensionalSize::Init(const Handle(StepRepr_ShapeAspect)&     aAppliesTo,
                                     const Handle(TCollection_HAsciiString)& aName)
{

  theAppliesTo = aAppliesTo;

  theName = aName;
}

//=================================================================================================

Handle(StepRepr_ShapeAspect) StepShape_DimensionalSize::AppliesTo() const
{
  return theAppliesTo;
}

//=================================================================================================

void StepShape_DimensionalSize::SetAppliesTo(const Handle(StepRepr_ShapeAspect)& aAppliesTo)
{
  theAppliesTo = aAppliesTo;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepShape_DimensionalSize::Name() const
{
  return theName;
}

//=================================================================================================

void StepShape_DimensionalSize::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}
