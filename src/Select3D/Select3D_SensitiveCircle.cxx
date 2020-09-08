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

#include <ElCLib.hxx>
#include <Precision.hxx>
#include <Select3D_SensitiveTriangle.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveCircle,Select3D_SensitivePoly)

namespace
{
  static Standard_Integer GetCircleNbPoints (const gp_Circ& theCircle,
                                             const Standard_Integer theNbPnts)
  {
    // Check if number of points is invalid.
    // In this case myPolyg raises Standard_ConstructionError
    // exception (look constructor bellow).
    if (theNbPnts <= 0)
      return 0;

    if (theCircle.Radius() > Precision::Confusion())
      return 2 * theNbPnts + 1;

    // The radius is too small and circle degenerates into point
    return 1;
  }

  //! Definition of circle polyline
  static void initCircle (Select3D_PointData& thePolygon,
                          const gp_Circ& theCircle,
                          const Standard_Real theU1,
                          const Standard_Real theU2,
                          const Standard_Integer theNbPnts)
  {
    const Standard_Real aStep = (theU2 - theU1) / theNbPnts;
    const Standard_Real aRadius = theCircle.Radius();
    Standard_Integer aPntIdx = 0;
    Standard_Real aCurU = theU1;
    gp_Pnt aP1;
    gp_Vec aV1;
    for (Standard_Integer anIndex = 1; anIndex <= theNbPnts; ++anIndex, aCurU += aStep)
    {
      ElCLib::CircleD1 (aCurU, theCircle.Position(), theCircle.Radius(), aP1, aV1);
      thePolygon.SetPnt (aPntIdx++, aP1);

      aV1.Normalize();
      const gp_Pnt aP2 = aP1.XYZ() + aV1.XYZ() * Tan (aStep * 0.5) * aRadius;
      thePolygon.SetPnt (aPntIdx++, aP2);
    }
    aP1 = ElCLib::CircleValue (theU2, theCircle.Position(), theCircle.Radius());
    thePolygon.SetPnt (theNbPnts * 2, aP1);
  }
}

//=======================================================================
//function : Select3D_SensitiveCircle (constructor)
//purpose  : Definition of a sensitive circle
//=======================================================================
Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                   const gp_Circ& theCircle,
                                                   const Standard_Boolean theIsFilled,
                                                   const Standard_Integer theNbPnts)
: Select3D_SensitivePoly (theOwnerId, !theIsFilled, GetCircleNbPoints (theCircle, theNbPnts)),
  myCircle (theCircle),
  myStart (0.0),
  myEnd (2.0 * M_PI)
{
  mySensType = theIsFilled ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
  myCenter3D = theCircle.Position().Location();
  if (myPolyg.Size() != 1)
  {
    initCircle (myPolyg, theCircle, myStart, myEnd, theNbPnts);
  }
  // Radius = 0.0
  else
  {
    myPolyg.SetPnt (0, theCircle.Position().Location());
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
Select3D_SensitiveCircle::Select3D_SensitiveCircle (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                    const gp_Circ& theCircle,
                                                    const Standard_Real theU1,
                                                    const Standard_Real theU2,
                                                    const Standard_Boolean theIsFilled,
                                                    const Standard_Integer theNbPnts)
: Select3D_SensitivePoly (theOwnerId, !theIsFilled, GetCircleNbPoints (theCircle, theNbPnts)),
  myCircle (theCircle),
  myStart (Min (theU1, theU2)),
  myEnd (Max (theU1, theU2))
{
  mySensType = theIsFilled ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
  myCenter3D = theCircle.Position().Location();
  if (myPolyg.Size() != 1)
  {
    initCircle (myPolyg, theCircle, myStart, myEnd, theNbPnts);
  }
  else
  {
    myPolyg.SetPnt (0, theCircle.Position().Location());
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
Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectMgr_EntityOwner)& theOwnerId,
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

Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectMgr_EntityOwner)& theOwnerId,
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
// function : ToBuildBVH
// purpose  : 
//=======================================================================
Standard_Boolean Select3D_SensitiveCircle::ToBuildBVH() const
{
  if (mySensType != Select3D_TOS_BOUNDARY)
  {
    return Standard_False;
  }

  return Select3D_SensitivePoly::ToBuildBVH();
}

//=======================================================================
// function : Matches
// purpose  : Checks whether the circle overlaps current selecting volume
//=======================================================================
Standard_Boolean Select3D_SensitiveCircle::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult)
{
  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    if (!Select3D_SensitivePoly::Matches (theMgr, thePickResult))
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
      if (theMgr.GetActiveSelectionType() == SelectBasics_SelectingVolumeManager::Polyline)
      {
        SelectBasics_PickResult aDummy;
        return theMgr.Overlaps (anArrayOfPnt, mySensType, aDummy);
      }
      for (Standard_Integer aPntIdx = anArrayOfPnt->Lower(); aPntIdx <= anArrayOfPnt->Upper(); ++aPntIdx)
      {
        if (!theMgr.Overlaps (anArrayOfPnt->Value(aPntIdx)))
        {
          return Standard_False;
        }
      }
      return Standard_True;
    }

    if (!theMgr.Overlaps (anArrayOfPnt, Select3D_TOS_INTERIOR, thePickResult))
    {
      return Standard_False;
    }
    thePickResult.SetDistToGeomCenter(distanceToCOG(theMgr));
  }

  return Standard_True;
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
  if (!Precision::IsInfinite (myCircle.Radius()))
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
