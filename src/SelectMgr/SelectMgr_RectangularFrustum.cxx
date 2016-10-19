// Created on: 2014-05-22
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <NCollection_Vector.hxx>
#include <Poly_Array1OfTriangle.hxx>

#include <SelectMgr_RectangularFrustum.hxx>

// =======================================================================
// function : segmentSegmentDistance
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::segmentSegmentDistance (const gp_Pnt& theSegPnt1,
                                                           const gp_Pnt& theSegPnt2,
                                                           Standard_Real& theDepth)
{
  gp_XYZ anU = theSegPnt2.XYZ() - theSegPnt1.XYZ();
  gp_XYZ aV = myViewRayDir.XYZ();
  gp_XYZ aW = theSegPnt1.XYZ() - myNearPickedPnt.XYZ();

  Standard_Real anA = anU.Dot (anU);
  Standard_Real aB = anU.Dot (aV);
  Standard_Real aC = aV.Dot (aV);
  Standard_Real aD = anU.Dot (aW);
  Standard_Real anE = aV.Dot (aW);
  Standard_Real aCoef = anA * aC - aB * aB;
  Standard_Real aSn = aCoef;
  Standard_Real aTc, aTn, aTd = aCoef;

  if (aCoef < Precision::Confusion())
  {
    aTn = anE;
    aTd = aC;
  }
  else
  {
    aSn = (aB * anE - aC * aD);
    aTn = (anA * anE - aB * aD);
    if (aSn < 0.0)
    {
      aTn = anE;
      aTd = aC;
    }
    else if (aSn > aCoef)
    {
      aTn = anE + aB;
      aTd = aC;
    }
  }

  if (aTn < 0.0)
  {
    aTn = 0.0;
  }
  else if (aTn > aTd)
  {
    aTn = aTd;
  }
  aTc = (Abs (aTn) < Precision::Confusion() ? 0.0 : aTn / aTd);

  gp_Pnt aClosestPnt = myNearPickedPnt.XYZ() + myViewRayDir.XYZ() * aTc;
  theDepth = myNearPickedPnt.Distance (aClosestPnt) * myScale;
}

// =======================================================================
// function : segmentPlaneIntersection
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::segmentPlaneIntersection (const gp_Vec& thePlane,
                                                             const gp_Pnt& thePntOnPlane,
                                                             Standard_Real& theDepth)
{
  gp_XYZ anU = myViewRayDir.XYZ();
  gp_XYZ aW = myNearPickedPnt.XYZ() - thePntOnPlane.XYZ();
  Standard_Real aD = thePlane.Dot (anU);
  Standard_Real aN = -thePlane.Dot (aW);

  if (Abs (aD) < Precision::Confusion())
  {
    if (Abs (aN) < Precision::Angular())
    {
      theDepth = DBL_MAX;
      return;
    }
    else
    {
      theDepth = DBL_MAX;
      return;
    }
  }

  Standard_Real aParam = aN / aD;
  if (aParam < 0.0 || aParam > 1.0)
  {
    theDepth = DBL_MAX;
    return;
  }

  gp_Pnt aClosestPnt = myNearPickedPnt.XYZ() + anU * aParam;
  theDepth = myNearPickedPnt.Distance (aClosestPnt) * myScale;
}

namespace
{
  // =======================================================================
  // function : computeFrustum
  // purpose  : Computes base frustum data: its vertices and edge directions
  // =======================================================================
  void computeFrustum (const gp_Pnt2d theMinPnt, const gp_Pnt2d& theMaxPnt,
                       const Handle(SelectMgr_FrustumBuilder)& theBuilder,
                       gp_Pnt* theVertices, gp_Vec* theEdges)
  {
    // LeftTopNear
    theVertices[0] = theBuilder->ProjectPntOnViewPlane (theMinPnt.X(),
                                                        theMaxPnt.Y(),
                                                        0.0);
    // LeftTopFar
    theVertices[1] = theBuilder->ProjectPntOnViewPlane (theMinPnt.X(),
                                                        theMaxPnt.Y(),
                                                        1.0);
    // LeftBottomNear
    theVertices[2] = theBuilder->ProjectPntOnViewPlane (theMinPnt.X(),
                                                        theMinPnt.Y(),
                                                        0.0);
    // LeftBottomFar
    theVertices[3] = theBuilder->ProjectPntOnViewPlane (theMinPnt.X(),
                                                        theMinPnt.Y(),
                                                        1.0);
    // RightTopNear
    theVertices[4] = theBuilder->ProjectPntOnViewPlane (theMaxPnt.X(),
                                                        theMaxPnt.Y(),
                                                        0.0);
    // RightTopFar
    theVertices[5] = theBuilder->ProjectPntOnViewPlane (theMaxPnt.X(),
                                                        theMaxPnt.Y(),
                                                        1.0);
    // RightBottomNear
    theVertices[6] = theBuilder->ProjectPntOnViewPlane (theMaxPnt.X(),
                                                        theMinPnt.Y(),
                                                        0.0);
    // RightBottomFar
    theVertices[7] = theBuilder->ProjectPntOnViewPlane (theMaxPnt.X(),
                                                        theMinPnt.Y(),
                                                        1.0);

    // Horizontal
    theEdges[0] = theVertices[4].XYZ() - theVertices[0].XYZ();
    // Vertical
    theEdges[1] = theVertices[2].XYZ() - theVertices[0].XYZ();
    // LeftLower
    theEdges[2] = theVertices[2].XYZ() - theVertices[3].XYZ();
    // RightLower
    theEdges[3] = theVertices[6].XYZ() - theVertices[7].XYZ();
    // LeftUpper
    theEdges[4] = theVertices[0].XYZ() - theVertices[1].XYZ();
    // RightUpper
    theEdges[5] = theVertices[4].XYZ() - theVertices[5].XYZ();
  }

  // =======================================================================
  // function : computeNormals
  // purpose  : Computes normals to frustum faces
  // =======================================================================
  void computeNormals (const gp_Vec* theEdges, gp_Vec* theNormals)
  {
    // Top
    theNormals[0] = theEdges[0].Crossed (theEdges[4]);
    // Bottom
    theNormals[1] = theEdges[2].Crossed (theEdges[0]);
    // Left
    theNormals[2] = theEdges[4].Crossed (theEdges[1]);
    // Right
    theNormals[3] = theEdges[1].Crossed (theEdges[5]);
    // Near
    theNormals[4] = theEdges[0].Crossed (theEdges[1]);
    // Far
    theNormals[5] = -theNormals[4];
  }
}

// =======================================================================
// function : cacheVertexProjections
// purpose  : Caches projection of frustum's vertices onto its plane directions
//            and {i, j, k}
// =======================================================================
void SelectMgr_RectangularFrustum::cacheVertexProjections (SelectMgr_RectangularFrustum* theFrustum) const
{
  if (theFrustum->myIsOrthographic)
  {
    // project vertices onto frustum normals
    // Since orthographic view volume's faces are always a pairwise translation of
    // one another, only 2 vertices that belong to opposite faces can be projected
    // to simplify calculations.
    Standard_Integer aVertIdxs[6] = { LeftTopNear, LeftBottomNear,       // opposite planes in height direction
                                      LeftBottomNear, RightBottomNear,   // opposite planes in width direcion
                                      LeftBottomFar, RightBottomNear };  // opposite planes in depth direction
    for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 5; aPlaneIdx += 2)
    {
      Standard_Real aProj1 = theFrustum->myPlanes[aPlaneIdx].XYZ().Dot (theFrustum->myVertices[aVertIdxs[aPlaneIdx]].XYZ());
      Standard_Real aProj2 = theFrustum->myPlanes[aPlaneIdx].XYZ().Dot (theFrustum->myVertices[aVertIdxs[aPlaneIdx + 1]].XYZ());
      theFrustum->myMinVertsProjections[aPlaneIdx] = Min (aProj1, aProj2);
      theFrustum->myMaxVertsProjections[aPlaneIdx] = Max (aProj1, aProj2);
    }
  }
  else
  {
    // project all vertices onto frustum normals
    for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 6; ++aPlaneIdx)
    {
      Standard_Real aMax = -DBL_MAX;
      Standard_Real aMin = DBL_MAX;
      const gp_XYZ& aPlane = theFrustum->myPlanes[aPlaneIdx].XYZ();
      for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
      {
        Standard_Real aProjection = aPlane.Dot (theFrustum->myVertices[aVertIdx].XYZ());
        aMin = Min (aMin, aProjection);
        aMax = Max (aMax, aProjection);
      }
      theFrustum->myMinVertsProjections[aPlaneIdx] = aMin;
      theFrustum->myMaxVertsProjections[aPlaneIdx] = aMax;
    }
  }

  // project vertices onto {i, j, k}
  for (Standard_Integer aDim = 0; aDim < 3; ++aDim)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin = DBL_MAX;
    for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
    {
      const gp_XYZ& aVert = theFrustum->myVertices[aVertIdx].XYZ();
      aMax = Max (aVert.GetData()[aDim], aMax);
      aMin = Min (aVert.GetData()[aDim], aMin);
    }
    theFrustum->myMaxOrthoVertsProjections[aDim] = aMax;
    theFrustum->myMinOrthoVertsProjections[aDim] = aMin;
  }
}

// =======================================================================
// function : Build
// purpose  : Build volume according to the point and given pixel
//            tolerance
// =======================================================================
void SelectMgr_RectangularFrustum::Build (const gp_Pnt2d &thePoint)
{
  myNearPickedPnt = myBuilder->ProjectPntOnViewPlane (thePoint.X(), thePoint.Y(), 0.0);
  myFarPickedPnt = myBuilder->ProjectPntOnViewPlane (thePoint.X(), thePoint.Y(), 1.0);
  myViewRayDir = myFarPickedPnt.XYZ() - myNearPickedPnt.XYZ();
  myMousePos = thePoint;

  gp_Pnt2d aMinPnt (thePoint.X() - myPixelTolerance * 0.5,
                    thePoint.Y() - myPixelTolerance * 0.5);
  gp_Pnt2d aMaxPnt (thePoint.X() + myPixelTolerance * 0.5,
                    thePoint.Y() + myPixelTolerance * 0.5);

  // calculate base frustum characteristics: vertices and edge directions
  computeFrustum (aMinPnt, aMaxPnt, myBuilder, myVertices, myEdgeDirs);

  // compute frustum normals
  computeNormals (myEdgeDirs, myPlanes);

  // compute vertices projections onto frustum normals and
  // {i, j, k} vectors and store them to corresponding class fields
  cacheVertexProjections (this);

  myViewClipRange.Clear();

  myScale = 1.0;
}

// =======================================================================
// function : Build
// purpose  : Build volume according to the selected rectangle
// =======================================================================
void SelectMgr_RectangularFrustum::Build (const gp_Pnt2d& theMinPnt,
                                          const gp_Pnt2d& theMaxPnt)
{
  myNearPickedPnt = myBuilder->ProjectPntOnViewPlane ((theMinPnt.X() + theMaxPnt.X()) * 0.5,
                                                      (theMinPnt.Y() + theMaxPnt.Y()) * 0.5,
                                                      0.0);
  myFarPickedPnt = myBuilder->ProjectPntOnViewPlane ((theMinPnt.X() + theMaxPnt.X()) * 0.5,
                                                     (theMinPnt.Y() + theMaxPnt.Y()) * 0.5,
                                                     1.0);
  myViewRayDir = myFarPickedPnt.XYZ() - myNearPickedPnt.XYZ();

  // calculate base frustum characteristics: vertices and edge directions
  computeFrustum (theMinPnt, theMaxPnt, myBuilder, myVertices, myEdgeDirs);

  // compute frustum normals
  computeNormals (myEdgeDirs, myPlanes);

  // compute vertices projections onto frustum normals and
  // {i, j, k} vectors and store them to corresponding class fields
  cacheVertexProjections (this);

  myViewClipRange.Clear();

  myScale = 1.0;
}

// =======================================================================
// function : ScaleAndTransform
// purpose  : IMPORTANT: Scaling makes sense only for frustum built on a single point!
//            Note that this method does not perform any checks on type of the frustum.
//            Returns a copy of the frustum resized according to the scale factor given
//            and transforms it using the matrix given.
//            There are no default parameters, but in case if:
//                - transformation only is needed: @theScaleFactor must be initialized
//                  as any negative value;
//                - scale only is needed: @theTrsf must be set to gp_Identity.
// =======================================================================
Handle(SelectMgr_BaseFrustum) SelectMgr_RectangularFrustum::ScaleAndTransform (const Standard_Integer theScaleFactor,
                                                                               const gp_GTrsf& theTrsf) const
{
  Standard_ASSERT_RAISE (theScaleFactor > 0,
    "Error! Pixel tolerance for selection should be greater than zero");

  Handle(SelectMgr_RectangularFrustum) aRes = new SelectMgr_RectangularFrustum();
  const Standard_Boolean isToScale = theScaleFactor != 1;
  const Standard_Boolean isToTrsf  = theTrsf.Form() != gp_Identity;

  if (!isToScale && !isToTrsf)
    return aRes;

  aRes->myIsOrthographic = myIsOrthographic;
  const SelectMgr_RectangularFrustum* aRef = this;

  if (isToScale)
  {
    aRes->myNearPickedPnt = myNearPickedPnt;
    aRes->myFarPickedPnt  = myFarPickedPnt;
    aRes->myViewRayDir    = myViewRayDir;

    const gp_Pnt2d aMinPnt (myMousePos.X() - theScaleFactor * 0.5,
                            myMousePos.Y() - theScaleFactor * 0.5);
    const gp_Pnt2d aMaxPnt (myMousePos.X() + theScaleFactor * 0.5,
                            myMousePos.Y() + theScaleFactor * 0.5);

    // recompute base frustum characteristics from scratch
    computeFrustum (aMinPnt, aMaxPnt, myBuilder, aRes->myVertices, aRes->myEdgeDirs);

    aRef = aRes.get();
  }

  if (isToTrsf)
  {
    const Standard_Real aRefScale = aRef->myFarPickedPnt.SquareDistance (aRef->myNearPickedPnt);

    gp_Pnt aPoint = aRef->myNearPickedPnt;
    theTrsf.Transforms (aPoint.ChangeCoord());
    aRes->myNearPickedPnt = aPoint;

    aPoint.SetXYZ (aRef->myFarPickedPnt.XYZ());
    theTrsf.Transforms (aPoint.ChangeCoord());
    aRes->myFarPickedPnt = aPoint;

    aRes->myViewRayDir    = aRes->myFarPickedPnt.XYZ() - aRes->myNearPickedPnt.XYZ();

    for (Standard_Integer anIt = 0; anIt < 8; anIt++)
    {
      aPoint = aRef->myVertices[anIt];
      theTrsf.Transforms (aPoint.ChangeCoord());
      aRes->myVertices[anIt] = aPoint;
    }

    // Horizontal
    aRes->myEdgeDirs[0] = aRes->myVertices[4].XYZ() - aRes->myVertices[0].XYZ();
    // Vertical
    aRes->myEdgeDirs[1] = aRes->myVertices[2].XYZ() - aRes->myVertices[0].XYZ();
    // LeftLower
    aRes->myEdgeDirs[2] = aRes->myVertices[2].XYZ() - aRes->myVertices[3].XYZ();
    // RightLower
    aRes->myEdgeDirs[3] = aRes->myVertices[6].XYZ() - aRes->myVertices[7].XYZ();
    // LeftUpper
    aRes->myEdgeDirs[4] = aRes->myVertices[0].XYZ() - aRes->myVertices[1].XYZ();
    // RightUpper
    aRes->myEdgeDirs[5] = aRes->myVertices[4].XYZ() - aRes->myVertices[5].XYZ();

    // Compute scale to transform depth from local coordinate system to world coordinate system
    aRes->myScale = Sqrt (aRefScale / aRes->myFarPickedPnt.SquareDistance (aRes->myNearPickedPnt));
  }

  // compute frustum normals
  computeNormals (aRes->myEdgeDirs, aRes->myPlanes);

  cacheVertexProjections (aRes.get());

  aRes->myViewClipRange = myViewClipRange;
  aRes->myIsViewClipEnabled = myIsViewClipEnabled;
  aRes->myMousePos      = myMousePos;

  return aRes;
}

// =======================================================================
// function : Overlaps
// purpose  : Returns true if selecting volume is overlapped by
//            axis-aligned bounding box with minimum corner at point
//            theMinPnt and maximum at point theMaxPnt
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                         const SelectMgr_Vec3& theBoxMax,
                                                         Standard_Boolean*     theInside)
{
  return hasOverlap (theBoxMin, theBoxMax, theInside);
}

// =======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and
//            given axis-aligned box
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                         const SelectMgr_Vec3& theBoxMax,
                                                         Standard_Real& theDepth)
{
  if (!hasOverlap (theBoxMin, theBoxMax))
    return Standard_False;

  gp_Pnt aNearestPnt (RealLast(), RealLast(), RealLast());
  aNearestPnt.SetX (Max (Min (myNearPickedPnt.X(), theBoxMax.x()), theBoxMin.x()));
  aNearestPnt.SetY (Max (Min (myNearPickedPnt.Y(), theBoxMax.y()), theBoxMin.y()));
  aNearestPnt.SetZ (Max (Min (myNearPickedPnt.Z(), theBoxMax.z()), theBoxMin.z()));

  theDepth = aNearestPnt.Distance (myNearPickedPnt);

  return isViewClippingOk (theDepth);
}

// =======================================================================
// function : Overlaps
// purpose  : Intersection test between defined volume and given point
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const gp_Pnt& thePnt,
                                                         Standard_Real& theDepth)
{
  if (!hasOverlap (thePnt))
    return Standard_False;

  gp_XYZ aV = thePnt.XYZ() - myNearPickedPnt.XYZ();
  gp_Pnt aDetectedPnt =
    myNearPickedPnt.XYZ() + myViewRayDir.XYZ() * (aV.Dot (myViewRayDir.XYZ()) / myViewRayDir.Dot (myViewRayDir));

  theDepth = aDetectedPnt.Distance (myNearPickedPnt) * myScale;

  return isViewClippingOk (theDepth);
}

// =======================================================================
// function : Overlaps
// purpose  : Intersection test between defined volume and given point
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const gp_Pnt& thePnt)
{
  return hasOverlap (thePnt);
}

// =======================================================================
// function : Overlaps
// purpose  : Checks if line segment overlaps selecting frustum
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const gp_Pnt& thePnt1,
                                                         const gp_Pnt& thePnt2,
                                                         Standard_Real& theDepth)
{
  theDepth = -DBL_MAX;
  if (!hasOverlap (thePnt1, thePnt2))
    return Standard_False;

  segmentSegmentDistance (thePnt1, thePnt2, theDepth);

  return isViewClippingOk (theDepth);
}

// =======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and given
//            ordered set of points, representing line segments. The test
//            may be considered of interior part or boundary line defined
//            by segments depending on given sensitivity type
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const TColgp_Array1OfPnt& theArrayOfPnts,
                                                         Select3D_TypeOfSensitivity theSensType,
                                                         Standard_Real& theDepth)
{
  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    Standard_Integer aMatchingSegmentsNb = -1;
    theDepth = DBL_MAX;
    const Standard_Integer aLower  = theArrayOfPnts.Lower();
    const Standard_Integer anUpper = theArrayOfPnts.Upper();
    for (Standard_Integer aPntIter = aLower; aPntIter <= anUpper; ++aPntIter)
    {
      const gp_Pnt& aStartPnt = theArrayOfPnts.Value (aPntIter);
      const gp_Pnt& aEndPnt   = theArrayOfPnts.Value (aPntIter == anUpper ? aLower : (aPntIter + 1));
      if (hasOverlap (aStartPnt, aEndPnt))
      {
        aMatchingSegmentsNb++;
        Standard_Real aSegmentDepth = RealLast();
        segmentSegmentDistance (aStartPnt, aEndPnt, aSegmentDepth);
        theDepth = Min (theDepth, aSegmentDepth);
      }
    }

    if (aMatchingSegmentsNb == -1)
      return Standard_False;
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    gp_Vec aPolyNorm (gp_XYZ (RealLast(), RealLast(), RealLast()));
    if (!hasOverlap (theArrayOfPnts, aPolyNorm))
      return Standard_False;

    segmentPlaneIntersection (aPolyNorm,
                              theArrayOfPnts.Value (theArrayOfPnts.Lower()),
                              theDepth);
  }

  return isViewClippingOk (theDepth);
}

// =======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and given
//            triangle. The test may be considered of interior part or
//            boundary line defined by triangle vertices depending on
//            given sensitivity type
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const gp_Pnt& thePnt1,
                                                         const gp_Pnt& thePnt2,
                                                         const gp_Pnt& thePnt3,
                                                         Select3D_TypeOfSensitivity theSensType,
                                                         Standard_Real& theDepth)
{
  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    const gp_Pnt aPntsArrayBuf[4] = { thePnt1, thePnt2, thePnt3, thePnt1 };
    const TColgp_Array1OfPnt aPntsArray (aPntsArrayBuf[0], 1, 4);
    return Overlaps (aPntsArray, Select3D_TOS_BOUNDARY, theDepth);
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    gp_Vec aTriangleNormal (gp_XYZ (RealLast(), RealLast(), RealLast()));
    if (!hasOverlap (thePnt1, thePnt2, thePnt3, aTriangleNormal))
      return Standard_False;

    // check if intersection point belongs to triangle's interior part
    gp_XYZ aTrEdges[3] = { thePnt2.XYZ() - thePnt1.XYZ(),
                           thePnt3.XYZ() - thePnt2.XYZ(),
                           thePnt1.XYZ() - thePnt3.XYZ() };

    Standard_Real anAlpha = aTriangleNormal.Dot (myViewRayDir);
    if (Abs (anAlpha) < gp::Resolution())
    {
      // handle degenerated triangles: in this case, there is no possible way to detect overlap correctly.
      if (aTriangleNormal.SquareMagnitude() < gp::Resolution())
      {
        theDepth = std::numeric_limits<Standard_Real>::max();
        return Standard_False;
      }

      // handle the case when triangle normal and selecting frustum direction are orthogonal: for this case, overlap
      // is detected correctly, and distance to triangle's plane can be measured as distance to its arbitrary vertex.
      const gp_XYZ aDiff = myNearPickedPnt.XYZ() - thePnt1.XYZ();
      theDepth = aTriangleNormal.Dot (aDiff) * myScale;

      return isViewClippingOk (theDepth);
    }

    gp_XYZ anEdge = (thePnt1.XYZ() - myNearPickedPnt.XYZ()) * (1.0 / anAlpha);

    Standard_Real aTime = aTriangleNormal.Dot (anEdge);

    gp_XYZ aVec = myViewRayDir.XYZ().Crossed (anEdge);

    Standard_Real anU = aVec.Dot (aTrEdges[2]);
    Standard_Real aV  = aVec.Dot (aTrEdges[0]);

    Standard_Boolean isInterior = (aTime >= 0.0) && (anU >= 0.0) && (aV >= 0.0) && (anU + aV <= 1.0);

    if (isInterior)
    {
      gp_Pnt aDetectedPnt = myNearPickedPnt.XYZ() + myViewRayDir.XYZ() * aTime;
      theDepth = myNearPickedPnt.Distance (aDetectedPnt) * myScale;

      return isViewClippingOk (theDepth);
    }

    gp_Pnt aPnts[3] = {thePnt1, thePnt2, thePnt3};
    Standard_Real aMinDist = RealLast();
    Standard_Integer aNearestEdgeIdx = -1;
    gp_Pnt aPtOnPlane = myNearPickedPnt.XYZ() + myViewRayDir.XYZ() * aTime;
    for (Standard_Integer anEdgeIdx = 0; anEdgeIdx < 3; ++anEdgeIdx)
    {
      gp_XYZ aW = aPtOnPlane.XYZ() - aPnts[anEdgeIdx].XYZ();
      Standard_Real aCoef = aTrEdges[anEdgeIdx].Dot (aW) / aTrEdges[anEdgeIdx].Dot (aTrEdges[anEdgeIdx]);
      Standard_Real aDist = aPtOnPlane.Distance (aPnts[anEdgeIdx].XYZ() + aCoef * aTrEdges[anEdgeIdx]);
      if (aMinDist > aDist)
      {
        aMinDist = aDist;
        aNearestEdgeIdx = anEdgeIdx;
      }
    }
    segmentSegmentDistance (aPnts[aNearestEdgeIdx], aPnts[(aNearestEdgeIdx + 1) % 3], theDepth);
  }

  return isViewClippingOk (theDepth);
}

// =======================================================================
// function : DistToGeometryCenter
// purpose  : Measures distance between 3d projection of user-picked
//            screen point and given point theCOG
// =======================================================================
Standard_Real SelectMgr_RectangularFrustum::DistToGeometryCenter (const gp_Pnt& theCOG)
{
  return theCOG.Distance (myNearPickedPnt) * myScale;
}

// =======================================================================
// function : DetectedPoint
// purpose  : Calculates the point on a view ray that was detected during
//            the run of selection algo by given depth
// =======================================================================
gp_Pnt SelectMgr_RectangularFrustum::DetectedPoint (const Standard_Real theDepth) const
{
  return myNearPickedPnt.XYZ() + myViewRayDir.Normalized().XYZ() * theDepth;
}

// =======================================================================
// function : computeClippingRange
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::computeClippingRange (const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                                         Standard_Real& theDepthMin,
                                                         Standard_Real& theDepthMax)
{
  theDepthMax = DBL_MAX;
  theDepthMin = -DBL_MAX;
  Standard_Real aPlaneA, aPlaneB, aPlaneC, aPlaneD;
  for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (thePlanes); aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aClipPlane = aPlaneIt.Value();
    if (!aClipPlane->IsOn())
      continue;

    gp_Pln aGeomPlane = aClipPlane->ToPlane();

    aGeomPlane.Coefficients (aPlaneA, aPlaneB, aPlaneC, aPlaneD);

    const gp_XYZ& aPlaneDirXYZ = aGeomPlane.Axis().Direction().XYZ();

    Standard_Real aDotProduct = myViewRayDir.XYZ ().Dot (aPlaneDirXYZ);
    Standard_Real aDistance = - myNearPickedPnt.XYZ ().Dot (aPlaneDirXYZ)
                              - aPlaneD;

    // check whether the pick line is parallel to clip plane
    if (Abs (aDotProduct) < Precision::Angular())
    {
      // line lies below the plane and is not clipped, skip
      continue;
    }

    // compute distance to point of pick line intersection with the plane
    Standard_Real aParam = aDistance / aDotProduct;

    // check if ray intersects the plane, in case aIntDist < 0
    // the plane is "behind" the ray
    if (aParam < 0.0)
    {
      continue;
    }

    const gp_Pnt anIntersectionPt = myNearPickedPnt.XYZ() + myViewRayDir.XYZ() * aParam;
    const Standard_Real aDistToPln = anIntersectionPt.Distance (myNearPickedPnt);

    // change depth limits for case of opposite and directed planes
    if (aDotProduct < 0.0)
    {
      theDepthMax = Min (aDistToPln, theDepthMax);
    }
    else if (aDistToPln > theDepthMin)
    {
      theDepthMin = Max (aDistToPln, theDepthMin);
    }
  }
}

// =======================================================================
// function : IsClipped
// purpose  : Checks if the point of sensitive in which selection was
//            detected belongs to the region defined by clipping planes
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::IsClipped (const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                                          const Standard_Real theDepth)
{
  Standard_Real aMaxDepth, aMinDepth;
  computeClippingRange (thePlanes, aMinDepth, aMaxDepth);

  return (theDepth <= aMinDepth || theDepth >= aMaxDepth);
}

// =======================================================================
// function : SetViewClipping
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::SetViewClipping (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes)
{
  if (thePlanes.IsNull()
   || thePlanes->IsEmpty())
  {
    myViewClipRange.Clear();
    return;
  }

  Standard_Real aMaxDepth, aMinDepth;
  computeClippingRange (*thePlanes, aMinDepth, aMaxDepth);
  myViewClipRange.Set (aMinDepth, aMaxDepth);
}

// =======================================================================
// function : isViewClippingOk
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::isViewClippingOk (const Standard_Real theDepth) const
{
  if (!myViewClipRange.IsValid()
   || !myIsViewClipEnabled)
  {
    return Standard_True;
  }

  return myViewClipRange.MaxDepth() > theDepth
    && myViewClipRange.MinDepth() < theDepth;
}

// =======================================================================
// function : GetPlanes
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::GetPlanes (NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const
{
  thePlaneEquations.Clear();

  SelectMgr_Vec4 anEquation;
  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 6; ++aPlaneIdx)
  {
    const gp_Vec& aPlaneNorm = myIsOrthographic && aPlaneIdx % 2 == 1 ?
      myPlanes[aPlaneIdx - 1].Reversed() : myPlanes[aPlaneIdx];
    anEquation.x() = aPlaneNorm.X();
    anEquation.y() = aPlaneNorm.Y();
    anEquation.z() = aPlaneNorm.Z();
    anEquation.w() = - (aPlaneNorm.XYZ().Dot (myVertices[aPlaneIdx % 2 == 0 ? aPlaneIdx : aPlaneIdx + 2].XYZ()));
    thePlaneEquations.Append (anEquation);
  }
}
