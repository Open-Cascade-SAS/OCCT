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
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SiUnitAndThermodynamicTemperatureUnit.hxx>
#include <StepBasic_ThermodynamicTemperatureUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_SiUnitAndThermodynamicTemperatureUnit,StepBasic_SiUnit)

//=======================================================================
//function : StepBasic_SiUnitAndThermodynamicTemperatureUnit
//purpose  : 
//=======================================================================
StepBasic_SiUnitAndThermodynamicTemperatureUnit::StepBasic_SiUnitAndThermodynamicTemperatureUnit ()
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndThermodynamicTemperatureUnit::Init(const Standard_Boolean hasAprefix,
                                                           const StepBasic_SiPrefix aPrefix,
                                                           const StepBasic_SiUnitName aName)
{
  // --- class inherited fields ---
  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
    
  // --- ANDOR componant fields ---
  thermodynamicTemperatureUnit = new StepBasic_ThermodynamicTemperatureUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  thermodynamicTemperatureUnit->Init(aDimensions);
}


//=======================================================================
//function : SetThermodynamicTemperatureUnit
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndThermodynamicTemperatureUnit::SetThermodynamicTemperatureUnit
  (const Handle(StepBasic_ThermodynamicTemperatureUnit)& aThermodynamicTemperatureUnit)
{
  thermodynamicTemperatureUnit = aThermodynamicTemperatureUnit;
}


//=======================================================================
//function : SolidAngleUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_ThermodynamicTemperatureUnit) StepBasic_SiUnitAndThermodynamicTemperatureUnit::ThermodynamicTemperatureUnit() const
{
  return thermodynamicTemperatureUnit;
}

