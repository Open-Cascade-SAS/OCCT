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
#include <StepBasic_SiUnitAndSolidAngleUnit.hxx>
#include <StepBasic_SolidAngleUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_SiUnitAndSolidAngleUnit,StepBasic_SiUnit)

StepBasic_SiUnitAndSolidAngleUnit::StepBasic_SiUnitAndSolidAngleUnit ()
{
}

void StepBasic_SiUnitAndSolidAngleUnit::Init(const Standard_Boolean hasAprefix,
					     const StepBasic_SiPrefix aPrefix,
					     const StepBasic_SiUnitName aName)
{
  // --- class inherited fields ---
  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
    
  // --- ANDOR componant fields ---
  solidAngleUnit = new StepBasic_SolidAngleUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  solidAngleUnit->Init(aDimensions);
}


void StepBasic_SiUnitAndSolidAngleUnit::SetSolidAngleUnit(const Handle(StepBasic_SolidAngleUnit)& aSolidAngleUnit)
{
  solidAngleUnit = aSolidAngleUnit;
}

Handle(StepBasic_SolidAngleUnit) StepBasic_SiUnitAndSolidAngleUnit::SolidAngleUnit() const
{
  return solidAngleUnit;
}

