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

//Modified by ROB : Traque des UpdateConversion intempestifs.

#include <AIS_DataMapIteratorOfDataMapOfSelStat.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_LocalStatus.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <SelectMgr_CompositionFilter.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_OrFilter.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_ShapeTypeFilter.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(AIS_LocalContext,Standard_Transient)

//=======================================================================
//function : AIS_LocalContext
//purpose  : 
//=======================================================================


AIS_LocalContext::AIS_LocalContext(){}

AIS_LocalContext::AIS_LocalContext(const Handle(AIS_InteractiveContext)& aCtx,
				   const Standard_Integer Index,
				   const Standard_Boolean LoadDisplayed,
				   const Standard_Boolean AcceptStandardModes,
				   const Standard_Boolean AcceptEraseOfTemp,
				   const Standard_Boolean /*BothViewers*/):
myCTX(aCtx),
myLoadDisplayed(LoadDisplayed),
myAcceptStdMode(AcceptStandardModes),
myAcceptErase(AcceptEraseOfTemp),
mySM(aCtx->SelectionManager()),
myMainVS(aCtx->MainSelector()),
myFilters(new SelectMgr_OrFilter()),
myAutoHilight(Standard_True),
myMapOfOwner (new SelectMgr_IndexedMapOfOwner()),
mySelection(new AIS_Selection()),
mylastindex(0),
mylastgood(0),
myCurDetected(0),
myAISCurDetected(0),
mySubintStyle (new Prs3d_Drawer())
{
  mySubintStyle->Link (aCtx->SelectionStyle());
  mySubintStyle->SetColor        (aCtx->SelectionStyle()->Color());
  mySubintStyle->SetTransparency (aCtx->SelectionStyle()->Transparency());
  mySubintStyle->SetMethod       (aCtx->SelectionStyle()->Method());

  // bind self to AIS_InteractiveContext::myLocalContexts. Further, the
  // constructor executes logic that implies that the context is already
  // created and mapped.
  aCtx->myLocalContexts.Bind (Index, this);

  myMainVS->ResetSelectionActivationStatus();
  myMainPM = aCtx->MainPrsMgr();

  mySM->Add(myMainVS);
  if(myLoadDisplayed) LoadContextObjects();
  Process();

}


//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================

void AIS_LocalContext::SetContext(const Handle(AIS_InteractiveContext)& aCtx)
{  myCTX = aCtx;}

//=======================================================================
//function : Display
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::Display(const Handle(AIS_InteractiveObject)& anInteractive,
					   const Standard_Integer WhichMode,
					   const Standard_Boolean AllowShapeDecomposition,
					   const Standard_Integer ActivationMode)
{
  if(myActiveObjects.IsBound(anInteractive)){
    const Handle(AIS_LocalStatus)& STAT = myActiveObjects(anInteractive);
    
    if(STAT->DisplayMode() == -1){
      if(!myMainPM->IsDisplayed(anInteractive,WhichMode))
	myMainPM->Display(anInteractive,WhichMode);
      if(STAT->IsTemporary())
	STAT->SetDisplayMode(WhichMode);
    }
    else if(STAT->DisplayMode()!=WhichMode && STAT->IsTemporary()){
      myMainPM->Erase(anInteractive,STAT->DisplayMode());
      STAT->SetDisplayMode(WhichMode);
      if(!myMainPM->IsDisplayed(anInteractive,WhichMode))
	myMainPM->Display(anInteractive,WhichMode);
    }
    
    if(ActivationMode!=-1){
      if(!STAT->IsActivated(ActivationMode)){
	STAT->ClearSelectionModes();
	mySM->Load(anInteractive,myMainVS);
	STAT->AddSelectionMode(ActivationMode);
	mySM->Activate(anInteractive,ActivationMode,myMainVS);
      }
    }
  }
  else {
    Handle(AIS_LocalStatus) Att = new AIS_LocalStatus();
    
    if(anInteractive->AcceptShapeDecomposition() && AllowShapeDecomposition)
      Att->SetDecomposition(Standard_True);
    else 
      Att->SetDecomposition(Standard_False);
    // status temporary or not
    if(myCTX->DisplayStatus(anInteractive) == AIS_DS_None ||
       myCTX->DisplayStatus(anInteractive) == AIS_DS_Temporary)
      Att->SetTemporary(Standard_True);
    else
      Att->SetTemporary(Standard_False); 


    
    if(!myCTX->IsDisplayed(anInteractive,WhichMode)){
      
      //storing information....
      Att->SetDisplayMode(WhichMode);
      if (ActivationMode!=-1)
	Att->AddSelectionMode(ActivationMode);
      Standard_Integer HiMod = anInteractive->HasHilightMode()? anInteractive->HilightMode(): WhichMode;
      Att->SetHilightMode(HiMod);

      if(!myMainPM->IsDisplayed(anInteractive,WhichMode))
	myMainPM->Display(anInteractive,WhichMode);
      
      if(ActivationMode!=-1){
	mySM->Load(anInteractive,myMainVS);
	mySM->Activate(anInteractive,ActivationMode,myMainVS);
      }
    }
    else{
      Standard_Integer HiMod = anInteractive->HasHilightMode()? anInteractive->HilightMode(): WhichMode;
      Att->SetHilightMode(HiMod);
    }
    myActiveObjects.Bind(anInteractive,Att);
  }  
  Process(anInteractive);

  
  

  return Standard_True;
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::
Load(const Handle(AIS_InteractiveObject)& anInteractive,
     const Standard_Boolean AllowShapeDecomposition,
     const Standard_Integer ActivationMode)
{
  if (myActiveObjects.IsBound (anInteractive))
  {
    if (anInteractive->HasSelection (ActivationMode))
    {
      const Handle(SelectMgr_Selection)& aSel = anInteractive->Selection (ActivationMode);
      if (aSel->GetSelectionState() != SelectMgr_SOS_Activated)
      {
        if (!myMainVS->Contains (anInteractive))
        {
          mySM->Load (anInteractive, myMainVS);
        }
        mySM->Activate (anInteractive, ActivationMode, myMainVS);
        return Standard_True;
      }
    }
    return Standard_False;
  }

  Handle(AIS_LocalStatus) Att = new AIS_LocalStatus();
  
  if(anInteractive->AcceptShapeDecomposition() && AllowShapeDecomposition)
    Att->SetDecomposition(Standard_True);
  else 
    Att->SetDecomposition(Standard_False);
  
  if(!myCTX->IsDisplayed(anInteractive))
    Att->SetTemporary(Standard_True);
  else
    Att->SetTemporary(Standard_False);
  Att->SetDisplayMode(-1);
  
  //storing information....
  if(ActivationMode!=-1)
    Att->AddSelectionMode(ActivationMode);
  Standard_Integer HiMod = anInteractive->HasHilightMode()? anInteractive->HilightMode():0;
  Att->SetHilightMode(HiMod);
  //Action
  
  mySM->Load(anInteractive,myMainVS);
  if(ActivationMode != -1){
    mySM->Activate(anInteractive,ActivationMode,myMainVS);
  }
  myActiveObjects.Bind(anInteractive,Att);
  Process(anInteractive);
  return Standard_True;
}

//=======================================================================
//function : ClearPrs
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::
ClearPrs(const Handle(AIS_InteractiveObject)& anInteractive,
	 const Standard_Integer aMode)
{
  if(!myActiveObjects.IsBound(anInteractive))
    return Standard_False;

  Standard_Boolean jobdone(Standard_False);
  const Handle(AIS_LocalStatus)& STAT = myActiveObjects(anInteractive);
  
  //Display step
  if(STAT->IsSubIntensityOn()) {
    STAT->SubIntensityOff();
    if(STAT->HilightMode()==aMode)
      myMainPM->Unhighlight(anInteractive);
  }
  myMainPM->Clear(anInteractive,aMode); // correction connexions 23/09/97
  jobdone = Standard_True;
  if(STAT->DisplayMode()==aMode)
    STAT->SetDisplayMode(-1);
  return jobdone;
}
//=======================================================================
//function : Erase
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::
Erase(const Handle(AIS_InteractiveObject)& anInteractive)
{
  if(!myActiveObjects.IsBound(anInteractive))
    return Standard_False;
  const Handle(AIS_LocalStatus)& STAT = myActiveObjects(anInteractive);
  
  //Display step
  if(STAT->IsSubIntensityOn()) {
    STAT->SubIntensityOff();
    myMainPM->Unhighlight (anInteractive);
  }

  Standard_Boolean status(Standard_False);

  if(STAT->DisplayMode()!=-1) {
    if(IsSelected(anInteractive))
      AddOrRemoveSelected(anInteractive);
    if(myMainPM->IsHighlighted(anInteractive,STAT->HilightMode()))
    {
      myMainPM->Unhighlight (anInteractive);
    }
    myMainPM->SetVisibility (anInteractive, STAT->DisplayMode(), Standard_False);
    STAT->SetDisplayMode(-1);
    status = Standard_True;
  }
  if(STAT->IsTemporary()){
    if(myMainPM->IsDisplayed(anInteractive,STAT->HilightMode()))
      myMainPM->SetVisibility (anInteractive, STAT->HilightMode(), Standard_False);
  }

  // Deactivate selectable entities of interactive object
  const Handle(SelectMgr_SelectableObject)& anObj = anInteractive; // to avoid ambiguity
  if (mySM->Contains (anObj))
  {
    while (!STAT->SelectionModes().IsEmpty())
    {
      mySM->Deactivate (anInteractive, STAT->SelectionModes().Last(), myMainVS);
      STAT->RemoveSelectionMode (STAT->SelectionModes().Last());
    }
  }

  ClearOutdatedSelection (anInteractive, Standard_True);

  return status;
}


//=======================================================================
//function : SetShapeDecomposition
//purpose  : 
//=======================================================================

void AIS_LocalContext::SetShapeDecomposition(const Handle(AIS_InteractiveObject)& aStoredObject, 
						const Standard_Boolean aStatus)
{
  if(!myActiveObjects.IsBound(aStoredObject)) return;
  
  if(aStatus == myActiveObjects(aStoredObject)->Decomposed()) 
    return;
  
  myActiveObjects(aStoredObject)->SetDecomposition(aStatus);

  Process(aStoredObject);
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void AIS_LocalContext::Clear(const AIS_ClearMode aType)
{
  switch (aType){
  case AIS_CM_All:
    {
      ClearObjects();
      myFilters->Clear();
      while(!myListOfStandardMode.IsEmpty())
	DeactivateStandardMode(AIS_Shape::SelectionType(myListOfStandardMode.Last()));
      break;
    }
  case AIS_CM_Interactive:
    ClearObjects();
    break;
  case AIS_CM_Filters:
    myFilters->Clear();
    break;
  case AIS_CM_StandardModes:
    {
      while(!myListOfStandardMode.IsEmpty())
	DeactivateStandardMode(AIS_Shape::SelectionType(myListOfStandardMode.Last()));
      break;
    }
  case AIS_CM_TemporaryShapePrs:
    ClearDetected();
  }
}
//=======================================================================
//function : ActivateMode
//purpose  : 
//=======================================================================

void AIS_LocalContext::ActivateMode(const Handle(AIS_InteractiveObject)& aSelectable,
				       const Standard_Integer aMode)
{
  if(!myActiveObjects.IsBound(aSelectable)) return;
//  if(myActiveObjects(aSelectable)->SelectionMode()!=aMode)
//    mySM->Deactivate(aSelectable,aMode,myMainVS);
  if(aMode != -1){
    myActiveObjects(aSelectable)->AddSelectionMode(aMode);
    mySM->Activate(aSelectable,aMode,myMainVS);
  }
}
//=======================================================================
//function : ActivateMode
//purpose  : 
//=======================================================================

void AIS_LocalContext::DeactivateMode(const Handle(AIS_InteractiveObject)& aSelectable,
					 const Standard_Integer aMode)
{
  if(!myActiveObjects.IsBound(aSelectable)) return;
  
  if(aMode==-1) return;
  
  myActiveObjects(aSelectable)->RemoveSelectionMode(aMode);
  mySM->Deactivate(aSelectable,aMode,myMainVS);
}
//=======================================================================
//function : ActivateMode
//purpose  : 
//=======================================================================

void AIS_LocalContext::Deactivate(const Handle(AIS_InteractiveObject)& aSelectable)
{
  if(!myActiveObjects.IsBound(aSelectable)) return;
  
  mySM->Deactivate(aSelectable, -1, myMainVS);
  myActiveObjects(aSelectable)->ClearSelectionModes();
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::Remove(const Handle(AIS_InteractiveObject)& aSelectable)
{
  if(!myActiveObjects.IsBound (aSelectable))
  {
    return Standard_False;
  }

  if (IsSelected (aSelectable))
  {
    AddOrRemoveSelected (aSelectable, Standard_False);
  }

  const Handle(AIS_LocalStatus)& Att = myActiveObjects (aSelectable);

  TColStd_ListIteratorOfListOfInteger It;

  // Deactivate standard modes
  if (Att->Decomposed())
  {
    for (It.Initialize (myListOfStandardMode); It.More(); It.Next())
    {
      mySM->Deactivate (aSelectable, It.Value(), myMainVS);
    }
  }

  // If object or temporary presentations
  if (Att->IsTemporary())
  {
    if (Att->IsSubIntensityOn())
    {
      myMainPM->Unhighlight (aSelectable);
    }

    myMainPM->Erase (aSelectable, Att->DisplayMode());
    if (myMainPM->IsDisplayed (aSelectable, Att->HilightMode()))
    {
      myMainPM->Erase (aSelectable, Att->HilightMode());
    }
  }
  // If subintensity used
  else if (Att->IsSubIntensityOn())
  {
    myCTX->SubIntensityOff (aSelectable, Standard_False);
  }

  // Deactivate stored selection modes
  for (It.Initialize (Att->SelectionModes()); It.More(); It.Next())
  {
    mySM->Deactivate (aSelectable, It.Value(), myMainVS);
  }

  // Remove the interactive object from selection manager
  const Handle(SelectMgr_SelectableObject)& anObj = aSelectable; // to avoid ambiguity
  if (mySM->Contains (anObj))
  {
    mySM->Remove (anObj);
  }
  ClearOutdatedSelection (aSelectable, Standard_True);

  // This should be done at the very end because most methods use
  // myActiveObjects even during clean-up
  myActiveObjects.UnBind (aSelectable);
  return Standard_True;
}

//=======================================================================
//function : ActivateStandardMode
//purpose  : 
//=======================================================================

void AIS_LocalContext::ActivateStandardMode(const TopAbs_ShapeEnum aType)
{
  
  //check if it is not in the list
  TColStd_ListIteratorOfListOfInteger It(myListOfStandardMode);
  for(;It.More();It.Next())
    if(It.Value()==aType)  return;
  Standard_Integer IMode = AIS_Shape::SelectionMode(aType);
  

  // create a hidden filter answering ok to the type except for :
  // if the type is shape...
  // if the filters already impact at the type <aType>
  if(aType != TopAbs_SHAPE){
    if(myStdFilters[IMode].IsNull())
      myStdFilters[IMode] = new StdSelect_ShapeTypeFilter(aType);
    if(!HasFilters(aType))
      myFilters->Add(myStdFilters[IMode]);
  }
  
  // the mode is activated for all objects of type Shape 
  // accepting the decomposition in standard mode.
  myListOfStandardMode.Append(IMode);
  
  AIS_DataMapIteratorOfDataMapOfSelStat ItM(myActiveObjects);

  for(;ItM.More();ItM.Next()){
    if(ItM.Value()->Decomposed())
      myCTX->SelectionManager()->Activate(ItM.Key(),
					  IMode,
					  myMainVS);
    ItM.Value()->AddSelectionMode (IMode);
  }
  
}

//=======================================================================
//function : DeActivateStandardMode
//purpose  : 
//=======================================================================

void AIS_LocalContext::DeactivateStandardMode(const TopAbs_ShapeEnum aType)
{
  TColStd_ListIteratorOfListOfInteger It(myListOfStandardMode);
  Standard_Integer IMode = AIS_Shape::SelectionMode(aType);
  for(;It.More();It.Next())
    if(It.Value()==IMode) {
      AIS_DataMapIteratorOfDataMapOfSelStat ItM(myActiveObjects);
      
      for(;ItM.More();ItM.Next()){
	if(ItM.Value()->Decomposed()){
	  myCTX->SelectionManager()->Deactivate(ItM.Key(),
						IMode,
						myMainVS);
	  ItM.Value()->RemoveSelectionMode(IMode);
	}
      }
      myListOfStandardMode.Remove(It);
      if(myFilters->IsIn(myStdFilters[IMode]))
	myFilters->Remove(myStdFilters[IMode]);
      return;
    }	
}

//=======================================================================
//function : AddFilter
//purpose  : 
//=======================================================================

void AIS_LocalContext::AddFilter(const Handle(SelectMgr_Filter)& aFilter)
{
  // it is checked if the filter impacts at the type of active sub-shape 
  // for which a filter of type has been already implemented...

  TColStd_ListIteratorOfListOfInteger It(myListOfStandardMode);
  
  for(;It.More();It.Next()){
    if(aFilter->ActsOn(AIS_Shape::SelectionType(It.Value())))
      if(myFilters->IsIn(myStdFilters[It.Value()]))
	myFilters->Remove(myStdFilters[It.Value()]);
  } 
  myFilters->Add(aFilter);
}

//=======================================================================
//function : RemoveFilter
//purpose  : 
//=======================================================================

void AIS_LocalContext::RemoveFilter(const Handle(SelectMgr_Filter)& aFilter)
{
  if(myFilters->IsIn(aFilter)) myFilters->Remove(aFilter);
  
  // it is checked if the filter for type standard is active.
  // if yes, it is checked there are still similarities among the
  // remaining filters...
  //     otherwise, the standard filter is restored to
  //     continu selecting active modes...
  TColStd_ListIteratorOfListOfInteger It(myListOfStandardMode);
  TopAbs_ShapeEnum SE;
  for(;It.More();It.Next()){
    SE = AIS_Shape::SelectionType(It.Value());
    if(aFilter->ActsOn(SE))
      if(!HasFilters(SE))
	myFilters->Add(myStdFilters[It.Value()]);
  }
}

//=======================================================================
//function : Terminate
//purpose  :
//=======================================================================

void AIS_LocalContext::Terminate (const Standard_Boolean theToUpdate)
{
  ClearDetected();
  Clear();
  myMapOfOwner->Clear();
  
  mylastindex=0;
  // clear the selector...
  myMainVS->Clear();
  
  for (AIS_NListOfEntityOwner::Iterator aSelIter (mySelection->Objects()); aSelIter.More(); aSelIter.Next())
  {
    aSelIter.Value()->SetSelected (Standard_False);
  }
  mySelection->Clear();

  Handle(V3d_View) aDummyView;
  myMainVS->ClearSensitive (aDummyView);

  if (theToUpdate)
  {
    myCTX->UpdateCurrentViewer();
  }
}


//=======================================================================
//function : SubIntensity
//purpose  : 
//=======================================================================

void AIS_LocalContext::SubIntensityOn(const Handle(AIS_InteractiveObject)& anObject)
{
  if(!myActiveObjects.IsBound(anObject)) return;
  mySubintStyle->SetColor (myCTX->SubIntensityColor());
  
  const Handle(AIS_LocalStatus)& Att = myActiveObjects(anObject);

  if(Att->IsTemporary())
  {
    myMainPM->Color (anObject, mySubintStyle, Att->DisplayMode());
  }
  
  Att->SubIntensityOn();
}
//=======================================================================
//function : SubIntensity
//purpose  : 
//=======================================================================

void AIS_LocalContext::SubIntensityOff(const Handle(AIS_InteractiveObject)& anObject)
{
  if(!myActiveObjects.IsBound(anObject)) return;
  
  const Handle(AIS_LocalStatus)& Att = myActiveObjects(anObject);

  if(Att->IsTemporary()) 
    myMainPM->Unhighlight(anObject);
  Att->SubIntensityOff();
}


//=======================================================================
//function : Hilight
//purpose  : 
//=======================================================================

void AIS_LocalContext::Hilight(const  Handle(AIS_InteractiveObject)& anObject)
{
  if(!myActiveObjects.IsBound(anObject)){
    Standard_Integer HiMod = anObject->HasHilightMode()? anObject->HilightMode() : 0; 
    Handle(AIS_LocalStatus) Att = new AIS_LocalStatus(Standard_True,
						      Standard_False,
						      -1,-1,HiMod);
    myActiveObjects.Bind(anObject,Att);
    
  }
  const Handle(AIS_LocalStatus)& Att = myActiveObjects(anObject);
  myMainPM->Color(anObject, myCTX->getHiStyle (anObject, anObject->GlobalSelOwner()), Att->HilightMode());
  Att->SubIntensityOn();
}
//=======================================================================
//function : Hilight
//purpose  : 
//=======================================================================

void AIS_LocalContext::Hilight (const Handle(AIS_InteractiveObject)& theObj,
                                const Handle(Prs3d_Drawer)& theStyle)
{
  if (!myActiveObjects.IsBound (theObj))
  {
    Handle(AIS_LocalStatus) aStatus = new AIS_LocalStatus
      (Standard_True, Standard_False, -1, -1, theObj->HasHilightMode() ? theObj->HilightMode() : 0);
    myActiveObjects.Bind (theObj, aStatus);
  }
  const Handle(AIS_LocalStatus)& aStatus = myActiveObjects (theObj);
  myMainPM->Color (theObj, theStyle, aStatus->HilightMode());
  aStatus->SubIntensityOn();
  aStatus->SetHilightStyle (theStyle);
}

//=======================================================================
//function : Unhilight
//purpose  : 
//=======================================================================

void AIS_LocalContext::Unhilight(const Handle(AIS_InteractiveObject)& anObject)
{
  if(!myActiveObjects.IsBound(anObject)) return;
  
  // chieck if by hazard the object is somewhere else...
  Standard_Integer Indx;
  Standard_DISABLE_DEPRECATION_WARNINGS
  Standard_Boolean IsSomeWhereElse  = 
    myCTX->IsInLocal(anObject,Indx) && Indx != myCTX->IndexOfCurrentLocal();
  Standard_ENABLE_DEPRECATION_WARNINGS
  
  const Handle(AIS_LocalStatus)& Att = myActiveObjects(anObject);
  myMainPM->Unhighlight (anObject);
  if(Att->IsTemporary() && Att->DisplayMode()==-1)
    if(!IsSomeWhereElse)
      myMainPM->SetVisibility (anObject, Att->HilightMode(), Standard_False);

  Att->SubIntensityOff();
  Att->SetHilightStyle (Handle(Prs3d_Drawer)());
}


//=======================================================================
//function : IsIn
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::
IsIn(const Handle(AIS_InteractiveObject)& anObject) const 
{
  return myActiveObjects.IsBound(anObject);
}

//=======================================================================
//function : IsHilighted
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::IsHilighted(const Handle(AIS_InteractiveObject)& anObject) const 
{
  if(!myActiveObjects.IsBound(anObject)) return Standard_False;
  return myActiveObjects(anObject)->IsSubIntensityOn();
}

Standard_Boolean AIS_LocalContext::HighlightStyle (const Handle(AIS_InteractiveObject)& theObject,
                                                   Handle(Prs3d_Drawer)& theStyle) const
{
  if (!myActiveObjects.IsBound (theObject))
    return Standard_False;

  if (myActiveObjects (theObject)->IsSubIntensityOn())
  {
    theStyle = myActiveObjects (theObject)->HilightStyle();
    return Standard_True;
  }

  return Standard_False;
}


void AIS_LocalContext::SetDisplayPriority(const Handle(AIS_InteractiveObject)& anObject,
					  const Standard_Integer Prior)
{
  if(!myActiveObjects.IsBound(anObject)) return;
  const Handle(AIS_LocalStatus)& STAT = myActiveObjects(anObject);
  if(STAT->DisplayMode()==-1) return;
  myMainPM->SetDisplayPriority(anObject,STAT->DisplayMode(),Prior);
  if(STAT->IsSubIntensityOn())
    myMainPM->SetDisplayPriority(anObject,STAT->HilightMode(),Prior);
  
  
}

//=======================================================================
//function : DisplayedObjects
//purpose  : 
//=======================================================================
Standard_Integer AIS_LocalContext::DisplayedObjects(TColStd_MapOfTransient& theMap) const
{
  Standard_Integer NbDisp(0);
  for(AIS_DataMapIteratorOfDataMapOfSelStat it(myActiveObjects);it.More();it.Next()){
    const Handle(SelectMgr_SelectableObject)& SO = it.Key();
    if(!theMap.Contains(SO))
      if(it.Value()->DisplayMode()!=-1){
	theMap.Add(SO);
	NbDisp++;
      }
  }
  return NbDisp;
}


//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::IsDisplayed(const Handle(AIS_InteractiveObject)& anObject) const 
{
  if(!myActiveObjects.IsBound(anObject)) return Standard_False;
  return (myActiveObjects(anObject)->DisplayMode()!=-1);
}

//=======================================================================
//function : IsDisplayed
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::IsDisplayed(const Handle(AIS_InteractiveObject)& anObject,
						  const Standard_Integer aMode) const 
{
  if(!myActiveObjects.IsBound(anObject)) return Standard_False;
  return (myActiveObjects(anObject)->DisplayMode()==aMode);
}

//=======================================================================
//function : SelectionModes
//purpose  : 
//=======================================================================

const TColStd_ListOfInteger& AIS_LocalContext::
SelectionModes(const Handle(AIS_InteractiveObject)& anObject) const 
{
  return myActiveObjects(anObject)->SelectionModes(); 
}

//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

TCollection_AsciiString AIS_LocalContext::Status() const 
{
  TCollection_AsciiString t;
  return t;
}

const Handle(AIS_LocalStatus)& AIS_LocalContext::Status(const Handle(AIS_InteractiveObject)& anObject) const 
{
  return myActiveObjects(anObject);
}

//=======================================================================
//function : LoadContextObjects
//purpose  : 
//=======================================================================

void AIS_LocalContext::LoadContextObjects()
{
  AIS_ListIteratorOfListOfInteractive It;
  if(myLoadDisplayed) {
    AIS_ListOfInteractive LL;
    myCTX->DisplayedObjects(LL,Standard_True);
    Handle(AIS_LocalStatus) Att;
    for (It.Initialize(LL);It.More();It.Next()){
      const Handle(AIS_InteractiveObject)& anObj = It.Value();
      Att= new AIS_LocalStatus();
      Att->SetDecomposition((anObj->AcceptShapeDecomposition() && myAcceptStdMode));
      Att->SetTemporary(Standard_False);
      Att->SetHilightMode(anObj->HasHilightMode()? anObj->HilightMode(): 0);
      for (anObj->Init(); anObj->More(); anObj->Next())
      {
        const Handle(SelectMgr_Selection)& aSel = anObj->CurrentSelection();
        aSel->SetSelectionState (SelectMgr_SOS_Deactivated);
      }
      myActiveObjects.Bind(anObj,Att);
    }
  }
}

void AIS_LocalContext::UnloadContextObjects()
{
  AIS_ListIteratorOfListOfInteractive It;
  if(myLoadDisplayed) 
  {
    AIS_ListOfInteractive LL;
    myCTX->DisplayedObjects(LL,Standard_True);
    
    for (It.Initialize(LL);It.More();It.Next())
    {
      myActiveObjects.UnBind(It.Value());
    }
  }
}
//=======================================================================
//function : Process
//purpose  : 
//=======================================================================

void AIS_LocalContext::Process(const Handle(SelectMgr_SelectableObject)& anObject)
{ 
  if(!myActiveObjects.IsBound(anObject)) return;
  if(myActiveObjects(anObject)->Decomposed())
    ActivateStandardModes(anObject);
  else
    {
      TColStd_ListIteratorOfListOfInteger It(myActiveObjects(anObject)->SelectionModes());
      for(;It.More();It.Next())
	myCTX->SelectionManager()->Activate(anObject,It.Value(),myMainVS);
    }
}

//=======================================================================
//function : Process
//purpose  : 
//=======================================================================

void AIS_LocalContext::Process()
{ 

  myMainVS->Clear();
  
  AIS_DataMapIteratorOfDataMapOfSelStat It(myActiveObjects);
  
  for(;It.More();It.Next()){
    myCTX->SelectionManager()->Load(It.Key(),myMainVS);
    if(It.Value()->Decomposed()) 
      ActivateStandardModes(It.Key());
    else if( myCTX->GetAutoActivateSelection() )
    {
      It.Value()->AddSelectionMode(0);
      myCTX->SelectionManager()->Activate(It.Key(),0,myMainVS);
    }
  }

}

//=======================================================================
//function : ActivateModes
//purpose  : 
//=======================================================================

void AIS_LocalContext::ActivateStandardModes(const Handle(SelectMgr_SelectableObject)& anObject)
{ 
  if(!myActiveObjects.IsBound(anObject)) return;
  
  TColStd_ListIteratorOfListOfInteger itl (myListOfStandardMode);

  const Handle(AIS_LocalStatus)&  LS = myActiveObjects(anObject);
  if(LS->Decomposed()){
    for(;itl.More();itl.Next()){
      myCTX->SelectionManager()->Activate(anObject,itl.Value(),myMainVS);
      LS->AddSelectionMode(itl.Value());
    }
  }
}


//=======================================================================
//function : ClearObjects
//purpose  : 
//=======================================================================

void AIS_LocalContext::ClearObjects()
{
  AIS_DataMapIteratorOfDataMapOfSelStat It(myActiveObjects);
  for(;It.More();It.Next())
    {
      Handle(AIS_InteractiveObject) SO =
	Handle(AIS_InteractiveObject)::DownCast(It.Key());
      
      const Handle(AIS_LocalStatus)& CurAtt = It.Value();
      //TColStd_ListIteratorOfListOfInteger ItL;
      // if object is temporary the presentations managed by myMainPM are removed
      AIS_DisplayStatus TheDS = myCTX->DisplayStatus(SO);
      
      if(TheDS != AIS_DS_Displayed)
      {
        if (myMainPM->IsDisplayed(SO,CurAtt->DisplayMode()))
        {
          if (CurAtt->IsSubIntensityOn() && myMainPM->IsHighlighted (SO, CurAtt->HilightMode()))
          {
            myMainPM->Unhighlight (SO);
          }
          myMainPM->Erase (SO, CurAtt->DisplayMode());
        }

        if (CurAtt->IsTemporary())
        {
          myMainPM->Erase (SO, CurAtt->DisplayMode());
          //myMainPM->Clear(SO,CurAtt->DisplayMode());
        }
      }
      else {
	if (CurAtt->IsSubIntensityOn())
        {
          myCTX->SubIntensityOff(SO, Standard_False);
        }
	Standard_Integer DiMo = SO->HasDisplayMode()?
	  SO->DisplayMode():myCTX->DisplayMode();
	if(CurAtt->DisplayMode()!=-1 &&
	   CurAtt->DisplayMode()!= DiMo)
	  myMainPM->Erase(SO,CurAtt->DisplayMode());
      }
      
      TColStd_ListIteratorOfListOfInteger aSelModeIter (CurAtt->SelectionModes());
      for ( ; aSelModeIter.More(); aSelModeIter.Next())
      {
        Standard_Integer aSelMode = aSelModeIter.Value();
        mySM->Deactivate (SO, aSelMode, myMainVS);
      }

    }
  ClearSelected( Standard_False );

  // Clear selection structures for temporary objects, created in local context
  for (AIS_DataMapIteratorOfDataMapOfSelStat anIter (myActiveObjects); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->IsTemporary())
    {
      mySM->Remove (anIter.Key(), myMainVS);
    }
  }

  myActiveObjects.Clear();
}


Standard_Boolean AIS_LocalContext::IsDecompositionOn() const 
{return !myListOfStandardMode.IsEmpty();}




//=======================================================================
//function : HasAlreadyFilters
//purpose  : 
//=======================================================================

Standard_Boolean AIS_LocalContext::
HasFilters(const TopAbs_ShapeEnum aType) const 
{
  return myFilters->ActsOn(aType);
}

void AIS_LocalContext::ClearDetected()
{
  for(Standard_Integer I=1;I<=myMapOfOwner->Extent();I++)
  {
    if(!myMapOfOwner->FindKey (I).IsNull())
    {
      if(myMapOfOwner->FindKey (I)->IsHilighted(myMainPM))
        myMapOfOwner->FindKey (I)->Unhilight(myMainPM);
      else
      {
        const Handle(SelectMgr_SelectableObject)& SO = 
          myMapOfOwner->FindKey (I)->Selectable();
        if(myActiveObjects.IsBound(SO))
        {
          const Handle(AIS_LocalStatus)& Att = myActiveObjects(SO);

          if(Att->IsTemporary() &&
             Att->DisplayMode()==-1 && 
             Att->SelectionModes().IsEmpty())
          {
            myMapOfOwner->FindKey (I)->Clear(myMainPM);
          }
        }
      }
    }
  }
}

//=======================================================================
//function : BeginImmediateDraw
//purpose  :
//=======================================================================
Standard_Boolean AIS_LocalContext::BeginImmediateDraw()
{
  if (myMainPM->IsImmediateModeOn())
  {
    myMainPM->BeginImmediateDraw();
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : ImmediateAdd
//purpose  :
//=======================================================================
Standard_Boolean AIS_LocalContext::ImmediateAdd (const Handle(AIS_InteractiveObject)& theObj,
                                                 const Standard_Integer               theMode)
{
  if (!myMainPM->IsImmediateModeOn())
  {
    return Standard_False;
  }

  myMainPM->AddToImmediateList (myMainPM->Presentation (theObj, theMode)->Presentation());
  return Standard_True;
}

//=======================================================================
//function : EndImmediateDraw
//purpose  :
//=======================================================================
Standard_Boolean AIS_LocalContext::EndImmediateDraw (const Handle(V3d_Viewer)& theViewer)
{
  if (!myMainPM->IsImmediateModeOn())
  {
    return Standard_False;
  }

  myMainPM->EndImmediateDraw (theViewer);
  return Standard_True;
}

// =======================================================================
// function : ClearImmediateDraw
// purpose  :
// =======================================================================
void AIS_LocalContext::ClearImmediateDraw()
{
  myMainPM->ClearImmediateDraw();
}

//=======================================================================
//function : IsImmediateModeOn
//purpose  :
//=======================================================================
Standard_Boolean AIS_LocalContext::IsImmediateModeOn() const
{
  return myMainPM->IsImmediateModeOn();
}

void AIS_LocalContext::SetPixelTolerance(const Standard_Integer aPrecision) {

  myMainVS->SetPixelTolerance(aPrecision);
}

Standard_Integer AIS_LocalContext::PixelTolerance() const {

  return myMainVS->PixelTolerance();
}

//=======================================================================
//function : SetSelectionSensitivity
//purpose  : Allows to manage sensitivity of a particular selection of interactive object theObject
//=======================================================================
void AIS_LocalContext::SetSelectionSensitivity (const Handle(AIS_InteractiveObject)& theObject,
                                                const Standard_Integer theMode,
                                                const Standard_Integer theNewSensitivity)
{
  mySM->SetSelectionSensitivity (theObject, theMode, theNewSensitivity);
}
