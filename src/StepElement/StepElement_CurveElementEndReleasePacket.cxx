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
