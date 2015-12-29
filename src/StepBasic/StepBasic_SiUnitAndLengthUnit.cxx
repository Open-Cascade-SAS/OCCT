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
#include <StepBasic_LengthUnit.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SiUnitAndLengthUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_SiUnitAndLengthUnit,StepBasic_SiUnit)

StepBasic_SiUnitAndLengthUnit::StepBasic_SiUnitAndLengthUnit ()
{
}

void StepBasic_SiUnitAndLengthUnit::Init(const Standard_Boolean hasAprefix,
					 const StepBasic_SiPrefix aPrefix,
					 const StepBasic_SiUnitName aName)
{
  // --- classe inherited fields ---
  // --- ANDOR componant fields ---
  lengthUnit = new StepBasic_LengthUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  lengthUnit->Init(aDimensions);

  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
}


void StepBasic_SiUnitAndLengthUnit::SetLengthUnit(const Handle(StepBasic_LengthUnit)& aLengthUnit)
{
  lengthUnit = aLengthUnit;
}

Handle(StepBasic_LengthUnit) StepBasic_SiUnitAndLengthUnit::LengthUnit() const
{
  return lengthUnit;
}


