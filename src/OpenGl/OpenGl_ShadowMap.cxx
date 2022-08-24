// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <OpenGl_ShadowMap.hxx>

#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <Graphic3d_CView.hxx>
#include <Message_Messenger.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_ShadowMap, OpenGl_NamedResource)

// =======================================================================
// function : OpenGl_ShadowMap
// purpose  :
// =======================================================================
OpenGl_ShadowMap::OpenGl_ShadowMap()
: OpenGl_NamedResource ("shadow_map"),
  myShadowMapFbo (new OpenGl_FrameBuffer (myResourceId + ":fbo")),
  myShadowCamera (new Graphic3d_Camera()),
  myShadowMapBias (0.0f)
{
  //
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_ShadowMap::Release (OpenGl_Context* theCtx)
{
  myShadowMapFbo->Release (theCtx);
}

// =======================================================================
// function : ~OpenGl_ShadowMap
// purpose  :
// =======================================================================
OpenGl_ShadowMap::~OpenGl_ShadowMap()
{
  Release (NULL);
}

// =======================================================================
// function : EstimatedDataSize
// purpose  :
// =======================================================================
Standard_Size OpenGl_ShadowMap::EstimatedDataSize() const
{
  return myShadowMapFbo->EstimatedDataSize();
}

// =======================================================================
// function : IsValid
// purpose  :
// =======================================================================
bool OpenGl_ShadowMap::IsValid() const
{
  return myShadowMapFbo->IsValid();
}

// =======================================================================
// function : Texture
// purpose  :
// =======================================================================
const Handle(OpenGl_Texture)& OpenGl_ShadowMap::Texture() const
{
  return myShadowMapFbo->DepthStencilTexture();
}

// =======================================================================
// function : UpdateCamera
// purpose  :
// =======================================================================
bool OpenGl_ShadowMap::UpdateCamera (const Graphic3d_CView& theView,
                                     const gp_XYZ* theOrigin)
{
  const Bnd_Box aMinMaxBox  = theOrigin == NULL ? theView.MinMaxValues (false) : Bnd_Box(); // applicative min max boundaries
  const Bnd_Box aGraphicBox = aMinMaxBox;

  switch (myShadowLight->Type())
  {
    case Graphic3d_TypeOfLightSource_Ambient:
    {
      return false; // not applicable
    }
    case Graphic3d_TypeOfLightSource_Directional:
    {
      if (theOrigin != NULL)
      {
        Graphic3d_Mat4d aTrans;
        aTrans.Translate (Graphic3d_Vec3d (theOrigin->X(), theOrigin->Y(), theOrigin->Z()));
        Graphic3d_Mat4d anOrientMat = myShadowCamera->OrientationMatrix() * aTrans;
        myLightMatrix = myShadowCamera->ProjectionMatrixF() * Graphic3d_Mat4 (anOrientMat);
        return true;
      }

      Graphic3d_Vec4d aDir (myShadowLight->Direction().X(), myShadowLight->Direction().Y(), myShadowLight->Direction().Z(), 0.0);
      if (myShadowLight->IsHeadlight())
      {
        Graphic3d_Mat4d anOrientInv;
        theView.Camera()->OrientationMatrix().Inverted (anOrientInv);
        aDir = anOrientInv * aDir;
      }
      myShadowCamera->SetZeroToOneDepth (theView.Camera()->IsZeroToOneDepth());
      myShadowCamera->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
      myShadowCamera->SetDirection (gp_Dir (aDir.x(), aDir.y(), aDir.z()));
      myShadowCamera->SetUp (!myShadowCamera->Direction().IsParallel (gp::DY(), Precision::Angular())
                            ? gp::DY()
                            : gp::DX());
      myShadowCamera->OrthogonalizeUp();

      // Fitting entire scene to the light might produce a shadow map of too low resolution.
      // More reliable approach would be putting a center to a current eye position and limiting maximum range,
      // so that shadow range will be limited to some reasonable distance from current eye.
      if (myShadowCamera->FitMinMax (aMinMaxBox, 10.0 * Precision::Confusion(), false))
      {
        myShadowCamera->SetScale (Max (myShadowCamera->ViewDimensions().X() * 1.1, myShadowCamera->ViewDimensions().Y() * 1.1)); // add margin
      }
      myShadowCamera->ZFitAll (1.0, aMinMaxBox, aGraphicBox);
      myLightMatrix = myShadowCamera->ProjectionMatrixF() * myShadowCamera->OrientationMatrixF();
      return true;
    }
    case Graphic3d_TypeOfLightSource_Positional:
    {
      // render into cubemap shadowmap texture
      return false; // not implemented
    }
    case Graphic3d_TypeOfLightSource_Spot:
    {
      if (theOrigin != NULL)
      {
        Graphic3d_Mat4d aTrans;
        aTrans.Translate (Graphic3d_Vec3d (theOrigin->X(), theOrigin->Y(), theOrigin->Z()));
        Graphic3d_Mat4d anOrientMat = myShadowCamera->OrientationMatrix() * aTrans;
        myLightMatrix = myShadowCamera->ProjectionMatrixF() * Graphic3d_Mat4 (anOrientMat);
        return true;
      }

      Graphic3d_Vec4d aDir (myShadowLight->Direction().X(), myShadowLight->Direction().Y(), myShadowLight->Direction().Z(), 0.0);
      if (myShadowLight->IsHeadlight())
      {
        Graphic3d_Mat4d anOrientInv;
        theView.Camera()->OrientationMatrix().Inverted (anOrientInv);
        aDir = anOrientInv * aDir;
      }

      myShadowCamera->SetZeroToOneDepth (theView.Camera()->IsZeroToOneDepth());
      myShadowCamera->SetProjectionType (Graphic3d_Camera::Projection_Perspective);

      const gp_Pnt& aLightPos = myShadowLight->Position();
      Standard_Real aDistance (aMinMaxBox.Distance (Bnd_Box (aLightPos, aLightPos))
                             + aMinMaxBox.CornerMin().Distance (aMinMaxBox.CornerMax()));
      myShadowCamera->SetDistance (aDistance);
      myShadowCamera->MoveEyeTo (aLightPos);
      myShadowCamera->SetDirectionFromEye (myShadowLight->Direction());
      myShadowCamera->SetUp (!myShadowCamera->Direction().IsParallel (gp::DY(), Precision::Angular())
                            ? gp::DY()
                            : gp::DX());
      myShadowCamera->OrthogonalizeUp();
      myShadowCamera->SetZRange (1.0, aDistance);

      myLightMatrix = myShadowCamera->ProjectionMatrixF() * myShadowCamera->OrientationMatrixF();

      return true;
    }
  }
  return false;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_ShadowMapArray::Release (OpenGl_Context* theCtx)
{
  for (Standard_Integer anIter = Lower(); anIter <= Upper(); ++anIter)
  {
    if (const Handle(OpenGl_ShadowMap)& aShadow = ChangeValue (anIter))
    {
      aShadow->Release (theCtx);
    }
  }
}

// =======================================================================
// function : EstimatedDataSize
// purpose  :
// =======================================================================
Standard_Size OpenGl_ShadowMapArray::EstimatedDataSize() const
{
  Standard_Size aSize = 0;
  for (Standard_Integer anIter = Lower(); anIter <= Upper(); ++anIter)
  {
    if (const Handle(OpenGl_ShadowMap)& aShadow = Value (anIter))
    {
      aSize += aShadow->EstimatedDataSize();
    }
  }
  return aSize;
}
