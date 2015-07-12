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

#include <Standard_Transient.hxx>
#include <StepShape_DimensionalCharacteristic.hxx>
#include <StepShape_DimensionalLocation.hxx>
#include <StepShape_DimensionalSize.hxx>

//=======================================================================
//function : StepShape_DimensionalCharacteristic
//purpose  : 
//=======================================================================
StepShape_DimensionalCharacteristic::StepShape_DimensionalCharacteristic ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepShape_DimensionalCharacteristic::CaseNum (const Handle(Standard_Transient)& ent) const
{
  if (ent.IsNull()) return 0;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalLocation))) return 1;
  if (ent->IsKind(STANDARD_TYPE(StepShape_DimensionalSize))) return 2;
  return 0;
}

//=======================================================================
//function : DimensionalLocation
//purpose  : 
//=======================================================================

Handle(StepShape_DimensionalLocation) StepShape_DimensionalCharacteristic::DimensionalLocation () const
{
  return Handle(StepShape_DimensionalLocation)::DownCast(Value());
}

//=======================================================================
//function : DimensionalSize
//purpose  : 
//=======================================================================

Handle(StepShape_DimensionalSize) StepShape_DimensionalCharacteristic::DimensionalSize () const
{
  return Handle(StepShape_DimensionalSize)::DownCast(Value());
}
