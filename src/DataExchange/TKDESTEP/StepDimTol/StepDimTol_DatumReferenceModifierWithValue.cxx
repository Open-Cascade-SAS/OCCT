// Created on: 2015-07-16
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

#include <StepDimTol_DatumReferenceModifierWithValue.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepDimTol_DatumReferenceModifierWithValue, Standard_Transient)

//=================================================================================================

StepDimTol_DatumReferenceModifierWithValue::StepDimTol_DatumReferenceModifierWithValue() {}

//=================================================================================================

void StepDimTol_DatumReferenceModifierWithValue::Init(
  const StepDimTol_DatumReferenceModifierType&   theModifierType,
  const Handle(StepBasic_LengthMeasureWithUnit)& theModifierValue)
{
  myModifierType  = theModifierType;
  myModifierValue = theModifierValue;
}
