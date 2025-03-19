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

#include <Standard_Type.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepRepr_ReprItemAndLengthMeasureWithUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ReprItemAndLengthMeasureWithUnit,
                           StepRepr_ReprItemAndMeasureWithUnit)

//=================================================================================================

StepRepr_ReprItemAndLengthMeasureWithUnit::StepRepr_ReprItemAndLengthMeasureWithUnit()
    : StepRepr_ReprItemAndMeasureWithUnit()
{
  myLengthMeasureWithUnit = new StepBasic_LengthMeasureWithUnit();
}

//=================================================================================================

void StepRepr_ReprItemAndLengthMeasureWithUnit::SetLengthMeasureWithUnit(
  const Handle(StepBasic_LengthMeasureWithUnit)& aLMWU)
{
  myLengthMeasureWithUnit = aLMWU;
}

//=================================================================================================

Handle(StepBasic_LengthMeasureWithUnit) StepRepr_ReprItemAndLengthMeasureWithUnit::
  GetLengthMeasureWithUnit() const
{
  return myLengthMeasureWithUnit;
}
