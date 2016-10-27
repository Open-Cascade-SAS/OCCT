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

#include <AIS_AnimationCamera.hxx>

#include <Graphic3d_Camera.hxx>
#include <Precision.hxx>
#include <V3d_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_AnimationCamera, AIS_Animation)

//=============================================================================
//function : AIS_AnimationCamera
//purpose  :
//=============================================================================
AIS_AnimationCamera::AIS_AnimationCamera (const TCollection_AsciiString& theAnimationName,
                                          const Handle(V3d_View)& theView)
: AIS_Animation (theAnimationName),
  myView (theView)
{
  //
}

//=============================================================================
//function : update
//purpose  :
//=============================================================================
void AIS_AnimationCamera::update (const AIS_AnimationProgress& theProgress)
{
  if (myView.IsNull()
   || myCamStart.IsNull()
   || myCamEnd.IsNull())
  {
    return;
  }

  Handle(Graphic3d_Camera) aCamera = myView->Camera();

  Graphic3d_CameraLerp aCamLerp (myCamStart, myCamEnd);
  aCamLerp.Interpolate (theProgress.LocalNormalized, aCamera);

  const Standard_Boolean aPrevImmUpdate = myView->SetImmediateUpdate (Standard_False);
  myView->SetCamera (aCamera);
  myView->SetImmediateUpdate (aPrevImmUpdate);
  myView->Invalidate();
}
