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

#ifndef _StepRepr_ReprItemAndPlaneAngleMeasureWithUnit_HeaderFile
#define _StepRepr_ReprItemAndPlaneAngleMeasureWithUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>
class StepBasic_PlaneAngleMeasureWithUnit;

class StepRepr_ReprItemAndPlaneAngleMeasureWithUnit;
DEFINE_STANDARD_HANDLE(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit, StepRepr_ReprItemAndMeasureWithUnit)


class StepRepr_ReprItemAndPlaneAngleMeasureWithUnit : public StepRepr_ReprItemAndMeasureWithUnit
{

public:

  
  Standard_EXPORT StepRepr_ReprItemAndPlaneAngleMeasureWithUnit();
  
  Standard_EXPORT void SetPlaneAngleMeasureWithUnit (const Handle(StepBasic_PlaneAngleMeasureWithUnit)& aLMWU);
  
  Standard_EXPORT Handle(StepBasic_PlaneAngleMeasureWithUnit) GetPlaneAngleMeasureWithUnit() const;

  DEFINE_STANDARD_RTTIEXT(StepRepr_ReprItemAndPlaneAngleMeasureWithUnit,StepRepr_ReprItemAndMeasureWithUnit)

private:
  Handle(StepBasic_PlaneAngleMeasureWithUnit) myPlaneAngleMeasureWithUnit;
};
#endif // _StepRepr_ReprItemAndPlaneAngleMeasureWithUnit_HeaderFile
