// Created on: 2003-08-21
// Created by: Sergey KUUL
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepRepr_ReprItemAndLengthMeasureWithUnit_HeaderFile
#define _StepRepr_ReprItemAndLengthMeasureWithUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>
class StepBasic_LengthMeasureWithUnit;

class StepRepr_ReprItemAndLengthMeasureWithUnit;
DEFINE_STANDARD_HANDLE(StepRepr_ReprItemAndLengthMeasureWithUnit,
                       StepRepr_ReprItemAndMeasureWithUnit)

class StepRepr_ReprItemAndLengthMeasureWithUnit : public StepRepr_ReprItemAndMeasureWithUnit
{

public:
  Standard_EXPORT StepRepr_ReprItemAndLengthMeasureWithUnit();

  Standard_EXPORT void SetLengthMeasureWithUnit(
    const Handle(StepBasic_LengthMeasureWithUnit)& aLMWU);

  Standard_EXPORT Handle(StepBasic_LengthMeasureWithUnit) GetLengthMeasureWithUnit() const;

  DEFINE_STANDARD_RTTIEXT(StepRepr_ReprItemAndLengthMeasureWithUnit,
                          StepRepr_ReprItemAndMeasureWithUnit)

private:
  Handle(StepBasic_LengthMeasureWithUnit) myLengthMeasureWithUnit;
};
#endif // _StepRepr_ReprItemAndLengthMeasureWithUnit_HeaderFile
