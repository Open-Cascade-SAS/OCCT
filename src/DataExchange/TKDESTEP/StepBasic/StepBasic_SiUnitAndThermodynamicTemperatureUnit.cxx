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
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_SiUnitAndThermodynamicTemperatureUnit.hxx>
#include <StepBasic_ThermodynamicTemperatureUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_SiUnitAndThermodynamicTemperatureUnit, StepBasic_SiUnit)

//=================================================================================================

StepBasic_SiUnitAndThermodynamicTemperatureUnit::StepBasic_SiUnitAndThermodynamicTemperatureUnit()
{
}

//=================================================================================================

void StepBasic_SiUnitAndThermodynamicTemperatureUnit::Init(const bool     hasAprefix,
                                                           const StepBasic_SiPrefix   aPrefix,
                                                           const StepBasic_SiUnitName aName)
{
  // --- class inherited fields ---
  // --- ANDOR component fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);

  // --- ANDOR component fields ---
  thermodynamicTemperatureUnit = new StepBasic_ThermodynamicTemperatureUnit();
  occ::handle<StepBasic_DimensionalExponents> aDimensions;
  aDimensions.Nullify();
  thermodynamicTemperatureUnit->Init(aDimensions);
}

//=================================================================================================

void StepBasic_SiUnitAndThermodynamicTemperatureUnit::SetThermodynamicTemperatureUnit(
  const occ::handle<StepBasic_ThermodynamicTemperatureUnit>& aThermodynamicTemperatureUnit)
{
  thermodynamicTemperatureUnit = aThermodynamicTemperatureUnit;
}

//=================================================================================================

occ::handle<StepBasic_ThermodynamicTemperatureUnit> StepBasic_SiUnitAndThermodynamicTemperatureUnit::
  ThermodynamicTemperatureUnit() const
{
  return thermodynamicTemperatureUnit;
}
