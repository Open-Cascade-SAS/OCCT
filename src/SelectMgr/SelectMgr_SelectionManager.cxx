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

#include <OSD_Environment.hxx>
#include <SelectMgr_DataMapIteratorOfDataMapOfObjectSelectors.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SequenceOfSelector.hxx>
#include <SelectMgr_ViewerSelector.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_SelectionManager,Standard_Transient)

namespace
{
  static bool containsSelector (const SelectMgr_SequenceOfSelector& theSelectorsSeq,
                                const Handle(SelectMgr_ViewerSelector)& theSelector)
  {
    for (SelectMgr_SequenceOfSelector::Iterator aSelectorIter (theSelectorsSeq); aSelectorIter.More(); aSelectorIter.Next())
    {
      if (aSelectorIter.Value() == theSelector)
      {
        return true;
      }
    }
    return false;
  }
}

//==================================================
// Function: Create
// Purpose :
//==================================================
SelectMgr_SelectionManager::SelectMgr_SelectionManager()
{
  //
}

//==================================================
// Function: Add
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Add (const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  mySelectors.Add (theSelector);
}

//==================================================
// Function: Remove
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Remove (const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  for (SelectMgr_DataMapIteratorOfDataMapOfObjectSelectors aSelIter (myLocal); aSelIter.More(); aSelIter.Next())
  {
    SelectMgr_SequenceOfSelector& theSelectors = myLocal.ChangeFind (aSelIter.Key());
    for (SelectMgr_SequenceOfSelector::Iterator aSelectorIter (theSelectors); aSelectorIter.More(); aSelectorIter.Next())
    {
      if (aSelectorIter.Value() == theSelector)
      {
        theSelectors.Remove (aSelectorIter);
        break;
      }
    }
  }

  mySelectors.Remove (theSelector);
}

//==================================================
// Function: Contains
// Purpose :
//==================================================
Standard_Boolean SelectMgr_SelectionManager::Contains (const Handle(SelectMgr_ViewerSelector)& theSelector) const
{
  return mySelectors.Contains (theSelector);
}

//==================================================
// Function: Contains
// Purpose :
//==================================================
Standard_Boolean SelectMgr_SelectionManager::Contains (const Handle(SelectMgr_SelectableObject)& theObject) const
{
  return myGlobal.Contains (theObject)
      || myLocal.IsBound (theObject);
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
  for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
  {
    const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorsIter.Key();
    if (!aSelector->Contains (theObject) && theObject->HasOwnPresentations())
    {
      aSelector->AddSelectableObject (theObject);
    }
  }
  if (theMode != -1)
    loadMode (theObject, theMode);
}


//==================================================
// Function: Load
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Load (const Handle(SelectMgr_SelectableObject)& theObject,
                                       const Handle(SelectMgr_ViewerSelector)& theSelector,
                                       const Standard_Integer theMode)
{
  mySelectors.Add (theSelector);
  if (theMode != -1)
  {
    loadMode (theObject, theMode, theSelector);
  }

  if (theObject->HasOwnPresentations())
  {
    theSelector->AddSelectableObject (theObject);
  }

  if (SelectMgr_SequenceOfSelector* aSelectors = myLocal.ChangeSeek (theObject))
  {
    if (!containsSelector (*aSelectors, theSelector))
    {
      aSelectors->Append (theSelector);
    }
  }
  else
  {
    if (!myGlobal.Contains (theObject))
    {
      for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObject->Children()); anChildrenIter.More(); anChildrenIter.Next())
      {
        Load (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theSelector, theMode);
      }
      if (!theObject->HasOwnPresentations())
        return;

      SelectMgr_SequenceOfSelector aGlobSelectors;
      aGlobSelectors.Append (theSelector);
      myLocal.Bind (theObject, aGlobSelectors);
    }
  }
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
    for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      const Handle(SelectMgr_ViewerSelector)& aCurSelector = aSelectorsIter.Key();
      if (!aCurSelector->Contains (theObject))
      {
        continue;
      }

      for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
      {
        aCurSelector->RemoveSelectionOfObject (theObject, aSelIter.Value());
        aSelIter.Value()->UpdateBVHStatus (SelectMgr_TBU_Remove);
      }
      aCurSelector->RemoveSelectableObject (theObject);
    }

    myGlobal.Remove (theObject);
  }
  else if (SelectMgr_SequenceOfSelector* aSelectors = myLocal.ChangeSeek (theObject))
  {
    for (SelectMgr_SequenceOfSelector::Iterator aSelectorIter (*aSelectors); aSelectorIter.More(); aSelectorIter.Next())
    {
      const Handle(SelectMgr_ViewerSelector)& aCurSelector = aSelectorIter.Value();
      if (!aCurSelector->Contains (theObject))
        continue;

      for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
      {
        aCurSelector->RemoveSelectionOfObject (theObject, aSelIter.Value());
        aSelIter.Value()->UpdateBVHStatus (SelectMgr_TBU_Remove);
      }
      aCurSelector->RemoveSelectableObject (theObject);
    }

    myLocal.UnBind (theObject);
  }

  theObject->ClearSelections();
}

//==================================================
// Function: Remove
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Remove (const Handle(SelectMgr_SelectableObject)& theObject,
                                         const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  if (!theSelector->Contains (theObject))
    return;

  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObject->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    Remove (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theSelector);
  }
  if (!theObject->HasOwnPresentations())
    return;

  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
  {
    theSelector->RemoveSelectionOfObject (theObject, aSelIter.Value());
    aSelIter.Value()->UpdateBVHStatus (SelectMgr_TBU_Remove);
  }
  theSelector->RemoveSelectableObject (theObject);

  if (SelectMgr_SequenceOfSelector* aSelectors = myLocal.ChangeSeek (theObject))
  {
    for (SelectMgr_SequenceOfSelector::Iterator aSelectorIter (*aSelectors); aSelectorIter.More(); aSelectorIter.Next())
    {
      if (aSelectorIter.Value() == theSelector)
      {
        aSelectors->Remove (aSelectorIter);
        break;
      }
    }

    if (aSelectors->IsEmpty())
    {
      myLocal.UnBind (theObject);
    }
  }
}

//==================================================
// Function: Activate
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Activate (const Handle(SelectMgr_SelectableObject)& theObject,
                                           const Standard_Integer theMode,
                                           const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  if (theMode == -1)
    return;

  if (!theSelector.IsNull() && !mySelectors.Contains (theSelector))
    return;

  for (PrsMgr_ListOfPresentableObjectsIter anChildIter (theObject->Children()); anChildIter.More(); anChildIter.Next())
  {
    Activate (Handle(SelectMgr_SelectableObject)::DownCast (anChildIter.Value()), theMode, theSelector);
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

  if (theSelector.IsNull())
  {
    if (myGlobal.Contains (theObject))
    {
      for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
      {
        const Handle(SelectMgr_ViewerSelector)& aCurSelector = aSelectorsIter.Key();
        Activate (theObject, theMode, aCurSelector);
      }
    }
    else if (SelectMgr_SequenceOfSelector* aSelectors = myLocal.ChangeSeek (theObject))
    {
      for (SelectMgr_SequenceOfSelector::Iterator aSelectorIter (*aSelectors); aSelectorIter.More(); aSelectorIter.Next())
      {
        Handle(SelectMgr_ViewerSelector) aCurSelector = aSelectorIter.Value();
        Activate (theObject, theMode, aCurSelector);
      }
    }
  }

  const Handle(SelectMgr_Selection)& aSelection = theObject->Selection (theMode);
  switch (aSelection->UpdateStatus())
  {
    case SelectMgr_TOU_Full:
    {
      if (theObject->HasSelection (theMode))
      {
        theSelector->RemoveSelectionOfObject (theObject, aSelection);
      }
      theObject->RecomputePrimitives (theMode);
      // pass through SelectMgr_TOU_Partial
    }
    Standard_FALLTHROUGH
    case SelectMgr_TOU_Partial:
    {
      theObject->UpdateTransformations (aSelection);
      theSelector->RebuildObjectsTree();
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
      theSelector->AddSelectionToObject (theObject, aSelection);
      break;
    }
    case SelectMgr_TBU_Remove:
    {
      if (aSelection->GetSelectionState() == SelectMgr_SOS_Deactivated)
      {
        theSelector->AddSelectionToObject (theObject, aSelection);
      }
      break;
    }
    default:
      break;
  }
  aSelection->UpdateBVHStatus (SelectMgr_TBU_None);

  if (myGlobal.Contains (theObject))
  {
    theSelector->Activate (theObject->Selection (theMode));
  }
  else
  {
    if (SelectMgr_SequenceOfSelector* aSelectors = myLocal.ChangeSeek (theObject))
    {
      if (!containsSelector (*aSelectors, theSelector))
      {
        aSelectors->Append (theSelector);
      }
      theSelector->Activate (theObject->Selection (theMode));
    }
  }
}

//==================================================
// Function: Deactivate
// Purpose :
//==================================================
void SelectMgr_SelectionManager::Deactivate (const Handle(SelectMgr_SelectableObject)& theObject,
                                             const Standard_Integer theMode,
                                             const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObject->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    Deactivate (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theMode, theSelector);
  }
  if (!theObject->HasOwnPresentations())
  {
    return;
  }
  if (!myGlobal.Contains(theObject)
   && !myLocal.IsBound  (theObject))
  {
    return;
  }

  const Handle(SelectMgr_Selection)& aSel = theObject->Selection (theMode);
  if (!theSelector.IsNull())
  {
    if (theMode == -1)
    {
      for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
      {
        theSelector->Deactivate (aSelIter.Value());
      }
    }
    else if (!aSel.IsNull())
    {
      theSelector->Deactivate (aSel);
    }
    return;
  }

  for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorIter (mySelectors); aSelectorIter.More(); aSelectorIter.Next())
  {
    const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorIter.Key();
    if (theMode == -1)
    {
      for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
      {
        aSelector->Deactivate (aSelIter.Value());
      }
    }
    else if (!aSel.IsNull())
    {
      aSelector->Deactivate (aSel);
    }
  }
}

//=======================================================================
//function : IsActivated
//purpose  :
//=======================================================================
Standard_Boolean SelectMgr_SelectionManager::IsActivated (const Handle(SelectMgr_SelectableObject)& theObject,
                                                          const Standard_Integer theMode,
                                                          const Handle(SelectMgr_ViewerSelector)& theSelector) const
{
  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObject->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    if (IsActivated (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theMode, theSelector))
      return Standard_True;
  }
  if (!theObject->HasOwnPresentations())
  {
    return Standard_False;
  }
  if (!myGlobal.Contains(theObject)
   && !myLocal.IsBound  (theObject))
  {
    return Standard_False;
  }

  if (theMode == -1 && theSelector.IsNull())
  {
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
    {
      if (IsActivated (theObject, aSelIter.Value()->Mode()))
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

  if (!theSelector.IsNull())
  {
    return theSelector->Status (aSelection) == SelectMgr_SOS_Activated;
  }

  for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorIter (mySelectors); aSelectorIter.More(); aSelectorIter.Next())
  {
    const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorIter.Key();
    if (aSelector->Status (aSelection) == SelectMgr_SOS_Activated)
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : ClearSelectionStructures
//purpose  : Removes sensitive entities from all viewer selectors
//           after method Clear() was called to the selection they belonged to
//           or it was recomputed somehow
//=======================================================================
void SelectMgr_SelectionManager::ClearSelectionStructures (const Handle(SelectMgr_SelectableObject)& theObj,
                                                           const Standard_Integer theMode,
                                                           const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObj->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    ClearSelectionStructures (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theMode, theSelector);
  }

  if (!theObj->HasOwnPresentations())
  {
    return;
  }
  if (!myGlobal.Contains(theObj)
   && !myLocal.IsBound  (theObj))
  {
    return;
  }

  if (theSelector.IsNull())
  {
    for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorsIter.Key();
      ClearSelectionStructures (theObj, theMode, aSelector);
    }
    return;
  }

  if (theMode != -1)
  {
    if (const Handle(SelectMgr_Selection)& aSelection = theObj->Selection (theMode))
    {
      theSelector->RemoveSelectionOfObject (theObj, aSelection);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_Add);
    }
  }
  else
  {
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObj->Selections()); aSelIter.More(); aSelIter.Next())
    {
      const Handle(SelectMgr_Selection)& aSelection = aSelIter.Value();
      theSelector->RemoveSelectionOfObject (theObj, aSelection);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_Add);
    }
  }
  theSelector->RebuildObjectsTree();
}

//=======================================================================
//function : RestoreSelectionStructuress
//purpose  : Re-adds newely calculated sensitive  entities of recomputed selection
//           defined by mode theMode to all viewer selectors contained that selection.
//=======================================================================
void SelectMgr_SelectionManager::RestoreSelectionStructures (const Handle(SelectMgr_SelectableObject)& theObj,
                                                             const Standard_Integer theMode,
                                                             const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  for (PrsMgr_ListOfPresentableObjectsIter anChildrenIter (theObj->Children()); anChildrenIter.More(); anChildrenIter.Next())
  {
    RestoreSelectionStructures (Handle(SelectMgr_SelectableObject)::DownCast (anChildrenIter.Value()), theMode, theSelector);
  }
  if (!theObj->HasOwnPresentations())
  {
    return;
  }
  if (!myGlobal.Contains(theObj)
   && !myLocal.IsBound  (theObj))
  {
    return;
  }

  if (theSelector.IsNull())
  {
    for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorsIter.Key();
      RestoreSelectionStructures (theObj, theMode, aSelector);
    }
    return;
  }

  if (theMode != -1)
  {
    if (const Handle(SelectMgr_Selection)& aSelection = theObj->Selection (theMode))
    {
      theSelector->AddSelectionToObject (theObj, aSelection);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
    }
  }
  else
  {
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObj->Selections()); aSelIter.More(); aSelIter.Next())
    {
      const Handle(SelectMgr_Selection)& aSelection = aSelIter.Value();
      theSelector->AddSelectionToObject (theObj, aSelection);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
    }
  }
  theSelector->RebuildObjectsTree();
}

//=======================================================================
//function : rebuildSelectionStructures
//purpose  : Internal function that marks 1st level BVH of object theObj
//           as outdated
//=======================================================================
void SelectMgr_SelectionManager::rebuildSelectionStructures (const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  if (!theSelector.IsNull())
  {
    theSelector->RebuildObjectsTree();
    return;
  }

  for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
  {
    const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorsIter.Key();
    rebuildSelectionStructures (aSelector);
  }
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

  for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorIter (mySelectors); aSelectorIter.More(); aSelectorIter.Next())
  {
    const Handle(SelectMgr_ViewerSelector)& aCurSelector = aSelectorIter.Key();

    ClearSelectionStructures (theObject, theMode, aCurSelector);
    theObject->RecomputePrimitives (theMode);
    RestoreSelectionStructures (theObject, theMode, aCurSelector);
    theSelection->UpdateStatus (SelectMgr_TOU_None);
    theSelection->UpdateBVHStatus (SelectMgr_TBU_None);
  }
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
  if (!myGlobal.Contains (theObject)
   && !myLocal.IsBound (theObject))
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
    if (theIsForce)
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
          rebuildSelectionStructures();
          break;
        }
        default:
          break;
      }
      aSelection->UpdateStatus (SelectMgr_TOU_None);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
    }

    for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorIter (mySelectors); aSelectorIter.More(); aSelectorIter.Next())
    {
      const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorIter.Key();
      Update (theObject, aSelector, Standard_False);
    }
  }
}

//==================================================
// Function: Update
// Purpose : Attention, it is required to know what is done...
//==================================================
void SelectMgr_SelectionManager::Update (const Handle(SelectMgr_SelectableObject)& theObject,
                                         const Handle(SelectMgr_ViewerSelector)& theSelector,
                                         const Standard_Boolean theIsForce)
{
  if (!mySelectors.Contains (theSelector))
  {
    return;
  }

  if (!myGlobal.Contains (theObject))
  {
    const SelectMgr_SequenceOfSelector* aSelectors = myLocal.Seek (theObject);
    if (aSelectors == NULL
    || !containsSelector (*aSelectors, theSelector))
    {
      return;
    }
  }

  for (PrsMgr_ListOfPresentableObjectsIter aChildIter (theObject->Children()); aChildIter.More(); aChildIter.Next())
  {
    Update (Handle(SelectMgr_SelectableObject)::DownCast (aChildIter.Value()), theSelector, theIsForce);
  }
  if (!theObject->HasOwnPresentations())
  {
    return;
  }

  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theObject->Selections()); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_Selection)& aSelection = aSelIter.Value();
    if (theIsForce)
    {
      switch (aSelection->UpdateStatus())
      {
        case SelectMgr_TOU_Full:
        {
          ClearSelectionStructures (theObject, aSelection->Mode());
          theObject->RecomputePrimitives (aSelection->Mode());
          RestoreSelectionStructures (theObject, aSelection->Mode());
          // pass through SelectMgr_TOU_Partial
        }
        Standard_FALLTHROUGH
        case SelectMgr_TOU_Partial:
        {
          theObject->UpdateTransformations (aSelection);
          rebuildSelectionStructures();
          break;
        }
        default:
          break;
      }
      aSelection->UpdateStatus (SelectMgr_TOU_None);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
    }

    if (theSelector->Status (aSelection) == SelectMgr_SOS_Activated)
    {
      switch (aSelection->UpdateStatus())
      {
        case SelectMgr_TOU_Full:
        {
          ClearSelectionStructures (theObject, aSelection->Mode(), theSelector);
          theObject->RecomputePrimitives (aSelection->Mode());
          RestoreSelectionStructures (theObject, aSelection->Mode(), theSelector);
          // pass through SelectMgr_TOU_Partial
        }
        Standard_FALLTHROUGH
        case SelectMgr_TOU_Partial:
        {
          theObject->UpdateTransformations (aSelection);
          theSelector->RebuildObjectsTree();
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
                                           const Standard_Integer theMode,
                                           const Handle(SelectMgr_ViewerSelector)& theSelector)
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
      }
    }
    return;
  }

  Handle(SelectMgr_Selection) aNewSel = new SelectMgr_Selection (theMode);
  theObject->AddSelection (aNewSel, theMode);
  if (!theSelector.IsNull())
  {
    theSelector->AddSelectionToObject (theObject, aNewSel);
    aNewSel->UpdateBVHStatus (SelectMgr_TBU_None);
    return;
  }

  if (myGlobal.Contains (theObject))
  {
    for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorIter (mySelectors); aSelectorIter.More(); aSelectorIter.Next())
    {
      const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorIter.Key();
      aSelector->AddSelectionToObject (theObject, aNewSel);
      aNewSel->UpdateBVHStatus (SelectMgr_TBU_None);
    }
  }
  else if (SelectMgr_SequenceOfSelector* aSelectors = myLocal.ChangeSeek (theObject))
  {
    for (SelectMgr_SequenceOfSelector::Iterator aSelectorIter (*aSelectors); aSelectorIter.More(); aSelectorIter.Next())
    {
      aSelectorIter.Value()->AddSelectionToObject (theObject, aNewSel);
      aNewSel->UpdateBVHStatus (SelectMgr_TBU_None);
    }
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
  Standard_ASSERT_RAISE (theNewSens > 0, "Error! Selection sensitivity have positive value.");
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
  if (myGlobal.Contains (theObject))
  {
    for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorsIter.Key();
      if (aSelector->Contains (theObject))
      {
        aSelector->myTolerances.Decrement (aPrevSens);
        aSelector->myTolerances.Add (theNewSens);
        aSelector->myToUpdateTolerance = Standard_True;
      }
    }
  }
  if (myLocal.IsBound (theObject))
  {
    const SelectMgr_SequenceOfSelector& aSelectors = myLocal (theObject);
    for (SelectMgr_SequenceOfSelector::Iterator aLocalIter (aSelectors); aLocalIter.More(); aLocalIter.Next())
    {
      Handle(SelectMgr_ViewerSelector)& aCurSel = aLocalIter.ChangeValue();
      aCurSel->myTolerances.Decrement (aPrevSens);
      aCurSel->myTolerances.Add (theNewSens);
      aCurSel->myToUpdateTolerance = Standard_True;
    }
  }
}

//=======================================================================
//function : UpdateSelection
//purpose  :
//=======================================================================
void SelectMgr_SelectionManager::UpdateSelection (const Handle(SelectMgr_SelectableObject)& theObject)
{
  if (myGlobal.Contains (theObject))
  {
    for (NCollection_Map<Handle(SelectMgr_ViewerSelector)>::Iterator aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      const Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorsIter.Key();
      if (aSelector->Contains (theObject))
      {
        aSelector->MoveSelectableObject (theObject);
      }
    }
  }

  if (myLocal.IsBound (theObject))
  {
    const SelectMgr_SequenceOfSelector& aSelectors = myLocal (theObject);
    for (SelectMgr_SequenceOfSelector::Iterator aSelectorsIter (aSelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      Handle(SelectMgr_ViewerSelector)& aSelector = aSelectorsIter.ChangeValue();
      if (aSelector->Contains (theObject))
      {
        aSelector->MoveSelectableObject (theObject);
      }
    }
  }
}
