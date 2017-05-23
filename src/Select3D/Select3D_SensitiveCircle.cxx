// Created on: 1996-02-06
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Select3D_SensitiveCircle.hxx>

#include <Geom_Circle.hxx>
#include <Precision.hxx>
#include <Select3D_SensitiveTriangle.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveCircle,Select3D_SensitivePoly)

namespace
{
  static Standard_Integer GetCircleNbPoints (const Handle(Geom_Circle)& theCircle,
                                             const Standard_Integer theNbPnts)
  {
    // Check if number of points is invalid.
    // In this case myPolyg raises Standard_ConstructionError
    // exception (look constructor bellow).
    if (theNbPnts <= 0)
      return 0;

    if (theCircle->Radius() > Precision::Confusion())
      return 2 * theNbPnts + 1;

    // The radius is too small and circle degenerates into point
    return 1;
  }

  static Standard_Integer GetArcNbPoints (const Handle(Geom_Circle)& theCircle,
                                          const Standard_Integer theNbPnts)
  {
    // There is no need to check number of points here.
    // In case of invalid number of points this method returns
    // -1 or smaller value.
    if (theCircle->Radius() > Precision::Confusion())
      return 2 * theNbPnts - 1;

    // The radius is too small and circle degenerates into point
    return 1;
  }
}

//=======================================================================
//function : Select3D_SensitiveCircle (constructor)
//purpose  : Definition of a sensitive circle
//=======================================================================
Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                   const Handle(Geom_Circle)& theCircle,
                                                   const Standard_Boolean theIsFilled,
                                                   const Standard_Integer theNbPnts)
: Select3D_SensitivePoly (theOwnerId, !theIsFilled, GetCircleNbPoints (theCircle, theNbPnts)),
  myCircle (theCircle),
  myStart (0),
  myEnd (0)
{
  mySensType = theIsFilled ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
  if (myPolyg.Size() != 1)
  {
    gp_Pnt aP1, aP2;
    gp_Vec aV1;
    Standard_Real anUStart = theCircle->FirstParameter();
    Standard_Real anUEnd = theCircle->LastParameter();
    Standard_Real aStep = (anUEnd - anUStart) / theNbPnts;
    Standard_Real aRadius = theCircle->Radius();
    Standard_Integer aPntIdx = 1;
    Standard_Real aCurU = anUStart;
    for (Standard_Integer anIndex = 1; anIndex <= theNbPnts; anIndex++)
    {
      theCircle->D1 (aCurU, aP1, aV1);

      aV1.Normalize();
      myPolyg.SetPnt (aPntIdx - 1, aP1);
      aPntIdx++;
      aP2 = gp_Pnt (aP1.X() + aV1.X() * tan (aStep / 2.0) * aRadius,
                    aP1.Y() + aV1.Y() * tan (aStep / 2.0) * aRadius,
                    aP1.Z() + aV1.Z() * tan (aStep / 2.0) * aRadius);
      myPolyg.SetPnt (aPntIdx - 1, aP2);
      aPntIdx++;
      aCurU += aStep;
    }

    // Copy the first point to the last point of myPolyg
    myPolyg.SetPnt (theNbPnts * 2, myPolyg.Pnt (0));
    // Get myCenter3D
    myCenter3D = theCircle->Location();
  }
  // Radius = 0.0
  else
  {
    myPolyg.SetPnt (0, theCircle->Location());
    // Get myCenter3D
    myCenter3D = myPolyg.Pnt (0);
  }

  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    SetSensitivityFactor (6);
  }
}

//=======================================================================
//function : Select3D_SensitiveCircle (constructor)
//purpose  : Definition of a sensitive arc
//=======================================================================
Select3D_SensitiveCircle::Select3D_SensitiveCircle (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                    const Handle(Geom_Circle)& theCircle,
                                                    const Standard_Real theU1,
                                                    const Standard_Real theU2,
                                                    const Standard_Boolean theIsFilled,
                                                    const Standard_Integer theNbPnts)
: Select3D_SensitivePoly (theOwnerId, !theIsFilled, GetArcNbPoints (theCircle, theNbPnts)),
  myCircle (theCircle),
  myStart (Min (theU1, theU2)),
  myEnd (Max (theU1, theU2))
{
  mySensType = theIsFilled ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;

  if (myPolyg.Size() != 1)
  {
    gp_Pnt aP1, aP2;
    gp_Vec aV1;

    Standard_Real aStep = (myEnd - myStart) / (theNbPnts - 1);
    Standard_Real aRadius = theCircle->Radius();
    Standard_Integer aPntIdx = 1;
    Standard_Real aCurU = myStart;

    for (Standard_Integer anIndex = 1; anIndex <= theNbPnts - 1; anIndex++)
    {
      theCircle->D1 (aCurU, aP1, aV1);
      aV1.Normalize();
      myPolyg.SetPnt (aPntIdx - 1, aP1);
      aPntIdx++;
      aP2 = gp_Pnt (aP1.X() + aV1.X() * tan (aStep /2.0) * aRadius,
                    aP1.Y() + aV1.Y() * tan (aStep /2.0) * aRadius,
                    aP1.Z() + aV1.Z() * tan (aStep /2.0) * aRadius);
      myPolyg.SetPnt (aPntIdx - 1, aP2);
      aPntIdx++;
      aCurU += aStep;
    }
    theCircle->D0 (myEnd, aP1);
    myPolyg.SetPnt (theNbPnts * 2 - 2, aP1);
    // Get myCenter3D
    myCenter3D = theCircle->Location();
  }
  else
  {
    myPolyg.SetPnt (0, theCircle->Location());
    // Get myCenter3D
    myCenter3D = myPolyg.Pnt (0);
  }

  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    SetSensitivityFactor (6);
  }
}

//=======================================================================
//function : Select3D_SensitiveCircle
//purpose  :
//=======================================================================
Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                   const Handle(TColgp_HArray1OfPnt)& thePnts3d,
                                                   const Standard_Boolean theIsFilled)
: Select3D_SensitivePoly (theOwnerId, thePnts3d, static_cast<Standard_Boolean> (!theIsFilled)),
  myStart (0),
  myEnd (0)
{
  mySensType = theIsFilled ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;

  if (myPolyg.Size() != 1)
    computeCenter3D();
  else
    myCenter3D = myPolyg.Pnt (0);

  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    SetSensitivityFactor (6);
  }
}

//=======================================================================
//function : Select3D_SensitiveCircle
//purpose  :
//=======================================================================

Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                   const TColgp_Array1OfPnt& thePnts3d,
                                                   const Standard_Boolean theIsFilled)
: Select3D_SensitivePoly (theOwnerId, thePnts3d, !theIsFilled),
  myStart (0),
  myEnd (0)
{
  mySensType = theIsFilled ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;

  if (myPolyg.Size() != 1)
    computeCenter3D();
  else
    myCenter3D = myPolyg.Pnt (0);

  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    SetSensitivityFactor (6);
  }
}

//=======================================================================
// function : BVH
// purpose  : Builds BVH tree for a circle's edge segments if needed
//=======================================================================
void Select3D_SensitiveCircle::BVH()
{
  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    Select3D_SensitivePoly::BVH();
  }
}

//=======================================================================
// function : Matches
// purpose  : Checks whether the circle overlaps current selecting volume
//=======================================================================
Standard_Boolean Select3D_SensitiveCircle::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult)
{
  Standard_Real aDepth     = RealLast();
  Standard_Real aDistToCOG = RealLast();

  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    if (!Select3D_SensitivePoly::Matches (theMgr, thePickResult))
    {
      thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
      return Standard_False;
    }
  }
  else if (mySensType == Select3D_TOS_INTERIOR)
  {
    Handle(TColgp_HArray1OfPnt) anArrayOfPnt;
    Points3D (anArrayOfPnt);
    if (!theMgr.IsOverlapAllowed())
    {
      thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
      for (Standard_Integer aPntIdx = anArrayOfPnt->Lower(); aPntIdx <= anArrayOfPnt->Upper(); ++aPntIdx)
      {
        if (!theMgr.Overlaps (anArrayOfPnt->Value (aPntIdx)))
          return Standard_False;
      }
      return Standard_True;
    }

    if (!theMgr.Overlaps (anArrayOfPnt, Select3D_TOS_INTERIOR, aDepth))
    {
      thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
      return Standard_False;
    }
    else
    {
      thePickResult = SelectBasics_PickResult (aDepth, theMgr.DistToGeometryCenter (myCenter3D));
    }
  }

  return Standard_True;
}

void Select3D_SensitiveCircle::ArrayBounds (Standard_Integer & theLow,
                                            Standard_Integer & theUp) const
{
    theLow = 0;
    theUp = myPolyg.Size() - 1;
}

//=======================================================================
//function : GetPoint3d
//purpose  :
//=======================================================================
gp_Pnt Select3D_SensitiveCircle::GetPoint3d (const Standard_Integer thePntIdx) const
{
  if (thePntIdx >= 0 && thePntIdx < myPolyg.Size())
    return myPolyg.Pnt (thePntIdx);

  return gp_Pnt();
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveCircle::GetConnected()
{
  Standard_Boolean isFilled = mySensType == Select3D_TOS_INTERIOR;
  // Create a copy of this
  Handle(Select3D_SensitiveEntity) aNewEntity;
  // this was constructed using Handle(Geom_Circle)
  if(!myCircle.IsNull())
  {
    if ((myEnd - myStart) > Precision::Confusion())
    {
      // Arc
      aNewEntity = new Select3D_SensitiveCircle (myOwnerId, myCircle, myStart, myEnd, isFilled);
    }
    else
    {
      // Circle
      aNewEntity = new Select3D_SensitiveCircle (myOwnerId, myCircle, isFilled);
    }
  }
  // this was constructed using TColgp_Array1OfPnt
  else
  {
    Standard_Integer aSize = myPolyg.Size();
    TColgp_Array1OfPnt aPolyg (1, aSize);
    for(Standard_Integer anIndex = 1; anIndex <= aSize; ++anIndex)
    {
      aPolyg.SetValue(anIndex, myPolyg.Pnt (anIndex-1));
    }
    aNewEntity = new Select3D_SensitiveCircle (myOwnerId, aPolyg, isFilled);
  }

  return aNewEntity;
}

//=======================================================================
//function : computeCenter3D
//purpose  :
//=======================================================================
void Select3D_SensitiveCircle::computeCenter3D()
{
  gp_XYZ aCenter;
  Standard_Integer aNbPnts = myPolyg.Size();
  if (aNbPnts != 1)
  {
    // The mass of points system
    Standard_Integer aMass = aNbPnts - 1;
    // Find the circle barycenter
    for (Standard_Integer anIndex = 0; anIndex < aNbPnts - 1; ++anIndex)
    {
      aCenter += myPolyg.Pnt(anIndex);
    }
    myCenter3D = aCenter / aMass;
  }
  else
  {
    myCenter3D = myPolyg.Pnt(0);
  }
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of the circle. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_SensitiveCircle::CenterOfGeometry() const
{
  return myCenter3D;
}

//=======================================================================
// function : distanceToCOG
// purpose  :
//=======================================================================
Standard_Real Select3D_SensitiveCircle::distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter (myCenter3D);
}
