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
#include <StepBasic_ConversionBasedUnitAndMassUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_MassUnit.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ConversionBasedUnitAndMassUnit, StepBasic_ConversionBasedUnit)

//=================================================================================================

StepBasic_ConversionBasedUnitAndMassUnit::StepBasic_ConversionBasedUnitAndMassUnit() {}

//=================================================================================================

void StepBasic_ConversionBasedUnitAndMassUnit::Init(
  const Handle(StepBasic_DimensionalExponents)& aDimensions,
  const Handle(TCollection_HAsciiString)&       aName,
  const Handle(StepBasic_MeasureWithUnit)&      aConversionFactor)
{
  // --- ANDOR component fields ---
  StepBasic_ConversionBasedUnit::Init(aDimensions, aName, aConversionFactor);

  // --- ANDOR component fields ---
  massUnit = new StepBasic_MassUnit();
  massUnit->Init(aDimensions);
}

//=================================================================================================

void StepBasic_ConversionBasedUnitAndMassUnit::SetMassUnit(
  const Handle(StepBasic_MassUnit)& aMassUnit)
{
  massUnit = aMassUnit;
}

//=================================================================================================

Handle(StepBasic_MassUnit) StepBasic_ConversionBasedUnitAndMassUnit::MassUnit() const
{
  return massUnit;
}
