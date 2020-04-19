// Created on: 2013-12-25
// Created by: Varvara POSKONINA
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <Graphic3d_CullingTool.hxx>

#include <Precision.hxx>

#include <limits>

// =======================================================================
// function : Graphic3d_CullingTool
// purpose  :
// =======================================================================
Graphic3d_CullingTool::Graphic3d_CullingTool()
: myClipVerts (0, Graphic3d_Camera::FrustumVerticesNB),
  myIsProjectionParallel (Standard_True),
  myCamScale (1.0),
  myPixelSize (1.0)
{
  //
}

// =======================================================================
// function : SetViewVolume
// purpose  :
// =======================================================================
void Graphic3d_CullingTool::SetViewVolume (const Handle(Graphic3d_Camera)& theCamera,
                                           const Graphic3d_Mat4d& theModelWorld)
{
  const bool hasModelTrsf = !theModelWorld.IsIdentity();
  if (!myWorldViewProjState.IsChanged (theCamera->WorldViewProjState())
   && !hasModelTrsf)
  {
    return;
  }

  myIsProjectionParallel = theCamera->IsOrthographic();
  const gp_Dir aCamDir = theCamera->Direction();

  myCamera             = theCamera;
  myProjectionMat      = theCamera->ProjectionMatrix();
  myWorldViewMat       = theCamera->OrientationMatrix();
  myWorldViewProjState = theCamera->WorldViewProjState();
  myCamEye.SetValues (theCamera->Eye().X(), theCamera->Eye().Y(), theCamera->Eye().Z());
  myCamDir.SetValues (aCamDir.X(), aCamDir.Y(), aCamDir.Z());
  if (hasModelTrsf)
  {
    Graphic3d_Mat4d aModelInv;
    theModelWorld.Inverted (aModelInv);
    myCamEye = (aModelInv * Graphic3d_Vec4d (myCamEye, 1.0)).xyz();
    myCamDir = (aModelInv * Graphic3d_Vec4d (myCamDir, 0.0)).xyz();
  }
  myCamScale = theCamera->IsOrthographic()
             ? theCamera->Scale()
             : 2.0 * Tan (theCamera->FOVy() * M_PI / 360.0); // same as theCamera->Scale()/theCamera->Distance()

  // Compute frustum points
  theCamera->FrustumPoints (myClipVerts, theModelWorld);

  // Compute frustum planes
  // Vertices go in order:
  // 0, 2, 1
  const Standard_Integer aLookup1[] = { 0, 1, 0 };
  const Standard_Integer aLookup2[] = { 0, 0, 1 };
  Standard_Integer aShifts[]        = { 0, 0, 0 };

  // Planes go in order:
  // LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR
  for (Standard_Integer aFaceIdx = 0; aFaceIdx < 3; ++aFaceIdx)
  {
    for (Standard_Integer i = 0; i < 2; ++i)
    {
      Graphic3d_Vec3d aPlanePnts[3];
      for (Standard_Integer aPntIter = 0; aPntIter < 3; ++aPntIter)
      {
        aShifts[aFaceIdx] = i;
        aShifts[(aFaceIdx + 1) % 3] = aLookup1[aPntIter];
        aShifts[(aFaceIdx + 2) % 3] = aLookup2[aPntIter];
        
        aPlanePnts[aPntIter] = myClipVerts[aShifts[0] * 2 * 2 + aShifts[1] * 2 + aShifts[2]];
      }
      
      myClipPlanes[aFaceIdx * 2 + i].Origin = aPlanePnts[0];
      myClipPlanes[aFaceIdx * 2 + i].Normal =
        Graphic3d_Vec3d::Cross (aPlanePnts[1] - aPlanePnts[0],
                                aPlanePnts[2] - aPlanePnts[0]).Normalized() * (i == 0 ? -1.f : 1.f);
    }
  }
}

// =======================================================================
// function : SetViewportSize
// purpose  :
// =======================================================================
void Graphic3d_CullingTool::SetViewportSize (Standard_Integer theViewportWidth,
                                             Standard_Integer theViewportHeight,
                                             Standard_Real theResolutionRatio)
{
  myViewportHeight = theViewportHeight > 0 ? theViewportHeight : 1;
  myViewportWidth  = theViewportWidth  > 0 ? theViewportWidth  : 1;
  myPixelSize = Max (theResolutionRatio / myViewportHeight,
                     theResolutionRatio / myViewportWidth);
}

// =======================================================================
// function : SignedPlanePointDistance
// purpose  :
// =======================================================================
Standard_Real Graphic3d_CullingTool::SignedPlanePointDistance (const Graphic3d_Vec4d& theNormal,
                                                               const Graphic3d_Vec4d& thePnt)
{
  const Standard_Real aNormLength = std::sqrt (theNormal.x() * theNormal.x()
                                             + theNormal.y() * theNormal.y()
                                             + theNormal.z() * theNormal.z());

  if (aNormLength < gp::Resolution())
    return 0.0;

  const Standard_Real anInvNormLength = 1.0 / aNormLength;
  const Standard_Real aD  = theNormal.w() * anInvNormLength;
  const Standard_Real anA = theNormal.x() * anInvNormLength;
  const Standard_Real aB  = theNormal.y() * anInvNormLength;
  const Standard_Real aC  = theNormal.z() * anInvNormLength;
  return aD + (anA * thePnt.x() + aB * thePnt.y() + aC * thePnt.z());
}

// =======================================================================
// function : SetCullingDistance
// purpose  :
// =======================================================================
void Graphic3d_CullingTool::SetCullingDistance (CullingContext& theCtx,
                                                Standard_Real theDistance) const
{
  theCtx.DistCull = -1.0;
  if (!myIsProjectionParallel)
  {
    theCtx.DistCull = theDistance > 0.0 && !Precision::IsInfinite (theDistance)
                    ? theDistance
                    : -1.0;
  }
}

// =======================================================================
// function : SetCullingSize
// purpose  :
// =======================================================================
void Graphic3d_CullingTool::SetCullingSize (CullingContext& theCtx,
                                            Standard_Real theSize) const
{
  theCtx.SizeCull2 = -1.0;
  if (theSize > 0.0 && !Precision::IsInfinite (theSize))
  {
    theCtx.SizeCull2 = myPixelSize * theSize;
    theCtx.SizeCull2 *= myCamScale;
    theCtx.SizeCull2 *= theCtx.SizeCull2;
  }
}

// =======================================================================
// function : CacheClipPtsProjections
// purpose  :
// =======================================================================
void Graphic3d_CullingTool::CacheClipPtsProjections()
{
  // project frustum onto its own normals
  const Standard_Integer anIncFactor = myIsProjectionParallel ? 2 : 1;
  for (Standard_Integer aPlaneIter = 0; aPlaneIter < PlanesNB - 1; aPlaneIter += anIncFactor)
  {
    Standard_Real aMaxProj = -std::numeric_limits<Standard_Real>::max();
    Standard_Real aMinProj =  std::numeric_limits<Standard_Real>::max();
    for (Standard_Integer aCornerIter = 0; aCornerIter < Graphic3d_Camera::FrustumVerticesNB; ++aCornerIter)
    {
      Standard_Real aProjection = myClipVerts[aCornerIter].Dot (myClipPlanes[aPlaneIter].Normal);
      aMaxProj = Max (aProjection, aMaxProj);
      aMinProj = Min (aProjection, aMinProj);
    }
    myMaxClipProjectionPts[aPlaneIter] = aMaxProj;
    myMinClipProjectionPts[aPlaneIter] = aMinProj;
  }

  // project frustum onto main axes
  Graphic3d_Vec3d anAxes[] = { Graphic3d_Vec3d (1.0, 0.0, 0.0),
                               Graphic3d_Vec3d (0.0, 1.0, 0.0),
                               Graphic3d_Vec3d (0.0, 0.0, 1.0) };
  for (Standard_Integer aDim = 0; aDim < 3; ++aDim)
  {
    Standard_Real aMaxProj = -std::numeric_limits<Standard_Real>::max();
    Standard_Real aMinProj =  std::numeric_limits<Standard_Real>::max();
    for (Standard_Integer aCornerIter = 0; aCornerIter < Graphic3d_Camera::FrustumVerticesNB; ++aCornerIter)
    {
      Standard_Real aProjection = myClipVerts[aCornerIter].Dot (anAxes[aDim]);
      aMaxProj = Max (aProjection, aMaxProj);
      aMinProj = Min (aProjection, aMinProj);
    }
    myMaxOrthoProjectionPts[aDim] = aMaxProj;
    myMinOrthoProjectionPts[aDim] = aMinProj;
  }
}
