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


#include <OSD_Environment.hxx>
#include <SelectMgr_DataMapIteratorOfDataMapOfObjectSelectors.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <SelectMgr_SequenceOfSelector.hxx>
#include <SelectMgr_ViewerSelector.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_SelectionManager,Standard_Transient)

static Standard_Integer FindIndex (const SelectMgr_SequenceOfSelector& theSelectorsSeq,
                                   const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  Standard_Integer aFoundIdx = 0;

  for (Standard_Integer anIdx = 1; anIdx <= theSelectorsSeq.Length() && aFoundIdx==0; anIdx++)
  {
    if (theSelector == theSelectorsSeq.Value (anIdx))
      aFoundIdx = anIdx;
  }

  return aFoundIdx;
}

//==================================================
// Function: Create
// Purpose :
//==================================================
SelectMgr_SelectionManager::SelectMgr_SelectionManager() {}

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
    Standard_Integer aRank = FindIndex (theSelectors, theSelector);
    if (aRank != 0 && aRank <= theSelectors.Length())
      theSelectors.Remove (aRank);
  }

  if (mySelectors.Contains (theSelector))
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
  if (myGlobal.Contains (theObject))
    return Standard_True;

  if (myLocal.IsBound (theObject))
    return Standard_True;

  return Standard_False;
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
  for (TColStd_MapIteratorOfMapOfTransient aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
  {
    Handle(SelectMgr_ViewerSelector) aSelector =
      Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorsIter.Key());
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
  if (!mySelectors.Contains (theSelector))
  {
    mySelectors.Add (theSelector);
  }

  if (theMode != -1)
    loadMode (theObject, theMode, theSelector);

  if (theObject->HasOwnPresentations())
    theSelector->AddSelectableObject (theObject);

  if (myLocal.IsBound (theObject))
  {
    SelectMgr_SequenceOfSelector& aSelectors = myLocal.ChangeFind (theObject);
    if (FindIndex (aSelectors, theSelector) == 0)
    {
        aSelectors.Append (theSelector);
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

      SelectMgr_SequenceOfSelector aSelectors;
      aSelectors.Append (theSelector);
      myLocal.Bind (theObject, aSelectors);
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
    for (TColStd_MapIteratorOfMapOfTransient aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      Handle(SelectMgr_ViewerSelector) aCurSelector =
        Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorsIter.Key());

      if (!aCurSelector->Contains (theObject))
        continue;

      for (theObject->Init(); theObject->More(); theObject->Next())
      {
        aCurSelector->RemoveSelectionOfObject (theObject, theObject->CurrentSelection());
        theObject->CurrentSelection()->UpdateBVHStatus (SelectMgr_TBU_Remove);
      }
      aCurSelector->RemoveSelectableObject (theObject);
    }

    myGlobal.Remove (theObject);
  }
  else if (myLocal.IsBound (theObject))
  {
    SelectMgr_SequenceOfSelector& aSelectors = myLocal.ChangeFind (theObject);
    for (Standard_Integer aSelectorsIdx = 1; aSelectorsIdx <= aSelectors.Length(); aSelectorsIdx++)
    {
      Handle(SelectMgr_ViewerSelector) aCurSelector = aSelectors (aSelectorsIdx);
      if (!aCurSelector->Contains (theObject))
        continue;

      for (theObject->Init(); theObject->More(); theObject->Next())
      {
        aCurSelector->RemoveSelectionOfObject (theObject, theObject->CurrentSelection());
        theObject->CurrentSelection()->UpdateBVHStatus (SelectMgr_TBU_Remove);
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

  for (theObject->Init(); theObject->More(); theObject->Next())
  {
    theSelector->RemoveSelectionOfObject (theObject, theObject->CurrentSelection());
    theObject->CurrentSelection()->UpdateBVHStatus (SelectMgr_TBU_Remove);
  }

  theSelector->RemoveSelectableObject (theObject);

  if (myLocal.IsBound (theObject))
  {
    SelectMgr_SequenceOfSelector& aSelectors = myLocal.ChangeFind (theObject);
    for (Standard_Integer aSelectorIdx = 1; aSelectorIdx <= aSelectors.Length(); aSelectorIdx++)
    {
      if (aSelectors (aSelectorIdx) == theSelector)
      {
        aSelectors.Remove (aSelectorIdx);
        break;
      }
    }

    if (aSelectors.IsEmpty())
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
  if (theObject->HasSelection (theMode))
  {
    isComputed = theObject->Selection (theMode)->IsEmpty() ? 0 : 1;
  }

  if (!isComputed)
    loadMode (theObject, theMode);

  if (theSelector.IsNull())
  {
    if (myGlobal.Contains (theObject))
    {
      for (TColStd_MapIteratorOfMapOfTransient aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
      {
        Handle(SelectMgr_ViewerSelector) aCurSelector =
          Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorsIter.Key());
        Activate (theObject, theMode, aCurSelector);
      }
    }
    else if (myLocal.IsBound (theObject))
    {
      SelectMgr_SequenceOfSelector& theSelectors = myLocal.ChangeFind (theObject);
      for (Standard_Integer aSelectorIdx = 1; aSelectorIdx <= theSelectors.Length(); aSelectorIdx++)
      {
        Handle(SelectMgr_ViewerSelector) aCurSelector = theSelectors (aSelectorIdx);
        Activate (theObject, theMode, aCurSelector);
      }
    }
  }

  const Handle(SelectMgr_Selection)& aSelection = theObject->Selection (theMode);

  switch (aSelection->UpdateStatus())
  {
  case SelectMgr_TOU_Full:
    if (theObject->HasSelection (theMode))
      theSelector->RemoveSelectionOfObject (theObject, aSelection);
    theObject->RecomputePrimitives (theMode);
  case SelectMgr_TOU_Partial:
    if(theObject->HasTransformation())
      theObject->UpdateTransformations (aSelection);
    theSelector->RebuildObjectsTree();
    break;
  default:
    break;
  }
  aSelection->UpdateStatus(SelectMgr_TOU_None);

  switch (aSelection->BVHUpdateStatus())
  {
  case SelectMgr_TBU_Add:
  case SelectMgr_TBU_Renew:
    theSelector->AddSelectionToObject (theObject, aSelection);
    break;
  case SelectMgr_TBU_Remove:
    if (aSelection->GetSelectionState() == SelectMgr_SOS_Deactivated)
      theSelector->AddSelectionToObject (theObject, aSelection);
    break;
  default:
    break;
  }
  aSelection->UpdateBVHStatus (SelectMgr_TBU_None);

  if (myGlobal.Contains (theObject))
  {
    const Standard_Integer aGlobalSelMode = theObject->GlobalSelectionMode();
    if (theMode != aGlobalSelMode && theSelector->IsActive (theObject, aGlobalSelMode))
    {
      theSelector->Deactivate (theObject->Selection (aGlobalSelMode));
    }
    theSelector->Activate (theObject->Selection (theMode));
  }
  else
  {
    if (myLocal.IsBound (theObject))
    {
      if (FindIndex (myLocal.Find (theObject), theSelector) == 0)
        (myLocal.ChangeFind (theObject)).Append (theSelector);
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
    return;

  Standard_Boolean isInGlobal = myGlobal.Contains (theObject);
  Standard_Boolean hasSelection = theMode == -1 ? Standard_True : theObject->HasSelection (theMode);

  if (theSelector.IsNull())
  {
    Handle(SelectMgr_ViewerSelector) aSelector;
    for (TColStd_MapIteratorOfMapOfTransient aSelectorIter (mySelectors); aSelectorIter.More(); aSelectorIter.Next())
    {
      aSelector = Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorIter.Key());
      if (isInGlobal || myLocal.IsBound (theObject))
      {
        if (theMode == -1)
        {
          for (theObject->Init(); theObject->More(); theObject->Next())
          {
            aSelector->Deactivate (theObject->CurrentSelection());
          }
        }
        else
        {
          if (hasSelection)
            aSelector->Deactivate (theObject->Selection (theMode));
        }
      }
    }
  }
  else
  {
    if (theMode == -1)
    {
      for (theObject->Init(); theObject->More(); theObject->Next())
      {
        theSelector->Deactivate (theObject->CurrentSelection());
      }
    }
    else
      if (hasSelection)
        theSelector->Deactivate (theObject->Selection (theMode));
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
    return Standard_False;

  if (!(myGlobal.Contains (theObject) || myLocal.IsBound (theObject)))
    return Standard_False;

  if (theMode == -1 && theSelector.IsNull())
  {
    for (theObject->Init(); theObject->More(); theObject->Next())
    {
      if (IsActivated (theObject, theObject->CurrentSelection()->Mode()))
        return Standard_True;
    }

    return Standard_False;
  }

  if (!theObject->HasSelection (theMode))
    return Standard_False;

  const Handle(SelectMgr_Selection)& aSelection = theObject->Selection (theMode);
  if (theSelector.IsNull())
  {
    for (TColStd_MapIteratorOfMapOfTransient aSelectorIter (mySelectors); aSelectorIter.More(); aSelectorIter.Next())
    {
      Handle(SelectMgr_ViewerSelector) aSelector (Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorIter.Key()));
      if (aSelector->Status (aSelection) == SelectMgr_SOS_Activated)
        return Standard_True;
    }
  }
  else
  {
    return theSelector->Status (aSelection) == SelectMgr_SOS_Activated;
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
    return;

  if (theSelector.IsNull())
  {
    if (!(myGlobal.Contains (theObj) || myLocal.IsBound(theObj)))
      return;

    TColStd_MapIteratorOfMapOfTransient aSelectorsIter (mySelectors);
    Handle(SelectMgr_ViewerSelector) aSelector;
    for( ; aSelectorsIter.More(); aSelectorsIter.Next())
    {
      aSelector = Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorsIter.Key());
      ClearSelectionStructures (theObj, theMode, aSelector);
    }
  }
  else
  {
    if (!(myGlobal.Contains (theObj) || myLocal.IsBound (theObj)))
      return;

    if (theMode != -1)
    {
      if (theObj->HasSelection (theMode))
      {
        const Handle(SelectMgr_Selection)& aSelection = theObj->Selection (theMode);
        if (theObj->HasSelection (theMode))
        {
          theSelector->RemoveSelectionOfObject (theObj, aSelection);
          aSelection->UpdateBVHStatus (SelectMgr_TBU_Add);
        }
      }
    }
    else
    {
      for (theObj->Init(); theObj->More(); theObj->Next())
      {
        const Handle(SelectMgr_Selection)& aSelection = theObj->CurrentSelection();
        theSelector->RemoveSelectionOfObject (theObj, aSelection);
        aSelection->UpdateBVHStatus (SelectMgr_TBU_Add);
      }
    }
    theSelector->RebuildObjectsTree();
  }
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
    return;

  if (theSelector.IsNull())
  {
    if (!(myGlobal.Contains (theObj) || myLocal.IsBound(theObj)))
      return;

    TColStd_MapIteratorOfMapOfTransient aSelectorsIter (mySelectors);
    Handle(SelectMgr_ViewerSelector) aSelector;
    for( ; aSelectorsIter.More(); aSelectorsIter.Next())
    {
      aSelector = Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorsIter.Key());
      RestoreSelectionStructures (theObj, theMode, aSelector);
    }
  }
  else
  {
    if (!(myGlobal.Contains (theObj) || myLocal.IsBound (theObj)))
      return;

    if (theMode != -1)
    {
      if (theObj->HasSelection (theMode))
      {
        const Handle(SelectMgr_Selection)& aSelection = theObj->Selection (theMode);
        if (theObj->HasSelection (theMode))
        {
          theSelector->AddSelectionToObject (theObj, aSelection);
          aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
        }
      }
    }
    else
    {
      for (theObj->Init(); theObj->More(); theObj->Next())
      {
        const Handle(SelectMgr_Selection)& aSelection = theObj->CurrentSelection();
        theSelector->AddSelectionToObject (theObj, aSelection);
        aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
      }
    }
    theSelector->RebuildObjectsTree();
  }
}

//=======================================================================
//function : rebuildSelectionStructures
//purpose  : Internal function that marks 1st level BVH of object theObj
//           as outdated
//=======================================================================
void SelectMgr_SelectionManager::rebuildSelectionStructures (const Handle(SelectMgr_ViewerSelector)& theSelector)
{
  if (theSelector.IsNull())
  {
    Handle(SelectMgr_ViewerSelector) aSelector;
    for(TColStd_MapIteratorOfMapOfTransient aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      aSelector = Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorsIter.Key());
      rebuildSelectionStructures (aSelector);
    }
  }
  else
  {
    theSelector->RebuildObjectsTree();
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

  for (TColStd_MapIteratorOfMapOfTransient aSelectorIter (mySelectors); aSelectorIter.More(); aSelectorIter.Next())
  {
    Handle(SelectMgr_ViewerSelector) aCurSelector = Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorIter.Key());

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
    return;

  if (!(myGlobal.Contains (theObject) || myLocal.IsBound (theObject)))
    return;

  if (theMode == -1)
  {
    for (theObject->Init(); theObject->More(); theObject->Next())
    {
      const Handle(SelectMgr_Selection)& aSelection = theObject->CurrentSelection();
      Standard_Integer aSelMode = aSelection->Mode();
      recomputeSelectionMode (theObject, aSelection, aSelMode);
    }
  }
  else
  {
    if (!theObject->HasSelection (theMode))
      return;

    const Handle(SelectMgr_Selection)& aSelection = theObject->Selection (theMode);
    recomputeSelectionMode (theObject, aSelection, theMode);
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
    return;

  for (theObject->Init(); theObject->More(); theObject->Next())
  {
    const Handle(SelectMgr_Selection)& aSelection = theObject->CurrentSelection();
    if (theIsForce)
    {
      switch (aSelection->UpdateStatus())
      {
      case SelectMgr_TOU_Full:
        ClearSelectionStructures (theObject, aSelection->Mode());
        theObject->RecomputePrimitives (aSelection->Mode()); // no break on purpose...
        RestoreSelectionStructures (theObject, aSelection->Mode());
      case SelectMgr_TOU_Partial:
        theObject->UpdateTransformations (aSelection);
        rebuildSelectionStructures();
        break;
      default:
        break;
      }
      aSelection->UpdateStatus (SelectMgr_TOU_None);
      aSelection->UpdateBVHStatus (SelectMgr_TBU_None);
    }

    for (TColStd_MapIteratorOfMapOfTransient aSelectorIter (mySelectors); aSelectorIter.More(); aSelectorIter.Next())
    {
      Handle(SelectMgr_ViewerSelector) aSelector (Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorIter.Key()));
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
    return;

  Standard_Boolean isKnown = myGlobal.Contains (theObject);
  if (!isKnown)
    isKnown = (myLocal.IsBound (theObject) && (FindIndex (myLocal.Find (theObject), theSelector) != 0));
  if (!isKnown)
    return;

  for (PrsMgr_ListOfPresentableObjectsIter aChildIter (theObject->Children()); aChildIter.More(); aChildIter.Next())
  {
    Update (Handle(SelectMgr_SelectableObject)::DownCast (aChildIter.Value()), theSelector, theIsForce);
  }

  if (!theObject->HasOwnPresentations())
    return;

  for (theObject->Init(); theObject->More(); theObject->Next())
  {
    const Handle(SelectMgr_Selection)& aSelection = theObject->CurrentSelection();
    if (theIsForce)
    {
      switch (aSelection->UpdateStatus())
      {
      case SelectMgr_TOU_Full:
        ClearSelectionStructures (theObject, aSelection->Mode());
        theObject->RecomputePrimitives (aSelection->Mode());
        RestoreSelectionStructures (theObject, aSelection->Mode());
      case SelectMgr_TOU_Partial:
        theObject->UpdateTransformations (aSelection);
        rebuildSelectionStructures();
        break;
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
        ClearSelectionStructures (theObject, aSelection->Mode(), theSelector);
        theObject->RecomputePrimitives (aSelection->Mode());
        RestoreSelectionStructures (theObject, aSelection->Mode(), theSelector);
      case SelectMgr_TOU_Partial:
        if (theObject->HasTransformation())
        {
          theObject->UpdateTransformations (aSelection);
          theSelector->RebuildObjectsTree();
        }
        break;
      default:
        break;
      }

      aSelection->UpdateStatus(SelectMgr_TOU_None);
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
    return;

  if (!theObject->HasSelection (theMode))
  {
    Handle(SelectMgr_Selection) aNewSel = new SelectMgr_Selection (theMode);
    theObject->AddSelection (aNewSel, theMode);
    if (theSelector.IsNull())
    {
      if (myGlobal.Contains (theObject))
      {
        TColStd_MapIteratorOfMapOfTransient aSelectorIter (mySelectors);
        for ( ; aSelectorIter.More(); aSelectorIter.Next())
        {
          Handle(SelectMgr_ViewerSelector) aSelector =
            Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorIter.Key());
          aSelector->AddSelectionToObject (theObject, aNewSel);
          aNewSel->UpdateBVHStatus (SelectMgr_TBU_None);
        }
      }
      else if (myLocal.IsBound (theObject))
      {
        const SelectMgr_SequenceOfSelector& aSelectors = myLocal (theObject);
        for (Standard_Integer aSelectorIdx = 1; aSelectorIdx <= aSelectors.Length(); ++aSelectorIdx)
        {
          aSelectors (aSelectorIdx)->AddSelectionToObject (theObject, aNewSel);
          aNewSel->UpdateBVHStatus (SelectMgr_TBU_None);
        }
      }
    }
    else
    {
      theSelector->AddSelectionToObject (theObject, aNewSel);
      aNewSel->UpdateBVHStatus (SelectMgr_TBU_None);
    }
  }
  else if (theObject->Selection (theMode)->IsEmpty())
  {
    if (theObject->Selection (theMode)->BVHUpdateStatus() == SelectMgr_TBU_Remove)
    {
      Handle(SelectMgr_Selection) aNewSel = new SelectMgr_Selection (theMode);
      theObject->AddSelection (aNewSel, theMode);
      theObject->Selection (theMode)->UpdateBVHStatus (SelectMgr_TBU_Remove);
      theObject->Selection (theMode)->SetSelectionState (SelectMgr_SOS_Deactivated);
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
  for (theObject->Init(); theObject->More(); theObject->Next())
    theObject->CurrentSelection()->UpdateStatus (theType);
}

//=======================================================================
//function : SetUpdateMode
//purpose  :
//=======================================================================
void SelectMgr_SelectionManager::SetUpdateMode (const Handle(SelectMgr_SelectableObject)& theObject,
                                                const Standard_Integer theMode,
                                                const SelectMgr_TypeOfUpdate theType)
{
  if (theObject->HasSelection (theMode))
    theObject->Selection (theMode)->UpdateStatus (theType);
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
  Standard_ASSERT_RAISE (theNewSens > 0,
    "Error! Selection sensitivity have positive value.");

  if (theObject.IsNull() || !theObject->HasSelection (theMode))
    return;

  Handle(SelectMgr_Selection) aSel = theObject->Selection (theMode);
  const Standard_Integer aPrevSens = aSel->Sensitivity();
  aSel->SetSensitivity (theNewSens);

  if (!(myGlobal.Contains (theObject) || myLocal.IsBound (theObject)))
    return;

  if (myGlobal.Contains (theObject))
  {
    for (TColStd_MapIteratorOfMapOfTransient aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      Handle(SelectMgr_ViewerSelector) aSelector = Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorsIter.Key());
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
    for (TColStd_MapIteratorOfMapOfTransient aSelectorsIter (mySelectors); aSelectorsIter.More(); aSelectorsIter.Next())
    {
      Handle(SelectMgr_ViewerSelector) aSelector = Handle(SelectMgr_ViewerSelector)::DownCast (aSelectorsIter.Key());
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
