// File:	StepElement_CurveElementEndReleasePacket.cxx
// Created:	Thu Dec 12 17:29:00 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_CurveElementEndReleasePacket.ixx>

//=======================================================================
//function : StepElement_CurveElementEndReleasePacket
//purpose  : 
//=======================================================================

StepElement_CurveElementEndReleasePacket::StepElement_CurveElementEndReleasePacket ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_CurveElementEndReleasePacket::Init (const StepElement_CurveElementFreedom &aReleaseFreedom,
                                                     const Standard_Real aReleaseStiffness)
{

  theReleaseFreedom = aReleaseFreedom;

  theReleaseStiffness = aReleaseStiffness;
}

//=======================================================================
//function : ReleaseFreedom
//purpose  : 
//=======================================================================

StepElement_CurveElementFreedom StepElement_CurveElementEndReleasePacket::ReleaseFreedom () const
{
  return theReleaseFreedom;
}

//=======================================================================
//function : SetReleaseFreedom
//purpose  : 
//=======================================================================

void StepElement_CurveElementEndReleasePacket::SetReleaseFreedom (const StepElement_CurveElementFreedom &aReleaseFreedom)
{
  theReleaseFreedom = aReleaseFreedom;
}

//=======================================================================
//function : ReleaseStiffness
//purpose  : 
//=======================================================================

Standard_Real StepElement_CurveElementEndReleasePacket::ReleaseStiffness () const
{
  return theReleaseStiffness;
}

//=======================================================================
//function : SetReleaseStiffness
//purpose  : 
//=======================================================================

void StepElement_CurveElementEndReleasePacket::SetReleaseStiffness (const Standard_Real aReleaseStiffness)
{
  theReleaseStiffness = aReleaseStiffness;
}
