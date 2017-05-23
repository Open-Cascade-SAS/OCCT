// Created on: 1997-05-14
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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

#include <Select3D_SensitiveTriangle.hxx>

#include <Precision.hxx>
#include <Bnd_Box.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveTriangle,Select3D_SensitiveEntity)

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitiveTriangle::Select3D_SensitiveTriangle (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                        const gp_Pnt& thePnt0,
                                                        const gp_Pnt& thePnt1,
                                                        const gp_Pnt& thePnt2,
                                                        const Select3D_TypeOfSensitivity theType)
: Select3D_SensitiveEntity (theOwnerId),
  mySensType (theType)
{
  myPoints[0] = thePnt0;
  myPoints[1] = thePnt1;
  myPoints[2] = thePnt2;
  myCentroid = (thePnt0.XYZ() + thePnt1.XYZ() + thePnt2.XYZ()) * (1.0 / 3.0);
}

//==================================================
// Function: Matches
// Purpose : Checks whether the triangle overlaps
//           current selecting volume
//==================================================
Standard_Boolean Select3D_SensitiveTriangle::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                      SelectBasics_PickResult& thePickResult)
{
  Standard_Real aDepth     = RealLast();
  Standard_Real aDistToCOG = RealLast();
  if (!theMgr.IsOverlapAllowed())
  {
    return theMgr.Overlaps (myPoints[0])
        && theMgr.Overlaps (myPoints[1])
        && theMgr.Overlaps (myPoints[2]);
  }

  if (!theMgr.Overlaps (myPoints[0], myPoints[1], myPoints[2], mySensType, aDepth))
  {
    thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
    return Standard_False;
  }

  aDistToCOG = theMgr.DistToGeometryCenter (myCentroid);
  thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
  return Standard_True;
}

//==================================================
// Function: GetConnected
// Purpose :
//==================================================
Handle(Select3D_SensitiveEntity) Select3D_SensitiveTriangle::GetConnected()
{
  // Create a copy of this
  Handle(Select3D_SensitiveEntity) aNewEntity =
    new Select3D_SensitiveTriangle (myOwnerId, myPoints[0], myPoints[1], myPoints[2], mySensType);

  return aNewEntity;
}

//==================================================
// Function: BoundingBox
// Purpose : Returns bounding box of the triangle.
//           If location transformation is set, it
//           will be applied
//==================================================
Select3D_BndBox3d Select3D_SensitiveTriangle::BoundingBox()
{
  const SelectMgr_Vec3 aMinPnt = SelectMgr_Vec3 (Min (myPoints[0].X(), Min (myPoints[1].X(), myPoints[2].X())),
                                                 Min (myPoints[0].Y(), Min (myPoints[1].Y(), myPoints[2].Y())),
                                                 Min (myPoints[0].Z(), Min (myPoints[1].Z(), myPoints[2].Z())));
  const SelectMgr_Vec3 aMaxPnt = SelectMgr_Vec3 (Max (myPoints[0].X(), Max (myPoints[1].X(), myPoints[2].X())),
                                                 Max (myPoints[0].Y(), Max (myPoints[1].Y(), myPoints[2].Y())),
                                                 Max (myPoints[0].Z(), Max (myPoints[1].Z(), myPoints[2].Z())));
  return Select3D_BndBox3d (aMinPnt, aMaxPnt);
}
