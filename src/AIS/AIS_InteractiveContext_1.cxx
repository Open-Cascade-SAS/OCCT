// Created on: 1997-01-29
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


#include <AIS_DataMapIteratorOfDataMapOfIOStatus.hxx>
#include <AIS_GlobalStatus.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_MapIteratorOfMapOfInteractive.hxx>
#include <AIS_MapOfInteractive.hxx>
#include <AIS_Selection.hxx>
#include <AIS_Shape.hxx>
#include <AIS_StatusOfDetection.hxx>
#include <AIS_StatusOfPick.hxx>
#include <Aspect_Grid.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Quantity_Color.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_OrFilter.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_Transient.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_Light.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_SpotLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

typedef NCollection_DataMap<Handle(AIS_InteractiveObject), NCollection_Handle<SelectMgr_SequenceOfOwner> > AIS_MapOfObjSelectedOwners;

//=======================================================================
//function : highlightWithColor
//purpose  :
//=======================================================================
void AIS_InteractiveContext::highlightWithColor (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                 const Quantity_NameOfColor theColor,
                                                 const Handle(V3d_Viewer)& theViewer)
{
  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());
  if (anObj.IsNull())
    return;
  const Standard_Integer aHiMode = anObj->HasHilightMode() ? anObj->HilightMode() : 0;

  myMainPM->BeginImmediateDraw();
  theOwner->HilightWithColor (myMainPM, theColor, aHiMode);
  myMainPM->EndImmediateDraw (theViewer.IsNull() ? myMainVwr : theViewer);
}

//=======================================================================
//function : highlightSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::highlightSelected (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                const Quantity_NameOfColor theSelColor)
{
  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());
  if (anObj.IsNull())
    return;
  const Standard_Integer aHiMode = anObj->HasHilightMode() ? anObj->HilightMode() : 0;

  if (!theOwner->IsAutoHilight())
  {
    AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());
    const Handle(AIS_Selection)& aCurSel = AIS_Selection::CurrentSelection();
    SelectMgr_SequenceOfOwner aSeq;
    for (aCurSel->Init(); aCurSel->More(); aCurSel->Next())
    {
      const Handle(SelectMgr_EntityOwner) aSelOwnr =
        Handle(SelectMgr_EntityOwner)::DownCast (aCurSel->Value());
      if (aSelOwnr->Selectable() != anObj)
        continue;
      aSeq.Append (aSelOwnr);
    }
    anObj->HilightSelected (myMainPM, aSeq);
  }
  else
  {
    theOwner->HilightWithColor (myMainPM, theSelColor, aHiMode);
  }
}

//=======================================================================
//function : unhighlightSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::unhighlightSelected (const Standard_Boolean theIsToHilightSubIntensity)
{
  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());
  Handle(AIS_Selection) aCurSel = AIS_Selection::Selection (myCurrentName.ToCString());
  NCollection_IndexedMap<Handle(AIS_InteractiveObject)> anObjToClear;
  for (aCurSel->Init(); aCurSel->More(); aCurSel->Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner =
      Handle(SelectMgr_EntityOwner)::DownCast (aCurSel->Value());
    if (anOwner.IsNull() || !anOwner->HasSelectable())
      continue;

    const Handle(AIS_InteractiveObject) anInteractive =
      Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
    if (anOwner->IsAutoHilight())
    {
      const Standard_Integer aHiMode = anInteractive->HasHilightMode() ? anInteractive->HilightMode() : 0;
      anOwner->Unhilight (myMainPM, aHiMode);
      if (theIsToHilightSubIntensity)
      {
        if (myObjects.IsBound (anInteractive) && myObjects (anInteractive)->IsSubIntensityOn())
        {
          anOwner->HilightWithColor (myMainPM, mySubIntensity, aHiMode);
        }
      }
    }
    else
    {
      if (!anObjToClear.Contains (anInteractive))
        anObjToClear.Add (anInteractive);
    }
    anOwner->State (0);
    if (anOwner == anInteractive->GlobalSelOwner())
    {
      myObjects.ChangeFind (anInteractive)->SetHilightStatus (Standard_False);
    }
  }
  for (NCollection_IndexedMap<Handle(AIS_InteractiveObject)>::Iterator anIter (anObjToClear); anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anObj = anIter.Value();
    const Standard_Integer aHiMode = anObj->HasHilightMode() ? anObj->HilightMode() : 0;
    myMainPM->Unhighlight (anObj, aHiMode);
    anObj->ClearSelected();
  }
}

//=======================================================================
//function : MoveTo
//purpose  :
//=======================================================================
AIS_StatusOfDetection AIS_InteractiveContext::MoveTo (const Standard_Integer  theXPix,
                                                      const Standard_Integer  theYPix,
                                                      const Handle(V3d_View)& theView,
                                                      const Standard_Boolean  theToRedrawOnUpdate)
{
  if (HasOpenedContext())
  {
    myWasLastMain = Standard_True;
    return myLocalContexts (myCurLocalIndex)->MoveTo (theXPix, theYPix, theView, theToRedrawOnUpdate);
  }

  myAISCurDetected = 0;
  myAISDetectedSeq.Clear();

  if (theView->Viewer() != myMainVwr)
  {
    return AIS_SOD_Error;
  }

  // preliminaires
  myLastPicked  = myLastinMain;
  myWasLastMain = Standard_True;
  AIS_StatusOfDetection aStatus        = AIS_SOD_Nothing;
  Standard_Boolean      toUpdateViewer = Standard_False;

  myFilters->SetDisabledObjects (theView->View()->HiddenObjects());
  myMainSel->Pick (theXPix, theYPix, theView);

  // filling of myAISDetectedSeq sequence storing information about detected AIS objects
  // (the objects must be AIS_Shapes)
  const Standard_Integer aDetectedNb = myMainSel->NbPicked();
  Standard_Integer aNewDetected = 0;
  for (Standard_Integer aDetIter = 1; aDetIter <= aDetectedNb; ++aDetIter)
  {
    Handle(SelectMgr_EntityOwner) anOwner = myMainSel->Picked (aDetIter);
    if (anOwner.IsNull()
     || !myFilters->IsOk (anOwner))
    {
      continue;
    }

    if (aNewDetected < 1)
    {
      aNewDetected = aDetIter;
    }
    Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
    if (!anObj.IsNull())
    {
      myAISDetectedSeq.Append (anObj);
    }
  }

  if (aNewDetected >= 1)
  {
    // does nothing if previously detected object is equal to the current one
    Handle(SelectMgr_EntityOwner) aNewPickedOwner = myMainSel->Picked (aNewDetected);
    if (aNewPickedOwner == myLastPicked)
    {
      return myLastPicked->IsSelected()
           ? AIS_SOD_Selected
           : AIS_SOD_OnlyOneDetected;
    }
 
    // Previously detected object is unhilighted if it is not selected or hilighted 
    // with selection color if it is selected. Such highlighting with selection color 
    // is needed only if myToHilightSelected flag is true. In this case previously detected
    // object has been already highlighted with myHilightColor during previous MoveTo() 
    // method call. As result it is necessary to rehighligt it with mySelectionColor.
    if (!myLastPicked.IsNull() && myLastPicked->HasSelectable())
    {
      myMainPM->ClearImmediateDraw();
      const Handle(AIS_InteractiveObject) aLastPickedAIS = Handle(AIS_InteractiveObject)::DownCast (myLastPicked->Selectable());
      const Standard_Integer aHiMod = aLastPickedAIS->HasHilightMode() ? aLastPickedAIS->HilightMode() : 0;
      if (!myLastPicked->IsSelected())
      {
        myLastPicked->Unhilight (myMainPM, aHiMod);
        toUpdateViewer = Standard_True;
      }
      else if (myToHilightSelected)
      {
        highlightWithColor (aNewPickedOwner, mySelectionColor, theView->Viewer());
        toUpdateViewer = Standard_True;
      }
    }

    // initialize myLastPicked field with currently detected object
    myLastPicked = aNewPickedOwner;
    myLastinMain = myLastPicked;

    // highlight detected object if it is not selected or myToHilightSelected flag is true
    if (myLastPicked->HasSelectable())
    {
      if (!myLastPicked->IsSelected() || myToHilightSelected)
      {
        highlightWithColor (myLastPicked, myHilightColor, theView->Viewer());
        toUpdateViewer = Standard_True;
      }

      aStatus = myLastPicked->IsSelected()
              ? AIS_SOD_Selected
              : AIS_SOD_OnlyOneDetected;
    }
  }
  else
  {
    // previously detected object is unhilighted if it is not selected or hilighted
    // with selection color if it is selected
    aStatus = AIS_SOD_Nothing;
    if (!myLastPicked.IsNull() && myLastPicked->HasSelectable())
    {
      myMainPM->ClearImmediateDraw();
      const Handle(AIS_InteractiveObject) aLastPickedAIS = Handle(AIS_InteractiveObject)::DownCast (myLastPicked->Selectable());
      Standard_Integer aHiMod = aLastPickedAIS->HasHilightMode() ? aLastPickedAIS->HilightMode() : 0;
      if (!myLastPicked->IsSelected())
      {
        if (myLastPicked->IsAutoHilight())
        {
          myLastPicked->Unhilight (myMainPM, aHiMod);
        }
        toUpdateViewer = Standard_True;
      }
      else if (myToHilightSelected)
      {
        highlightSelected (myLastPicked, mySelectionColor);
        toUpdateViewer = Standard_True;
      }
    }

    myLastinMain.Nullify();
    myLastPicked.Nullify();
  }

  if (toUpdateViewer
   && theToRedrawOnUpdate)
  {
    theView->Viewer()->Update();
  }

  mylastmoveview = theView;
  return aStatus;
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_InteractiveContext::Select (const Standard_Integer  theXPMin,
                                                 const Standard_Integer  theYPMin,
                                                 const Standard_Integer  theXPMax,
                                                 const Standard_Integer  theYPMax,
                                                 const Handle(V3d_View)& theView,
                                                 const Standard_Boolean  toUpdateViewer)
{
  // all objects detected by the selector are taken, previous current objects are emptied,
  // new objects are put...

  if (HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->Select (theXPMin, theYPMin,
                                                     theXPMax, theYPMax,
                                                     theView, toUpdateViewer);
  }

  ClearSelected (Standard_False);

  Handle(StdSelect_ViewerSelector3d) aSelector;

  if (theView->Viewer() == myMainVwr)
  {
    aSelector = myMainSel;
    myWasLastMain = Standard_True;
  }

  aSelector->Pick (theXPMin, theYPMin, theXPMax, theYPMax, theView);
  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());

  for (aSelector->Init(); aSelector->More(); aSelector->Next())
  {
    const Handle(SelectMgr_EntityOwner)& aCurOwner = aSelector->Picked();
    if (aCurOwner.IsNull() || !aCurOwner->HasSelectable() || !myFilters->IsOk (aCurOwner))
      continue;

    AIS_Selection::Select (aCurOwner);
    aCurOwner->State (1);
  }

  HilightSelected (toUpdateViewer);

  Standard_Integer aSelNum = NbSelected();

  return (aSelNum == 0) ? AIS_SOP_NothingSelected
                        : (aSelNum == 1) ? AIS_SOP_OneSelected
                                         : AIS_SOP_SeveralSelected;
  
}

//=======================================================================
//function : Select
//purpose  : Selection by polyline
//=======================================================================
AIS_StatusOfPick AIS_InteractiveContext::Select (const TColgp_Array1OfPnt2d& thePolyline,
                                                 const Handle(V3d_View)&     theView,
                                                 const Standard_Boolean      toUpdateViewer)
{
  // all objects detected by the selector are taken, previous current objects are emptied,
  // new objects are put...

  if (HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->Select (thePolyline, theView, toUpdateViewer);
  }

  ClearSelected (Standard_False);

  Handle(StdSelect_ViewerSelector3d) aSelector;

  if (theView->Viewer() == myMainVwr)
  {
    aSelector = myMainSel;
    myWasLastMain = Standard_True;
  }

  aSelector->Pick (thePolyline, theView);
  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());

  for (aSelector->Init(); aSelector->More(); aSelector->Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner =
      Handle(SelectMgr_EntityOwner)::DownCast (aSelector->Picked());
    if (anOwner.IsNull() || !anOwner->HasSelectable() || !myFilters->IsOk (anOwner))
      continue;

    AIS_Selection::Select (anOwner);
    anOwner->State (1);
  }

  HilightSelected (toUpdateViewer);

  Standard_Integer aSelNum = NbSelected();

  return (aSelNum == 0) ? AIS_SOP_NothingSelected
                        : (aSelNum == 1) ? AIS_SOP_OneSelected
                                         : AIS_SOP_SeveralSelected;
  
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_InteractiveContext::Select (const Standard_Boolean toUpdateViewer)
{
  if (HasOpenedContext())
  {
    if(myWasLastMain)
    {
      return myLocalContexts(myCurLocalIndex)->Select (toUpdateViewer);
    }
    else
    {
      myLocalContexts(myCurLocalIndex)->SetSelected (Handle(AIS_InteractiveObject)::DownCast (myLastPicked->Selectable()), toUpdateViewer);
      return AIS_SOP_OneSelected;
    }
  }

  myMainPM->ClearImmediateDraw();
  if (myWasLastMain && !myLastinMain.IsNull())
  {
    if(!myLastinMain->IsSelected())
    {
      SetSelected (myLastinMain, Standard_False);
      if(toUpdateViewer)
      {
        UpdateCurrentViewer();
      }
    }
  }
  else
  {
    unhighlightSelected (Standard_True);

    AIS_Selection::Select();
    if (toUpdateViewer && myWasLastMain)
    {
        UpdateCurrentViewer();
    }
  }

  Standard_Integer aSelNum = NbSelected();

  return (aSelNum == 0) ? AIS_SOP_NothingSelected
                        : (aSelNum == 1) ? AIS_SOP_OneSelected
                                         : AIS_SOP_SeveralSelected;
}

//=======================================================================
//function : ShiftSelect
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_InteractiveContext::ShiftSelect (const Standard_Boolean toUpdateViewer)
{
  if (HasOpenedContext())
  {
    if(myWasLastMain)
    {
      return myLocalContexts (myCurLocalIndex)->ShiftSelect (toUpdateViewer);
    }
    else
    {
      myLocalContexts (myCurLocalIndex)->AddOrRemoveSelected (myLastPicked, toUpdateViewer);

      Standard_Integer aSelNum = NbSelected();
      return (aSelNum == 0) ? AIS_SOP_NothingSelected
                            : (aSelNum == 1) ? AIS_SOP_OneSelected
                                             : AIS_SOP_SeveralSelected;
    }
  }

  myMainPM->ClearImmediateDraw();
  if (myWasLastMain && !myLastinMain.IsNull())
  {
    AddOrRemoveSelected (myLastinMain, toUpdateViewer);
  }

  Standard_Integer aSelNum = NbSelected();

  return (aSelNum == 0) ? AIS_SOP_NothingSelected
                        : (aSelNum == 1) ? AIS_SOP_OneSelected
                        : AIS_SOP_SeveralSelected;
}

//=======================================================================
//function : ShiftSelect
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_InteractiveContext::ShiftSelect (const Standard_Integer theXPMin,
                                                      const Standard_Integer theYPMin,
                                                      const Standard_Integer theXPMax,
                                                      const Standard_Integer theYPMax,
                                                      const Handle(V3d_View)& theView,
                                                      const Standard_Boolean toUpdateViewer)
{
  if (HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->ShiftSelect (theXPMin, theYPMin, theXPMax, theYPMax,
                                                          theView, toUpdateViewer);
  }

  UnhilightSelected (Standard_False);

  Handle(StdSelect_ViewerSelector3d) aSelector;
  if (theView->Viewer() == myMainVwr)
  {
    aSelector = myMainSel;
    myWasLastMain = Standard_True;
  }
  else
  {
    return AIS_SOP_NothingSelected;
  }

  aSelector->Pick (theXPMin, theYPMin, theXPMax, theYPMax, theView);
  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());
  for (aSelector->Init(); aSelector->More(); aSelector->Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner =  Handle(SelectMgr_EntityOwner)::DownCast (aSelector->Picked());
    if (anOwner.IsNull() || !anOwner->HasSelectable() || !myFilters->IsOk (anOwner))
      continue;

    AIS_SelectStatus aSelStatus = AIS_Selection::Select (anOwner);
    Standard_Integer aState = (aSelStatus == AIS_SS_Added) ? 1 : 0;
    anOwner->State (aState);
  }

  HilightSelected (toUpdateViewer);

  Standard_Integer aSelNum = NbSelected();

  return (aSelNum == 0) ? AIS_SOP_NothingSelected
                        : (aSelNum == 1) ? AIS_SOP_OneSelected
                                         : AIS_SOP_SeveralSelected;

}

//=======================================================================
//function : ShiftSelect
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_InteractiveContext::ShiftSelect (const TColgp_Array1OfPnt2d& thePolyline,
                                                      const Handle(V3d_View)& theView,
                                                      const Standard_Boolean toUpdateViewer)
{
  if (HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->ShiftSelect (thePolyline, theView, toUpdateViewer);
  }

  UnhilightSelected (Standard_False);

  Handle(StdSelect_ViewerSelector3d) aSelector;

  if (theView->Viewer() == myMainVwr)
  {
    aSelector= myMainSel;
    myWasLastMain = Standard_True;
  }
  else
  {
    return AIS_SOP_NothingSelected;
  }

  aSelector->Pick (thePolyline, theView);

  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());
  for (aSelector->Init(); aSelector->More(); aSelector->Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner =  Handle(SelectMgr_EntityOwner)::DownCast (aSelector->Picked());
    if (anOwner.IsNull() || !anOwner->HasSelectable() || !myFilters->IsOk (anOwner))
      continue;

    AIS_SelectStatus aSelStatus = AIS_Selection::Select (anOwner);
    Standard_Integer aState = (aSelStatus == AIS_SS_Added) ? 1 : 0;
    anOwner->State (aState);
  }

  HilightSelected (toUpdateViewer);

  Standard_Integer aSelNum = NbSelected();

  return (aSelNum == 0) ? AIS_SOP_NothingSelected
                        : (aSelNum == 1) ? AIS_SOP_OneSelected
                                         : AIS_SOP_SeveralSelected;
}

//=======================================================================
//function : SetCurrentObject
//purpose  : OBSOLETE, please use SetSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
void AIS_InteractiveContext::SetCurrentObject (const Handle(AIS_InteractiveObject)& theObject,
                                               const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
    return;

  SetSelected (theObject, theToUpdateViewer);
}

//=======================================================================
//function : AddOrRemoveCurrentObject
//purpose  : OBSOLETE, please use AddOrRemoveSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
void AIS_InteractiveContext::AddOrRemoveCurrentObject (const Handle(AIS_InteractiveObject)& theObj,
                                                       const Standard_Boolean theIsToUpdateViewer)
{
  if (HasOpenedContext())
    return;

  AddOrRemoveSelected (theObj, theIsToUpdateViewer);
}
//=======================================================================
//function : UpdateCurrent
//purpose  : OBSOLETE, please use UpdateSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
void AIS_InteractiveContext::UpdateCurrent()
{
  UpdateSelected();
}

//=======================================================================
//function : IsCurrent
//purpose  : OBSOLETE, please use IsSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsCurrent (const Handle(AIS_InteractiveObject)& theObject) const
{
  return IsSelected (theObject);
}

//=======================================================================
//function : InitCurrent
//purpose  : OBSOLETE, please use InitSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
void AIS_InteractiveContext::InitCurrent()
{
  if (HasOpenedContext())
    return;

  InitSelected();
}

//=======================================================================
//function : MoreCurrent
//purpose  : OBSOLETE, please use MoreSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
Standard_Boolean AIS_InteractiveContext::MoreCurrent() const 
{
  return !HasOpenedContext() && MoreSelected();
}

//=======================================================================
//function : NextCurrent
//purpose  : OBSOLETE, please use NextSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
void AIS_InteractiveContext::NextCurrent()
{
  if (HasOpenedContext())
    return;

  NextSelected();
}

//=======================================================================
//function : Current
//purpose  : OBSOLETE, please use SelectedInteractive() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
Handle(AIS_InteractiveObject) AIS_InteractiveContext::Current() const 
{
  return HasOpenedContext() ? NULL : SelectedInteractive();
}

//=======================================================================
//function : NbCurrents
//purpose  : OBSOLETE, please use NbSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
Standard_Integer AIS_InteractiveContext::NbCurrents()
{
  return HasOpenedContext() ? -1 : NbSelected();
}

//=======================================================================
//function : HilightCurrents
//purpose  : OBSOLETE, please use HilightSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
void AIS_InteractiveContext::HilightCurrents (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
    return;

  HilightSelected (theToUpdateViewer);
}

//=======================================================================
//function : UnhilightCurrents
//purpose  : OBSOLETE, please use UnhilightSelected() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
void AIS_InteractiveContext::UnhilightCurrents (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
    return;

  UnhilightSelected (theToUpdateViewer);
}

//=======================================================================
//function : ClearCurrents
//purpose  : OBSOLETE, please use ClearCurrents() instead
//TODO     : Remove in process of local context deletion
//=======================================================================
void AIS_InteractiveContext::ClearCurrents(const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
    return;

  ClearSelected (theToUpdateViewer);
}


//=======================================================================
//function : HilightSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::HilightSelected (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
  {
    return myLocalContexts (myCurLocalIndex)->HilightPicked (theToUpdateViewer);
  }

  // In case of selection without using local context
  myMainPM->ClearImmediateDraw();
  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());
  Handle(AIS_Selection) aSel = AIS_Selection::Selection (myCurrentName.ToCString());
  AIS_MapOfObjSelectedOwners anObjOwnerMap;
  for (aSel->Init(); aSel->More(); aSel->Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner =
      Handle(SelectMgr_EntityOwner)::DownCast (aSel->Value());
    if (!anOwner.IsNull() && anOwner->HasSelectable())
    {
      const Handle(AIS_InteractiveObject) anObj =
        Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
      if (anOwner == anObj->GlobalSelOwner())
      {
        Handle(AIS_GlobalStatus)& aState = myObjects.ChangeFind (anObj);
        aState->SetHilightStatus (Standard_True);
        aState->SetHilightColor (mySelectionColor);
      }
      anOwner->State (1);
      if (!anOwner->IsAutoHilight())
      {
        NCollection_Handle<SelectMgr_SequenceOfOwner> aSeq;
        if (anObjOwnerMap.Find (anObj, aSeq))
        {
          aSeq->Append (anOwner);
        }
        else
        {
          aSeq = new SelectMgr_SequenceOfOwner();
          aSeq->Append (anOwner);
          anObjOwnerMap.Bind (anObj, aSeq);
        }
      }
      else
      {
        const Standard_Integer aHiMode = anObj->HasHilightMode() ? anObj->HilightMode() : 0;
        anOwner->HilightWithColor (myMainPM, mySelectionColor, aHiMode);
      }
    }
  }

  if (!anObjOwnerMap.IsEmpty())
  {
    for (AIS_MapOfObjSelectedOwners::Iterator anIter (anObjOwnerMap); anIter.More(); anIter.Next())
    {
      anIter.Key()->HilightSelected (myMainPM, *anIter.Value());
    }
    anObjOwnerMap.Clear();
  }

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : UnhilightSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnhilightSelected (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
  {
    return myLocalContexts (myCurLocalIndex)->UnhilightPicked (theToUpdateViewer);
  }

  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());
  Handle(AIS_Selection) aSel = AIS_Selection::Selection (myCurrentName.ToCString());
  for (aSel->Init(); aSel->More(); aSel->Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner =
      Handle(SelectMgr_EntityOwner)::DownCast (aSel->Value());
    if (!anOwner.IsNull() && anOwner->HasSelectable())
    {
      const Handle(AIS_InteractiveObject) anObj =
        Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
      if (anOwner == anObj->GlobalSelOwner())
      {
        myObjects.ChangeFind (anObj)->SetHilightStatus (Standard_False);
      }
      anOwner->State (0);
      const Standard_Integer aHiMode = anObj->HasHilightMode() ? anObj->HasHilightMode() : 0;
      anOwner->Unhilight (myMainPM, aHiMode);
    }
  }

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}


//=======================================================================
//function : ClearSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ClearSelected (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
    return myLocalContexts (myCurLocalIndex)->ClearSelected (theToUpdateViewer);

  if (NbSelected() == 0)
    return;

  unhighlightSelected();

  AIS_Selection::Select();
  myMainPM->ClearImmediateDraw();

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : UpdateSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UpdateSelected (const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->UpdateSelected (theToUpdateViewer);
  }

  HilightSelected (theToUpdateViewer);
}

//=======================================================================
//function : SetSelected
//purpose  : Sets the whole object as selected and highlights it with selection color
//=======================================================================
void AIS_InteractiveContext::SetSelected (const Handle(AIS_InteractiveObject)& theObject,
                                          const Standard_Boolean theToUpdateViewer)
{
  if(HasOpenedContext())
  {
    return myLocalContexts (myCurLocalIndex)->SetSelected (theObject, theToUpdateViewer);
  }

  if (theObject.IsNull())
    return;
  if(!myObjects.IsBound (theObject))
    Display (theObject, Standard_False);
  if (theObject->HasSelection (0))
    return;

  if (NbSelected() == 1 && myObjects (theObject)->IsHilighted())
  {
    Quantity_NameOfColor aHiCol;
    Standard_Boolean hasHiCol = Standard_False;
    if (IsHilighted (theObject, hasHiCol, aHiCol))
    {
      if (hasHiCol && aHiCol!= mySelectionColor)
      {
        HilightWithColor (theObject, mySelectionColor, theToUpdateViewer);
      }
    }
    return;
  }

  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());
  Handle(AIS_Selection) aCurSel = AIS_Selection::Selection (myCurrentName.ToCString());
  for (aCurSel->Init(); aCurSel->More(); aCurSel->Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner =
      Handle(SelectMgr_EntityOwner)::DownCast (aCurSel->Value());
    if (anOwner.IsNull() || !anOwner->HasSelectable() || !myFilters->IsOk (anOwner))
      continue;

    Handle(AIS_InteractiveObject) aSelectable =
      Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
    Unhilight (aSelectable, Standard_False);
    anOwner->State (0);
    if (anOwner == aSelectable->GlobalSelOwner())
    {
      myObjects.ChangeFind (aSelectable)->SetHilightStatus (Standard_False);
    }
  }

  // added to avoid untimely viewer update...
  const Handle(SelectMgr_Selection)& aSel = theObject->Selection (0);
  if (aSel->IsEmpty())
    return;
  aSel->Init();
  Handle(SelectMgr_EntityOwner) anOwner =
    Handle(SelectMgr_EntityOwner)::DownCast (aSel->Sensitive()->BaseSensitive()->OwnerId());
  AIS_Selection::ClearAndSelect (anOwner);
  anOwner->State (1);
  if (anOwner == theObject->GlobalSelOwner())
  {
    Handle(AIS_GlobalStatus)& aState = myObjects.ChangeFind (theObject);
    aState->SetHilightStatus (Standard_True);
    aState->SetHilightColor (mySelectionColor);
  }
  Quantity_NameOfColor aHiCol;
  Standard_Boolean hasHiCol = Standard_False;
  if (IsHilighted (theObject, hasHiCol, aHiCol))
  {
    if (hasHiCol && aHiCol!= mySelectionColor)
    {
      HilightWithColor (theObject, mySelectionColor, Standard_False);
    }
  }
  else
  {
    HilightWithColor (theObject, mySelectionColor, Standard_False);
  }

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : SetSelected
//purpose  : Sets the whole object as selected and highlights it with selection color
//=======================================================================
void AIS_InteractiveContext::SetSelected (const Handle(SelectMgr_EntityOwner)& theOwner,
                                          const Standard_Boolean theToUpdateViewer)
{
  if (theOwner.IsNull() || !theOwner->HasSelectable() || !myFilters->IsOk (theOwner))
    return;

  const Handle(AIS_InteractiveObject) anObject =
    Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());

  if (NbSelected() == 1 && theOwner->IsSelected())
  {
    Quantity_NameOfColor aCustomColor;
    Standard_Boolean isCustomColorSet;
    if (IsHilighted (theOwner, isCustomColorSet, aCustomColor))
    {
      if (isCustomColorSet && aCustomColor != mySelectionColor)
      {
        const Standard_Integer aHiMode = anObject->HasHilightMode() ? anObject->HilightMode() : 0;
        theOwner->HilightWithColor (myMainPM, mySelectionColor, aHiMode);
      }
    }
    return;
  }

  if (!myObjects.IsBound (anObject))
    Display (anObject, Standard_False);

  unhighlightSelected();

  AIS_Selection::ClearAndSelect (theOwner);
  theOwner->State (1);
  Quantity_NameOfColor aCustomColor;
  Standard_Boolean isCustomColorSet;
  if (!IsHilighted (theOwner, isCustomColorSet, aCustomColor) || (isCustomColorSet && aCustomColor!= mySelectionColor))
  {
    highlightSelected (theOwner, mySelectionColor);
  }

  if (theOwner == anObject->GlobalSelOwner())
  {
    Handle(AIS_GlobalStatus)& aState = myObjects.ChangeFind (anObject);
    aState->SetHilightStatus (Standard_True);
    aState->SetHilightColor (mySelectionColor);
  }

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : AddOrRemoveSelected
//purpose  : Adds or removes current object from AIS selection and highlights/unhighlights it.
//           Since this method makes sence only for neutral point selection of a whole object,
//           if 0 selection of the object is empty this method simply does nothing.
//=======================================================================
void AIS_InteractiveContext::AddOrRemoveSelected (const Handle(AIS_InteractiveObject)& theObject,
                                                  const Standard_Boolean theToUpdateViewer)
{
  if (!theObject->HasInteractiveContext())
    theObject->SetContext (this);

  if (HasOpenedContext())
    return myLocalContexts (myCurLocalIndex)->AddOrRemoveSelected (theObject, theToUpdateViewer);

  if (theObject.IsNull() || !myObjects.IsBound (theObject) || !theObject->HasSelection (0))
    return;

  const Handle(SelectMgr_Selection)& aSel = theObject->Selection (0);

  if (aSel->IsEmpty())
    return;

  aSel->Init();
  const Handle(SelectMgr_EntityOwner) anOwner =
    Handle(SelectMgr_EntityOwner)::DownCast (aSel->Sensitive()->BaseSensitive()->OwnerId());

  if (anOwner.IsNull() || !anOwner->HasSelectable())
    return;

  AddOrRemoveSelected (anOwner, theToUpdateViewer);
}
//=======================================================================
//function : AddOrRemoveSelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::AddOrRemoveSelected (const TopoDS_Shape& aShap,
				            const Standard_Boolean updateviewer)
{ 
  if(!HasOpenedContext()) {
#ifdef OCCT_DEBUG
    cout<<" Attempt to remove a selected shape with no opened local context"<<endl;
#endif
    return;
  }
  
  myLocalContexts(myCurLocalIndex)->AddOrRemoveSelected(aShap,updateviewer);
  if(updateviewer) UpdateCurrentViewer();
  
}

//=======================================================================
//function : AddOrRemoveSelected
//purpose  : Allows to highlight or unhighlight the owner given depending on
//           its selection status
//=======================================================================
void AIS_InteractiveContext::AddOrRemoveSelected (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                  const Standard_Boolean theToUpdateViewer)
{
  if (HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->AddOrRemoveSelected (theOwner, theToUpdateViewer);

  if (theOwner.IsNull() || !theOwner->HasSelectable())
    return;

  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());
  Handle(AIS_Selection) aCurSel = AIS_Selection::Selection (myCurrentName.ToCString());

  AIS_SelectStatus aSelStat = AIS_Selection::Select (theOwner);

  Standard_Integer aState = aSelStat == AIS_SS_Added ?  1 : 0;
  theOwner->State (aState);
  const Handle(AIS_InteractiveObject) anObj =
    Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());
  const Standard_Boolean isGlobal = anObj->GlobalSelOwner() == theOwner;
  Handle(AIS_GlobalStatus)& aStatus = myObjects.ChangeFind (anObj);
  const Standard_Integer aHiMode = anObj->HasHilightMode() ? anObj->HilightMode() : 0;
  if (aState == 1)
  {
    highlightSelected (theOwner, mySelectionColor);
    if (isGlobal)
    {
      aStatus->SetHilightStatus (Standard_True);
      aStatus->SetHilightColor (mySelectionColor);
    }
  }
  else
  {
    if (theOwner->IsAutoHilight())
      theOwner->Unhilight (myMainPM, aHiMode);
    else
      anObj->ClearSelected();
    aStatus->SetHilightStatus (Standard_False);
    aStatus->SetHilightColor (Quantity_NOC_WHITE);
  }

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}


//=======================================================================
//function : IsSelected
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsSelected (const Handle(AIS_InteractiveObject)& theObj) const
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->IsSelected (theObj);

  if (theObj.IsNull() || !myObjects.IsBound (theObj))
    return Standard_False;

  const Standard_Integer aGlobalSelMode = theObj->GlobalSelectionMode();
  const TColStd_ListOfInteger& anActivatedModes = myObjects (theObj)->SelectionModes();
  Standard_Boolean isGlobalModeActivated = Standard_False;
  for (TColStd_ListIteratorOfListOfInteger aModeIter (anActivatedModes); aModeIter.More(); aModeIter.Next())
  {
    if (aModeIter.Value() == aGlobalSelMode)
    {
      isGlobalModeActivated = Standard_True;
      break;
    }
  }
  if (!theObj->HasSelection (aGlobalSelMode) || !isGlobalModeActivated)
    return Standard_False;

  return myObjects (theObj)->IsHilighted();
}

//=======================================================================
//function : IsSelected
//purpose  : Returns true is the owner given is selected
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsSelected (const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  if (HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->IsSelected (theOwner);

  if (theOwner.IsNull())
    return Standard_False;

  return theOwner->IsSelected();
}

//=======================================================================
//function : InitSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::InitSelected()
{
  if (HasOpenedContext())
  {
    myLocalContexts (myCurLocalIndex)->InitSelected();
    return;
  }

  AIS_Selection::Selection (myCurrentName.ToCString())->Init();
}

//=======================================================================
//function : MoreSelected
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::MoreSelected() const
{
  if (HasOpenedContext())
    return myLocalContexts (myCurLocalIndex)->MoreSelected();

  return AIS_Selection::Selection (myCurrentName.ToCString())->More();
}

//=======================================================================
//function : NextSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::NextSelected()
{
  if(HasOpenedContext())
  {
    return myLocalContexts (myCurLocalIndex)->NextSelected();
    return;
  }

  AIS_Selection::Selection (myCurrentName.ToCString())->Next();
}

//=======================================================================
//function : HasSelectedShape
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HasSelectedShape() const
{
  if(HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->HasSelectedShape();
  }

  Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (SelectedInteractive());
  return !aShape.IsNull();
}

//=======================================================================
//function : SelectedShape
//purpose  :
//=======================================================================
TopoDS_Shape AIS_InteractiveContext::SelectedShape() const
{
  if (HasOpenedContext())
  {
    return myLocalContexts (myCurLocalIndex)->SelectedShape();
  }

  if (AIS_Selection::Selection (myCurrentName.ToCString())->Extent() == 0)
    return TopoDS_Shape();

  const Handle(StdSelect_BRepOwner) anOwner =
    Handle(StdSelect_BRepOwner)::DownCast (AIS_Selection::Selection (myCurrentName.ToCString())->Value());
  if (!anOwner->HasSelectable())
    return TopoDS_Shape();

  return anOwner->Shape().Located (anOwner->Location() * anOwner->Shape().Location());
}

//=======================================================================
//function : SelectedInteractive
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_InteractiveContext::SelectedInteractive() const 
{
  if (HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->SelectedInteractive();
  }

  const Handle(SelectMgr_EntityOwner) anOwner =
    Handle(SelectMgr_EntityOwner)::DownCast (AIS_Selection::Selection (myCurrentName.ToCString())->Value());
  if (anOwner.IsNull() || !anOwner->HasSelectable())
    return NULL;

  return Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
}
//=======================================================================
//function : SelectedOwner
//purpose  :
//=======================================================================
Handle(SelectMgr_EntityOwner) AIS_InteractiveContext::SelectedOwner() const
{
  if(HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->SelectedOwner();
  }

  Handle(AIS_Selection) aCurSel = AIS_Selection::Selection (myCurrentName.ToCString());

  return aCurSel->Extent() > 0 ?
    Handle(SelectMgr_EntityOwner)::DownCast (aCurSel->Value()) : NULL;
}

//=======================================================================
//function : EntityOwners
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::EntityOwners(Handle(SelectMgr_IndexedMapOfOwner)& theOwners,
					  const Handle(AIS_InteractiveObject)& theIObj,
					  const Standard_Integer theMode) const 
{
  if ( theIObj.IsNull() )
      return;

  TColStd_ListOfInteger aModes;
  if ( theMode == -1 )
    ActivatedModes( theIObj, aModes );
  else
    aModes.Append( theMode );

  if (theOwners.IsNull())
    theOwners = new SelectMgr_IndexedMapOfOwner();

  TColStd_ListIteratorOfListOfInteger anItr( aModes );
  for (; anItr.More(); anItr.Next() )
  {
    int aMode = anItr.Value();
    if ( !theIObj->HasSelection( aMode ) )
      continue;

    Handle(SelectMgr_Selection) aSel = theIObj->Selection(aMode);

    for ( aSel->Init(); aSel->More(); aSel->Next() )
    {
      Handle(SelectBasics_SensitiveEntity) aEntity = aSel->Sensitive()->BaseSensitive();
      if ( aEntity.IsNull() )
	continue;

      Handle(SelectMgr_EntityOwner) aOwner =
	Handle(SelectMgr_EntityOwner)::DownCast(aEntity->OwnerId());
      if ( !aOwner.IsNull() )
	theOwners->Add( aOwner );
    }
  }
}

//=======================================================================
//function : NbSelected
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::NbSelected()
{
  Standard_Integer aNbSelected = 0;
  for (InitSelected(); MoreSelected(); NextSelected())
  {
    aNbSelected++;
  }

  return aNbSelected;
}

//=======================================================================
//function : HasApplicative
//purpose  :
//=======================================================================
  Standard_Boolean AIS_InteractiveContext::HasApplicative() const 
{
  return SelectedInteractive()->HasOwner();
}

//=======================================================================
//function : Applicative
//purpose  :
//=======================================================================
Handle(Standard_Transient) AIS_InteractiveContext::Applicative() const 
{
  return SelectedInteractive()->GetOwner();
}

//==================================================
// Function: HasDetected
// Purpose :
//==================================================
Standard_Boolean AIS_InteractiveContext::HasDetected() const
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->HasDetected();

  return !myLastPicked.IsNull();
}

//=======================================================================
//function : HasDetectedShape
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::HasDetectedShape() const 
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->HasDetectedShape();
  return Standard_False;
}

//=======================================================================
//function : DetectedShape
//purpose  : 
//=======================================================================

const TopoDS_Shape&
AIS_InteractiveContext::DetectedShape() const
{
  return myLocalContexts(myCurLocalIndex)->DetectedShape();
}					    

//=======================================================================
//function : DetectedInteractive
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_InteractiveContext::DetectedInteractive() const
{
  if (HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->DetectedInteractive();

  return Handle(AIS_InteractiveObject)::DownCast (myLastPicked->Selectable());
}


Standard_Boolean AIS_InteractiveContext::HasNextDetected() const 
{
  if(!HasOpenedContext())
    return Standard_False; // temporaire
  else
    return myLocalContexts(myCurLocalIndex)->HasNextDetected();
  
}


//=======================================================================
//function : DetectedOwner
//purpose  : 
//=======================================================================
Handle(SelectMgr_EntityOwner) AIS_InteractiveContext::DetectedOwner() const
{
  if (HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->DetectedOwner();

  return myLastPicked;
}

//=======================================================================
//function : HilightNextDetected
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::HilightNextDetected (const Handle(V3d_View)& theView,
                                                              const Standard_Boolean  theToRedrawImmediate)
{
  return HasOpenedContext()
       ? myLocalContexts (myCurLocalIndex)->HilightNextDetected (theView, theToRedrawImmediate)
       : 0;
    
}

//=======================================================================
//function : HilightNextDetected
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::HilightPreviousDetected (const Handle(V3d_View)& theView,
                                                                  const Standard_Boolean  theToRedrawImmediate)
{
  return HasOpenedContext()
       ? myLocalContexts (myCurLocalIndex)->HilightPreviousDetected (theView, theToRedrawImmediate)
       : 0;
    
}

//=======================================================================
//function : InitDetected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::InitDetected()
{
  if (HasOpenedContext())
  {
    myLocalContexts(myCurLocalIndex)->InitDetected();
    return;
  }

  if(myAISDetectedSeq.Length() != 0)
  {
    myAISCurDetected = 1;
  }
}

//=======================================================================
//function : MoreDetected
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::MoreDetected() const
{
  if (HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->MoreDetected();
  }

  return (myAISCurDetected > 0 && myAISCurDetected <= myAISDetectedSeq.Length()) ?
          Standard_True : Standard_False;
}

//=======================================================================
//function : NextDetected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::NextDetected()
{
  if(HasOpenedContext())
  {
    myLocalContexts(myCurLocalIndex)->NextDetected();
    return;
  }

  myAISCurDetected++;
}

//=======================================================================
//function : DetectedCurrentShape
//purpose  :
//=======================================================================
const TopoDS_Shape& AIS_InteractiveContext::DetectedCurrentShape() const
{
  if (HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->DetectedCurrentShape();
  }

  static TopoDS_Shape aDummyShape;

  Handle(AIS_Shape) aCurrentShape = Handle(AIS_Shape)::DownCast (DetectedCurrentObject());

  if (aCurrentShape.IsNull())
  {
    return aDummyShape;
  }

  return aCurrentShape->Shape();
}

//=======================================================================
//function : DetectedCurrentObject
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_InteractiveContext::DetectedCurrentObject() const
{
  if (HasOpenedContext())
  {
    return myLocalContexts(myCurLocalIndex)->DetectedCurrentObject();
  }

  return MoreDetected() ? myAISDetectedSeq(myAISCurDetected) : NULL;
}

//=======================================================================
//function : FirstSelectedObject
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_InteractiveContext::FirstSelectedObject()
{
  Handle(AIS_InteractiveObject) anObject;

  if (HasOpenedContext())
    return anObject;

  InitSelected();
  if (MoreSelected())
  {
    return SelectedInteractive();
  }
  return anObject;
}

//=======================================================================
//function : RedrawImmediate
//purpose  : Redisplays immediate strucures of the viewer given according to their visibility
//=======================================================================
void AIS_InteractiveContext::RedrawImmediate (const Handle(V3d_Viewer)& theViewer)
{
  myMainPM->RedrawImmediate (theViewer);
}
