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

#include <SelectMgr_Selection.hxx>

#include <SelectMgr_EntityOwner.hxx>
#include <Standard_NullObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_Selection, Standard_Transient)

//=================================================================================================

SelectMgr_Selection::SelectMgr_Selection(const Standard_Integer theModeIdx)
    : myMode(theModeIdx),
      mySelectionState(SelectMgr_SOS_Unknown),
      myBVHUpdateStatus(SelectMgr_TBU_None),
      mySensFactor(2),
      myIsCustomSens(Standard_False)
{
}

SelectMgr_Selection::~SelectMgr_Selection()
{
  Destroy();
}

//=================================================================================================

void SelectMgr_Selection::Destroy()
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anEntityIter(myEntities);
       anEntityIter.More();
       anEntityIter.Next())
  {
    Handle(SelectMgr_SensitiveEntity)& anEntity = anEntityIter.ChangeValue();
    anEntity->BaseSensitive()->Set(NULL);
  }
  mySensFactor = 2;
}

//=================================================================================================

void SelectMgr_Selection::Add(const Handle(Select3D_SensitiveEntity)& theSensitive)
{
  // if input is null: in debug mode raise exception
  Standard_NullObject_Raise_if(theSensitive.IsNull(),
                               "Null sensitive entity is added to the selection");
  if (theSensitive.IsNull())
  {
    // in release mode do not add
    return;
  }

  Handle(SelectMgr_SensitiveEntity) anEntity = new SelectMgr_SensitiveEntity(theSensitive);
  myEntities.Append(anEntity);
  if (mySelectionState == SelectMgr_SOS_Activated && !anEntity->IsActiveForSelection())
  {
    anEntity->SetActiveForSelection();
  }

  if (myIsCustomSens)
  {
    anEntity->BaseSensitive()->SetSensitivityFactor(mySensFactor);
  }
  else
  {
    mySensFactor = Max(mySensFactor, anEntity->BaseSensitive()->SensitivityFactor());
  }
}

//=================================================================================================

void SelectMgr_Selection::Clear()
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anEntityIter(myEntities);
       anEntityIter.More();
       anEntityIter.Next())
  {
    Handle(SelectMgr_SensitiveEntity)& anEntity = anEntityIter.ChangeValue();
    anEntity->Clear();
  }

  myEntities.Clear();
}

//==================================================
// function: SetSensitivity
// purpose : Changes sensitivity of the selection and all its entities to the given value.
//           IMPORTANT: This method does not update any outer selection structures, so for
//           proper updates use SelectMgr_SelectionManager::SetSelectionSensitivity method.
//==================================================
void SelectMgr_Selection::SetSensitivity(const Standard_Integer theNewSens)
{
  mySensFactor   = theNewSens;
  myIsCustomSens = Standard_True;
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anEntityIter(myEntities);
       anEntityIter.More();
       anEntityIter.Next())
  {
    Handle(SelectMgr_SensitiveEntity)& anEntity = anEntityIter.ChangeValue();
    anEntity->BaseSensitive()->SetSensitivityFactor(theNewSens);
  }
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void SelectMgr_Selection::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  NCollection_Map<Handle(SelectMgr_EntityOwner)> anOwners;
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anIterator(myEntities);
       anIterator.More();
       anIterator.Next())
  {
    const Handle(SelectMgr_SensitiveEntity)& anEntity = anIterator.Value();
    if (anEntity.IsNull() || anEntity->BaseSensitive().IsNull())
    {
      continue;
    }
    const Handle(SelectMgr_EntityOwner)& anOwner = anEntity->BaseSensitive()->OwnerId();
    if (anOwners.Add(anOwner))
    {
      OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anOwner.get())
    }
  }

  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anIterator(myEntities);
       anIterator.More();
       anIterator.Next())
  {
    const Handle(SelectMgr_SensitiveEntity)& anEntity = anIterator.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anEntity.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mySelectionState)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mySensFactor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUpdateStatus)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myBVHUpdateStatus)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsCustomSens)
}
