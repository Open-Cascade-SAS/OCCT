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

#include <Bnd_Box2d.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <TColgp_Array1OfPnt2d.hxx>


IMPLEMENT_STANDARD_RTTIEXT(MeshVS_DummySensitiveEntity,SelectBasics_SensitiveEntity)

//================================================================
// Function : Constructor MeshVS_DummySensitiveEntity
// Purpose  :
//================================================================
MeshVS_DummySensitiveEntity::MeshVS_DummySensitiveEntity (const Handle(SelectBasics_EntityOwner)& theOwnerId)
: SelectBasics_SensitiveEntity (theOwnerId)
{}

//================================================================
// Function : NbSubElements
// Purpose  :
//================================================================
Standard_Integer MeshVS_DummySensitiveEntity::NbSubElements()
{
  return -1;
}

//================================================================
// Function : Matches
// Purpose  :
//================================================================
Standard_Boolean MeshVS_DummySensitiveEntity::Matches (SelectBasics_SelectingVolumeManager& /*theMgr*/,
                                                       SelectBasics_PickResult& /*thePickResult*/)
{
  return Standard_False;
}

//================================================================
// Function : BoundingBox
// Purpose  :
//================================================================
Select3D_BndBox3d MeshVS_DummySensitiveEntity::BoundingBox()
{
  return Select3D_BndBox3d();
}

//================================================================
// Function : ElementsNb
// Purpose  :
//================================================================
void MeshVS_DummySensitiveEntity::BVH()
{}

//================================================================
// Function : Clear
// Purpose  :
//================================================================
void MeshVS_DummySensitiveEntity::Clear()
{}

//=======================================================================
//function : HasInitLocation
//purpose  :
//=======================================================================
Standard_Boolean MeshVS_DummySensitiveEntity::HasInitLocation() const
{
  return Standard_False;
}

//=======================================================================
//function : InvInitLocation
//purpose  :
//=======================================================================
inline gp_GTrsf MeshVS_DummySensitiveEntity::InvInitLocation() const
{
  return gp_GTrsf();
}
