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

#include <SelectMgr_AxisIntersector.hxx>

#include <BVH_Tools.hxx>
#include <Precision.hxx>
#include <SelectBasics_PickResult.hxx>
#include <SelectMgr_ViewClipRange.hxx>

//=================================================================================================

SelectMgr_AxisIntersector::SelectMgr_AxisIntersector()
{
  //
}

//=================================================================================================

SelectMgr_AxisIntersector::~SelectMgr_AxisIntersector()
{
  //
}

//=================================================================================================

void SelectMgr_AxisIntersector::Init(const gp_Ax1& theAxis)
{
  mySelectionType = SelectMgr_SelectionType_Point;
  myAxis          = theAxis;
}

//=================================================================================================

void SelectMgr_AxisIntersector::Build() {}

//=================================================================================================

void SelectMgr_AxisIntersector::SetCamera(const occ::handle<Graphic3d_Camera>&) {}

//=================================================================================================

occ::handle<SelectMgr_BaseIntersector> SelectMgr_AxisIntersector::ScaleAndTransform(
  const int                                    theScaleFactor,
  const gp_GTrsf&                              theTrsf,
  const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::ScaleAndTransform() should be called "
                        "after selection axis initialization");

  (void)theScaleFactor;
  (void)theBuilder;
  if (theTrsf.Form() == gp_Identity)
  {
    return new SelectMgr_AxisIntersector();
  }

  gp_Pnt aTransformedLoc = myAxis.Location();
  theTrsf.Transforms(aTransformedLoc.ChangeCoord());
  gp_XYZ   aTransformedDir = myAxis.Direction().XYZ();
  gp_GTrsf aTrsf           = theTrsf;
  aTrsf.SetTranslationPart(gp_XYZ(0., 0., 0.));
  aTrsf.Transforms(aTransformedDir);

  occ::handle<SelectMgr_AxisIntersector> aRes = new SelectMgr_AxisIntersector();
  aRes->myAxis                                = gp_Ax1(aTransformedLoc, gp_Dir(aTransformedDir));
  aRes->mySelectionType                       = mySelectionType;
  return aRes;
}

//=======================================================================
// function : CopyWithBuilder
// purpose  : Returns a copy of the frustum using the given frustum builder configuration.
//            Returned frustum should be re-constructed before being used.
//=======================================================================
occ::handle<SelectMgr_BaseIntersector> SelectMgr_AxisIntersector::CopyWithBuilder(
  const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const
{
  (void)theBuilder;
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::CopyWithBuilder() should be called "
                        "after selection axis initialization");

  occ::handle<SelectMgr_AxisIntersector> aRes = new SelectMgr_AxisIntersector();
  aRes->myAxis                                = myAxis;
  aRes->mySelectionType                       = mySelectionType;

  return aRes;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::hasIntersection(const NCollection_Vec3<double>& theBoxMin,
                                                const NCollection_Vec3<double>& theBoxMax,
                                                double&                         theTimeEnter,
                                                double&                         theTimeLeave) const
{
  const gp_Pnt&      anAxisLoc = myAxis.Location();
  const gp_Dir&      anAxisDir = myAxis.Direction();
  BVH_Ray<double, 3> aRay(NCollection_Vec3<double>(anAxisLoc.X(), anAxisLoc.Y(), anAxisLoc.Z()),
                          NCollection_Vec3<double>(anAxisDir.X(), anAxisDir.Y(), anAxisDir.Z()));
  return BVH_Tools<double, 3>::RayBoxIntersection(aRay,
                                                  theBoxMin,
                                                  theBoxMax,
                                                  theTimeEnter,
                                                  theTimeLeave);
}

//=================================================================================================

bool SelectMgr_AxisIntersector::hasIntersection(const gp_Pnt& thePnt, double& theDepth) const
{
  const gp_Pnt& anAxisLoc = myAxis.Location();
  const gp_Dir& anAxisDir = myAxis.Direction();

  // Check that vectors are co-directed (thePnt lies on this axis)
  gp_Dir aDirToPnt(thePnt.XYZ() - anAxisLoc.XYZ());
  if (!anAxisDir.IsEqual(aDirToPnt, Precision::Angular()))
  {
    return false;
  }
  theDepth = anAxisLoc.Distance(thePnt);
  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::raySegmentDistance(const gp_Pnt&            theSegPnt1,
                                                   const gp_Pnt&            theSegPnt2,
                                                   SelectBasics_PickResult& thePickResult) const
{
  const gp_XYZ anU = theSegPnt2.XYZ() - theSegPnt1.XYZ();
  const gp_XYZ aV  = myAxis.Direction().XYZ();
  const gp_XYZ aW  = theSegPnt1.XYZ() - myAxis.Location().XYZ();

  const gp_XYZ anUVNormVec    = aV.Crossed(anU);
  const double anUVNormVecMod = anUVNormVec.Modulus();
  if (anUVNormVecMod <= Precision::Confusion())
  {
    // Lines have no intersection
    thePickResult.Invalidate();
    return false;
  }

  const gp_XYZ anUWNormVec    = aW.Crossed(anU);
  const double anUWNormVecMod = anUWNormVec.Modulus();
  if (anUWNormVecMod <= Precision::Confusion())
  {
    // Lines have no intersection
    thePickResult.Invalidate();
    return false;
  }

  const double aParam = anUWNormVec.Dot(anUVNormVec) / anUVNormVec.SquareModulus();
  if (aParam < 0.0)
  {
    // Intersection is out of axis start point
    thePickResult.Invalidate();
    return false;
  }

  const gp_XYZ anIntersectPnt = myAxis.Location().XYZ() + aV * aParam;
  if ((anIntersectPnt - theSegPnt1.XYZ()).Modulus() + (anIntersectPnt - theSegPnt2.XYZ()).Modulus()
      > anU.Modulus() + Precision::Confusion())
  {
    // Intersection point doesn't lie on the segment
    thePickResult.Invalidate();
    return false;
  }

  thePickResult.SetDepth(myAxis.Location().Distance(anIntersectPnt));
  thePickResult.SetPickedPoint(anIntersectPnt);
  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::rayPlaneIntersection(const gp_Vec&            thePlane,
                                                     const gp_Pnt&            thePntOnPlane,
                                                     SelectBasics_PickResult& thePickResult) const
{
  gp_XYZ anU = myAxis.Direction().XYZ();
  gp_XYZ aW  = myAxis.Location().XYZ() - thePntOnPlane.XYZ();
  double aD  = thePlane.Dot(anU);
  double aN  = -thePlane.Dot(aW);

  if (std::abs(aD) < Precision::Confusion())
  {
    thePickResult.Invalidate();
    return false;
  }

  double aParam = aN / aD;
  if (aParam < 0.0)
  {
    thePickResult.Invalidate();
    return false;
  }

  gp_Pnt aClosestPnt = myAxis.Location().XYZ() + anU * aParam;
  thePickResult.SetDepth(myAxis.Location().Distance(aClosestPnt));
  thePickResult.SetPickedPoint(aClosestPnt);
  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                                            const NCollection_Vec3<double>& theBoxMax,
                                            bool*                           theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsBox() should be called after "
                        "selection axis initialization");

  (void)theInside;
  double aTimeEnter, aTimeLeave;
  if (!hasIntersection(theBoxMin, theBoxMax, aTimeEnter, aTimeLeave))
  {
    return false;
  }
  if (theInside != nullptr)
  {
    *theInside &= (aTimeEnter >= 0.0);
  }
  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                                            const NCollection_Vec3<double>& theBoxMax,
                                            const SelectMgr_ViewClipRange&  theClipRange,
                                            SelectBasics_PickResult&        thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsBox() should be called after "
                        "selection axis initialization");

  double aTimeEnter, aTimeLeave;
  if (!hasIntersection(theBoxMin, theBoxMax, aTimeEnter, aTimeLeave))
  {
    return false;
  }

  double    aDepth = 0.0;
  Bnd_Range aRange(std::max(aTimeEnter, 0.0), aTimeLeave);
  aRange.GetMin(aDepth);

  if (!theClipRange.GetNearestDepth(aRange, aDepth))
  {
    return false;
  }

  thePickResult.SetDepth(aDepth);

  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsPoint(const gp_Pnt&                  thePnt,
                                              const SelectMgr_ViewClipRange& theClipRange,
                                              SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsPoint() should be called after "
                        "selection axis initialization");

  double aDepth = 0.0;
  if (!hasIntersection(thePnt, aDepth))
  {
    return false;
  }

  thePickResult.SetDepth(aDepth);
  thePickResult.SetPickedPoint(thePnt);

  return !theClipRange.IsClipped(thePickResult.Depth());
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsPoint(const gp_Pnt& thePnt) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsPoint() should be called after "
                        "selection axis initialization");

  double aDepth = 0.0;
  return hasIntersection(thePnt, aDepth);
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsSegment(const gp_Pnt&                  thePnt1,
                                                const gp_Pnt&                  thePnt2,
                                                const SelectMgr_ViewClipRange& theClipRange,
                                                SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsSegment() should be called "
                        "after selection axis initialization");

  if (!raySegmentDistance(thePnt1, thePnt2, thePickResult))
  {
    return false;
  }

  return !theClipRange.IsClipped(thePickResult.Depth());
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsPolygon(const NCollection_Array1<gp_Pnt>& theArrayOfPnts,
                                                Select3D_TypeOfSensitivity        theSensType,
                                                const SelectMgr_ViewClipRange&    theClipRange,
                                                SelectBasics_PickResult& thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsPolygon() should be called "
                        "after selection axis initialization");

  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    int                     aMatchingSegmentsNb = -1;
    SelectBasics_PickResult aPickResult;
    thePickResult.Invalidate();
    const int aLower  = theArrayOfPnts.Lower();
    const int anUpper = theArrayOfPnts.Upper();
    for (int aPntIter = aLower; aPntIter <= anUpper; ++aPntIter)
    {
      const gp_Pnt& aStartPnt = theArrayOfPnts.Value(aPntIter);
      const gp_Pnt& aEndPnt   = theArrayOfPnts.Value(aPntIter == anUpper ? aLower : (aPntIter + 1));
      if (raySegmentDistance(aStartPnt, aEndPnt, aPickResult))
      {
        aMatchingSegmentsNb++;
        thePickResult = SelectBasics_PickResult::Min(thePickResult, aPickResult);
      }
    }

    if (aMatchingSegmentsNb == -1)
    {
      return false;
    }
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    int           aStartIdx = theArrayOfPnts.Lower();
    const gp_XYZ& aPnt1     = theArrayOfPnts.Value(aStartIdx).XYZ();
    const gp_XYZ& aPnt2     = theArrayOfPnts.Value(aStartIdx + 1).XYZ();
    const gp_XYZ& aPnt3     = theArrayOfPnts.Value(aStartIdx + 2).XYZ();
    const gp_XYZ  aVec1     = aPnt1 - aPnt2;
    const gp_XYZ  aVec2     = aPnt3 - aPnt2;
    gp_Vec        aPolyNorm = aVec2.Crossed(aVec1);
    if (aPolyNorm.Magnitude() <= Precision::Confusion())
    {
      // treat degenerated polygon as point
      return OverlapsPoint(theArrayOfPnts.First(), theClipRange, thePickResult);
    }
    else if (!rayPlaneIntersection(aPolyNorm, theArrayOfPnts.First(), thePickResult))
    {
      return false;
    }
  }

  return !theClipRange.IsClipped(thePickResult.Depth());
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsTriangle(const gp_Pnt&                  thePnt1,
                                                 const gp_Pnt&                  thePnt2,
                                                 const gp_Pnt&                  thePnt3,
                                                 Select3D_TypeOfSensitivity     theSensType,
                                                 const SelectMgr_ViewClipRange& theClipRange,
                                                 SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsTriangle() should be called "
                        "after selection axis initialization");

  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    const gp_Pnt                     aPntsArrayBuf[4] = {thePnt1, thePnt2, thePnt3, thePnt1};
    const NCollection_Array1<gp_Pnt> aPntsArray(aPntsArrayBuf[0], 1, 4);
    return OverlapsPolygon(aPntsArray, Select3D_TOS_BOUNDARY, theClipRange, thePickResult);
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    gp_Vec       aTriangleNormal(gp_XYZ(RealLast(), RealLast(), RealLast()));
    const gp_XYZ aTrEdges[3] = {thePnt2.XYZ() - thePnt1.XYZ(),
                                thePnt3.XYZ() - thePnt2.XYZ(),
                                thePnt1.XYZ() - thePnt3.XYZ()};
    aTriangleNormal          = aTrEdges[2].Crossed(aTrEdges[0]);
    if (aTriangleNormal.SquareMagnitude() < gp::Resolution())
    {
      // consider degenerated triangle as point or segment
      return aTrEdges[0].SquareModulus() > gp::Resolution()
               ? OverlapsSegment(thePnt1, thePnt2, theClipRange, thePickResult)
               : (aTrEdges[1].SquareModulus() > gp::Resolution()
                    ? OverlapsSegment(thePnt2, thePnt3, theClipRange, thePickResult)
                    : OverlapsPoint(thePnt1, theClipRange, thePickResult));
    }

    const gp_Pnt aPnts[3] = {thePnt1, thePnt2, thePnt3};
    const double anAlpha  = aTriangleNormal.XYZ().Dot(myAxis.Direction().XYZ());
    if (std::abs(anAlpha) < gp::Resolution())
    {
      // handle the case when triangle normal and selecting frustum direction are orthogonal
      SelectBasics_PickResult aPickResult;
      thePickResult.Invalidate();
      for (int anEdgeIter = 0; anEdgeIter < 3; ++anEdgeIter)
      {
        const gp_Pnt& aStartPnt = aPnts[anEdgeIter];
        const gp_Pnt& anEndPnt  = aPnts[anEdgeIter < 2 ? anEdgeIter + 1 : 0];
        if (raySegmentDistance(aStartPnt, anEndPnt, aPickResult))
        {
          thePickResult = SelectBasics_PickResult::Min(thePickResult, aPickResult);
        }
      }
      thePickResult.SetSurfaceNormal(aTriangleNormal);
      return thePickResult.IsValid() && !theClipRange.IsClipped(thePickResult.Depth());
    }

    // check if intersection point belongs to triangle's interior part
    const gp_XYZ anEdge = (thePnt1.XYZ() - myAxis.Location().XYZ()) * (1.0 / anAlpha);

    const double aTime = aTriangleNormal.Dot(anEdge);
    const gp_XYZ aVec  = myAxis.Direction().XYZ().Crossed(anEdge);
    const double anU   = aVec.Dot(aTrEdges[2]);
    const double aV    = aVec.Dot(aTrEdges[0]);

    const bool   isInterior = (aTime >= 0.0) && (anU >= 0.0) && (aV >= 0.0) && (anU + aV <= 1.0);
    const gp_Pnt aPtOnPlane = myAxis.Location().XYZ() + myAxis.Direction().XYZ() * aTime;
    if (isInterior)
    {
      thePickResult.SetDepth(myAxis.Location().Distance(aPtOnPlane));
      thePickResult.SetPickedPoint(aPtOnPlane);
      thePickResult.SetSurfaceNormal(aTriangleNormal);
      return !theClipRange.IsClipped(thePickResult.Depth());
    }

    double aMinDist         = RealLast();
    int    aNearestEdgeIdx1 = -1;
    for (int anEdgeIdx = 0; anEdgeIdx < 3; ++anEdgeIdx)
    {
      gp_XYZ aW    = aPtOnPlane.XYZ() - aPnts[anEdgeIdx].XYZ();
      double aCoef = aTrEdges[anEdgeIdx].Dot(aW) / aTrEdges[anEdgeIdx].Dot(aTrEdges[anEdgeIdx]);
      double aDist = aPtOnPlane.Distance(aPnts[anEdgeIdx].XYZ() + aCoef * aTrEdges[anEdgeIdx]);
      if (aDist < aMinDist)
      {
        aMinDist         = aDist;
        aNearestEdgeIdx1 = anEdgeIdx;
      }
    }
    int          aNearestEdgeIdx2 = (aNearestEdgeIdx1 + 1) % 3;
    const gp_Vec aVec12(aPnts[aNearestEdgeIdx1], aPnts[aNearestEdgeIdx2]);
    if (aVec12.SquareMagnitude() > gp::Resolution()
        && myAxis.Direction().IsParallel(aVec12, Precision::Angular()))
    {
      aNearestEdgeIdx2 = aNearestEdgeIdx1 == 0 ? 2 : aNearestEdgeIdx1 - 1;
    }
    if (raySegmentDistance(aPnts[aNearestEdgeIdx1], aPnts[aNearestEdgeIdx2], thePickResult))
    {
      thePickResult.SetSurfaceNormal(aTriangleNormal);
    }
  }

  return thePickResult.IsValid() && !theClipRange.IsClipped(thePickResult.Depth());
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsSphere(const gp_Pnt& theCenter,
                                               const double  theRadius,
                                               bool*         theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsSphere() should be called after "
                        "selection axis initialization");
  (void)theInside;
  double aTimeEnter = 0.0, aTimeLeave = 0.0;
  if (!RaySphereIntersection(theCenter,
                             theRadius,
                             myAxis.Location(),
                             myAxis.Direction(),
                             aTimeEnter,
                             aTimeLeave))
  {
    return false;
  }
  if (theInside != nullptr)
  {
    *theInside &= (aTimeEnter >= 0.0);
  }
  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsSphere(const gp_Pnt&                  theCenter,
                                               const double                   theRadius,
                                               const SelectMgr_ViewClipRange& theClipRange,
                                               SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsSphere() should be called after "
                        "selection axis initialization");
  double aTimeEnter = 0.0, aTimeLeave = 0.0;
  if (!RaySphereIntersection(theCenter,
                             theRadius,
                             myAxis.Location(),
                             myAxis.Direction(),
                             aTimeEnter,
                             aTimeLeave))
  {
    return false;
  }

  double    aDepth = 0.0;
  Bnd_Range aRange(std::max(aTimeEnter, 0.0), aTimeLeave);
  aRange.GetMin(aDepth);
  if (!theClipRange.GetNearestDepth(aRange, aDepth))
  {
    return false;
  }

  const gp_Pnt aPntOnSphere(myAxis.Location().XYZ() + myAxis.Direction().XYZ() * aDepth);
  const gp_Vec aNormal(aPntOnSphere.XYZ() - theCenter.XYZ());
  thePickResult.SetDepth(aDepth);
  thePickResult.SetPickedPoint(aPntOnSphere);
  thePickResult.SetSurfaceNormal(aNormal);
  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsCylinder(const double                   theBottomRad,
                                                 const double                   theTopRad,
                                                 const double                   theHeight,
                                                 const gp_Trsf&                 theTrsf,
                                                 const bool                     theIsHollow,
                                                 const SelectMgr_ViewClipRange& theClipRange,
                                                 SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsCylinder() should be called "
                        "after selection axis initialization");
  double  aTimeEnter = 0.0, aTimeLeave = 0.0;
  gp_Trsf aTrsfInv = theTrsf.Inverted();
  gp_Pnt  aLoc     = myAxis.Location().Transformed(aTrsfInv);
  gp_Dir  aRayDir  = myAxis.Direction().Transformed(aTrsfInv);
  if (!RayCylinderIntersection(theBottomRad,
                               theTopRad,
                               theHeight,
                               aLoc,
                               aRayDir,
                               theIsHollow,
                               aTimeEnter,
                               aTimeLeave))
  {
    return false;
  }

  double    aDepth = 0.0;
  Bnd_Range aRange(std::max(aTimeEnter, 0.0), std::max(aTimeEnter, aTimeLeave));
  aRange.GetMin(aDepth);
  if (!theClipRange.GetNearestDepth(aRange, aDepth))
  {
    return false;
  }

  const gp_Pnt aPntOnCylinder = aLoc.XYZ() + aRayDir.XYZ() * aDepth;
  thePickResult.SetDepth(aDepth);
  thePickResult.SetPickedPoint(aPntOnCylinder.Transformed(theTrsf));
  if (std::abs(aPntOnCylinder.Z()) < Precision::Confusion())
  {
    thePickResult.SetSurfaceNormal(-gp::DZ().Transformed(theTrsf));
  }
  else if (std::abs(aPntOnCylinder.Z() - theHeight) < Precision::Confusion())
  {
    thePickResult.SetSurfaceNormal(gp::DZ().Transformed(theTrsf));
  }
  else
  {
    thePickResult.SetSurfaceNormal(
      gp_Vec(aPntOnCylinder.X(), aPntOnCylinder.Y(), 0.0).Transformed(theTrsf));
  }
  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsCylinder(const double   theBottomRad,
                                                 const double   theTopRad,
                                                 const double   theHeight,
                                                 const gp_Trsf& theTrsf,
                                                 const bool     theIsHollow,
                                                 bool*          theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsCylinder() should be called "
                        "after selection axis initialization");
  double  aTimeEnter = 0.0, aTimeLeave = 0.0;
  gp_Trsf aTrsfInv = theTrsf.Inverted();
  gp_Pnt  aLoc     = myAxis.Location().Transformed(aTrsfInv);
  gp_Dir  aRayDir  = myAxis.Direction().Transformed(aTrsfInv);
  if (!RayCylinderIntersection(theBottomRad,
                               theTopRad,
                               theHeight,
                               aLoc,
                               aRayDir,
                               theIsHollow,
                               aTimeEnter,
                               aTimeLeave))
  {
    return false;
  }
  if (theInside != nullptr)
  {
    *theInside &= (aTimeEnter >= 0.0);
  }
  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsCircle(const double                   theRadius,
                                               const gp_Trsf&                 theTrsf,
                                               const bool                     theIsFilled,
                                               const SelectMgr_ViewClipRange& theClipRange,
                                               SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsCircle() should be called after "
                        "selection axis initialization");
  double  aTime    = 0.0;
  gp_Trsf aTrsfInv = theTrsf.Inverted();
  gp_Pnt  aLoc     = myAxis.Location().Transformed(aTrsfInv);
  gp_Dir  aRayDir  = myAxis.Direction().Transformed(aTrsfInv);
  if (!RayCircleIntersection(theRadius, aLoc, aRayDir, theIsFilled, aTime))
  {
    return false;
  }

  double aDepth = std::max(aTime, 0.0);
  if (theClipRange.IsClipped(aDepth))
  {
    return false;
  }

  const gp_Pnt aPntOnCylinder = aLoc.XYZ() + aRayDir.XYZ() * aDepth;
  thePickResult.SetDepth(aDepth);
  thePickResult.SetPickedPoint(aPntOnCylinder.Transformed(theTrsf));
  if (std::abs(aPntOnCylinder.Z()) < Precision::Confusion())
  {
    thePickResult.SetSurfaceNormal(-gp::DZ().Transformed(theTrsf));
  }
  else
  {
    thePickResult.SetSurfaceNormal(
      gp_Vec(aPntOnCylinder.X(), aPntOnCylinder.Y(), 0.0).Transformed(theTrsf));
  }

  return true;
}

//=================================================================================================

bool SelectMgr_AxisIntersector::OverlapsCircle(const double   theRadius,
                                               const gp_Trsf& theTrsf,
                                               const bool     theIsFilled,
                                               bool*          theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::OverlapsCircle() should be called after "
                        "selection axis initialization");
  double  aTime    = 0.0;
  gp_Trsf aTrsfInv = theTrsf.Inverted();
  gp_Pnt  aLoc     = myAxis.Location().Transformed(aTrsfInv);
  gp_Dir  aRayDir  = myAxis.Direction().Transformed(aTrsfInv);
  if (!RayCircleIntersection(theRadius, aLoc, aRayDir, theIsFilled, aTime))
  {
    return false;
  }
  if (theInside != nullptr)
  {
    *theInside &= (aTime >= 0.0);
  }
  return true;
}

//=================================================================================================

const gp_Pnt& SelectMgr_AxisIntersector::GetNearPnt() const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::GetNearPnt() should be called after "
                        "selection axis initialization");

  return myAxis.Location();
}

//=================================================================================================

const gp_Pnt& SelectMgr_AxisIntersector::GetFarPnt() const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::GetFarPnt() should be called after "
                        "selection axis initialization");

  static gp_Pnt anInfPnt(RealLast(), RealLast(), RealLast());
  return anInfPnt;
}

//=================================================================================================

const gp_Dir& SelectMgr_AxisIntersector::GetViewRayDirection() const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::GetViewRayDirection() should be called "
                        "after selection axis initialization");

  return myAxis.Direction();
}

//=================================================================================================

double SelectMgr_AxisIntersector::DistToGeometryCenter(const gp_Pnt& theCOG) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::DistToGeometryCenter() should be called "
                        "after selection axis initialization");

  return theCOG.Distance(myAxis.Location());
}

//=================================================================================================

gp_Pnt SelectMgr_AxisIntersector::DetectedPoint(const double theDepth) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Point,
                        "Error! SelectMgr_AxisIntersector::DetectedPoint() should be called after "
                        "selection axis initialization");

  return myAxis.Location().XYZ() + myAxis.Direction().XYZ() * theDepth;
}

//=================================================================================================

void SelectMgr_AxisIntersector::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, SelectMgr_AxisIntersector)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, SelectMgr_BaseIntersector)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myAxis)
}
