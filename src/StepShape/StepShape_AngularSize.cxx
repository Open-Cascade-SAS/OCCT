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

#include <Standard_Type.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepShape_AngularSize.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepShape_AngularSize,StepShape_DimensionalSize)

//=======================================================================
//function : StepShape_AngularSize
//purpose  : 
//=======================================================================
StepShape_AngularSize::StepShape_AngularSize ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepShape_AngularSize::Init (const Handle(StepRepr_ShapeAspect) &aDimensionalSize_AppliesTo,
                                  const Handle(TCollection_HAsciiString) &aDimensionalSize_Name,
                                  const StepShape_AngleRelator aAngleSelection)
{
  StepShape_DimensionalSize::Init(aDimensionalSize_AppliesTo,
                                  aDimensionalSize_Name);

  theAngleSelection = aAngleSelection;
}

//=======================================================================
//function : AngleSelection
//purpose  : 
//=======================================================================

StepShape_AngleRelator StepShape_AngularSize::AngleSelection () const
{
  return theAngleSelection;
}

//=======================================================================
//function : SetAngleSelection
//purpose  : 
//=======================================================================

void StepShape_AngularSize::SetAngleSelection (const StepShape_AngleRelator aAngleSelection)
{
  theAngleSelection = aAngleSelection;
}
