// File:	StepFEA_CurveElementIntervalConstant.cxx
// Created:	Thu Dec 12 17:51:04 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
