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
#include <StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI,StepRepr_ReprItemAndMeasureWithUnitAndQRI)

//=======================================================================
//function : StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI
//purpose  : 
//=======================================================================
StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI::StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI() : StepRepr_ReprItemAndMeasureWithUnitAndQRI()
{
  myLengthMeasureWithUnit = new StepBasic_LengthMeasureWithUnit();
}

//=======================================================================
//function : SetLengthMeasureWithUnit
//purpose  : 
//=======================================================================

void StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI::SetLengthMeasureWithUnit
  (const Handle(StepBasic_LengthMeasureWithUnit)& aLMWU) 
{
  myLengthMeasureWithUnit = aLMWU;
}


//=======================================================================
//function : GetLengthMeasureWithUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_LengthMeasureWithUnit) StepRepr_ReprItemAndLengthMeasureWithUnitAndQRI::
       GetLengthMeasureWithUnit() const
{
  return myLengthMeasureWithUnit;
}
