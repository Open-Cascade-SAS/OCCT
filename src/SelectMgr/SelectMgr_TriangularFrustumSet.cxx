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

// =======================================================================
// function : SelectMgr_TriangularFrustumSet
// purpose  :
// =======================================================================
SelectMgr_TriangularFrustumSet::SelectMgr_TriangularFrustumSet()
    : myToAllowOverlap(Standard_False)
{
}

// =======================================================================
// function : ~SelectMgr_TriangularFrustumSet
// purpose  :
// =======================================================================
SelectMgr_TriangularFrustumSet::~SelectMgr_TriangularFrustumSet()
{
  //
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void SelectMgr_TriangularFrustumSet::Init(const TColgp_Array1OfPnt2d& thePoints)
{
  if (mySelPolyline.Points.IsNull())
  {
    mySelPolyline.Points = new TColgp_HArray1OfPnt2d(thePoints.Lower(), thePoints.Upper());
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

  Handle(NCollection_IncAllocator) anAllocator = new NCollection_IncAllocator(MEMORY_BLOCK_SIZE);
  Handle(BRepMesh_DataStructureOfDelaun) aMeshStructure =
    new BRepMesh_DataStructureOfDelaun(anAllocator);
  Standard_Integer           aPtsLower = mySelPolyline.Points->Lower();
  Standard_Integer           aPtsUpper = mySelPolyline.Points->Upper();
  IMeshData::VectorOfInteger anIndexes(mySelPolyline.Points->Size(), anAllocator);
  myBoundaryPoints.Resize(aPtsLower,
                          aPtsLower + 2 * (mySelPolyline.Points->Size()) - 1,
                          Standard_False);

  for (Standard_Integer aPtIdx = aPtsLower; aPtIdx <= aPtsUpper; ++aPtIdx)
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

  Standard_Real aPtSum = 0;
  for (Standard_Integer aIdx = aPtsLower; aIdx <= aPtsUpper; ++aIdx)
  {
    Standard_Integer aNextIdx = (aIdx % mySelPolyline.Points->Length()) + 1;
    aPtSum += (mySelPolyline.Points->Value(aNextIdx).Coord().X()
               - mySelPolyline.Points->Value(aIdx).Coord().X())
              * (mySelPolyline.Points->Value(aNextIdx).Coord().Y()
                 + mySelPolyline.Points->Value(aIdx).Coord().Y());
  }
  Standard_Boolean isClockwiseOrdered = aPtSum < 0;

  for (Standard_Integer aIdx = 0; aIdx < anIndexes.Length(); ++aIdx)
  {
    Standard_Integer aPtIdx     = isClockwiseOrdered ? aIdx : (aIdx + 1) % anIndexes.Length();
    Standard_Integer aNextPtIdx = isClockwiseOrdered ? (aIdx + 1) % anIndexes.Length() : aIdx;
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
    const Standard_Integer   aTriangleId      = aTriangleIt.Key();
    const BRepMesh_Triangle& aCurrentTriangle = aMeshStructure->GetElement(aTriangleId);

    if (aCurrentTriangle.Movability() == BRepMesh_Deleted)
      continue;

    Standard_Integer aTriangleVerts[3];
    aMeshStructure->ElementNodes(aCurrentTriangle, aTriangleVerts);

    gp_Pnt2d aPts[3];
    for (Standard_Integer aVertIdx = 0; aVertIdx < 3; ++aVertIdx)
    {
      const BRepMesh_Vertex& aVertex = aMeshStructure->GetNode(aTriangleVerts[aVertIdx]);
      aPts[aVertIdx]                 = aVertex.Coord();
    }

    Handle(SelectMgr_TriangularFrustum) aTrFrustum = new SelectMgr_TriangularFrustum();
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
Handle(SelectMgr_BaseIntersector) SelectMgr_TriangularFrustumSet::ScaleAndTransform(
  const Standard_Integer                  theScale,
  const gp_GTrsf&                         theTrsf,
  const Handle(SelectMgr_FrustumBuilder)& theBuilder) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::ScaleAndTransform() should be "
                        "called after selection frustum initialization");

  Handle(SelectMgr_TriangularFrustumSet) aRes = new SelectMgr_TriangularFrustumSet();
  aRes->SetCamera(myCamera);
  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    aRes->myFrustums.Append(Handle(SelectMgr_TriangularFrustum)::DownCast(
      anIter.Value()->ScaleAndTransform(theScale, theTrsf, theBuilder)));
  }

  aRes->myBoundaryPoints.Resize(myBoundaryPoints.Lower(), myBoundaryPoints.Upper(), Standard_False);
  for (Standard_Integer anIdx = myBoundaryPoints.Lower(); anIdx <= myBoundaryPoints.Upper();
       anIdx++)
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
Handle(SelectMgr_BaseIntersector) SelectMgr_TriangularFrustumSet::CopyWithBuilder(
  const Handle(SelectMgr_FrustumBuilder)& theBuilder) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::CopyWithBuilder() should be called "
                        "after selection frustum initialization");

  Standard_ASSERT_RAISE(
    !theBuilder.IsNull(),
    "Error! SelectMgr_TriangularFrustumSet::CopyWithBuilder() should be called with valid builder");

  Handle(SelectMgr_TriangularFrustumSet) aRes = new SelectMgr_TriangularFrustumSet();
  aRes->SetCamera(myCamera);
  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    aRes->myFrustums.Append(
      Handle(SelectMgr_TriangularFrustum)::DownCast(anIter.Value()->CopyWithBuilder(theBuilder)));
  }
  aRes->mySelectionType  = mySelectionType;
  aRes->mySelPolyline    = mySelPolyline;
  aRes->myToAllowOverlap = myToAllowOverlap;
  aRes->SetBuilder(theBuilder);
  return aRes;
}

// =======================================================================
// function : OverlapsBox
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsBox(
  const SelectMgr_Vec3&          theMinPnt,
  const SelectMgr_Vec3&          theMaxPnt,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->OverlapsBox(theMinPnt, theMaxPnt, theClipRange, thePickResult))
    {
      return Standard_True;
    }
  }

  return Standard_False;
}

// =======================================================================
// function : OverlapsBox
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsBox(const SelectMgr_Vec3& theMinPnt,
                                                             const SelectMgr_Vec3& theMaxPnt,
                                                             Standard_Boolean*     theInside) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    if (!anIter.Value()->OverlapsBox(theMinPnt, theMaxPnt, NULL))
    {
      continue;
    }

    if (myToAllowOverlap || theInside == NULL)
    {
      return Standard_True;
    }

    gp_Pnt aMinMaxPnts[2] = {gp_Pnt(theMinPnt.x(), theMinPnt.y(), theMinPnt.z()),
                             gp_Pnt(theMaxPnt.x(), theMaxPnt.y(), theMaxPnt.z())};

    gp_Pnt anOffset[3] = {gp_Pnt(aMinMaxPnts[1].X() - aMinMaxPnts[0].X(), 0.0, 0.0),
                          gp_Pnt(0.0, aMinMaxPnts[1].Y() - aMinMaxPnts[0].Y(), 0.0),
                          gp_Pnt(0.0, 0.0, aMinMaxPnts[1].Z() - aMinMaxPnts[0].Z())};

    Standard_Integer aSign = 1;
    for (Standard_Integer aPntsIdx = 0; aPntsIdx < 2; aPntsIdx++)
    {
      for (Standard_Integer aCoordIdx = 0; aCoordIdx < 3; aCoordIdx++)
      {
        gp_Pnt anOffsetPnt = aMinMaxPnts[aPntsIdx].XYZ() + aSign * anOffset[aCoordIdx].XYZ();
        if (isIntersectBoundary(aMinMaxPnts[aPntsIdx], anOffsetPnt)
            || isIntersectBoundary(anOffsetPnt,
                                   anOffsetPnt.XYZ() + aSign * anOffset[(aCoordIdx + 1) % 3].XYZ()))
        {
          *theInside &= Standard_False;
          return Standard_True;
        }
      }
      aSign = -aSign;
    }
    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : OverlapsPoint
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsPoint(
  const gp_Pnt&                  thePnt,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->OverlapsPoint(thePnt, theClipRange, thePickResult))
    {
      return Standard_True;
    }
  }

  return Standard_False;
}

// =======================================================================
// function : OverlapsPolygon
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsPolygon(
  const TColgp_Array1OfPnt&      theArrayOfPts,
  Select3D_TypeOfSensitivity     theSensType,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    if (!anIter.Value()->OverlapsPolygon(theArrayOfPts, theSensType, theClipRange, thePickResult))
    {
      continue;
    }

    if (myToAllowOverlap)
    {
      return Standard_True;
    }

    Standard_Integer aPtsLower = theArrayOfPts.Lower();
    Standard_Integer aPtsUpper = theArrayOfPts.Upper();
    for (Standard_Integer anIdx = aPtsLower; anIdx <= aPtsUpper; anIdx++)
    {
      if (isIntersectBoundary(theArrayOfPts.Value(anIdx),
                              theArrayOfPts.Value(anIdx < aPtsUpper ? anIdx + 1 : aPtsLower)))
      {
        return Standard_False;
      }
    }
    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : OverlapsSegment
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsSegment(
  const gp_Pnt&                  thePnt1,
  const gp_Pnt&                  thePnt2,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    if (!anIter.Value()->OverlapsSegment(thePnt1, thePnt2, theClipRange, thePickResult))
    {
      continue;
    }

    if (myToAllowOverlap)
    {
      return Standard_True;
    }

    if (isIntersectBoundary(thePnt1, thePnt2))
    {
      return Standard_False;
    }
    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : OverlapsTriangle
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsTriangle(
  const gp_Pnt&                  thePnt1,
  const gp_Pnt&                  thePnt2,
  const gp_Pnt&                  thePnt3,
  Select3D_TypeOfSensitivity     theSensType,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");

  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    if (!anIter.Value()
           ->OverlapsTriangle(thePnt1, thePnt2, thePnt3, theSensType, theClipRange, thePickResult))
    {
      continue;
    }

    if (myToAllowOverlap)
    {
      return Standard_True;
    }

    if (isIntersectBoundary(thePnt1, thePnt2) || isIntersectBoundary(thePnt2, thePnt3)
        || isIntersectBoundary(thePnt3, thePnt1))
    {
      return Standard_False;
    }
    return Standard_True;
  }

  return Standard_False;
}

//=================================================================================================

Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsSphere(
  const gp_Pnt&       theCenter,
  const Standard_Real theRadius,
  Standard_Boolean* /*theInside*/) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");
  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->OverlapsSphere(theCenter, theRadius, NULL))
    {
      // select 3 points of the frustum and build a plane on them
      Standard_Real    aMaxDist1 = 0.0, aMaxDist2 = 0.0;
      Standard_Integer anIdx1 = myBoundaryPoints.Lower();
      Standard_Integer anIdx2 = myBoundaryPoints.Lower();
      Standard_Integer anIdx3 = myBoundaryPoints.Lower();
      for (Standard_Integer anIdx = myBoundaryPoints.Lower();
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
      Standard_Real      anAngleSum = 0.0;
      TColgp_Array1OfPnt aBoundaries(myBoundaryPoints.Lower(),
                                     myBoundaryPoints.Size() / 2 + myBoundaryPoints.Lower());

      for (Standard_Integer anIdx = myBoundaryPoints.Lower();
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
      Standard_Boolean isCenterInside   = Abs(anAngleSum - 2 * M_PI) < Precision::Confusion();
      Standard_Boolean isBoundaryInside = Standard_False;
      Standard_Boolean isIntersectSphereBoundaries =
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
  return Standard_False;
}

//=================================================================================================

Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsSphere(
  const gp_Pnt&                  theCenter,
  const Standard_Real            theRadius,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");
  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->OverlapsSphere(theCenter, theRadius, theClipRange, thePickResult))
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsCylinder(
  const Standard_Real            theBottomRad,
  const Standard_Real            theTopRad,
  const Standard_Real            theHeight,
  const gp_Trsf&                 theTrsf,
  const Standard_Boolean         theIsHollow,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");
  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
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

Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsCylinder(
  const Standard_Real    theBottomRad,
  const Standard_Real    theTopRad,
  const Standard_Real    theHeight,
  const gp_Trsf&         theTrsf,
  const Standard_Boolean theIsHollow,
  Standard_Boolean*      theInside) const
{
  const gp_Dir aCylNorm(gp::DZ().Transformed(theTrsf));
  const gp_Pnt aBottomCenter(gp::Origin().Transformed(theTrsf));
  const gp_Pnt aTopCenter = aBottomCenter.XYZ() + aCylNorm.XYZ() * theHeight;

  const gp_Vec aVecPlane1(myFrustums.First()->myVertices[0], myFrustums.First()->myVertices[1]);
  const gp_Vec aVecPlane2(myFrustums.First()->myVertices[0], myFrustums.First()->myVertices[2]);

  const gp_Dir        aDirNorm(aVecPlane1.Crossed(aVecPlane2));
  const Standard_Real anAngle   = aCylNorm.Angle(aDirNorm);
  const Standard_Real aCosAngle = Cos(anAngle);
  const gp_Pln        aPln(myFrustums.First()->myVertices[0], aDirNorm);
  Standard_Real       aCoefA, aCoefB, aCoefC, aCoefD;
  aPln.Coefficients(aCoefA, aCoefB, aCoefC, aCoefD);

  const Standard_Real aTBottom =
    -(aBottomCenter.XYZ().Dot(aDirNorm.XYZ()) + aCoefD) / aDirNorm.Dot(aDirNorm);
  const gp_Pnt aBottomCenterProject(aCoefA * aTBottom + aBottomCenter.X(),
                                    aCoefB * aTBottom + aBottomCenter.Y(),
                                    aCoefC * aTBottom + aBottomCenter.Z());

  const Standard_Real aTTop =
    -(aTopCenter.XYZ().Dot(aDirNorm.XYZ()) + aCoefD) / aDirNorm.Dot(aDirNorm);
  const gp_Pnt aTopCenterProject(aCoefA * aTTop + aTopCenter.X(),
                                 aCoefB * aTTop + aTopCenter.Y(),
                                 aCoefC * aTTop + aTopCenter.Z());

  gp_XYZ              aCylNormProject;
  const gp_XYZ        aTopBottomVec  = aTopCenterProject.XYZ() - aBottomCenterProject.XYZ();
  const Standard_Real aTopBottomDist = aTopBottomVec.Modulus();
  if (aTopBottomDist > 0.0)
  {
    aCylNormProject = aTopBottomVec / aTopBottomDist;
  }

  gp_Pnt aPoints[6];
  aPoints[0] = aBottomCenterProject.XYZ() - aCylNormProject * theBottomRad * Abs(aCosAngle);
  aPoints[1] = aTopCenterProject.XYZ() + aCylNormProject * theTopRad * Abs(aCosAngle);
  const gp_Dir aDirEndFaces = (aCylNorm.IsParallel(aDirNorm, Precision::Angular()))
                                ? gp::DY().Transformed(theTrsf)
                                : aCylNorm.Crossed(aDirNorm);

  aPoints[2] = aTopCenterProject.XYZ() + aDirEndFaces.XYZ() * theTopRad;
  aPoints[3] = aTopCenterProject.XYZ() - aDirEndFaces.XYZ() * theTopRad;
  aPoints[4] = aBottomCenterProject.XYZ() + aDirEndFaces.XYZ() * theBottomRad;
  aPoints[5] = aBottomCenterProject.XYZ() - aDirEndFaces.XYZ() * theBottomRad;

  gp_Pnt             aVerticesBuf[3];
  TColgp_Array1OfPnt aVertices(aVerticesBuf[0], 0, 2);

  bool isCylInsideTriangSet = true;
  for (int i = 0; i < 6; ++i)
  {
    bool isInside = false;
    for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
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
  if (theInside != NULL)
  {
    *theInside &= isCylInsideTriangSet;
  }
  if (isCylInsideTriangSet)
  {
    return true;
  }
  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
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

Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsCircle(
  const Standard_Real            theRadius,
  const gp_Trsf&                 theTrsf,
  const Standard_Boolean         theIsFilled,
  const SelectMgr_ViewClipRange& theClipRange,
  SelectBasics_PickResult&       thePickResult) const
{
  Standard_ASSERT_RAISE(mySelectionType == SelectMgr_SelectionType_Polyline,
                        "Error! SelectMgr_TriangularFrustumSet::Overlaps() should be called after "
                        "selection frustum initialization");
  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
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

Standard_Boolean SelectMgr_TriangularFrustumSet::OverlapsCircle(const Standard_Real    theRadius,
                                                                const gp_Trsf&         theTrsf,
                                                                const Standard_Boolean theIsFilled,
                                                                Standard_Boolean* theInside) const
{
  const gp_Pnt aCenter(gp::Origin().Transformed(theTrsf));
  const gp_Vec aVecPlane1(myFrustums.First()->myVertices[0], myFrustums.First()->myVertices[1]);
  const gp_Vec aVecPlane2(myFrustums.First()->myVertices[0], myFrustums.First()->myVertices[2]);

  const gp_Dir  aDirNorm(aVecPlane1.Crossed(aVecPlane2));
  const gp_Pln  aPln(myFrustums.First()->myVertices[0], aDirNorm);
  Standard_Real aCoefA, aCoefB, aCoefC, aCoefD;
  aPln.Coefficients(aCoefA, aCoefB, aCoefC, aCoefD);

  const Standard_Real aT = -(aCenter.XYZ().Dot(aDirNorm.XYZ()) + aCoefD) / aDirNorm.Dot(aDirNorm);
  const gp_Pnt        aCenterProject(aCoefA * aT + aCenter.X(),
                              aCoefB * aT + aCenter.Y(),
                              aCoefC * aT + aCenter.Z());

  gp_Pnt             aVerticesBuf[3];
  TColgp_Array1OfPnt aVertices(aVerticesBuf[0], 0, 2);

  if (!theIsFilled)
  {
    for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
    {
      if (!anIter.Value()->OverlapsCircle(theRadius, theTrsf, theIsFilled, theInside))
      {
        continue;
      }

      if (myToAllowOverlap)
      {
        return Standard_True;
      }

      if (isIntersectBoundary(theRadius, theTrsf, theIsFilled))
      {
        if (theInside != NULL)
        {
          *theInside &= Standard_False;
        }
        return Standard_False;
      }
      return Standard_True;
    }
  }
  else
  {
    for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
    {
      if (!anIter.Value()->OverlapsCircle(theRadius, theTrsf, theIsFilled, theInside))
      {
        continue;
      }

      if (myToAllowOverlap)
      {
        return Standard_True;
      }

      if (isIntersectBoundary(theRadius, theTrsf, theIsFilled))
      {
        return Standard_False;
      }
      return Standard_True;
    }
  }

  if (theInside != NULL)
  {
    *theInside &= Standard_False;
  }

  return Standard_False;
}

// =======================================================================
// function : GetPlanes
// purpose  :
// =======================================================================
void SelectMgr_TriangularFrustumSet::GetPlanes(
  NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const
{
  thePlaneEquations.Clear();

  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    anIter.Value()->GetPlanes(thePlaneEquations);
  }
}

//=================================================================================================

void SelectMgr_TriangularFrustumSet::SetAllowOverlapDetection(const Standard_Boolean theIsToAllow)
{
  myToAllowOverlap = theIsToAllow;
}

//=================================================================================================

Standard_Boolean SelectMgr_TriangularFrustumSet::pointInTriangle(const gp_Pnt& thePnt,
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

  if (u.Dot(v) < 0.0 || u.Dot(w) < 0.0)
  {
    return false;
  }

  return true;
}

//=================================================================================================

Standard_Boolean SelectMgr_TriangularFrustumSet::segmentSegmentIntersection(
  const gp_Pnt& theStartPnt1,
  const gp_Pnt& theEndPnt1,
  const gp_Pnt& theStartPnt2,
  const gp_Pnt& theEndPnt2)
{
  gp_XYZ aVec1  = theEndPnt1.XYZ() - theStartPnt1.XYZ();
  gp_XYZ aVec2  = theEndPnt2.XYZ() - theStartPnt2.XYZ();
  gp_XYZ aVec21 = theStartPnt2.XYZ() - theStartPnt1.XYZ();
  gp_XYZ aVec12 = theStartPnt1.XYZ() - theStartPnt2.XYZ();
  if (Abs(aVec21.DotCross(aVec1, aVec2)) > Precision::Confusion()
      || Abs(aVec12.DotCross(aVec2, aVec1)) > Precision::Confusion())
  {
    // lines are not coplanar
    return false;
  }

  double aValue1 =
    aVec21.Crossed(aVec2).Dot(aVec1.Crossed(aVec2)) / aVec1.Crossed(aVec2).SquareModulus();
  double aValue2 =
    aVec12.Crossed(aVec1).Dot(aVec2.Crossed(aVec1)) / aVec2.Crossed(aVec1).SquareModulus();
  if (aValue1 < 0.0 || aValue1 > 1.0 || aValue2 < 0.0 || aValue2 > 1.0)
  {
    return false;
  }
  return true;
}

//=================================================================================================

Standard_Boolean SelectMgr_TriangularFrustumSet::isIntersectBoundary(
  const Standard_Real    theRadius,
  const gp_Trsf&         theTrsf,
  const Standard_Boolean theIsFilled) const
{
  Standard_Integer aFacesNb = myBoundaryPoints.Size() / 2;

  const gp_Pnt& aCircCenter = theTrsf.TranslationPart();
  gp_Ax2        anAxis;
  anAxis.Transform(theTrsf);
  Handle(Geom_Circle) aCirc = new Geom_Circle(anAxis, theRadius);

  gp_Dir               aCircNorm  = gp_Dir(0, 0, 1).Transformed(theTrsf);
  Handle(Geom_Surface) aCircPlane = new Geom_Plane(aCircCenter, aCircNorm);

  for (Standard_Integer anIdx = myBoundaryPoints.Lower();
       anIdx < aFacesNb + myBoundaryPoints.Lower();
       anIdx++)
  {
    gp_Pnt aFace[4] = {myBoundaryPoints.Value(anIdx),
                       myBoundaryPoints.Value(anIdx + aFacesNb),
                       myBoundaryPoints.Value(anIdx % aFacesNb + 1 + aFacesNb),
                       myBoundaryPoints.Value(anIdx % aFacesNb + 1)};

    gp_Dir aBndPlaneNorm           = gp_Vec(aFace[0], aFace[1]).Crossed(gp_Vec(aFace[0], aFace[2]));
    Handle(Geom_Surface) aBndPlane = new Geom_Plane(aFace[0], aBndPlaneNorm);

    GeomInt_IntSS anInterSS(aCircPlane, aBndPlane, Precision::Confusion());
    if (!anInterSS.IsDone() || anInterSS.NbLines() == 0)
    {
      continue;
    }

    const Handle(Geom_Line)& anInterLine = Handle(Geom_Line)::DownCast(anInterSS.Line(1));
    Standard_Real            aDistance   = anInterLine->Lin().Distance(aCircCenter);
    if (aDistance > theRadius)
    {
      continue;
    }

    gp_Lin aLine       = anInterLine->Lin();
    gp_Lin aNormalLine = aLine.Normal(aCircCenter);
    gp_Pnt aCrossPoint =
      aCircCenter.Translated(aNormalLine.Direction().Reversed().XYZ() * aDistance);

    Standard_Real anOffset = Sqrt(theRadius * theRadius - aDistance * aDistance);
    // Line-circle intersection points
    gp_Pnt aP1 = aCrossPoint.Translated(aLine.Direction().XYZ() * anOffset);
    gp_Pnt aP2 = aCrossPoint.Translated(aLine.Direction().Reversed().XYZ() * anOffset);

    if (pointInTriangle(aP1, aFace[0], aFace[1], aFace[2])
        || pointInTriangle(aP1, aFace[0], aFace[2], aFace[3])
        || pointInTriangle(aP2, aFace[0], aFace[1], aFace[2])
        || pointInTriangle(aP2, aFace[0], aFace[2], aFace[3]))
    {
      return Standard_True;
    }

    if (theIsFilled || segmentSegmentIntersection(aP1, aP2, aFace[0], aFace[1])
        || segmentSegmentIntersection(aP1, aP2, aFace[1], aFace[2])
        || segmentSegmentIntersection(aP1, aP2, aFace[2], aFace[3])
        || segmentSegmentIntersection(aP1, aP2, aFace[0], aFace[3]))
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean SelectMgr_TriangularFrustumSet::isIntersectBoundary(const gp_Pnt& thePnt1,
                                                                     const gp_Pnt& thePnt2) const
{
  Standard_Integer aFacesNb = myBoundaryPoints.Size() / 2;
  gp_Vec           aDir     = thePnt2.XYZ() - thePnt1.XYZ();
  gp_Pnt           anOrig   = thePnt1;

  for (Standard_Integer anIdx = myBoundaryPoints.Lower();
       anIdx < aFacesNb + myBoundaryPoints.Lower();
       anIdx++)
  {
    gp_Pnt aFace[4] = {myBoundaryPoints.Value(anIdx),
                       myBoundaryPoints.Value(anIdx + aFacesNb),
                       myBoundaryPoints.Value(anIdx % aFacesNb + 1 + aFacesNb),
                       myBoundaryPoints.Value(anIdx % aFacesNb + 1)};

    if (segmentTriangleIntersection(anOrig, aDir, aFace[0], aFace[1], aFace[2])
        || segmentTriangleIntersection(anOrig, aDir, aFace[0], aFace[2], aFace[3]))
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
// function : segmentTriangleIntersection
// purpose  : Moller-Trumbore ray-triangle intersection test
//=======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::segmentTriangleIntersection(const gp_Pnt& theOrig,
                                                                             const gp_Vec& theDir,
                                                                             const gp_Pnt& theV1,
                                                                             const gp_Pnt& theV2,
                                                                             const gp_Pnt& theV3)
{
  gp_Vec        aPVec, aTVec, aQVec;
  Standard_Real aD, aInvD, anU, aV, aT;

  gp_Vec anEdge1 = theV2.XYZ() - theV1.XYZ();
  gp_Vec anEdge2 = theV3.XYZ() - theV1.XYZ();

  aPVec = theDir.Crossed(anEdge2);
  aD    = anEdge1.Dot(aPVec);
  if (fabs(aD) < gp::Resolution())
  {
    return Standard_False;
  }

  aInvD = 1.0 / aD;
  aTVec = theOrig.XYZ() - theV1.XYZ();
  anU   = aInvD * aTVec.Dot(aPVec);
  if (anU < 0.0 || anU > 1.0)
  {
    return Standard_False;
  }

  aQVec = aTVec.Crossed(anEdge1);
  aV    = aInvD * theDir.Dot(aQVec);
  if (aV < 0.0 || anU + aV > 1.0)
  {
    return Standard_False;
  }

  aT = aInvD * anEdge2.Dot(aQVec);
  if (aT < 0 || aT > 1)
  {
    return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : DetectedPoint
// purpose  :
// =======================================================================
gp_Pnt SelectMgr_TriangularFrustumSet::DetectedPoint(const Standard_Real theDepth) const
{
  (void)theDepth;
  throw Standard_ProgramError("SelectMgr_TriangularFrustumSet::DetectedPoint() should not be "
                              "called for Polyline selection type");
}

//=================================================================================================

void SelectMgr_TriangularFrustumSet::DumpJson(Standard_OStream& theOStream,
                                              Standard_Integer  theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, SelectMgr_TriangularFrustumSet)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, SelectMgr_BaseFrustum)

  for (SelectMgr_TriangFrustums::Iterator anIter(myFrustums); anIter.More(); anIter.Next())
  {
    const Handle(SelectMgr_TriangularFrustum)& aFrustum = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aFrustum.get())
  }
}
