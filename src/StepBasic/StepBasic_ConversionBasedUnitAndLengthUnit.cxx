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
#include <StepBasic_ConversionBasedUnitAndLengthUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_LengthUnit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ConversionBasedUnitAndLengthUnit,StepBasic_ConversionBasedUnit)

StepBasic_ConversionBasedUnitAndLengthUnit::StepBasic_ConversionBasedUnitAndLengthUnit ()  {}

void StepBasic_ConversionBasedUnitAndLengthUnit::Init(const Handle(StepBasic_DimensionalExponents)& aDimensions,
						      const Handle(TCollection_HAsciiString)& aName,
						      const Handle(StepBasic_MeasureWithUnit)& aConversionFactor)
{
  // --- ANDOR componant fields ---
  StepBasic_ConversionBasedUnit::Init(aDimensions, aName, aConversionFactor);
  
  // --- ANDOR componant fields ---
  lengthUnit = new StepBasic_LengthUnit();
  lengthUnit->Init(aDimensions);
}


void StepBasic_ConversionBasedUnitAndLengthUnit::SetLengthUnit(const Handle(StepBasic_LengthUnit)& aLengthUnit)
{
  lengthUnit = aLengthUnit;
}

Handle(StepBasic_LengthUnit) StepBasic_ConversionBasedUnitAndLengthUnit::LengthUnit() const
{
  return lengthUnit;
}

