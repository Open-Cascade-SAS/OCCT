// Created on: 2002-12-14
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepFEA_FreedomAndCoefficient.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_FreedomAndCoefficient, Standard_Transient)

//=================================================================================================

StepFEA_FreedomAndCoefficient::StepFEA_FreedomAndCoefficient() {}

//=================================================================================================

void StepFEA_FreedomAndCoefficient::Init(const StepFEA_DegreeOfFreedom&               aFreedom,
                                         const StepElement_MeasureOrUnspecifiedValue& aA)
{

  theFreedom = aFreedom;

  theA = aA;
}

//=================================================================================================

StepFEA_DegreeOfFreedom StepFEA_FreedomAndCoefficient::Freedom() const
{
  return theFreedom;
}

//=================================================================================================

void StepFEA_FreedomAndCoefficient::SetFreedom(const StepFEA_DegreeOfFreedom& aFreedom)
{
  theFreedom = aFreedom;
}

//=================================================================================================

StepElement_MeasureOrUnspecifiedValue StepFEA_FreedomAndCoefficient::A() const
{
  return theA;
}

//=================================================================================================

void StepFEA_FreedomAndCoefficient::SetA(const StepElement_MeasureOrUnspecifiedValue& aA)
{
  theA = aA;
}
