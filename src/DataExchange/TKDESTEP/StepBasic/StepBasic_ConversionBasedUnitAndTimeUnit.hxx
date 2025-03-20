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

#ifndef _StepBasic_ConversionBasedUnitAndTimeUnit_HeaderFile
#define _StepBasic_ConversionBasedUnitAndTimeUnit_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepBasic_ConversionBasedUnit.hxx>
class StepBasic_TimeUnit;
class StepBasic_DimensionalExponents;
class TCollection_HAsciiString;
class StepBasic_MeasureWithUnit;

class StepBasic_ConversionBasedUnitAndTimeUnit;
DEFINE_STANDARD_HANDLE(StepBasic_ConversionBasedUnitAndTimeUnit, StepBasic_ConversionBasedUnit)

class StepBasic_ConversionBasedUnitAndTimeUnit : public StepBasic_ConversionBasedUnit
{

public:
  //! Returns a ConversionBasedUnitAndTimeUnit
  Standard_EXPORT StepBasic_ConversionBasedUnitAndTimeUnit();

  Standard_EXPORT void Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
                            const Handle(TCollection_HAsciiString)&       aName,
                            const Handle(StepBasic_MeasureWithUnit)&      aConversionFactor);

  Standard_EXPORT void SetTimeUnit(const Handle(StepBasic_TimeUnit)& aTimeUnit);

  Standard_EXPORT Handle(StepBasic_TimeUnit) TimeUnit() const;

  DEFINE_STANDARD_RTTIEXT(StepBasic_ConversionBasedUnitAndTimeUnit, StepBasic_ConversionBasedUnit)

protected:
private:
  Handle(StepBasic_TimeUnit) timeUnit;
};

#endif // _StepBasic_ConversionBasedUnitAndTimeUnit_HeaderFile
