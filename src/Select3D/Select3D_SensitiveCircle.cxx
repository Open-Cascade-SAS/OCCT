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

#include <gp_Ax3.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveCircle, Select3D_SensitiveEntity)

//=======================================================================
// function : Select3D_SensitiveCircle (constructor)
// purpose  : Definition of a sensitive circle
//=======================================================================
Select3D_SensitiveCircle::Select3D_SensitiveCircle(const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                   const gp_Circ&                       theCircle,
                                                   const Standard_Boolean               theIsFilled)
    : Select3D_SensitiveEntity(theOwnerId)
{
  myRadius = theCircle.Radius();
  myTrsf.SetTransformation(theCircle.Position(), gp::XOY());

  mySensType = theIsFilled ? Select3D_TOS_INTERIOR : Select3D_TOS_BOUNDARY;
  if (mySensType == Select3D_TOS_BOUNDARY)
  {
    SetSensitivityFactor(6);
  }
}

//=======================================================================
// function : Matches
// purpose  : Checks whether the circle overlaps current selecting volume
//=======================================================================
Standard_Boolean Select3D_SensitiveCircle::Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                   SelectBasics_PickResult& thePickResult)
{
  const Standard_Boolean aIsFilled = mySensType == Select3D_TOS_INTERIOR;

  if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
  {
    if (!theMgr.IsOverlapAllowed())
    {
      bool isInside = true;
      return theMgr.OverlapsCircle(myRadius, myTrsf, aIsFilled, &isInside) && isInside;
    }
    else
    {
      return theMgr.OverlapsCircle(myRadius, myTrsf, aIsFilled, NULL);
    }
  }
  if (!theMgr.OverlapsCircle(myRadius, myTrsf, aIsFilled, thePickResult))
  {
    return false;
  }

  thePickResult.SetDistToGeomCenter(theMgr.DistToGeometryCenter(CenterOfGeometry()));

  return Standard_True;
}

//=================================================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveCircle::GetConnected()
{
  Standard_Boolean                 anIsFilled = mySensType == Select3D_TOS_INTERIOR;
  Handle(Select3D_SensitiveEntity) aNewEntity =
    new Select3D_SensitiveCircle(myOwnerId, Circle(), anIsFilled);
  return aNewEntity;
}

//=================================================================================================

Select3D_BndBox3d Select3D_SensitiveCircle::BoundingBox()
{
  Graphic3d_Mat4d aTrsf;
  myTrsf.GetMat4(aTrsf);

  Select3D_BndBox3d aBox(SelectMgr_Vec3(-myRadius, -myRadius, 0),
                         SelectMgr_Vec3(myRadius, myRadius, 0));
  aBox.Transform(aTrsf);

  return aBox;
}

//=================================================================================================

gp_Pnt Select3D_SensitiveCircle::CenterOfGeometry() const
{
  return gp_Pnt(myTrsf.TranslationPart());
}
