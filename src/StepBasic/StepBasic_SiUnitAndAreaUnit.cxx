// Created on: 1999-10-11
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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
#include <StepBasic_AreaUnit.hxx>
#include <StepBasic_DimensionalExponents.hxx>
#include <StepBasic_SiUnitAndAreaUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_SiUnitAndAreaUnit,StepBasic_SiUnit)

StepBasic_SiUnitAndAreaUnit::StepBasic_SiUnitAndAreaUnit () 
{
}

void StepBasic_SiUnitAndAreaUnit::SetAreaUnit(const Handle(StepBasic_AreaUnit)& anAreaUnit)
{
  areaUnit = anAreaUnit;
}

Handle(StepBasic_AreaUnit) StepBasic_SiUnitAndAreaUnit::AreaUnit() const
{
  return areaUnit;
}
