// Created on: 1995-02-16
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

#include <Standard_NullObject.hxx>

#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_Selection,Standard_Transient)

//==================================================
// Function: SelectMgr_Selection
// Purpose :
//==================================================
SelectMgr_Selection::SelectMgr_Selection (const Standard_Integer theModeIdx)
: myMode (theModeIdx),
  mySelectionState (SelectMgr_SOS_Unknown),
  myBVHUpdateStatus (SelectMgr_TBU_None),
  mySensFactor (2),
  myIsCustomSens (Standard_False)
{}

SelectMgr_Selection::~SelectMgr_Selection()
{
  Destroy();
}

//==================================================
// Function: Destroy
// Purpose :
//==================================================
void SelectMgr_Selection::Destroy()
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anEntityIter (myEntities); anEntityIter.More(); anEntityIter.Next())
  {
    Handle(SelectMgr_SensitiveEntity)& anEntity = anEntityIter.ChangeValue();
    anEntity->BaseSensitive()->Set (NULL);
  }
  mySensFactor = 2;
}

//==================================================
// Function: ADD
// Purpose :
//==================================================
void SelectMgr_Selection::Add (const Handle(SelectBasics_SensitiveEntity)& theSensitive)
{
  // if input is null:
  // in debug mode raise exception
  Standard_NullObject_Raise_if
    (theSensitive.IsNull(), "Null sensitive entity is added to the selection");

  // in release mode do not add
  if (!theSensitive.IsNull())
  {
    Handle(SelectMgr_SensitiveEntity) anEntity = new SelectMgr_SensitiveEntity (theSensitive);
    myEntities.Append (anEntity);
    if (mySelectionState == SelectMgr_SOS_Activated &&
        !anEntity->IsActiveForSelection())
    {
      anEntity->SetActiveForSelection();
    }

    if (myIsCustomSens)
    {
      anEntity->BaseSensitive()->SetSensitivityFactor (mySensFactor);
    }
    else
    {
      mySensFactor = Max (mySensFactor,
                          anEntity->BaseSensitive()->SensitivityFactor());
    }
  }
}	

//==================================================
// Function: Clear
// Purpose :
//==================================================
void SelectMgr_Selection::Clear()
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anEntityIter (myEntities); anEntityIter.More(); anEntityIter.Next())
  {
    Handle(SelectMgr_SensitiveEntity)& anEntity = anEntityIter.ChangeValue();
    anEntity->Clear();
  }

  myEntities.Clear();
}

//==================================================
// Function: IsEmpty 
// Purpose :
//==================================================
Standard_Boolean SelectMgr_Selection::IsEmpty() const
{
  return myEntities.IsEmpty();
}

//==================================================
// function: GetEntityById
// purpose : Returns sensitive entity stored by
//           index theIdx in entites vector
//==================================================
Handle(SelectMgr_SensitiveEntity)& SelectMgr_Selection::GetEntityById (const Standard_Integer theIdx)
{
  return myEntities.ChangeValue (theIdx);
}

//==================================================
// function: GetSelectionState
// purpose : Returns status of selection
//==================================================
SelectMgr_StateOfSelection SelectMgr_Selection::GetSelectionState() const
{
  return mySelectionState;
}

//==================================================
// function: SetSelectionState
// purpose : Sets status of selection
//==================================================
void SelectMgr_Selection::SetSelectionState (const SelectMgr_StateOfSelection theState) const
{
  mySelectionState = theState;
}

//==================================================
// function: Sensitivity
// purpose : Returns sensitivity of the selection
//==================================================
Standard_Integer SelectMgr_Selection::Sensitivity() const
{
  return mySensFactor;
}

//==================================================
// function: SetSensitivity
// purpose : Changes sensitivity of the selection and all its entities to the given value.
//           IMPORTANT: This method does not update any outer selection structures, so for
//           proper updates use SelectMgr_SelectionManager::SetSelectionSensitivity method.
//==================================================
void SelectMgr_Selection::SetSensitivity (const Standard_Integer theNewSens)
{
  mySensFactor = theNewSens;
  myIsCustomSens = Standard_True;
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anEntityIter (myEntities); anEntityIter.More(); anEntityIter.Next())
  {
    Handle(SelectMgr_SensitiveEntity)& anEntity = anEntityIter.ChangeValue();
    anEntity->BaseSensitive()->SetSensitivityFactor (theNewSens);
  }
}
