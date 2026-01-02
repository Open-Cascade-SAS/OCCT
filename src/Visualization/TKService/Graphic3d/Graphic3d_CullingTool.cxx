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

//=================================================================================================

Graphic3d_CullingTool::Graphic3d_CullingTool()
    : myClipVerts(0, Graphic3d_Camera::FrustumVerticesNB),
      myIsProjectionParallel(true),
      myCamScale(1.0),
      myPixelSize(1.0)
{}

//=================================================================================================

void Graphic3d_CullingTool::SetViewVolume(const occ::handle<Graphic3d_Camera>& theCamera,
                                          const NCollection_Mat4<double>&      theModelWorld)
{
  const bool hasModelTrsf = !theModelWorld.IsIdentity();
  if (!myWorldViewProjState.IsChanged(theCamera->WorldViewProjState()) && !hasModelTrsf)
  {
    return;
  }

  myIsProjectionParallel = theCamera->IsOrthographic();
  const gp_Dir aCamDir   = theCamera->Direction();

  myCamera             = theCamera;
  myProjectionMat      = theCamera->ProjectionMatrix();
  myWorldViewMat       = theCamera->OrientationMatrix();
  myWorldViewProjState = theCamera->WorldViewProjState();
  myCamEye.SetValues(theCamera->Eye().X(), theCamera->Eye().Y(), theCamera->Eye().Z());
  myCamDir.SetValues(aCamDir.X(), aCamDir.Y(), aCamDir.Z());
  if (hasModelTrsf)
  {
    NCollection_Mat4<double> aModelInv;
    theModelWorld.Inverted(aModelInv);
    myCamEye = (aModelInv * NCollection_Vec4<double>(myCamEye, 1.0)).xyz();
    myCamDir = (aModelInv * NCollection_Vec4<double>(myCamDir, 0.0)).xyz();
  }
  myCamScale = theCamera->IsOrthographic()
                 ? theCamera->Scale()
                 : 2.0
                     * std::tan(theCamera->FOVy() * M_PI
                                / 360.0); // same as theCamera->Scale()/theCamera->Distance()

  // Compute frustum points
  theCamera->FrustumPoints(myClipVerts, theModelWorld);

  // Compute frustum planes
  // Vertices go in order:
  // 0, 2, 1
  const int aLookup1[] = {0, 1, 0};
  const int aLookup2[] = {0, 0, 1};
  int       aShifts[]  = {0, 0, 0};

  // Planes go in order:
  // LEFT, RIGHT, BOTTOM, TOP, NEAR, FAR
  for (int aFaceIdx = 0; aFaceIdx < 3; ++aFaceIdx)
  {
    for (int i = 0; i < 2; ++i)
    {
      NCollection_Vec3<double> aPlanePnts[3];
      for (int aPntIter = 0; aPntIter < 3; ++aPntIter)
      {
        aShifts[aFaceIdx]           = i;
        aShifts[(aFaceIdx + 1) % 3] = aLookup1[aPntIter];
        aShifts[(aFaceIdx + 2) % 3] = aLookup2[aPntIter];

        aPlanePnts[aPntIter] = myClipVerts[aShifts[0] * 2 * 2 + aShifts[1] * 2 + aShifts[2]];
      }

      myClipPlanes[aFaceIdx * 2 + i].Origin = aPlanePnts[0];
      myClipPlanes[aFaceIdx * 2 + i].Normal =
        NCollection_Vec3<double>::Cross(aPlanePnts[1] - aPlanePnts[0],
                                        aPlanePnts[2] - aPlanePnts[0])
          .Normalized()
        * (i == 0 ? -1.f : 1.f);
    }
  }
}

//=================================================================================================

void Graphic3d_CullingTool::SetViewportSize(int    theViewportWidth,
                                            int    theViewportHeight,
                                            double theResolutionRatio)
{
  myViewportHeight = theViewportHeight > 0 ? theViewportHeight : 1;
  myViewportWidth  = theViewportWidth > 0 ? theViewportWidth : 1;
  myPixelSize =
    std::max(theResolutionRatio / myViewportHeight, theResolutionRatio / myViewportWidth);
}

//=================================================================================================

double Graphic3d_CullingTool::SignedPlanePointDistance(const NCollection_Vec4<double>& theNormal,
                                                       const NCollection_Vec4<double>& thePnt)
{
  const double aNormLength = std::sqrt(theNormal.x() * theNormal.x() + theNormal.y() * theNormal.y()
                                       + theNormal.z() * theNormal.z());

  if (aNormLength < gp::Resolution())
    return 0.0;

  const double anInvNormLength = 1.0 / aNormLength;
  const double aD              = theNormal.w() * anInvNormLength;
  const double anA             = theNormal.x() * anInvNormLength;
  const double aB              = theNormal.y() * anInvNormLength;
  const double aC              = theNormal.z() * anInvNormLength;
  return aD + (anA * thePnt.x() + aB * thePnt.y() + aC * thePnt.z());
}

//=================================================================================================

void Graphic3d_CullingTool::SetCullingDistance(CullingContext& theCtx, double theDistance) const
{
  theCtx.DistCull = -1.0;
  if (!myIsProjectionParallel)
  {
    theCtx.DistCull = theDistance > 0.0 && !Precision::IsInfinite(theDistance) ? theDistance : -1.0;
  }
}

//=================================================================================================

void Graphic3d_CullingTool::SetCullingSize(CullingContext& theCtx, double theSize) const
{
  theCtx.SizeCull2 = -1.0;
  if (theSize > 0.0 && !Precision::IsInfinite(theSize))
  {
    theCtx.SizeCull2 = myPixelSize * theSize;
    theCtx.SizeCull2 *= myCamScale;
    theCtx.SizeCull2 *= theCtx.SizeCull2;
  }
}

//=================================================================================================

void Graphic3d_CullingTool::CacheClipPtsProjections()
{
  // project frustum onto its own normals
  const int anIncFactor = myIsProjectionParallel ? 2 : 1;
  for (int aPlaneIter = 0; aPlaneIter < PlanesNB - 1; aPlaneIter += anIncFactor)
  {
    double aMaxProj = -std::numeric_limits<double>::max();
    double aMinProj = std::numeric_limits<double>::max();
    for (int aCornerIter = 0; aCornerIter < Graphic3d_Camera::FrustumVerticesNB; ++aCornerIter)
    {
      double aProjection = myClipVerts[aCornerIter].Dot(myClipPlanes[aPlaneIter].Normal);
      aMaxProj           = std::max(aProjection, aMaxProj);
      aMinProj           = std::min(aProjection, aMinProj);
    }
    myMaxClipProjectionPts[aPlaneIter] = aMaxProj;
    myMinClipProjectionPts[aPlaneIter] = aMinProj;
  }

  // project frustum onto main axes
  NCollection_Vec3<double> anAxes[] = {NCollection_Vec3<double>(1.0, 0.0, 0.0),
                                       NCollection_Vec3<double>(0.0, 1.0, 0.0),
                                       NCollection_Vec3<double>(0.0, 0.0, 1.0)};
  for (int aDim = 0; aDim < 3; ++aDim)
  {
    double aMaxProj = -std::numeric_limits<double>::max();
    double aMinProj = std::numeric_limits<double>::max();
    for (int aCornerIter = 0; aCornerIter < Graphic3d_Camera::FrustumVerticesNB; ++aCornerIter)
    {
      double aProjection = myClipVerts[aCornerIter].Dot(anAxes[aDim]);
      aMaxProj           = std::max(aProjection, aMaxProj);
      aMinProj           = std::min(aProjection, aMinProj);
    }
    myMaxOrthoProjectionPts[aDim] = aMaxProj;
    myMinOrthoProjectionPts[aDim] = aMinProj;
  }
}
