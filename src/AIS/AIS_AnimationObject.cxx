// Created by: Anastasia BORISOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <AIS_AnimationObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_AnimationObject, AIS_BaseAnimationObject)

//=============================================================================
//function : Constructor
//purpose  :
//=============================================================================
AIS_AnimationObject::AIS_AnimationObject (const TCollection_AsciiString& theAnimationName,
                                          const Handle(AIS_InteractiveContext)& theContext,
                                          const Handle(AIS_InteractiveObject)&  theObject,
                                          const gp_Trsf& theTrsfStart,
                                          const gp_Trsf& theTrsfEnd)
: AIS_BaseAnimationObject (theAnimationName, theContext, theObject),
  myTrsfLerp (theTrsfStart, theTrsfEnd)
{
  //
}

//=============================================================================
//function : update
//purpose  :
//=============================================================================
void AIS_AnimationObject::update (const AIS_AnimationProgress& theProgress)
{
  if (myObject.IsNull())
  {
    return;
  }

  gp_Trsf aTrsf;
  myTrsfLerp.Interpolate (theProgress.LocalNormalized, aTrsf);
  updateTrsf (aTrsf);
}
