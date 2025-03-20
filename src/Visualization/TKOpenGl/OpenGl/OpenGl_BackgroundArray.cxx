// Created on: 2015-01-16
// Created by: Anastasia BORISOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <OpenGl_BackgroundArray.hxx>

#include <Aspect_FillMethod.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_View.hxx>

// =======================================================================
// method  : Constructor
// purpose :
// =======================================================================
OpenGl_BackgroundArray::OpenGl_BackgroundArray(const Graphic3d_TypeOfBackground theType)
    : OpenGl_PrimitiveArray(NULL, Graphic3d_TOPA_TRIANGLES, NULL, NULL, NULL),
      myType(theType),
      myFillMethod(Aspect_FM_NONE),
      myViewWidth(0),
      myViewHeight(0),
      myToUpdate(Standard_False)
{
  myDrawMode   = GL_TRIANGLES;
  myIsFillType = true;

  myGradientParams.color1 = OpenGl_Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  myGradientParams.color2 = OpenGl_Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  myGradientParams.type   = Aspect_GradientFillMethod_None;
}

// =======================================================================
// method  : SetTextureParameters
// purpose :
// =======================================================================
void OpenGl_BackgroundArray::SetTextureParameters(const Aspect_FillMethod theFillMethod)
{
  if (myType != Graphic3d_TOB_TEXTURE)
  {
    return;
  }

  myFillMethod = theFillMethod;
  invalidateData();
}

// =======================================================================
// method  : SetTextureFillMethod
// purpose :
// =======================================================================
void OpenGl_BackgroundArray::SetTextureFillMethod(const Aspect_FillMethod theFillMethod)
{
  myFillMethod = theFillMethod;
  invalidateData();
}

// =======================================================================
// method  : SetGradientParameters
// purpose :
// =======================================================================
void OpenGl_BackgroundArray::SetGradientParameters(const Quantity_Color&           theColor1,
                                                   const Quantity_Color&           theColor2,
                                                   const Aspect_GradientFillMethod theType)
{
  if (myType != Graphic3d_TOB_GRADIENT)
  {
    return;
  }

  Standard_Real anR, aG, aB;
  theColor1.Values(anR, aG, aB, Quantity_TOC_RGB);
  myGradientParams.color1 = OpenGl_Vec4((float)anR, (float)aG, (float)aB, 0.0f);

  theColor2.Values(anR, aG, aB, Quantity_TOC_RGB);
  myGradientParams.color2 = OpenGl_Vec4((float)anR, (float)aG, (float)aB, 0.0f);

  myGradientParams.type = theType;
  invalidateData();
}

// =======================================================================
// method  : SetGradientFillMethod
// purpose :
// =======================================================================
void OpenGl_BackgroundArray::SetGradientFillMethod(const Aspect_GradientFillMethod theType)
{
  if (myType != Graphic3d_TOB_GRADIENT)
  {
    return;
  }

  myGradientParams.type = theType;
  invalidateData();
}

// =======================================================================
// method  : IsDefined
// purpose :
// =======================================================================
bool OpenGl_BackgroundArray::IsDefined() const
{
  switch (myType)
  {
    case Graphic3d_TOB_GRADIENT:
      return myGradientParams.type != Aspect_GradientFillMethod_None;
    case Graphic3d_TOB_TEXTURE:
      return myFillMethod != Aspect_FM_NONE;
    case Graphic3d_TOB_CUBEMAP:
      return Standard_True;
    case Graphic3d_TOB_NONE:
      return Standard_False;
  }
  return Standard_False;
}

// =======================================================================
// method  : invalidateData
// purpose :
// =======================================================================
void OpenGl_BackgroundArray::invalidateData()
{
  myToUpdate = Standard_True;
}

// =======================================================================
// method  : init
// purpose :
// =======================================================================
Standard_Boolean OpenGl_BackgroundArray::init(const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  if (myIndices.IsNull())
  {
    myIndices = new Graphic3d_IndexBuffer(Graphic3d_Buffer::DefaultAllocator());
  }
  if (myAttribs.IsNull())
  {
    myAttribs = new Graphic3d_Buffer(Graphic3d_Buffer::DefaultAllocator());
  }

  switch (myType)
  {
    case Graphic3d_TOB_GRADIENT: {
      if (!createGradientArray(aCtx))
      {
        return Standard_False;
      }
      break;
    }
    case Graphic3d_TOB_TEXTURE: {
      if (!createTextureArray(theWorkspace))
      {
        return Standard_False;
      }
      break;
    }
    case Graphic3d_TOB_CUBEMAP: {
      if (!createCubeMapArray())
      {
        return Standard_False;
      }
      break;
    }
    case Graphic3d_TOB_NONE:
    default: {
      return Standard_False;
    }
  }

  // Init VBO
  if (myIsVboInit)
  {
    clearMemoryGL(aCtx);
  }
  buildVBO(aCtx, Standard_True);
  myIsVboInit = Standard_True;

  // Data is up-to-date
  myToUpdate = Standard_False;
  return Standard_True;
}

// =======================================================================
// method  : createGradientArray
// purpose :
// =======================================================================
Standard_Boolean OpenGl_BackgroundArray::createGradientArray(
  const Handle(OpenGl_Context)& theCtx) const
{
  // Initialize data for primitive array
  Graphic3d_Attribute aGragientAttribInfo[] = {{Graphic3d_TOA_POS, Graphic3d_TOD_VEC2},
                                               {Graphic3d_TOA_COLOR, Graphic3d_TOD_VEC3}};

  if (!myAttribs->Init(4, aGragientAttribInfo, 2))
  {
    return Standard_False;
  }
  if (!myIndices->Init<unsigned short>(6))
  {
    return Standard_False;
  }
  const unsigned short THE_FS_QUAD_TRIS[6] = {0, 1, 3, 1, 2, 3};
  for (unsigned int aVertIter = 0; aVertIter < 6; ++aVertIter)
  {
    myIndices->SetIndex(aVertIter, THE_FS_QUAD_TRIS[aVertIter]);
  }

  OpenGl_Vec2 aVertices[4] = {OpenGl_Vec2(float(myViewWidth), 0.0f),
                              OpenGl_Vec2(float(myViewWidth), float(myViewHeight)),
                              OpenGl_Vec2(0.0f, float(myViewHeight)),
                              OpenGl_Vec2(0.0f, 0.0f)};

  float* aCorners[4]     = {};
  float  aDiagCorner1[3] = {};
  float  aDiagCorner2[3] = {};

  switch (myGradientParams.type)
  {
    case Aspect_GradientFillMethod_Horizontal: {
      aCorners[0] = myGradientParams.color2.ChangeData();
      aCorners[1] = myGradientParams.color2.ChangeData();
      aCorners[2] = myGradientParams.color1.ChangeData();
      aCorners[3] = myGradientParams.color1.ChangeData();
      break;
    }
    case Aspect_GradientFillMethod_Vertical: {
      aCorners[0] = myGradientParams.color2.ChangeData();
      aCorners[1] = myGradientParams.color1.ChangeData();
      aCorners[2] = myGradientParams.color1.ChangeData();
      aCorners[3] = myGradientParams.color2.ChangeData();
      break;
    }
    case Aspect_GradientFillMethod_Diagonal1: {
      aCorners[0]     = myGradientParams.color2.ChangeData();
      aCorners[2]     = myGradientParams.color1.ChangeData();
      aDiagCorner1[0] = aDiagCorner2[0] = 0.5f * (aCorners[0][0] + aCorners[2][0]);
      aDiagCorner1[1] = aDiagCorner2[1] = 0.5f * (aCorners[0][1] + aCorners[2][1]);
      aDiagCorner1[2] = aDiagCorner2[2] = 0.5f * (aCorners[0][2] + aCorners[2][2]);
      aCorners[1]                       = aDiagCorner1;
      aCorners[3]                       = aDiagCorner2;
      break;
    }
    case Aspect_GradientFillMethod_Diagonal2: {
      aCorners[1]     = myGradientParams.color1.ChangeData();
      aCorners[3]     = myGradientParams.color2.ChangeData();
      aDiagCorner1[0] = aDiagCorner2[0] = 0.5f * (aCorners[1][0] + aCorners[3][0]);
      aDiagCorner1[1] = aDiagCorner2[1] = 0.5f * (aCorners[1][1] + aCorners[3][1]);
      aDiagCorner1[2] = aDiagCorner2[2] = 0.5f * (aCorners[1][2] + aCorners[3][2]);
      aCorners[0]                       = aDiagCorner1;
      aCorners[2]                       = aDiagCorner2;
      break;
    }
    case Aspect_GradientFillMethod_Corner1:
    case Aspect_GradientFillMethod_Corner2:
    case Aspect_GradientFillMethod_Corner3:
    case Aspect_GradientFillMethod_Corner4: {
      Graphic3d_Attribute aCornerAttribInfo[] = {{Graphic3d_TOA_POS, Graphic3d_TOD_VEC2},
                                                 {Graphic3d_TOA_UV, Graphic3d_TOD_VEC2}};

      OpenGl_Vec2 anUVs[4] = {OpenGl_Vec2(1.0f, 0.0f),
                              OpenGl_Vec2(1.0f, 1.0f),
                              OpenGl_Vec2(0.0f, 1.0f),
                              OpenGl_Vec2(0.0f, 0.0f)};

      if (!myAttribs->Init(4, aCornerAttribInfo, 2))
      {
        return Standard_False;
      }
      for (Standard_Integer anIt = 0; anIt < 4; ++anIt)
      {
        OpenGl_Vec2* aVertData = reinterpret_cast<OpenGl_Vec2*>(myAttribs->changeValue(anIt));
        *aVertData             = aVertices[anIt];

        OpenGl_Vec2* anUvData = reinterpret_cast<OpenGl_Vec2*>(myAttribs->changeValue(anIt)
                                                               + myAttribs->AttributeOffset(1));
        // cyclically move highlighted corner depending on myGradientParams.type
        *anUvData = anUVs[(anIt + myGradientParams.type - Aspect_GradientFillMethod_Corner1) % 4];
      }
      return Standard_True;
    }
    case Aspect_GradientFillMethod_Elliptical: {
      // construction of a circle circumscribed about a view rectangle
      // using parametric equation (scaled by aspect ratio and centered)
      const Standard_Integer aSubdiv = 64;
      if (!myAttribs->Init(aSubdiv + 2, aGragientAttribInfo, 2))
      {
        return Standard_False;
      }

      OpenGl_Vec2 anEllipVerts[aSubdiv + 2];
      anEllipVerts[0]      = OpenGl_Vec2(float(myViewWidth) / 2.0f, float(myViewHeight) / 2.0f);
      Standard_Real aTetta = (M_PI * 2.0) / aSubdiv;
      Standard_Real aParam = 0.0;
      for (Standard_Integer anIt = 1; anIt < aSubdiv + 2; ++anIt)
      {
        anEllipVerts[anIt] =
          OpenGl_Vec2(float(Cos(aParam) * Sqrt(2.0) * myViewWidth / 2.0 + myViewWidth / 2.0f),
                      float(Sin(aParam) * Sqrt(2.0) * myViewHeight / 2.0 + myViewHeight / 2.0f));

        aParam += aTetta;
      }
      for (Standard_Integer anIt = 0; anIt < aSubdiv + 2; ++anIt)
      {
        OpenGl_Vec2* aVertData = reinterpret_cast<OpenGl_Vec2*>(myAttribs->changeValue(anIt));
        *aVertData             = anEllipVerts[anIt];

        OpenGl_Vec3* aColorData = reinterpret_cast<OpenGl_Vec3*>(myAttribs->changeValue(anIt)
                                                                 + myAttribs->AttributeOffset(1));
        *aColorData             = myGradientParams.color2.rgb();
      }
      // the central vertex is colored in different way
      OpenGl_Vec3* aColorData =
        reinterpret_cast<OpenGl_Vec3*>(myAttribs->changeValue(0) + myAttribs->AttributeOffset(1));
      *aColorData = myGradientParams.color1.rgb();

      if (!myIndices->Init<unsigned short>(3 * aSubdiv))
      {
        return Standard_False;
      }
      for (Standard_Integer aCurTri = 0; aCurTri < aSubdiv; aCurTri++)
      {
        myIndices->SetIndex(aCurTri * 3 + 0, 0);
        myIndices->SetIndex(aCurTri * 3 + 1, aCurTri + 1);
        myIndices->SetIndex(aCurTri * 3 + 2, aCurTri + 2);
      }

      return Standard_True;
    }
    case Aspect_GradientFillMethod_None: {
      break;
    }
  }

  for (Standard_Integer anIt = 0; anIt < 4; ++anIt)
  {
    OpenGl_Vec2* aVertData = reinterpret_cast<OpenGl_Vec2*>(myAttribs->changeValue(anIt));
    *aVertData             = aVertices[anIt];

    OpenGl_Vec3* aColorData =
      reinterpret_cast<OpenGl_Vec3*>(myAttribs->changeValue(anIt) + myAttribs->AttributeOffset(1));
    *aColorData = theCtx
                    ->Vec4FromQuantityColor(
                      OpenGl_Vec4(aCorners[anIt][0], aCorners[anIt][1], aCorners[anIt][2], 1.0f))
                    .rgb();
  }

  return Standard_True;
}

// =======================================================================
// method  : createTextureArray
// purpose :
// =======================================================================
Standard_Boolean OpenGl_BackgroundArray::createTextureArray(
  const Handle(OpenGl_Workspace)& theWorkspace) const
{
  Graphic3d_Attribute aTextureAttribInfo[] = {{Graphic3d_TOA_POS, Graphic3d_TOD_VEC2},
                                              {Graphic3d_TOA_UV, Graphic3d_TOD_VEC2}};

  if (!myAttribs->Init(4, aTextureAttribInfo, 2))
  {
    return Standard_False;
  }

  GLfloat aTexRangeX = 1.0f; // texture <s> coordinate
  GLfloat aTexRangeY = 1.0f; // texture <t> coordinate

  // Set up for stretching or tiling
  GLfloat anOffsetX = 0.5f * (float)myViewWidth;
  GLfloat anOffsetY = 0.5f * (float)myViewHeight;

  // Setting this coefficient to -1.0f allows to tile textures relatively to the top-left corner of
  // the view (value 1.0f corresponds to the initial behavior - tiling from the bottom-left corner)
  GLfloat aCoef = -1.0f;

  // Get texture parameters
  const Handle(OpenGl_Context)& aCtx         = theWorkspace->GetGlContext();
  const OpenGl_Aspects*         anAspectFace = theWorkspace->Aspects();
  GLfloat aTextureWidth  = (GLfloat)anAspectFace->TextureSet(aCtx)->First()->SizeX();
  GLfloat aTextureHeight = (GLfloat)anAspectFace->TextureSet(aCtx)->First()->SizeY();

  if (myFillMethod == Aspect_FM_CENTERED)
  {
    anOffsetX = 0.5f * aTextureWidth;
    anOffsetY = 0.5f * aTextureHeight;
  }
  else if (myFillMethod == Aspect_FM_TILED)
  {
    aTexRangeX = (GLfloat)myViewWidth / aTextureWidth;
    aTexRangeY = (GLfloat)myViewHeight / aTextureHeight;
  }

  // NOTE: texture is mapped using GL_REPEAT wrapping mode so integer part
  // is simply ignored, and negative multiplier is here for convenience only
  // and does not result e.g. in texture mirroring

  OpenGl_Vec2* aData = reinterpret_cast<OpenGl_Vec2*>(myAttribs->changeValue(0));
  aData[0]           = OpenGl_Vec2(anOffsetX, -aCoef * anOffsetY);
  aData[1]           = OpenGl_Vec2(aTexRangeX, 0.0f);

  aData    = reinterpret_cast<OpenGl_Vec2*>(myAttribs->changeValue(1));
  aData[0] = OpenGl_Vec2(anOffsetX, aCoef * anOffsetY);
  aData[1] = OpenGl_Vec2(aTexRangeX, aCoef * aTexRangeY);

  aData    = reinterpret_cast<OpenGl_Vec2*>(myAttribs->changeValue(2));
  aData[0] = OpenGl_Vec2(-anOffsetX, -aCoef * anOffsetY);
  aData[1] = OpenGl_Vec2(0.0f, 0.0f);

  aData    = reinterpret_cast<OpenGl_Vec2*>(myAttribs->changeValue(3));
  aData[0] = OpenGl_Vec2(-anOffsetX, aCoef * anOffsetY);
  aData[1] = OpenGl_Vec2(0.0f, aCoef * aTexRangeY);

  if (!myIndices->Init<unsigned short>(6))
  {
    return Standard_False;
  }
  const unsigned short THE_FS_QUAD_TRIS[6] = {0, 1, 2, 1, 3, 2};
  for (unsigned int aVertIter = 0; aVertIter < 6; ++aVertIter)
  {
    myIndices->SetIndex(aVertIter, THE_FS_QUAD_TRIS[aVertIter]);
  }

  return Standard_True;
}

// =======================================================================
// method  : createCubeMapArray
// purpose :
// =======================================================================
Standard_Boolean OpenGl_BackgroundArray::createCubeMapArray() const
{
  const Graphic3d_Attribute aCubeMapAttribInfo[] = {{Graphic3d_TOA_POS, Graphic3d_TOD_VEC3}};

  if (myAttribs.IsNull())
  {
    myAttribs = new Graphic3d_Buffer(Graphic3d_Buffer::DefaultAllocator());
    myIndices = new Graphic3d_IndexBuffer(Graphic3d_Buffer::DefaultAllocator());
  }
  if (!myAttribs->Init(8, aCubeMapAttribInfo, 1) || !myIndices->Init<unsigned short>(6 * 3 * 2))
  {
    return Standard_False;
  }

  {
    OpenGl_Vec3* aData = reinterpret_cast<OpenGl_Vec3*>(myAttribs->changeValue(0));
    aData[0].SetValues(-1.0, -1.0, 1.0);
    aData[1].SetValues(1.0, -1.0, 1.0);
    aData[2].SetValues(-1.0, 1.0, 1.0);
    aData[3].SetValues(1.0, 1.0, 1.0);
    aData[4].SetValues(-1.0, -1.0, -1.0);
    aData[5].SetValues(1.0, -1.0, -1.0);
    aData[6].SetValues(-1.0, 1.0, -1.0);
    aData[7].SetValues(1.0, 1.0, -1.0);
  }
  {
    const unsigned short THE_BOX_TRIS[] = {
      0, 1, 2, 2, 1, 3, // top face
      1, 5, 7, 1, 7, 3, // right face
      0, 6, 4, 0, 2, 6, // left face
      4, 6, 5, 6, 7, 5, // bottom face
      0, 5, 1, 0, 4, 5, // front face
      2, 7, 6, 2, 3, 7  // back face
    };
    for (unsigned int aVertIter = 0; aVertIter < 6 * 3 * 2; ++aVertIter)
    {
      myIndices->SetIndex(aVertIter, THE_BOX_TRIS[aVertIter]);
    }
  }

  return Standard_True;
}

// =======================================================================
// method  : Render
// purpose :
// =======================================================================
void OpenGl_BackgroundArray::Render(const Handle(OpenGl_Workspace)& theWorkspace,
                                    Graphic3d_Camera::Projection    theProjection) const
{
  const Handle(OpenGl_Context)& aCtx       = theWorkspace->GetGlContext();
  Standard_Integer              aViewSizeX = aCtx->Viewport()[2];
  Standard_Integer              aViewSizeY = aCtx->Viewport()[3];
  Graphic3d_Vec2i               aTileOffset, aTileSize;

  if (aCtx->Camera()->Tile().IsValid())
  {
    aViewSizeX = aCtx->Camera()->Tile().TotalSize.x();
    aViewSizeY = aCtx->Camera()->Tile().TotalSize.y();

    aTileOffset = aCtx->Camera()->Tile().OffsetLowerLeft();
    aTileSize   = aCtx->Camera()->Tile().TileSize;
  }
  if (myToUpdate || myViewWidth != aViewSizeX || myViewHeight != aViewSizeY || myAttribs.IsNull()
      || myVboAttribs.IsNull())
  {
    myViewWidth  = aViewSizeX;
    myViewHeight = aViewSizeY;
    init(theWorkspace);
  }

  OpenGl_Mat4 aProjection = aCtx->ProjectionState.Current();
  OpenGl_Mat4 aWorldView  = aCtx->WorldViewState.Current();

  if (myType == Graphic3d_TOB_CUBEMAP)
  {
    Graphic3d_Camera aCamera(aCtx->Camera());
    aCamera.SetZRange(0.01, 1.0); // is needed to avoid perspective camera exception

    // cancel translation
    aCamera.MoveEyeTo(gp_Pnt(0.0, 0.0, 0.0));

    // Handle projection matrix:
    // - Cancel any head-to-eye translation for HMD display;
    // - Ignore stereoscopic projection in case of non-HMD 3D display
    //   (ideally, we would need a stereoscopic cubemap image; adding a parallax makes no sense);
    // - Force perspective projection when orthographic camera is active
    //   (orthographic projection makes no sense for cubemap).
    const bool isCustomProj = aCamera.IsCustomStereoFrustum() || aCamera.IsCustomStereoProjection();
    aCamera.SetProjectionType(theProjection == Graphic3d_Camera::Projection_Orthographic
                                  || !isCustomProj
                                ? Graphic3d_Camera::Projection_Perspective
                                : theProjection);

    aProjection = aCamera.ProjectionMatrixF();
    aWorldView  = aCamera.OrientationMatrixF();
    if (isCustomProj)
    {
      // get projection matrix without pre-multiplied stereoscopic head-to-eye translation
      if (theProjection == Graphic3d_Camera::Projection_MonoLeftEye)
      {
        Graphic3d_Mat4 aMatProjL, aMatHeadToEyeL, aMatProjR, aMatHeadToEyeR;
        aCamera.StereoProjectionF(aMatProjL, aMatHeadToEyeL, aMatProjR, aMatHeadToEyeR);
        aProjection = aMatProjL;
      }
      else if (theProjection == Graphic3d_Camera::Projection_MonoRightEye)
      {
        Graphic3d_Mat4 aMatProjL, aMatHeadToEyeL, aMatProjR, aMatHeadToEyeR;
        aCamera.StereoProjectionF(aMatProjL, aMatHeadToEyeL, aMatProjR, aMatHeadToEyeR);
        aProjection = aMatProjR;
      }
    }
  }
  else
  {
    aProjection.InitIdentity();
    aWorldView.InitIdentity();
    if (aCtx->Camera()->Tile().IsValid())
    {
      aWorldView.SetDiagonal(OpenGl_Vec4(2.0f / aTileSize.x(), 2.0f / aTileSize.y(), 1.0f, 1.0f));
      if (myType == Graphic3d_TOB_GRADIENT)
      {
        aWorldView.SetColumn(3,
                             OpenGl_Vec4(-1.0f - 2.0f * aTileOffset.x() / aTileSize.x(),
                                         -1.0f - 2.0f * aTileOffset.y() / aTileSize.y(),
                                         0.0f,
                                         1.0f));
      }
      else
      {
        aWorldView.SetColumn(
          3,
          OpenGl_Vec4(
            -1.0f + (float)aViewSizeX / aTileSize.x() - 2.0f * aTileOffset.x() / aTileSize.x(),
            -1.0f + (float)aViewSizeY / aTileSize.y() - 2.0f * aTileOffset.y() / aTileSize.y(),
            0.0f,
            1.0f));
      }
    }
    else
    {
      aWorldView.SetDiagonal(OpenGl_Vec4(2.0f / myViewWidth, 2.0f / myViewHeight, 1.0f, 1.0f));
      if (myType == Graphic3d_TOB_GRADIENT)
      {
        aWorldView.SetColumn(3, OpenGl_Vec4(-1.0f, -1.0f, 0.0f, 1.0f));
      }
    }
  }

  aCtx->ProjectionState.Push();
  aCtx->WorldViewState.Push();
  aCtx->ProjectionState.SetCurrent(aProjection);
  aCtx->WorldViewState.SetCurrent(aWorldView);
  aCtx->ApplyProjectionMatrix();
  aCtx->ApplyModelViewMatrix();

  OpenGl_PrimitiveArray::Render(theWorkspace);

  aCtx->ProjectionState.Pop();
  aCtx->WorldViewState.Pop();
  aCtx->ApplyProjectionMatrix();
}
