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

#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>

#include <Select3D_InteriorSensitivePointSet.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_InteriorSensitivePointSet,Select3D_SensitiveSet)

namespace {

// Internal class for creation of planar polygons
class Select3D_Plane
{
public:

  Select3D_Plane()
    : myPlane (0.0),
      myIsInitialized (Standard_False)
  {}

  Standard_Boolean Contains (const gp_Pnt& thePnt) const
  {
    if (!myIsInitialized)
      return Standard_False;

    Standard_Real aRes = myPlane.x() * thePnt.X() +
                         myPlane.y() * thePnt.Y() +
                         myPlane.z() * thePnt.Z() +
                         myPlane.w();

    if (aRes < Precision::Confusion())
      return Standard_True;

    return Standard_False;
  }

  void MakePlane (const gp_Pnt& thePnt1,
                  const gp_Pnt& thePnt2,
                  const gp_Pnt& thePnt3)
  {
    const gp_XYZ& aVec1 = thePnt2.XYZ() - thePnt1.XYZ();
    const gp_XYZ& aVec2 = thePnt3.XYZ() - thePnt1.XYZ();
    const gp_XYZ& aDir  = aVec1.Crossed (aVec2);
    Standard_Real aD = aDir.Dot (thePnt1.XYZ().Reversed());
    myPlane = NCollection_Vec4<Standard_Real> (aDir.X(), aDir.Y(), aDir.Z(), aD);
    myIsInitialized = Standard_True;
  }

  void Invalidate()
  {
    myIsInitialized = Standard_False;
  }

  Standard_Boolean IsValid() const
  {
    return myIsInitialized;
  }

private:
  NCollection_Vec4<Standard_Real> myPlane;
  Standard_Boolean                myIsInitialized;
};

} // anonymous namespace


// =======================================================================
// function : Select3D_InteriorSensitivePointSet
// purpose  : Splits the given point set thePoints onto planar convex
//            polygons
// =======================================================================
Select3D_InteriorSensitivePointSet::Select3D_InteriorSensitivePointSet (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                                        const TColgp_Array1OfPnt& thePoints)
  : Select3D_SensitiveSet (theOwnerId)
{
  Select3D_Plane aPlane;
  Standard_Integer aLowerIdx = thePoints.Lower();
  Standard_Integer anUpperIdx = thePoints.Upper();
  Standard_Integer aStartIdx = aLowerIdx, anEndIdx = 0;
  Select3D_BndBox3d aBndBox;
  gp_XYZ aPntSum (0.0, 0.0, 0.0);
  for (Standard_Integer aPntIter = aLowerIdx; aPntIter <= anUpperIdx; ++aPntIter)
  {
    gp_Pnt aPnt1, aPnt2;
    const gp_Pnt& aPnt3 = thePoints.Value (aPntIter);
    aPntSum += aPnt3.XYZ();
    SelectMgr_Vec3 aCurrPnt (aPnt3.X(), aPnt3.Y(), aPnt3.Z());
    aBndBox.Add (aCurrPnt);
    if (aPntIter - aLowerIdx >= 2)
    {
      aPnt1 = thePoints.Value (aPntIter - 2);
      aPnt2 = thePoints.Value (aPntIter - 1);
    }
    if (aPntIter - aStartIdx == 2 && !aPlane.IsValid())
    {
      aPlane.MakePlane (aPnt1, aPnt2, aPnt3);
      aStartIdx = aPntIter - 2;
      anEndIdx = aPntIter;

      if (anEndIdx == anUpperIdx)
      {
        Handle (TColgp_HArray1OfPnt) aPointsArray = new TColgp_HArray1OfPnt (0, anEndIdx - aStartIdx);
        for (Standard_Integer aIdx = aStartIdx; aIdx <= anEndIdx; ++aIdx)
        {
          aPointsArray->SetValue (aIdx - aStartIdx, thePoints.Value(aIdx));
        }
        Handle(Select3D_SensitivePoly) aPlanarPolyg = new Select3D_SensitivePoly (theOwnerId,
                                                                                 aPointsArray,
                                                                                 Standard_False);
        myPlanarPolygons.Append (aPlanarPolyg);
      }
    }
    else if (aPlane.IsValid())
    {
      const gp_XYZ& aVec1 = aPnt1.XYZ() - aPnt2.XYZ();
      const gp_XYZ& aVec2 = aPnt3.XYZ() - aPnt2.XYZ();
      Standard_Real anAngle = aVec1.Dot (aVec2);
      if (!aPlane.Contains (thePoints.Value (aPntIter)) || anAngle > Precision::Confusion())
      {
        // subtract 1 due to indexation from zero in sub-polygons
        Standard_Integer anUpperBound = aPntIter - aStartIdx - 1;
        Handle (TColgp_HArray1OfPnt) aPointsArray = new TColgp_HArray1OfPnt (0, anUpperBound);
        for (Standard_Integer aIdx = aStartIdx; aIdx <= aStartIdx + anUpperBound; ++aIdx)
        {
          aPointsArray->SetValue (aIdx - aStartIdx, thePoints.Value (aIdx));
        }
        Handle(Select3D_SensitivePoly) aPlanarPolyg = new Select3D_SensitivePoly (theOwnerId,
                                                                                  aPointsArray,
                                                                                  Standard_True);
        myPlanarPolygons.Append (aPlanarPolyg);
        aStartIdx = aPntIter;
        anEndIdx = aPntIter;
        aPlane.Invalidate();
      }
      else
      {
        anEndIdx++;
        if (anEndIdx == anUpperIdx)
        {
          Handle (TColgp_HArray1OfPnt) aPointsArray = new TColgp_HArray1OfPnt (0, anEndIdx - aStartIdx);
          for (Standard_Integer aIdx = aStartIdx; aIdx <= anEndIdx; ++aIdx)
          {
            aPointsArray->SetValue (aIdx - aStartIdx, thePoints.Value (aIdx));
          }
          Handle(Select3D_SensitivePoly) aPlanarPolyg = new Select3D_SensitivePoly (theOwnerId,
                                                                                    aPointsArray,
                                                                                    Standard_True);
          myPlanarPolygons.Append (aPlanarPolyg);
        }
      }
    }
  }

  myCOG = aPntSum / thePoints.Length();
  myBndBox = aBndBox;

  myPolygonsIdxs = new TColStd_HArray1OfInteger (0, myPlanarPolygons.Length() - 1);
  for (Standard_Integer aIdx = 0; aIdx < myPlanarPolygons.Length(); ++aIdx)
  {
    myPolygonsIdxs->SetValue (aIdx, aIdx);
  }
}

// =======================================================================
// function : GetPoints
// purpose  : Initializes the given array theHArrayOfPnt by 3d
//            coordinates of vertices of the whole point set
// =======================================================================
void Select3D_InteriorSensitivePointSet::GetPoints (Handle(TColgp_HArray1OfPnt)& theHArrayOfPnt)
{
  Standard_Integer aSize = 0;
  for (Standard_Integer anIdx = 0; anIdx < myPlanarPolygons.Length(); ++anIdx)
  {
    const Handle(Select3D_SensitivePoly)& aPolygon = myPlanarPolygons.Value (anIdx);
    aSize += aPolygon->NbSubElements();
  }

  theHArrayOfPnt = new TColgp_HArray1OfPnt (1, aSize);
  Standard_Integer anOutputPntArrayIdx = 1;

  for (Standard_Integer aPolygIdx = 0; aPolygIdx < myPlanarPolygons.Length(); ++aPolygIdx)
  {
    const Handle(Select3D_SensitivePoly)& aPolygon = myPlanarPolygons.Value (aPolygIdx);
    Handle(TColgp_HArray1OfPnt) aPoints;
    aPolygon->Points3D (aPoints);
    Standard_Integer anUpper = aPolygIdx < myPlanarPolygons.Length() - 1 ? aPoints->Upper() : aPoints->Upper() + 1;
    for (Standard_Integer aPntIter = 1; aPntIter < anUpper; ++aPntIter)
    {
      theHArrayOfPnt->SetValue (anOutputPntArrayIdx, aPoints->Value (aPntIter));
      anOutputPntArrayIdx++;
    }
    aPoints.Nullify();
  }
}

//=======================================================================
// function : Size
// purpose  : Returns the length of vector of planar convex polygons
//=======================================================================
Standard_Integer Select3D_InteriorSensitivePointSet::Size() const
{
  return myPlanarPolygons.Length();
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of planar convex polygon with index
//            theIdx
//=======================================================================
Select3D_BndBox3d Select3D_InteriorSensitivePointSet::Box (const Standard_Integer theIdx) const
{
  Standard_Integer aPolygIdx = myPolygonsIdxs->Value (theIdx);
  return myPlanarPolygons.Value (aPolygIdx)->BoundingBox();
}

//=======================================================================
// function : Center
// purpose  : Returns geometry center of planar convex polygon with index
//            theIdx in the vector along the given axis theAxis
//=======================================================================
Standard_Real Select3D_InteriorSensitivePointSet::Center (const Standard_Integer theIdx,
                                                          const Standard_Integer theAxis) const
{
  Standard_Integer aPolygIdx = myPolygonsIdxs->Value (theIdx);
  const gp_XYZ& aCOG = myPlanarPolygons.Value (aPolygIdx)->CenterOfGeometry().XYZ();
  Standard_Real aCenter = theAxis == 0 ? aCOG.X() : (theAxis == 1 ? aCOG.Y() : aCOG.Z());

  return aCenter;
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2 in the vector
//=======================================================================
void Select3D_InteriorSensitivePointSet::Swap (const Standard_Integer theIdx1,
                                               const Standard_Integer theIdx2)
{
  Standard_Integer aPolygIdx1 = myPolygonsIdxs->Value (theIdx1);
  Standard_Integer aPolygIdx2 = myPolygonsIdxs->Value (theIdx2);

  myPolygonsIdxs->ChangeValue (theIdx1) = aPolygIdx2;
  myPolygonsIdxs->ChangeValue (theIdx2) = aPolygIdx1;
}

// =======================================================================
// function : overlapsElement
// purpose  : Checks whether the planar convex polygon with index theIdx
//            in myPlanarPolygons overlaps the current selecting volume
// =======================================================================
Standard_Boolean Select3D_InteriorSensitivePointSet::overlapsElement (SelectBasics_SelectingVolumeManager& theMgr,
                                                                      Standard_Integer theElemIdx,
                                                                      Standard_Real& theMatchDepth)
{
  Standard_Integer aPolygIdx = myPolygonsIdxs->Value (theElemIdx);
  const Handle(Select3D_SensitivePoly)& aPolygon = myPlanarPolygons.Value (aPolygIdx);
  Handle(TColgp_HArray1OfPnt) aPoints;
  aPolygon->Points3D (aPoints);
  return theMgr.Overlaps (aPoints, Select3D_TOS_INTERIOR, theMatchDepth);
}

// =======================================================================
// function : elementIsInside
// purpose  :
// =======================================================================
Standard_Boolean Select3D_InteriorSensitivePointSet::elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                                      const Standard_Integer               theElemIdx)
{
  Standard_Real aDummy;
  return overlapsElement (theMgr, theElemIdx, aDummy);
}

// =======================================================================
// function : distanceToCOG
// purpose  : Calculates distance from the 3d projection of used-picked
//            screen point to center of the geometry
// =======================================================================
Standard_Real Select3D_InteriorSensitivePointSet::distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter (myCOG);
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
Standard_Integer Select3D_InteriorSensitivePointSet::NbSubElements()
{
  return myPlanarPolygons.Length();
}
