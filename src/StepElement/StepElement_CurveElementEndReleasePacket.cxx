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
#include <StepElement_CurveElementEndReleasePacket.hxx>
#include <StepElement_CurveElementFreedom.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_CurveElementEndReleasePacket,Standard_Transient)

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
