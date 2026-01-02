// Created on: 2014-11-21
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

#include <SelectMgr_TriangularFrustumSet.hxx>

#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_Delaun.hxx>
#include <Geom_Plane.hxx>
#include <GeomInt_IntSS.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <NCollection_IncAllocator.hxx>
#include <SelectMgr_FrustumBuilder.hxx>

namespace
{
static const size_t MEMORY_BLOCK_SIZE = 512 * 7;
}

//=================================================================================================

SelectMgr_TriangularFrustumSet::SelectMgr_TriangularFrustumSet()
    : myToAllowOverlap(false)
{
}

//=================================================================================================

SelectMgr_TriangularFrustumSet::~SelectMgr_TriangularFrustumSet()
{
  //
}

//=================================================================================================

void SelectMgr_TriangularFrustumSet::Init(const NCollection_Array1<gp_Pnt2d>& thePoints)
{
  if (mySelPolyline.Points.IsNull())
  {
    mySelPolyline.Points = new NCollection_HArray1<gp_Pnt2d>(thePoints.Lower(), thePoints.Upper());
  }
  mySelPolyline.Points->Resize(thePoints.Lower(), thePoints.Upper(), false);
  *mySelPolyline.Points = thePoints;
  mySelectionType       = SelectMgr_SelectionType_Polyline;
}

// =======================================================================
// function : Build
// purpose  : Meshes polygon bounded by polyline. Than organizes a set of
//            triangular frustums, where each triangle's projection onto
//            near and far view frustum planes is considered as a frustum
//            base
// =======================================================================
void SelectMgr_TriangularFrustumSet::Build()
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline
                          || !mySelPolyline.Points.IsNull(),
                        "Error! SelectMgr_TriangularFrustumSet::Build() should be called after "
                        "selection frustum initialization");

  myFrustums.Clear();

  occ::handle<NCollection_IncAllocator> anAllocator =
    new NCollection_IncAllocator(MEMORY_BLOCK_SIZE);
  occ::handle<BRepMesh_DataStructureOfDelaun> aMeshStructure =
    new BRepMesh_DataStructureOfDelaun(anAllocator);
  int                        aPtsLower = mySelPolyline.Points->Lower();
  int                        aPtsUpper = mySelPolyline.Points->Upper();
  IMeshData::VectorOfInteger anIndexes(mySelPolyline.Points->Size(), anAllocator);
  myBoundaryPoints.Resize(aPtsLower, aPtsLower + 2 * (mySelPolyline.Points->Size()) - 1, false);

  for (int aPtIdx = aPtsLower; aPtIdx <= aPtsUpper; ++aPtIdx)
  {
    BRepMesh_Vertex aVertex(mySelPolyline.Points->Value(aPtIdx).XY(), aPtIdx, BRepMesh_Frontier);
    anIndexes.Append(aMeshStructure->AddNode(aVertex));
    const gp_Pnt aNearPnt =
      myBuilder->ProjectPntOnViewPlane(aVertex.Coord().X(), aVertex.Coord().Y(), 0.0);
    const gp_Pnt aFarPnt =
      myBuilder->ProjectPntOnViewPlane(aVertex.Coord().X(), aVertex.Coord().Y(), 1.0);
    myBoundaryPoints.SetValue(aPtIdx, aNearPnt);
    myBoundaryPoints.SetValue(aPtIdx + mySelPolyline.Points->Size(), aFarPnt);
  }

  double aPtSum = 0;
  for (int aIdx = aPtsLower; aIdx <= aPtsUpper; ++aIdx)
  {
    int aNextIdx = (aIdx % mySelPolyline.Points->Length()) + 1;
    aPtSum += (mySelPolyline.Points->Value(aNextIdx).Coord().X()
               - mySelPolyline.Points->Value(aIdx).Coord().X())
              * (mySelPolyline.Points->Value(aNextIdx).Coord().Y()
                 + mySelPolyline.Points->Value(aIdx).Coord().Y());
  }
  bool isClockwiseOrdered = aPtSum < 0;

  for (int aIdx = 0; aIdx < anIndexes.Length(); ++aIdx)
  {
    int           aPtIdx     = isClockwiseOrdered ? aIdx : (aIdx + 1) % anIndexes.Length();
    int           aNextPtIdx = isClockwiseOrdered ? (aIdx + 1) % anIndexes.Length() : aIdx;
    BRepMesh_Edge anEdge(anIndexes.Value(aPtIdx), anIndexes.Value(aNextPtIdx), BRepMesh_Frontier);
    aMeshStructure->AddLink(anEdge);
  }

  BRepMesh_Delaun                aTriangulation(aMeshStructure, anIndexes);
  const IMeshData::MapOfInteger& aTriangles = aMeshStructure->ElementsOfDomain();
  if (aTriangles.Extent() < 1)
    return;

  IMeshData::IteratorOfMapOfInteger aTriangleIt(aTriangles);
  for (; aTriangleIt.More(); aTriangleIt.Next())
  {
    const int                aTriangleId      = aTriangleIt.Key();
    const BRepMesh_Triangle& aCurrentTriangle = aMeshStructure->GetElement(aTriangleId);

    if (aCurrentTriangle.Movability() == BRepMesh_Deleted)
      continue;

    int aTriangleVerts[3];
    aMeshStructure->ElementNodes(aCurrentTriangle, aTriangleVerts);

    gp_Pnt2d aPts[3];
    for (int aVertIdx = 0; aVertIdx < 3; ++aVertIdx)
    {
      const BRepMesh_Vertex& aVertex = aMeshStructure->GetNode(aTriangleVerts[aVertIdx]);
      aPts[aVertIdx]                 = aVertex.Coord();
    }

    occ::handle<SelectMgr_TriangularFrustum> aTrFrustum = new SelectMgr_TriangularFrustum();
    aTrFrustum->Init(aPts[0], aPts[1], aPts[2]);
    aTrFrustum->SetBuilder(myBuilder);
    aTrFrustum->Build();
    myFrustums.Append(aTrFrustum);
  }

  aMeshStructure.Nullify();
  anAllocator.Nullify();
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
occ::handle<SelectMgr_BaseIntersector> SelectMgr_TriangularFrustumSet::ScaleAndTransform(
  const int                                    theScale,
  const gp_GTrsf&                              theTrsf,
  const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::ScaleAndTransform() should be "
                        "called after selection frustum initialization");

  occ::handle<SelectMgr_TriangularFrustumSet> aRes = new SelectMgr_TriangularFrustumSet();
  aRes->SetCamera(myCamera);
  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    aRes->myFrustums.Append(occ::down_cast<SelectMgr_TriangularFrustum>(
      anIter.Value()->ScaleAndTransform(theScale, theTrsf, theBuilder)));
  }

  aRes->myBoundaryPoints.Resize(myBoundaryPoints.Lower(), myBoundaryPoints.Upper(), false);
  for (int anIdx = myBoundaryPoints.Lower(); anIdx <= myBoundaryPoints.Upper(); anIdx++)
  {
    gp_Pnt aPoint = myBoundaryPoints.Value(anIdx);
    theTrsf.Transforms(aPoint.ChangeCoord());
    aRes->myBoundaryPoints.SetValue(anIdx, aPoint);
  }

  aRes->mySelectionType      = mySelectionType;
  aRes->mySelPolyline.Points = mySelPolyline.Points;
  aRes->SetBuilder(theBuilder);
  return aRes;
}

//=======================================================================
// function : CopyWithBuilder
// purpose  : Returns a copy of the frustum using the given frustum builder configuration.
//            Returned frustum should be re-constructed before being used.
//=======================================================================
occ::handle<SelectMgr_BaseIntersector> SelectMgr_TriangularFrustumSet::CopyWithBuilder(
  const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::CopyWithBuilder() should be called "
                        "after selection frustum initialization");

  Standard_ASSERT_RAISE(
    !theBuilder.IsNull(),
    "Error! SelectMgr_TriangularFrustumSet::CopyWithBuilder() should be called with valid builder");

  occ::handle<SelectMgr_TriangularFrustumSet> aRes = new SelectMgr_TriangularFrustumSet();
  aRes->SetCamera(myCamera);
  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    aRes->myFrustums.Append(
      occ::down_cast<SelectMgr_TriangularFrustum>(anIter.Value()->CopyWithBuilder(theBuilder)));
  }
  aRes->mySelectionType  = mySelectionType;
  aRes->mySelPolyline    = mySelPolyline;
  aRes->myToAllowOverlap = myToAllowOverlap;
  aRes->SetBuilder(theBuilder);
  return aRes;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsBox(const NCollection_Vec3<double>& theMinPnt,
                                                 const NCollection_Vec3<double>& theMaxPnt,
                                                 const SelectMgr_ViewClipRange&  theClipRange,
                                                 SelectBasics_PickResult& thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value()->OverlapsBox(theMinPnt, theMaxPnt, theClipRange, thePickResult))
    {
      return true;
    }
  }

  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsBox(const NCollection_Vec3<double>& theMinPnt,
                                                 const NCollection_Vec3<double>& theMaxPnt,
                                                 bool*                           theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (!anIter.Value()->OverlapsBox(theMinPnt, theMaxPnt, nullptr))
    {
      continue;
    }

    if (myToAllowOverlap || theInside == nullptr)
    {
      return true;
    }

    gp_Pnt aMinMaxPnts[2] = {gp_Pnt(theMinPnt.x(), theMinPnt.y(), theMinPnt.z()),
                             gp_Pnt(theMaxPnt.x(), theMaxPnt.y(), theMaxPnt.z())};

    gp_Pnt anOffset[3] = {gp_Pnt(aMinMaxPnts[1].X() - aMinMaxPnts[0].X(), 0.0, 0.0),
                          gp_Pnt(0.0, aMinMaxPnts[1].Y() - aMinMaxPnts[0].Y(), 0.0),
                          gp_Pnt(0.0, 0.0, aMinMaxPnts[1].Z() - aMinMaxPnts[0].Z())};

    int aSign = 1;
    for (int aPntsIdx = 0; aPntsIdx < 2; aPntsIdx++)
    {
      for (int aCoordIdx = 0; aCoordIdx < 3; aCoordIdx++)
      {
        gp_Pnt anOffsetPnt = aMinMaxPnts[aPntsIdx].XYZ() + aSign * anOffset[aCoordIdx].XYZ();
        if (isIntersectBoundary(aMinMaxPnts[aPntsIdx], anOffsetPnt)
            || isIntersectBoundary(anOffsetPnt,
                                   anOffsetPnt.XYZ() + aSign * anOffset[(aCoordIdx + 1) % 3].XYZ()))
        {
          *theInside &= false;
          return true;
        }
      }
      aSign = -aSign;
    }
    return true;
  }

  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsPoint(const gp_Pnt&                  thePnt,
                                                   const SelectMgr_ViewClipRange& theClipRange,
                                                   SelectBasics_PickResult& thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value()->OverlapsPoint(thePnt, theClipRange, thePickResult))
    {
      return true;
    }
  }

  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsPolygon(
  const NCollection_Array1<gp_Pnt>& theArrayOfPts,
  Select3D_TypeOfSensitivity        theSensType,
  const SelectMgr_ViewClipRange&    theClipRange,
  SelectBasics_PickResult&          thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (!anIter.Value()->OverlapsPolygon(theArrayOfPts, theSensType, theClipRange, thePickResult))
    {
      continue;
    }

    if (myToAllowOverlap)
    {
      return true;
    }

    int aPtsLower = theArrayOfPts.Lower();
    int aPtsUpper = theArrayOfPts.Upper();
    for (int anIdx = aPtsLower; anIdx <= aPtsUpper; anIdx++)
    {
      if (isIntersectBoundary(theArrayOfPts.Value(anIdx),
                              theArrayOfPts.Value(anIdx < aPtsUpper ? anIdx + 1 : aPtsLower)))
      {
        return false;
      }
    }
    return true;
  }

  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsSegment(const gp_Pnt&                  thePnt1,
                                                     const gp_Pnt&                  thePnt2,
                                                     const SelectMgr_ViewClipRange& theClipRange,
                                                     SelectBasics_PickResult& thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (!anIter.Value()->OverlapsSegment(thePnt1, thePnt2, theClipRange, thePickResult))
    {
      continue;
    }

    if (myToAllowOverlap)
    {
      return true;
    }

    if (isIntersectBoundary(thePnt1, thePnt2))
    {
      return false;
    }
    return true;
  }

  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsTriangle(const gp_Pnt&                  thePnt1,
                                                      const gp_Pnt&                  thePnt2,
                                                      const gp_Pnt&                  thePnt3,
                                                      Select3D_TypeOfSensitivity     theSensType,
                                                      const SelectMgr_ViewClipRange& theClipRange,
                                                      SelectBasics_PickResult& thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (!anIter.Value()
           ->OverlapsTriangle(thePnt1, thePnt2, thePnt3, theSensType, theClipRange, thePickResult))
    {
      continue;
    }

    if (myToAllowOverlap)
    {
      return true;
    }

    if (isIntersectBoundary(thePnt1, thePnt2) || isIntersectBoundary(thePnt2, thePnt3)
        || isIntersectBoundary(thePnt3, thePnt1))
    {
      return false;
    }
    return true;
  }

  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsSphere(const gp_Pnt& theCenter,
                                                    const double  theRadius,
                                                    bool* /*theInside*/) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");
  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value()->OverlapsSphere(theCenter, theRadius, nullptr))
    {
      // select 3 points of the frustum and build a plane on them
      double aMaxDist1 = 0.0, aMaxDist2 = 0.0;
      int    anIdx1 = myBoundaryPoints.Lower();
      int    anIdx2 = myBoundaryPoints.Lower();
      int    anIdx3 = myBoundaryPoints.Lower();
      for (int anIdx = myBoundaryPoints.Lower();
           anIdx < myBoundaryPoints.Size() / 2 + myBoundaryPoints.Lower();
           anIdx++)
      {
        if (myBoundaryPoints[anIdx1].Distance(myBoundaryPoints[anIdx]) < Precision::Confusion())
        {
          continue;
        }
        else if (aMaxDist1 < myBoundaryPoints[anIdx1].Distance(myBoundaryPoints[anIdx]))
        {
          if (anIdx2 != anIdx3)
          {
            anIdx3    = anIdx2;
            aMaxDist2 = aMaxDist1;
          }
          anIdx2    = anIdx;
          aMaxDist1 = myBoundaryPoints[anIdx1].Distance(myBoundaryPoints[anIdx]);
        }
        else if (aMaxDist2 < myBoundaryPoints[anIdx2].Distance(myBoundaryPoints[anIdx]))
        {
          anIdx3    = anIdx;
          aMaxDist2 = myBoundaryPoints[anIdx2].Distance(myBoundaryPoints[anIdx]);
        }
      }
      gp_Vec aVecPlane1(myBoundaryPoints[anIdx1], myBoundaryPoints[anIdx2]);
      gp_Vec aVecPlane2(myBoundaryPoints[anIdx1], myBoundaryPoints[anIdx3]);

      const gp_Dir aNorm(aVecPlane1.Crossed(aVecPlane2));

      // distance from point(x,y,z) to plane(A,B,C,D) d = | Ax + By + Cz + D | / sqrt (A^2 + B^2 +
      // C^2) = aPnt.Dot (Norm) / 1
      const gp_Pnt aCenterProj = theCenter.XYZ() - aNorm.XYZ() * theCenter.XYZ().Dot(aNorm.XYZ());

      // If the center of the sphere is inside of the volume projection, then anAngleSum will be
      // equal 2*M_PI
      double                     anAngleSum = 0.0;
      NCollection_Array1<gp_Pnt> aBoundaries(myBoundaryPoints.Lower(),
                                             myBoundaryPoints.Size() / 2
                                               + myBoundaryPoints.Lower());

      for (int anIdx = myBoundaryPoints.Lower();
           anIdx < myBoundaryPoints.Size() / 2 + myBoundaryPoints.Lower();
           anIdx++)
      {
        aBoundaries.SetValue(anIdx, myBoundaryPoints[anIdx]);

        gp_Pnt aPnt1 = myBoundaryPoints.Value(anIdx);
        gp_Pnt aPnt2 = myBoundaryPoints.Value(anIdx + 1);

        // Projections of the points on the plane
        gp_Pnt aPntProj1 = aPnt1.XYZ() - aNorm.XYZ() * aPnt1.XYZ().Dot(aNorm.XYZ());
        gp_Pnt aPntProj2 = aPnt2.XYZ() - aNorm.XYZ() * aPnt2.XYZ().Dot(aNorm.XYZ());

        gp_Vec aVecAngle1(aCenterProj, aPntProj1);
        gp_Vec aVecAngle2(aCenterProj, aPntProj2);
        anAngleSum += aVecAngle1.Angle(aVecAngle2);
      }
      bool isCenterInside   = std::abs(anAngleSum - 2 * M_PI) < Precision::Confusion();
      bool isBoundaryInside = false;
      bool isIntersectSphereBoundaries =
        IsBoundaryIntersectSphere(aCenterProj, theRadius, aNorm, aBoundaries, isBoundaryInside);

      if (myToAllowOverlap)
      {
        return isIntersectSphereBoundaries || isCenterInside;
      }
      else
      {
        return !isIntersectSphereBoundaries && isCenterInside && !isBoundaryInside;
      }
    }
  }
  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsSphere(const gp_Pnt&                  theCenter,
                                                    const double                   theRadius,
                                                    const SelectMgr_ViewClipRange& theClipRange,
                                                    SelectBasics_PickResult& thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");
  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value()->OverlapsSphere(theCenter, theRadius, theClipRange, thePickResult))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsCylinder(const double                   theBottomRad,
                                                      const double                   theTopRad,
                                                      const double                   theHeight,
                                                      const gp_Trsf&                 theTrsf,
                                                      const bool                     theIsHollow,
                                                      const SelectMgr_ViewClipRange& theClipRange,
                                                      SelectBasics_PickResult& thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");
  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value()->OverlapsCylinder(theBottomRad,
                                         theTopRad,
                                         theHeight,
                                         theTrsf,
                                         theIsHollow,
                                         theClipRange,
                                         thePickResult))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsCylinder(const double   theBottomRad,
                                                      const double   theTopRad,
                                                      const double   theHeight,
                                                      const gp_Trsf& theTrsf,
                                                      const bool     theIsHollow,
                                                      bool*          theInside) const
{
  const gp_Dir aCylNorm(gp::DZ().Transformed(theTrsf));
  const gp_Pnt aBottomCenter(gp::Origin().Transformed(theTrsf));
  const gp_Pnt aTopCenter = aBottomCenter.XYZ() + aCylNorm.XYZ() * theHeight;

  const gp_Vec aVecPlane1(myFrustums.First()->myVertices[0], myFrustums.First()->myVertices[1]);
  const gp_Vec aVecPlane2(myFrustums.First()->myVertices[0], myFrustums.First()->myVertices[2]);

  const gp_Dir aDirNorm(aVecPlane1.Crossed(aVecPlane2));
  const double anAngle   = aCylNorm.Angle(aDirNorm);
  const double aCosAngle = std::cos(anAngle);
  const gp_Pln aPln(myFrustums.First()->myVertices[0], aDirNorm);
  double       aCoefA, aCoefB, aCoefC, aCoefD;
  aPln.Coefficients(aCoefA, aCoefB, aCoefC, aCoefD);

  const double aTBottom =
    -(aBottomCenter.XYZ().Dot(aDirNorm.XYZ()) + aCoefD) / aDirNorm.Dot(aDirNorm);
  const gp_Pnt aBottomCenterProject(aCoefA * aTBottom + aBottomCenter.X(),
                                    aCoefB * aTBottom + aBottomCenter.Y(),
                                    aCoefC * aTBottom + aBottomCenter.Z());

  const double aTTop = -(aTopCenter.XYZ().Dot(aDirNorm.XYZ()) + aCoefD) / aDirNorm.Dot(aDirNorm);
  const gp_Pnt aTopCenterProject(aCoefA * aTTop + aTopCenter.X(),
                                 aCoefB * aTTop + aTopCenter.Y(),
                                 aCoefC * aTTop + aTopCenter.Z());

  gp_XYZ       aCylNormProject;
  const gp_XYZ aTopBottomVec  = aTopCenterProject.XYZ() - aBottomCenterProject.XYZ();
  const double aTopBottomDist = aTopBottomVec.Modulus();
  if (aTopBottomDist > 0.0)
  {
    aCylNormProject = aTopBottomVec / aTopBottomDist;
  }

  gp_Pnt aPoints[6];
  aPoints[0] = aBottomCenterProject.XYZ() - aCylNormProject * theBottomRad * std::abs(aCosAngle);
  aPoints[1] = aTopCenterProject.XYZ() + aCylNormProject * theTopRad * std::abs(aCosAngle);
  const gp_Dir aDirEndFaces = (aCylNorm.IsParallel(aDirNorm, Precision::Angular()))
                                ? gp::DY().Transformed(theTrsf)
                                : aCylNorm.Crossed(aDirNorm);

  aPoints[2] = aTopCenterProject.XYZ() + aDirEndFaces.XYZ() * theTopRad;
  aPoints[3] = aTopCenterProject.XYZ() - aDirEndFaces.XYZ() * theTopRad;
  aPoints[4] = aBottomCenterProject.XYZ() + aDirEndFaces.XYZ() * theBottomRad;
  aPoints[5] = aBottomCenterProject.XYZ() - aDirEndFaces.XYZ() * theBottomRad;

  gp_Pnt                     aVerticesBuf[3];
  NCollection_Array1<gp_Pnt> aVertices(aVerticesBuf[0], 0, 2);

  bool isCylInsideTriangSet = true;
  for (int i = 0; i < 6; ++i)
  {
    bool isInside = false;
    for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
         anIter.More();
         anIter.Next())
    {

      for (int anIdx = 0; anIdx < 3; anIdx++)
      {
        aVertices[anIdx] = anIter.Value()->myVertices[anIdx];
      }
      if (anIter.Value()->isDotInside(aPoints[i], aVertices))
      {
        isInside = true;
        break;
      }
    }
    isCylInsideTriangSet &= isInside;
  }
  if (theInside != nullptr)
  {
    *theInside &= isCylInsideTriangSet;
  }
  if (isCylInsideTriangSet)
  {
    return true;
  }
  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value()
          ->OverlapsCylinder(theBottomRad, theTopRad, theHeight, theTrsf, theIsHollow, theInside))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsCircle(const double                   theRadius,
                                                    const gp_Trsf&                 theTrsf,
                                                    const bool                     theIsFilled,
                                                    const SelectMgr_ViewClipRange& theClipRange,
                                                    SelectBasics_PickResult& thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");
  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Value()
          ->OverlapsCircle(theRadius, theTrsf, theIsFilled, theClipRange, thePickResult))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::OverlapsCircle(const double   theRadius,
                                                    const gp_Trsf& theTrsf,
                                                    const bool     theIsFilled,
                                                    bool*          theInside) const
{
  const gp_Pnt aCenter(gp::Origin().Transformed(theTrsf));
  const gp_Vec aVecPlane1(myFrustums.First()->myVertices[0], myFrustums.First()->myVertices[1]);
  const gp_Vec aVecPlane2(myFrustums.First()->myVertices[0], myFrustums.First()->myVertices[2]);

  const gp_Dir aDirNorm(aVecPlane1.Crossed(aVecPlane2));
  const gp_Pln aPln(myFrustums.First()->myVertices[0], aDirNorm);
  double       aCoefA, aCoefB, aCoefC, aCoefD;
  aPln.Coefficients(aCoefA, aCoefB, aCoefC, aCoefD);

  const double aT = -(aCenter.XYZ().Dot(aDirNorm.XYZ()) + aCoefD) / aDirNorm.Dot(aDirNorm);
  const gp_Pnt aCenterProject(aCoefA * aT + aCenter.X(),
                              aCoefB * aT + aCenter.Y(),
                              aCoefC * aT + aCenter.Z());

  gp_Pnt                     aVerticesBuf[3];
  NCollection_Array1<gp_Pnt> aVertices(aVerticesBuf[0], 0, 2);

  if (!theIsFilled)
  {
    for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
         anIter.More();
         anIter.Next())
    {
      if (!anIter.Value()->OverlapsCircle(theRadius, theTrsf, theIsFilled, theInside))
      {
        continue;
      }

      if (myToAllowOverlap)
      {
        return true;
      }

      if (isIntersectBoundary(theRadius, theTrsf, theIsFilled))
      {
        if (theInside != nullptr)
        {
          *theInside &= false;
        }
        return false;
      }
      return true;
    }
  }
  else
  {
    for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
         anIter.More();
         anIter.Next())
    {
      if (!anIter.Value()->OverlapsCircle(theRadius, theTrsf, theIsFilled, theInside))
      {
        continue;
      }

      if (myToAllowOverlap)
      {
        return true;
      }

      if (isIntersectBoundary(theRadius, theTrsf, theIsFilled))
      {
        return false;
      }
      return true;
    }
  }

  if (theInside != nullptr)
  {
    *theInside &= false;
  }

  return false;
}

//=================================================================================================

void SelectMgr_TriangularFrustumSet::GetPlanes(
  NCollection_Vector<NCollection_Vec4<double>>& thePlaneEquations) const
{
  thePlaneEquations.Clear();

  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->GetPlanes(thePlaneEquations);
  }
}

//=================================================================================================

void SelectMgr_TriangularFrustumSet::SetAllowOverlapDetection(const bool theIsToAllow)
{
  myToAllowOverlap = theIsToAllow;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::pointInTriangle(const gp_Pnt& thePnt,
                                                     const gp_Pnt& theV1,
                                                     const gp_Pnt& theV2,
                                                     const gp_Pnt& theV3)
{
  gp_Vec a = theV1.XYZ() - thePnt.XYZ();
  gp_Vec b = theV2.XYZ() - thePnt.XYZ();
  gp_Vec c = theV3.XYZ() - thePnt.XYZ();

  gp_Vec u = b.Crossed(c);
  gp_Vec v = c.Crossed(a);
  gp_Vec w = a.Crossed(b);

  return u.Dot(v) >= 0.0 && u.Dot(w) >= 0.0;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::segmentSegmentIntersection(const gp_Pnt& theStartPnt1,
                                                                const gp_Pnt& theEndPnt1,
                                                                const gp_Pnt& theStartPnt2,
                                                                const gp_Pnt& theEndPnt2)
{
  gp_XYZ aVec1  = theEndPnt1.XYZ() - theStartPnt1.XYZ();
  gp_XYZ aVec2  = theEndPnt2.XYZ() - theStartPnt2.XYZ();
  gp_XYZ aVec21 = theStartPnt2.XYZ() - theStartPnt1.XYZ();
  gp_XYZ aVec12 = theStartPnt1.XYZ() - theStartPnt2.XYZ();
  if (std::abs(aVec21.DotCross(aVec1, aVec2)) > Precision::Confusion()
      || std::abs(aVec12.DotCross(aVec2, aVec1)) > Precision::Confusion())
  {
    // lines are not coplanar
    return false;
  }

  double aValue1 =
    aVec21.Crossed(aVec2).Dot(aVec1.Crossed(aVec2)) / aVec1.Crossed(aVec2).SquareModulus();
  double aValue2 =
    aVec12.Crossed(aVec1).Dot(aVec2.Crossed(aVec1)) / aVec2.Crossed(aVec1).SquareModulus();
  return aValue1 >= 0.0 && aValue1 <= 1.0 && aValue2 >= 0.0 && aValue2 <= 1.0;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::isIntersectBoundary(const double   theRadius,
                                                         const gp_Trsf& theTrsf,
                                                         const bool     theIsFilled) const
{
  int aFacesNb = myBoundaryPoints.Size() / 2;

  const gp_Pnt& aCircCenter = theTrsf.TranslationPart();
  gp_Ax2        anAxis;
  anAxis.Transform(theTrsf);
  occ::handle<Geom_Circle> aCirc = new Geom_Circle(anAxis, theRadius);

  gp_Dir                    aCircNorm  = gp_Dir(gp_Dir::D::Z).Transformed(theTrsf);
  occ::handle<Geom_Surface> aCircPlane = new Geom_Plane(aCircCenter, aCircNorm);

  for (int anIdx = myBoundaryPoints.Lower(); anIdx < aFacesNb + myBoundaryPoints.Lower(); anIdx++)
  {
    gp_Pnt aFace[4] = {myBoundaryPoints.Value(anIdx),
                       myBoundaryPoints.Value(anIdx + aFacesNb),
                       myBoundaryPoints.Value(anIdx % aFacesNb + 1 + aFacesNb),
                       myBoundaryPoints.Value(anIdx % aFacesNb + 1)};

    gp_Dir aBndPlaneNorm = gp_Vec(aFace[0], aFace[1]).Crossed(gp_Vec(aFace[0], aFace[2]));
    occ::handle<Geom_Surface> aBndPlane = new Geom_Plane(aFace[0], aBndPlaneNorm);

    GeomInt_IntSS anInterSS(aCircPlane, aBndPlane, Precision::Confusion());
    if (!anInterSS.IsDone() || anInterSS.NbLines() == 0)
    {
      continue;
    }

    const occ::handle<Geom_Line>& anInterLine = occ::down_cast<Geom_Line>(anInterSS.Line(1));
    double                        aDistance   = anInterLine->Lin().Distance(aCircCenter);
    if (aDistance > theRadius)
    {
      continue;
    }

    gp_Lin aLine       = anInterLine->Lin();
    gp_Lin aNormalLine = aLine.Normal(aCircCenter);
    gp_Pnt aCrossPoint =
      aCircCenter.Translated(aNormalLine.Direction().Reversed().XYZ() * aDistance);

    double anOffset = std::sqrt(theRadius * theRadius - aDistance * aDistance);
    // Line-circle intersection points
    gp_Pnt aP1 = aCrossPoint.Translated(aLine.Direction().XYZ() * anOffset);
    gp_Pnt aP2 = aCrossPoint.Translated(aLine.Direction().Reversed().XYZ() * anOffset);

    if (pointInTriangle(aP1, aFace[0], aFace[1], aFace[2])
        || pointInTriangle(aP1, aFace[0], aFace[2], aFace[3])
        || pointInTriangle(aP2, aFace[0], aFace[1], aFace[2])
        || pointInTriangle(aP2, aFace[0], aFace[2], aFace[3]))
    {
      return true;
    }

    if (theIsFilled || segmentSegmentIntersection(aP1, aP2, aFace[0], aFace[1])
        || segmentSegmentIntersection(aP1, aP2, aFace[1], aFace[2])
        || segmentSegmentIntersection(aP1, aP2, aFace[2], aFace[3])
        || segmentSegmentIntersection(aP1, aP2, aFace[0], aFace[3]))
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

bool SelectMgr_TriangularFrustumSet::isIntersectBoundary(const gp_Pnt& thePnt1,
                                                         const gp_Pnt& thePnt2) const
{
  int    aFacesNb = myBoundaryPoints.Size() / 2;
  gp_Vec aDir     = thePnt2.XYZ() - thePnt1.XYZ();
  gp_Pnt anOrig   = thePnt1;

  for (int anIdx = myBoundaryPoints.Lower(); anIdx < aFacesNb + myBoundaryPoints.Lower(); anIdx++)
  {
    gp_Pnt aFace[4] = {myBoundaryPoints.Value(anIdx),
                       myBoundaryPoints.Value(anIdx + aFacesNb),
                       myBoundaryPoints.Value(anIdx % aFacesNb + 1 + aFacesNb),
                       myBoundaryPoints.Value(anIdx % aFacesNb + 1)};

    if (segmentTriangleIntersection(anOrig, aDir, aFace[0], aFace[1], aFace[2])
        || segmentTriangleIntersection(anOrig, aDir, aFace[0], aFace[2], aFace[3]))
    {
      return true;
    }
  }
  return false;
}

//=======================================================================
// function : segmentTriangleIntersection
// purpose  : Moller-Trumbore ray-triangle intersection test
//=======================================================================
bool SelectMgr_TriangularFrustumSet::segmentTriangleIntersection(const gp_Pnt& theOrig,
                                                                 const gp_Vec& theDir,
                                                                 const gp_Pnt& theV1,
                                                                 const gp_Pnt& theV2,
                                                                 const gp_Pnt& theV3)
{
  gp_Vec aPVec, aTVec, aQVec;
  double aD, aInvD, anU, aV, aT;

  gp_Vec anEdge1 = theV2.XYZ() - theV1.XYZ();
  gp_Vec anEdge2 = theV3.XYZ() - theV1.XYZ();

  aPVec = theDir.Crossed(anEdge2);
  aD    = anEdge1.Dot(aPVec);
  if (fabs(aD) < gp::Resolution())
  {
    return false;
  }

  aInvD = 1.0 / aD;
  aTVec = theOrig.XYZ() - theV1.XYZ();
  anU   = aInvD * aTVec.Dot(aPVec);
  if (anU < 0.0 || anU > 1.0)
  {
    return false;
  }

  aQVec = aTVec.Crossed(anEdge1);
  aV    = aInvD * theDir.Dot(aQVec);
  if (aV < 0.0 || anU + aV > 1.0)
  {
    return false;
  }

  aT = aInvD * anEdge2.Dot(aQVec);
  return aT >= 0 && aT <= 1;
}

//=================================================================================================

gp_Pnt SelectMgr_TriangularFrustumSet::DetectedPoint(const double theDepth) const
{
  (void)theDepth;
  throw Standard_ProgramError("SelectMgr_TriangularFrustumSet::DetectedPoint() should not be "
                              "called for Polyline selection type");
}

//=================================================================================================

void SelectMgr_TriangularFrustumSet::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, SelectMgr_TriangularFrustumSet)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, SelectMgr_BaseFrustum)

  for (NCollection_List<occ::handle<SelectMgr_TriangularFrustum>>::Iterator anIter(myFrustums);
       anIter.More();
       anIter.Next())
  {
    const occ::handle<SelectMgr_TriangularFrustum>& aFrustum = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aFrustum.get())
  }
}
