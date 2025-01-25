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

#include <SelectMgr_RectangularFrustum.hxx>

#include <BVH_Tools.hxx>
#include <gp_Pln.hxx>
#include <NCollection_Vector.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <SelectMgr_FrustumBuilder.hxx>
#include <SelectMgr_ViewClipRange.hxx>

// =======================================================================
// function : Constructor
// purpose  :
// =======================================================================
SelectMgr_RectangularFrustum::SelectMgr_RectangularFrustum()
    : myScale(1.0)
{
}

// =======================================================================
// function : segmentSegmentDistance
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::segmentSegmentDistance(
  const gp_Pnt&            theSegPnt1,
  const gp_Pnt&            theSegPnt2,
  SelectBasics_PickResult& thePickResult) const
{
  gp_XYZ anU = theSegPnt2.XYZ() - theSegPnt1.XYZ();
  // clang-format off
  gp_XYZ aV = myFarPickedPnt.XYZ() - myNearPickedPnt.XYZ(); // use unnormalized vector instead of myViewRayDir to clip solutions behind Far plane
  // clang-format on
  gp_XYZ aW = theSegPnt1.XYZ() - myNearPickedPnt.XYZ();

  Standard_Real anA   = anU.Dot(anU);
  Standard_Real aB    = anU.Dot(aV);
  Standard_Real aC    = aV.Dot(aV);
  Standard_Real aD    = anU.Dot(aW);
  Standard_Real anE   = aV.Dot(aW);
  Standard_Real aCoef = anA * aC - aB * aB;
  Standard_Real aSn   = aCoef;
  Standard_Real aTc, aTn, aTd = aCoef;

  if (aCoef < gp::Resolution())
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
  aTc = (Abs(aTd) < gp::Resolution() ? 0.0 : aTn / aTd);

  const gp_Pnt aClosestPnt = myNearPickedPnt.XYZ() + aV * aTc;
  thePickResult.SetDepth(myNearPickedPnt.Distance(aClosestPnt) * myScale);

  const gp_Vec        aPickedVec    = aClosestPnt.XYZ() - theSegPnt1.XYZ();
  const gp_Vec        aFigureVec    = theSegPnt2.XYZ() - theSegPnt1.XYZ();
  const Standard_Real aPickedVecMod = aPickedVec.Magnitude();
  const Standard_Real aFigureVecMod = aFigureVec.Magnitude();
  if (aPickedVecMod <= gp::Resolution() || aFigureVecMod <= gp::Resolution())
  {
    thePickResult.SetPickedPoint(aClosestPnt);
    return;
  }

  const Standard_Real aCosOfAngle  = aFigureVec.Dot(aPickedVec) / (aPickedVecMod * aFigureVecMod);
  const Standard_Real aSegPntShift = Min(aFigureVecMod, Max(0.0, aCosOfAngle * aPickedVecMod));
  thePickResult.SetPickedPoint(theSegPnt1.XYZ()
                               + aFigureVec.XYZ() * (aSegPntShift / aFigureVecMod));
}

// =======================================================================
// function : segmentPlaneIntersection
// purpose  :
// =======================================================================
bool SelectMgr_RectangularFrustum::segmentPlaneIntersection(
  const gp_Vec&            thePlane,
  const gp_Pnt&            thePntOnPlane,
  SelectBasics_PickResult& thePickResult) const
{
  // clang-format off
  gp_XYZ anU = myFarPickedPnt.XYZ() - myNearPickedPnt.XYZ(); // use unnormalized vector instead of myViewRayDir to clip solutions behind Far plane by > 1.0 check
  // clang-format on
  gp_XYZ        aW = myNearPickedPnt.XYZ() - thePntOnPlane.XYZ();
  Standard_Real aD = thePlane.Dot(anU);
  Standard_Real aN = -thePlane.Dot(aW);

  if (Abs(aD) < Precision::Confusion())
  {
    if (Abs(aN) < Precision::Angular())
    {
      thePickResult.Invalidate();
      return false;
    }
    else
    {
      thePickResult.Invalidate();
      return false;
    }
  }

  Standard_Real aParam = aN / aD;
  if (aParam < 0.0
      || aParam > 1.0) // > 1.0 check could be removed for an infinite ray and anU=myViewRayDir
  {
    thePickResult.Invalidate();
    return false;
  }

  gp_Pnt aClosestPnt = myNearPickedPnt.XYZ() + anU * aParam;
  thePickResult.SetDepth(myNearPickedPnt.Distance(aClosestPnt) * myScale);
  return true;
}

namespace
{
// =======================================================================
// function : computeFrustum
// purpose  : Computes base frustum data: its vertices and edge directions
// =======================================================================
void computeFrustum(const gp_Pnt2d                          theMinPnt,
                    const gp_Pnt2d&                         theMaxPnt,
                    const Handle(SelectMgr_FrustumBuilder)& theBuilder,
                    gp_Pnt*                                 theVertices,
                    gp_Vec*                                 theEdges)
{
  // LeftTopNear
  theVertices[0] = theBuilder->ProjectPntOnViewPlane(theMinPnt.X(), theMaxPnt.Y(), 0.0);
  // LeftTopFar
  theVertices[1] = theBuilder->ProjectPntOnViewPlane(theMinPnt.X(), theMaxPnt.Y(), 1.0);
  // LeftBottomNear
  theVertices[2] = theBuilder->ProjectPntOnViewPlane(theMinPnt.X(), theMinPnt.Y(), 0.0);
  // LeftBottomFar
  theVertices[3] = theBuilder->ProjectPntOnViewPlane(theMinPnt.X(), theMinPnt.Y(), 1.0);
  // RightTopNear
  theVertices[4] = theBuilder->ProjectPntOnViewPlane(theMaxPnt.X(), theMaxPnt.Y(), 0.0);
  // RightTopFar
  theVertices[5] = theBuilder->ProjectPntOnViewPlane(theMaxPnt.X(), theMaxPnt.Y(), 1.0);
  // RightBottomNear
  theVertices[6] = theBuilder->ProjectPntOnViewPlane(theMaxPnt.X(), theMinPnt.Y(), 0.0);
  // RightBottomFar
  theVertices[7] = theBuilder->ProjectPntOnViewPlane(theMaxPnt.X(), theMinPnt.Y(), 1.0);

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
void computeNormals(const gp_Vec* theEdges, gp_Vec* theNormals)
{
  // Top
  theNormals[0] = theEdges[0].Crossed(theEdges[4]);
  // Bottom
  theNormals[1] = theEdges[2].Crossed(theEdges[0]);
  // Left
  theNormals[2] = theEdges[4].Crossed(theEdges[1]);
  // Right
  theNormals[3] = theEdges[1].Crossed(theEdges[5]);
  // Near
  theNormals[4] = theEdges[0].Crossed(theEdges[1]);
  // Far
  theNormals[5] = -theNormals[4];
}
} // namespace

// =======================================================================
// function : cacheVertexProjections
// purpose  : Caches projection of frustum's vertices onto its plane directions
//            and {i, j, k}
// =======================================================================
void SelectMgr_RectangularFrustum::cacheVertexProjections(
  SelectMgr_RectangularFrustum* theFrustum) const
{
  if (theFrustum->Camera()->IsOrthographic())
  {
    // project vertices onto frustum normals
    // Since orthographic view volume's faces are always a pairwise translation of
    // one another, only 2 vertices that belong to opposite faces can be projected
    // to simplify calculations.
    Standard_Integer aVertIdxs[6] = {LeftTopNear,
                                     LeftBottomNear, // opposite planes in height direction
                                     LeftBottomNear,
                                     RightBottomNear,  // opposite planes in width direction
                                                       // clang-format off
                                      LeftBottomFar, RightBottomNear };  // opposite planes in depth direction
                                                       // clang-format on
    for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 5; aPlaneIdx += 2)
    {
      Standard_Real aProj1 = theFrustum->myPlanes[aPlaneIdx].XYZ().Dot(
        theFrustum->myVertices[aVertIdxs[aPlaneIdx]].XYZ());
      Standard_Real aProj2 = theFrustum->myPlanes[aPlaneIdx].XYZ().Dot(
        theFrustum->myVertices[aVertIdxs[aPlaneIdx + 1]].XYZ());
      theFrustum->myMinVertsProjections[aPlaneIdx] = Min(aProj1, aProj2);
      theFrustum->myMaxVertsProjections[aPlaneIdx] = Max(aProj1, aProj2);
    }
  }
  else
  {
    // project all vertices onto frustum normals
    for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 6; ++aPlaneIdx)
    {
      Standard_Real aMax   = -DBL_MAX;
      Standard_Real aMin   = DBL_MAX;
      const gp_XYZ& aPlane = theFrustum->myPlanes[aPlaneIdx].XYZ();
      for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
      {
        Standard_Real aProjection = aPlane.Dot(theFrustum->myVertices[aVertIdx].XYZ());
        aMin                      = Min(aMin, aProjection);
        aMax                      = Max(aMax, aProjection);
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
      aMax                = Max(aVert.GetData()[aDim], aMax);
      aMin                = Min(aVert.GetData()[aDim], aMin);
    }
    theFrustum->myMaxOrthoVertsProjections[aDim] = aMax;
    theFrustum->myMinOrthoVertsProjections[aDim] = aMin;
  }
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::Init(const gp_Pnt2d& thePoint)
{
  mySelectionType = SelectMgr_SelectionType_Point;
  mySelRectangle.SetMousePos(thePoint);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::Init(const gp_Pnt2d& theMinPnt, const gp_Pnt2d& theMaxPnt)
{
  mySelectionType = SelectMgr_SelectionType_Box;
  mySelRectangle.SetMinPnt(theMinPnt);
  mySelRectangle.SetMaxPnt(theMaxPnt);
}

// =======================================================================
// function : Build
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::Build()
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Build() should be called after "
                        "selection frustum initialization");
  gp_Pnt2d aMinPnt, aMaxPnt;
  if (mySelectionType == SelectMgr_SelectionType_Point)
  {
    const gp_Pnt2d& aMousePos = mySelRectangle.MousePos();
    myNearPickedPnt           = myBuilder->ProjectPntOnViewPlane(aMousePos.X(), aMousePos.Y(), 0.0);
    myFarPickedPnt            = myBuilder->ProjectPntOnViewPlane(aMousePos.X(), aMousePos.Y(), 1.0);

    aMinPnt.SetCoord(aMousePos.X() - myPixelTolerance * 0.5,
                     aMousePos.Y() - myPixelTolerance * 0.5);
    aMaxPnt.SetCoord(aMousePos.X() + myPixelTolerance * 0.5,
                     aMousePos.Y() + myPixelTolerance * 0.5);
  }
  else
  {
    aMinPnt         = mySelRectangle.MinPnt();
    aMaxPnt         = mySelRectangle.MaxPnt();
    myNearPickedPnt = myBuilder->ProjectPntOnViewPlane((aMinPnt.X() + aMaxPnt.X()) * 0.5,
                                                       (aMinPnt.Y() + aMaxPnt.Y()) * 0.5,
                                                       0.0);
    myFarPickedPnt  = myBuilder->ProjectPntOnViewPlane((aMinPnt.X() + aMaxPnt.X()) * 0.5,
                                                      (aMinPnt.Y() + aMaxPnt.Y()) * 0.5,
                                                      1.0);
  }

  myViewRayDir = myFarPickedPnt.XYZ() - myNearPickedPnt.XYZ();

  // calculate base frustum characteristics: vertices and edge directions
  computeFrustum(aMinPnt, aMaxPnt, myBuilder, myVertices, myEdgeDirs);

  // compute frustum normals
  computeNormals(myEdgeDirs, myPlanes);

  // compute vertices projections onto frustum normals and
  // {i, j, k} vectors and store them to corresponding class fields
  cacheVertexProjections(this);

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
Handle(SelectMgr_BaseIntersector) SelectMgr_RectangularFrustum::ScaleAndTransform(
  const Standard_Integer                  theScaleFactor,
  const gp_GTrsf&                         theTrsf,
  const Handle(SelectMgr_FrustumBuilder)& theBuilder) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::ScaleAndTransform() should be called "
                        "after selection frustum initialization");

  Standard_ASSERT_RAISE(theScaleFactor >= 0,
                        "Error! Pixel tolerance for selection should not be negative");

  Handle(SelectMgr_RectangularFrustum) aRes      = new SelectMgr_RectangularFrustum();
  const Standard_Boolean               isToScale = theScaleFactor != 1;
  const Standard_Boolean               isToTrsf  = theTrsf.Form() != gp_Identity;

  if (!isToScale && !isToTrsf)
  {
    aRes->SetBuilder(theBuilder);
    return aRes;
  }

  aRes->SetCamera(myCamera);
  const SelectMgr_RectangularFrustum* aRef = this;

  if (isToScale)
  {
    aRes->myNearPickedPnt = myNearPickedPnt;
    aRes->myFarPickedPnt  = myFarPickedPnt;
    aRes->myViewRayDir    = myViewRayDir;

    const gp_Pnt2d& aMousePos = mySelRectangle.MousePos();
    const gp_Pnt2d  aMinPnt(aMousePos.X() - theScaleFactor * 0.5,
                           aMousePos.Y() - theScaleFactor * 0.5);
    const gp_Pnt2d  aMaxPnt(aMousePos.X() + theScaleFactor * 0.5,
                           aMousePos.Y() + theScaleFactor * 0.5);

    // recompute base frustum characteristics from scratch
    computeFrustum(aMinPnt, aMaxPnt, myBuilder, aRes->myVertices, aRes->myEdgeDirs);

    aRef = aRes.get();
  }

  if (isToTrsf)
  {
    const Standard_Real aRefScale = aRef->myFarPickedPnt.SquareDistance(aRef->myNearPickedPnt);

    gp_Pnt aPoint = aRef->myNearPickedPnt;
    theTrsf.Transforms(aPoint.ChangeCoord());
    aRes->myNearPickedPnt = aPoint;

    aPoint.SetXYZ(aRef->myFarPickedPnt.XYZ());
    theTrsf.Transforms(aPoint.ChangeCoord());
    aRes->myFarPickedPnt = aPoint;

    aRes->myViewRayDir = aRes->myFarPickedPnt.XYZ() - aRes->myNearPickedPnt.XYZ();

    for (Standard_Integer anIt = 0; anIt < 8; anIt++)
    {
      aPoint = aRef->myVertices[anIt];
      theTrsf.Transforms(aPoint.ChangeCoord());
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
    aRes->myScale = Sqrt(aRefScale / aRes->myFarPickedPnt.SquareDistance(aRes->myNearPickedPnt));
  }

  aRes->SetBuilder(theBuilder);

  // compute frustum normals
  computeNormals(aRes->myEdgeDirs, aRes->myPlanes);

  cacheVertexProjections(aRes.get());

  aRes->mySelectionType = mySelectionType;
  aRes->mySelRectangle  = mySelRectangle;
  return aRes;
}

// =======================================================================
// function : CopyWithBuilder
// purpose  : Returns a copy of the frustum using the given frustum builder configuration.
//            Returned frustum should be re-constructed before being used.
// =======================================================================
Handle(SelectMgr_BaseIntersector) SelectMgr_RectangularFrustum::CopyWithBuilder(
  const Handle(SelectMgr_FrustumBuilder)& theBuilder) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::CopyWithBuilder() should be called "
                        "after selection frustum initialization");

  Standard_ASSERT_RAISE(
    !theBuilder.IsNull(),
    "Error! SelectMgr_RectangularFrustum::CopyWithBuilder() should be called with valid builder");

  Handle(SelectMgr_RectangularFrustum) aRes = new SelectMgr_RectangularFrustum();
  aRes->mySelectionType                     = mySelectionType;
  aRes->mySelRectangle                      = mySelRectangle;
  aRes->myPixelTolerance                    = myPixelTolerance;
  aRes->SetBuilder(theBuilder);

  return aRes;
}

// =======================================================================
// function : IsScalable
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::IsScalable() const
{
  return mySelectionType == SelectMgr_SelectionType_Point;
}

// =======================================================================
// function : OverlapsBox
// purpose  : Returns true if selecting volume is overlapped by
//            axis-aligned bounding box with minimum corner at point
//            theMinPnt and maximum at point theMaxPnt
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::OverlapsBox(const SelectMgr_Vec3& theBoxMin,
                                                           const SelectMgr_Vec3& theBoxMax,
                                                           Standard_Boolean*     theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");

  return hasBoxOverlap(theBoxMin, theBoxMax, theInside);
}

// =======================================================================
// function : OverlapsBox
// purpose  : SAT intersection test between defined volume and
//            given axis-aligned box
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::OverlapsBox(
  const SelectMgr_Vec3&          theBoxMin,
  const SelectMgr_Vec3&          theBoxMax,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");

  if (!hasBoxOverlap(theBoxMin, theBoxMax))
    return Standard_False;

  Standard_Real             aDepth = 0.0;
  BVH_Ray<Standard_Real, 3> aRay(
    SelectMgr_Vec3(myNearPickedPnt.X(), myNearPickedPnt.Y(), myNearPickedPnt.Z()),
    SelectMgr_Vec3(myViewRayDir.X(), myViewRayDir.Y(), myViewRayDir.Z()));
  Standard_Real aTimeEnter, aTimeLeave;
  if (!BVH_Tools<Standard_Real, 3>::RayBoxIntersection(aRay,
                                                       theBoxMin,
                                                       theBoxMax,
                                                       aTimeEnter,
                                                       aTimeLeave))
  {
    gp_Pnt aNearestPnt(RealLast(), RealLast(), RealLast());
    aNearestPnt.SetX(Max(Min(myNearPickedPnt.X(), theBoxMax.x()), theBoxMin.x()));
    aNearestPnt.SetY(Max(Min(myNearPickedPnt.Y(), theBoxMax.y()), theBoxMin.y()));
    aNearestPnt.SetZ(Max(Min(myNearPickedPnt.Z(), theBoxMax.z()), theBoxMin.z()));

    aDepth = aNearestPnt.Distance(myNearPickedPnt);
    thePickResult.SetDepth(aDepth);
    return !theClipRange.IsClipped(thePickResult.Depth());
  }

  Bnd_Range aRange(Max(aTimeEnter, 0.0), aTimeLeave);
  aRange.GetMin(aDepth);

  if (!theClipRange.GetNearestDepth(aRange, aDepth))
  {
    return Standard_False;
  }

  thePickResult.SetDepth(aDepth);

  return Standard_True;
}

// =======================================================================
// function : OverlapsPoint
// purpose  : Intersection test between defined volume and given point
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::OverlapsPoint(
  const gp_Pnt&                  thePnt,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");

  if (!hasPointOverlap(thePnt))
    return Standard_False;

  gp_XYZ              aV     = thePnt.XYZ() - myNearPickedPnt.XYZ();
  const Standard_Real aDepth = aV.Dot(myViewRayDir.XYZ());

  thePickResult.SetDepth(Abs(aDepth) * myScale);
  thePickResult.SetPickedPoint(thePnt);

  return !theClipRange.IsClipped(thePickResult.Depth());
}

// =======================================================================
// function : OverlapsPoint
// purpose  : Intersection test between defined volume and given point
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::OverlapsPoint(const gp_Pnt& thePnt) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");

  return hasPointOverlap(thePnt);
}

// =======================================================================
// function : OverlapsSegment
// purpose  : Checks if line segment overlaps selecting frustum
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::OverlapsSegment(
  const gp_Pnt&                  thePnt1,
  const gp_Pnt&                  thePnt2,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");

  if (!hasSegmentOverlap(thePnt1, thePnt2))
    return Standard_False;

  segmentSegmentDistance(thePnt1, thePnt2, thePickResult);

  return !theClipRange.IsClipped(thePickResult.Depth());
}

// =======================================================================
// function : OverlapsPolygon
// purpose  : SAT intersection test between defined volume and given
//            ordered set of points, representing line segments. The test
//            may be considered of interior part or boundary line defined
//            by segments depending on given sensitivity type
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::OverlapsPolygon(
  const TColgp_Array1OfPnt&      theArrayOfPnts,
  Select3D_TypeOfSensitivity     theSensType,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");

  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    Standard_Integer        aMatchingSegmentsNb = -1;
    SelectBasics_PickResult aPickResult;
    thePickResult.Invalidate();
    const Standard_Integer aLower  = theArrayOfPnts.Lower();
    const Standard_Integer anUpper = theArrayOfPnts.Upper();
    for (Standard_Integer aPntIter = aLower; aPntIter <= anUpper; ++aPntIter)
    {
      const gp_Pnt& aStartPnt = theArrayOfPnts.Value(aPntIter);
      const gp_Pnt& aEndPnt   = theArrayOfPnts.Value(aPntIter == anUpper ? aLower : (aPntIter + 1));
      if (hasSegmentOverlap(aStartPnt, aEndPnt))
      {
        aMatchingSegmentsNb++;
        segmentSegmentDistance(aStartPnt, aEndPnt, aPickResult);
        thePickResult = SelectBasics_PickResult::Min(thePickResult, aPickResult);
      }
    }

    if (aMatchingSegmentsNb == -1)
      return Standard_False;
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    gp_Vec aPolyNorm(gp_XYZ(RealLast(), RealLast(), RealLast()));
    if (!hasPolygonOverlap(theArrayOfPnts, aPolyNorm))
    {
      return Standard_False;
    }

    if (aPolyNorm.Magnitude() <= Precision::Confusion())
    {
      // treat degenerated polygon as point
      return OverlapsPoint(theArrayOfPnts.First(), theClipRange, thePickResult);
    }
    else if (!segmentPlaneIntersection(aPolyNorm, theArrayOfPnts.First(), thePickResult))
    {
      return Standard_False;
    }
  }

  return !theClipRange.IsClipped(thePickResult.Depth());
}

// =======================================================================
// function : OverlapsTriangle
// purpose  : SAT intersection test between defined volume and given
//            triangle. The test may be considered of interior part or
//            boundary line defined by triangle vertices depending on
//            given sensitivity type
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::OverlapsTriangle(
  const gp_Pnt&                  thePnt1,
  const gp_Pnt&                  thePnt2,
  const gp_Pnt&                  thePnt3,
  Select3D_TypeOfSensitivity     theSensType,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");

  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    const gp_Pnt             aPntsArrayBuf[4] = {thePnt1, thePnt2, thePnt3, thePnt1};
    const TColgp_Array1OfPnt aPntsArray(aPntsArrayBuf[0], 1, 4);
    return OverlapsPolygon(aPntsArray, Select3D_TOS_BOUNDARY, theClipRange, thePickResult);
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    gp_Vec aTriangleNormal(gp_XYZ(RealLast(), RealLast(), RealLast()));
    if (!hasTriangleOverlap(thePnt1, thePnt2, thePnt3, aTriangleNormal))
    {
      return Standard_False;
    }

    const gp_XYZ aTrEdges[3] = {thePnt2.XYZ() - thePnt1.XYZ(),
                                thePnt3.XYZ() - thePnt2.XYZ(),
                                thePnt1.XYZ() - thePnt3.XYZ()};
    if (aTriangleNormal.SquareMagnitude() < gp::Resolution())
    {
      // consider degenerated triangle as point or segment
      return aTrEdges[0].SquareModulus() > gp::Resolution()
               ? OverlapsSegment(thePnt1, thePnt2, theClipRange, thePickResult)
               : (aTrEdges[1].SquareModulus() > gp::Resolution()
                    ? OverlapsSegment(thePnt2, thePnt3, theClipRange, thePickResult)
                    : OverlapsPoint(thePnt1, theClipRange, thePickResult));
    }

    const gp_Pnt        aPnts[3] = {thePnt1, thePnt2, thePnt3};
    const Standard_Real anAlpha  = aTriangleNormal.XYZ().Dot(myViewRayDir.XYZ());
    if (Abs(anAlpha) < gp::Resolution())
    {
      // handle the case when triangle normal and selecting frustum direction are orthogonal
      SelectBasics_PickResult aPickResult;
      thePickResult.Invalidate();
      for (Standard_Integer anEdgeIter = 0; anEdgeIter < 3; ++anEdgeIter)
      {
        const gp_Pnt& aStartPnt = aPnts[anEdgeIter];
        const gp_Pnt& anEndPnt  = aPnts[anEdgeIter < 2 ? anEdgeIter + 1 : 0];
        segmentSegmentDistance(aStartPnt, anEndPnt, aPickResult);
        thePickResult = SelectBasics_PickResult::Min(thePickResult, aPickResult);
      }
      thePickResult.SetSurfaceNormal(aTriangleNormal);
      return !theClipRange.IsClipped(thePickResult.Depth());
    }

    // check if intersection point belongs to triangle's interior part
    const gp_XYZ anEdge = (thePnt1.XYZ() - myNearPickedPnt.XYZ()) * (1.0 / anAlpha);

    const Standard_Real aTime = aTriangleNormal.Dot(anEdge);
    const gp_XYZ        aVec  = myViewRayDir.XYZ().Crossed(anEdge);
    const Standard_Real anU   = aVec.Dot(aTrEdges[2]);
    const Standard_Real aV    = aVec.Dot(aTrEdges[0]);

    const Standard_Boolean isInterior =
      (aTime >= 0.0) && (anU >= 0.0) && (aV >= 0.0) && (anU + aV <= 1.0);
    const gp_Pnt aPtOnPlane = myNearPickedPnt.XYZ() + myViewRayDir.XYZ() * aTime;
    if (isInterior)
    {
      thePickResult.SetDepth(myNearPickedPnt.Distance(aPtOnPlane) * myScale);
      thePickResult.SetPickedPoint(aPtOnPlane);
      thePickResult.SetSurfaceNormal(aTriangleNormal);
      return !theClipRange.IsClipped(thePickResult.Depth());
    }

    Standard_Real    aMinDist         = RealLast();
    Standard_Integer aNearestEdgeIdx1 = -1;
    for (Standard_Integer anEdgeIdx = 0; anEdgeIdx < 3; ++anEdgeIdx)
    {
      gp_XYZ        aW = aPtOnPlane.XYZ() - aPnts[anEdgeIdx].XYZ();
      Standard_Real aCoef =
        aTrEdges[anEdgeIdx].Dot(aW) / aTrEdges[anEdgeIdx].Dot(aTrEdges[anEdgeIdx]);
      Standard_Real aDist =
        aPtOnPlane.Distance(aPnts[anEdgeIdx].XYZ() + aCoef * aTrEdges[anEdgeIdx]);
      if (aDist < aMinDist)
      {
        aMinDist         = aDist;
        aNearestEdgeIdx1 = anEdgeIdx;
      }
    }
    Standard_Integer aNearestEdgeIdx2 = (aNearestEdgeIdx1 + 1) % 3;
    const gp_Vec     aVec12(aPnts[aNearestEdgeIdx1], aPnts[aNearestEdgeIdx2]);
    if (aVec12.SquareMagnitude() > gp::Resolution()
        && myViewRayDir.IsParallel(aVec12, Precision::Angular()))
    {
      aNearestEdgeIdx2 = aNearestEdgeIdx1 == 0 ? 2 : aNearestEdgeIdx1 - 1;
    }
    segmentSegmentDistance(aPnts[aNearestEdgeIdx1], aPnts[aNearestEdgeIdx2], thePickResult);
    thePickResult.SetSurfaceNormal(aTriangleNormal);
  }

  return !theClipRange.IsClipped(thePickResult.Depth());
}

//=================================================================================================

Standard_Boolean SelectMgr_RectangularFrustum::OverlapsCylinder(
  const Standard_Real            theBottomRad,
  const Standard_Real            theTopRad,
  const Standard_Real            theHeight,
  const gp_Trsf&                 theTrsf,
  const Standard_Boolean         theIsHollow,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");
  Standard_Real aTimes[2] = {0.0, 0.0};
  const gp_Trsf aTrsfInv  = theTrsf.Inverted();
  const gp_Pnt  aLoc      = myNearPickedPnt.Transformed(aTrsfInv);
  const gp_Dir  aRayDir   = myViewRayDir.Transformed(aTrsfInv);
  if (!RayCylinderIntersection(theBottomRad,
                               theTopRad,
                               theHeight,
                               aLoc,
                               aRayDir,
                               theIsHollow,
                               aTimes[0],
                               aTimes[1]))
  {
    return Standard_False;
  }

  Standard_Integer aResTime = 0;
  thePickResult.SetDepth(aTimes[aResTime] * myScale);
  if (theClipRange.IsClipped(thePickResult.Depth()))
  {
    aResTime = 1;
    thePickResult.SetDepth(aTimes[aResTime] * myScale);
  }

  const gp_Pnt aPntOnCylinder = aLoc.XYZ() + aRayDir.XYZ() * aTimes[aResTime];
  if (Abs(aPntOnCylinder.Z()) < Precision::Confusion())
  {
    thePickResult.SetSurfaceNormal(-gp::DZ().Transformed(theTrsf));
  }
  else if (Abs(aPntOnCylinder.Z() - theHeight) < Precision::Confusion())
  {
    thePickResult.SetSurfaceNormal(gp::DZ().Transformed(theTrsf));
  }
  else
  {
    thePickResult.SetSurfaceNormal(
      gp_Vec(aPntOnCylinder.X(), aPntOnCylinder.Y(), 0.0).Transformed(theTrsf));
  }
  thePickResult.SetPickedPoint(aPntOnCylinder.Transformed(theTrsf));
  return !theClipRange.IsClipped(thePickResult.Depth());
}

//=================================================================================================

Standard_Boolean SelectMgr_RectangularFrustum::OverlapsCircle(
  const Standard_Real            theRadius,
  const gp_Trsf&                 theTrsf,
  const Standard_Boolean         theIsFilled,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");
  Standard_Real aTime    = 0.0;
  const gp_Trsf aTrsfInv = theTrsf.Inverted();
  const gp_Pnt  aLoc     = myNearPickedPnt.Transformed(aTrsfInv);
  const gp_Dir  aRayDir  = myViewRayDir.Transformed(aTrsfInv);
  if (!theIsFilled)
  {
    if (!hasCircleOverlap(theRadius, theTrsf, theIsFilled, NULL))
    {
      return Standard_False;
    }
    if (aRayDir.Z() != 0)
    {
      aTime = (0 - aLoc.Z()) / aRayDir.Z();
    }
  }
  else if (!RayCircleIntersection(theRadius, aLoc, aRayDir, theIsFilled, aTime))
  {
    return Standard_False;
  }

  thePickResult.SetDepth(aTime * myScale);
  if (theClipRange.IsClipped(thePickResult.Depth()))
  {
    thePickResult.SetDepth(aTime * myScale);
  }

  const gp_Pnt aPntOnCircle = aLoc.XYZ() + aRayDir.XYZ() * aTime;
  if (Abs(aPntOnCircle.Z()) < Precision::Confusion())
  {
    thePickResult.SetSurfaceNormal(-gp::DZ().Transformed(theTrsf));
  }
  else
  {
    thePickResult.SetSurfaceNormal(
      gp_Vec(aPntOnCircle.X(), aPntOnCircle.Y(), 0.0).Transformed(theTrsf));
  }
  thePickResult.SetPickedPoint(aPntOnCircle.Transformed(theTrsf));
  return !theClipRange.IsClipped(thePickResult.Depth());
}

//=================================================================================================

Standard_Boolean SelectMgr_RectangularFrustum::isIntersectCircle(
  const Standard_Real       theRadius,
  const gp_Pnt&             theCenter,
  const gp_Trsf&            theTrsf,
  const TColgp_Array1OfPnt& theVertices) const
{
  const gp_Trsf aTrsfInv = theTrsf.Inverted();
  const gp_Dir  aRayDir  = gp_Dir(myEdgeDirs[4 == 4 ? 4 : 0]).Transformed(aTrsfInv);
  if (aRayDir.Z() == 0.0)
  {
    return false;
  }

  for (Standard_Integer anIdx = theVertices.Lower(); anIdx <= theVertices.Upper(); anIdx++)
  {
    const gp_Pnt aPntStart  = theVertices.Value(anIdx).Transformed(aTrsfInv);
    const gp_Pnt aPntFinish = anIdx == theVertices.Upper()
                                ? theVertices.Value(theVertices.Lower()).Transformed(aTrsfInv)
                                : theVertices.Value(anIdx + 1).Transformed(aTrsfInv);

    // Project points on the end face plane
    const Standard_Real aParam1 = (theCenter.Z() - aPntStart.Z()) / aRayDir.Z();
    const Standard_Real aX1     = aPntStart.X() + aRayDir.X() * aParam1;
    const Standard_Real anY1    = aPntStart.Y() + aRayDir.Y() * aParam1;

    const Standard_Real aParam2 = (theCenter.Z() - aPntFinish.Z()) / aRayDir.Z();
    const Standard_Real aX2     = aPntFinish.X() + aRayDir.X() * aParam2;
    const Standard_Real anY2    = aPntFinish.Y() + aRayDir.Y() * aParam2;

    // Solving quadratic equation anA * T^2 + 2 * aK * T + aC = 0
    const Standard_Real anA = (aX1 - aX2) * (aX1 - aX2) + (anY1 - anY2) * (anY1 - anY2);
    const Standard_Real aK  = aX1 * (aX2 - aX1) + anY1 * (anY2 - anY1);
    const Standard_Real aC  = aX1 * aX1 + anY1 * anY1 - theRadius * theRadius;

    const Standard_Real aDiscr = aK * aK - anA * aC;
    if (aDiscr >= 0.0)
    {
      const Standard_Real aT1 = (-aK + Sqrt(aDiscr)) / anA;
      const Standard_Real aT2 = (-aK - Sqrt(aDiscr)) / anA;
      if ((aT1 >= 0 && aT1 <= 1) || (aT2 >= 0 && aT2 <= 1))
      {
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

Standard_Boolean SelectMgr_RectangularFrustum::isSegmentsIntersect(const gp_Pnt& thePnt1Seg1,
                                                                   const gp_Pnt& thePnt2Seg1,
                                                                   const gp_Pnt& thePnt1Seg2,
                                                                   const gp_Pnt& thePnt2Seg2) const
{
  const gp_Mat aMatPln(thePnt2Seg1.X() - thePnt1Seg1.X(),
                       thePnt2Seg1.Y() - thePnt1Seg1.Y(),
                       thePnt2Seg1.Z() - thePnt1Seg1.Z(),
                       thePnt1Seg2.X() - thePnt1Seg1.X(),
                       thePnt1Seg2.Y() - thePnt1Seg1.Y(),
                       thePnt1Seg2.Z() - thePnt1Seg1.Z(),
                       thePnt2Seg2.X() - thePnt1Seg1.X(),
                       thePnt2Seg2.Y() - thePnt1Seg1.Y(),
                       thePnt2Seg2.Z() - thePnt1Seg1.Z());
  if (Abs(aMatPln.Determinant()) > Precision::Confusion())
  {
    return false;
  }

  Standard_Real aFst[4] = {thePnt1Seg1.X(), thePnt2Seg1.X(), thePnt1Seg2.X(), thePnt2Seg2.X()};
  Standard_Real aSnd[4] = {thePnt1Seg1.Y(), thePnt2Seg1.Y(), thePnt1Seg2.Y(), thePnt2Seg2.Y()};
  if (aFst[0] == aFst[2] && aFst[1] == aFst[3])
  {
    aFst[0] = thePnt1Seg1.Z();
    aFst[1] = thePnt2Seg1.Z();
    aFst[2] = thePnt1Seg2.Z();
    aFst[3] = thePnt2Seg2.Z();
  }
  if (aSnd[0] == aSnd[2] && aSnd[1] == aSnd[3])
  {
    aSnd[0] = thePnt1Seg1.Z();
    aSnd[1] = thePnt2Seg1.Z();
    aSnd[2] = thePnt1Seg2.Z();
    aSnd[3] = thePnt2Seg2.Z();
  }
  const gp_Mat2d aMat(gp_XY(aFst[0] - aFst[1], aSnd[0] - aSnd[1]),
                      gp_XY(aFst[3] - aFst[2], aSnd[3] - aSnd[2]));

  const gp_Mat2d aMatU(gp_XY(aFst[0] - aFst[2], aSnd[0] - aSnd[2]),
                       gp_XY(aFst[3] - aFst[2], aSnd[3] - aSnd[2]));

  const gp_Mat2d aMatV(gp_XY(aFst[0] - aFst[1], aSnd[0] - aSnd[1]),
                       gp_XY(aFst[0] - aFst[2], aSnd[0] - aSnd[2]));
  if (aMat.Determinant() == 0.0)
  {
    return false;
  }

  const Standard_Real anU = aMatU.Determinant() / aMat.Determinant();
  const Standard_Real aV  = aMatV.Determinant() / aMat.Determinant();
  if (anU >= 0.0 && anU <= 1.0 && aV >= 0.0 && aV <= 1.0)
  {
    return true;
  }
  return false;
}

//=================================================================================================

Standard_Boolean SelectMgr_RectangularFrustum::OverlapsCylinder(const Standard_Real    theBottomRad,
                                                                const Standard_Real    theTopRad,
                                                                const Standard_Real    theHeight,
                                                                const gp_Trsf&         theTrsf,
                                                                const Standard_Boolean theIsHollow,
                                                                Standard_Boolean* theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");

  return hasCylinderOverlap(theBottomRad, theTopRad, theHeight, theTrsf, theIsHollow, theInside);
}

//=================================================================================================

Standard_Boolean SelectMgr_RectangularFrustum::OverlapsCircle(const Standard_Real    theRadius,
                                                              const gp_Trsf&         theTrsf,
                                                              const Standard_Boolean theIsFilled,
                                                              Standard_Boolean* theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");

  return hasCircleOverlap(theRadius, theTrsf, theIsFilled, theInside);
}

// =======================================================================
// function : GetMousePosition
// purpose  :
// =======================================================================
const gp_Pnt2d& SelectMgr_RectangularFrustum::GetMousePosition() const
{
  if (mySelectionType == SelectMgr_SelectionType_Point)
  {
    return mySelRectangle.MousePos();
  }
  return base_type::GetMousePosition();
}

// =======================================================================
// function : OverlapsSphere
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::OverlapsSphere(
  const gp_Pnt&                  theCenter,
  const Standard_Real            theRadius,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");
  Standard_Real aTimeEnter = 0.0, aTimeLeave = 0.0;
  if (!RaySphereIntersection(theCenter,
                             theRadius,
                             myNearPickedPnt,
                             myViewRayDir,
                             aTimeEnter,
                             aTimeLeave))
  {
    return Standard_False;
  }

  thePickResult.SetDepth(aTimeEnter * myScale);
  if (theClipRange.IsClipped(thePickResult.Depth()))
  {
    thePickResult.SetDepth(aTimeLeave * myScale);
  }
  gp_Pnt aPntOnSphere(myNearPickedPnt.XYZ() + myViewRayDir.XYZ() * thePickResult.Depth() / myScale);
  gp_Vec aNormal(aPntOnSphere.XYZ() - theCenter.XYZ());
  thePickResult.SetPickedPoint(aPntOnSphere);
  thePickResult.SetSurfaceNormal(aNormal);
  return !theClipRange.IsClipped(thePickResult.Depth());
}

// =======================================================================
// function : OverlapsSphere
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::OverlapsSphere(const gp_Pnt&       theCenter,
                                                              const Standard_Real theRadius,
                                                              Standard_Boolean*   theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::Overlaps() should be called after "
                        "selection frustum initialization");
  return hasSphereOverlap(theCenter, theRadius, theInside);
}

// =======================================================================
// function : DistToGeometryCenter
// purpose  : Measures distance between 3d projection of user-picked
//            screen point and given point theCOG
// =======================================================================
Standard_Real SelectMgr_RectangularFrustum::DistToGeometryCenter(const gp_Pnt& theCOG) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point
                          || mySelectionType == SelectMgr_SelectionType_Box,
                        "Error! SelectMgr_RectangularFrustum::DistToGeometryCenter() should be "
                        "called after selection frustum initialization");

  return theCOG.Distance(myNearPickedPnt) * myScale;
}

// =======================================================================
// function : DetectedPoint
// purpose  : Calculates the point on a view ray that was detected during
//            the run of selection algo by given depth
// =======================================================================
gp_Pnt SelectMgr_RectangularFrustum::DetectedPoint(const Standard_Real theDepth) const
{
  Standard_ASSERT_RAISE(
    mySelectionType == SelectMgr_SelectionType_Point,
    "SelectMgr_RectangularFrustum::DetectedPoint() should be called only for Point selection type");
  return myNearPickedPnt.XYZ() + myViewRayDir.XYZ() * theDepth / myScale;
}

// =======================================================================
// function : GetPlanes
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::GetPlanes(
  NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const
{
  thePlaneEquations.Clear();

  SelectMgr_Vec4 anEquation;
  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 6; ++aPlaneIdx)
  {
    const gp_Vec& aPlaneNorm = Camera()->IsOrthographic() && aPlaneIdx % 2 == 1
                                 ? myPlanes[aPlaneIdx - 1].Reversed()
                                 : myPlanes[aPlaneIdx];
    anEquation.x()           = aPlaneNorm.X();
    anEquation.y()           = aPlaneNorm.Y();
    anEquation.z()           = aPlaneNorm.Z();
    anEquation.w() =
      -(aPlaneNorm.XYZ().Dot(myVertices[aPlaneIdx % 2 == 0 ? aPlaneIdx : aPlaneIdx + 2].XYZ()));
    thePlaneEquations.Append(anEquation);
  }
}

//=================================================================================================

void SelectMgr_RectangularFrustum::DumpJson(Standard_OStream& theOStream,
                                            Standard_Integer  theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, SelectMgr_RectangularFrustum)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, SelectMgr_Frustum)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myNearPickedPnt)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myFarPickedPnt)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myViewRayDir)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &mySelRectangle.MinPnt())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &mySelRectangle.MaxPnt())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myScale)
}
