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

#include <limits>

#include <OpenGl_BVHTreeSelector.hxx>
#include <OpenGl_BVHClipPrimitiveSet.hxx>
#include <Graphic3d_GraphicDriver.hxx>

// =======================================================================
// function : OpenGl_BVHTreeSelector
// purpose  :
// =======================================================================
OpenGl_BVHTreeSelector::OpenGl_BVHTreeSelector()
: myIsProjectionParallel (Standard_True)
{
  //
}

// =======================================================================
// function : SetViewVolume
// purpose  : Retrieves view volume's planes equations and its vertices from projection and world-view matrices.
// =======================================================================
void OpenGl_BVHTreeSelector::SetViewVolume (const Handle(Graphic3d_Camera)& theCamera)
{
  if (!myWorldViewProjState.IsChanged (theCamera->WorldViewProjState()))
    return;

  myIsProjectionParallel = theCamera->IsOrthographic();

  myCamera             = theCamera;
  myProjectionMat      = theCamera->ProjectionMatrix();
  myWorldViewMat       = theCamera->OrientationMatrix();
  myWorldViewProjState = theCamera->WorldViewProjState();

  Standard_Real nLeft = 0.0, nRight = 0.0, nTop = 0.0, nBottom = 0.0;
  Standard_Real fLeft = 0.0, fRight = 0.0, fTop = 0.0, fBottom = 0.0;
  Standard_Real aNear = 0.0, aFar   = 0.0;
  if (!myIsProjectionParallel)
  {
    // handle perspective projection
    aNear   = myProjectionMat.GetValue (2, 3) / (- 1.0 + myProjectionMat.GetValue (2, 2));
    aFar    = myProjectionMat.GetValue (2, 3) / (  1.0 + myProjectionMat.GetValue (2, 2));
    // Near plane
    nLeft   = aNear * (myProjectionMat.GetValue (0, 2) - 1.0) / myProjectionMat.GetValue (0, 0);
    nRight  = aNear * (myProjectionMat.GetValue (0, 2) + 1.0) / myProjectionMat.GetValue (0, 0);
    nTop    = aNear * (myProjectionMat.GetValue (1, 2) + 1.0) / myProjectionMat.GetValue (1, 1);
    nBottom = aNear * (myProjectionMat.GetValue (1, 2) - 1.0) / myProjectionMat.GetValue (1, 1);
    // Far plane
    fLeft   = aFar  * (myProjectionMat.GetValue (0, 2) - 1.0) / myProjectionMat.GetValue (0, 0);
    fRight  = aFar  * (myProjectionMat.GetValue (0, 2) + 1.0) / myProjectionMat.GetValue (0, 0);
    fTop    = aFar  * (myProjectionMat.GetValue (1, 2) + 1.0) / myProjectionMat.GetValue (1, 1);
    fBottom = aFar  * (myProjectionMat.GetValue (1, 2) - 1.0) / myProjectionMat.GetValue (1, 1);
  }
  else
  {
    // handle orthographic projection
    aNear   = (1.0 / myProjectionMat.GetValue (2, 2)) * (myProjectionMat.GetValue (2, 3) + 1.0);
    aFar    = (1.0 / myProjectionMat.GetValue (2, 2)) * (myProjectionMat.GetValue (2, 3) - 1.0);
    // Near plane
    nLeft   = ( 1.0 + myProjectionMat.GetValue (0, 3)) / (-myProjectionMat.GetValue (0, 0));
    fLeft   = nLeft;
    nRight  = ( 1.0 - myProjectionMat.GetValue (0, 3)) /   myProjectionMat.GetValue (0, 0);
    fRight  = nRight;
    nTop    = ( 1.0 - myProjectionMat.GetValue (1, 3)) /   myProjectionMat.GetValue (1, 1);
    fTop    = nTop;
    nBottom = (-1.0 - myProjectionMat.GetValue (1, 3)) /   myProjectionMat.GetValue (1, 1);
    fBottom = nBottom;
  }

  OpenGl_Vec4d aLeftTopNear     (nLeft,  nTop,    -aNear, 1.0), aRightBottomFar (fRight, fBottom, -aFar, 1.0);
  OpenGl_Vec4d aLeftBottomNear  (nLeft,  nBottom, -aNear, 1.0), aRightTopFar    (fRight, fTop,    -aFar, 1.0);
  OpenGl_Vec4d aRightBottomNear (nRight, nBottom, -aNear, 1.0), aLeftTopFar     (fLeft,  fTop,    -aFar, 1.0);
  OpenGl_Vec4d aRightTopNear    (nRight, nTop,    -aNear, 1.0), aLeftBottomFar  (fLeft,  fBottom, -aFar, 1.0);

  const OpenGl_Mat4d aViewProj = myWorldViewMat * myProjectionMat;
  OpenGl_Mat4d anInvWorldView;
  myWorldViewMat.Inverted (anInvWorldView);

  myClipVerts[ClipVert_LeftTopNear]     = anInvWorldView * aLeftTopNear;
  myClipVerts[ClipVert_RightBottomFar]  = anInvWorldView * aRightBottomFar;
  myClipVerts[ClipVert_LeftBottomNear]  = anInvWorldView * aLeftBottomNear;
  myClipVerts[ClipVert_RightTopFar]     = anInvWorldView * aRightTopFar;
  myClipVerts[ClipVert_RightBottomNear] = anInvWorldView * aRightBottomNear;
  myClipVerts[ClipVert_LeftTopFar]      = anInvWorldView * aLeftTopFar;
  myClipVerts[ClipVert_RightTopNear]    = anInvWorldView * aRightTopNear;
  myClipVerts[ClipVert_LeftBottomFar]   = anInvWorldView * aLeftBottomFar;

  // UNNORMALIZED!
  myClipPlanes[Plane_Left]   = aViewProj.GetRow (3) + aViewProj.GetRow (0);
  myClipPlanes[Plane_Right]  = aViewProj.GetRow (3) - aViewProj.GetRow (0);
  myClipPlanes[Plane_Top]    = aViewProj.GetRow (3) - aViewProj.GetRow (1);
  myClipPlanes[Plane_Bottom] = aViewProj.GetRow (3) + aViewProj.GetRow (1);
  myClipPlanes[Plane_Near]   = aViewProj.GetRow (3) + aViewProj.GetRow (2);
  myClipPlanes[Plane_Far]    = aViewProj.GetRow (3) - aViewProj.GetRow (2);

  gp_Pnt aPtCenter = theCamera->Center();
  OpenGl_Vec4d aCenter (aPtCenter.X(), aPtCenter.Y(), aPtCenter.Z(), 1.0);

  for (Standard_Integer aPlaneIter = 0; aPlaneIter < PlanesNB; ++aPlaneIter)
  {
    OpenGl_Vec4d anEq = myClipPlanes[aPlaneIter];
    if (SignedPlanePointDistance (anEq, aCenter) > 0)
    {
      anEq *= -1.0;
      myClipPlanes[aPlaneIter] = anEq;
    }
  }
}

// =======================================================================
// function : SetViewportSize
// purpose  :
// =======================================================================
void OpenGl_BVHTreeSelector::SetViewportSize (const Standard_Integer theViewportWidth,
                                              const Standard_Integer theViewportHeight)
{
  myViewportHeight = theViewportHeight;
  myViewportWidth = theViewportWidth;
}

// =======================================================================
// function : SignedPlanePointDistance
// purpose  :
// =======================================================================
Standard_Real OpenGl_BVHTreeSelector::SignedPlanePointDistance (const OpenGl_Vec4d& theNormal,
                                                                const OpenGl_Vec4d& thePnt)
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
// function : CacheClipPtsProjections
// purpose  : Caches view volume's vertices projections along its normals and AABBs dimensions
//            Must be called at the beginning of each BVH tree traverse loop
// =======================================================================
void OpenGl_BVHTreeSelector::CacheClipPtsProjections()
{
  const Standard_Integer anIncFactor = myIsProjectionParallel ? 2 : 1;
  for (Standard_Integer aPlaneIter = 0; aPlaneIter < 5; aPlaneIter += anIncFactor)
  {
    const OpenGl_Vec4d aPlane = myClipPlanes[aPlaneIter];
    Standard_Real aMaxProj = -std::numeric_limits<Standard_Real>::max();
    Standard_Real aMinProj =  std::numeric_limits<Standard_Real>::max();
    for (Standard_Integer aCornerIter = 0; aCornerIter < ClipVerticesNB; ++aCornerIter)
    {
      Standard_Real aProjection = aPlane.x() * myClipVerts[aCornerIter].x()
                                + aPlane.y() * myClipVerts[aCornerIter].y()
                                + aPlane.z() * myClipVerts[aCornerIter].z();
      aMaxProj = Max (aProjection, aMaxProj);
      aMinProj = Min (aProjection, aMinProj);
    }
    myMaxClipProjectionPts[aPlaneIter] = aMaxProj;
    myMinClipProjectionPts[aPlaneIter] = aMinProj;
  }

  for (Standard_Integer aDim = 0; aDim < 3; ++aDim)
  {
    Standard_Real aMaxProj = -std::numeric_limits<Standard_Real>::max();
    Standard_Real aMinProj =  std::numeric_limits<Standard_Real>::max();
    for (Standard_Integer aCornerIter = 0; aCornerIter < ClipVerticesNB; ++aCornerIter)
    {
      Standard_Real aProjection = aDim == 0
                                ? myClipVerts[aCornerIter].x()
                                : (aDim == 1
                                 ? myClipVerts[aCornerIter].y()
                                 : myClipVerts[aCornerIter].z());
      aMaxProj = Max (aProjection, aMaxProj);
      aMinProj = Min (aProjection, aMinProj);
    }
    myMaxOrthoProjectionPts[aDim] = aMaxProj;
    myMinOrthoProjectionPts[aDim] = aMinProj;
  }
}

// =======================================================================
// function : Intersect
// purpose  : Detects if AABB overlaps view volume using separating axis theorem (SAT)
// =======================================================================
Standard_Boolean OpenGl_BVHTreeSelector::Intersect (const OpenGl_Vec3d& theMinPt,
                                                    const OpenGl_Vec3d& theMaxPt) const
{
  //     E1
  //    |_ E0
  //   /
  //    E2

  // E0 test
  if (theMinPt.x() > myMaxOrthoProjectionPts[0]
   || theMaxPt.x() < myMinOrthoProjectionPts[0])
  {
    return Standard_False;
  }

  // E1 test
  if (theMinPt.y() > myMaxOrthoProjectionPts[1]
   || theMaxPt.y() < myMinOrthoProjectionPts[1])
  {
    return Standard_False;
  }

  // E2 test
  if (theMinPt.z() > myMaxOrthoProjectionPts[2]
   || theMaxPt.z() < myMinOrthoProjectionPts[2])
  {
    return Standard_False;
  }

  Standard_Real aBoxProjMax = 0.0, aBoxProjMin = 0.0;
  const Standard_Integer anIncFactor = myIsProjectionParallel ? 2 : 1;
  for (Standard_Integer aPlaneIter = 0; aPlaneIter < 5; aPlaneIter += anIncFactor)
  {
    OpenGl_Vec4d aPlane = myClipPlanes[aPlaneIter];
    aBoxProjMax = (aPlane.x() > 0.0 ? (aPlane.x() * theMaxPt.x()) : aPlane.x() * theMinPt.x())
                + (aPlane.y() > 0.0 ? (aPlane.y() * theMaxPt.y()) : aPlane.y() * theMinPt.y())
                + (aPlane.z() > 0.0 ? (aPlane.z() * theMaxPt.z()) : aPlane.z() * theMinPt.z());
    if (aBoxProjMax > myMinClipProjectionPts[aPlaneIter]
     && aBoxProjMax < myMaxClipProjectionPts[aPlaneIter])
    {
      continue;
    }

    aBoxProjMin = (aPlane.x() < 0.0 ? aPlane.x() * theMaxPt.x() : aPlane.x() * theMinPt.x())
                + (aPlane.y() < 0.0 ? aPlane.y() * theMaxPt.y() : aPlane.y() * theMinPt.y())
                + (aPlane.z() < 0.0 ? aPlane.z() * theMaxPt.z() : aPlane.z() * theMinPt.z());
    if (aBoxProjMin > myMaxClipProjectionPts[aPlaneIter]
     || aBoxProjMax < myMinClipProjectionPts[aPlaneIter])
    {
      return Standard_False;
    }
  }

  return Standard_True;
}
