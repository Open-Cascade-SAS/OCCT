// Created on: 2007-07-06
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <NIS_InteractiveContext.hxx>
#include <NIS_InteractiveObject.hxx>
#include <NIS_View.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Bnd_B2f.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_InteractiveContext, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT (NIS_InteractiveContext, Standard_Transient)

static void markAllDrawersUpdated   (const NCollection_Map<Handle(NIS_Drawer)>&);

//=======================================================================
//function : NIS_InteractiveContext()
//purpose  : Constructor
//=======================================================================

NIS_InteractiveContext::NIS_InteractiveContext ()
  : myAllocator       (new NIS_Allocator(1024*100)),
    myLastObjectId    (0),
    myObjects         (1000),
//     myDrawers       (101, myAllocator),
    mySelectionMode   (Mode_NoSelection),
    myIsShareDrawList (Standard_True)
{
  // ID == 0 is invalid so we reserve this item from subsequent allocation.
  myObjects.SetValue(myLastObjectId, NULL);
}

//=======================================================================
//function : ~NIS_InteractiveContext
//purpose  : Destructor
//=======================================================================

NIS_InteractiveContext::~NIS_InteractiveContext ()
{
  // Unregister this context in all referred views
  NCollection_List<Handle(NIS_View)>::Iterator anIterV (myViews);
  for (; anIterV.More(); anIterV.Next())
    if (anIterV.Value().IsNull() == Standard_False)
      anIterV.Value()->RemoveContext(this);

//   // Unregister this context in all referred drawers
//   NCollection_Map<Handle(NIS_Drawer)>::Iterator anIterD (myDrawers);
//   for (; anIterD.More(); anIterD.Next())
//     if (anIterD.Value().IsNull() == Standard_False)
//       anIterD.Value()->myCtx = 0L;
}


//=======================================================================
//function : AttachView
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::AttachView (const Handle(NIS_View)& theView)
{
  if (theView.IsNull() == Standard_False) {
    NCollection_List<Handle(NIS_View)>::Iterator anIter (myViews);
    for (; anIter.More(); anIter.Next())
      if (anIter.Value() == theView)
        break;
    if (anIter.More() == Standard_False) {
      myViews.Append (theView);
      theView->AddContext (this);
      NCollection_Map<Handle(NIS_Drawer)>::Iterator anIterD (myDrawers);
      for (; anIterD.More(); anIterD.Next()) {
        const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
        if (aDrawer.IsNull() == Standard_False)
          aDrawer->myLists.Append(aDrawer->createDefaultList(theView));
      }
    }
  }
}

//=======================================================================
//function : DetachView
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::DetachView (const Handle(NIS_View)& theView)
{
  if (theView.IsNull() == Standard_False) {
    NCollection_List<Handle(NIS_View)>::Iterator anIter (myViews);
    for (; anIter.More(); anIter.Next())
      if (anIter.Value() == theView) {
        myViews.Remove(anIter);
        theView->RemoveContext(this);
        NCollection_Map<Handle(NIS_Drawer)>::Iterator anIterD (myDrawers);
        for (; anIterD.More(); anIterD.Next()) {
          const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
          if (aDrawer.IsNull() == Standard_False) {
            NCollection_List<NIS_DrawList*>::Iterator anIterL(aDrawer->myLists);
            for (; anIterL.More(); anIterL.Next())
              if (anIterL.Value()->GetView() == theView) {
                delete anIterL.Value();
                aDrawer->myLists.Remove (anIterL);
                break;
              }
          }
        }
        break;
      }
  }
}

//=======================================================================
//function : GetObject
//purpose  : 
//=======================================================================

const Handle(NIS_InteractiveObject)& NIS_InteractiveContext::GetObject
                   (const Standard_Integer theID) const
{
  if (!myObjects.IsBound(theID))
  {
    static Handle(NIS_InteractiveObject) aNull;
    return aNull;
  }
  return myObjects(theID);
}

//=======================================================================
//function : redraw
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::redraw (const Handle(NIS_View)&    theView,
                                     const NIS_Drawer::DrawType theType)
{
  NCollection_Map <Handle(NIS_Drawer)>::Iterator anIter (myDrawers);
  for (; anIter.More(); anIter.Next())
    if (anIter.Value().IsNull() == Standard_False)
      anIter.Value()->redraw (theType, theView);
}

//=======================================================================
//function : GetBox
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::GetBox (Bnd_B3f&         theBox,
                                     const NIS_View * theView) const
{
  NCollection_Map <Handle(NIS_Drawer)>::Iterator anIterD (myDrawers);
  for (; anIterD.More(); anIterD.Next())
    if (anIterD.Value().IsNull() == Standard_False)
      theBox.Add(anIterD.Value()->GetBox(theView));
}

//=======================================================================
//function : Display
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::Display
                                (Handle(NIS_InteractiveObject)& theObj,
                                 const Handle(NIS_Drawer)&      theDrawer,
                                 const Standard_Boolean        isUpdateViews)
{
  if (theObj.IsNull())
    return;
  objectForDisplay(theObj, theObj->DrawType());
  const Handle(NIS_Drawer)& aDrawer = drawerForDisplay(theObj, theDrawer);
  // Display Object as Normal or Transparent if it has been hidden
  if (theObj->IsHidden())
    theObj->myIsHidden = Standard_False;

  // Set Update flag in the Drawer
  if (isUpdateViews)
    aDrawer->SetUpdated (theObj->DrawType());
}

//=======================================================================
//function : DisplayOnTop
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::DisplayOnTop
                                (Handle(NIS_InteractiveObject)& theObj,
                                 const Handle(NIS_Drawer)&      theDrawer,
                                 const Standard_Boolean        isUpdateViews)
{
  if (theObj.IsNull())
    return;

  objectForDisplay(theObj, NIS_Drawer::Draw_Top);
  const Handle(NIS_Drawer)& aDrawer = drawerForDisplay(theObj, theDrawer);

  // Display Object as Normal or Transparent if it has been hidden
  if (theObj->IsHidden())
    theObj->myIsHidden = Standard_False;

  // Set Update flag in the Drawer
  if (isUpdateViews)
    aDrawer->SetUpdated (theObj->DrawType());
}

//=======================================================================
//function : Erase
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::Erase (const Handle(NIS_InteractiveObject)& theObj,
                                    const Standard_Boolean isUpdateViews)
{
  if (theObj->IsHidden() == Standard_False) {
    theObj->myIsHidden = Standard_True;
    const Handle(NIS_Drawer)& aDrawer = theObj->GetDrawer();
    if (aDrawer.IsNull() == Standard_False) {
      // Unhilight the erased object
      if (theObj->IsDynHilighted()) {
        NCollection_List<Handle(NIS_View)>::Iterator anIterV (myViews);
        for (; anIterV.More(); anIterV.Next())
          if (anIterV.Value().IsNull() == Standard_False)
            anIterV.Value()->DynamicUnhilight (theObj);
      }
      // Update status of lists
      if (isUpdateViews)
        aDrawer->SetUpdated(theObj->DrawType());
    }
  }
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::Remove (const Handle(NIS_InteractiveObject)& theObj,
                                     const Standard_Boolean isUpdateViews)
{
  if (theObj.IsNull() == Standard_False) {
    const Handle(NIS_Drawer)& aDrawer = theObj->GetDrawer();
    if ( aDrawer.IsNull() )
      return;
    if (aDrawer->myCtx == this) {
      // Remove the hilighting if the object has been hilighted
      if (theObj->IsDynHilighted()) {
        NCollection_List<Handle(NIS_View)>::Iterator anIterV (myViews);
        for (; anIterV.More(); anIterV.Next())
          if (anIterV.Value().IsNull() == Standard_False)
            anIterV.Value()->DynamicUnhilight (theObj);
      }
      // Remove the object from the context
      const Standard_Integer anID = theObj->ID();
      const NIS_Drawer::DrawType aDrawType (theObj->DrawType()); 
      if (myMapObjects[Standard_Integer(aDrawType)].Remove(anID))
        aDrawer->removeObject(theObj.operator->(), isUpdateViews);
      theObj->myID = 0;
      theObj->myDrawer.Nullify();
      myObjects.UnsetValue(anID);
      myMapNonSelectableObjects.Remove(anID);
    }
  }
}

//=======================================================================
//function : DisplayAll
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::DisplayAll ()
{
  // UnHide all objects in the Context
  NCollection_SparseArray <Handle(NIS_InteractiveObject)>::ConstIterator
    anIter(myObjects);
  for (; anIter.More(); anIter.Next()) {
    const Handle(NIS_InteractiveObject)& anObj = anIter.Value();
    if (anObj.IsNull() == Standard_False)
      if (anObj->IsHidden())
        anObj->myIsHidden = Standard_False;
  }

  // Update status of objects in Drawers (particularly cancel dyn. hilighting)
  NCollection_Map<Handle(NIS_Drawer)>::Iterator anIterD (myDrawers);
  for (; anIterD.More(); anIterD.Next()) {
    const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
    if (aDrawer.IsNull() == Standard_False) {
      aDrawer->SetUpdated (NIS_Drawer::Draw_Normal,
                           NIS_Drawer::Draw_Top,
                           NIS_Drawer::Draw_Transparent,
                           NIS_Drawer::Draw_Hilighted);
    }
  }
}

//=======================================================================
//function : EraseAll
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::EraseAll ()
{
  // Hide all objects in the Context
  NCollection_SparseArray <Handle(NIS_InteractiveObject)>::ConstIterator
    anIter(myObjects);
  for (; anIter.More(); anIter.Next()) {
    const Handle(NIS_InteractiveObject)& anObj = anIter.Value();
    if (anObj.IsNull() == Standard_False) {
      if (anObj->IsHidden() == Standard_False)
        anObj->myIsHidden = Standard_True;
      if (anObj->IsDynHilighted()) {
        NCollection_List<Handle(NIS_View)>::Iterator anIterV (myViews);
        for (; anIterV.More(); anIterV.Next())
          if (anIterV.Value().IsNull() == Standard_False)
            anIterV.Value()->DynamicUnhilight (anObj);
      }
    }
  }

  // Update status of objects in Drawers (particularly cancel dyn. hilighting)
  NCollection_Map<Handle(NIS_Drawer)>::Iterator anIterD (myDrawers);
  for (; anIterD.More(); anIterD.Next()) {
    const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
    if (aDrawer.IsNull() == Standard_False) {
      aDrawer->SetUpdated (NIS_Drawer::Draw_Normal,
                           NIS_Drawer::Draw_Top,
                           NIS_Drawer::Draw_Transparent,
                           NIS_Drawer::Draw_Hilighted);
//         if (aList.myDynHilighted.IsEmpty() == Standard_False) {
//           aList.myIsUpdated[NIS_Drawer::Draw_DynHilighted]= Standard_True;
//           aList.myDynHilighted.Clear();
//         }

    }
  }
}

//=======================================================================
//function : RemoveAll
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::RemoveAll ()
{
  // Remove objects from the Context
  NCollection_SparseArray <Handle(NIS_InteractiveObject)>::Iterator
    anIter(myObjects);
  for (; anIter.More(); anIter.Next()) {
    Handle(NIS_InteractiveObject)& anObj = anIter.ChangeValue();
    if (anObj.IsNull() == Standard_False) {
      if (anObj->IsDynHilighted()) {
        NCollection_List<Handle(NIS_View)>::Iterator anIterV (myViews);
        for (; anIterV.More(); anIterV.Next())
          if (anIterV.Value().IsNull() == Standard_False)
            anIterV.Value()->DynamicUnhilight (anObj);
      }
      anObj->myID = 0;
      anObj->myDrawer.Nullify();
      anObj.Nullify();
    }
  }

  // Mark all draw lists to be removed in the view callback
  NCollection_Map<Handle(NIS_Drawer)>::Iterator anIterD (myDrawers);
  for (; anIterD.More(); anIterD.Next()) {
    const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
    if (aDrawer.IsNull() == Standard_False) {
      aDrawer->myMapID.Clear();
      aDrawer->UpdateExListId(NULL);
      aDrawer->myLists.Clear();
    }
  }
  // Remove Drawers
  myDrawers.Clear();

  // Release memory
  myAllocator->Reset();
  myAllocator->ResetCounters();

  // Remove objects from maps
  myMapObjects[0].Clear();
  myMapObjects[1].Clear();
  myMapObjects[2].Clear();
  myMapObjects[3].Clear();
  myMapNonSelectableObjects.Clear();
  myObjects.Clear();
}

//=======================================================================
//function : RebuildViews
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::RebuildViews ()
{
  const Handle(NIS_Allocator) aNewAlloc = compactObjects();

  // Recalculate all DrawLists in all drawers
  markAllDrawersUpdated(myDrawers);

  // It is time to destroy the old allocator, not before this line. Because
  // the old allocator is needed to tidy up draw lists in SetUpdated() calls.
  if (aNewAlloc.IsNull() == Standard_False)
    myAllocator = aNewAlloc;

  NCollection_List<Handle(NIS_View)>::Iterator anIterV(myViews);
  for (; anIterV.More(); anIterV.Next()) {
    const Handle(NIS_View)& aView = anIterV.Value();
    if (aView.IsNull() == Standard_False)
      aView->Redraw();
  }
}

//=======================================================================
//function : UpdateViews
//purpose  : Only repaint the views refreshing their presentations only for
//           those drawers that have been marked as updated.
//=======================================================================

void NIS_InteractiveContext::UpdateViews ()
{
  const Handle(NIS_Allocator) aNewAlloc = compactObjects();
  if (aNewAlloc.IsNull() == Standard_False)
    myAllocator = aNewAlloc;

  NCollection_List<Handle(NIS_View)>::Iterator anIterV(myViews);
  for (; anIterV.More(); anIterV.Next()) {
    const Handle(NIS_View)& aView = anIterV.Value();
    if (aView.IsNull() == Standard_False)
      aView->Redraw();
  }
}

//=======================================================================
//function : SetSelected
//purpose  : 
//=======================================================================

Standard_Boolean NIS_InteractiveContext::SetSelected
                        (const Handle(NIS_InteractiveObject)& theObj,
                         const Standard_Boolean              isSelected)
{
  Standard_Boolean aResult (Standard_False);
  if (theObj.IsNull() == Standard_False) {
    const Standard_Integer anID = theObj->ID();
    if (isSelected == Standard_False) {
      if (myMapObjects[NIS_Drawer::Draw_Hilighted].Remove(anID)) {
        deselectObj (theObj, anID);
        aResult = Standard_True;
      }
    } else {
      if (IsSelectable(anID) == Standard_True) {
        if (myMapObjects[NIS_Drawer::Draw_Hilighted].Add(anID)) {
          selectObj (theObj, anID);
          aResult = Standard_True;
        }
      }
    }
  }
  return aResult;
}

//=======================================================================
//function : ProcessSelection
//purpose  : 
//=======================================================================

Standard_Boolean NIS_InteractiveContext::ProcessSelection
                        (const Handle(NIS_InteractiveObject)& theObj,
                         const Standard_Boolean              isMultiple)
{
  Standard_Boolean aResult (Standard_False);
  Standard_Integer anID (0);
  Standard_Boolean wasSelected (Standard_False);
  if (theObj.IsNull() == Standard_False) {
    const Handle(NIS_Drawer)& aDrawer = theObj->GetDrawer();
    if (aDrawer.IsNull() == Standard_False) {
      if (aDrawer->GetContext() == this) {
        anID = theObj->ID();
        wasSelected = myMapObjects[NIS_Drawer::Draw_Hilighted].Contains (anID);
      }
    }
  }

  switch (mySelectionMode) {
  case Mode_Normal:
  case Mode_Additive:
    if (isMultiple == Standard_False) {
      ClearSelected();
      aResult = Standard_True;
    } else if (wasSelected && mySelectionMode == Mode_Normal) {
      myMapObjects[NIS_Drawer::Draw_Hilighted].Remove( anID );
      deselectObj (theObj, anID);
      aResult = Standard_True;
      break;
    }
    if (wasSelected == Standard_False && IsSelectable(anID) == Standard_True) {
      myMapObjects[NIS_Drawer::Draw_Hilighted].Add( anID );
      selectObj (theObj, anID);
      aResult = Standard_True;
    }
    break;
  case Mode_Exclusive:
    if (wasSelected) {
      myMapObjects[NIS_Drawer::Draw_Hilighted].Remove( anID );
      deselectObj (theObj, anID);
      aResult = Standard_True;
    }
    break;
  default: ;
  }
  return aResult;
}

//=======================================================================
//function : ProcessSelection
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::ProcessSelection
                        (const TColStd_PackedMapOfInteger& mapSel,
                         const Standard_Boolean            isMultiple)
{
  //subtract non-selectable objects
  TColStd_PackedMapOfInteger aMap;
  aMap.Subtraction (mapSel, myMapNonSelectableObjects);

  TColStd_MapIteratorOfPackedMapOfInteger anIter;
  switch (mySelectionMode) {
  case Mode_Normal:
    if (isMultiple == Standard_False) {
      ClearSelected();
      myMapObjects[NIS_Drawer::Draw_Hilighted] = aMap;
      for (anIter.Initialize (aMap); anIter.More(); anIter.Next()) {
        const Standard_Integer anID = anIter.Key();
        selectObj (myObjects(anID), anID);
      }
    } else {
      TColStd_PackedMapOfInteger aMapSub;
      aMapSub.Intersection(aMap, myMapObjects[NIS_Drawer::Draw_Hilighted]);
      aMap.Subtract (myMapObjects[NIS_Drawer::Draw_Hilighted]);
      myMapObjects[NIS_Drawer::Draw_Hilighted].Unite (aMap);
      myMapObjects[NIS_Drawer::Draw_Hilighted].Subtract (aMapSub);
      for (anIter.Initialize (aMap); anIter.More(); anIter.Next()) {
        const Standard_Integer anID = anIter.Key();
        selectObj (myObjects(anID), anID);
      }
      for (anIter.Initialize (aMapSub); anIter.More(); anIter.Next()) {
        const Standard_Integer anID = anIter.Key();
        deselectObj (myObjects(anID), anID);
      }
    }
    break;
  case Mode_Additive:
    aMap.Subtract (myMapObjects[NIS_Drawer::Draw_Hilighted]);
    myMapObjects[NIS_Drawer::Draw_Hilighted].Unite (aMap);
    for (anIter.Initialize (aMap); anIter.More(); anIter.Next()) {
      const Standard_Integer anID = anIter.Key();
      selectObj (myObjects(anID), anID);
    }
    break;
  case Mode_Exclusive:
    aMap.Intersect (myMapObjects[NIS_Drawer::Draw_Hilighted]);
    myMapObjects[NIS_Drawer::Draw_Hilighted].Subtract (aMap);
    for (anIter.Initialize (aMap); anIter.More(); anIter.Next()) {
      const Standard_Integer anID = anIter.Key();
      deselectObj (myObjects(anID), anID);
    }
    break;
  default: ;
  }
}

//=======================================================================
//function : IsSelected
//purpose  : 
//=======================================================================

Standard_Boolean NIS_InteractiveContext::IsSelected
                                (const Handle(NIS_InteractiveObject)& theObj)
{
  Standard_Boolean aResult (Standard_False);
  if (theObj.IsNull() == Standard_False) {
    const Standard_Integer anID = theObj->ID();
    aResult = myMapObjects[NIS_Drawer::Draw_Hilighted].Contains(anID);
  }
  return aResult;
}

//=======================================================================
//function : ClearSelected
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::ClearSelected ()
{
  TColStd_MapIteratorOfPackedMapOfInteger anIter
    (myMapObjects[NIS_Drawer::Draw_Hilighted]);
  for (; anIter.More(); anIter.Next()) {
    const Standard_Integer anID = anIter.Key();
    deselectObj (myObjects(anID), anID);
  }
  myMapObjects[NIS_Drawer::Draw_Hilighted].Clear();
}

//=======================================================================
//function : SetSelected
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::SetSelected
                        (const TColStd_PackedMapOfInteger& mapSel,
                         const Standard_Boolean            isAdded)
{
  //subtract non-selectable objects
  TColStd_PackedMapOfInteger aMap;
  aMap.Subtraction (mapSel, myMapNonSelectableObjects);

  if (aMap.IsEmpty() && isAdded == Standard_False)
    ClearSelected();
  else {
    // Deselect objects
    TColStd_MapIteratorOfPackedMapOfInteger anIter;
    if (isAdded == Standard_False) {
      TColStd_PackedMapOfInteger aMapSub;
      aMapSub.Subtraction (myMapObjects[NIS_Drawer::Draw_Hilighted], aMap);
      for (anIter.Initialize(aMapSub); anIter.More(); anIter.Next()) {
        const Standard_Integer anID = anIter.Key();
        deselectObj (myObjects(anID), anID);
      }
      myMapObjects[NIS_Drawer::Draw_Hilighted].Subtract(aMapSub);
    }
    aMap.Subtract (myMapObjects[NIS_Drawer::Draw_Hilighted]);
    myMapObjects[NIS_Drawer::Draw_Hilighted].Unite (aMap);

    // Select objects
    for (anIter.Initialize (aMap); anIter.More(); anIter.Next()) {
      const Standard_Integer anID = anIter.Key();
      selectObj (myObjects(anID), anID);
    }
  }
}

//=======================================================================
//function : selectObject
//purpose  : 
//=======================================================================

Standard_Real NIS_InteractiveContext::selectObject
                                (Handle(NIS_InteractiveObject)&  theSel,
                                 NCollection_List<DetectedEnt>& theDetected,
                                 const gp_Ax1&                  theAxis,
                                 const Standard_Real            theOver,
                                 const Standard_Boolean         isOnlySel) const
{
  static const Standard_Real anInfiniteDist = 0.5 * RealLast();
  Standard_Real aMinDist(anInfiniteDist);
  if (mySelectionMode != Mode_NoSelection || isOnlySel == Standard_False)
  {
    DetectedEnt anEnt;
    NCollection_SparseArray <Handle(NIS_InteractiveObject)>::ConstIterator
      anIter(myObjects);
    for (; anIter.More(); anIter.Next()) {
      const Handle(NIS_InteractiveObject)& anObj = anIter.Value();
      if (anObj.IsNull() == Standard_False)
        if (anObj->IsDisplayed() &&
            (!myMapNonSelectableObjects.Contains (anObj->ID()) ||
             isOnlySel == Standard_False))
        {
          // Pass the object through the SelectFilter if available
          if (mySelectFilter.IsNull() == Standard_False)
            if (mySelectFilter->IsOk (anObj.operator->()) == Standard_False)
              continue;
          // Check the intersection with the box
          const Bnd_B3f& aBox = anObj->GetBox();
          if (aBox.IsOut (theAxis, Standard_False, theOver) == Standard_False)
          {
            anEnt.Dist = anObj->Intersect (theAxis, theOver);
            if (anEnt.Dist < anInfiniteDist) {
              anEnt.PObj = anObj.operator->();
              // Insert the detected entity in the sorted list
              NCollection_List<DetectedEnt>::Iterator anIterD(theDetected);
              for (; anIterD.More(); anIterD.Next()) {
                if (anEnt.Dist < anIterD.Value().Dist) {
                  theDetected.InsertBefore(anEnt, anIterD);
                  break;
                }
              }
              if (anIterD.More() == Standard_False)
                theDetected.Append(anEnt);
              if (anEnt.Dist < aMinDist) {
                aMinDist = anEnt.Dist;
                theSel = anObj;
              }
            }
          }
        }
    }
  }
  return aMinDist;
}

//=======================================================================
//function : SetSelectable
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::SetSelectable 
                        (const TColStd_PackedMapOfInteger& objIDs,
                         const Standard_Boolean            isSelectable)
{
  if (isSelectable) { // subtract given IDs from myMapNonSelectableObjects
    myMapNonSelectableObjects.Subtract(objIDs);
  }
  else {              // deselect IDs, add them to myMapNonSelectableObjects
    Standard_Integer anID;
    TColStd_MapIteratorOfPackedMapOfInteger anIter(objIDs);
    for (; anIter.More(); anIter.Next()) {
      anID = anIter.Key();
      if ( myMapObjects[NIS_Drawer::Draw_Hilighted].Contains(anID)) {
        myMapObjects[NIS_Drawer::Draw_Hilighted].Remove(anID);
        deselectObj (myObjects(anID), anID);
      }
    }
    myMapNonSelectableObjects.Unite(objIDs);
  }
}

//=======================================================================
//function : selectObjects
//purpose  : 
//=======================================================================

Standard_Boolean NIS_InteractiveContext::selectObjects
                        (TColStd_PackedMapOfInteger& mapObj,
                         const Bnd_B3f&              theBox,
                         const gp_Trsf&              theTrf,
                         const gp_Trsf&              theTrfInv,
                         const Standard_Boolean      isFullyIncluded) const
{
  Standard_Boolean aResult (Standard_False);
  if (mySelectionMode != Mode_NoSelection) {
    NCollection_SparseArray <Handle(NIS_InteractiveObject)>::ConstIterator
      anIter(myObjects);
    for (; anIter.More(); anIter.Next()) {
      const Handle(NIS_InteractiveObject)& anObj = anIter.Value();
      if (anObj.IsNull() == Standard_False)
        if (anObj->IsDisplayed()) {
          // Pass the object through the SelectFilter if available
          if (mySelectFilter.IsNull() == Standard_False)
            if (mySelectFilter->IsOk (anObj.operator->()) == Standard_False)
              continue;
          // Check the intersection with the box
          const Bnd_B3f& aBox = anObj->GetBox();
          if (aBox.IsIn (theBox, theTrf)) {
            mapObj.Add (anObj->ID());
            aResult = Standard_True;
          } else if (aBox.IsOut (theBox, theTrf) == Standard_False)
            if (anObj->Intersect (theBox, theTrfInv, isFullyIncluded))
            {
              mapObj.Add (anObj->ID());
              aResult = Standard_True;
            }
        }
    }
  }
  return aResult;
}

//=======================================================================
//function : selectObjects
//purpose  : 
//=======================================================================

Standard_Boolean NIS_InteractiveContext::selectObjects
                          (TColStd_PackedMapOfInteger    &mapObj,
                           const NCollection_List<gp_XY> &thePolygon,
                           const Bnd_B2f                 &thePolygonBox,
                           const gp_Trsf                 &theTrfInv,
                           const Standard_Boolean         isFullyIn) const
{
  Standard_Boolean aResult (Standard_False);

  if (mySelectionMode != Mode_NoSelection) {
    NCollection_SparseArray <Handle(NIS_InteractiveObject)>::ConstIterator
      anIter(myObjects);

    for (; anIter.More(); anIter.Next()) {
      const Handle(NIS_InteractiveObject)& anObj = anIter.Value();

      if (anObj.IsNull() == Standard_False)
        if (anObj->IsDisplayed()) {
          // Pass the object through the SelectFilter if available
          if (mySelectFilter.IsNull() == Standard_False)
            if (mySelectFilter->IsOk (anObj.operator->()) == Standard_False)
              continue;

          // Comvert 3d box to 2d one
          const Bnd_B3f    &aBox = anObj->GetBox();
          Bnd_B2f           aB2d;
          Standard_Real     aX[2];
          Standard_Real     aY[2];
          Standard_Real     aZ[2];
          gp_XYZ            aBoxVtx;
          Standard_Integer  i;

          aBox.CornerMin().Coord(aX[0], aY[0], aZ[0]);
          aBox.CornerMax().Coord(aX[1], aY[1], aZ[1]);

          for (i = 0; i < 8; i++) {
            aBoxVtx.SetX(aX[(i & 1) ? 1 : 0]);
            aBoxVtx.SetY(aY[(i & 2) ? 1 : 0]);
            aBoxVtx.SetZ(aZ[(i & 4) ? 1 : 0]);
            theTrfInv.Transforms(aBoxVtx);
            aB2d.Add(gp_XY(aBoxVtx.X(), aBoxVtx.Y()));
          }

          // Check the intersection with the box
          if (thePolygonBox.IsOut(aB2d) == Standard_False) {
            if (anObj->Intersect(thePolygon, theTrfInv, isFullyIn)) {
              mapObj.Add (anObj->ID());
              aResult = Standard_True;
            }
          }
        }
    }
  }
  return aResult;
}

//=======================================================================
//function : deselectObj
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::deselectObj
                        (const Handle(NIS_InteractiveObject)& theObj,
                         const Standard_Integer               theID)
{
  if (theObj.IsNull() == Standard_False) {
    const Handle(NIS_Drawer)& aDrawer = theObj->GetDrawer();
    if (theObj->IsTransparent()) {
      myMapObjects[NIS_Drawer::Draw_Transparent].Add(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Transparent);
    } else if (theObj->myBaseType == NIS_Drawer::Draw_Top) {
      myMapObjects[NIS_Drawer::Draw_Top].Add(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Top);
    } else {
      myMapObjects[NIS_Drawer::Draw_Normal].Add(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Normal);
    }
    aDrawer->SetUpdated(NIS_Drawer::Draw_Hilighted);
    theObj->myDrawType = theObj->myBaseType;
  }
}

//=======================================================================
//function : selectObj
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::selectObj
                        (const Handle(NIS_InteractiveObject)& theObj,
                         const Standard_Integer               theID)
{
  if (theObj.IsNull() == Standard_False) {
    const Handle(NIS_Drawer)& aDrawer = theObj->GetDrawer();
    if (theObj->IsTransparent()) {
      myMapObjects[NIS_Drawer::Draw_Transparent].Remove(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Transparent);
    } else if (theObj->myDrawType == NIS_Drawer::Draw_Top) {
      myMapObjects[NIS_Drawer::Draw_Top].Remove(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Top);
    } else {
      myMapObjects[NIS_Drawer::Draw_Normal].Remove(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Normal);
    }
    aDrawer->SetUpdated(NIS_Drawer::Draw_Hilighted);
    theObj->myDrawType = NIS_Drawer::Draw_Hilighted;
  }
}

//=======================================================================
//function : drawerForDisplay
//purpose  : 
//=======================================================================

const Handle(NIS_Drawer)& NIS_InteractiveContext::drawerForDisplay
                                (const Handle(NIS_InteractiveObject)& theObj,
                                 const Handle(NIS_Drawer)&            theDrawer)
{
  Handle(NIS_Drawer) aDrawer;
  if (theDrawer.IsNull() == Standard_False) {
    if (theDrawer->myCtx != this)
      Standard_NoSuchObject::Raise ("NIS_InteractiveContext::Display (0)");
    aDrawer = theDrawer;
  } else {
    const Handle(NIS_Drawer)& anObjDrawer = theObj->GetDrawer();
    if (anObjDrawer.IsNull() == Standard_False)
      return anObjDrawer;
    aDrawer = theObj->DefaultDrawer(0L);
    aDrawer->myCtx = this;
  }
  return theObj->SetDrawer (aDrawer, Standard_False);
}

//=======================================================================
//function : objectIdForDisplay
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::objectForDisplay
                                (Handle(NIS_InteractiveObject)& theObj,
                                 const NIS_Drawer::DrawType    theDrawType)
{
  if (theObj->ID() == 0) {
    // Create a new ID for this object
    Handle(NIS_InteractiveObject) anObj;
    theObj->Clone(myAllocator, anObj);
    theObj = anObj;
    anObj->myID = ++myLastObjectId;
    myObjects.SetValue (myLastObjectId, anObj);
    myMapObjects[theDrawType].Add(anObj->myID);
    anObj->myDrawType = theDrawType;
  }
}

//=======================================================================
//function : compactObjects
//purpose  : 
//=======================================================================

Handle(NIS_Allocator) NIS_InteractiveContext::compactObjects()
{
  Handle(NIS_Allocator) aNewAlloc;

  NCollection_List<Handle(NIS_View)>::Iterator anIterV;
  // Check if the memory used by objects has to be compacted.
  const Standard_Size nAllocated = myAllocator->NAllocated();

  if (nAllocated > 1024*1024) {
    const Standard_Size nFreed = myAllocator->NFreed();
    if ((nFreed * 5) / 3 > nAllocated || nFreed > 20*1024*1024)
    {
      for (anIterV.Init(myViews); anIterV.More(); anIterV.Next()) {
        const Handle(NIS_View)& aView = anIterV.Value();
        if (aView.IsNull() == Standard_False) {
          aView->myDynHilighted.Nullify();
          aView->GetDetected().Clear();
        }
      }
      // Compact the memory: clone all objects to a new allocator, release
      // the old allocator instance.
      aNewAlloc = new NIS_Allocator;
      NCollection_SparseArray<Handle(NIS_InteractiveObject)>::Iterator
        anIter(myObjects);
      for (; anIter.More(); anIter.Next()) {
        if (anIter.Value().IsNull() == Standard_False) {
          Handle(NIS_InteractiveObject)& aNewObj = anIter.ChangeValue();
          const Handle(NIS_InteractiveObject) anObj = aNewObj;
          aNewObj.Nullify();
          anObj->CloneWithID(aNewAlloc, aNewObj);
        }
      }
    }
  }
  return aNewAlloc;
}

//=======================================================================
//function : markAllDrawersUpdated
//purpose  : 
//=======================================================================

void markAllDrawersUpdated (const NCollection_Map<Handle(NIS_Drawer)>& lstDrv)
{
  NCollection_Map<Handle(NIS_Drawer)>::Iterator anIterD (lstDrv);
  for (; anIterD.More(); anIterD.Next()) {
    const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
    if (aDrawer.IsNull() == Standard_False) {
      aDrawer->SetUpdated (NIS_Drawer::Draw_Normal,
                           NIS_Drawer::Draw_Top,
                           NIS_Drawer::Draw_Transparent,
                           NIS_Drawer::Draw_Hilighted);
    }
  }
}
