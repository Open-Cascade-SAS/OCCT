// Created on: 2003-01-22
// Created by: data exchange team
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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

#include <StepFEA_CurveElementIntervalLinearlyVarying.ixx>

//=======================================================================
//function : StepFEA_CurveElementIntervalLinearlyVarying
//purpose  : 
//=======================================================================

StepFEA_CurveElementIntervalLinearlyVarying::StepFEA_CurveElementIntervalLinearlyVarying ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_CurveElementIntervalLinearlyVarying::Init (const Handle(StepFEA_CurveElementLocation) &aCurveElementInterval_FinishPosition,
                                                        const Handle(StepBasic_EulerAngles) &aCurveElementInterval_EuAngles,
                                                        const Handle(StepElement_HArray1OfCurveElementSectionDefinition) &aSections)
{
  StepFEA_CurveElementInterval::Init(aCurveElementInterval_FinishPosition,
                                     aCurveElementInterval_EuAngles);

  theSections = aSections;
}

//=======================================================================
//function : Sections
//purpose  : 
//=======================================================================

Handle(StepElement_HArray1OfCurveElementSectionDefinition) StepFEA_CurveElementIntervalLinearlyVarying::Sections () const
{
  return theSections;
}

//=======================================================================
//function : SetSections
//purpose  : 
//=======================================================================

void StepFEA_CurveElementIntervalLinearlyVarying::SetSections (const Handle(StepElement_HArray1OfCurveElementSectionDefinition) &aSections)
{
  theSections = aSections;
}
