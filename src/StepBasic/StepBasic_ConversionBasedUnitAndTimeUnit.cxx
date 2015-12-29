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
#include <StepBasic_ConversionBasedUnit.hxx>
#include <StepBasic_ConversionBasedUnitAndTimeUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepBasic_TimeUnit.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ConversionBasedUnitAndTimeUnit,StepBasic_ConversionBasedUnit)

StepBasic_ConversionBasedUnitAndTimeUnit::StepBasic_ConversionBasedUnitAndTimeUnit ()
{
}

void StepBasic_ConversionBasedUnitAndTimeUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
						    const Handle(TCollection_HAsciiString)& aName,
						    const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
  // --- ANDOR componant fields ---
  StepBasic_ConversionBasedUnit::Init(aDimensions, aName, aConversionFactor);
  
  // --- ANDOR componant fields ---
  timeUnit = new StepBasic_TimeUnit();
  timeUnit->Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndTimeUnit::SetTimeUnit(const Handle(StepBasic_TimeUnit)& aTimeUnit)
{
  timeUnit = aTimeUnit;
}

Handle(StepBasic_TimeUnit) StepBasic_ConversionBasedUnitAndTimeUnit::TimeUnit() const
{
  return timeUnit;
}

