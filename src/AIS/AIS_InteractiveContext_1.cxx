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
#include <Select3D_SensitiveEntity.hxx>
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
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_Light.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_SpotLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

typedef NCollection_DataMap<Handle(AIS_InteractiveObject), NCollection_Handle<SelectMgr_SequenceOfOwner> > AIS_MapOfObjSelectedOwners;

namespace
{
  TopoDS_Shape AIS_InteractiveContext_myDummyShape;
}

//=======================================================================
//function : highlightWithColor
//purpose  :
//=======================================================================
void AIS_InteractiveContext::highlightWithColor (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                 const Handle(V3d_Viewer)& theViewer)
{
  const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());
  if (anObj.IsNull())
  {
    return;
  }

  const Handle(Prs3d_Drawer)& aStyle = getHiStyle (anObj, theOwner);
  const Standard_Integer aHiMode = getHilightMode (anObj, aStyle, -1);

  myMainPM->BeginImmediateDraw();
  theOwner->HilightWithColor (myMainPM, aStyle, aHiMode);
  myMainPM->EndImmediateDraw (theViewer.IsNull() ? myMainVwr : theViewer);
}

//=======================================================================
//function : highlightSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::highlightSelected (const Handle(SelectMgr_EntityOwner)& theOwner)
{
  AIS_NListOfEntityOwner anOwners;
  const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());
  if (anObj.IsNull())
  {
    return;
  }

  if (!theOwner->IsAutoHilight())
  {
    SelectMgr_SequenceOfOwner aSeq;
    for (AIS_NListOfEntityOwner::Iterator aSelIter (mySelection->Objects()); aSelIter.More(); aSelIter.Next())
    {
      if (aSelIter.Value()->IsSameSelectable (anObj))
      {
        anOwners.Append (aSelIter.Value());
      }
    }
  }
  else
  {
    anOwners.Append (theOwner);
  }
  highlightOwners (anOwners);
}

//=======================================================================
//function : highlightGlobal
//purpose  :
//=======================================================================
void AIS_InteractiveContext::highlightGlobal (const Handle(AIS_InteractiveObject)& theObj,
                                              const Handle(Prs3d_Drawer)& theStyle,
                                              const Standard_Integer theDispMode)
{
  if (theObj.IsNull())
  {
    return;
  }

  const Standard_Integer aHiMode = getHilightMode (theObj, theStyle, theDispMode);
  const Handle(SelectMgr_EntityOwner)& aGlobOwner = theObj->GlobalSelOwner();

  if (aGlobOwner.IsNull())
  {
    myMainPM->Color (theObj, theStyle, aHiMode);
    return;
  }

  AIS_NListOfEntityOwner anOwners;
  if (!aGlobOwner->IsAutoHilight())
  {
    SelectMgr_SequenceOfOwner aSeq;
    for (AIS_NListOfEntityOwner::Iterator aSelIter (mySelection->Objects()); aSelIter.More(); aSelIter.Next())
    {
      if (aSelIter.Value()->IsSameSelectable (theObj))
      {
        anOwners.Append (aSelIter.Value());
      }
    }
  }
  else
  {
    anOwners.Append (aGlobOwner);
  }
  highlightOwners (anOwners);
}

//=======================================================================
//function : unhighlightSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::unhighlightSelected (const Standard_Boolean theIsToHilightSubIntensity)
{
  unhighlightOwners (mySelection->Objects(), theIsToHilightSubIntensity);
}

//=======================================================================
//function : unhighlightOwners
//purpose  :
//=======================================================================
void AIS_InteractiveContext::unhighlightOwners (const AIS_NListOfEntityOwner& theOwners,
                                                const Standard_Boolean theIsToHilightSubIntensity)
{
  NCollection_IndexedMap<Handle(AIS_InteractiveObject)> anObjToClear;
  for (AIS_NListOfEntityOwner::Iterator aSelIter (theOwners); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner = aSelIter.Value();
    const Handle(AIS_InteractiveObject) anInteractive = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
    Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek (anInteractive);
    if (!aStatusPtr)
    {
      continue;
    }

    if (anOwner->IsAutoHilight())
    {
      anOwner->Unhilight (myMainPM);
      if (theIsToHilightSubIntensity)
      {
        if ((*aStatusPtr)->IsSubIntensityOn())
        {
          const Standard_Integer aHiMode = getHilightMode (anInteractive, (*aStatusPtr)->HilightStyle(), (*aStatusPtr)->DisplayMode());
          highlightWithSubintensity (anOwner, aHiMode);
        }
      }
    }
    else
    {
      anObjToClear.Add (anInteractive);
    }
    if (anOwner == anInteractive->GlobalSelOwner())
    {
      (*aStatusPtr)->SetHilightStatus (Standard_False);
    }
  }
  for (NCollection_IndexedMap<Handle(AIS_InteractiveObject)>::Iterator anIter (anObjToClear); anIter.More(); anIter.Next())
  {
    const Handle(AIS_InteractiveObject)& anObj = anIter.Value();
    myMainPM->Unhighlight (anObj);
    anObj->ClearSelected();
  }
}

//=======================================================================
//function : unhighlightGlobal
//purpose  :
//=======================================================================
void AIS_InteractiveContext::unhighlightGlobal (const Handle(AIS_InteractiveObject)& theObj)
{
  if (theObj.IsNull())
  {
    return;
  }

  const Handle(SelectMgr_EntityOwner)& aGlobOwner = theObj->GlobalSelOwner();
  if (aGlobOwner.IsNull())
  {
    myMainPM->Unhighlight (theObj);
    return;
  }

  AIS_NListOfEntityOwner anOwners;
  anOwners.Append (aGlobOwner);
  unhighlightOwners (anOwners);
}

//=======================================================================
//function : turnOnSubintensity
//purpose  :
//=======================================================================
void AIS_InteractiveContext::turnOnSubintensity (const Handle(AIS_InteractiveObject)& theObject,
                                                 const Standard_Integer theDispMode,
                                                 const Standard_Boolean theIsDisplayedOnly) const
{
  // the only differ with selection highlight is color, so sync transparency values
  const Handle(Prs3d_Drawer)& aSubStyle = myStyles[Prs3d_TypeOfHighlight_SubIntensity];
  aSubStyle->SetTransparency (myStyles[Prs3d_TypeOfHighlight_Selected]->Transparency());

  if (theObject.IsNull())
  {
    for (AIS_DataMapIteratorOfDataMapOfIOStatus anObjsIter (myObjects); anObjsIter.More(); anObjsIter.Next())
    {
      const Handle(AIS_GlobalStatus)& aStatus = anObjsIter.Value();
      if (aStatus->GraphicStatus() != AIS_DS_Displayed && theIsDisplayedOnly)
        continue;

      aStatus->SubIntensityOn();
      myMainPM->Color (anObjsIter.Key(), aSubStyle, theDispMode != -1 ? theDispMode : aStatus->DisplayMode());
    }
  }
  else
  {
    Handle(AIS_GlobalStatus) aStatus;
    if (!myObjects.Find (theObject, aStatus))
      return;

    if (aStatus->GraphicStatus() != AIS_DS_Displayed && theIsDisplayedOnly)
      return;

    aStatus->SubIntensityOn();
    myMainPM->Color (theObject, aSubStyle, theDispMode != -1 ? theDispMode : aStatus->DisplayMode());
  }
}

//=======================================================================
//function : highlightWithSubintensity
//purpose  :
//=======================================================================
void AIS_InteractiveContext::highlightWithSubintensity (const Handle(AIS_InteractiveObject)& theObject,
                                                        const Standard_Integer theMode) const
{
  // the only differ with selection highlight is color, so
  // sync transparency values
  myStyles[Prs3d_TypeOfHighlight_SubIntensity]->SetTransparency (myStyles[Prs3d_TypeOfHighlight_Selected]->Transparency());

  myMainPM->Color (theObject, myStyles[Prs3d_TypeOfHighlight_SubIntensity], theMode);
}

//=======================================================================
//function : highlightWithSubintensity
//purpose  :
//=======================================================================
void AIS_InteractiveContext::highlightWithSubintensity (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                        const Standard_Integer theMode) const
{
  // the only differ with selection highlight is color, so
  // sync transparency values
  myStyles[Prs3d_TypeOfHighlight_SubIntensity]->SetTransparency (myStyles[Prs3d_TypeOfHighlight_Selected]->Transparency());

  theOwner->HilightWithColor (myMainPM, myStyles[Prs3d_TypeOfHighlight_SubIntensity], theMode);
}

//=======================================================================
//function : isSlowHiStyle
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::isSlowHiStyle (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                        const Handle(V3d_Viewer)& theViewer) const
{
  if (const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable()))
  {
    const Handle(Prs3d_Drawer)& aHiStyle = getHiStyle (anObj, myLastPicked);
    return aHiStyle->ZLayer() == Graphic3d_ZLayerId_UNKNOWN
       || !theViewer->ZLayerSettings (aHiStyle->ZLayer()).IsImmediate();
  }
  return Standard_False;
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
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError ("AIS_InteractiveContext::MoveTo() - invalid argument");
  }

  myCurDetected = 0;
  myCurHighlighted = 0;
  myDetectedSeq.Clear();
  myLastActiveView = theView.get();

  // preliminaires
  AIS_StatusOfDetection aStatus        = AIS_SOD_Nothing;
  Standard_Boolean      toUpdateViewer = Standard_False;

  myFilters->SetDisabledObjects (theView->View()->HiddenObjects());
  myMainSel->Pick (theXPix, theYPix, theView);

  // filling of myAISDetectedSeq sequence storing information about detected AIS objects
  // (the objects must be AIS_Shapes)
  const Standard_Integer aDetectedNb = myMainSel->NbPicked();
  Standard_Integer aNewDetected = 0;
  Standard_Boolean toIgnoreDetTop = Standard_False;
  for (Standard_Integer aDetIter = 1; aDetIter <= aDetectedNb; ++aDetIter)
  {
    Handle(SelectMgr_EntityOwner) anOwner = myMainSel->Picked (aDetIter);
    if (anOwner.IsNull()
     || !myFilters->IsOk (anOwner))
    {
      if (myPickingStrategy == SelectMgr_PickingStrategy_OnlyTopmost)
      {
        toIgnoreDetTop = Standard_True;
      }
      continue;
    }

    if (aNewDetected < 1
    && !toIgnoreDetTop)
    {
      aNewDetected = aDetIter;
    }

    myDetectedSeq.Append (aDetIter);
  }

  if (aNewDetected >= 1)
  {
    myCurHighlighted = myDetectedSeq.Lower();

    // Does nothing if previously detected object is equal to the current one.
    // However in advanced selection modes the owners comparison
    // is not effective because in that case only one owner manage the
    // selection in current selection mode. It is necessary to check the current detected
    // entity and hilight it only if the detected entity is not the same as
    // previous detected (IsForcedHilight call)
    Handle(SelectMgr_EntityOwner) aNewPickedOwner = myMainSel->Picked (aNewDetected);
    if (aNewPickedOwner == myLastPicked && !aNewPickedOwner->IsForcedHilight())
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
      if (isSlowHiStyle (myLastPicked, theView->Viewer()))
      {
        theView->Viewer()->Invalidate();
      }

      clearDynamicHighlight();
      toUpdateViewer = Standard_True;
    }

    // initialize myLastPicked field with currently detected object
    myLastPicked = aNewPickedOwner;

    // highlight detected object if it is not selected or myToHilightSelected flag is true
    if (myLastPicked->HasSelectable())
    {
      if (myAutoHilight
       && (!myLastPicked->IsSelected()
         || myToHilightSelected))
      {
        if (isSlowHiStyle (myLastPicked, theView->Viewer()))
        {
          theView->Viewer()->Invalidate();
        }

        highlightWithColor (myLastPicked, theView->Viewer());
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
    if (myAutoHilight
    && !myLastPicked.IsNull()
     && myLastPicked->HasSelectable())
    {
      if (isSlowHiStyle (myLastPicked, theView->Viewer()))
      {
        theView->Viewer()->Invalidate();
      }

      clearDynamicHighlight();
      toUpdateViewer = Standard_True;
    }

    myLastPicked.Nullify();
  }

  if (toUpdateViewer
   && theToRedrawOnUpdate)
  {
    if (theView->ComputedMode())
    {
      theView->Viewer()->Update();
    }
    else
    {
      if (theView->IsInvalidated())
      {
        theView->Viewer()->Redraw();
      }
      else
      {
        theView->Viewer()->RedrawImmediate();
      }
    }
  }

  return aStatus;
}

//=======================================================================
//function : AddSelect
//purpose  : 
//=======================================================================
AIS_StatusOfPick AIS_InteractiveContext::AddSelect (const Handle(SelectMgr_EntityOwner)& theObject)
{
  mySelection->AddSelect (theObject);

  Standard_Integer aSelNum = NbSelected();
  return (aSelNum == 0) ? AIS_SOP_NothingSelected
                        : (aSelNum == 1) ? AIS_SOP_OneSelected
                                         : AIS_SOP_SeveralSelected;
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
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError ("AIS_InteractiveContext::Select() - invalid argument");
  }

  // all objects detected by the selector are taken, previous current objects are emptied,
  // new objects are put...
  ClearSelected (Standard_False);
  myLastActiveView = theView.get();
  myMainSel->Pick (theXPMin, theYPMin, theXPMax, theYPMax, theView);
  for (Standard_Integer aPickIter = 1; aPickIter <= myMainSel->NbPicked(); ++aPickIter)
  {
    const Handle(SelectMgr_EntityOwner)& aCurOwner = myMainSel->Picked (aPickIter);
    if (aCurOwner.IsNull() || !aCurOwner->HasSelectable() || !myFilters->IsOk (aCurOwner))
      continue;

    mySelection->Select (aCurOwner);
  }

  if (myAutoHilight)
  {
    HilightSelected (toUpdateViewer);
  }

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
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError ("AIS_InteractiveContext::Select() - invalid argument");
  }

  // all objects detected by the selector are taken, previous current objects are emptied,
  // new objects are put...
  ClearSelected (Standard_False);
  myLastActiveView = theView.get();
  myMainSel->Pick (thePolyline, theView);
  for (Standard_Integer aPickIter = 1; aPickIter <= myMainSel->NbPicked(); ++aPickIter)
  {
    const Handle(SelectMgr_EntityOwner) anOwner = myMainSel->Picked (aPickIter);
    if (anOwner.IsNull() || !anOwner->HasSelectable() || !myFilters->IsOk (anOwner))
      continue;

    mySelection->Select (anOwner);
  }

  if (myAutoHilight)
  {
    HilightSelected (toUpdateViewer);
  }

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
  if (!myLastPicked.IsNull())
  {
    Graphic3d_Vec2i aMousePos (-1, -1);
    if (myMainSel->GetManager().GetActiveSelectionType() == SelectBasics_SelectingVolumeManager::Point)
    {
      aMousePos.SetValues ((Standard_Integer )myMainSel->GetManager().GetMousePosition().X(),
                           (Standard_Integer )myMainSel->GetManager().GetMousePosition().Y());
    }
    if (myLastPicked->HandleMouseClick (aMousePos, Aspect_VKeyMouse_LeftButton, Aspect_VKeyFlags_NONE, false))
    {
      return AIS_SOP_NothingSelected;
    }

    if (myAutoHilight)
    {
      clearDynamicHighlight();
    }
    if (!myLastPicked->IsSelected()
      || myLastPicked->IsForcedHilight()
      || NbSelected() > 1)
    {
      SetSelected (myLastPicked, Standard_False);
      if(toUpdateViewer)
      {
        UpdateCurrentViewer();
      }
    }
  }
  else
  {
    ClearSelected (toUpdateViewer);
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
  if (myAutoHilight)
  {
    clearDynamicHighlight();
  }
  if (!myLastPicked.IsNull())
  {
    AddOrRemoveSelected (myLastPicked, toUpdateViewer);
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
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError ("AIS_InteractiveContext::ShiftSelect() - invalid argument");
  }

  myLastActiveView = theView.get();
  if (myAutoHilight)
  {
    UnhilightSelected (Standard_False);
  }
  myMainSel->Pick (theXPMin, theYPMin, theXPMax, theYPMax, theView);
  for (Standard_Integer aPickIter = 1; aPickIter <= myMainSel->NbPicked(); ++aPickIter)
  {
    const Handle(SelectMgr_EntityOwner) anOwner = myMainSel->Picked (aPickIter);
    if (anOwner.IsNull() || !anOwner->HasSelectable() || !myFilters->IsOk (anOwner))
      continue;

    mySelection->Select (anOwner);
  }

  if (myAutoHilight)
  {
    HilightSelected (toUpdateViewer);
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
AIS_StatusOfPick AIS_InteractiveContext::ShiftSelect (const TColgp_Array1OfPnt2d& thePolyline,
                                                      const Handle(V3d_View)& theView,
                                                      const Standard_Boolean toUpdateViewer)
{
  if (theView->Viewer() != myMainVwr)
  {
    throw Standard_ProgramError ("AIS_InteractiveContext::ShiftSelect() - invalid argument");
  }

  myLastActiveView = theView.get();
  if (myAutoHilight)
  {
    UnhilightSelected (Standard_False);
  }
  myMainSel->Pick (thePolyline, theView);
  for (Standard_Integer aPickIter = 1; aPickIter <= myMainSel->NbPicked(); ++aPickIter)
  {
    const Handle(SelectMgr_EntityOwner) anOwner = myMainSel->Picked (aPickIter);
    if (anOwner.IsNull() || !anOwner->HasSelectable() || !myFilters->IsOk (anOwner))
      continue;

    mySelection->Select (anOwner);
  }

  if (myAutoHilight)
  {
    HilightSelected (toUpdateViewer);
  }

  Standard_Integer aSelNum = NbSelected();

  return (aSelNum == 0) ? AIS_SOP_NothingSelected
                        : (aSelNum == 1) ? AIS_SOP_OneSelected
                                         : AIS_SOP_SeveralSelected;
}

//=======================================================================
//function : HilightSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::HilightSelected (const Standard_Boolean theToUpdateViewer)
{
  // In case of selection without using local context
  clearDynamicHighlight();

  highlightOwners (mySelection->Objects());

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : highlightOwners
//purpose  :
//=======================================================================
void AIS_InteractiveContext::highlightOwners (const AIS_NListOfEntityOwner& theOwners)
{
  AIS_MapOfObjSelectedOwners anObjOwnerMap;
  for (AIS_NListOfEntityOwner::Iterator aSelIter (theOwners); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_EntityOwner) anOwner = aSelIter.Value();
    const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (anOwner->Selectable());
    if (anObj.IsNull())
      continue;

    const Handle(Prs3d_Drawer)& anObjSelStyle = getSelStyle (anObj, anOwner);
    Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek (anObj);
    if (!aStatusPtr)
    {
      continue;
    }
    if (anOwner == anObj->GlobalSelOwner())
    {
      (*aStatusPtr)->SetHilightStatus (Standard_True);
      (*aStatusPtr)->SetHilightStyle (anObjSelStyle);
    }
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
      const Standard_Integer aHiMode = getHilightMode (anObj, anObjSelStyle, (*aStatusPtr)->DisplayMode());
      anOwner->HilightWithColor (myMainPM, anObjSelStyle, aHiMode);
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
}

//=======================================================================
//function : UnhilightSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::UnhilightSelected (const Standard_Boolean theToUpdateViewer)
{
  unhighlightSelected();

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}


//=======================================================================
//function : ClearSelected
//purpose  :
//=======================================================================
void AIS_InteractiveContext::ClearSelected (const Standard_Boolean theToUpdateViewer)
{
  if (NbSelected() == 0)
    return;

  if (myAutoHilight)
  {
    unhighlightSelected();
  }

  mySelection->Clear();
  if (myAutoHilight)
  {
    clearDynamicHighlight();
  }

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : SetSelected
//purpose  : Sets the whole object as selected and highlights it with selection color
//=======================================================================
void AIS_InteractiveContext::SetSelected (const Handle(AIS_InteractiveObject)& theObject,
                                          const Standard_Boolean theToUpdateViewer)
{
  if (theObject.IsNull())
  {
    return;
  }

  if (!myObjects.IsBound (theObject))
  {
    return;
  }

  Handle(SelectMgr_EntityOwner) anOwner = theObject->GlobalSelOwner();
  if (anOwner.IsNull())
  {
    return;
  }

  const Handle(Prs3d_Drawer)& anObjSelStyle = getSelStyle (theObject, anOwner);
  if (NbSelected() == 1 && myObjects (theObject)->IsHilighted() && myAutoHilight)
  {
    Handle(Prs3d_Drawer) aCustomStyle;
    if (HighlightStyle (theObject, aCustomStyle))
    {
      if (!aCustomStyle.IsNull() && anObjSelStyle != aCustomStyle)
      {
        HilightWithColor (theObject, anObjSelStyle, theToUpdateViewer);
      }
    }
    return;
  }

  for (AIS_NListOfEntityOwner::Iterator aSelIter (mySelection->Objects()); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_EntityOwner)& aSelOwner = aSelIter.Value();
    if (!myFilters->IsOk (aSelOwner))
    {
      continue;
    }

    Handle(AIS_InteractiveObject) aSelectable = Handle(AIS_InteractiveObject)::DownCast (aSelOwner->Selectable());
    if (myAutoHilight)
    {
      Unhilight (aSelectable, Standard_False);
    }
    if (aSelOwner == aSelectable->GlobalSelOwner())
    {
      if (Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek (aSelectable))
      {
        (*aStatusPtr)->SetHilightStatus (Standard_False);
      }
    }
  }

  // added to avoid untimely viewer update...
  mySelection->ClearAndSelect (anOwner);

  if (myAutoHilight)
  {
    Handle(Prs3d_Drawer) aCustomStyle;
    if (HighlightStyle (theObject, aCustomStyle))
    {
      if (!aCustomStyle.IsNull() && anObjSelStyle != aCustomStyle)
      {
        HilightWithColor (theObject, anObjSelStyle, Standard_False);
      }
    }
    else
    {
      HilightWithColor (theObject, anObjSelStyle, Standard_False);
    }
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

  const Handle(AIS_InteractiveObject) anObject = Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());
  const Handle(Prs3d_Drawer)& anObjSelStyle = getSelStyle (anObject, theOwner);
  if (NbSelected() == 1 && theOwner->IsSelected() && !theOwner->IsForcedHilight())
  {
    Handle(Prs3d_Drawer) aCustomStyle;
    if (myAutoHilight && HighlightStyle (theOwner, aCustomStyle))
    {
      if (!aCustomStyle.IsNull() && anObjSelStyle != aCustomStyle)
      {
        const Standard_Integer aHiMode = anObject->HasHilightMode() ? anObject->HilightMode() : 0;
        theOwner->HilightWithColor (myMainPM, anObjSelStyle, aHiMode);
      }
    }
    return;
  }

  if (!myObjects.IsBound (anObject))
    return;

  if (myAutoHilight)
  {
    unhighlightSelected();
  }

  mySelection->ClearAndSelect (theOwner);
  if (myAutoHilight)
  {
    Handle(Prs3d_Drawer) aCustomStyle;
    if (!HighlightStyle (theOwner, aCustomStyle) ||
      (!aCustomStyle.IsNull() && aCustomStyle != anObjSelStyle))
    {
      highlightSelected (theOwner);
    }
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
  if (theObject.IsNull()
  || !myObjects.IsBound (theObject))
  {
    return;
  }

  const Handle(SelectMgr_EntityOwner) anOwner = theObject->GlobalSelOwner();
  if (!anOwner.IsNull()
    && anOwner->HasSelectable())
  {
    AddOrRemoveSelected (anOwner, theToUpdateViewer);
  }
}

//=======================================================================
//function : AddOrRemoveSelected
//purpose  : Allows to highlight or unhighlight the owner given depending on
//           its selection status
//=======================================================================
void AIS_InteractiveContext::AddOrRemoveSelected (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                  const Standard_Boolean theToUpdateViewer)
{
  if (theOwner.IsNull() || !theOwner->HasSelectable())
    return;

  if (!myFilters->IsOk(theOwner) && !theOwner->IsSelected())
    return;

  mySelection->Select (theOwner);

  if (myAutoHilight)
  {
    const Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast (theOwner->Selectable());
    Handle(AIS_GlobalStatus)* aStatusPtr = myObjects.ChangeSeek (anObj);
    if (!aStatusPtr)
    {
      return;
    }

    if (theOwner->IsSelected())
    {
      highlightSelected (theOwner);
    }
    else
    {
      AIS_NListOfEntityOwner anOwners;
      anOwners.Append (theOwner);
      unhighlightOwners (anOwners);

      (*aStatusPtr)->SetHilightStyle (Handle(Prs3d_Drawer)());
    }
  }

  if (theToUpdateViewer)
    UpdateCurrentViewer();
}

// =======================================================================
// function : SetSelectedState
// purpose  :
// =======================================================================
Standard_Boolean AIS_InteractiveContext::SetSelectedState (const Handle(SelectMgr_EntityOwner)& theEntity,
                                                           const Standard_Boolean theIsSelected)
{
  if (theEntity.IsNull())
  {
    throw Standard_ProgramError ("Internal error: AIS_InteractiveContext::SetSelectedState() called with NO object");
  }

  if (!theEntity->HasSelectable()
    || mySelection->IsSelected (theEntity) == theIsSelected)
  {
    return false;
  }

  if (theEntity->IsAutoHilight())
  {
    AddOrRemoveSelected (theEntity, false);
    return true;
  }

  if (theIsSelected)
  {
    const AIS_SelectStatus aSelStatus = mySelection->AddSelect (theEntity);
    theEntity->SetSelected (true);
    return aSelStatus == AIS_SS_Added;
  }
  else
  {
    const AIS_SelectStatus aSelStatus = mySelection->Select (theEntity);
    theEntity->SetSelected (false);
    return aSelStatus == AIS_SS_Removed;
  }
}

//=======================================================================
//function : IsSelected
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::IsSelected (const Handle(AIS_InteractiveObject)& theObj) const
{
  if (theObj.IsNull())
  {
    return Standard_False;
  }

  const Handle(AIS_GlobalStatus)* aStatus = myObjects.Seek (theObj);
  if (aStatus == NULL)
  {
    return Standard_False;
  }

  const Standard_Integer aGlobalSelMode = theObj->GlobalSelectionMode();
  const TColStd_ListOfInteger& anActivatedModes = (*aStatus)->SelectionModes();
  for (TColStd_ListIteratorOfListOfInteger aModeIter (anActivatedModes); aModeIter.More(); aModeIter.Next())
  {
    if (aModeIter.Value() == aGlobalSelMode)
    {
      if (Handle(SelectMgr_EntityOwner) aGlobOwner = theObj->GlobalSelOwner())
      {
        return aGlobOwner->IsSelected();
      }
      return Standard_False;
    }
  }
  return Standard_False;
}

//=======================================================================
//function : FirstSelectedObject
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_InteractiveContext::FirstSelectedObject() const
{
  return !mySelection->Objects().IsEmpty()
        ? Handle(AIS_InteractiveObject)::DownCast (mySelection->Objects().First()->Selectable())
        : Handle(AIS_InteractiveObject)();
}

//=======================================================================
//function : HasSelectedShape
//purpose  :
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HasSelectedShape() const
{
  if (!mySelection->More())
    return Standard_False;

  const Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast (mySelection->Value());
  return !anOwner.IsNull() && anOwner->HasShape();
}

//=======================================================================
//function : SelectedShape
//purpose  :
//=======================================================================
TopoDS_Shape AIS_InteractiveContext::SelectedShape() const
{
  if (!mySelection->More())
    return TopoDS_Shape();

  const Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast (mySelection->Value());
  if (anOwner.IsNull() || !anOwner->HasSelectable())
    return TopoDS_Shape();

  return anOwner->Shape().Located (anOwner->Location() * anOwner->Shape().Location());
}

//=======================================================================
//function : EntityOwners
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::EntityOwners(Handle(SelectMgr_IndexedMapOfOwner)& theOwners,
					  const Handle(AIS_InteractiveObject)& theIObj,
					  const Standard_Integer theMode) const 
{
  if (theIObj.IsNull())
  {
    return;
  }

  TColStd_ListOfInteger aModes;
  if (theMode == -1)
  {
    ActivatedModes (theIObj, aModes);
  }
  else
  {
    aModes.Append (theMode);
  }

  if (theOwners.IsNull())
  {
    theOwners = new SelectMgr_IndexedMapOfOwner();
  }

  for (TColStd_ListIteratorOfListOfInteger anItr (aModes); anItr.More(); anItr.Next())
  {
    const int aMode = anItr.Value();
    const Handle(SelectMgr_Selection)& aSel = theIObj->Selection (aMode);
    if (aSel.IsNull())
    {
      continue;
    }

    for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter (aSel->Entities()); aSelEntIter.More(); aSelEntIter.Next())
    {
      if (Handle(Select3D_SensitiveEntity) aEntity = aSelEntIter.Value()->BaseSensitive())
      {
        if (const Handle(SelectMgr_EntityOwner)& aOwner = aEntity->OwnerId())
        {
          theOwners->Add (aOwner);
        }
      }
    }
  }
}

//=======================================================================
//function : HasDetectedShape
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveContext::HasDetectedShape() const 
{
  Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast (myLastPicked);
  return !anOwner.IsNull()
       && anOwner->HasShape();
}

//=======================================================================
//function : DetectedShape
//purpose  : 
//=======================================================================
const TopoDS_Shape& AIS_InteractiveContext::DetectedShape() const
{
  Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast (myLastPicked);
  return anOwner->Shape();
}

//=======================================================================
//function : HilightNextDetected
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::HilightNextDetected (const Handle(V3d_View)& theView,
                                                              const Standard_Boolean  theToRedrawImmediate)
{
  myMainPM->ClearImmediateDraw();
  if (myDetectedSeq.IsEmpty())
  {
    return 0;
  }

  if (++myCurHighlighted > myDetectedSeq.Upper())
  {
    myCurHighlighted = myDetectedSeq.Lower();
  }
  const Handle(SelectMgr_EntityOwner)& anOwner = myMainSel->Picked (myDetectedSeq (myCurHighlighted));
  if (anOwner.IsNull())
  {
    return 0;
  }

  highlightWithColor (anOwner, theView->Viewer());
  myLastPicked = anOwner;

  if (theToRedrawImmediate)
  {
    myMainPM->RedrawImmediate (theView->Viewer());
    myMainVwr->RedrawImmediate();
  }

  return myCurHighlighted;
}

//=======================================================================
//function : HilightPreviousDetected
//purpose  :
//=======================================================================
Standard_Integer AIS_InteractiveContext::HilightPreviousDetected (const Handle(V3d_View)& theView,
                                                                  const Standard_Boolean  theToRedrawImmediate)
{
  myMainPM->ClearImmediateDraw();
  if (myDetectedSeq.IsEmpty())
  {
    return 0;
  }

  if (--myCurHighlighted < myDetectedSeq.Lower())
  {
    myCurHighlighted = myDetectedSeq.Upper();
  }
  const Handle(SelectMgr_EntityOwner)& anOwner = myMainSel->Picked (myDetectedSeq (myCurHighlighted));
  if (anOwner.IsNull())
  {
    return 0;
  }

  highlightWithColor (anOwner, theView->Viewer());
  myLastPicked = anOwner;

  if (theToRedrawImmediate)
  {
    myMainPM->RedrawImmediate (theView->Viewer());
    myMainVwr->RedrawImmediate();
  }

  return myCurHighlighted;
}

//=======================================================================
//function : DetectedCurrentOwner
//purpose  :
//=======================================================================
Handle(SelectMgr_EntityOwner) AIS_InteractiveContext::DetectedCurrentOwner() const
{
  return MoreDetected()
       ? myMainSel->Picked (myDetectedSeq (myCurDetected))
       : Handle(SelectMgr_EntityOwner)();
}

//=======================================================================
//function : DetectedCurrentShape
//purpose  :
//=======================================================================
const TopoDS_Shape& AIS_InteractiveContext::DetectedCurrentShape() const
{
  Standard_DISABLE_DEPRECATION_WARNINGS
  Handle(AIS_Shape) aCurrentShape = Handle(AIS_Shape)::DownCast (DetectedCurrentObject());
  Standard_ENABLE_DEPRECATION_WARNINGS
  return !aCurrentShape.IsNull()
        ? aCurrentShape->Shape()
        : AIS_InteractiveContext_myDummyShape;
}

//=======================================================================
//function : DetectedCurrentObject
//purpose  :
//=======================================================================
Handle(AIS_InteractiveObject) AIS_InteractiveContext::DetectedCurrentObject() const
{
  return MoreDetected()
       ? Handle(AIS_InteractiveObject)::DownCast (myMainSel->Picked (myDetectedSeq (myCurDetected))->Selectable())
       : Handle(AIS_InteractiveObject)();
}
