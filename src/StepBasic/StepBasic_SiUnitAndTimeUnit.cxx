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
#include <StepBasic_SiUnitAndTimeUnit.hxx>
#include <StepBasic_TimeUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_SiUnitAndTimeUnit,StepBasic_SiUnit)

StepBasic_SiUnitAndTimeUnit::StepBasic_SiUnitAndTimeUnit ()  
{
}

void StepBasic_SiUnitAndTimeUnit::Init(const Standard_Boolean hasAprefix,
				       const StepBasic_SiPrefix aPrefix,
				       const StepBasic_SiUnitName aName)
{
  // --- class inherited fields ---
  // --- ANDOR componant fields ---
  StepBasic_SiUnit::Init(hasAprefix, aPrefix, aName);

  // --- ANDOR componant fields ---
  timeUnit = new StepBasic_TimeUnit();
  Handle(StepBasic_DimensionalExponents) aDimensions;
  aDimensions.Nullify();
  timeUnit->Init(aDimensions);
}


void StepBasic_SiUnitAndTimeUnit::SetTimeUnit(const Handle(StepBasic_TimeUnit)& aTimeUnit)
{
  timeUnit = aTimeUnit;
}

Handle(StepBasic_TimeUnit) StepBasic_SiUnitAndTimeUnit::TimeUnit() const
{
  return timeUnit;
}


