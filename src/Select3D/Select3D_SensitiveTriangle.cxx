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

#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Bnd_Box.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_HANDLE (Select3D_SensitiveTriangle, Select3D_SensitiveEntity)
IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveTriangle, Select3D_SensitiveEntity)

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitiveTriangle::Select3D_SensitiveTriangle (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                        const gp_Pnt& thePnt0,
                                                        const gp_Pnt& thePnt1,
                                                        const gp_Pnt& thePnt2,
                                                        const Select3D_TypeOfSensitivity theType)
: Select3D_SensitivePoly (theOwnerId, Standard_False, 3),
  mySensType (theType)
{
  myPolyg.SetPnt (0, thePnt0);
  myPolyg.SetPnt (1, thePnt1);
  myPolyg.SetPnt (2, thePnt2);
  myCentroid = (gp_XYZ (myPolyg.Pnt (0)) + gp_XYZ (myPolyg.Pnt (1)) + gp_XYZ (myPolyg.Pnt (2)))
                * (0.3);
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
  gp_Pnt aPnt1 = myPolyg.Pnt3d (0);
  gp_Pnt aPnt2 = myPolyg.Pnt3d (1);
  gp_Pnt aPnt3 = myPolyg.Pnt3d (2);
  Standard_Boolean isMatched = theMgr.Overlaps (aPnt1, aPnt2, aPnt3, mySensType, aDepth);
  if (isMatched)
  {
    aDistToCOG = theMgr.DistToGeometryCenter (myCentroid);
  }

  thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);

  return isMatched;
}

//==================================================
// Function: Points3D
// Purpose :
//==================================================
void Select3D_SensitiveTriangle::Points3D (gp_Pnt& thePnt0, gp_Pnt& thePnt1, gp_Pnt& thePnt2) const
{
  thePnt0 = myPolyg.Pnt (0);
  thePnt1 = myPolyg.Pnt (1);
  thePnt2 = myPolyg.Pnt (2);
}

//==================================================
// Function: Center3D
// Purpose :
//==================================================
gp_Pnt Select3D_SensitiveTriangle::Center3D() const
{
  gp_XYZ aPnt1, aPnt2, aPnt3;
  aPnt1 = myPolyg.Pnt (0);
  aPnt2 = myPolyg.Pnt (1);
  aPnt3 = myPolyg.Pnt (2);
  return gp_Pnt ((aPnt1 + aPnt2 + aPnt3) / 3.0);
}

//==================================================
// Function: GetConnected
// Purpose :
//==================================================
Handle(Select3D_SensitiveEntity) Select3D_SensitiveTriangle::GetConnected()
{
  // Create a copy of this
  Handle(Select3D_SensitiveEntity) aNewEntity =
    new Select3D_SensitiveTriangle (myOwnerId, myPolyg.Pnt(0), myPolyg.Pnt(1), myPolyg.Pnt(2), mySensType);

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
  gp_Pnt aPnt1 = myPolyg.Pnt3d (0);
  gp_Pnt aPnt2 = myPolyg.Pnt3d (1);
  gp_Pnt aPnt3 = myPolyg.Pnt3d (2);
  const SelectMgr_Vec3 aMinPnt = SelectMgr_Vec3 (Min (aPnt1.X(), Min (aPnt2.X(), aPnt3.X())),
                                                 Min (aPnt1.Y(), Min (aPnt2.Y(), aPnt3.Y())),
                                                 Min (aPnt1.Z(), Min (aPnt2.Z(), aPnt3.Z())));
  const SelectMgr_Vec3 aMaxPnt = SelectMgr_Vec3 (Max (aPnt1.X(), Max (aPnt2.X(), aPnt3.X())),
                                                 Max (aPnt1.Y(), Max (aPnt2.Y(), aPnt3.Y())),
                                                 Max (aPnt1.Z(), Max (aPnt2.Z(), aPnt3.Z())));
  return Select3D_BndBox3d (aMinPnt, aMaxPnt);
}

//==================================================
// Function: NbSubElements
// Purpose : Returns the amount of points
//==================================================
Standard_Integer Select3D_SensitiveTriangle::NbSubElements()
{
  return 3;
}
