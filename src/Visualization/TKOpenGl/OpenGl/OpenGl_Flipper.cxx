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

#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_Workspace.hxx>

#include <gp_Ax2.hxx>

//=================================================================================================

OpenGl_Flipper::OpenGl_Flipper(const gp_Ax2& theReferenceSystem)
    : OpenGl_Element(),
      myReferenceOrigin((float)theReferenceSystem.Location().X(),
                        (float)theReferenceSystem.Location().Y(),
                        (float)theReferenceSystem.Location().Z(),
                        1.0f),
      myReferenceX((float)theReferenceSystem.XDirection().X(),
                   (float)theReferenceSystem.XDirection().Y(),
                   (float)theReferenceSystem.XDirection().Z(),
                   1.0f),
      myReferenceY((float)theReferenceSystem.YDirection().X(),
                   (float)theReferenceSystem.YDirection().Y(),
                   (float)theReferenceSystem.YDirection().Z(),
                   1.0f),
      myReferenceZ((float)theReferenceSystem.Axis().Direction().X(),
                   (float)theReferenceSystem.Axis().Direction().Y(),
                   (float)theReferenceSystem.Axis().Direction().Z(),
                   1.0f),
      myIsEnabled(true)
{
  //
}

//=================================================================================================

void OpenGl_Flipper::Release(OpenGl_Context*)
{
  //
}

//=================================================================================================

void OpenGl_Flipper::Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const
{
  // Check if rendering is to be in immediate mode
  const occ::handle<OpenGl_Context>& aContext = theWorkspace->GetGlContext();
  if (!myIsEnabled)
  {
    // restore matrix state
    aContext->ModelWorldState.Pop();

    // Apply since we probably in the middle of something.
    aContext->ApplyModelWorldMatrix();
    return;
  }

  aContext->ModelWorldState.Push();

  NCollection_Mat4<float> aModelWorldMatrix;
  aModelWorldMatrix.Convert(aContext->ModelWorldState.Current());

  NCollection_Mat4<float> aMatrixMV = aContext->WorldViewState.Current() * aModelWorldMatrix;

  const NCollection_Vec4<float> aMVReferenceOrigin = aMatrixMV * myReferenceOrigin;
  const NCollection_Vec4<float> aMVReferenceX =
    aMatrixMV * NCollection_Vec4<float>(myReferenceX.xyz() + myReferenceOrigin.xyz(), 1.0f);
  const NCollection_Vec4<float> aMVReferenceY =
    aMatrixMV * NCollection_Vec4<float>(myReferenceY.xyz() + myReferenceOrigin.xyz(), 1.0f);
  const NCollection_Vec4<float> aMVReferenceZ =
    aMatrixMV * NCollection_Vec4<float>(myReferenceZ.xyz() + myReferenceOrigin.xyz(), 1.0f);

  const NCollection_Vec4<float> aDirX = aMVReferenceX - aMVReferenceOrigin;
  const NCollection_Vec4<float> aDirY = aMVReferenceY - aMVReferenceOrigin;
  const NCollection_Vec4<float> aDirZ = aMVReferenceZ - aMVReferenceOrigin;

  bool isReversedX = aDirX.xyz().Dot(NCollection_Vec3<float>::DX()) < 0.0f;
  bool isReversedY = aDirY.xyz().Dot(NCollection_Vec3<float>::DY()) < 0.0f;
  bool isReversedZ = aDirZ.xyz().Dot(NCollection_Vec3<float>::DZ()) < 0.0f;

  // compute flipping (rotational transform)
  NCollection_Mat4<float> aTransform;
  if ((isReversedX || isReversedY) && !isReversedZ)
  {
    // invert by Z axis: left, up vectors mirrored
    aTransform.SetColumn(0, -aTransform.GetColumn(0).xyz());
    aTransform.SetColumn(1, -aTransform.GetColumn(1).xyz());
  }
  else if (isReversedY && isReversedZ)
  {
    // rotate by X axis: up, forward vectors mirrored
    aTransform.SetColumn(1, -aTransform.GetColumn(1).xyz());
    aTransform.SetColumn(2, -aTransform.GetColumn(2).xyz());
  }
  else if (isReversedZ)
  {
    // rotate by Y axis: left, forward vectors mirrored
    aTransform.SetColumn(0, -aTransform.GetColumn(0).xyz());
    aTransform.SetColumn(2, -aTransform.GetColumn(2).xyz());
  }
  else
  {
    return;
  }

  // do rotation in origin around reference system "forward" direction
  NCollection_Mat4<float> aRefAxes;
  NCollection_Mat4<float> aRefInv;
  aRefAxes.SetColumn(0, myReferenceX.xyz());
  aRefAxes.SetColumn(1, myReferenceY.xyz());
  aRefAxes.SetColumn(2, myReferenceZ.xyz());
  aRefAxes.SetColumn(3, myReferenceOrigin.xyz());
  aRefAxes.Inverted(aRefInv);

  aTransform = aRefAxes * aTransform * aRefInv;

  // transform model-view matrix
  aModelWorldMatrix = aModelWorldMatrix * aTransform;

  // load transformed model-view matrix
  aContext->ModelWorldState.SetCurrent(aModelWorldMatrix);
  aContext->ApplyModelWorldMatrix();
}

//=================================================================================================

void OpenGl_Flipper::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, OpenGl_Flipper)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, OpenGl_Element)
}
