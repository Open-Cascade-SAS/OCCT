// Created on: 2021-04-19
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

#include <Select3D_SensitiveCylinder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveCylinder, Select3D_SensitiveEntity)

//=================================================================================================

Select3D_SensitiveCylinder::Select3D_SensitiveCylinder(
  const Handle(SelectMgr_EntityOwner)& theOwnerId,
  const Standard_Real                  theBottomRad,
  const Standard_Real                  theTopRad,
  const Standard_Real                  theHeight,
  const gp_Trsf&                       theTrsf,
  const Standard_Boolean               theIsHollow)
    : Select3D_SensitiveEntity(theOwnerId),
      myTrsf(theTrsf),
      myBottomRadius(theBottomRad),
      myTopRadius(theTopRad),
      myHeight(theHeight),
      myIsHollow(theIsHollow)
{
}

//=================================================================================================

Standard_Boolean Select3D_SensitiveCylinder::Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                     SelectBasics_PickResult& thePickResult)
{
  if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
  {
    if (!theMgr.IsOverlapAllowed())
    {
      bool isInside = true;
      return theMgr.OverlapsCylinder(myBottomRadius,
                                     myTopRadius,
                                     myHeight,
                                     myTrsf,
                                     myIsHollow,
                                     &isInside)
             && isInside;
    }
    else
    {
      return theMgr
        .OverlapsCylinder(myBottomRadius, myTopRadius, myHeight, myTrsf, myIsHollow, NULL);
    }
  }
  if (!theMgr.OverlapsCylinder(myBottomRadius,
                               myTopRadius,
                               myHeight,
                               myTrsf,
                               myIsHollow,
                               thePickResult))
  {
    return false;
  }

  thePickResult.SetDistToGeomCenter(theMgr.DistToGeometryCenter(CenterOfGeometry()));
  return true;
}

//=================================================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveCylinder::GetConnected()
{
  Handle(Select3D_SensitiveEntity) aNewEntity =
    new Select3D_SensitiveCylinder(myOwnerId, myBottomRadius, myTopRadius, myHeight, myTrsf);
  return aNewEntity;
}

//=================================================================================================

Select3D_BndBox3d Select3D_SensitiveCylinder::BoundingBox()
{
  Standard_Real   aMaxRad = Max(myBottomRadius, myTopRadius);
  Graphic3d_Mat4d aTrsf;
  myTrsf.GetMat4(aTrsf);

  Select3D_BndBox3d aBox(SelectMgr_Vec3(-aMaxRad, -aMaxRad, 0),
                         SelectMgr_Vec3(aMaxRad, aMaxRad, myHeight));
  aBox.Transform(aTrsf);

  return aBox;
}

//=================================================================================================

gp_Pnt Select3D_SensitiveCylinder::CenterOfGeometry() const
{
  return gp_Pnt(0, 0, myHeight / 2).Transformed(myTrsf);
}
