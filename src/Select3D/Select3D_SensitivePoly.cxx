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

#include <Select3D_SensitivePoly.hxx>

#include <ElCLib.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitivePoly,Select3D_SensitiveSet)

namespace
{
  static Standard_Integer GetCircleNbPoints (const gp_Circ& theCircle,
                                             const Standard_Integer theNbPnts,
                                             const Standard_Real theU1,
                                             const Standard_Real theU2,
                                             const Standard_Boolean theIsFilled)
  {
    // Check if number of points is invalid.
    // In this case myPolyg raises Standard_ConstructionError
    // exception (see constructor below).
    if (theNbPnts <= 0)
    {
      return 0;
    }

    if (theCircle.Radius() > Precision::Confusion())
    {
      const Standard_Boolean isSector = theIsFilled && Abs (Abs (theU2 - theU1) - 2.0 * M_PI) > gp::Resolution();
      return 2 * theNbPnts + 1 + (isSector ? 2 : 0);
    }

    // The radius is too small and circle degenerates into point
    return 1;
  }

  //! Definition of circle polyline
  static void initCircle (Select3D_PointData& thePolygon,
                          const gp_Circ&      theCircle,
                          const Standard_Real theU1,
                          const Standard_Real theU2,
                          const Standard_Boolean theIsFilled,
                          const Standard_Integer theNbPnts)
  {
    const Standard_Real aStep = (theU2 - theU1) / theNbPnts;
    const Standard_Real aRadius = theCircle.Radius();
    Standard_Integer aPntIdx = 0;
    Standard_Real aCurU = theU1;
    gp_Pnt aP1;
    gp_Vec aV1;

    const Standard_Boolean isSector = Abs (theU2 - theU1 - 2.0 * M_PI) > gp::Resolution();

    if (isSector && theIsFilled) { thePolygon.SetPnt (aPntIdx++, theCircle.Location()); }

    for (Standard_Integer anIndex = 1; anIndex <= theNbPnts; ++anIndex, aCurU += aStep)
    {
      ElCLib::CircleD1 (aCurU, theCircle.Position(), theCircle.Radius(), aP1, aV1);
      thePolygon.SetPnt (aPntIdx++, aP1);

      aV1.Normalize();
      const gp_Pnt aP2 = aP1.XYZ() + aV1.XYZ() * Tan (aStep * 0.5) * aRadius;
      thePolygon.SetPnt (aPntIdx++, aP2);
    }
    aP1 = ElCLib::CircleValue (theU2, theCircle.Position(), theCircle.Radius());
    thePolygon.SetPnt (aPntIdx++, aP1);

    if (isSector && theIsFilled) { thePolygon.SetPnt (aPntIdx++, theCircle.Location()); }
  }
}

//==================================================
// Function: Select3D_SensitivePoly
// Purpose :
//==================================================
Select3D_SensitivePoly::Select3D_SensitivePoly (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                const TColgp_Array1OfPnt& thePoints,
                                                const Standard_Boolean theIsBVHEnabled)
: Select3D_SensitiveSet (theOwnerId),
  myPolyg (thePoints.Upper() - thePoints.Lower() + 1),
  mySensType (Select3D_TOS_BOUNDARY)
{
  Standard_Integer aLowerIdx = thePoints.Lower();
  Standard_Integer anUpperIdx = thePoints.Upper();
  gp_XYZ aPntSum (0.0, 0.0, 0.0);

  Select3D_BndBox3d aBndBox;
  for (Standard_Integer aIdx = aLowerIdx; aIdx <= anUpperIdx; ++aIdx)
  {
    aPntSum += thePoints.Value (aIdx).XYZ();
    const SelectMgr_Vec3 aPnt (thePoints.Value (aIdx).X(),
                               thePoints.Value (aIdx).Y(),
                               thePoints.Value (aIdx).Z());
    aBndBox.Add (aPnt);
    myPolyg.SetPnt (aIdx - aLowerIdx, thePoints.Value (aIdx));
  }

  myBndBox = aBndBox;
  myCOG = aPntSum / myPolyg.Size();

  if (theIsBVHEnabled)
  {
    const Standard_Integer aPntsNum = myPolyg.Size();
    mySegmentIndexes = new TColStd_HArray1OfInteger (0, aPntsNum - 2);
    for (Standard_Integer aSegmIter = 0; aSegmIter < aPntsNum - 1; ++aSegmIter)
    {
      mySegmentIndexes->SetValue (aSegmIter, aSegmIter);
    }
  }

  myIsComputed = Standard_True;
}

//==================================================
// Function: Select3D_SensitivePoly
// Purpose :
//==================================================
Select3D_SensitivePoly::Select3D_SensitivePoly (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                const Handle(TColgp_HArray1OfPnt)& thePoints,
                                                const Standard_Boolean theIsBVHEnabled)
: Select3D_SensitiveSet (theOwnerId),
  myPolyg (thePoints->Upper() - thePoints->Lower() + 1),
  mySensType (Select3D_TOS_BOUNDARY)
{
  Standard_Integer aLowerIdx = thePoints->Lower();
  Standard_Integer anUpperIdx = thePoints->Upper();
  gp_XYZ aPntSum (0.0, 0.0, 0.0);

  Select3D_BndBox3d aBndBox;
  for (Standard_Integer aIdx = aLowerIdx; aIdx <= anUpperIdx; ++aIdx)
  {
    aPntSum += thePoints->Value (aIdx).XYZ();
    const SelectMgr_Vec3 aPnt (thePoints->Value (aIdx).X(),
                               thePoints->Value (aIdx).Y(),
                               thePoints->Value (aIdx).Z());
    aBndBox.Add (aPnt);
    myPolyg.SetPnt (aIdx - aLowerIdx, thePoints->Value (aIdx));
  }

  myBndBox = aBndBox;
  myCOG = aPntSum / myPolyg.Size();

  if (theIsBVHEnabled)
  {
    const Standard_Integer aPntsNum = myPolyg.Size();
    mySegmentIndexes = new TColStd_HArray1OfInteger (0, aPntsNum - 2);
    for (Standard_Integer aSegmIter = 0; aSegmIter < aPntsNum - 1; ++aSegmIter)
    {
      mySegmentIndexes->SetValue (aSegmIter, aSegmIter);
    }
  }

  myIsComputed = Standard_True;
}

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitivePoly::Select3D_SensitivePoly (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                const Standard_Boolean theIsBVHEnabled,
                                                const Standard_Integer theNbPnts)
: Select3D_SensitiveSet (theOwnerId),
  myPolyg (theNbPnts),
  mySensType (Select3D_TOS_BOUNDARY)
{
  if (theIsBVHEnabled)
  {
    mySegmentIndexes = new TColStd_HArray1OfInteger (0, theNbPnts - 2);
    for (Standard_Integer aIdx = 0; aIdx < theNbPnts - 1; ++aIdx)
    {
      mySegmentIndexes->SetValue (aIdx, aIdx);
    }
  }
  myCOG = gp_Pnt (RealLast(), RealLast(), RealLast());
  myIsComputed = Standard_False;
}

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitivePoly::Select3D_SensitivePoly (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                const gp_Circ& theCircle,
                                                const Standard_Real theU1,
                                                const Standard_Real theU2,
                                                const Standard_Boolean theIsFilled,
                                                const Standard_Integer theNbPnts)
: Select3D_SensitivePoly (theOwnerId, !theIsFilled, GetCircleNbPoints (theCircle, theNbPnts, theU1, theU2, theIsFilled))
{
  mySensType = theIsFilled ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;

  if (myPolyg.Size() != 1)
  {
    initCircle (myPolyg, theCircle, Min (theU1, theU2), Max (theU1, theU2), theIsFilled, theNbPnts);
  }
  else
  {
    myPolyg.SetPnt (0, theCircle.Position().Location());
  }

  if (!theIsFilled)
  {
    SetSensitivityFactor (6);
  }
}

//=======================================================================
// function : Matches
// purpose  :
//=======================================================================
Standard_Boolean Select3D_SensitivePoly::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                  SelectBasics_PickResult& thePickResult)
{
  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    if (!Select3D_SensitiveSet::Matches (theMgr, thePickResult))
    {
      return Standard_False;
    }
  }
  else if (mySensType == Select3D_TOS_INTERIOR)
  {
    Handle(TColgp_HArray1OfPnt) anArrayOfPnt;
    Points3D (anArrayOfPnt);
    if (!theMgr.IsOverlapAllowed())
    {
      if (theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Polyline)
      {
        SelectBasics_PickResult aDummy;
        return theMgr.OverlapsPolygon (anArrayOfPnt->Array1(), mySensType, aDummy);
      }
      for (Standard_Integer aPntIdx = anArrayOfPnt->Lower(); aPntIdx <= anArrayOfPnt->Upper(); ++aPntIdx)
      {
        if (!theMgr.OverlapsPoint (anArrayOfPnt->Value(aPntIdx)))
        {
          return Standard_False;
        }
      }
      return Standard_True;
    }

    if (!theMgr.OverlapsPolygon (anArrayOfPnt->Array1(), Select3D_TOS_INTERIOR, thePickResult))
    {
      return Standard_False;
    }
    thePickResult.SetDistToGeomCenter (distanceToCOG(theMgr));
  }

  return Standard_True;
}

//==================================================
// function : BoundingBox
// purpose  : Returns bounding box of a polygon. If location
//            transformation is set, it will be applied
//==================================================
Select3D_BndBox3d Select3D_SensitivePoly::BoundingBox()
{
  if (myBndBox.IsValid())
    return myBndBox;

  Select3D_BndBox3d aBndBox;
  for (Standard_Integer aPntIter = 0; aPntIter < myPolyg.Size(); ++aPntIter)
  {
    SelectMgr_Vec3 aPnt (myPolyg.Pnt (aPntIter).x,
                         myPolyg.Pnt (aPntIter).y,
                         myPolyg.Pnt (aPntIter).z);
    aBndBox.Add (aPnt);
  }

  myBndBox = aBndBox;

  return myBndBox;
}

//==================================================
// Function: Size
// Purpose : Returns the amount of segments of
//           the poly
//==================================================
Standard_Integer Select3D_SensitivePoly::Size() const
{
  if (!mySegmentIndexes.IsNull())
    return mySegmentIndexes->Length();

  return -1;
}

//==================================================
// Function: Box
// Purpose : Returns bounding box of segment with
//           index theIdx
//==================================================
Select3D_BndBox3d Select3D_SensitivePoly::Box (const Standard_Integer theIdx) const
{
  if (mySegmentIndexes.IsNull())
    return Select3D_BndBox3d (SelectMgr_Vec3 (RealLast()));

  const Standard_Integer aSegmentIdx = mySegmentIndexes->Value (theIdx);
  gp_Pnt aPnt1 = myPolyg.Pnt3d (aSegmentIdx);
  gp_Pnt aPnt2 = myPolyg.Pnt3d (aSegmentIdx + 1);

  const SelectMgr_Vec3 aMinPnt (Min (aPnt1.X(), aPnt2.X()),
                                Min (aPnt1.Y(), aPnt2.Y()),
                                Min (aPnt1.Z(), aPnt2.Z()));
  const SelectMgr_Vec3 aMaxPnt (Max (aPnt1.X(), aPnt2.X()),
                                Max (aPnt1.Y(), aPnt2.Y()),
                                Max (aPnt1.Z(), aPnt2.Z()));

  return Select3D_BndBox3d (aMinPnt, aMaxPnt);
}

//==================================================
// Function: Center
// Purpose : Returns geometry center of sensitive
//           entity index theIdx in the vector along
//           the given axis theAxis
//==================================================
Standard_Real Select3D_SensitivePoly::Center (const Standard_Integer theIdx,
                                              const Standard_Integer theAxis) const
{
  if (mySegmentIndexes.IsNull())
    return RealLast();

  const Select3D_BndBox3d aBndBox = Box (theIdx);
  const SelectMgr_Vec3 aCenter = (aBndBox.CornerMin() + aBndBox.CornerMax()) * 0.5;
  return theAxis == 0 ? aCenter.x() : (theAxis == 1 ? aCenter.y() : aCenter.z());
}

//==================================================
// Function: Swap
// Purpose : Swaps items with indexes theIdx1 and
//           theIdx2 in the vector
//==================================================
void Select3D_SensitivePoly::Swap (const Standard_Integer theIdx1,
                                   const Standard_Integer theIdx2)
{
  if (mySegmentIndexes.IsNull())
    return;

  const Standard_Integer aSegmentIdx1 = mySegmentIndexes->Value (theIdx1);
  const Standard_Integer aSegmentIdx2 = mySegmentIndexes->Value (theIdx2);
  mySegmentIndexes->ChangeValue (theIdx1) = aSegmentIdx2;
  mySegmentIndexes->ChangeValue (theIdx2) = aSegmentIdx1;
}

//==================================================
// Function: overlapsElement
// Purpose : Checks whether the segment with index
//           theIdx overlaps the current selecting
//           volume
//==================================================
Standard_Boolean Select3D_SensitivePoly::overlapsElement (SelectBasics_PickResult& thePickResult,
                                                          SelectBasics_SelectingVolumeManager& theMgr,
                                                          Standard_Integer theElemIdx,
                                                          Standard_Boolean theIsFullInside)
{
  if (mySegmentIndexes.IsNull())
  {
    return Standard_False;
  }
  else if (theIsFullInside)
  {
    return Standard_True;
  }

  const Standard_Integer aSegmentIdx = mySegmentIndexes->Value (theElemIdx);
  gp_Pnt aPnt1 = myPolyg.Pnt3d (aSegmentIdx);
  gp_Pnt aPnt2 = myPolyg.Pnt3d (aSegmentIdx + 1);
  return theMgr.OverlapsSegment (aPnt1, aPnt2, thePickResult);
}

//==================================================
// Function : elementIsInside
// Purpose  :
//==================================================
Standard_Boolean Select3D_SensitivePoly::elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                          Standard_Integer theElemIdx,
                                                          Standard_Boolean theIsFullInside)
{
  if (theIsFullInside)
  {
    return Standard_True;
  }

  const Standard_Integer aSegmentIdx = mySegmentIndexes->Value (theElemIdx);
  if (theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Polyline)
  {
    SelectBasics_PickResult aDummy;
    return theMgr.OverlapsSegment (myPolyg.Pnt3d (aSegmentIdx + 0), myPolyg.Pnt3d (aSegmentIdx + 1), aDummy);
  }
  return theMgr.OverlapsPoint (myPolyg.Pnt3d (aSegmentIdx + 0))
      && theMgr.OverlapsPoint (myPolyg.Pnt3d (aSegmentIdx + 1));
}

//==================================================
// Function: distanceToCOG
// Purpose : Calculates distance from the 3d
//           projection of used-picked screen point
//           to center of the geometry
//==================================================
Standard_Real Select3D_SensitivePoly::distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr)
{
  if (!myIsComputed)
  {
    gp_XYZ aCenter (0.0, 0.0, 0.0);
    for (Standard_Integer aIdx = 0; aIdx < myPolyg.Size(); ++aIdx)
    {
      aCenter += myPolyg.Pnt (aIdx);
    }
    myCOG = aCenter / myPolyg.Size();
    myIsComputed = Standard_True;
  }

  return theMgr.DistToGeometryCenter (myCOG);
}

//==================================================
// Function: NbSubElements
// Purpose : Returns the amount of segments in poly
//==================================================
Standard_Integer Select3D_SensitivePoly::NbSubElements() const
{
  return myPolyg.Size();
}

//==================================================
// Function: CenterOfGeometry
// Purpose : Returns center of the point set. If
//           location transformation is set, it will
//           be applied
//==================================================
gp_Pnt Select3D_SensitivePoly::CenterOfGeometry() const
{
  if (!myIsComputed)
  {
    gp_XYZ aCenter (0.0, 0.0, 0.0);
    for (Standard_Integer aIdx = 0; aIdx < myPolyg.Size(); ++aIdx)
    {
      aCenter += myPolyg.Pnt (aIdx);
    }
    myCOG = aCenter / myPolyg.Size();
    myIsComputed = Standard_True;
  }

  return myCOG;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Select3D_SensitivePoly::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, Select3D_SensitiveSet)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myBndBox)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsComputed)
}
