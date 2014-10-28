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

#define UKI60826	//GG_161199	Use the requested selection color instead of default

#define	IMP280200	//GG Don't returns a NULL shape in SelectedShape()
//			method when the current selected is a shape !

#define BUC60814	//GG_300101	Idem UKI60826

#define OCC138          //VTN Avoding infinit loop in AddOrRemoveCurrentObject method.

#define OCC9657

#include <AIS_InteractiveContext.jxx>
#include <SelectMgr_EntityOwner.hxx>
#include <AIS_Selection.hxx>
#include <AIS_StatusOfDetection.hxx>
#include <AIS_StatusOfPick.hxx>

#include <AIS_DataMapIteratorOfDataMapOfIOStatus.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_GlobalStatus.hxx>
#include <AIS_Shape.hxx>

#include <Aspect_Grid.hxx>

#include <V3d_Light.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_SpotLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_AmbientLight.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectBasics_SensitiveEntity.hxx>

#include <Prs3d_Presentation.hxx>

#ifdef OCC9657
#include <AIS_MapOfInteractive.hxx>
#include <AIS_MapIteratorOfMapOfInteractive.hxx>
#endif

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

  // allonzy
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
    if (aNewPickedOwner->Selectable() == myLastPicked)
    {
      return myLastPicked->State() == 1
           ? AIS_SOD_Selected
           : AIS_SOD_OnlyOneDetected;
    }
 
    // Previously detected object is unhilighted if it is not selected or hilighted 
    // with selection color if it is selected. Such highlighting with selection color 
    // is needed only if myToHilightSelected flag is true. In this case previously detected
    // object has been already highlighted with myHilightColor during previous MoveTo() 
    // method call. As result it is necessary to rehighligt it with mySelectionColor.
    if (!myLastPicked.IsNull())
    {
      const Standard_Integer aHiMod = myLastPicked->HasHilightMode() ? myLastPicked->HilightMode() : 0;
      if (myLastPicked->State() != 1)
      {
        myMainPM->Unhighlight (myLastPicked, aHiMod);
        toUpdateViewer = Standard_True;
      }
      else if (myToHilightSelected)
      {
        myMainPM->Color (myLastPicked, mySelectionColor, aHiMod);
        toUpdateViewer = Standard_True;
      }
    }

    // initialize myLastPicked field with currently detected object
    myLastPicked = Handle(AIS_InteractiveObject)::DownCast (aNewPickedOwner->Selectable());
    myLastinMain = myLastPicked;

    // highlight detected object if it is not selected or myToHilightSelected flag is true
    if (!myLastPicked.IsNull())
    {
      if (myLastPicked->State() != 1 || myToHilightSelected)
      {
        const Standard_Integer aHiMod = myLastPicked->HasHilightMode() ? myLastPicked->HilightMode() : 0;
        myMainPM->Color (myLastPicked, myHilightColor, aHiMod);
        toUpdateViewer = Standard_True;
      }

      aStatus = myLastPicked->State() == 1
              ? AIS_SOD_Selected
              : AIS_SOD_OnlyOneDetected;
    }
  }
  else 
  {
    // previously detected object is unhilighted if it is not selected or hilighted
    // with selection color if it is selected
    aStatus = AIS_SOD_Nothing;
    if (!myLastPicked.IsNull())
    {
      Standard_Integer aHiMod = myLastPicked->HasHilightMode() ? myLastPicked->HilightMode() : 0;
      if (myLastPicked->State() != 1)
      {
        myMainPM->Unhighlight (myLastPicked, aHiMod);
        toUpdateViewer = Standard_True;
      }
      else if (myToHilightSelected)
      {
        myMainPM->Color (myLastPicked, mySelectionColor, aHiMod);
        toUpdateViewer = Standard_True;
      }
    }

    myLastinMain.Nullify();
    myLastPicked.Nullify();
  }

  if (toUpdateViewer)
  {
    if (theToRedrawOnUpdate)
    {
      theView->Viewer()->Update();
    }
    else
    {
      theView->Viewer()->Invalidate();
    }
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

  ClearCurrents (Standard_False);

  Handle(StdSelect_ViewerSelector3d) aSelector;

  if (theView->Viewer() == myMainVwr)
  {
    aSelector = myMainSel;
    myWasLastMain = Standard_True;
  }

  aSelector->Pick (theXPMin, theYPMin, theXPMax, theYPMax, theView);
  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());

  AIS_MapOfInteractive anObjectsToSelect;
  for (aSelector->Init(); aSelector->More(); aSelector->Next())
  {
    Handle(AIS_InteractiveObject) anObj = 
      Handle(AIS_InteractiveObject)::DownCast (aSelector->Picked()->Selectable());
    if (!anObj.IsNull())
    {
      anObjectsToSelect.Add (anObj);
    }
  }

  AIS_MapIteratorOfMapOfInteractive anIt (anObjectsToSelect);
  for ( ; anIt.More(); anIt.Next())
  {
    AIS_Selection::Select (anIt.Key());
    anIt.Key()->State (1);
  }

  HilightCurrents (toUpdateViewer);

  Standard_Integer aSelNum = NbCurrents();

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

  ClearCurrents (Standard_False);

  Handle(StdSelect_ViewerSelector3d) aSelector;

  if (theView->Viewer() == myMainVwr)
  {
    aSelector = myMainSel;
    myWasLastMain = Standard_True;
  }

  aSelector->Pick (thePolyline, theView);
  AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());

  AIS_MapOfInteractive anObjectsToSelect;
  for (aSelector->Init(); aSelector->More(); aSelector->Next())
  {
    Handle(AIS_InteractiveObject) anObj = 
      Handle( AIS_InteractiveObject )::DownCast (aSelector->Picked()->Selectable());
    if (!anObj.IsNull())
    {
      anObjectsToSelect.Add (anObj);
    }
  }

  AIS_MapIteratorOfMapOfInteractive anIt (anObjectsToSelect);
  for ( ; anIt.More(); anIt.Next())
  {
    AIS_Selection::Select (anIt.Key());
    anIt.Key()->State (1);
  }

  HilightCurrents (toUpdateViewer);

  Standard_Integer aSelNum = NbCurrents();

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
      myLocalContexts(myCurLocalIndex)->SetSelected (myLastPicked, toUpdateViewer);
      return AIS_SOP_OneSelected;
    }
  }

  if (myWasLastMain && !myLastinMain.IsNull())
  {
    if(myLastinMain->State() != 1)
    {
      SetCurrentObject (myLastinMain,Standard_False);
      if(toUpdateViewer)
      {
        UpdateCurrentViewer();
      }
    }
  }
  else
  {
    AIS_Selection::SetCurrentSelection (myCurrentName.ToCString());
    Handle(AIS_Selection) aSelection = AIS_Selection::CurrentSelection();
    Handle(AIS_InteractiveObject) anObj;
    for (aSelection->Init(); aSelection->More(); aSelection->Next())
    {
      anObj = Handle(AIS_InteractiveObject)::DownCast (aSelection->Value());
      anObj->State (0);
      Unhilight (anObj,Standard_False);
      if (myObjects.IsBound (anObj) && myObjects(anObj)->IsSubIntensityOn())
      {
        HilightWithColor (anObj, mySubIntensity, Standard_False);
      }
    }

    AIS_Selection::Select();
    if (toUpdateViewer && myWasLastMain)
    {
        UpdateCurrentViewer();
    }
  }

  Standard_Integer aSelNum = NbCurrents();

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

  if (myWasLastMain && !myLastinMain.IsNull())
  {
    AddOrRemoveCurrentObject (myLastinMain, toUpdateViewer);
  }

  Standard_Integer aCurrentSelNum = NbCurrents();

  return (aCurrentSelNum == 0) ? AIS_SOP_NothingSelected
                               : (aCurrentSelNum == 1) ? AIS_SOP_OneSelected
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

  UnhilightCurrents (Standard_False);

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
    Handle(AIS_InteractiveObject) anObjToSelect =  Handle(AIS_InteractiveObject)::DownCast (aSelector->Picked()->Selectable());
    if (!anObjToSelect.IsNull())
    {
      AIS_SelectStatus aSelStatus = AIS_Selection::Select (anObjToSelect);
      Standard_Integer aState = (aSelStatus == AIS_SS_Added) ? 1 : 0;
      anObjToSelect->State (aState);
    }
  }

  HilightCurrents (toUpdateViewer);

  Standard_Integer aSelNum = NbCurrents();

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

  UnhilightCurrents (Standard_False);

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
    Handle(AIS_InteractiveObject) anObjToSelect = Handle(AIS_InteractiveObject)::DownCast (aSelector->Picked()->Selectable());
    if (!anObjToSelect.IsNull())
    {
      AIS_SelectStatus aSelStatus = AIS_Selection::Select (anObjToSelect);
      Standard_Integer aState = (aSelStatus == AIS_SS_Added ) ? 1 : 0;
      anObjToSelect->State (aState);
    }
  }

  HilightCurrents (toUpdateViewer);

  Standard_Integer aSelNum = NbCurrents();

  return (aSelNum == 0) ? AIS_SOP_NothingSelected
                        : (aSelNum == 1) ? AIS_SOP_OneSelected
                                         : AIS_SOP_SeveralSelected;
}

//=======================================================================
//function : SetCurrentObject
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::SetCurrentObject(const Handle(AIS_InteractiveObject)& anIObj,
					      const Standard_Boolean updateviewer)
{
  // single case processing...
  if(NbCurrents()==1 && anIObj->State()==1){
    Quantity_NameOfColor HiCol;
    Standard_Boolean HasHiCol;
    if(IsHilighted(anIObj,HasHiCol,HiCol)){
      if(HasHiCol && HiCol!= mySelectionColor) {
#ifdef UKI60826
	HilightWithColor(anIObj,mySelectionColor,updateviewer);
#else
	Hilight(anIObj,updateviewer);
#endif
      }
    }
    return;
  }


  // 
  if(!HasOpenedContext()) {
    if(anIObj.IsNull()) return;
    if(!myObjects.IsBound(anIObj)) 
      Display(anIObj,Standard_False);
    
    AIS_Selection::SetCurrentSelection(myCurrentName.ToCString());
    Handle(AIS_Selection) sel = AIS_Selection::Selection(myCurrentName.ToCString());
    Handle(Standard_Transient) TR;
    Handle(AIS_InteractiveObject) IO;
    sel->Init();
    while (sel->More()) {
      TR = sel->Value();
      IO = *((Handle(AIS_InteractiveObject)*)&TR);
      Unhilight(IO,Standard_False);
      IO->State(0);
      sel->Next();
    }
    
    // added to avoid untimely viewer update...
    AIS_Selection::ClearAndSelect(anIObj);
    anIObj->State(1);
    Quantity_NameOfColor HiCol;
    Standard_Boolean HasHiCol;
    if(IsHilighted(anIObj,HasHiCol,HiCol)){
      if(HasHiCol && HiCol!= mySelectionColor) {
#ifdef UKI60826
	  HilightWithColor(anIObj,mySelectionColor,Standard_False);
#else
	  Hilight(anIObj,Standard_False);
#endif
      }
    }
    else{
#ifdef UKI60826
	HilightWithColor(anIObj,mySelectionColor,Standard_False);
#else
      Hilight(anIObj,Standard_False);
#endif
    }
    if (updateviewer) 
      UpdateCurrentViewer();
  }
  else{
#ifdef OCCT_DEBUG
    cout<<"Nothing Done : Opened Local Context"<<endl;
#endif
  }
}

//=======================================================================
//function : AddOrRemoveCurrentObject
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::
AddOrRemoveCurrentObject(const Handle(AIS_InteractiveObject)& anIObj,
			 const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return;
//OCC5682 to remove an object from neutral point even if Local Context is opened (Remove method)
  if(!HasOpenedContext() /*OCC5682*/|| myObjects.IsBound(anIObj)) {
    
    AIS_Selection::SetCurrentSelection(myCurrentName.ToCString());
    Handle(AIS_Selection) sel = AIS_Selection::Selection(myCurrentName.ToCString());

    AIS_SelectStatus SelStat = AIS_Selection::Select(anIObj);
    
    Standard_Integer mod = (SelStat==AIS_SS_Added) ? 1 : 0;
    anIObj->State(mod);
    if(mod==1)
#ifdef BUC60814
      HilightWithColor(anIObj,mySelectionColor,Standard_False);
#else
      Hilight(anIObj,Standard_False);
#endif
    else 
      Unhilight(anIObj,Standard_False);
    
    
    if(updateviewer) 
      UpdateCurrentViewer();
  }
  else{
#ifdef OCCT_DEBUG
    cout<<"Nothing Done : Opened Local Context"<<endl;
#endif
  }
}
//=======================================================================
//function : UpdateCurrent
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UpdateCurrent()
{
  HilightCurrents();
}

//=======================================================================
//function : IsCurrent
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::IsCurrent(const Handle(AIS_InteractiveObject)& anIObj) const 
{

  if (anIObj.IsNull()) return Standard_False;
  return (anIObj->State()==1);
  
  //   Handle(AIS_Selection) sel = AIS_Selection::Selection(myCurrentName.ToCString());
  //   sel->Init();
  //   while (sel->More()) {
  //     if (anIObj == sel->Value()) return Standard_True;
  //     sel->Next();
  //   }
  //   return Standard_False;

}

//=======================================================================
//function : InitCurrent
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::InitCurrent()
{
  AIS_Selection::Selection(myCurrentName.ToCString())->Init();
}

//=======================================================================
//function : MoreCurrent
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::MoreCurrent() const 
{
  return AIS_Selection::Selection(myCurrentName.ToCString())->More();

}

//=======================================================================
//function : NextCurrent
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::NextCurrent()
{
  AIS_Selection::Selection(myCurrentName.ToCString())->Next();
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

Handle(AIS_InteractiveObject) AIS_InteractiveContext::Current() const 
{
  Handle(Standard_Transient) TR = AIS_Selection::Selection(myCurrentName.ToCString())->Value();
  Handle(AIS_InteractiveObject) IO = *((Handle(AIS_InteractiveObject)*)&TR);
  return IO;
}

//=======================================================================
//function : FirstCurrentObject
//purpose  : 
//=======================================================================
Handle(AIS_InteractiveObject) AIS_InteractiveContext::FirstCurrentObject()  
{
  Handle(AIS_InteractiveObject) IO ;
  InitCurrent();
  if(MoreCurrent()){
    return Current();
  }
  return IO;
}

//=======================================================================
//function : NbCurrents
//purpose  : 
//=======================================================================
Standard_Integer AIS_InteractiveContext::NbCurrents()
{
  Standard_Integer i(0);
  for(InitCurrent();MoreCurrent();NextCurrent())
    {i++;}
  return i;
}

//=======================================================================
//function : HilightCurrents
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::HilightCurrents(const Standard_Boolean updateviewer)
{
  AIS_Selection::SetCurrentSelection(myCurrentName.ToCString());
  Handle(AIS_Selection) sel = AIS_Selection::Selection(myCurrentName.ToCString());
  Handle(Standard_Transient) TR;
  Handle(AIS_InteractiveObject) IO;
  sel->Init();
  while (sel->More()) {
    TR = sel->Value();
    IO = *((Handle(AIS_InteractiveObject)*)&TR);
#ifdef BUC60814
    HilightWithColor(IO,mySelectionColor,Standard_False);
#else
    Hilight(IO,Standard_False);
#endif
    sel->Next();
  }
  if(updateviewer) 
    UpdateCurrentViewer();
}

//=======================================================================
//function : UnhilightCurrents
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UnhilightCurrents(const Standard_Boolean updateviewer)
{
  AIS_Selection::SetCurrentSelection(myCurrentName.ToCString());
  Handle(AIS_Selection) sel = AIS_Selection::Selection(myCurrentName.ToCString());
  Handle(Standard_Transient) TR;
  Handle(AIS_InteractiveObject) IO;
  sel->Init();
  while (sel->More()) {
    TR = sel->Value();
    IO = *((Handle(AIS_InteractiveObject)*)&TR);
    Unhilight(IO,Standard_False);
    sel->Next();
  }
  if(updateviewer) 
    UpdateCurrentViewer();
}

//=======================================================================
//function : ClearCurrents
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ClearCurrents(const Standard_Boolean updateviewer)
{
  if(NbCurrents()==0) return;
  AIS_Selection::SetCurrentSelection(myCurrentName.ToCString());
  Handle(AIS_Selection) S = AIS_Selection::CurrentSelection();
  Handle(Standard_Transient) Tr;
  Handle(AIS_InteractiveObject) IO;
  for(S->Init();S->More();S->Next()){
    Tr = S->Value();
    IO = (*((Handle(AIS_InteractiveObject)*)&Tr));
    IO->State(0);
    Unhilight(IO,Standard_False);
  }
  AIS_Selection::Select();
  if(updateviewer)
    UpdateCurrentViewer();
}


//=======================================================================
//function : HilightSelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::HilightSelected(const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext()){
    AIS_Selection::SetCurrentSelection(mySelectionName.ToCString());
    Handle(AIS_Selection) sel = AIS_Selection::Selection(myCurrentName.ToCString());
    Handle(Standard_Transient) TR;
    Handle(AIS_InteractiveObject) IO;
    sel->Init();
    while (sel->More()) {
      TR = sel->Value();
      IO = *((Handle(AIS_InteractiveObject)*)&TR);
#ifdef BUC60814
      HilightWithColor(IO,mySelectionColor,Standard_False);
#else
      Hilight(IO,Standard_False);
#endif
      sel->Next();
    }
  }
  else
    {
      myLocalContexts(myCurLocalIndex)->HilightPicked(updateviewer);
    }
  if(updateviewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : UnhilightSelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UnhilightSelected(const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext()){
    AIS_Selection::SetCurrentSelection(mySelectionName.ToCString());
    Handle(AIS_Selection) sel = AIS_Selection::Selection(myCurrentName.ToCString());
    Handle(Standard_Transient) TR;
    Handle(AIS_InteractiveObject) IO;
    sel->Init();
    while (sel->More()) {
      TR = sel->Value();
      IO = *((Handle(AIS_InteractiveObject)*)&TR);
      Unhilight(IO,Standard_False);
      sel->Next();
    }
    if(updateviewer)
      UpdateCurrentViewer();
  }
  else
    {
      myLocalContexts(myCurLocalIndex)->UnhilightPicked(updateviewer);
    }
}


//=======================================================================
//function : ClearSelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ClearSelected(const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext())
    ClearCurrents(updateviewer);
  else 
    myLocalContexts(myCurLocalIndex)->ClearSelected(updateviewer);
  
}


//=======================================================================
//function : SetSelectedCurrent
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetSelectedCurrent()
{


#ifdef OCCT_DEBUG
  cout<<"Not Yet Implemented"<<endl;
#endif
}

//=======================================================================
//function : UpdateSelection
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UpdateSelected(const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext()) {
    HilightSelected();
#ifndef BUG
    return;
#endif
  }
  myLocalContexts(myCurLocalIndex)->UpdateSelected(updateviewer);
}

//=======================================================================
//function : SetCurrentObject
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::SetSelected(const Handle(AIS_InteractiveObject)& anIObj,const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext()) 
    SetCurrentObject(anIObj,updateviewer);
  else
    myLocalContexts(myCurLocalIndex)->SetSelected(anIObj,updateviewer);
}

//=======================================================================
//function : AddOrRemoveSelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::AddOrRemoveSelected(const Handle(AIS_InteractiveObject)& anIObj,
						 const Standard_Boolean updateviewer)
{
  if(!anIObj->HasInteractiveContext()) anIObj->SetContext(this);
  if(!HasOpenedContext())
    AddOrRemoveCurrentObject(anIObj,updateviewer);
  else
    myLocalContexts(myCurLocalIndex)->AddOrRemoveSelected(anIObj,updateviewer);
  
}
//=======================================================================
//function : AddOrRemoveSelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::AddOrRemoveSelected(const TopoDS_Shape& aShap,
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
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::AddOrRemoveSelected(const Handle(SelectMgr_EntityOwner)& Own,
						 const Standard_Boolean updateviewer)
{ 
  if(!HasOpenedContext()) {
#ifdef OCCT_DEBUG
    cout<<" Attempt to remove a selected ownr with no opened local context"<<endl;
#endif
    return;
  }
  
  myLocalContexts(myCurLocalIndex)->AddOrRemoveSelected(Own,Standard_False);
  if(updateviewer) UpdateCurrentViewer();
  
}



//=======================================================================
//function : IsSelected
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::
IsSelected(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  if (anIObj.IsNull()) return Standard_False;
  
  if(!HasOpenedContext()) 
    return (anIObj->State()==1);
  else 
    return myLocalContexts(myCurLocalIndex)->IsSelected(anIObj);
}


//=======================================================================
//function : InitSelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::InitSelected()
{
  if(!HasOpenedContext())
    AIS_Selection::Selection(myCurrentName.ToCString())->Init();
  else
    myLocalContexts(myCurLocalIndex)->InitSelected();
}

//=======================================================================
//function : MoreSelected
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::MoreSelected() const 
{
  if(!HasOpenedContext())
    return AIS_Selection::Selection(myCurrentName.ToCString())->More();
  return myLocalContexts(myCurLocalIndex)->MoreSelected();
}

//=======================================================================
//function : NextSelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::NextSelected()
{
  if(!HasOpenedContext())
    AIS_Selection::Selection(myCurrentName.ToCString())->Next();
  else
    myLocalContexts(myCurLocalIndex)->NextSelected();
}

//=======================================================================
//function : HasSelectedShape
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::HasSelectedShape() const 
{
  if(!HasOpenedContext()) {
#ifdef IMP280200
    Handle(AIS_Shape) shape = 
	Handle(AIS_Shape)::DownCast(SelectedInteractive());
    if( !shape.IsNull() ) return Standard_True;
#endif
    return Standard_False;
  }
  return myLocalContexts(myCurLocalIndex)->HasShape();
  
  
}

//=======================================================================
//function : SelectedShape
//purpose  : 
//=======================================================================

TopoDS_Shape AIS_InteractiveContext::SelectedShape() const 
{
  if (!HasOpenedContext())
  {
    TopoDS_Shape aResShape;
    Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (SelectedInteractive());
    if (!aShape.IsNull())
    {
      aResShape = aShape->Shape().Located (TopLoc_Location (SelectedInteractive()->Transformation()) * aShape->Shape().Location());
    }

    return aResShape;
  } 
  else
  {
    return myLocalContexts (myCurLocalIndex)->SelectedShape();
  }
}

//=======================================================================
//function : Interactive
//purpose  : 
//=======================================================================

Handle(AIS_InteractiveObject) AIS_InteractiveContext::Interactive() const 
{
  return SelectedInteractive();
}

Handle(AIS_InteractiveObject) AIS_InteractiveContext::SelectedInteractive() const 
{
  if(!HasOpenedContext()){
    Handle(Standard_Transient) TR  =AIS_Selection::Selection(myCurrentName.ToCString())->Value();
    Handle(AIS_InteractiveObject) IO = *((Handle(AIS_InteractiveObject)*)&TR);
    return IO;}
  
  return  myLocalContexts(myCurLocalIndex)->SelectedInteractive();
  
}
//=======================================================================
//function : Interactive
//purpose  : 
//=======================================================================

Handle(SelectMgr_EntityOwner) AIS_InteractiveContext::SelectedOwner() const 
{
  if(!HasOpenedContext()){
    Handle(SelectMgr_EntityOwner) Ownr;
    return Ownr;
  }
  return  myLocalContexts(myCurLocalIndex)->SelectedOwner();
  
}

//=======================================================================
//function : EntityOwners
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::EntityOwners(SelectMgr_IndexedMapOfOwner& theOwners,
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

  TColStd_ListIteratorOfListOfInteger anItr( aModes );
  for (; anItr.More(); anItr.Next() )
  {
    int aMode = anItr.Value();
    if ( !theIObj->HasSelection( aMode ) )
      continue;

    Handle(SelectMgr_Selection) aSel = theIObj->Selection( aMode );

    for ( aSel->Init(); aSel->More(); aSel->Next() )
    {
      Handle(SelectBasics_SensitiveEntity) aEntity = aSel->Sensitive();
      if ( aEntity.IsNull() )
	continue;

      Handle(SelectMgr_EntityOwner) aOwner =
	Handle(SelectMgr_EntityOwner)::DownCast(aEntity->OwnerId());
      if ( !aOwner.IsNull() )
	theOwners.Add( aOwner );
    }
  }
}

Standard_Integer AIS_InteractiveContext::NbSelected() 
{
  Standard_Integer i(0);
  for(InitSelected();MoreSelected();NextSelected())
    {i++;}
  return i;
}

//=======================================================================
//function : HasApplicative
//purpose  : 
//=======================================================================

  Standard_Boolean AIS_InteractiveContext::HasApplicative() const 
{
  return Interactive()->HasOwner();
}

//=======================================================================
//function : Applicative
//purpose  : 
//=======================================================================

Handle(Standard_Transient) AIS_InteractiveContext::Applicative() const 
{
  return Interactive()->GetOwner();
}



//==================================================
// Function: 
// Purpose :
//==================================================
Standard_Boolean AIS_InteractiveContext::HasDetected() const 
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->HasDetected();
  else
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

Handle(AIS_InteractiveObject) 
AIS_InteractiveContext::DetectedInteractive() const 
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->DetectedInteractive();
  return myLastPicked;
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
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->DetectedOwner();
  Handle(SelectMgr_EntityOwner) Ownr;
  return Ownr;
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
