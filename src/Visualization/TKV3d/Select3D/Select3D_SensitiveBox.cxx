// Created on: 1995-04-13
// Created by: Robert COUBLANC
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

#include <Select3D_SensitiveBox.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveBox, Select3D_SensitiveEntity)

//=================================================================================================

Select3D_SensitiveBox::Select3D_SensitiveBox(const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                             const Bnd_Box&                       theBox)
    : Select3D_SensitiveEntity(theOwnerId)
{
  Standard_Real aXMax, aYMax, aZMax;
  Standard_Real aXMin, aYMin, aZMin;
  theBox.Get(aXMin, aYMin, aZMin, aXMax, aYMax, aZMax);
  myBox =
    Select3D_BndBox3d(SelectMgr_Vec3(aXMin, aYMin, aZMin), SelectMgr_Vec3(aXMax, aYMax, aZMax));
  myCenter3d = (gp_XYZ(aXMin, aYMin, aZMin) + gp_XYZ(aXMax, aYMax, aZMax)) * (1.0 / 2.0);
}

//=================================================================================================

Select3D_SensitiveBox::Select3D_SensitiveBox(const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                             const Standard_Real                  theXMin,
                                             const Standard_Real                  theYMin,
                                             const Standard_Real                  theZMin,
                                             const Standard_Real                  theXMax,
                                             const Standard_Real                  theYMax,
                                             const Standard_Real                  theZMax)
    : Select3D_SensitiveEntity(theOwnerId)
{
  myBox = Select3D_BndBox3d(SelectMgr_Vec3(theXMin, theYMin, theZMin),
                            SelectMgr_Vec3(theXMax, theYMax, theZMax));
  myCenter3d =
    (gp_XYZ(theXMin, theYMin, theZMin) + gp_XYZ(theXMax, theYMax, theZMax)) * (1.0 / 2.0);
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of sub-entities in sensitive
//=======================================================================
Standard_Integer Select3D_SensitiveBox::NbSubElements() const
{
  return 1;
}

//=================================================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveBox::GetConnected()
{
  Bnd_Box aBox;
  aBox.Update(myBox.CornerMin().x(),
              myBox.CornerMin().y(),
              myBox.CornerMin().z(),
              myBox.CornerMax().x(),
              myBox.CornerMax().y(),
              myBox.CornerMax().z());
  Handle(Select3D_SensitiveBox) aNewEntity = new Select3D_SensitiveBox(myOwnerId, aBox);

  return aNewEntity;
}

//=======================================================================
// function : Matches
// purpose  : Checks whether the box overlaps current selecting volume
//=======================================================================
Standard_Boolean Select3D_SensitiveBox::Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                SelectBasics_PickResult&             thePickResult)
{
  if (!theMgr.IsOverlapAllowed()) // check for inclusion
  {
    Standard_Boolean isInside = Standard_True;
    return theMgr.OverlapsBox(myBox.CornerMin(), myBox.CornerMax(), &isInside) && isInside;
  }

  if (!theMgr.OverlapsBox(myBox.CornerMin(), myBox.CornerMax(), thePickResult)) // check for overlap
  {
    return Standard_False;
  }

  thePickResult.SetDistToGeomCenter(theMgr.DistToGeometryCenter(myCenter3d));
  return Standard_True;
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of the box. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_SensitiveBox::CenterOfGeometry() const
{
  return myCenter3d;
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns coordinates of the box. If location transformation
//            is set, it will be applied
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveBox::BoundingBox()
{
  return myBox;
}

//=================================================================================================

void Select3D_SensitiveBox::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Select3D_SensitiveEntity)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBox)
}
