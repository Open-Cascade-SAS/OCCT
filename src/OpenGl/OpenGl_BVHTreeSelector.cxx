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

#include <OpenGl_BVHTreeSelector.hxx>
#include <OpenGl_BVHClipPrimitiveSet.hxx>

#include <vector>
#include <algorithm>

// =======================================================================
// function : DotProduct
// purpose  : Calculates a dot product of 4-dimensional vectors in homogeneous coordinates
// =======================================================================
static Standard_ShortReal DotProduct (const OpenGl_Vec4& theA,
                                      const OpenGl_Vec4& theB)
{
  return theA.x() * theB.x() + theA.y() * theB.y() + theA.z() * theB.z();
}

// =======================================================================
// function : BinarySign
// purpose  :
// =======================================================================
static OpenGl_Vec4 BinarySign (const OpenGl_Vec4& theVec)
{
  return OpenGl_Vec4 (theVec.x() > 0.0f ? 1.0f : 0.0f,
                      theVec.y() > 0.0f ? 1.0f : 0.0f,
                      theVec.z() > 0.0f ? 1.0f : 0.0f,
                      theVec.w() > 0.0f ? 1.0f : 0.0f);
}

// =======================================================================
// function : InversedBinarySign
// purpose  :
// =======================================================================
static OpenGl_Vec4 InversedBinarySign (const OpenGl_Vec4& theVec)
{
  return OpenGl_Vec4 (theVec.x() > 0.0f ? 0.0f : 1.0f,
                      theVec.y() > 0.0f ? 0.0f : 1.0f,
                      theVec.z() > 0.0f ? 0.0f : 1.0f,
                      theVec.w() > 0.0f ? 0.0f : 1.0f);
}

// =======================================================================
// function : OpenGl_BVHTreeSelector
// purpose  :
// =======================================================================
OpenGl_BVHTreeSelector::OpenGl_BVHTreeSelector()
: myIsProjectionParallel (Standard_True),
  myProjectionState      (0),
  myModelViewState       (0)
{
  //
}

// =======================================================================
// function : SetClipVolume
// purpose  : Retrieves view volume's planes equations and its vertices from projection and modelview matrices.
// =======================================================================
void OpenGl_BVHTreeSelector::SetViewVolume (const Handle(Graphic3d_Camera)& theCamera)
{
  myIsProjectionParallel = theCamera->IsOrthographic();
  const OpenGl_Mat4& aProjMat  = theCamera->ProjectionMatrixF();
  const OpenGl_Mat4& aModelMat = theCamera->OrientationMatrixF();

  Standard_ShortReal nLeft = 0.0f, nRight = 0.0f, nTop = 0.0f, nBottom = 0.0f;
  Standard_ShortReal fLeft = 0.0f, fRight = 0.0f, fTop = 0.0f, fBottom = 0.0f;
  Standard_ShortReal aNear = 0.0f, aFar   = 0.0f;
  if (!myIsProjectionParallel)
  {
    // handle perspective projection
    aNear   = aProjMat.GetValue (2, 3) / (- 1.0f + aProjMat.GetValue (2, 2));
    aFar    = aProjMat.GetValue (2, 3) / (  1.0f + aProjMat.GetValue (2, 2));
    // Near plane
    nLeft   = aNear * (aProjMat.GetValue (0, 2) - 1.0f) / aProjMat.GetValue (0, 0);
    nRight  = aNear * (aProjMat.GetValue (0, 2) + 1.0f) / aProjMat.GetValue (0, 0);
    nTop    = aNear * (aProjMat.GetValue (1, 2) + 1.0f) / aProjMat.GetValue (1, 1);
    nBottom = aNear * (aProjMat.GetValue (1, 2) - 1.0f) / aProjMat.GetValue (1, 1);
    // Far plane
    fLeft   = aFar  * (aProjMat.GetValue (0, 2) - 1.0f) / aProjMat.GetValue (0, 0);
    fRight  = aFar  * (aProjMat.GetValue (0, 2) + 1.0f) / aProjMat.GetValue (0, 0);
    fTop    = aFar  * (aProjMat.GetValue (1, 2) + 1.0f) / aProjMat.GetValue (1, 1);
    fBottom = aFar  * (aProjMat.GetValue (1, 2) - 1.0f) / aProjMat.GetValue (1, 1);
  }
  else
  {
    // handle orthographic projection
    aNear   = (1.0f / aProjMat.GetValue (2, 2)) * (aProjMat.GetValue (2, 3) + 1.0f);
    aFar    = (1.0f / aProjMat.GetValue (2, 2)) * (aProjMat.GetValue (2, 3) - 1.0f);
    // Near plane
    nLeft   = ( 1.0f + aProjMat.GetValue (0, 3)) / (-aProjMat.GetValue (0, 0));
    fLeft   = nLeft;
    nRight  = ( 1.0f - aProjMat.GetValue (0, 3)) /   aProjMat.GetValue (0, 0);
    fRight  = nRight;
    nTop    = ( 1.0f - aProjMat.GetValue (1, 3)) /   aProjMat.GetValue (1, 1);
    fTop    = nTop;
    nBottom = (-1.0f - aProjMat.GetValue (1, 3)) /   aProjMat.GetValue (1, 1);
    fBottom = nBottom;
  }

  OpenGl_Vec4 aLeftTopNear     (nLeft,  nTop,    -aNear, 1.0f), aRightBottomFar (fRight, fBottom, -aFar, 1.0f);
  OpenGl_Vec4 aLeftBottomNear  (nLeft,  nBottom, -aNear, 1.0f), aRightTopFar    (fRight, fTop,    -aFar, 1.0f);
  OpenGl_Vec4 aRightBottomNear (nRight, nBottom, -aNear, 1.0f), aLeftTopFar     (fLeft,  fTop,    -aFar, 1.0f);
  OpenGl_Vec4 aRightTopNear    (nRight, nTop,    -aNear, 1.0f), aLeftBottomFar  (fLeft,  fBottom, -aFar, 1.0f);

  const OpenGl_Mat4 aViewProj = aModelMat * aProjMat;
  OpenGl_Mat4 anInvModelView;
  aModelMat.Inverted(anInvModelView);

  myClipVerts[ClipVert_LeftTopNear]     = anInvModelView * aLeftTopNear;
  myClipVerts[ClipVert_RightBottomFar]  = anInvModelView * aRightBottomFar;
  myClipVerts[ClipVert_LeftBottomNear]  = anInvModelView * aLeftBottomNear;
  myClipVerts[ClipVert_RightTopFar]     = anInvModelView * aRightTopFar;
  myClipVerts[ClipVert_RightBottomNear] = anInvModelView * aRightBottomNear;
  myClipVerts[ClipVert_LeftTopFar]      = anInvModelView * aLeftTopFar;
  myClipVerts[ClipVert_RightTopNear]    = anInvModelView * aRightTopNear;
  myClipVerts[ClipVert_LeftBottomFar]   = anInvModelView * aLeftBottomFar;

  // UNNORMALIZED!
  myClipPlanes[Plane_Left]   = aViewProj.GetRow (3) + aViewProj.GetRow (0);
  myClipPlanes[Plane_Right]  = aViewProj.GetRow (3) - aViewProj.GetRow (0);
  myClipPlanes[Plane_Top]    = aViewProj.GetRow (3) - aViewProj.GetRow (1);
  myClipPlanes[Plane_Bottom] = aViewProj.GetRow (3) + aViewProj.GetRow (1);
  myClipPlanes[Plane_Near]   = aViewProj.GetRow (3) + aViewProj.GetRow (2);
  myClipPlanes[Plane_Far]    = aViewProj.GetRow (3) - aViewProj.GetRow (2);

  gp_Pnt aPtCenter = theCamera->Center();
  OpenGl_Vec4 aCenter (static_cast<Standard_ShortReal> (aPtCenter.X()),
                       static_cast<Standard_ShortReal> (aPtCenter.Y()),
                       static_cast<Standard_ShortReal> (aPtCenter.Z()),
                       1.0f);

  for (Standard_Integer aPlaneIter = 0; aPlaneIter < PlanesNB; ++aPlaneIter)
  {
    OpenGl_Vec4 anEq = myClipPlanes[aPlaneIter];
    if (SignedPlanePointDistance (anEq, aCenter) > 0)
    {
      anEq *= -1.0f;
      myClipPlanes[aPlaneIter] = anEq;
    }
  }
}

// =======================================================================
// function : SignedPlanePointDistance
// purpose  :
// =======================================================================
Standard_ShortReal OpenGl_BVHTreeSelector::SignedPlanePointDistance (const OpenGl_Vec4& theNormal,
                                                                     const OpenGl_Vec4& thePnt)
{
  const Standard_ShortReal aNormLength = std::sqrt (theNormal.x() * theNormal.x()
                                                  + theNormal.y() * theNormal.y()
                                                  + theNormal.z() * theNormal.z());

  if (aNormLength < FLT_EPSILON)
    return 0.0f;

  const Standard_ShortReal anInvNormLength = 1.0f / aNormLength;
  const Standard_ShortReal aD  = theNormal.w() * anInvNormLength;
  const Standard_ShortReal anA = theNormal.x() * anInvNormLength;
  const Standard_ShortReal aB  = theNormal.y() * anInvNormLength;
  const Standard_ShortReal aC  = theNormal.z() * anInvNormLength;
  return aD + (anA * thePnt.x() + aB * thePnt.y() + aC * thePnt.z());
}

// =======================================================================
// function : CacheClipPtsProjections
// purpose  : Caches view volume's vertices projections along its normals and AABBs dimensions
//            Must be called at the beginning of each BVH tree traverse loop
// =======================================================================
void OpenGl_BVHTreeSelector::CacheClipPtsProjections()
{
  Standard_ShortReal aProjectedVerts[ClipVerticesNB];
  for (Standard_Integer aPlaneIter = 0; aPlaneIter < PlanesNB; ++aPlaneIter)
  {
    const OpenGl_Vec4 aPlane = myClipPlanes[aPlaneIter];
    for (Standard_Integer aCornerIter = 0; aCornerIter < ClipVerticesNB; ++aCornerIter)
    {
      Standard_ShortReal aProjection = DotProduct (aPlane, myClipVerts[aCornerIter]);
      aProjectedVerts[aCornerIter] = aProjection;
    }
    myMaxClipProjectionPts[aPlaneIter] = *std::max_element (aProjectedVerts, aProjectedVerts + ClipVerticesNB);
    myMinClipProjectionPts[aPlaneIter] = *std::min_element (aProjectedVerts, aProjectedVerts + ClipVerticesNB);
  }

  OpenGl_Vec4 aDimensions[3] =
  {
    OpenGl_Vec4 (1.0f, 0.0f, 0.0f, 1.0f),
    OpenGl_Vec4 (0.0f, 1.0f, 0.0f, 1.0f),
    OpenGl_Vec4 (0.0f, 0.0f, 1.0f, 1.0f)
  };

  for (Standard_Integer aDim = 0; aDim < 3; ++aDim)
  {
    for (Standard_Integer aCornerIter = 0; aCornerIter < ClipVerticesNB; ++aCornerIter)
    {
      Standard_ShortReal aProjection = DotProduct (aDimensions[aDim], myClipVerts[aCornerIter]);
      aProjectedVerts[aCornerIter] = aProjection;
    }
    myMaxOrthoProjectionPts[aDim] = *std::max_element (aProjectedVerts, aProjectedVerts + ClipVerticesNB);
    myMinOrthoProjectionPts[aDim] = *std::min_element (aProjectedVerts, aProjectedVerts + ClipVerticesNB);
  }
}

// =======================================================================
// function : Intersect
// purpose  : Detects if AABB overlaps view volume using separating axis theorem (SAT)
// =======================================================================
Standard_Boolean OpenGl_BVHTreeSelector::Intersect (const OpenGl_Vec4& theMinPt,
                                                    const OpenGl_Vec4& theMaxPt) const
{
  //     E1
  //    |_ E0
  //   /
  //    E2
  const OpenGl_Vec4  aShiftedBoxMax  = theMaxPt - theMinPt;
  Standard_ShortReal aBoxProjMax     = 0.0f, aBoxProjMin     = 0.0f;
  Standard_ShortReal aFrustumProjMax = 0.0f, aFrustumProjMin = 0.0f;

  // E0 test
  aBoxProjMax = aShiftedBoxMax.x();
  aFrustumProjMax = myMaxOrthoProjectionPts[0] - DotProduct (OpenGl_Vec4 (1.0f, 0.0f, 0.0f, 1.0f), theMinPt);
  aFrustumProjMin = myMinOrthoProjectionPts[0] - DotProduct (OpenGl_Vec4 (1.0f, 0.0f, 0.0f, 1.0f), theMinPt);
  if (aBoxProjMin > aFrustumProjMax
   || aBoxProjMax < aFrustumProjMin)
  {
    return Standard_False;
  }

  // E1 test
  aBoxProjMax = aShiftedBoxMax.y();
  aFrustumProjMax = myMaxOrthoProjectionPts[1] - DotProduct (OpenGl_Vec4 (0.0f, 1.0f, 0.0f, 1.0f), theMinPt);
  aFrustumProjMin = myMinOrthoProjectionPts[1] - DotProduct (OpenGl_Vec4 (0.0f, 1.0f, 0.0f, 1.0f), theMinPt);
  if (aBoxProjMin > aFrustumProjMax
   || aBoxProjMax < aFrustumProjMin)
  {
    return Standard_False;
  }

  // E2 test
  aBoxProjMax = aShiftedBoxMax.z();
  aFrustumProjMax = myMaxOrthoProjectionPts[2] - DotProduct (OpenGl_Vec4 (0.0f, 0.0f, 1.0f, 1.0f), theMinPt);
  aFrustumProjMin = myMinOrthoProjectionPts[2] - DotProduct (OpenGl_Vec4 (0.0f, 0.0f, 1.0f, 1.0f), theMinPt);
  if (aBoxProjMin > aFrustumProjMax
   || aBoxProjMax < aFrustumProjMin)
  {
    return Standard_False;
  }

  const Standard_Integer anIncFactor = myIsProjectionParallel ? 2 : 1;
  for (Standard_Integer aPlaneIter = 0; aPlaneIter < 5; aPlaneIter += anIncFactor)
  {
    OpenGl_Vec4 aPlane = myClipPlanes[aPlaneIter];
    OpenGl_Vec4 aPt1 (0.0f), aPt2 (0.0f);
    aPt1 = BinarySign (aPlane) * aShiftedBoxMax;
    aBoxProjMax = DotProduct (aPlane, aPt1);
    aFrustumProjMax = myMaxClipProjectionPts[aPlaneIter] - DotProduct (aPlane, theMinPt);
    aFrustumProjMin = myMinClipProjectionPts[aPlaneIter] - DotProduct (aPlane, theMinPt);
    if (aFrustumProjMin < aBoxProjMax
     && aBoxProjMax < aFrustumProjMax)
    {
      continue;
    }

    aPt2 = InversedBinarySign (aPlane) * aShiftedBoxMax;
    aBoxProjMin = DotProduct (aPlane, aPt2);
    if (aBoxProjMin > aFrustumProjMax
     || aBoxProjMax < aFrustumProjMin)
    {
      return Standard_False;
    }
  }

  return Standard_True;
}
