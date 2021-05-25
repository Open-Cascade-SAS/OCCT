// Created on: 1995-02-13
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

#include <SelectMgr_SelectionManager.hxx>

#include <Select3D_SensitiveGroup.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_Selection.hxx>
#include <StdSelect_BRepSelectionTool.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_SelectionManager,Standard_Transient)

//==================================================
// Function: Create
// Purpose :
//==================================================
SelectMgr_SelectionManager::SelectMgr_SelectionManager (const Handle(SelectMgr_ViewerSelector)& theSelector)
: mySelector (theSelector)
{
  //
}

//==================================================
// Function: Contains
// Purpose :
//==================================================
Standard_Boolean SelectMgr_SelectionManager::Contains (const Handle(SelectMgr_SelectableObject)& theObject) const
{
  return myGlobal.Contains (theObject);
}

//==================================================
// Function: Load
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Load (const Handle(SelectMgr_SelectableObject)& theObject,
                                       const Standard_Integer theMode)
{
  if (myGlobal.Contains(theObject))
    return;

  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObject->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    Load (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theMode);
  }

  if (!theObject->HasOwnPresentations())
    return;

  myGlobal.Add(theObject);
  if (!mySelector->Contains (theObject) && theObject->HasOwnPresentations())
  {
    mySelector->AddSelectableObject (theObject);
  }
  if (theMode != -1)
    loadMode (theObject, theMode);
}

//==================================================
// Function: Remove
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Remove (const Handle(SelectMgr_SelectableObject)& theObject)
{
  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObject->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    Remove (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()));
  }

  if (!theObject->HasOwnPresentations())
    return;

  if (myGlobal.Contains (theObject))
  {
    if (mySelector->Contains (theObject))
    {
      for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
      {
        mySelector->RemoveSelectionOfObject (theObject, aSelIter.Value());
        aSelIter.Value()->UpdateBVHStatus (SelectMgr_TBU_Remove);
        mySelector->Deactivate (aSelIter.Value());
      }
      mySelector->RemoveSelectableObject (theObject);
    }
    myGlobal.Remove (theObject);
  }

  theObject->ClearSelections();
}

//==================================================
// Function: Activate
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Activate (const Handle(SelectMgr_SelectableObject)& theObject,
                                           const Standard_Integer theMode)
{
  if (theMode == -1)
    return;

  for (PrsMgr_ListOfPresentableObjectsIter anChildIter (theObject->Children()); anChildIter.More(); anChildIter.Next())
  {
    Handle(SelectMgr_SelectableObject) aChild = Handle(SelectMgr_SelectableObject)::DownCast (anChildIter.Value());
    if (aChild->DisplayStatus() != PrsMgr_DisplayStatus_Erased)
    {
      Activate (aChild, theMode);
    }
  }
  if (!theObject->HasOwnPresentations())
    return;

  Standard_Boolean isComputed = Standard_False;
  if (const Handle(SelectMgr_Selection)& aSelOld = theObject->Selection (theMode))
  {
    isComputed = !aSelOld->IsEmpty();
  }
  if (!isComputed)
  {
    loadMode (theObject, theMode);
  }

  const Handle(SelectMgr_Selection)& aSelection = theObject->Selection (theMode);
  switch (aSelection->UpdateStatus())
  {
    case SelectMgr_TOU_Full:
    {
      if (theObject->HasSelection (theMode))
      {
        mySelector->RemoveSelectionOfObject (theObject, aSelection);
      }
      theObject->RecomputePrimitives (theMode);
      // pass through SelectMgr_TOU_Partial
    }
    Standard_FALLTHROUGH
    case SelectMgr_TOU_Partial:
    {
      theObject->UpdateTransformations (aSelection);
      mySelector->RebuildObjectsTree();
      break;
    }
    default:
      break;
  }
  aSelection->UpdateStatus(SelectMgr_TOU_None);

  switch (aSelection->BVHUpdateStatus())
  {
    case SelectMgr_TBU_Add:
    case SelectMgr_TBU_Renew:
    {
      mySelector->AddSelectionToObject (theObject, aSelection);
      break;
    }
    case SelectMgr_TBU_Remove:
    {
      if (aSelection->GetSelectionState() == SelectMgr_SOS_Deactivated)
      {
        mySelector->AddSelectionToObject (theObject, aSelection);
      }
      break;
    }
    default:
      break;
  }
  aSelection->UpdateBVHStatus (SelectMgr_TBU_None);

  if (myGlobal.Contains (theObject))
  {
    mySelector->Activate (theObject->Selection (theMode));
  }
}

//==================================================
// Function: Deactivate
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Deactivate (const Handle(SelectMgr_SelectableObject)& theObject,
                                             const Standard_Integer theMode)
{
  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObject->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    Deactivate (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theMode);
  }
  if (!theObject->HasOwnPresentations())
  {
    return;
  }
  if (!myGlobal.Contains(theObject))
  {
    return;
  }

  const Handle(SelectMgr_Selection)& aSel = theObject->Selection (theMode);
  if (theMode == -1)
  {
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
    {
      mySelector->Deactivate (aSelIter.Value());
    }
  }
  else if (!aSel.IsNull())
  {
    mySelector->Deactivate (aSel);
  }
}

//=======================================================================
//function : IsActivated
//purpose  :
//=======================================================================
Standard_Boolean SelectMgr_SelectionManager::IsActivated (const Handle(SelectMgr_SelectableObject)& theObject,
                                                          const Standard_Integer theMode) const
{
  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObject->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    if (IsActivated (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theMode))
      return Standard_True;
  }
  if (!theObject->HasOwnPresentations())
  {
    return Standard_False;
  }
  if (!myGlobal.Contains(theObject))
  {
    return Standard_False;
  }

  if (theMode == -1)
  {
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
    {
      if (mySelector->Status (aSelIter.Value()) == SelectMgr_SOS_Activated)
      {
        return Standard_True;
      }
    }
    return Standard_False;
  }

  const Handle(SelectMgr_Selection)& aSelection = theObject->Selection (theMode);
  if (aSelection.IsNull())
  {
    return Standard_False;
  }
  return !aSelection.IsNull()
       && mySelector->Status (aSelection) == SelectMgr_SOS_Activated;
}

//=======================================================================
//function : ClearSelectionStructures
//purpose  : Removes sensitive entities from all viewer selectors
//           after method Clear() was called to the selection they belonged to
//           or it was recomputed somehow
//=======================================================================
void SelectMgr_SelectionManager::ClearSelectionStructures (const Handle(SelectMgr_SelectableObject)& theObj,
                                                           const Standard_Integer theMode)
{
  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObj->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    ClearSelectionStructures (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theMode);
  }

  if (!theObj->HasOwnPresentations())
  {
    return;
  }
  if (!myGlobal.Contains(theObj))
  {
    return;
  }

  if (theMode != -1)
  {
    if (const Handle(SelectMgr_Selection)& aSelection = theObj->Selection (theMode))
    {
      mySelector->RemoveSelectionOfObject (theObj, aSelection);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_Add);
    }
  }
  else
  {
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObj->Selections()); aSelIter.More(); aSelIter.Next())
    {
      const Handle(SelectMgr_Selection)& aSelection = aSelIter.Value();
      mySelector->RemoveSelectionOfObject (theObj, aSelection);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_Add);
    }
  }
  mySelector->RebuildObjectsTree();
}

//=======================================================================
//function : RestoreSelectionStructuress
//purpose  : Re-adds newely calculated sensitive  entities of recomputed selection
//           defined by mode theMode to all viewer selectors contained that selection.
//=======================================================================
void SelectMgr_SelectionManager::RestoreSelectionStructures (const Handle(SelectMgr_SelectableObject)& theObj,
                                                             const Standard_Integer theMode)
{
  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObj->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    RestoreSelectionStructures (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theMode);
  }
  if (!theObj->HasOwnPresentations())
  {
    return;
  }
  if (!myGlobal.Contains(theObj))
  {
    return;
  }

  if (theMode != -1)
  {
    if (const Handle(SelectMgr_Selection)& aSelection = theObj->Selection (theMode))
    {
      mySelector->AddSelectionToObject (theObj, aSelection);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
    }
  }
  else
  {
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObj->Selections()); aSelIter.More(); aSelIter.Next())
    {
      const Handle(SelectMgr_Selection)& aSelection = aSelIter.Value();
      mySelector->AddSelectionToObject (theObj, aSelection);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
    }
  }
  mySelector->RebuildObjectsTree();
}

//==================================================
// Function: recomputeSelectionMode
// Purpose :
//==================================================
void SelectMgr_SelectionManager::recomputeSelectionMode (const Handle(SelectMgr_SelectableObject)& theObject,
                                                         const Handle(SelectMgr_Selection)& theSelection,
                                                         const Standard_Integer theMode)
{
  theSelection->UpdateStatus (SelectMgr_TOU_Full);

  ClearSelectionStructures (theObject, theMode);
  theObject->RecomputePrimitives (theMode);
  RestoreSelectionStructures (theObject, theMode);
  theSelection->UpdateStatus (SelectMgr_TOU_None);
  theSelection->UpdateBVHStatus (SelectMgr_TBU_None);
}

//==================================================
// Function: Update
// Purpose :
//==================================================
void SelectMgr_SelectionManager::RecomputeSelection (const Handle(SelectMgr_SelectableObject)& theObject,
                                                     const Standard_Boolean theIsForce,
                                                     const Standard_Integer theMode)
{
  if (theIsForce)
  {
    if (theMode == -1)
    {
      ClearSelectionStructures (theObject);
      theObject->RecomputePrimitives();
      theObject->UpdateTransformation();
      RestoreSelectionStructures (theObject);
    }
    else if (theObject->HasSelection (theMode))
    {
      ClearSelectionStructures (theObject, theMode);
      theObject->RecomputePrimitives (theMode);
      theObject->UpdateTransformation();
      RestoreSelectionStructures (theObject, theMode);
    }
    return;
  }

  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObject->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    RecomputeSelection (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theIsForce, theMode);
  }
  if (!theObject->HasOwnPresentations())
  {
    return;
  }
  if (!myGlobal.Contains (theObject))
  {
    return;
  }

  if (theMode == -1)
  {
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
    {
      const Handle(SelectMgr_Selection)& aSelection = aSelIter.Value();
      const Standard_Integer aSelMode = aSelection->Mode();
      recomputeSelectionMode (theObject, aSelection, aSelMode);
    }
  }
  else
  {
    if (const Handle(SelectMgr_Selection)& aSelection = theObject->Selection (theMode))
    {
      recomputeSelectionMode (theObject, aSelection, theMode);
    }
  }
}

//=======================================================================
//function : Update
//purpose  : Selections are recalculated if they are flagged
//           "TO RECALCULATE" and activated in one of selectors.
//           If ForceUpdate = True, and they are "TO RECALCULATE"
//           This is done without caring for the state of activation.
//=======================================================================
void SelectMgr_SelectionManager::Update (const Handle(SelectMgr_SelectableObject)& theObject,
                                         const Standard_Boolean theIsForce)
{
  for (PrsMgr_ListOfPresentableObjectsIter aChildIter (theObject->Children()); aChildIter.More(); aChildIter.Next())
  {
    Update (Handle(SelectMgr_SelectableObject)::DownCast (aChildIter.Value()), theIsForce);
  }
  if (!theObject->HasOwnPresentations())
  {
    return;
  }

  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSelection = aSelIter.Value();
    if (theIsForce || mySelector->Status (aSelection) == SelectMgr_SOS_Activated)
    {
      switch (aSelection->UpdateStatus())
      {
        case SelectMgr_TOU_Full:
        {
          ClearSelectionStructures (theObject, aSelection->Mode());
          theObject->RecomputePrimitives (aSelection->Mode()); // no break on purpose...
          RestoreSelectionStructures (theObject, aSelection->Mode());
          // pass through SelectMgr_TOU_Partial
        }
        Standard_FALLTHROUGH
        case SelectMgr_TOU_Partial:
        {
          theObject->UpdateTransformations (aSelection);
          mySelector->RebuildObjectsTree();
          break;
        }
        default:
          break;
      }
      aSelection->UpdateStatus (SelectMgr_TOU_None);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
    }
  }
}

//==================================================
// Function: loadMode
// Purpose : Private Method
//==================================================
void SelectMgr_SelectionManager::loadMode (const Handle(SelectMgr_SelectableObject)& theObject,
                                           const Standard_Integer theMode)
{
  if (theMode == -1)
  {
    return;
  }

  if (const Handle(SelectMgr_Selection)& aSelOld = theObject->Selection (theMode))
  {
    if (aSelOld->IsEmpty())
    {
      if (aSelOld->BVHUpdateStatus() == SelectMgr_TBU_Remove)
      {
        Handle(SelectMgr_Selection) aNewSel = new SelectMgr_Selection (theMode);
        theObject->AddSelection (aNewSel, theMode);
        aNewSel->UpdateBVHStatus (SelectMgr_TBU_Remove);
        aNewSel->SetSelectionState (SelectMgr_SOS_Deactivated);

        buildBVH (aNewSel);
      }
    }
    return;
  }

  Handle(SelectMgr_Selection) aNewSel = new SelectMgr_Selection (theMode);
  theObject->AddSelection (aNewSel, theMode);
  if (myGlobal.Contains (theObject))
  {
    mySelector->AddSelectionToObject (theObject, aNewSel);
    aNewSel->UpdateBVHStatus (SelectMgr_TBU_None);
  }

  buildBVH (aNewSel);
}

//==================================================
// Function: buildBVH
// Purpose : Private Method
//==================================================
void SelectMgr_SelectionManager::buildBVH (const Handle(SelectMgr_Selection)& theSelection)
{
  if (mySelector->ToPrebuildBVH())
  {
    for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator anIter (theSelection->Entities()); anIter.More(); anIter.Next())
    {
      const Handle(Select3D_SensitiveEntity)& anEntity = anIter.Value()->BaseSensitive();
      mySelector->QueueBVHBuild (anEntity);

      if (Handle(Select3D_SensitiveGroup) aGroup = Handle(Select3D_SensitiveGroup)::DownCast (anEntity))
      {
        for (Select3D_IndexedMapOfEntity::Iterator aSubEntitiesIter (aGroup->Entities()); aSubEntitiesIter.More(); aSubEntitiesIter.Next())
        {
          const Handle(Select3D_SensitiveEntity)& aSubEntity = aSubEntitiesIter.Value();
          mySelector->QueueBVHBuild (aSubEntity);
        }
      }
    }
  }
  else
  {
    StdSelect_BRepSelectionTool::PreBuildBVH (theSelection);
  }
}

//=======================================================================
//function : SetUpdateMode
//purpose  :
//=======================================================================
void SelectMgr_SelectionManager::SetUpdateMode (const Handle(SelectMgr_SelectableObject)& theObject,
                                                const SelectMgr_TypeOfUpdate theType)
{
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
  {
    aSelIter.Value()->UpdateStatus (theType);
  }
}

//=======================================================================
//function : SetUpdateMode
//purpose  :
//=======================================================================
void SelectMgr_SelectionManager::SetUpdateMode (const Handle(SelectMgr_SelectableObject)& theObject,
                                                const Standard_Integer theMode,
                                                const SelectMgr_TypeOfUpdate theType)
{
  if (const Handle(SelectMgr_Selection)& aSel = theObject->Selection (theMode))
  {
    aSel->UpdateStatus (theType);
  }
}

//=======================================================================
//function : SetSelectionSensitivity
//purpose  : Allows to manage sensitivity of a particular selection of interactive object theObject and
//           changes previous sensitivity value of all sensitive entities in selection with theMode
//           to the given theNewSensitivity.
//=======================================================================
void SelectMgr_SelectionManager::SetSelectionSensitivity (const Handle(SelectMgr_SelectableObject)& theObject,
                                                          const Standard_Integer theMode,
                                                          const Standard_Integer theNewSens)
{
  Standard_ASSERT_RAISE (theNewSens >= 0, "Error! Selection sensitivity should not be negative value.");
  if (theObject.IsNull())
  {
    return;
  }

  const Handle(SelectMgr_Selection)& aSel = theObject->Selection (theMode);
  if (aSel.IsNull())
  {
    return;
  }

  const Standard_Integer aPrevSens = aSel->Sensitivity();
  aSel->SetSensitivity (theNewSens);
  if (myGlobal.Contains (theObject)
   && mySelector->Contains (theObject))
  {
    mySelector->myTolerances.Decrement (aPrevSens);
    mySelector->myTolerances.Add (theNewSens);
  }
}

//=======================================================================
//function : UpdateSelection
//purpose  :
//=======================================================================
void SelectMgr_SelectionManager::UpdateSelection (const Handle(SelectMgr_SelectableObject)& theObject)
{
  if (myGlobal.Contains (theObject)
   && mySelector->Contains (theObject))
  {
    mySelector->MoveSelectableObject (theObject);
  }
}
