// Created on: 1997-01-17
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
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

// Modified by  XAB & Serguei Dec 97 (angle &deviation coeffts)

#include <AIS_ConnectedInteractive.hxx>
#include <AIS_DataMapIteratorOfDataMapOfILC.hxx>
#include <AIS_DataMapIteratorOfDataMapOfIOStatus.hxx>
#include <AIS_GlobalStatus.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_LocalStatus.hxx>
#include <AIS_MapIteratorOfMapOfInteractive.hxx>
#include <AIS_MultipleConnectedInteractive.hxx>
#include <AIS_Selection.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <HLRBRep.hxx>
#include <OSD_Environment.hxx>
#include <Precision.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <Quantity_Color.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_OrFilter.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_Atomic.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <UnitsAPI.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

//#include <AIS_DataMapIteratorOfDataMapOfInteractiveInteger.hxx>
namespace
{
  static volatile Standard_Integer THE_AIS_INDEX_SEL = 0;
  static volatile Standard_Integer THE_AIS_INDEX_CUR = 0;

  static TCollection_AsciiString AIS_Context_NewSelName()
  {
    return TCollection_AsciiString ("AIS_SelContext_")
         + TCollection_AsciiString (Standard_Atomic_Increment (&THE_AIS_INDEX_SEL));
  }

  static TCollection_AsciiString AIS_Context_NewCurName()
  {
    return TCollection_AsciiString ("AIS_CurContext_")
         + TCollection_AsciiString (Standard_Atomic_Increment (&THE_AIS_INDEX_CUR));
  }

  typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject), Handle(SelectMgr_IndexedMapOfOwner)> AIS_MapOfObjectOwners;
  typedef NCollection_DataMap<Handle(SelectMgr_SelectableObject), Handle(SelectMgr_IndexedMapOfOwner)>::Iterator AIS_MapIteratorOfMapOfObjectOwners;
}

//=======================================================================
//function : AIS_InteractiveContext
//purpose  : 
//=======================================================================

AIS_InteractiveContext::AIS_InteractiveContext(const Handle(V3d_Viewer)& MainViewer):
mgrSelector(new SelectMgr_SelectionManager()),
myMainPM(new PrsMgr_PresentationManager3d(MainViewer->StructureManager())),
myMainVwr(MainViewer),
myMainSel(new StdSelect_ViewerSelector3d()),
myWasLastMain(Standard_False),
myCurrentTouched(Standard_False),
mySelectedTouched(Standard_False),
myToHilightSelected(Standard_True),
myFilters(new SelectMgr_OrFilter()),
myDefaultDrawer(new Prs3d_Drawer()),
myDefaultColor(Quantity_NOC_GOLDENROD),
myHilightColor(Quantity_NOC_CYAN1),
mySelectionColor(Quantity_NOC_GRAY80),
myPreselectionColor(Quantity_NOC_GREEN),
mySubIntensity(Quantity_NOC_GRAY40),
myDisplayMode(0),
myCurLocalIndex(0),
myAISCurDetected(0),
myZDetectionFlag(0),
myIsAutoActivateSelMode( Standard_True )
{ 
  InitAttributes();
}

void AIS_InteractiveContext::Delete() const
{
  // clear the static current selection
  AIS_Selection::ClearCurrentSelection();

  // to avoid an exception
  if (AIS_Selection::Find (mySelectionName.ToCString()))
  {
    AIS_Selection::Remove (mySelectionName.ToCString());
  }

  // to avoid an exception
  if (AIS_Selection::Find (myCurrentName.ToCString()))
  {
    AIS_Selection::Remove (myCurrentName.ToCString());
  }

  // let's remove one reference explicitly. this operation's supposed to
  // be performed when mgrSelector will be destroyed but anyway...
  const Handle(SelectMgr_ViewerSelector)& aSelector = myMainSel; // to avoid ambiguity
  mgrSelector->Remove (aSelector);

  Handle(AIS_InteractiveContext) aNullContext;
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj = anObjIter.Key();
    anObj->SetContext (aNullContext);
    for (anObj->Init(); anObj->More(); anObj->Next())
    {
      anObj->CurrentSelection()->UpdateBVHStatus (SelectMgr_TBU_Renew);
    }
  }
  MMgt_TShared::Delete();
}

//=======================================================================
//function : AIS_SelectionName
//purpose  : 
//=======================================================================
const TCollection_AsciiString& AIS_InteractiveContext::SelectionName() const 
{
  if(!HasOpenedContext())
    return mySelectionName;
  return myLocalContexts(myCurLocalIndex)->SelectionName();

} 



//=======================================================================
//function : UpdateCurrentViewer
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UpdateCurrentViewer()
{
  if (!myMainVwr.IsNull())
    myMainVwr->Update();
}


//=======================================================================
//function : DomainOfMainViewer
//purpose  : 
//=======================================================================

Standard_CString AIS_InteractiveContext::DomainOfMainViewer() const 
{
  return myMainVwr->Domain();
  
}

//=======================================================================
//function : DisplayedObjects
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisplayedObjects (AIS_ListOfInteractive& theListOfIO,
                                               const Standard_Boolean theOnlyFromNeutral) const
{
  if (!HasOpenedContext()
   || theOnlyFromNeutral)
  {
    for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
    {
      if (anObjIter.Value()->GraphicStatus() == AIS_DS_Displayed)
      {
        theListOfIO.Append (anObjIter.Key());
      }
    }
    return;
  }

  // neutral point
  TColStd_MapOfTransient aDispMap;
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (anObjIter.Value()->GraphicStatus() == AIS_DS_Displayed)
    {
      aDispMap.Add (anObjIter.Key());
    }
  }

  // parse all local contexts...
  for (AIS_DataMapIteratorOfDataMapOfILC aCtxIter (myLocalContexts); aCtxIter.More(); aCtxIter.Next())
  {
    const Handle(AIS_LocalContext)& aLocCtx = aCtxIter.Value();
    aLocCtx->DisplayedObjects (aDispMap);
  }

  Handle(AIS_InteractiveObject) anObj;
  for (TColStd_MapIteratorOfMapOfTransient aDispMapIter (aDispMap); aDispMapIter.More(); aDispMapIter.Next())
  {
    const Handle(Standard_Transient)& aTransient = aDispMapIter.Key();
    anObj = Handle(AIS_InteractiveObject)::DownCast (aTransient);
    theListOfIO.Append (anObj);
  }
}

//=======================================================================
//function : DisplayedObjects
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisplayedObjects (const AIS_KindOfInteractive theKind,
                                               const Standard_Integer      theSign,
                                               AIS_ListOfInteractive&      theListOfIO,
                                               const Standard_Boolean /*OnlyFromNeutral*/) const
{
  ObjectsByDisplayStatus (theKind, theSign, AIS_DS_Displayed, theListOfIO);
}

//=======================================================================
//function : ErasedObjects
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ErasedObjects (AIS_ListOfInteractive& theListOfIO) const
{
  ObjectsByDisplayStatus (AIS_DS_Erased, theListOfIO);
}

//=======================================================================
//function : ErasedObjects
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ErasedObjects (const AIS_KindOfInteractive theKind,
                                            const Standard_Integer      theSign,
                                            AIS_ListOfInteractive&      theListOfIO) const
{
  ObjectsByDisplayStatus (theKind, theSign, AIS_DS_Erased, theListOfIO);
}

//=======================================================================
//function : ObjectsByDisplayStatus
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ObjectsByDisplayStatus (const AIS_DisplayStatus theStatus,
                                                     AIS_ListOfInteractive&  theListOfIO) const
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (anObjIter.Value()->GraphicStatus() == theStatus)
    {
      theListOfIO.Append (anObjIter.Key());
    }
  }
}

//=======================================================================
//function : ObjectsByDisplayStatus
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ObjectsByDisplayStatus (const AIS_KindOfInteractive theKind,
                                                     const Standard_Integer      theSign,
                                                     const AIS_DisplayStatus     theStatus,
                                                     AIS_ListOfInteractive&      theListOfIO) const
{
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (theStatus != AIS_DS_None
     && anObjIter.Value()->GraphicStatus() != theStatus)
    {
      continue;
    }
    else if (anObjIter.Key()->Type() != theKind)
    {
      continue;
    }

    if (theSign == -1
     || anObjIter.Key()->Signature() == theSign)
    {
      theListOfIO.Append (anObjIter.Key());
    }
  }
}

//=======================================================================
//function : ObjectsInside
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ObjectsInside (AIS_ListOfInteractive&      theListOfIO,
                                            const AIS_KindOfInteractive theKind,
                                            const Standard_Integer      theSign) const
{
  if (theKind == AIS_KOI_None
   && theSign == -1)
  {
    for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
    {
      theListOfIO.Append (anObjIter.Key());
    }
    return;
  }

  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (anObjIter.Key()->Type() != theKind)
    {
      continue;
    }

    if (theSign == -1
     || anObjIter.Key()->Signature() == theSign)
    {
      theListOfIO.Append (anObjIter.Key());
    }
  }
}

//=======================================================================
//function : ObjectsForView
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ObjectsForView (AIS_ListOfInteractive&  theListOfIO,
                                             const Handle(V3d_View)& theView,
                                             const Standard_Boolean  theIsVisibleInView,
                                             const AIS_DisplayStatus theStatus) const
{
  Handle(Graphic3d_CView) aViewImpl = theView->View();
  const Standard_Integer  aViewId   = aViewImpl->Identification();
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (theStatus != AIS_DS_None
     && anObjIter.Value()->GraphicStatus() != theStatus)
    {
      theListOfIO.Append (anObjIter.Key());
      continue;
    }

    Handle(Graphic3d_ViewAffinity) anAffinity = myMainVwr->StructureManager()->ObjectAffinity (anObjIter.Key());
    const Standard_Boolean isVisible = anAffinity->IsVisible (aViewId);
    if (isVisible == theIsVisibleInView)
    {
      theListOfIO.Append (anObjIter.Key());
    }
  }
}

//=======================================================================
//function : Display
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Display (const Handle(AIS_InteractiveObject)& theIObj,
                                      const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  Standard_Integer aDispMode = 0, aHiMod = -1, aSelMode = -1;
  GetDefModes (theIObj, aDispMode, aHiMod, aSelMode);

  Display (theIObj, aDispMode, myIsAutoActivateSelMode ? aSelMode : -1,
           theToUpdateViewer, theIObj->AcceptShapeDecomposition());
}

//=======================================================================
//function : SetViewAffinity
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetViewAffinity (const Handle(AIS_InteractiveObject)& theIObj,
                                              const Handle(V3d_View)&              theView,
                                              const Standard_Boolean               theIsVisible)
{
  if (theIObj.IsNull()
  || !myObjects.IsBound (theIObj))
  {
    return;
  }

  Handle(Graphic3d_ViewAffinity) anAffinity = myMainVwr->StructureManager()->ObjectAffinity (theIObj);
  Handle(Graphic3d_CView) aViewImpl = theView->View();
  anAffinity->SetVisible (aViewImpl->Identification(), theIsVisible == Standard_True);
  if (theIsVisible)
  {
    theView->View()->ChangeHiddenObjects()->Remove (theIObj.get());
  }
  else
  {
    theView->View()->ChangeHiddenObjects()->Add (theIObj.get());
  }
}

//=======================================================================
//function : Display
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Display (const Handle(AIS_InteractiveObject)& theIObj,
                                      const Standard_Integer               theDispMode,
                                      const Standard_Integer               theSelectionMode,
                                      const Standard_Boolean               theToUpdateViewer,
                                      const Standard_Boolean               theToAllowDecomposition,
                                      const AIS_DisplayStatus              theDispStatus)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (theDispStatus == AIS_DS_Erased)
  {
    Erase  (theIObj, theToUpdateViewer);
    Load (theIObj, theSelectionMode, theToAllowDecomposition);
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  if (theDispStatus == AIS_DS_Temporary
  && !HasOpenedContext())
  {
    return;
  }
  else if (HasOpenedContext())
  {
    if (theDispStatus == AIS_DS_None
     || theDispStatus == AIS_DS_Temporary)
    {
      myLocalContexts (myCurLocalIndex)->Display (theIObj, theDispMode, theToAllowDecomposition, theSelectionMode);
      if (theToUpdateViewer)
      {
        myMainVwr->Update();
      }
      return;
    }
  }

  if (!myObjects.IsBound (theIObj))
  {
    Handle(AIS_GlobalStatus) aStatus = new AIS_GlobalStatus (AIS_DS_Displayed, theDispMode, theSelectionMode);
    myObjects.Bind   (theIObj, aStatus);
    Handle(Graphic3d_ViewAffinity) anAffinity = myMainVwr->StructureManager()->RegisterObject (theIObj);
    myMainPM->Display(theIObj, theDispMode);
    if (theSelectionMode != -1)
    {
      const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
      if (!mgrSelector->Contains (anObj))
      {
        mgrSelector->Load (theIObj);
      }
      mgrSelector->Activate (theIObj, theSelectionMode, myMainSel);
    }
  }
  else
  {
    Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);
    if (aStatus->GraphicStatus() == AIS_DS_Temporary)
    {
      return;
    }

    // Erase presentations for all display modes different from aDispMode.
    // Then make sure aDispMode is displayed and maybe highlighted.
    // Finally, activate selection mode <SelMode> if not yet activated.
    TColStd_ListOfInteger aModesToRemove;
    for (TColStd_ListIteratorOfListOfInteger aDispModeIter (aStatus->DisplayedModes()); aDispModeIter.More(); aDispModeIter.Next())
    {
      const Standard_Integer anOldMode = aDispModeIter.Value();
      if (anOldMode != theDispMode)
      {
        aModesToRemove.Append (anOldMode);
        if(myMainPM->IsHighlighted (theIObj, anOldMode))
        {
          myMainPM->Unhighlight (theIObj, anOldMode);
        }
        myMainPM->Erase (theIObj, anOldMode);
      }
    }

    for (TColStd_ListIteratorOfListOfInteger aRemModeIter (aModesToRemove); aRemModeIter.More(); aRemModeIter.Next())
    {
      aStatus->RemoveDisplayMode (aRemModeIter.Value());
    }

    if (!aStatus->IsDModeIn (theDispMode))
    {
      aStatus->AddDisplayMode (theDispMode);
    }

    myMainPM->Display (theIObj, theDispMode);
    aStatus->SetGraphicStatus (AIS_DS_Displayed);
    if (aStatus->IsHilighted())
    {
      const Standard_Integer aHiMod = theIObj->HasHilightMode() ? theIObj->HilightMode() : theDispMode;
      myMainPM->Color (theIObj, aStatus->HilightColor(), aHiMod);
    }
    if (theSelectionMode != -1)
    {
      const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
      if (!mgrSelector->Contains (anObj))
      {
        mgrSelector->Load (theIObj);
      }
      if (!mgrSelector->IsActivated (theIObj, theSelectionMode))
      {
        if (!aStatus->IsSModeIn (theSelectionMode))
          aStatus->AddSelectionMode (theSelectionMode);
        mgrSelector->Activate (theIObj, theSelectionMode, myMainSel);
      }
    }
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : Load
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Load (const Handle(AIS_InteractiveObject)& theIObj,
                                   const Standard_Integer               theSelMode,
                                   const Standard_Boolean               theToAllowDecomposition)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  if (HasOpenedContext())
  {
    myLocalContexts (myCurLocalIndex)->Load (theIObj, theToAllowDecomposition, theSelMode);
    return;
  }

  if (theSelMode == -1
  && !theToAllowDecomposition)
  {
    if (!myObjects.IsBound (theIObj))
    {
      Standard_Integer aDispMode, aHiMod, aSelModeDef;
      GetDefModes (theIObj, aDispMode, aHiMod, aSelModeDef);
      Handle(AIS_GlobalStatus) aStatus = new AIS_GlobalStatus (AIS_DS_Erased, aDispMode, aSelModeDef);
      myObjects.Bind (theIObj, aStatus);
    }

    // Register theIObj in the selection manager to prepare further activation of selection
    const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
    if (!mgrSelector->Contains (anObj))
    {
      mgrSelector->Load (theIObj);
    }
  }
}

//=======================================================================
//function : Erase
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Erase (const Handle(AIS_InteractiveObject)& theIObj,
                                    const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }
  
  if (!theIObj->IsAutoHilight())
  {
    theIObj->ClearSelected();
  }

  Standard_Boolean wasInCtx = Standard_False;
  if (HasOpenedContext())
  {
    // First it is checked if it is possible to remove in the current local context
    // then one tries to remove in other local contexts, if they allow it...
    wasInCtx = myLocalContexts (myCurLocalIndex)->Erase (theIObj);
    for (AIS_DataMapIteratorOfDataMapOfILC aCtxIter (myLocalContexts); aCtxIter.More(); aCtxIter.Next())
    {
      if (aCtxIter.Value()->AcceptErase())
      {
        wasInCtx = aCtxIter.Value()->Erase (theIObj) || wasInCtx;
      }
    }
  }

  if (!wasInCtx)
  {
    EraseGlobal (theIObj, Standard_False);
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : EraseAll
//purpose  :
//=======================================================================
void AIS_InteractiveContext::EraseAll (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
  {
    return;
  }

  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    if (anObjIter.Value()->GraphicStatus() == AIS_DS_Displayed)
    {
      Erase (anObjIter.Key(), Standard_False);
    }
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : DisplayAll
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisplayAll (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
  {
    return;
  }

  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    const AIS_DisplayStatus aStatus = anObjIter.Value()->GraphicStatus();
    if (aStatus == AIS_DS_Erased)
    {
      Display (anObjIter.Key(), Standard_False);
    }
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : DisplaySelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisplaySelected (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
  {
    return;
  }

  Standard_Boolean      isFound  = Standard_False;
  Handle(AIS_Selection) aSelIter = AIS_Selection::Selection (myCurrentName.ToCString());
  for (aSelIter->Init(); aSelIter->More(); aSelIter->Next())
  {
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (aSelIter->Value());
    Display (anObj, Standard_False);
    isFound = Standard_True;
  }

  if (isFound && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : EraseSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::EraseSelected (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
  {
    return;
  }

  Standard_Boolean      isFound  = Standard_False;
  Handle(AIS_Selection) aSelIter = AIS_Selection::Selection(myCurrentName.ToCString());

  aSelIter->Init();
  while (aSelIter->More())
  {
    Handle(SelectMgr_EntityOwner) anOwner = Handle(SelectMgr_EntityOwner)::DownCast (aSelIter->Value());
    Handle(AIS_InteractiveObject) anObj   = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());

    Erase (anObj, Standard_False);
    isFound = Standard_True;

    aSelIter->Init();
  }

  if (isFound && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::KeepTemporary(const Handle(AIS_InteractiveObject)& anIObj,
                                                       const Standard_Integer WhichContext)
{
  if(anIObj.IsNull()) return Standard_False;

  if(!HasOpenedContext()) return Standard_False;
  if(myObjects.IsBound(anIObj)) return Standard_False;
  if(WhichContext!=-1 && !myLocalContexts.IsBound(WhichContext)) return Standard_False;
  
  // Protection : if one tries to preserve a temporary object
  // which is not in the local active context... rob 11-06-97

  Standard_Integer IsItInLocal = myCurLocalIndex;
  Standard_Boolean Found(Standard_False);

  while(IsItInLocal>0 && !Found){
    if(!myLocalContexts.IsBound(IsItInLocal))
      IsItInLocal--;
    else if(myLocalContexts(IsItInLocal)->IsIn(anIObj))
      Found = Standard_True;
    else
      IsItInLocal--;
  }

  if(!Found) return Standard_False;
  

//  const Handle(AIS_LocalStatus)& LS = (WhichContext== -1) ? 
//    myLocalContexts(IsItInLocal)->Status(anIObj):myLocalContexts(WhichContext)->Status(anIObj);
  // CLE
  // const Handle(AIS_LocalStatus)& LS = myLocalContexts(IsItInLocal)->Status(anIObj);
  Handle(AIS_LocalStatus) LS = myLocalContexts(IsItInLocal)->Status(anIObj);
  // ENDCLE
  
  
  if(LS->IsTemporary()){
    Standard_Integer DM,HM,SM;
    GetDefModes(anIObj,DM,HM,SM);
    
    SM = LS->SelectionModes().IsEmpty() ? SM : LS->SelectionModes().First();
    if(LS->DisplayMode()!= DM ){
      Standard_Integer LSM =  LS->SelectionModes().IsEmpty() ? -1 : LS->SelectionModes().First();
      myLocalContexts(IsItInLocal)->Display(anIObj,DM,LS->Decomposed(),LSM);
    }

    Handle (AIS_GlobalStatus) GS = new AIS_GlobalStatus(AIS_DS_Displayed,
                                                        DM,
                                                        SM,
                                                        Standard_False);
//    GS->SubIntensityOn();
    myObjects.Bind(anIObj,GS);
    mgrSelector->Load(anIObj);
    mgrSelector->Activate(anIObj,SM,myMainSel);
    
    LS->SetTemporary(Standard_False);
  }                                 
  return Standard_True;
}

//=======================================================================
//function : DisplayStatus
//purpose  :
//=======================================================================
AIS_DisplayStatus AIS_InteractiveContext::DisplayStatus (const Handle(AIS_InteractiveObject)& theIObj) const
{
  if (theIObj.IsNull())
  {
    return AIS_DS_None;
  }
  else if (myObjects.IsBound (theIObj))
  {
    return myObjects (theIObj)->GraphicStatus();
  }

  for (AIS_DataMapIteratorOfDataMapOfILC aCtxIter (myLocalContexts); aCtxIter.More(); aCtxIter.Next())
  {
    if (aCtxIter.Value()->IsIn (theIObj))
    {
      return AIS_DS_Temporary;
    }
  }
  return AIS_DS_None;
}

//=======================================================================
//function : DisplayedModes
//purpose  :
//=======================================================================
const TColStd_ListOfInteger& AIS_InteractiveContext::DisplayedModes (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return myObjects (theIObj)->DisplayedModes();
}

//=======================================================================
//function : Remove
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Remove (const Handle(AIS_InteractiveObject)& theIObj,
                                     const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (HasOpenedContext())
  {
    myLocalContexts (myCurLocalIndex)->Remove (theIObj);
    for (AIS_DataMapIteratorOfDataMapOfILC aCtxIter (myLocalContexts); aCtxIter.More(); aCtxIter.Next())
    {
      if (aCtxIter.Value()->AcceptErase())
      {
        aCtxIter.Value()->Remove (theIObj);
      }
    }
  }

  ClearGlobal (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : RemoveAll
//purpose  :
//=======================================================================
void AIS_InteractiveContext::RemoveAll (const Standard_Boolean theToUpdateViewer)
{
  AIS_ListOfInteractive aList;
  ObjectsInside (aList);
  for (AIS_ListIteratorOfListOfInteractive aListIterator (aList); aListIterator.More(); aListIterator.Next())
  {
    Remove (aListIterator.Value(), Standard_False);
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : ClearPrs
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ClearPrs (const Handle(AIS_InteractiveObject)& theIObj,
                                       const Standard_Integer               theMode,
                                       const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!HasOpenedContext())
  {
    ClearGlobalPrs (theIObj, theMode, theToUpdateViewer);
    return;
  }

  Standard_Boolean wasInCtx = myLocalContexts (myCurLocalIndex)->ClearPrs (theIObj, theMode);
  for (AIS_DataMapIteratorOfDataMapOfILC aCtxIter (myLocalContexts); aCtxIter.More(); aCtxIter.Next())
  {
    if (aCtxIter.Value()->AcceptErase())
    {
      wasInCtx = aCtxIter.Value()->ClearPrs (theIObj, theMode) || wasInCtx;
    }
  }
  if (!wasInCtx)
  {
    ClearGlobalPrs (theIObj, theMode, theToUpdateViewer);
  }
  else if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : Hilight
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Hilight (const Handle(AIS_InteractiveObject)& theIObj,
                                      const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }
  if (!HasOpenedContext())
  {
    if (!myObjects.IsBound (theIObj))
    {
      return;
    }

    Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);
    aStatus->SetHilightStatus (Standard_True);
    if (aStatus->GraphicStatus() == AIS_DS_Displayed)
    {
      Standard_Integer aHilightMode = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
      myMainPM->Highlight (theIObj, aHilightMode);
    }
  }
  else
  {
    myLocalContexts (myCurLocalIndex)->Hilight (theIObj);
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}
//=======================================================================
//function : Hilight
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::HilightWithColor(const Handle(AIS_InteractiveObject)& anIObj,
                                              const Quantity_NameOfColor aCol,
                                              const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return;

  if(!anIObj->HasInteractiveContext()) anIObj->SetContext(this);

  if (!HasOpenedContext())
  {
    if(!myObjects.IsBound(anIObj)) return;

    const Handle(AIS_GlobalStatus)& aStatus = myObjects(anIObj);
    aStatus->SetHilightStatus (Standard_True);

    if (aStatus->GraphicStatus() == AIS_DS_Displayed)
    {
      const Standard_Integer aHilightMode = anIObj->HasHilightMode() ? anIObj->HilightMode() : 0;
      myMainPM->Color (anIObj, aCol, aHilightMode);
      aStatus->SetHilightColor (aCol);
    }
  }
  else
  {
    myLocalContexts(myCurLocalIndex)->Hilight(anIObj,aCol);
  }
  if(updateviewer) myMainVwr->Update();
}

//=======================================================================
//function : Unhilight
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Unhilight(const Handle(AIS_InteractiveObject)& anIObj, const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return;

  if (!HasOpenedContext())
  {
    if(!myObjects.IsBound(anIObj)) return;

    const Handle(AIS_GlobalStatus)& aStatus = myObjects(anIObj);
    aStatus->SetHilightStatus (Standard_False);
    aStatus->SetHilightColor(Quantity_NOC_WHITE);

    if (aStatus->GraphicStatus() == AIS_DS_Displayed)
    {
      Standard_Integer aHilightMode = anIObj->HasHilightMode() ? anIObj->HilightMode() : 0;
      myMainPM->Unhighlight (anIObj, aHilightMode);
    }
  }
  else
  {
    myLocalContexts(myCurLocalIndex)->Unhilight(anIObj);
  }
  if(updateviewer) myMainVwr->Update();
}

//=======================================================================
//function : IsHilighted
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::IsHilighted(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  if(anIObj.IsNull()) return Standard_False;

  if (!HasOpenedContext()){
    if(!myObjects.IsBound(anIObj)) 
      return Standard_False;
    return myObjects(anIObj)->IsHilighted();
  }
  AIS_DataMapIteratorOfDataMapOfILC ItM(myLocalContexts);
  for(;ItM.More();ItM.Next()){
    if(ItM.Value()->IsHilighted(anIObj))
      return Standard_True;
  }
  return Standard_False;
}

Standard_Boolean AIS_InteractiveContext::IsHilighted(const Handle(AIS_InteractiveObject)& anIObj,
                                                     Standard_Boolean& WithColor,
                                                     Quantity_NameOfColor& TheHiCol) const
{
  if(!HasOpenedContext()){
    if(myObjects.IsBound(anIObj)){
      const Handle(AIS_GlobalStatus)& STAT = myObjects(anIObj);
      if(STAT->IsHilighted()){
        if(STAT->HilightColor()!=Quantity_NOC_WHITE){
          WithColor=Standard_True;
          TheHiCol = STAT->HilightColor();
        }
        else
          WithColor = Standard_False;
        return Standard_True;
      }
    }
    return Standard_False;
  }
  Standard_Integer MaxIndex = HighestIndex();
  for(Standard_Integer i=MaxIndex;i>=1 ; i--){
    if(myLocalContexts.IsBound(i)){
      if(myLocalContexts(i)->IsHilighted(anIObj,WithColor,TheHiCol))
        return Standard_True;
    }
    
  }
  return Standard_False;
}

//=======================================================================
//function : IsHilighted
//purpose  : Returns true if the objects global status is set to highlighted.
//           theIsCustomColor flag defines if highlight color is not equal to OCCT's
//           default Quantity_NOC_WHITE color. If theIsCustomColor is true,
//           custom highlight color name will be stored to theCustomColorName
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsHilighted (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                      Standard_Boolean& theIsCustomColor,
                                                      Quantity_NameOfColor& theCustomColorName) const
{
  if (theOwner.IsNull() || !theOwner->HasSelectable())
    return Standard_False;

  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());

  if (!myObjects.IsBound (anObj))
    return Standard_False;

  const Handle(AIS_GlobalStatus)& anObjStatus = myObjects (anObj);
  if (anObjStatus->IsHilighted())
  {
    if (anObjStatus->HilightColor() != Quantity_NOC_WHITE)
    {
      theIsCustomColor = Standard_True;
      theCustomColorName = anObjStatus->HilightColor();
    }
    else
    {
      theIsCustomColor = Standard_False;
    }

    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::IsDisplayed(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  if(anIObj.IsNull()) return Standard_False;


  if(myObjects.IsBound(anIObj)) 
    if(myObjects(anIObj)->GraphicStatus()==AIS_DS_Displayed)
      return Standard_True;
  
  AIS_DataMapIteratorOfDataMapOfILC ItM(myLocalContexts);
  for(;ItM.More();ItM.Next()){
    if(ItM.Value()->IsDisplayed(anIObj))
      return Standard_True;
  }
  return Standard_False;
  
}

//=======================================================================
//function : IsDisplayed
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsDisplayed (const Handle(AIS_InteractiveObject)& theIObj,
                                                      const Standard_Integer               theMode) const
{
  if (theIObj.IsNull())
  {
    return Standard_False;
  }

  if (myObjects.IsBound (theIObj))
  {
    Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);
    if (aStatus->GraphicStatus() == AIS_DS_Displayed
     && aStatus->IsDModeIn (theMode))
    {
      return Standard_True;
    }
  }

  for (AIS_DataMapIteratorOfDataMapOfILC aCtxIter (myLocalContexts); aCtxIter.More(); aCtxIter.Next())
  {
    if (aCtxIter.Value()->IsDisplayed (theIObj, theMode))
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : DisplayPriority
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::DisplayPriority (const Handle(AIS_InteractiveObject)& theIObj) const
{
  if (theIObj.IsNull())
  {
    return -1;
  }
  else if (!myObjects.IsBound (theIObj))
  {
    return 0;
  }

  Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);
  if (aStatus->GraphicStatus() == AIS_DS_Displayed
   || aStatus->GraphicStatus() == AIS_DS_Erased)
  {
    Standard_Integer aDispMode = theIObj->HasDisplayMode()
                               ? theIObj->DisplayMode()
                               : (theIObj->AcceptDisplayMode (myDisplayMode)
                                ? myDisplayMode
                                : 0);
    return myMainPM->DisplayPriority (theIObj, aDispMode);
  }
  return 0;
}

//=======================================================================
//function : SetDisplayPriority
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDisplayPriority (const Handle(AIS_InteractiveObject)& theIObj,
                                                 const Standard_Integer               thePriority)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  if (myObjects.IsBound (theIObj))
  {
    Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);
    if (aStatus->GraphicStatus() == AIS_DS_Displayed
     || aStatus->GraphicStatus() == AIS_DS_Erased)
    {
      Standard_Integer aDisplayMode = theIObj->HasDisplayMode()
                                    ? theIObj->DisplayMode()
                                    : (theIObj->AcceptDisplayMode (myDisplayMode)
                                     ? myDisplayMode
                                     : 0);
      myMainPM->SetDisplayPriority (theIObj, aDisplayMode, thePriority);
    }
  }
  else if (HasOpenedContext())
  {
    myLocalContexts (myCurLocalIndex)->SetDisplayPriority (theIObj, thePriority);
  }
}

//=======================================================================
//function : Redisplay
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Redisplay (const Handle(AIS_InteractiveObject)& theIObj,
                                        const Standard_Boolean               theToUpdateViewer,
                                        const Standard_Boolean               theAllModes)
{
  RecomputePrsOnly (theIObj, theToUpdateViewer, theAllModes);
  RecomputeSelectionOnly (theIObj);
}

//=======================================================================
//function : Redisplay
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Redisplay (const AIS_KindOfInteractive theKOI,
                                        const Standard_Integer    /*theSign*/,
                                        const Standard_Boolean      theToUpdateViewer)
{
  Standard_Boolean isRedisplayed = Standard_False;
  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj = anObjIter.Key();
    if (anObj->Type() != theKOI)
    {
      continue;
    }

    Redisplay (anObj, Standard_False);
    isRedisplayed = anObjIter.Value()->GraphicStatus() == AIS_DS_Displayed
                 || isRedisplayed;
  }

  if (theToUpdateViewer
   && isRedisplayed)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : RecomputePrsOnly
//purpose  :
//=======================================================================
void AIS_InteractiveContext::RecomputePrsOnly (const Handle(AIS_InteractiveObject)& theIObj,
                                               const Standard_Boolean               theToUpdateViewer,
                                               const Standard_Boolean               theAllModes)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->Update (theAllModes);
  if (!theToUpdateViewer)
  {
    return;
  }

  if (HasOpenedContext()
   || (myObjects.IsBound (theIObj)
    && myObjects (theIObj)->GraphicStatus() == AIS_DS_Displayed))
  {
    myMainVwr->Update();
  }
}
//=======================================================================
//function : RecomputeSelectionOnly
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::RecomputeSelectionOnly (const Handle(AIS_InteractiveObject)& theIO)
{
  if (theIO.IsNull())
  {
    return;
  }

  mgrSelector->RecomputeSelection (theIO);

  if (HasOpenedContext())
  {
    for (Standard_Integer aContextIdx = 1; aContextIdx <= myLocalContexts.Extent(); aContextIdx++)
    {
      myLocalContexts (aContextIdx)->ClearOutdatedSelection (theIO, Standard_False);
    }
    return;
  }

  if (!myObjects.IsBound (theIO) ||
      myObjects (theIO)->GraphicStatus() != AIS_DS_Displayed)
  {
    return;
  }

  TColStd_ListOfInteger aModes;
  ActivatedModes (theIO, aModes);
  TColStd_ListIteratorOfListOfInteger aModesIter (aModes);
  for (; aModesIter.More(); aModesIter.Next())
  {
    mgrSelector->Activate (theIO, aModesIter.Value(), myMainSel);
  }
}

//=======================================================================
//function : Update
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Update (const Handle(AIS_InteractiveObject)& theIObj,
                                     const Standard_Boolean               theUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  TColStd_ListOfInteger aPrsModes;
  theIObj->ToBeUpdated (aPrsModes);
  for (TColStd_ListIteratorOfListOfInteger aPrsModesIt (aPrsModes); aPrsModesIt.More(); aPrsModesIt.Next())
  {
    theIObj->Update (aPrsModesIt.Value(), Standard_False);
  }

  mgrSelector->Update(theIObj);

  for (Standard_Integer aContextIdx = 1; aContextIdx <= myLocalContexts.Extent(); aContextIdx++)
  {
    myLocalContexts (aContextIdx)->ClearOutdatedSelection (theIObj, Standard_False);
  }

  if (theUpdateViewer)
  {
    if (!myObjects.IsBound (theIObj))
    {
      return;
    }

    switch (myObjects (theIObj)->GraphicStatus())
    {
      case AIS_DS_Displayed:
      case AIS_DS_Temporary:
        myMainVwr->Update();
        break;
      default:
        break;
    }
  }
}

//=======================================================================
//function : SetLocation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetLocation (const Handle(AIS_InteractiveObject)& theIObj,
                                          const TopLoc_Location&               theLoc)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (theIObj->HasTransformation()
   && theLoc.IsIdentity())
  {
    theIObj->ResetTransformation();
    mgrSelector->Update (theIObj, Standard_False);
    return;
  }
  else if (theLoc.IsIdentity())
  {
    return;
  }

  // first reset the previous location to properly clean everything...
  if (theIObj->HasTransformation())
  {
    theIObj->ResetTransformation();
  }

  theIObj->SetLocalTransformation (theLoc.Transformation());

  if (!HasOpenedContext())
  {
    mgrSelector->Update (theIObj, Standard_False);
  }
  else
  {
    Handle(StdSelect_ViewerSelector3d) aTempSel = myLocalContexts (myCurLocalIndex)->MainSelector();
    mgrSelector->Update (theIObj, aTempSel, Standard_False);
  }
}

//=======================================================================
//function : ResetLocation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ResetLocation (const Handle(AIS_InteractiveObject)& theIObj)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->ResetTransformation();
  mgrSelector->Update (theIObj, Standard_False);
}

//=======================================================================
//function : HasLocation
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HasLocation (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return !theIObj.IsNull()
       && theIObj->HasTransformation();
}

//=======================================================================
//function : Location
//purpose  :
//=======================================================================
TopLoc_Location AIS_InteractiveContext::Location (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->Transformation();
}

//=======================================================================
//function : SetDeviationCoefficient
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDeviationCoefficient (const Standard_Real theCoefficient)
{
  myDefaultDrawer->SetDeviationCoefficient (theCoefficient);
}

//=======================================================================
//function : SetDeviationAngle
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDeviationAngle (const Standard_Real theAngle)
{
  myDefaultDrawer->SetDeviationAngle (theAngle);
}

//=======================================================================
//function : DeviationAngle
//purpose  : Gets  deviationAngle
//=======================================================================
Standard_Real AIS_InteractiveContext::DeviationAngle() const
{
  return myDefaultDrawer->DeviationAngle();
}

//=======================================================================
//function : DeviationCoefficient
//purpose  :
//=======================================================================
Standard_Real AIS_InteractiveContext::DeviationCoefficient() const
{
  return myDefaultDrawer->DeviationCoefficient();
}

//=======================================================================
//function : SetHLRDeviationCoefficient
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetHLRDeviationCoefficient (const Standard_Real theCoefficient)
{
  myDefaultDrawer->SetHLRDeviationCoefficient (theCoefficient);
}

//=======================================================================
//function : HLRDeviationCoefficient
//purpose  :
//=======================================================================
Standard_Real AIS_InteractiveContext::HLRDeviationCoefficient() const
{
  return myDefaultDrawer->HLRDeviationCoefficient();
}

//=======================================================================
//function : SetHLRAngle
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetHLRAngle (const Standard_Real theAngle)
{
  myDefaultDrawer->SetHLRAngle (theAngle);
}

//=======================================================================
//function : SetHLRAngleAndDeviation
//purpose  : compute with anangle a HLRAngle and a HLRDeviationCoefficient 
//           and set them in myHLRAngle and in myHLRDeviationCoefficient
//           of myDefaultDrawer 
//=======================================================================
void AIS_InteractiveContext::SetHLRAngleAndDeviation (const Standard_Real theAngle)
{
  Standard_Real anOutAngl, anOutDefl;
  HLRBRep::PolyHLRAngleAndDeflection (theAngle, anOutAngl, anOutDefl);

  myDefaultDrawer->SetHLRAngle                (anOutAngl);
  myDefaultDrawer->SetHLRDeviationCoefficient (anOutDefl);
}

//=======================================================================
//function : HLRAngle
//purpose  :
//=======================================================================
Standard_Real AIS_InteractiveContext::HLRAngle() const 
{
  return myDefaultDrawer->HLRAngle();
}

//=======================================================================
//function : SetDisplayMode
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDisplayMode (const AIS_DisplayMode  theMode,
                                             const Standard_Boolean theToUpdateViewer)
{
  if (theMode == myDisplayMode)
  {
    return;
  }

  for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjIter (myObjects); anObjIter.More(); anObjIter.Next())
  {
    Handle(AIS_InteractiveObject) anObj = anObjIter.Key();
    Standard_Boolean toProcess = anObj->IsKind (STANDARD_TYPE(AIS_Shape))
                              || anObj->IsKind (STANDARD_TYPE(AIS_ConnectedInteractive))
                              || anObj->IsKind (STANDARD_TYPE(AIS_MultipleConnectedInteractive));
    
    if (!toProcess
     ||  anObj->HasDisplayMode()
     || !anObj->AcceptDisplayMode (theMode))
    {
      continue;
    }

    Handle(AIS_GlobalStatus) aStatus = anObjIter.Value();
    if (aStatus->IsDModeIn (myDisplayMode))
    {
      aStatus->RemoveDisplayMode (myDisplayMode);
    }

    aStatus->AddDisplayMode (theMode);
    if (aStatus->GraphicStatus() == AIS_DS_Displayed)
    {
      myMainPM->Display (anObj, theMode);
      if (!myLastPicked.IsNull() && myLastPicked->Selectable() == anObj)
      {
        myMainPM->BeginImmediateDraw();
        myMainPM->Unhighlight (anObj, myDisplayMode);
        myMainPM->EndImmediateDraw (myMainVwr);
      }
      if (aStatus->IsSubIntensityOn())
      {
        myMainPM->Color (anObj, mySubIntensity, theMode);
      }
      myMainPM->SetVisibility (anObj, myDisplayMode, Standard_False);
    }
  }

  myDisplayMode = theMode;
  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : SetDisplayMode
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDisplayMode (const Handle(AIS_InteractiveObject)& theIObj,
                                             const Standard_Integer               theMode,
                                             const Standard_Boolean               theToUpdateViewer)
{
  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext(this);
  }

  if (!myObjects.IsBound (theIObj))
  {
    theIObj->SetDisplayMode (theMode);
    return;
  }
  else if (!theIObj->AcceptDisplayMode (theMode))
  {
    return;
  }

  Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);
  if (aStatus->GraphicStatus() != AIS_DS_Displayed)
  {
    theIObj->SetDisplayMode (theMode);
    return;
  }

  // erase presentations for all display modes different from <aMode>
  TColStd_ListOfInteger aModesToRemove;
  for (TColStd_ListIteratorOfListOfInteger aDispModeIter (aStatus->DisplayedModes()); aDispModeIter.More(); aDispModeIter.Next())
  {
    const Standard_Integer anOldMode = aDispModeIter.Value();
    if (anOldMode != theMode)
    {
      aModesToRemove.Append (anOldMode);
      if (myMainPM->IsHighlighted (theIObj, anOldMode))
      {
        myMainPM->Unhighlight (theIObj, anOldMode);
      }
      myMainPM->SetVisibility (theIObj, anOldMode, Standard_False);
    }
  }

  for (TColStd_ListIteratorOfListOfInteger aRemModeIter (aModesToRemove); aRemModeIter.More(); aRemModeIter.Next())
  {
    aStatus->RemoveDisplayMode (aRemModeIter.Value());
  }

  if (!aStatus->IsDModeIn (theMode))
  {
    aStatus->AddDisplayMode (theMode);
  }

  myMainPM->Display (theIObj, theMode);
  Standard_Integer aDispMode, aHiMode, aSelMode;
  GetDefModes (theIObj, aDispMode, aHiMode, aSelMode);
  if (aStatus->IsHilighted())
  {
    myMainPM->Highlight (theIObj, aHiMode);
  }
  if (aStatus->IsSubIntensityOn())
  {
    myMainPM->Color (theIObj, mySubIntensity, theMode);
  }

  if (theToUpdateViewer)
  {
    myMainVwr->Update();
  }
  theIObj->SetDisplayMode (theMode);
}

//=======================================================================
//function : UnsetDisplayMode
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetDisplayMode (const Handle(AIS_InteractiveObject)& theIObj,
                                               const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull()
  || !theIObj->HasDisplayMode())
  {
    return;
  }

  if (!myObjects.IsBound (theIObj))
  {
    theIObj->UnsetDisplayMode();
    return;
  }

  const Standard_Integer anOldMode = theIObj->DisplayMode();
  if (myDisplayMode == anOldMode)
  {
    return;
  }

  const Handle(AIS_GlobalStatus)& aStatus = myObjects (theIObj);
  aStatus->RemoveDisplayMode (anOldMode);
  if (!aStatus->IsDModeIn(myDisplayMode))
  {
    aStatus->AddDisplayMode (myDisplayMode);
  }

  if (aStatus->GraphicStatus() == AIS_DS_Displayed)
  {
    if (myMainPM->IsHighlighted (theIObj, anOldMode))
    {
      myMainPM->Unhighlight (theIObj, anOldMode);
    }
    myMainPM->SetVisibility (theIObj, anOldMode, Standard_False);
    myMainPM->Display (theIObj, myDisplayMode);

    Standard_Integer aDispMode, aHiMode, aSelMode;
    GetDefModes (theIObj, aDispMode, aHiMode, aSelMode);
    if (aStatus->IsHilighted())
    {
      myMainPM->Highlight (theIObj, aHiMode);
    }
    if (aStatus->IsSubIntensityOn())
    {
      myMainPM->Color (theIObj, mySubIntensity, myDisplayMode);
    }

    if (theToUpdateViewer)
    {
      myMainVwr->Update();
    }
  }

  theIObj->UnsetDisplayMode();
}

//=======================================================================
//function : SetCurrentFacingModel
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetCurrentFacingModel (const Handle(AIS_InteractiveObject)& theIObj,
                                                    const Aspect_TypeOfFacingModel       theModel)
{
  if (!theIObj.IsNull())
  {
    theIObj->SetCurrentFacingModel (theModel);
  }
}

//=======================================================================
//function : redisplayPrsRecModes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::redisplayPrsRecModes (const Handle(AIS_InteractiveObject)& theIObj,
                                                   const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj->RecomputeEveryPrs())
  {
    theIObj->Update (Standard_True);
    theIObj->UpdateSelection();
  }
  else
  {
    for (TColStd_ListIteratorOfListOfInteger aModes (theIObj->ListOfRecomputeModes()); aModes.More(); aModes.Next())
    {
      theIObj->Update (aModes.Value(), Standard_False);
    }
    theIObj->UpdateSelection();
    theIObj->SetRecomputeOk();
  }

  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
//function : redisplayPrsModes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::redisplayPrsModes (const Handle(AIS_InteractiveObject)& theIObj,
                                                const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj->RecomputeEveryPrs())
  {
    theIObj->Update (Standard_True);
    theIObj->UpdateSelection();
  }
  else
  {
    TColStd_ListOfInteger aModes;
    theIObj->ToBeUpdated (aModes);
    for (TColStd_ListIteratorOfListOfInteger aModeIter (aModes); aModeIter.More(); aModeIter.Next())
    {
      theIObj->Update (aModeIter.Value(), Standard_False);
    }
    theIObj->SetRecomputeOk();
  }

  if (theToUpdateViewer)
  {
    UpdateCurrentViewer();
  }
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetColor (const Handle(AIS_InteractiveObject)& theIObj,
                                       const Quantity_NameOfColor           theColor,
                                       const Standard_Boolean               theToUpdateViewer)
{
  SetColor (theIObj, Quantity_Color(theColor), theToUpdateViewer);
}

//=======================================================================
//function : SetColor
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetColor (const Handle(AIS_InteractiveObject)& theIObj,
                                       const Quantity_Color&                theColor,
                                       const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }
  theIObj->SetColor (theColor);
  redisplayPrsRecModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : SetIsoOnTriangulation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::IsoOnTriangulation (const Standard_Boolean theIsEnabled,
                                                 const Handle(AIS_InteractiveObject)& theObject)
{
  if (theObject.IsNull())
  {
    return;
  }

  theObject->SetIsoOnTriangulation (theIsEnabled);
}

//=======================================================================
//function : SetDeviationCoefficient
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDeviationCoefficient (const Handle(AIS_InteractiveObject)& theIObj,
                                                      const Standard_Real                  theCoefficient,
                                                      const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  // to be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if (theIObj->Type() != AIS_KOI_Object
   && theIObj->Type() != AIS_KOI_Shape)
  {
    return;
  }
  else if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (theIObj);
  aShape->SetOwnDeviationCoefficient (theCoefficient);
  redisplayPrsModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : SetHLRDeviationCoefficient
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetHLRDeviationCoefficient (const Handle(AIS_InteractiveObject)& theIObj,
                                                         const Standard_Real                  theCoefficient,
                                                         const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }
 
  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if (theIObj->Type() != AIS_KOI_Object
   && theIObj->Type() != AIS_KOI_Shape)
  {
    return;
  }
  else if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (theIObj);
  aShape->SetOwnHLRDeviationCoefficient (theCoefficient);
  redisplayPrsModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : SetDeviationAngle
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetDeviationAngle (const Handle(AIS_InteractiveObject)& theIObj,
                                                const Standard_Real                  theAngle,
                                                const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }
 
  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if (theIObj->Type() != AIS_KOI_Shape)
  {
    return;
  }
  else if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (theIObj);
  aShape->SetOwnDeviationAngle (theAngle);
  redisplayPrsModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : SetAngleAndDeviation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetAngleAndDeviation (const Handle(AIS_InteractiveObject)& theIObj,
                                                   const Standard_Real                  theAngle,
                                                   const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if (theIObj->Type() != AIS_KOI_Shape)
  {
    return;
  }
  if (theIObj->Signature() != 0)
  {
    return;
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (theIObj);
  aShape->SetAngleAndDeviation (theAngle);

  if (theIObj->RecomputeEveryPrs())
  {
    theIObj->Update (Standard_True);
    theIObj->UpdateSelection();
  }
  else
  {
    Update (theIObj, theToUpdateViewer);
  }
}

//=======================================================================
//function : SetHLRAngleAndDeviation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetHLRAngleAndDeviation (const Handle(AIS_InteractiveObject)& theIObj,
                                                      const Standard_Real                  theAngle,
                                                      const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if (theIObj->Type() != AIS_KOI_Shape)
  {
    return;
  }
  if (theIObj->Signature() != 0)
  {
    return;
  }
  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (theIObj);
  aShape->SetHLRAngleAndDeviation (theAngle);
  redisplayPrsModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : SetHLRDeviationAngle
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetHLRDeviationAngle (const Handle(AIS_InteractiveObject)& theIObj,
                                                   const Standard_Real                  theAngle,
                                                   const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if (theIObj->Type() != AIS_KOI_Shape)
  {
    return;
  }
  if (theIObj->Signature() != 0)
  {
    return;
  }
  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (theIObj);
  aShape->SetOwnHLRDeviationAngle (theAngle);
  redisplayPrsModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : UnsetColor
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetColor (const Handle(AIS_InteractiveObject)& theIObj,
                                         const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->UnsetColor();
  redisplayPrsRecModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : HasColor
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HasColor (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->HasColor();
}

//=======================================================================
//function : Color
//purpose  :
//=======================================================================
Quantity_NameOfColor AIS_InteractiveContext::Color (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->Color();
}

//=======================================================================
//function : Color
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Color (const Handle(AIS_InteractiveObject)& theIObj,
                                    Quantity_Color&                      theColor) const
{
  theIObj->Color (theColor);
}

//=======================================================================
//function : Width
//purpose  :
//=======================================================================
Standard_Real AIS_InteractiveContext::Width (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return theIObj->Width();
}

//=======================================================================
//function : SetWidth
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetWidth (const Handle(AIS_InteractiveObject)& theIObj,
                                       const Standard_Real                  theWidth,
                                       const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  theIObj->SetWidth (theWidth);
  redisplayPrsRecModes (theIObj, theToUpdateViewer);
  if (!myLastinMain.IsNull() && myLastinMain->Selectable() == theIObj)
  {
    if (myLastinMain->IsAutoHilight())
    {
      const Standard_Integer aHiMode =
        theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
      myLastinMain->HilightWithColor (myMainPM, myLastinMain->IsSelected() ? mySelectionColor : myHilightColor, aHiMode);
    }
    else
    {
      theIObj->HilightOwnerWithColor (myMainPM, myLastinMain->IsSelected() ? mySelectionColor : myHilightColor, myLastinMain);
    }
  }
}

//=======================================================================
//function : UnsetWidth
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetWidth (const Handle(AIS_InteractiveObject)& theIObj,
                                         const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->UnsetWidth();
  redisplayPrsRecModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : SetMaterial
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetMaterial (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Graphic3d_NameOfMaterial       theName,
                                          const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  theIObj->SetMaterial (theName);
  redisplayPrsRecModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : UnsetMaterial
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetMaterial (const Handle(AIS_InteractiveObject)& theIObj,
                                            const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }
  theIObj->UnsetMaterial();
  redisplayPrsRecModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : SetTransparency
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetTransparency (const Handle(AIS_InteractiveObject)& theIObj,
                                              const Standard_Real                  theValue,
                                              const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  if (!theIObj->IsTransparent()
    && theValue <= 0.05)
  {
    return;
  }

  if (theValue <= 0.05)
  {
    UnsetTransparency (theIObj, theToUpdateViewer);
    return;
  }

  theIObj->SetTransparency (theValue);
  redisplayPrsRecModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : UnsetTransparency
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetTransparency (const Handle(AIS_InteractiveObject)& theIObj,
                                                const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  theIObj->UnsetTransparency();
  redisplayPrsRecModes (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : SetSelectedAspect
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetSelectedAspect (const Handle(Prs3d_BasicAspect)& theAspect,
                                                const Standard_Boolean           theIsGlobalChange,
                                                const Standard_Boolean           theToUpdateViewer)
{
  if (HasOpenedContext())
  {
    return;
  }

  Standard_Boolean isFound = Standard_False;
  Handle(AIS_Selection) aSelIter = AIS_Selection::Selection (myCurrentName.ToCString());
  for (aSelIter->Init(); aSelIter->More(); aSelIter->Next())
  {
    isFound = Standard_True;
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (aSelIter->Value());
    anObj->SetAspect (theAspect, theIsGlobalChange);
  }

  if (isFound
   && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : SetLocalAttributes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetLocalAttributes (const Handle(AIS_InteractiveObject)& theIObj,
                                                 const Handle(Prs3d_Drawer)&          theDrawer,
                                                 const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }

  theIObj->SetAttributes (theDrawer);
  Update (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : UnsetLocalAttributes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnsetLocalAttributes (const Handle(AIS_InteractiveObject)& theIObj,
                                                   const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  if (!theIObj->HasInteractiveContext())
  {
    theIObj->SetContext (this);
  }
  theIObj->UnsetAttributes();
  Update (theIObj, theToUpdateViewer);
}

//=======================================================================
//function : Status
//purpose  :
//=======================================================================
void AIS_InteractiveContext::Status (const Handle(AIS_InteractiveObject)& theIObj,
                                     TCollection_ExtendedString&          theStatus) const
{
  theStatus = "";
  if (theIObj.IsNull()
  || !myObjects.IsBound (theIObj))
  {
    return;
  }

  theStatus += "\t ____________________________________________";
  theStatus += "\t| Known at Neutral Point:\n\tDisplayStatus:";
  const Handle(AIS_GlobalStatus)& aStatus = myObjects (theIObj);
  switch (aStatus->GraphicStatus())
  {
    case AIS_DS_Displayed:
    {
      theStatus += "\t| -->Displayed\n";
      break;
    }
    case AIS_DS_Erased:
    {
      theStatus += "\t| -->Erased\n";
      break;
    }
    default:
      break;
  }

  theStatus += "\t| Active Display Modes in the MainViewer :\n";
  for (TColStd_ListIteratorOfListOfInteger aDispModeIter (aStatus->DisplayedModes()); aDispModeIter.More(); aDispModeIter.Next())
  {
    theStatus += "\t|\t Mode ";
    theStatus += TCollection_AsciiString (aDispModeIter.Value());
    theStatus += "\n";
  }
  if (IsSelected(theIObj)) theStatus +="\t| Selected\n";

  theStatus += "\t| Active Selection Modes in the MainViewer :\n";
  for (TColStd_ListIteratorOfListOfInteger aSelModeIter (aStatus->SelectionModes()); aSelModeIter.More(); aSelModeIter.Next())
  {
    theStatus += "\t\t Mode ";
    theStatus += TCollection_AsciiString (aSelModeIter.Value());
    theStatus += "\n";
  }
  theStatus += "\t ____________________________________________";
}

//=======================================================================
//function : GetDefModes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::GetDefModes (const Handle(AIS_InteractiveObject)& theIObj,
                                          Standard_Integer&                    theDispMode,
                                          Standard_Integer&                    theHiMode,
                                          Standard_Integer&                    theSelMode) const
{
  if (theIObj.IsNull())
  {
    return;
  }

  theDispMode = theIObj->HasDisplayMode()
              ? theIObj->DisplayMode()
              : (theIObj->AcceptDisplayMode (myDisplayMode)
               ? myDisplayMode
               : 0);
  theHiMode  = theIObj->HasHilightMode()   ? theIObj->HilightMode()   : theDispMode;
  theSelMode = theIObj->GlobalSelectionMode();
}

//=======================================================================
//function : EraseGlobal
//purpose  :
//=======================================================================
void AIS_InteractiveContext::EraseGlobal (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Standard_Boolean               theToUpdateviewer)
{
  if (theIObj.IsNull()
  || !myObjects.IsBound (theIObj))
  {
    return;
  }

  Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);

  const Standard_Integer aDispMode = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
  if (aStatus->GraphicStatus() == AIS_DS_Temporary
   || aStatus->GraphicStatus() == AIS_DS_Erased)
  {
    return;
  }

  for (TColStd_ListIteratorOfListOfInteger aDispModeIter (aStatus->DisplayedModes()); aDispModeIter.More(); aDispModeIter.Next())
  {
    if (aStatus->IsHilighted())
    {
      if (IsCurrent (theIObj))
      {
        AddOrRemoveCurrentObject (theIObj, Standard_False);
      }
      else if (myMainPM->IsHighlighted (theIObj, aDispModeIter.Value()))
      {
        myMainPM->Unhighlight (theIObj, aDispModeIter.Value());
      }
    }

    myMainPM->SetVisibility (theIObj, aDispModeIter.Value(), Standard_False);
  }

  if (IsSelected (theIObj)
  && !aStatus->IsDModeIn (aDispMode))
  {
    myMainPM->SetVisibility (theIObj, aDispMode, Standard_False);
  }

  for (TColStd_ListIteratorOfListOfInteger aSelModeIter (aStatus->SelectionModes()); aSelModeIter.More(); aSelModeIter.Next())
  {
    mgrSelector->Deactivate (theIObj, aSelModeIter.Value(), myMainSel);
  }
  aStatus->ClearSelectionModes();
  aStatus->SetGraphicStatus (AIS_DS_Erased);

  if (theToUpdateviewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : unhighlightOwners
//purpose  :
//=======================================================================
void AIS_InteractiveContext::unhighlightOwners (const Handle(AIS_InteractiveObject)& theObject)
{
  Handle(AIS_Selection) aSel = AIS_Selection::Selection (myCurrentName.ToCString());
  aSel->Init();
  while (aSel->More())
  {
    const Handle(SelectMgr_EntityOwner) anOwner =
      Handle(SelectMgr_EntityOwner)::DownCast (aSel->Value());
    if (anOwner->Selectable() == theObject)
    {
      if (anOwner->IsSelected())
      {
        AddOrRemoveSelected (anOwner, Standard_False);
        aSel->Init();
        continue;
      }
    }
    aSel->Next();
  }
}

//=======================================================================
//function : ClearGlobal
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ClearGlobal (const Handle(AIS_InteractiveObject)& theIObj,
                                          const Standard_Boolean               theToUpdateviewer)
{
  if (theIObj.IsNull()
  || !myObjects.IsBound (theIObj))
  {
    // for cases when reference shape of connected interactives was not displayed
    // but its selection primitives were calculated
    const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
    mgrSelector->Remove (anObj);
    return;
  }

  Handle(AIS_GlobalStatus) aStatus = myObjects (theIObj);
  unhighlightOwners (theIObj);
  for (TColStd_ListIteratorOfListOfInteger aDispModeIter (aStatus->DisplayedModes()); aDispModeIter.More(); aDispModeIter.Next())
  {
    myMainPM->Erase (theIObj, aDispModeIter.Value());
    myMainPM->Clear (theIObj, aDispModeIter.Value());
  }

  // Object removes from Detected sequence
  for(Standard_Integer aDetIter = 1; aDetIter < myAISDetectedSeq.Length(); ++aDetIter)
  {
    Handle(AIS_InteractiveObject) anObj = DetectedCurrentObject();
    if (!anObj.IsNull()
      && anObj != theIObj)
    {
      myAISDetectedSeq.Remove (aDetIter);
    }
  }

  // remove IO from the selection manager to avoid memory leaks
  const Handle(SelectMgr_SelectableObject)& anObj = theIObj; // to avoid ambiguity
  mgrSelector->Remove (anObj);

  myObjects.UnBind (theIObj);
  myMainVwr->StructureManager()->UnregisterObject (theIObj);
  for (myMainVwr->InitDefinedViews(); myMainVwr->MoreDefinedViews(); myMainVwr->NextDefinedViews())
  {
    myMainVwr->DefinedView()->View()->ChangeHiddenObjects()->Remove (theIObj.get());
  }

  if (!myLastinMain.IsNull() && myLastinMain->Selectable() == theIObj)
    myLastinMain.Nullify();
  if (!myLastPicked.IsNull() && myLastPicked->Selectable() == theIObj)
    myLastPicked.Nullify();
  myMainPM->ClearImmediateDraw();

  if (theToUpdateviewer && aStatus->GraphicStatus() == AIS_DS_Displayed)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : ClearGlobalPrs
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ClearGlobalPrs (const Handle(AIS_InteractiveObject)& theIObj,
                                             const Standard_Integer               theMode,
                                             const Standard_Boolean               theToUpdateViewer)
{
  if (theIObj.IsNull()
  || !myObjects.IsBound (theIObj))
  {
    return;
  }

  const Handle(AIS_GlobalStatus)& aStatus = myObjects (theIObj);
  if (aStatus->IsDModeIn (theMode))
  {
    const Standard_Integer aDispMode = theIObj->HasHilightMode() ? theIObj->HilightMode() : 0;
    if (aDispMode == theMode
     && myMainPM->IsHighlighted (theIObj, theMode))
    {
      myMainPM->Unhighlight (theIObj, theMode);
    }

    myMainPM->Erase (theIObj, theMode);
    myMainPM->Clear (theIObj, theMode);
  }

  if (aStatus->GraphicStatus() == AIS_DS_Displayed
   && theToUpdateViewer)
  {
    myMainVwr->Update();
  }
}

//=======================================================================
//function : DrawHiddenLine
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::DrawHiddenLine() const
{
  return myDefaultDrawer->DrawHiddenLine();
}

//=======================================================================
//function : EnableDrawHiddenLine
//purpose  :
//=======================================================================
void AIS_InteractiveContext::EnableDrawHiddenLine() const
{
  myDefaultDrawer->EnableDrawHiddenLine();
}

//=======================================================================
//function : DisableDrawHiddenLine 
//purpose  :
//=======================================================================
void AIS_InteractiveContext::DisableDrawHiddenLine() const
{
  myDefaultDrawer->DisableDrawHiddenLine();
}

//=======================================================================
//function : HiddenLineAspect
//purpose  :
//=======================================================================
Handle (Prs3d_LineAspect) AIS_InteractiveContext::HiddenLineAspect() const
{
  return myDefaultDrawer->HiddenLineAspect();
}

//=======================================================================
//function : SetHiddenLineAspect
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetHiddenLineAspect (const Handle(Prs3d_LineAspect)& theAspect) const
{
  myDefaultDrawer->SetHiddenLineAspect (theAspect);
}

//=======================================================================
//function : SetIsoNumber
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetIsoNumber (const Standard_Integer theNb,
                                           const AIS_TypeOfIso    theType)
{
  switch (theType)
  {
    case AIS_TOI_IsoU:
      myDefaultDrawer->UIsoAspect()->SetNumber (theNb);
      break;
    case AIS_TOI_IsoV:
      myDefaultDrawer->VIsoAspect()->SetNumber (theNb);
      break;
    case AIS_TOI_Both:
      myDefaultDrawer->UIsoAspect()->SetNumber (theNb);
      myDefaultDrawer->VIsoAspect()->SetNumber (theNb);
      break;
  }
}

//=======================================================================
//function : IsoNumber
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::IsoNumber (const AIS_TypeOfIso theType)
{
  switch (theType)
  {
    case AIS_TOI_IsoU: return myDefaultDrawer->UIsoAspect()->Number();
    case AIS_TOI_IsoV: return myDefaultDrawer->VIsoAspect()->Number();
    case AIS_TOI_Both: return myDefaultDrawer->UIsoAspect()->Number() == myDefaultDrawer->VIsoAspect()->Number()
                            ? myDefaultDrawer->UIsoAspect()->Number()
                            : -1;
  }
  return 0;
}

//=======================================================================
//function : IsoOnPlane
//purpose  :
//=======================================================================
void AIS_InteractiveContext::IsoOnPlane (const Standard_Boolean theToSwitchOn)
{
  myDefaultDrawer->SetIsoOnPlane (theToSwitchOn);
}

//=======================================================================
//function : IsoOnPlane
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsoOnPlane() const
{
  return myDefaultDrawer->IsoOnPlane();
}

//=======================================================================
//function : IsoOnTriangulation
//purpose  :
//=======================================================================
void AIS_InteractiveContext::IsoOnTriangulation (const Standard_Boolean theToSwitchOn)
{
  myDefaultDrawer->SetIsoOnTriangulation (theToSwitchOn);
}

//=======================================================================
//function : IsoOnTriangulation
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsoOnTriangulation() const
{
  return myDefaultDrawer->IsoOnTriangulation();
}

//function : SetPixelTolerance
//purpose  : Disables the mechanism of adaptive tolerance calculation in
//           SelectMgr_ViewerSelector and sets the given tolerance for ALL
//           sensitive entities activated. For more information, see
//           SelectMgr_ViewerSelector.hxx
//=======================================================================
void AIS_InteractiveContext::SetPixelTolerance (const Standard_Integer thePrecision)
{
  if (HasOpenedContext())
  {
    myLocalContexts (myCurLocalIndex)->SetPixelTolerance (thePrecision);
  }
  else
  {
    myMainSel->SetPixelTolerance (thePrecision);
  }
}

//=======================================================================
//function : PixelTolerance
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::PixelTolerance() const
{
  return HasOpenedContext()
       ? myLocalContexts (myCurLocalIndex)->PixelTolerance()
       : myMainSel->PixelTolerance();
}

//=======================================================================
//function : SetSelectionSensitivity
//purpose  : Allows to manage sensitivity of a particular selection of interactive object theObject
//=======================================================================
void AIS_InteractiveContext::SetSelectionSensitivity (const Handle(AIS_InteractiveObject)& theObject,
                                                      const Standard_Integer theMode,
                                                      const Standard_Integer theNewSensitivity)
{
  if (HasOpenedContext())
  {
    myLocalContexts (myCurLocalIndex)->SetSelectionSensitivity (theObject, theMode, theNewSensitivity);
    return;
  }

  mgrSelector->SetSelectionSensitivity (theObject, theMode, theNewSensitivity);
}

//=======================================================================
//function : IsInLocal
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsInLocal (const Handle(AIS_InteractiveObject)& theIObj,
                                                    Standard_Integer&                    theIndex) const
{
  if (theIObj.IsNull())
  {
    return Standard_False;
  }

  // if it exists at neutral point 0 index is returned
  if (myObjects.IsBound (theIObj))
  {
    theIndex = 0;
    return Standard_False;
  }

  for (Standard_Integer aCtxIter = 1; aCtxIter <= myLocalContexts.Extent(); ++aCtxIter)
  {
    if (myLocalContexts.IsBound (aCtxIter))
    {
      if(myLocalContexts (aCtxIter)->IsIn (theIObj))
      {
        theIndex = aCtxIter;
        return Standard_True;
      }
    }
  }
  theIndex = -1;
  return Standard_False;
}

//=======================================================================
//function : InitAttributes
//purpose  :
//=======================================================================
void AIS_InteractiveContext::InitAttributes()
{
  mgrSelector->Add (myMainSel);
  myCurrentName   = AIS_Context_NewCurName();
  mySelectionName = AIS_Context_NewSelName();

  AIS_Selection::CreateSelection (mySelectionName.ToCString());
  AIS_Selection::CreateSelection (myCurrentName.ToCString());

  myDefaultDrawer->SetShadingAspectGlobal (Standard_False);
  Graphic3d_MaterialAspect aMat (Graphic3d_NOM_BRASS);
  myDefaultDrawer->ShadingAspect()->SetMaterial (aMat);

//  myDefaultDrawer->ShadingAspect()->SetColor(Quantity_NOC_GRAY70);
  Handle(Prs3d_LineAspect) aLineAspect = myDefaultDrawer->HiddenLineAspect();
  aLineAspect->SetColor      (Quantity_NOC_GRAY20);
  aLineAspect->SetWidth      (1.0);
  aLineAspect->SetTypeOfLine (Aspect_TOL_DASH);

  // tolerance to 2 pixels...
  SetPixelTolerance (2);

  // Customizing the drawer for trihedrons and planes...
  Handle(Prs3d_DatumAspect) aTrihAspect = myDefaultDrawer->DatumAspect();
  const Standard_Real aLength = 100.0;
  aTrihAspect->SetAxisLength (aLength, aLength, aLength);
  const Quantity_NameOfColor aColor = Quantity_NOC_LIGHTSTEELBLUE4;
  aTrihAspect->FirstAxisAspect() ->SetColor (aColor);
  aTrihAspect->SecondAxisAspect()->SetColor (aColor);
  aTrihAspect->ThirdAxisAspect() ->SetColor (aColor);

  Handle(Prs3d_PlaneAspect) aPlaneAspect = myDefaultDrawer->PlaneAspect();
  const Standard_Real aPlaneLength = 200.0;
  aPlaneAspect->SetPlaneLength (aPlaneLength, aPlaneLength);
  aPlaneAspect->EdgesAspect()->SetColor (Quantity_NOC_SKYBLUE);
}

//=======================================================================
//function : TrihedronSize
//purpose  :
//=======================================================================
Standard_Real AIS_InteractiveContext::TrihedronSize() const
{
  return myDefaultDrawer->DatumAspect()->FirstAxisLength();
}

//=======================================================================
//function : SetTrihedronSize
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetTrihedronSize (const Standard_Real    theVal,
                                               const Standard_Boolean /*updateviewer*/)
{
  myDefaultDrawer->DatumAspect()->SetAxisLength (theVal, theVal, theVal);
  Redisplay (AIS_KOI_Datum, 3, Standard_False);
  Redisplay (AIS_KOI_Datum, 4, Standard_True);
}

//=======================================================================
//function : SetPlaneSize
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetPlaneSize(const Standard_Real    theValX,
                                          const Standard_Real    theValY,
                                          const Standard_Boolean /*updateviewer*/)
{
  myDefaultDrawer->PlaneAspect()->SetPlaneLength (theValX, theValY);
  Redisplay (AIS_KOI_Datum, 7);
}

//=======================================================================
//function : SetPlaneSize
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetPlaneSize (const Standard_Real    theVal,
                                           const Standard_Boolean theToUpdateViewer)
{
  SetPlaneSize (theVal, theVal, theToUpdateViewer);
}

//=======================================================================
//function : PlaneSize
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::PlaneSize (Standard_Real& theX,
                                                    Standard_Real& theY) const
{
  theX = myDefaultDrawer->PlaneAspect()->PlaneXLength();
  theY = myDefaultDrawer->PlaneAspect()->PlaneYLength();
  return (Abs (theX - theY) <= Precision::Confusion());
}

//=======================================================================
//function : SetAutoActivateSelection
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetAutoActivateSelection (const Standard_Boolean theIsAuto)
{
  myIsAutoActivateSelMode = theIsAuto;
}

//=======================================================================
//function : GetAutoActivateSelection
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::GetAutoActivateSelection() const
{
  return myIsAutoActivateSelMode;
}

//=======================================================================
//function : SetZLayer
//purpose  :
//=======================================================================
void AIS_InteractiveContext::SetZLayer (const Handle(AIS_InteractiveObject)& theIObj,
                                        const Standard_Integer theLayerId)
{
  if (theIObj.IsNull())
    return;

  theIObj->SetZLayer (theLayerId);
}

//=======================================================================
//function : GetZLayer
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::GetZLayer (const Handle(AIS_InteractiveObject)& theIObj) const
{
  return !theIObj.IsNull()
       ?  theIObj->ZLayer()
       :  Graphic3d_ZLayerId_UNKNOWN;
}

//=======================================================================
//function : RebuildSelectionStructs
//purpose  : Rebuilds 1st level of BVH selection forcibly
//=======================================================================
void AIS_InteractiveContext::RebuildSelectionStructs()
{
  myMainSel->RebuildObjectsTree (Standard_True);
}

//=======================================================================
//function : Disconnect
//purpose  : Disconnects selectable object from an assembly and updates selection structures
//=======================================================================
void AIS_InteractiveContext::Disconnect (const Handle(AIS_InteractiveObject)& theAssembly,
                                         const Handle(AIS_InteractiveObject)& theObjToDisconnect)
{
  if (theAssembly->IsInstance ("AIS_MultipleConnectedInteractive"))
  {
    Handle(AIS_MultipleConnectedInteractive) theObj (Handle(AIS_MultipleConnectedInteractive)::DownCast (theAssembly));
    theObj->Disconnect (theObjToDisconnect);
    const Handle(SelectMgr_SelectableObject)& anObj = theObjToDisconnect; // to avoid ambiguity
    mgrSelector->Remove (anObj);
  }
  else if (theAssembly->IsInstance ("AIS_ConnectedInteractive") && theObjToDisconnect.IsNull())
  {
    Handle(AIS_ConnectedInteractive) theObj (Handle(AIS_ConnectedInteractive)::DownCast (theAssembly));
    theObj->Disconnect();
    const Handle(SelectMgr_SelectableObject)& anObj = theObj; // to avoid ambiguity
    mgrSelector->Remove (anObj);
  }
  else
    return;
}

//=======================================================================
//function : FitSelected
//purpose  : Fits the view corresponding to the bounds of selected objects
//=======================================================================
void AIS_InteractiveContext::FitSelected (const Handle(V3d_View)& theView,
                                          const Standard_Real theMargin,
                                          const Standard_Boolean theToUpdate)
{
  Standard_CString aSelName = HasOpenedContext() ?
      myLocalContexts (myCurLocalIndex)->SelectionName().ToCString()
    : myCurrentName.ToCString();

  Bnd_Box aBndSelected;

  const Handle(AIS_Selection)& aSelection = AIS_Selection::Selection (aSelName);
  AIS_MapOfObjectOwners anObjectOwnerMap;
  for (aSelection->Init(); aSelection->More(); aSelection->Next())
  {
    Handle(AIS_InteractiveObject) anObj (Handle(AIS_InteractiveObject)::DownCast (aSelection->Value()));
    if (!anObj.IsNull())
    {
      if (anObj->IsInfinite())
        continue;

      Bnd_Box aTmpBnd;
      anObj->BoundingBox (aTmpBnd);
      aBndSelected.Add (aTmpBnd);
    }
    else
    {
      Handle(SelectMgr_EntityOwner) anOwner (Handle(SelectMgr_EntityOwner)::DownCast (aSelection->Value()));
      if (anOwner.IsNull())
        continue;

      Handle(SelectMgr_IndexedMapOfOwner) anOwnerMap;
      if (!anObjectOwnerMap.Find (anOwner->Selectable(), anOwnerMap))
      {
        anOwnerMap = new SelectMgr_IndexedMapOfOwner();
        anObjectOwnerMap.Bind (anOwner->Selectable(), anOwnerMap);
      }

      anOwnerMap->Add (anOwner);
    }
  }

  for (AIS_MapIteratorOfMapOfObjectOwners anIter (anObjectOwnerMap); anIter.More(); anIter.Next())
  {
    const Handle(SelectMgr_SelectableObject) anObject = anIter.Key();
    Bnd_Box aTmpBox = anObject->BndBoxOfSelected (anIter.ChangeValue());
    aBndSelected.Add (aTmpBox);
  }

  anObjectOwnerMap.Clear();

  if (aBndSelected.IsVoid())
    return;

  theView->FitAll (aBndSelected, theMargin, theToUpdate);
}
