// Created on: 2003-09-29
// Created by: Alexander SOLOVYOV and Sergey LITONIN
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#include <MeshVS_DummySensitiveEntity.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_DummySensitiveEntity, Select3D_SensitiveEntity)

//=================================================================================================

MeshVS_DummySensitiveEntity::MeshVS_DummySensitiveEntity(
  const Handle(SelectMgr_EntityOwner)& theOwnerId)
    : Select3D_SensitiveEntity(theOwnerId)
{
}

//=================================================================================================

Standard_Integer MeshVS_DummySensitiveEntity::NbSubElements() const
{
  return -1;
}

//=================================================================================================

Standard_Boolean MeshVS_DummySensitiveEntity::Matches(
  SelectBasics_SelectingVolumeManager& /*theMgr*/,
  SelectBasics_PickResult& /*thePickResult*/)
{
  return Standard_False;
}

//=================================================================================================

Select3D_BndBox3d MeshVS_DummySensitiveEntity::BoundingBox()
{
  return Select3D_BndBox3d();
}

//=================================================================================================

void MeshVS_DummySensitiveEntity::BVH() {}

//=================================================================================================

void MeshVS_DummySensitiveEntity::Clear() {}

//=================================================================================================

Standard_Boolean MeshVS_DummySensitiveEntity::HasInitLocation() const
{
  return Standard_False;
}

//=================================================================================================

gp_GTrsf MeshVS_DummySensitiveEntity::InvInitLocation() const
{
  return gp_GTrsf();
}
