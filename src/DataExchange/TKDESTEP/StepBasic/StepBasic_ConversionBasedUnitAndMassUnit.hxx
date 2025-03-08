// Created on: 2003-02-10
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

#ifndef _StepBasic_ConversionBasedUnitAndMassUnit_HeaderFile
#define _StepBasic_ConversionBasedUnitAndMassUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ConversionBasedUnit.hxx>
class StepBasic_MassUnit;
class StepBasic_DimensionalExponents;
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;

class StepBasic_ConversionBasedUnitAndMassUnit;
DEFINE_STANDARD_HANDLE(StepBasic_ConversionBasedUnitAndMassUnit, StepBasic_ConversionBasedUnit)

class StepBasic_ConversionBasedUnitAndMassUnit : public StepBasic_ConversionBasedUnit
{

public:
  //! Returns a ConversionBasedUnitAndLengthUnit
  Standard_EXPORT StepBasic_ConversionBasedUnitAndMassUnit();

  Standard_EXPORT void Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
                            const Handle(TCollection_HAsciiString)&       aName,
                            const Handle(StepBasic_MeasureWithUnit)&      aConversionFactor);

  Standard_EXPORT void SetMassUnit(const Handle(StepBasic_MassUnit)& aMassUnit);

  Standard_EXPORT Handle(StepBasic_MassUnit) MassUnit() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_ConversionBasedUnitAndMassUnit, StepBasic_ConversionBasedUnit)

protected:
private:
  Handle(StepBasic_MassUnit) massUnit;
};

#endif // _StepBasic_ConversionBasedUnitAndMassUnit_HeaderFile
