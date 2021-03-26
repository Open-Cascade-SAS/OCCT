// Created on: 2021-03-04
// Created by: Maria KRYLOVA
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

#include <Select3D_SensitiveSphere.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveSphere, Select3D_SensitiveEntity)

// ==================================================
// Function: Select3D_SensitiveSphere
// Purpose :
// ==================================================
Select3D_SensitiveSphere::Select3D_SensitiveSphere (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                    const gp_Pnt& theCenter,
                                                    const Standard_Real theRadius)
: Select3D_SensitiveEntity (theOwnerId),
  myCenter (theCenter),
  myLastDetectedPoint (RealLast(), RealLast(), RealLast()),
  myRadius (theRadius)
{
}

// ==================================================
// Function: Mathes
// Purpose :
// ==================================================
Standard_Boolean Select3D_SensitiveSphere::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult)
{
  myLastDetectedPoint = gp_Pnt (RealLast(), RealLast(), RealLast());
  if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
  {
    if (!theMgr.IsOverlapAllowed())
    {
      Standard_Boolean isInside = Standard_True;
      return theMgr.OverlapsSphere (myCenter, myRadius, &isInside) && isInside;
    }
    else
    {
      return theMgr.OverlapsSphere (myCenter, myRadius, NULL);
    }
  }
  if (!theMgr.OverlapsSphere (myCenter, myRadius, thePickResult))
  {
    return Standard_False;
  }
  myLastDetectedPoint = thePickResult.PickedPoint();
  thePickResult.SetDistToGeomCenter (theMgr.DistToGeometryCenter (myCenter));
  return Standard_True;
}

// ==================================================
// Function: GetConnected
// Purpose :
// ==================================================
Handle(Select3D_SensitiveEntity) Select3D_SensitiveSphere::GetConnected()
{
  Handle(Select3D_SensitiveEntity) aNewEntity = new Select3D_SensitiveSphere (myOwnerId, myCenter, myRadius);
  return aNewEntity;
}

// ==================================================
// Function: BoundingBox
// Purpose :
// ==================================================
Select3D_BndBox3d Select3D_SensitiveSphere::BoundingBox()
{
  const SelectMgr_Vec3 aMinPnt = SelectMgr_Vec3 (myCenter.X() - myRadius, myCenter.Y() - myRadius, myCenter.Z() - myRadius);
  const SelectMgr_Vec3 aMaxPnt = SelectMgr_Vec3 (myCenter.X() + myRadius, myCenter.Y() + myRadius, myCenter.Z() + myRadius);
  return Select3D_BndBox3d (aMinPnt, aMaxPnt);
}
