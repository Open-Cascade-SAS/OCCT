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

#include <NIS_Drawer.hxx>
#include <NIS_View.hxx>
#include <NIS_InteractiveContext.hxx>
#include <NIS_InteractiveObject.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_NoSuchObject.hxx>

IMPLEMENT_STANDARD_HANDLE  (NIS_Drawer, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT (NIS_Drawer, Standard_Transient)

//=======================================================================
//function : NIS_Drawer
//purpose  : Destructor
//=======================================================================

NIS_Drawer::~NIS_Drawer ()
{
  NCollection_List<NIS_DrawList*>::Iterator anIter (myLists);
  for (; anIter.More(); anIter.Next())
    delete anIter.Value();
}

//=======================================================================
//function : Assign
//purpose  : 
//=======================================================================

void NIS_Drawer::Assign (const Handle(NIS_Drawer)& theOther)
{
  if (theOther->IsKind(DynamicType()) == Standard_False)
    Standard_TypeMismatch::Raise ("NIS_Drawer::Assign");
  myIniId        = theOther->myIniId;
  myObjPerDrawer = theOther->myObjPerDrawer;
  myTransparency = theOther->myTransparency;
}

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

Standard_Integer NIS_Drawer::HashCode(const Standard_Integer theN) const
{
  Standard_Integer aKey = ::HashCode (DynamicType(), theN);
  aKey += (myIniId / myObjPerDrawer);
  return ((aKey & 0x7fffffff) % theN) + 1;
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean NIS_Drawer::IsEqual (const Handle(NIS_Drawer)& theOther) const
{
  Standard_Boolean aResult (Standard_False);
  if (theOther.IsNull() == Standard_False)
    if (DynamicType() == theOther->DynamicType())
      if (theOther->myIniId/theOther->myObjPerDrawer == myIniId/myObjPerDrawer)
        aResult = Standard_True;

  if (aResult)
    if (fabs(myTransparency - theOther->myTransparency) > 0.01)
      aResult = Standard_False;

  return aResult;
}

//=======================================================================
//function : BeforeDraw
//purpose  : Called before Draw(), once per group of interactive objects.
//=======================================================================

void NIS_Drawer::BeforeDraw (const DrawType, const NIS_DrawList&)
{
}

//=======================================================================
//function : AfterDraw
//purpose  : Called after Draw(), once per group of interactive objects.
//=======================================================================

void NIS_Drawer::AfterDraw (const DrawType, const NIS_DrawList&)
{
}

//=======================================================================
//function : UpdateExListId
//purpose  : 
//=======================================================================

void NIS_Drawer::UpdateExListId (const Handle(NIS_View)& theView) const
{
  if (theView.IsNull()) {
    if (myCtx) {
      if (myCtx->myViews.IsEmpty() == Standard_False) {
        const Handle(NIS_View)& aView = myCtx->myViews.First();
        NCollection_List<NIS_DrawList *>::Iterator anIterL(myLists);
        for (; anIterL.More(); anIterL.Next()) {
          NIS_DrawList * const pList = anIterL.Value();
          pList->ClearListID(aView);
        }
      }
    }
  } else {
    NCollection_List<NIS_DrawList *>::Iterator anIterL(myLists);
    for (; anIterL.More(); anIterL.Next()) {
      NIS_DrawList * const pList = anIterL.Value();
      if (pList->GetView() == theView) {       
        pList->ClearListID(theView);
        break;
      }
    }
  }
}

//=======================================================================
//function : redraw
//purpose  : 
//=======================================================================

void NIS_Drawer::redraw (const DrawType           theType,
                         const Handle(NIS_View)&   theView)
{
  if (myCtx &&
      myMapID.IsEmpty() == Standard_False &&
      theView.IsNull() == Standard_False)
  {
    NCollection_List<NIS_DrawList*>::Iterator anIter (myLists);
    for (; anIter.More(); anIter.Next()) {
      NIS_DrawList& aDrawList = * anIter.ChangeValue();
      const Handle(NIS_View)& aView = aDrawList.GetView();
      if (aView == theView || aView.IsNull()) {
        if (aDrawList.IsUpdated(theType)) {
          // Get the IDs of objects concerned
          TColStd_PackedMapOfInteger mapObj;
          mapObj.Intersection (myCtx->myMapObjects[theType], myMapID);
#ifndef ARRAY_LISTS
          // Release the list that is no more in use
          if (mapObj.IsEmpty() && theType != Draw_DynHilighted) {
            aDrawList.ClearListID(theType);
            break;
          }
#endif
          aDrawList.BeginPrepare(theType);
          prepareList (theType, aDrawList, mapObj);
          aDrawList.EndPrepare(theType);
        }
        if (aDrawList.GetListID(theType) > 0)
          aDrawList.Call(theType);
        break;
      }
    }
  }
}

//=======================================================================
//function : SetUpdated
//purpose  : 
//=======================================================================

void NIS_Drawer::SetUpdated (const DrawType theType) const
{
  NCollection_List<NIS_DrawList*>::Iterator anIter (myLists);
  for (; anIter.More(); anIter.Next()) {
    NIS_DrawList& aDrawList = * anIter.ChangeValue();
    aDrawList.SetUpdated (theType);
  }
  const_cast<Bnd_B3f&>(myBox).Clear();
}

//=======================================================================
//function : SetUpdated
//purpose  : 
//=======================================================================

void NIS_Drawer::SetUpdated (const DrawType theType1,
                             const DrawType theType2) const
{
  NCollection_List<NIS_DrawList*>::Iterator anIter (myLists);
  for (; anIter.More(); anIter.Next()) {
    NIS_DrawList& aDrawList = * anIter.ChangeValue();
    aDrawList.SetUpdated (theType1);
    aDrawList.SetUpdated (theType2);
  }
  const_cast<Bnd_B3f&>(myBox).Clear();
}

//=======================================================================
//function : SetUpdated
//purpose  : 
//=======================================================================


void NIS_Drawer::SetUpdated (const DrawType theType1,
                             const DrawType theType2,
                             const DrawType theType3) const
{
  NCollection_List<NIS_DrawList*>::Iterator anIter (myLists);
  for (; anIter.More(); anIter.Next()) {
    NIS_DrawList& aDrawList = * anIter.ChangeValue();
    aDrawList.SetUpdated (theType1);
    aDrawList.SetUpdated (theType2);
    aDrawList.SetUpdated (theType3);
  }
  const_cast<Bnd_B3f&>(myBox).Clear();
}

//=======================================================================
//function : SetUpdated
//purpose  : 
//=======================================================================


void NIS_Drawer::SetUpdated (const DrawType theType1,
                             const DrawType theType2,
                             const DrawType theType3,
                             const DrawType theType4) const
{
  NCollection_List<NIS_DrawList*>::Iterator anIter (myLists);
  for (; anIter.More(); anIter.Next()) {
    NIS_DrawList& aDrawList = * anIter.ChangeValue();
    aDrawList.SetUpdated (theType1);
    aDrawList.SetUpdated (theType2);
    aDrawList.SetUpdated (theType3);
    aDrawList.SetUpdated (theType4);
  }
  const_cast<Bnd_B3f&>(myBox).Clear();
}

//=======================================================================
//function : SetDynamicHilighted
//purpose  : 
//=======================================================================

void NIS_Drawer::SetDynamicHilighted
                        (const Standard_Boolean              isHilighted,
                         const Handle(NIS_InteractiveObject)& theObj,
                         const Handle(NIS_View)&              theView)
{
  if (myCtx && theObj.IsNull() == Standard_False) {
    NCollection_List<NIS_DrawList*>::Iterator anIter (myLists);
    if (theView.IsNull()) {
      for (; anIter.More(); anIter.Next()) {
        NIS_DrawList& aDrawList = * anIter.ChangeValue();
        aDrawList.SetDynHilighted (isHilighted, theObj);
        aDrawList.SetUpdated (Draw_DynHilighted);
      }
      theObj->myIsDynHilighted = isHilighted;
    } else
      for (; anIter.More(); anIter.Next()) {
        NIS_DrawList& aDrawList = * anIter.ChangeValue();
        const Handle(NIS_View)& aView = aDrawList.GetView();
        if (aView == theView || aView.IsNull()) {
          aDrawList.SetDynHilighted (isHilighted, theObj);
          theObj->myIsDynHilighted = isHilighted;
          aDrawList.SetUpdated (Draw_DynHilighted);
        }
      }
  }
}

//=======================================================================
//function : removeObject
//purpose  : 
//=======================================================================

void NIS_Drawer::removeObject (const NIS_InteractiveObject * theObj,
                               const Standard_Boolean      isUpdateViews)
{
  const Standard_Integer anID = theObj->ID();
  myMapID.Remove (anID);
  // Stop dynamic hilighting if it has been activated
  if (theObj->IsDynHilighted())
    SetDynamicHilighted (Standard_False, theObj);
  if (myMapID.IsEmpty()) {
    UpdateExListId(NULL);
    // Remove the drawer from context.
    myCtx->myDrawers.Remove(this);
  }
  // Set Updated for the draw type.
  else if (theObj->IsHidden() == Standard_False && isUpdateViews)
    SetUpdated (theObj->DrawType());
}

//=======================================================================
//function : addObject
//purpose  : 
//=======================================================================

void NIS_Drawer::addObject (const NIS_InteractiveObject * theObj,
                            const Standard_Boolean        isShareList,
                            const Standard_Boolean        isUpdateViews)
{
  myMapID.Add (theObj->ID());

  // Fill the drawer (if new) with DrawList instances for available Views.
  if ( myLists.IsEmpty())
  {
    if (isShareList)
      myLists.Append (createDefaultList(NULL));
    else {
      NCollection_List<Handle(NIS_View)>::Iterator anIter(GetContext()->myViews);
      for (; anIter.More(); anIter.Next())
        myLists.Append (createDefaultList(anIter.Value()));
    }
  }

  if (theObj->IsHidden() == Standard_False && isUpdateViews)
    SetUpdated (theObj->DrawType());
}

//=======================================================================
//function : GetBox
//purpose  : 
//=======================================================================

const Bnd_B3f& NIS_Drawer::GetBox (const NIS_View * pView) const
{
  if (myBox.IsVoid()) {
    if (myCtx) {
      TColStd_MapIteratorOfPackedMapOfInteger anIter;
      if (pView == 0L)
        anIter.Initialize (myMapID);
      else {
        NCollection_List<NIS_DrawList*>::Iterator anIterL (myLists);
        for (; anIterL.More(); anIterL.Next()) {
          NIS_DrawList& aDrawList = * anIterL.ChangeValue();
          const Handle(NIS_View)& aView = aDrawList.GetView();
          if (aView.IsNull() || aView.operator->() == pView)
            break;
        }
        if (anIterL.More())
          anIter.Initialize (myMapID);
      }

      for (; anIter.More(); anIter.Next()) {
        const Handle(NIS_InteractiveObject)& anObj =
          myCtx->GetObject(anIter.Key());
        if (anObj.IsNull() == Standard_False)
          if (anObj->IsHidden() == Standard_False)
            const_cast<Bnd_B3f&>(myBox).Add (anObj->GetBox());
      }
    }
  }
  return myBox;
}


//=======================================================================
//function : prepareList
//purpose  : 
//=======================================================================
void NIS_Drawer::prepareList(const NIS_Drawer::DrawType         theType,
                             const NIS_DrawList&                theDrawList,
                             const TColStd_PackedMapOfInteger&  mapObj)
{
  if (!myCtx)
    return;

  // Dynamic hilighting is treated in a separate loop because the hilighted
  // instances are contained in the special list rather than in the Context
  if (theType == NIS_Drawer::Draw_DynHilighted) {
    NCollection_List<Handle(NIS_InteractiveObject)>::Iterator
      anIter (theDrawList.DynHilightedList());
    if (anIter.More()) {
      BeforeDraw (theType, theDrawList);
      for (; anIter.More(); anIter.Next())
        Draw (anIter.Value(), NIS_Drawer::Draw_DynHilighted, theDrawList);
      AfterDraw (theType, theDrawList);
    }
  } else {
    // The common part of two maps (objects for this draw type & objects in
    // the current Drawer) is used for updating the presentation.
    TColStd_MapIteratorOfPackedMapOfInteger anIter (mapObj);
    if (anIter.More()) {
      BeforeDraw (theType, theDrawList);
      for (; anIter.More(); anIter.Next()) {
        const Handle(NIS_InteractiveObject)& anObj =
          myCtx->GetObject(anIter.Key());
        if (anObj.IsNull() == Standard_False)
          if (anObj->IsHidden() == Standard_False)
            Draw (anObj, theType, theDrawList);
      }
      AfterDraw (theType, theDrawList);
    }
  }
}

//=======================================================================
//function : createDefaultList
//purpose  : 
//=======================================================================
NIS_DrawList* NIS_Drawer::createDefaultList
                        (const Handle(NIS_View)& theView) const
{
  return new NIS_DrawList(theView);
}
