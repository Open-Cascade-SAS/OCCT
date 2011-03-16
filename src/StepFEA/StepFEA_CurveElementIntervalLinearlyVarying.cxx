// File:	StepFEA_CurveElementIntervalLinearlyVarying.cxx
// Created:	Wed Jan 22 17:31:43 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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
