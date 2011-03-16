// File:	StepBasic_EulerAngles.cxx
// Created:	Thu Dec 12 15:38:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepBasic_EulerAngles.ixx>

//=======================================================================
//function : StepBasic_EulerAngles
//purpose  : 
//=======================================================================

StepBasic_EulerAngles::StepBasic_EulerAngles ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_EulerAngles::Init (const Handle(TColStd_HArray1OfReal) &aAngles)
{

  theAngles = aAngles;
}

//=======================================================================
//function : Angles
//purpose  : 
//=======================================================================

Handle(TColStd_HArray1OfReal) StepBasic_EulerAngles::Angles () const
{
  return theAngles;
}

//=======================================================================
//function : SetAngles
//purpose  : 
//=======================================================================

void StepBasic_EulerAngles::SetAngles (const Handle(TColStd_HArray1OfReal) &aAngles)
{
  theAngles = aAngles;
}
