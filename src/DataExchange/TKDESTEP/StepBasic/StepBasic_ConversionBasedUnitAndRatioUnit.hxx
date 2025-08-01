// Created on: 1994-06-17
// Created by: EXPRESS->CDL V0.2 Translator
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _StepBasic_ConversionBasedUnitAndRatioUnit_HeaderFile
#define _StepBasic_ConversionBasedUnitAndRatioUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ConversionBasedUnit.hxx>
class StepBasic_RatioUnit;
class StepBasic_DimensionalExponents;
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;

class StepBasic_ConversionBasedUnitAndRatioUnit;
DEFINE_STANDARD_HANDLE(StepBasic_ConversionBasedUnitAndRatioUnit, StepBasic_ConversionBasedUnit)

class StepBasic_ConversionBasedUnitAndRatioUnit : public StepBasic_ConversionBasedUnit
{

public:
  //! Returns a ConversionBasedUnitAndRatioUnit
  Standard_EXPORT StepBasic_ConversionBasedUnitAndRatioUnit();

  Standard_EXPORT void Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
                            const Handle(TCollection_HAsciiString)&       aName,
                            const Handle(Standard_Transient)&             aConversionFactor);

  Standard_EXPORT void SetRatioUnit(const Handle(StepBasic_RatioUnit)& aRatioUnit);

  Standard_EXPORT Handle(StepBasic_RatioUnit) RatioUnit() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_ConversionBasedUnitAndRatioUnit, StepBasic_ConversionBasedUnit)

protected:
private:
  Handle(StepBasic_RatioUnit) ratioUnit;
};

#endif // _StepBasic_ConversionBasedUnitAndRatioUnit_HeaderFile
