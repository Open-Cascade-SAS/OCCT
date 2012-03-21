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

#include <StepFEA_CurveElementInterval.ixx>

//=======================================================================
//function : StepFEA_CurveElementInterval
//purpose  : 
//=======================================================================

StepFEA_CurveElementInterval::StepFEA_CurveElementInterval ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_CurveElementInterval::Init (const Handle(StepFEA_CurveElementLocation) &aFinishPosition,
                                         const Handle(StepBasic_EulerAngles) &aEuAngles)
{

  theFinishPosition = aFinishPosition;

  theEuAngles = aEuAngles;
}

//=======================================================================
//function : FinishPosition
//purpose  : 
//=======================================================================

Handle(StepFEA_CurveElementLocation) StepFEA_CurveElementInterval::FinishPosition () const
{
  return theFinishPosition;
}

//=======================================================================
//function : SetFinishPosition
//purpose  : 
//=======================================================================

void StepFEA_CurveElementInterval::SetFinishPosition (const Handle(StepFEA_CurveElementLocation) &aFinishPosition)
{
  theFinishPosition = aFinishPosition;
}

//=======================================================================
//function : EuAngles
//purpose  : 
//=======================================================================

Handle(StepBasic_EulerAngles) StepFEA_CurveElementInterval::EuAngles () const
{
  return theEuAngles;
}

//=======================================================================
//function : SetEuAngles
//purpose  : 
//=======================================================================

void StepFEA_CurveElementInterval::SetEuAngles (const Handle(StepBasic_EulerAngles) &aEuAngles)
{
  theEuAngles = aEuAngles;
}
