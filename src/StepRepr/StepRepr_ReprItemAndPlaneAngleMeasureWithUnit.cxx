// Created on: 2015-07-22
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


#include <Standard_Type.hxx>
#include <StepBasic_PlaneAngleMeasureWithUnit.hxx>
#include <StepRepr_ReprItemAndPlaneAngleMeasureWithUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit,StepRepr_ReprItemAndMeasureWithUnit)

//=======================================================================
//function : StepRepr_ReprItemAndPlaneAngleMeasureWithUnit
//purpose  : 
//=======================================================================
StepRepr_ReprItemAndPlaneAngleMeasureWithUnit::StepRepr_ReprItemAndPlaneAngleMeasureWithUnit() : StepRepr_ReprItemAndMeasureWithUnit()
{
  myPlaneAngleMeasureWithUnit = new StepBasic_PlaneAngleMeasureWithUnit();
}

//=======================================================================
//function : SetPlaneAngleMeasureWithUnit
//purpose  : 
//=======================================================================

void StepRepr_ReprItemAndPlaneAngleMeasureWithUnit::SetPlaneAngleMeasureWithUnit
  (const Handle(StepBasic_PlaneAngleMeasureWithUnit)& aLMWU) 
{
  myPlaneAngleMeasureWithUnit = aLMWU;
}


//=======================================================================
//function : GetPlaneAngleMeasureWithUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_PlaneAngleMeasureWithUnit) StepRepr_ReprItemAndPlaneAngleMeasureWithUnit::
       GetPlaneAngleMeasureWithUnit() const
{
  return myPlaneAngleMeasureWithUnit;
}
