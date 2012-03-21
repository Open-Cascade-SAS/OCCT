// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepFEA_CurveElementIntervalConstant.ixx>

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
