// File:      NIS_InteractiveContext.cxx
// Created:   06.07.07 20:52
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007

#include <NIS_InteractiveContext.hxx>
#include <NIS_InteractiveObject.hxx>
#include <NIS_View.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <Standard_NoSuchObject.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_InteractiveContext, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT (NIS_InteractiveContext, Standard_Transient)

static void deselectObj (const Handle(NIS_InteractiveObject)&,
                         const Standard_Integer,
                         TColStd_PackedMapOfInteger *);
static void selectObj   (const Handle(NIS_InteractiveObject)&,
                         const Standard_Integer,
                         TColStd_PackedMapOfInteger *);

//=======================================================================
//function : NIS_InteractiveContext()
//purpose  : Constructor
//=======================================================================

NIS_InteractiveContext::NIS_InteractiveContext ()
  : mySelectionMode (Mode_NoSelection),
    myAllocator     (new NCollection_IncAllocator)
{
  // ID == 0 is invalid so we reserve this item from subsequent allocation.
  myObjects.Append (NULL);
}

//=======================================================================
//function : ~NIS_InteractiveContext
//purpose  : Destructor
//=======================================================================

NIS_InteractiveContext::~NIS_InteractiveContext ()
{
  // Unregister this context in all referred views
  NCollection_List<Handle_NIS_View>::Iterator anIterV (myViews);
  for (; anIterV.More(); anIterV.Next())
    if (anIterV.Value().IsNull() == Standard_False)
      anIterV.Value()->RemoveContext(this);

//   // Unregister this context in all referred drawers
//   NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD (myDrawers);
//   for (; anIterD.More(); anIterD.Next())
//     if (anIterD.Value().IsNull() == Standard_False)
//       anIterD.Value()->myCtx = 0L;
}


//=======================================================================
//function : AttachView
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::AttachView (const Handle_NIS_View& theView)
{
  if (theView.IsNull() == Standard_False) {
    NCollection_List<Handle_NIS_View>::Iterator anIter (myViews);
    for (; anIter.More(); anIter.Next())
      if (anIter.Value() == theView)
        break;
    if (anIter.More() == Standard_False) {
      myViews.Append (theView);
      theView->AddContext (this);
      NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD (myDrawers);
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

void NIS_InteractiveContext::DetachView (const Handle_NIS_View& theView)
{
  if (theView.IsNull() == Standard_False) {
    NCollection_List<Handle_NIS_View>::Iterator anIter (myViews);
    for (; anIter.More(); anIter.Next())
      if (anIter.Value() == theView) {
        myViews.Remove(anIter);
        theView->RemoveContext(this);
        NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD (myDrawers);
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
//function : redraw
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::redraw (const Handle(NIS_View)&    theView,
                                     const NIS_Drawer::DrawType theType)
{
  NCollection_Map <Handle_NIS_Drawer>::Iterator anIter (myDrawers);
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
  NCollection_Map <Handle_NIS_Drawer>::Iterator anIterD (myDrawers);
  for (; anIterD.More(); anIterD.Next())
    if (anIterD.Value().IsNull() == Standard_False)
      theBox.Add(anIterD.Value()->GetBox(theView));
}

//=======================================================================
//function : Display
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::Display
                                (const Handle_NIS_InteractiveObject& theObj,
                                 const Handle_NIS_Drawer&            theDrawer,
                                 const Standard_Boolean           isUpdateViews)
{
  if (theObj.IsNull())
    return;
  Standard_Integer anID = theObj->ID();
  Handle(NIS_Drawer) aDrawer = theDrawer;
  if (aDrawer.IsNull() == Standard_False) {
    if (aDrawer->myCtx != this)
      Standard_NoSuchObject::Raise ("NIS_InteractiveContext::Display (0)");
  } else {
    aDrawer = theObj->GetDrawer();
    if (aDrawer.IsNull()) {
      aDrawer = theObj->DefaultDrawer();
      aDrawer->myCtx = this;
    }
  }
  if (anID == 0) {
    // Create a new ID for this object
    theObj->myID = myObjects.Length();
    myObjects.Append (theObj);
    myMapObjects[NIS_Drawer::Draw_Normal].Add(theObj->myID);
  }
  aDrawer = theObj->SetDrawer (aDrawer);

    // Display Object as Normal or Transparent if it has been hidden
  if (theObj->IsHidden())
    theObj->myIsHidden = Standard_False;
//       if (theObj->IsTransparent()) {
//         myMapObjects[NIS_Drawer::Draw_Transparent].Add(anID);
//         theObj->myDrawType = NIS_Drawer::Draw_Transparent;
//       } else {
//         myMapObjects[NIS_Drawer::Draw_Normal].Add(anID);
//         theObj->myDrawType = NIS_Drawer::Draw_Normal;
//       }

  // Set Update flag in the Drawer
  if (isUpdateViews)
    aDrawer->SetUpdated (theObj->DrawType());
}

//=======================================================================
//function : Erase
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::Erase (const Handle_NIS_InteractiveObject& theObj,
                                    const Standard_Boolean isUpdateViews)
{
  if (theObj->IsHidden() == Standard_False) {
    theObj->myIsHidden = Standard_True;
    const Handle(NIS_Drawer)& aDrawer = theObj->GetDrawer();
    if (aDrawer.IsNull() == Standard_False) {
      // Unhilight the erased object
      if (theObj->IsDynHilighted()) {
        NCollection_List<Handle_NIS_View>::Iterator anIterV (myViews);
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

void NIS_InteractiveContext::Remove (const Handle_NIS_InteractiveObject& theObj,
                                     const Standard_Boolean isUpdateViews)
{
  if (theObj.IsNull() == Standard_False) {
    const Handle(NIS_Drawer)& aDrawer = theObj->GetDrawer();
    if (aDrawer->myCtx == this) {
      // Remove the hilighting if the object has been hilighted
      if (theObj->IsDynHilighted()) {
        NCollection_List<Handle_NIS_View>::Iterator anIterV (myViews);
        for (; anIterV.More(); anIterV.Next())
          if (anIterV.Value().IsNull() == Standard_False)
            anIterV.Value()->DynamicUnhilight (theObj);
      }
      // Remove the obejct from the context
      const Standard_Integer anID = theObj->ID();
      const NIS_Drawer::DrawType aDrawType (theObj->DrawType()); 
      if (myMapObjects[Standard_Integer(aDrawType)&0x3].Remove(anID))
        aDrawer->removeObject(theObj.operator->(), isUpdateViews);
      theObj->myID = 0;
      theObj->myDrawer.Nullify();
      myObjects(anID).Nullify();
    }
  }
}

//=======================================================================
//function : DisplayAll
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::DisplayAll (const Standard_Boolean isUpdateViews)
{
  // UnHide all objects in the Context
  NCollection_Vector <Handle_NIS_InteractiveObject>::Iterator anIter(myObjects);
  for (; anIter.More(); anIter.Next()) {
    const Handle(NIS_InteractiveObject)& anObj = anIter.Value();
    if (anObj.IsNull() == Standard_False)
      if (anObj->IsHidden())
        anObj->myIsHidden = Standard_False;
  }

  // Update status of objects in Drawers (particularly cancel dyn. hilighting)
  if (isUpdateViews) {
    NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD (myDrawers);
    for (; anIterD.More(); anIterD.Next()) {
      const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
      if (aDrawer.IsNull() == Standard_False) {
        aDrawer->SetUpdated (NIS_Drawer::Draw_Normal,
                             NIS_Drawer::Draw_Transparent,
                             NIS_Drawer::Draw_Hilighted);
      }
    }
  }
}

//=======================================================================
//function : EraseAll
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::EraseAll (const Standard_Boolean isUpdateViews)
{
  // Hide all objects in the Context
  NCollection_Vector <Handle_NIS_InteractiveObject>::Iterator anIter(myObjects);
  for (; anIter.More(); anIter.Next()) {
    const Handle(NIS_InteractiveObject)& anObj = anIter.Value();
    if (anObj.IsNull() == Standard_False) {
      if (anObj->IsHidden() == Standard_False)
        anObj->myIsHidden = Standard_True;
      if (anObj->IsDynHilighted()) {
        NCollection_List<Handle_NIS_View>::Iterator anIterV (myViews);
        for (; anIterV.More(); anIterV.Next())
          if (anIterV.Value().IsNull() == Standard_False)
            anIterV.Value()->DynamicUnhilight (anObj);
      }
    }
  }

  // Update status of objects in Drawers (particularly cancel dyn. hilighting)
  NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD (myDrawers);
  for (; anIterD.More(); anIterD.Next()) {
    const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
    if (aDrawer.IsNull() == Standard_False) {
      if (isUpdateViews)
        aDrawer->SetUpdated (NIS_Drawer::Draw_Normal,
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

void NIS_InteractiveContext::RemoveAll (const Standard_Boolean isUpdateViews)
{
  // Remove objects from the Context
  NCollection_Vector <Handle_NIS_InteractiveObject>::Iterator anIter(myObjects);
  for (; anIter.More(); anIter.Next()) {
    Handle(NIS_InteractiveObject)& anObj = anIter.ChangeValue();
    if (anObj.IsNull() == Standard_False) {
      if (anObj->IsDynHilighted()) {
        NCollection_List<Handle_NIS_View>::Iterator anIterV (myViews);
        for (; anIterV.More(); anIterV.Next())
          if (anIterV.Value().IsNull() == Standard_False)
            anIterV.Value()->DynamicUnhilight (anObj);
      }
      anObj->myID = 0;
      anObj->myDrawer.Nullify();
      anObj.Nullify();
    }
  }

  // Remove objects from Drawers (particularly cancel dynamic hilighting)
  NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD (myDrawers);
  for (; anIterD.More(); anIterD.Next()) {
    const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
    if (aDrawer.IsNull() == Standard_False) {
      aDrawer->myMapID.Clear();
      if (isUpdateViews)
        aDrawer->SetUpdated (NIS_Drawer::Draw_Normal,
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
//function : UpdateViews
//purpose  : 
//=======================================================================

void NIS_InteractiveContext::UpdateViews ()
{
  NCollection_Map<Handle_NIS_Drawer>::Iterator anIterD (myDrawers);
  for (; anIterD.More(); anIterD.Next()) {
    const Handle(NIS_Drawer)& aDrawer = anIterD.Value();
    if (aDrawer.IsNull() == Standard_False) {
      aDrawer->SetUpdated (NIS_Drawer::Draw_Normal,
                           NIS_Drawer::Draw_Transparent,
                           NIS_Drawer::Draw_Hilighted);
//         aList.myIsUpdated[NIS_Drawer::Draw_DynHilighted]  =
//           (aList.myDynHilighted.IsEmpty() == Standard_False);
    }
  }
  NCollection_List<Handle_NIS_View>::Iterator anIterV (myViews);
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
                        (const Handle_NIS_InteractiveObject& theObj,
                         const Standard_Boolean              isSelected)
{
  Standard_Boolean aResult (Standard_False);
  if (theObj.IsNull() == Standard_False) {
    const Standard_Integer anID = theObj->ID();
    if (isSelected == Standard_False) {
      if (myMapObjects[NIS_Drawer::Draw_Hilighted].Remove(anID)) {
        deselectObj (theObj, anID, &myMapObjects[0]);
        aResult = Standard_True;
      }
    } else {
      if (IsSelectable(anID) == Standard_True) {
        if (myMapObjects[NIS_Drawer::Draw_Hilighted].Add(anID)) {
          selectObj (theObj, anID, &myMapObjects[0]);
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
                        (const Handle_NIS_InteractiveObject& theObj,
                         const Standard_Boolean              isMultiple)
{
  Standard_Boolean aResult (Standard_False);
  Standard_Integer anID (0);
  Standard_Boolean wasSelected (Standard_False);
  if (theObj.IsNull() == Standard_False) {
    anID = theObj->ID();
    wasSelected = myMapObjects[NIS_Drawer::Draw_Hilighted].Contains (anID);
  }

  switch (mySelectionMode) {
  case Mode_Normal:
  case Mode_Additive:
    if (isMultiple == Standard_False) {
      ClearSelected();
      aResult = Standard_True;
    } else if (wasSelected && mySelectionMode == Mode_Normal) {
      myMapObjects[NIS_Drawer::Draw_Hilighted].Remove( anID );
      deselectObj (theObj, anID, &myMapObjects[0]);
      aResult = Standard_True;
      break;
    }
    if (wasSelected == Standard_False && IsSelectable(anID) == Standard_True) {
      myMapObjects[NIS_Drawer::Draw_Hilighted].Add( anID );
      selectObj (theObj, anID, &myMapObjects[0]);
      aResult = Standard_True;
    }
    break;
  case Mode_Exclusive:
    if (wasSelected) {
      myMapObjects[NIS_Drawer::Draw_Hilighted].Remove( anID );
      deselectObj (theObj, anID, &myMapObjects[0]);
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
        selectObj (myObjects(anID), anID, &myMapObjects[0]);
      }
    } else {
      TColStd_PackedMapOfInteger aMapSub;
      aMapSub.Intersection(aMap, myMapObjects[NIS_Drawer::Draw_Hilighted]);
      aMap.Subtract (myMapObjects[NIS_Drawer::Draw_Hilighted]);
      myMapObjects[NIS_Drawer::Draw_Hilighted].Unite (aMap);
      myMapObjects[NIS_Drawer::Draw_Hilighted].Subtract (aMapSub);
      for (anIter.Initialize (aMap); anIter.More(); anIter.Next()) {
        const Standard_Integer anID = anIter.Key();
        selectObj (myObjects(anID), anID, &myMapObjects[0]);
      }
      for (anIter.Initialize (aMapSub); anIter.More(); anIter.Next()) {
        const Standard_Integer anID = anIter.Key();
        deselectObj (myObjects(anID), anID, &myMapObjects[0]);
      }
    }
    break;
  case Mode_Additive:
    aMap.Subtract (myMapObjects[NIS_Drawer::Draw_Hilighted]);
    myMapObjects[NIS_Drawer::Draw_Hilighted].Unite (aMap);
    for (anIter.Initialize (aMap); anIter.More(); anIter.Next()) {
      const Standard_Integer anID = anIter.Key();
      selectObj (myObjects(anID), anID, &myMapObjects[0]);
    }
    break;
  case Mode_Exclusive:
    aMap.Intersect (myMapObjects[NIS_Drawer::Draw_Hilighted]);
    myMapObjects[NIS_Drawer::Draw_Hilighted].Subtract (aMap);
    for (anIter.Initialize (aMap); anIter.More(); anIter.Next()) {
      const Standard_Integer anID = anIter.Key();
      deselectObj (myObjects(anID), anID, &myMapObjects[0]);
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
                                (const Handle_NIS_InteractiveObject& theObj)
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
    deselectObj (myObjects(anID), anID, &myMapObjects[0]);
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
        deselectObj (myObjects(anID), anID, &myMapObjects[0]);
      }
      myMapObjects[NIS_Drawer::Draw_Hilighted].Subtract(aMapSub);
    }
    aMap.Subtract (myMapObjects[NIS_Drawer::Draw_Hilighted]);
    myMapObjects[NIS_Drawer::Draw_Hilighted].Unite (aMap);

    // Select objects
    for (anIter.Initialize (aMap); anIter.More(); anIter.Next()) {
      const Standard_Integer anID = anIter.Key();
      selectObj (myObjects(anID), anID, &myMapObjects[0]);
    }
  }
}

//=======================================================================
//function : selectObject
//purpose  : 
//=======================================================================

Standard_Real NIS_InteractiveContext::selectObject
                                (Handle_NIS_InteractiveObject& theSel,
                                 const gp_Ax1&                 theAxis,
                                 const Standard_Real           theOver,
                                 const Standard_Boolean        isOnlySel) const
{
  Standard_Real aResult (0.5 * RealLast());
  if (mySelectionMode != Mode_NoSelection || isOnlySel == Standard_False)
  {
    NCollection_Vector <Handle_NIS_InteractiveObject>::Iterator
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
            const Standard_Real aDist = anObj->Intersect (theAxis, theOver);
            if (aDist < aResult) {
              aResult = aDist;
              theSel = anObj;
            }
          }
        }
    }
  }
  return aResult;
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
        deselectObj (myObjects(anID), anID, &myMapObjects[0]);
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
    NCollection_Vector <Handle_NIS_InteractiveObject>::Iterator
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
//function : deselectObj
//purpose  : 
//=======================================================================

void deselectObj (const Handle(NIS_InteractiveObject)& theObj,
                  const Standard_Integer               theID,
                  TColStd_PackedMapOfInteger         * mapObjects)
{
  if (theObj.IsNull() == Standard_False) {
    const Handle(NIS_Drawer)& aDrawer = theObj->GetDrawer();
    if (theObj->IsTransparent()) {
      mapObjects[NIS_Drawer::Draw_Transparent].Add(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Transparent);
    } else {
      mapObjects[NIS_Drawer::Draw_Normal].Add(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Normal);
    }
    aDrawer->SetUpdated(NIS_Drawer::Draw_Hilighted);
  }
}

//=======================================================================
//function : selectObj
//purpose  : 
//=======================================================================

void selectObj (const Handle(NIS_InteractiveObject)& theObj,
                const Standard_Integer               theID,
                TColStd_PackedMapOfInteger         * mapObjects)
{
  if (theObj.IsNull() == Standard_False) {
    const Handle(NIS_Drawer)& aDrawer = theObj->GetDrawer();
    if (theObj->IsTransparent()) {
      mapObjects[NIS_Drawer::Draw_Transparent].Remove(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Transparent);
    } else {
      mapObjects[NIS_Drawer::Draw_Normal].Remove(theID);
      aDrawer->SetUpdated(NIS_Drawer::Draw_Normal);
    }
    aDrawer->SetUpdated(NIS_Drawer::Draw_Hilighted);
  }
}

