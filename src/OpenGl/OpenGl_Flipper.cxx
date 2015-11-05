// Created on: 2013-11-11
// Created by: Anastasia BORISOVA
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OpenGl_Flipper.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_Workspace.hxx>

#include <gp_Ax2.hxx>

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
OpenGl_Flipper::OpenGl_Flipper (const gp_Ax2& theReferenceSystem)
: OpenGl_Element(),
  myReferenceOrigin ((Standard_ShortReal )theReferenceSystem.Location().X(),
                     (Standard_ShortReal )theReferenceSystem.Location().Y(),
                     (Standard_ShortReal )theReferenceSystem.Location().Z(),
                     1.0f),
  myReferenceX ((Standard_ShortReal )theReferenceSystem.XDirection().X(),
                (Standard_ShortReal )theReferenceSystem.XDirection().Y(),
                (Standard_ShortReal )theReferenceSystem.XDirection().Z(),
                1.0f),
  myReferenceY ((Standard_ShortReal )theReferenceSystem.YDirection().X(),
                (Standard_ShortReal )theReferenceSystem.YDirection().Y(),
                (Standard_ShortReal )theReferenceSystem.YDirection().Z(),
                1.0f),
  myReferenceZ ((Standard_ShortReal )theReferenceSystem.Axis().Direction().X(),
                (Standard_ShortReal )theReferenceSystem.Axis().Direction().Y(),
                (Standard_ShortReal )theReferenceSystem.Axis().Direction().Z(),
                1.0f),
  myIsEnabled (Standard_True)
{
  //
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_Flipper::Release (OpenGl_Context*)
{
  //
}

// =======================================================================
// function : Render
// purpose  :
// =======================================================================
void OpenGl_Flipper::Render (const Handle(OpenGl_Workspace)& theWorkspace) const
{
  // Check if rendering is to be in immediate mode
  const Handle(OpenGl_Context)& aContext = theWorkspace->GetGlContext();
  if (!myIsEnabled)
  {
    // restore matrix state
    aContext->WorldViewState.Pop();

    // Apply since we probably in the middle of something.
    aContext->ApplyModelViewMatrix();
    return;
  }

  aContext->WorldViewState.Push();
  OpenGl_Mat4 aMatrixMV = aContext->WorldViewState.Current() * aContext->ModelWorldState.Current();

  const OpenGl_Vec4 aMVReferenceOrigin = aMatrixMV * myReferenceOrigin;
  const OpenGl_Vec4 aMVReferenceX      = aMatrixMV * OpenGl_Vec4 (myReferenceX.xyz() + myReferenceOrigin.xyz(), 1.0f);
  const OpenGl_Vec4 aMVReferenceY      = aMatrixMV * OpenGl_Vec4 (myReferenceY.xyz() + myReferenceOrigin.xyz(), 1.0f);
  const OpenGl_Vec4 aMVReferenceZ      = aMatrixMV * OpenGl_Vec4 (myReferenceZ.xyz() + myReferenceOrigin.xyz(), 1.0f);

  const OpenGl_Vec4 aDirX = aMVReferenceX - aMVReferenceOrigin;
  const OpenGl_Vec4 aDirY = aMVReferenceY - aMVReferenceOrigin;
  const OpenGl_Vec4 aDirZ = aMVReferenceZ - aMVReferenceOrigin;

  Standard_Boolean isReversedX = aDirX.xyz().Dot (OpenGl_Vec3::DX()) < 0.0f;
  Standard_Boolean isReversedY = aDirY.xyz().Dot (OpenGl_Vec3::DY()) < 0.0f;
  Standard_Boolean isReversedZ = aDirZ.xyz().Dot (OpenGl_Vec3::DZ()) < 0.0f;

  // compute flipping (rotational transform)
  OpenGl_Mat4 aTransform;
  if ((isReversedX || isReversedY) && !isReversedZ)
  {
    // invert by Z axis: left, up vectors mirrored
    aTransform.SetColumn (0, -aTransform.GetColumn (0).xyz());
    aTransform.SetColumn (1, -aTransform.GetColumn (1).xyz());
  }
  else if (isReversedY && isReversedZ)
  {
    // rotate by X axis: up, forward vectors mirrored
    aTransform.SetColumn (1, -aTransform.GetColumn (1).xyz());
    aTransform.SetColumn (2, -aTransform.GetColumn (2).xyz());
  }
  else if (isReversedZ)
  {
    // rotate by Y axis: left, forward vectors mirrored
    aTransform.SetColumn (0, -aTransform.GetColumn (0).xyz());
    aTransform.SetColumn (2, -aTransform.GetColumn (2).xyz());
  }
  else
  {
    return;
  }

  // do rotation in origin around reference system "forward" direction
  OpenGl_Mat4 aRefAxes;
  OpenGl_Mat4 aRefInv;
  aRefAxes.SetColumn (0, myReferenceX.xyz());
  aRefAxes.SetColumn (1, myReferenceY.xyz());
  aRefAxes.SetColumn (2, myReferenceZ.xyz());
  aRefAxes.SetColumn (3, myReferenceOrigin.xyz());
  aRefAxes.Inverted (aRefInv);

  aTransform = aRefAxes * aTransform * aRefInv;

  // transform model-view matrix
  aMatrixMV = aMatrixMV * aTransform;

  // load transformed model-view matrix
  aContext->WorldViewState.SetCurrent (aMatrixMV);
  aContext->ApplyWorldViewMatrix();
}
