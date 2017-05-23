// Created on: 1995-03-13
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

#include <Select3D_SensitiveEntity.hxx>

#include <Precision.hxx>
#include <SelectBasics_EntityOwner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveEntity,SelectBasics_SensitiveEntity)

//=======================================================================
//function : Select3D_SensitiveEntity
//purpose  :
//=======================================================================
Select3D_SensitiveEntity::Select3D_SensitiveEntity (const Handle(SelectBasics_EntityOwner)& theOwnerId)
: SelectBasics_SensitiveEntity (theOwnerId) {}

//=======================================================================
// function : Matches
// purpose  : Checks whether sensitive overlaps current selecting volume.
//            Stores minimum depth, distance to center of geometry and
//            closest point detected into thePickResult
//=======================================================================
Standard_Boolean Select3D_SensitiveEntity::Matches (SelectBasics_SelectingVolumeManager& /*theMgr*/,
                                                    SelectBasics_PickResult& /*thePickResult*/)
{
  return Standard_False;
}

//=======================================================================
//function : GetConnected
//purpose  : 
//=======================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitiveEntity::GetConnected()
{
  Handle(Select3D_SensitiveEntity) aNewEntity;
  return aNewEntity;
}

//=======================================================================
// function : BVH
// purpose  : Builds BVH tree for a sensitive if needed
//=======================================================================
void Select3D_SensitiveEntity::BVH()
{
  return;
}

//=======================================================================
// function : Clear
// purpose  : Cleans up resources and memory
//=======================================================================
void Select3D_SensitiveEntity::Clear()
{
  Set (NULL);
}

//=======================================================================
// function : HasInitLocation
// purpose  : Returns true if the shape corresponding to the entity has init location
//=======================================================================
Standard_Boolean Select3D_SensitiveEntity::HasInitLocation() const
{
  return Standard_False;
}

//=======================================================================
// function : InvInitLocation
// purpose  : Returns inversed location transformation matrix if the shape corresponding
//            to this entity has init location set. Otherwise, returns identity matrix.
//=======================================================================
gp_GTrsf Select3D_SensitiveEntity::InvInitLocation() const
{
  return gp_GTrsf();
}
