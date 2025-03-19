// Created on: 1995-03-10
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Select3D_SensitivePoint.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitivePoint, Select3D_SensitiveEntity)

//=================================================================================================

Select3D_SensitivePoint::Select3D_SensitivePoint(const Handle(SelectMgr_EntityOwner)& theOwner,
                                                 const gp_Pnt&                        thePoint)
    : Select3D_SensitiveEntity(theOwner)
{
  SetSensitivityFactor(12);
  myPoint = thePoint;
}

//=================================================================================================

Standard_Boolean Select3D_SensitivePoint::Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                  SelectBasics_PickResult& thePickResult)
{
  if (!theMgr.OverlapsPoint(myPoint, thePickResult))
  {
    return Standard_False;
  }

  thePickResult.SetDistToGeomCenter(thePickResult.Depth());
  return Standard_True;
}

//=================================================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitivePoint::GetConnected()
{
  Handle(Select3D_SensitivePoint) aNewEntity = new Select3D_SensitivePoint(myOwnerId, myPoint);
  return aNewEntity;
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of point. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_SensitivePoint::CenterOfGeometry() const
{
  return myPoint;
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns bounding box of the point. If location
//            transformation is set, it will be applied
//=======================================================================
Select3D_BndBox3d Select3D_SensitivePoint::BoundingBox()
{
  return Select3D_BndBox3d(SelectMgr_Vec3(myPoint.X(), myPoint.Y(), myPoint.Z()),
                           SelectMgr_Vec3(myPoint.X(), myPoint.Y(), myPoint.Z()));
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of sub-entities in sensitive
//=======================================================================
Standard_Integer Select3D_SensitivePoint::NbSubElements() const
{
  return 1;
}

//=================================================================================================

void Select3D_SensitivePoint::DumpJson(Standard_OStream& theOStream,
                                       Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Select3D_SensitiveEntity)
}
