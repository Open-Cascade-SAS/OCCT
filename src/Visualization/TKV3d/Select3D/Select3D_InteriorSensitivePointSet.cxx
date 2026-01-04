// Created on: 2014-08-15
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

#include <gp_XYZ.hxx>

#include <Select3D_InteriorSensitivePointSet.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_InteriorSensitivePointSet, Select3D_SensitiveSet)

namespace
{

// Internal class for creation of planar polygons
class Select3D_Plane
{
public:
  Select3D_Plane()
      : myPlane(0.0),
        myIsInitialized(false)
  {
  }

  bool Contains(const gp_Pnt& thePnt) const
  {
    if (!myIsInitialized)
      return false;

    double aRes =
      myPlane.x() * thePnt.X() + myPlane.y() * thePnt.Y() + myPlane.z() * thePnt.Z() + myPlane.w();

    return aRes < Precision::Confusion();
  }

  void MakePlane(const gp_Pnt& thePnt1, const gp_Pnt& thePnt2, const gp_Pnt& thePnt3)
  {
    const gp_XYZ& aVec1 = thePnt2.XYZ() - thePnt1.XYZ();
    const gp_XYZ& aVec2 = thePnt3.XYZ() - thePnt1.XYZ();
    const gp_XYZ& aDir  = aVec1.Crossed(aVec2);
    double        aD    = aDir.Dot(thePnt1.XYZ().Reversed());
    myPlane             = NCollection_Vec4<double>(aDir.X(), aDir.Y(), aDir.Z(), aD);
    myIsInitialized     = true;
  }

  void Invalidate() { myIsInitialized = false; }

  bool IsValid() const { return myIsInitialized; }

private:
  NCollection_Vec4<double> myPlane;
  bool                     myIsInitialized;
};

} // anonymous namespace

// =======================================================================
// function : Select3D_InteriorSensitivePointSet
// purpose  : Splits the given point set thePoints onto planar convex
//            polygons
// =======================================================================
Select3D_InteriorSensitivePointSet::Select3D_InteriorSensitivePointSet(
  const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
  const NCollection_Array1<gp_Pnt>&         thePoints)
    : Select3D_SensitiveSet(theOwnerId)
{
  Select3D_Plane    aPlane;
  int               aLowerIdx  = thePoints.Lower();
  int               anUpperIdx = thePoints.Upper();
  int               aStartIdx = aLowerIdx, anEndIdx = 0;
  Select3D_BndBox3d aBndBox;
  gp_XYZ            aPntSum(0.0, 0.0, 0.0);
  for (int aPntIter = aLowerIdx; aPntIter <= anUpperIdx; ++aPntIter)
  {
    gp_Pnt        aPnt1, aPnt2;
    const gp_Pnt& aPnt3 = thePoints.Value(aPntIter);
    aPntSum += aPnt3.XYZ();
    NCollection_Vec3<double> aCurrPnt(aPnt3.X(), aPnt3.Y(), aPnt3.Z());
    aBndBox.Add(aCurrPnt);
    if (aPntIter - aLowerIdx >= 2)
    {
      aPnt1 = thePoints.Value(aPntIter - 2);
      aPnt2 = thePoints.Value(aPntIter - 1);
    }
    if (aPntIter - aStartIdx == 2 && !aPlane.IsValid())
    {
      aPlane.MakePlane(aPnt1, aPnt2, aPnt3);
      aStartIdx = aPntIter - 2;
      anEndIdx  = aPntIter;

      if (anEndIdx == anUpperIdx)
      {
        occ::handle<NCollection_HArray1<gp_Pnt>> aPointsArray =
          new NCollection_HArray1<gp_Pnt>(0, anEndIdx - aStartIdx);
        for (int aIdx = aStartIdx; aIdx <= anEndIdx; ++aIdx)
        {
          aPointsArray->SetValue(aIdx - aStartIdx, thePoints.Value(aIdx));
        }
        occ::handle<Select3D_SensitivePoly> aPlanarPolyg =
          new Select3D_SensitivePoly(theOwnerId, aPointsArray, false);
        myPlanarPolygons.Append(aPlanarPolyg);
      }
    }
    else if (aPlane.IsValid())
    {
      const gp_XYZ& aVec1   = aPnt1.XYZ() - aPnt2.XYZ();
      const gp_XYZ& aVec2   = aPnt3.XYZ() - aPnt2.XYZ();
      double        anAngle = aVec1.Dot(aVec2);
      if (!aPlane.Contains(thePoints.Value(aPntIter)) || anAngle > Precision::Confusion())
      {
        // subtract 1 due to indexation from zero in sub-polygons
        int                                      anUpperBound = aPntIter - aStartIdx - 1;
        occ::handle<NCollection_HArray1<gp_Pnt>> aPointsArray =
          new NCollection_HArray1<gp_Pnt>(0, anUpperBound);
        for (int aIdx = aStartIdx; aIdx <= aStartIdx + anUpperBound; ++aIdx)
        {
          aPointsArray->SetValue(aIdx - aStartIdx, thePoints.Value(aIdx));
        }
        occ::handle<Select3D_SensitivePoly> aPlanarPolyg =
          new Select3D_SensitivePoly(theOwnerId, aPointsArray, true);
        myPlanarPolygons.Append(aPlanarPolyg);
        aStartIdx = aPntIter;
        anEndIdx  = aPntIter;
        aPlane.Invalidate();
      }
      else
      {
        anEndIdx++;
        if (anEndIdx == anUpperIdx)
        {
          occ::handle<NCollection_HArray1<gp_Pnt>> aPointsArray =
            new NCollection_HArray1<gp_Pnt>(0, anEndIdx - aStartIdx);
          for (int aIdx = aStartIdx; aIdx <= anEndIdx; ++aIdx)
          {
            aPointsArray->SetValue(aIdx - aStartIdx, thePoints.Value(aIdx));
          }
          occ::handle<Select3D_SensitivePoly> aPlanarPolyg =
            new Select3D_SensitivePoly(theOwnerId, aPointsArray, true);
          myPlanarPolygons.Append(aPlanarPolyg);
        }
      }
    }
  }

  myCOG    = aPntSum / thePoints.Length();
  myBndBox = aBndBox;

  myPolygonsIdxs = new NCollection_HArray1<int>(0, myPlanarPolygons.Length() - 1);
  for (int aIdx = 0; aIdx < myPlanarPolygons.Length(); ++aIdx)
  {
    myPolygonsIdxs->SetValue(aIdx, aIdx);
  }
}

// =======================================================================
// function : GetPoints
// purpose  : Initializes the given array theHArrayOfPnt by 3d
//            coordinates of vertices of the whole point set
// =======================================================================
void Select3D_InteriorSensitivePointSet::GetPoints(
  occ::handle<NCollection_HArray1<gp_Pnt>>& theHArrayOfPnt)
{
  int aSize = 0;
  for (int anIdx = 0; anIdx < myPlanarPolygons.Length(); ++anIdx)
  {
    const occ::handle<Select3D_SensitivePoly>& aPolygon = myPlanarPolygons.Value(anIdx);
    aSize += aPolygon->NbSubElements();
  }

  theHArrayOfPnt          = new NCollection_HArray1<gp_Pnt>(1, aSize);
  int anOutputPntArrayIdx = 1;

  for (int aPolygIdx = 0; aPolygIdx < myPlanarPolygons.Length(); ++aPolygIdx)
  {
    const occ::handle<Select3D_SensitivePoly>& aPolygon = myPlanarPolygons.Value(aPolygIdx);
    occ::handle<NCollection_HArray1<gp_Pnt>>   aPoints;
    aPolygon->Points3D(aPoints);
    int anUpper =
      aPolygIdx < myPlanarPolygons.Length() - 1 ? aPoints->Upper() : aPoints->Upper() + 1;
    for (int aPntIter = 1; aPntIter < anUpper; ++aPntIter)
    {
      theHArrayOfPnt->SetValue(anOutputPntArrayIdx, aPoints->Value(aPntIter));
      anOutputPntArrayIdx++;
    }
    aPoints.Nullify();
  }
}

//=======================================================================
// function : Size
// purpose  : Returns the length of vector of planar convex polygons
//=======================================================================
int Select3D_InteriorSensitivePointSet::Size() const
{
  return myPlanarPolygons.Length();
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of planar convex polygon with index
//            theIdx
//=======================================================================
Select3D_BndBox3d Select3D_InteriorSensitivePointSet::Box(const int theIdx) const
{
  int aPolygIdx = myPolygonsIdxs->Value(theIdx);
  return myPlanarPolygons.Value(aPolygIdx)->BoundingBox();
}

//=======================================================================
// function : Center
// purpose  : Returns geometry center of planar convex polygon with index
//            theIdx in the vector along the given axis theAxis
//=======================================================================
double Select3D_InteriorSensitivePointSet::Center(const int theIdx, const int theAxis) const
{
  const int    aPolygIdx = myPolygonsIdxs->Value(theIdx);
  const gp_Pnt aCOG      = myPlanarPolygons.Value(aPolygIdx)->CenterOfGeometry();
  return aCOG.Coord(theAxis - 1);
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2 in the vector
//=======================================================================
void Select3D_InteriorSensitivePointSet::Swap(const int theIdx1, const int theIdx2)
{
  int aPolygIdx1 = myPolygonsIdxs->Value(theIdx1);
  int aPolygIdx2 = myPolygonsIdxs->Value(theIdx2);

  myPolygonsIdxs->ChangeValue(theIdx1) = aPolygIdx2;
  myPolygonsIdxs->ChangeValue(theIdx2) = aPolygIdx1;
}

//=================================================================================================

bool Select3D_InteriorSensitivePointSet::overlapsElement(
  SelectBasics_PickResult&             thePickResult,
  SelectBasics_SelectingVolumeManager& theMgr,
  int                                  theElemIdx,
  bool)
{
  int                                        aPolygIdx = myPolygonsIdxs->Value(theElemIdx);
  const occ::handle<Select3D_SensitivePoly>& aPolygon  = myPlanarPolygons.Value(aPolygIdx);
  occ::handle<NCollection_HArray1<gp_Pnt>>   aPoints;
  aPolygon->Points3D(aPoints);
  return theMgr.OverlapsPolygon(aPoints->Array1(), Select3D_TOS_INTERIOR, thePickResult);
}

//=================================================================================================

bool Select3D_InteriorSensitivePointSet::elementIsInside(
  SelectBasics_SelectingVolumeManager& theMgr,
  int                                  theElemIdx,
  bool                                 theIsFullInside)
{
  SelectBasics_PickResult aDummy;
  return overlapsElement(aDummy, theMgr, theElemIdx, theIsFullInside);
}

// =======================================================================
// function : distanceToCOG
// purpose  : Calculates distance from the 3d projection of used-picked
//            screen point to center of the geometry
// =======================================================================
double Select3D_InteriorSensitivePointSet::distanceToCOG(
  SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter(myCOG);
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns bounding box of the point set. If location
//            transformation is set, it will be applied
//=======================================================================
Select3D_BndBox3d Select3D_InteriorSensitivePointSet::BoundingBox()
{
  return myBndBox;
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of the point set. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_InteriorSensitivePointSet::CenterOfGeometry() const
{
  return myCOG;
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of points in set
//=======================================================================
int Select3D_InteriorSensitivePointSet::NbSubElements() const
{
  return myPlanarPolygons.Length();
}

//=================================================================================================

void Select3D_InteriorSensitivePointSet::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Select3D_SensitiveSet)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBndBox)
}
