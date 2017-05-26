// Created on: 1995-01-23
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

#include <SelectBasics_SensitiveEntity.hxx>

#include <SelectBasics_EntityOwner.hxx>
#include <TColStd_HArray1OfBoolean.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectBasics_SensitiveEntity, Standard_Transient)

//=======================================================================
// function : SelectBasics_SensitiveEntity
// purpose  : Creates new empty sensitive entity instance
//=======================================================================
SelectBasics_SensitiveEntity::SelectBasics_SensitiveEntity (const Handle(SelectBasics_EntityOwner)& theOwnerId)
: myOwnerId (theOwnerId),
  mySFactor (2) {}

//=======================================================================
// function : Set
// purpose  : Sets owner of the entity
//=======================================================================
void SelectBasics_SensitiveEntity::Set (const Handle(SelectBasics_EntityOwner)& theOwnerId)
{
  myOwnerId = theOwnerId;
}
