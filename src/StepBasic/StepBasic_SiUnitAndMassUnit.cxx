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
#include <StepBasic_MassUnit.hxx>
#include <StepBasic_SiUnit.hxx>
#include <StepBasic_SiUnitAndMassUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_SiUnitAndMassUnit,StepBasic_SiUnit)

//=======================================================================
//function : StepBasic_SiUnitAndLengthUnit
//purpose  : 
//=======================================================================
StepBasic_SiUnitAndMassUnit::StepBasic_SiUnitAndMassUnit ()
{
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndMassUnit::Init(const Standard_Boolean hasAprefix,
                                       const StepBasic_SiPrefix aPrefix,
                                       const StepBasic_SiUnitName aName)
{
  // --- classe inherited fields ---
  // --- ANDOR componant fields ---
  massUnit = new StepBasic_MassUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  massUnit->Init(aDimensions);

  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);
}


//=======================================================================
//function : SetMassUnit
//purpose  : 
//=======================================================================

void StepBasic_SiUnitAndMassUnit::SetMassUnit(const Handle(StepBasic_MassUnit)& aMassUnit)
{
  massUnit = aMassUnit;
}


//=======================================================================
//function : MassUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_MassUnit) StepBasic_SiUnitAndMassUnit::MassUnit() const
{
  return massUnit;
}


