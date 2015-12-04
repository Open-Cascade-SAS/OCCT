// Created on: 2002-12-12
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

#include <Standard_Type.hxx>
#include <StepBasic_EulerAngles.hxx>
#include <StepElement_CurveElementSectionDefinition.hxx>
#include <StepFEA_CurveElementIntervalConstant.hxx>
#include <StepFEA_CurveElementLocation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_CurveElementIntervalConstant,StepFEA_CurveElementInterval)

//=======================================================================
//function : StepFEA_CurveElementIntervalConstant
//purpose  : 
//=======================================================================
StepFEA_CurveElementIntervalConstant::StepFEA_CurveElementIntervalConstant ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_CurveElementIntervalConstant::Init (const Handle(StepFEA_CurveElementLocation) &aCurveElementInterval_FinishPosition,
                                                 const Handle(StepBasic_EulerAngles) &aCurveElementInterval_EuAngles,
                                                 const Handle(StepElement_CurveElementSectionDefinition) &aSection)
{
  StepFEA_CurveElementInterval::Init(aCurveElementInterval_FinishPosition,
                                     aCurveElementInterval_EuAngles);

  theSection = aSection;
}

//=======================================================================
//function : Section
//purpose  : 
//=======================================================================

Handle(StepElement_CurveElementSectionDefinition) StepFEA_CurveElementIntervalConstant::Section () const
{
  return theSection;
}

//=======================================================================
//function : SetSection
//purpose  : 
//=======================================================================

void StepFEA_CurveElementIntervalConstant::SetSection (const Handle(StepElement_CurveElementSectionDefinition) &aSection)
{
  theSection = aSection;
}
