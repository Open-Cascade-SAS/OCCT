// File:	StepFEA_CurveElementInterval.cxx
// Created:	Thu Dec 12 17:51:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
