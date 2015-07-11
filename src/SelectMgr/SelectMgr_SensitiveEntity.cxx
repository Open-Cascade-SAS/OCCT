// Created on: 2014-08-15
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <SelectMgr_SensitiveEntity.hxx>


//=======================================================================
// function : SelectMgr_SensitiveEntity
// purpose  : Creates new inactive for selection object with base entity
//            theEntity
//=======================================================================
SelectMgr_SensitiveEntity::SelectMgr_SensitiveEntity (const Handle(SelectBasics_SensitiveEntity)& theEntity)
{
  mySensitive = theEntity;
  myIsActiveForSelection = Standard_False;
}

//=======================================================================
// function : Clear
// purpose  : Clears up all the resources and memory
//=======================================================================
void SelectMgr_SensitiveEntity::Clear()
{
  mySensitive->Clear();
  mySensitive.Nullify();
}

//=======================================================================
// function : BaseSensitive
// purpose  : Returns related instance of SelectBasics class
//=======================================================================
const Handle(SelectBasics_SensitiveEntity)& SelectMgr_SensitiveEntity::BaseSensitive() const
{
  return mySensitive;
}

//=======================================================================
// function : IsActiveForSelection
// purpose  : Returns true if this entity belongs to the active selection
//            mode of parent object
//=======================================================================
Standard_Boolean SelectMgr_SensitiveEntity::IsActiveForSelection() const
{
  return myIsActiveForSelection;
}

//=======================================================================
// function : ResetSelectionActiveStatus
// purpose  : Marks entity as inactive for selection
//=======================================================================
void SelectMgr_SensitiveEntity::ResetSelectionActiveStatus() const
{
  myIsActiveForSelection = Standard_False;
}

//=======================================================================
// function : SetActiveForSelection
// purpose  : Marks entity as active for selection
//=======================================================================
void SelectMgr_SensitiveEntity::SetActiveForSelection() const
{
  myIsActiveForSelection = Standard_True;
}
