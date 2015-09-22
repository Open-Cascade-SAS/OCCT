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


#include <AIS_DataMapIteratorOfDataMapOfILC.hxx>
#include <AIS_DataMapIteratorOfDataMapOfIOStatus.hxx>
#include <AIS_GlobalStatus.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_Selection.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Prs3d_BasicAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_OrFilter.hxx>
#include <SelectMgr_SelectionManager.hxx>
#include <Standard_Transient.hxx>
#include <StdSelect_ViewerSelector3d.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

//=======================================================================
//function : OpenLocalContext
//purpose  : 
//=======================================================================
Standard_Integer AIS_InteractiveContext::
OpenLocalContext(const Standard_Boolean UseDisplayedObjects, 
                 const Standard_Boolean AllowShapeDecomposition, 
                 const Standard_Boolean AcceptEraseOfTemporary,
                 const Standard_Boolean /*BothViewers*/)
{

  // the entities eventually detected just before the context was opened are unhighlighted...
  if(!IsSelected(myLastPicked)){
    if(!myLastPicked.IsNull()){
      const Handle(AIS_InteractiveObject) aLastPickedAIS =
        Handle(AIS_InteractiveObject)::DownCast (myLastPicked->Selectable());
      Standard_Integer HiMod = aLastPickedAIS->HasHilightMode()?aLastPickedAIS->HilightMode():0;
      myMainPM->Unhighlight (aLastPickedAIS, HiMod);
    }}
  
  if(!mylastmoveview.IsNull()){
    if(myCurLocalIndex>0)
      myLocalContexts(myCurLocalIndex)->UnhilightLastDetected(mylastmoveview);
  }
  
  // entities connected to dynamic selection at neutral point are set to 0.
  
  myLastinMain.Nullify();
  myLastPicked.Nullify();
  myWasLastMain = Standard_True;

  myCurLocalIndex = HighestIndex() + 1;
  
  Handle(AIS_LocalContext) NewLocal= new AIS_LocalContext(this,myCurLocalIndex,
                                                          UseDisplayedObjects,
                                                          AllowShapeDecomposition,
                                                          AcceptEraseOfTemporary);
  // the AIS_LocalContext bind itself to myLocalContexts
  // because procedures performed in AIS_LocalContext constructor
  // already may access myLocalContexts(myCurLocalIndex) (like methods AIS_LocalContext::IsSelected()).

#ifdef OCCT_DEBUG
  cout<<"\tOpen Local Context No "<<myCurLocalIndex<<endl;
  if(UseDisplayedObjects){
    cout<<"\t\tObjects from Neutral Point loaded"<<endl;
    if(AllowShapeDecomposition)
      cout<<"\t\tDecomposition Authorized for Loaded Shapes"<<endl;
    else
      cout<<"\t\tNo Decomposition Authorized for Loaded Shapes"<<endl;
  }
  else
    cout<<"\t\tNo Objects Were Loaded "<<endl;
#endif
  return myCurLocalIndex;
}

//=======================================================================
//function : CloseLocalContext
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::CloseLocalContext(const Standard_Integer Index,
                                               const Standard_Boolean updateviewer)
{

 Standard_Boolean debugmode(Standard_False);
#ifdef OCCT_DEBUG
 debugmode = Standard_True;
#endif
 
 Standard_Integer GoodIndex = (Index ==-1) ? myCurLocalIndex : Index;

 if(debugmode) cout<<"Call to CloseLocalContext - Index  "<<GoodIndex<<endl;
 if(!HasOpenedContext()){
   if(debugmode) cout<<"\t But No Local Context is opened"<<endl;
   return;
 }
 if(!myLocalContexts.IsBound(GoodIndex)) {
   if(debugmode) cout<<" Attempt to Close a non-existent Local Context"<<endl;
   return;
 }
 
 // the only open local context is closed...
 if(myLocalContexts.Extent()==1 && GoodIndex == myCurLocalIndex){
   
   myLocalContexts(myCurLocalIndex)->Terminate( updateviewer );
   myLocalContexts.UnBind(myCurLocalIndex);
   myCurLocalIndex = 0;

   ResetOriginalState(Standard_False);
   if(debugmode)
     cout<<"No More Opened Local Context "<<endl;
 }
 
 // Otherwise the local context will be still open after the current is closed
 else{
   Handle(StdSelect_ViewerSelector3d) VS = myLocalContexts(GoodIndex)->MainSelector();
   myLocalContexts(GoodIndex)->Terminate();
   myLocalContexts.UnBind(GoodIndex);
   // the current is closed...
   if(GoodIndex==myCurLocalIndex){
     myCurLocalIndex = HighestIndex();
   }
   else if(debugmode)
     cout<<"a No Current Local Context WasClosed"<<endl;
   
   if(debugmode) cout<<"Index Of CurrentLocalContext:"<<myCurLocalIndex<<endl;
   
 }
   
 
 if(updateviewer) myMainVwr->Update();
}

//=======================================================================
//function : CloseAllContexts
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::CloseAllContexts(const Standard_Boolean updateviewer)
{
  
  while(!myLocalContexts.IsEmpty()){
    CloseLocalContext(myCurLocalIndex,Standard_False);
  }
  
  ResetOriginalState(Standard_False);

  if(updateviewer) myMainVwr->Update();
}

//=======================================================================
//function : IndexOfCurrentLocal
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveContext::IndexOfCurrentLocal() const
{
  return myCurLocalIndex;
}

//=======================================================================
//function : ClearLocalContext
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ClearLocalContext(const AIS_ClearMode aMode)
{
  if (!HasOpenedContext()) return;
  myLocalContexts(myCurLocalIndex)->Clear(aMode);

}

//=======================================================================
//function : HighestIndex
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveContext::HighestIndex() const
{
  AIS_DataMapIteratorOfDataMapOfILC It(myLocalContexts);
  Standard_Integer HiInd = 0;
  for(;It.More();It.Next())
    HiInd = (It.Key()>HiInd) ? It.Key() : HiInd;
  return HiInd;

}


//=======================================================================
//function : Activate
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::
Activate(const Handle(AIS_InteractiveObject)& anIObj, 
         const Standard_Integer aMode,
         const Standard_Boolean theIsForce)
{
  if(!HasOpenedContext()){
    if(!myObjects.IsBound(anIObj)) return;
    const Handle(AIS_GlobalStatus)& STAT = myObjects(anIObj);
    if(STAT->GraphicStatus()==AIS_DS_Displayed || theIsForce)
      mgrSelector->Activate(anIObj,aMode,myMainSel);
    STAT ->AddSelectionMode(aMode);
  }
  else{
   myLocalContexts(myCurLocalIndex)->ActivateMode(anIObj,aMode);
  }
}

//=======================================================================
//function : LocalSelector
//purpose  : 
//=======================================================================
Handle( StdSelect_ViewerSelector3d ) AIS_InteractiveContext::LocalSelector() const
{
  if( !HasOpenedContext() )
      return Handle( StdSelect_ViewerSelector3d )();
  else
      return myLocalContexts( myCurLocalIndex )->MainSelector();
}


//=======================================================================
//function : DeActivate
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::
Deactivate(const Handle(AIS_InteractiveObject)& anIObj)
{
  if(!HasOpenedContext()){
    if(!myObjects.IsBound(anIObj)) return;
    TColStd_ListIteratorOfListOfInteger ItL;
    for(ItL.Initialize(myObjects(anIObj)->SelectionModes());
        ItL.More();
        ItL.Next()){
      if(myObjects(anIObj)->GraphicStatus() == AIS_DS_Displayed)
        mgrSelector->Deactivate(anIObj,ItL.Value(),myMainSel);
    }
    myObjects(anIObj)->ClearSelectionModes();
  }
  else{
    const Handle(AIS_LocalContext)& LC = myLocalContexts(myCurLocalIndex);
    LC->Deactivate(anIObj);
  }
}

//=======================================================================
//function : Deactivate
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Deactivate(const Handle(AIS_InteractiveObject)& anIObj, 
           const Standard_Integer aMode)
{
  if(!HasOpenedContext()){
    if(!myObjects.IsBound(anIObj)) return;
    const Handle(AIS_GlobalStatus)& STAT = myObjects(anIObj);

    if(STAT->GraphicStatus() == AIS_DS_Displayed)
      mgrSelector->Deactivate(anIObj,aMode,myMainSel);
    STAT->RemoveSelectionMode(aMode);
  }
  else{
   myLocalContexts(myCurLocalIndex)->DeactivateMode(anIObj,aMode);
  }
}

//=======================================================================
//function : ActivatedModes
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::
ActivatedModes(const Handle(AIS_InteractiveObject)& anIObj, 
               TColStd_ListOfInteger& theList) const 
{
  TColStd_ListIteratorOfListOfInteger ItL;
  if(!HasOpenedContext()){
    if(myObjects.IsBound(anIObj)){
      for(ItL.Initialize(myObjects(anIObj)->SelectionModes());
          ItL.More();
          ItL.Next())
        theList.Append(ItL.Value());
      
    }
  }
  else{
    if(myLocalContexts(myCurLocalIndex)->IsIn(anIObj)){
      for(ItL.Initialize(myLocalContexts(myCurLocalIndex)->SelectionModes(anIObj));
          ItL.More();
          ItL.Next())
        theList.Append(ItL.Value());
    }
  }
}

//=======================================================================
//function : SetShapeDecomposition
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetShapeDecomposition(const Handle(AIS_InteractiveObject)& anIObj,
                                                   const Standard_Boolean StdModeSensitive)
{
  if(!HasOpenedContext()) return;
  myLocalContexts(myCurLocalIndex)->SetShapeDecomposition(anIObj,StdModeSensitive);
}

//=======================================================================
//function : SetTemporaryAttributes
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::
SetTemporaryAttributes(const Handle(AIS_InteractiveObject)& /*anObj*/,
                       const Handle(Prs3d_Drawer)& /*aDrawer*/,
                       const Standard_Boolean /*updateviewer*/)
{
}

//=======================================================================
//function : SubIntensityOn
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::
SubIntensityOn(const Handle(AIS_InteractiveObject)& anIObj,
               const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext()){
    if(!myObjects.IsBound(anIObj))
      return;
    const Handle(AIS_GlobalStatus)& GB=myObjects(anIObj);
    if(GB->IsSubIntensityOn())
      return;
    GB->SubIntensityOn();
    Standard_Boolean UpdMain(Standard_False);
    
    for(TColStd_ListIteratorOfListOfInteger It(GB->DisplayedModes());It.More();It.Next()){
      if (GB->GraphicStatus()==AIS_DS_Displayed)
      {
        myMainPM->Color(anIObj,mySubIntensity,It.Value());
        UpdMain = Standard_True;
      }
    }
    if(updateviewer){
      if(UpdMain)
        myMainVwr->Update();
    }
  }
  else {
    if(myObjects.IsBound(anIObj)){
      const Handle(AIS_GlobalStatus)& STAT = myObjects(anIObj);
      STAT->SubIntensityOn();
      TColStd_ListIteratorOfListOfInteger ItL;
      for (ItL.Initialize(STAT->DisplayedModes());ItL.More();ItL.Next())
        myMainPM->Color(anIObj,mySubIntensity,ItL.Value());
    }
    else
      myLocalContexts(myCurLocalIndex)->SubIntensityOn(anIObj);
    
    if(updateviewer) myMainVwr->Update();
  }
}
//=======================================================================
//function : SubIntensityOff
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::
SubIntensityOff(const Handle(AIS_InteractiveObject)& anIObj,
                const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext()){
    if(!myObjects.IsBound(anIObj))
      return;
    const Handle(AIS_GlobalStatus)& GB=myObjects(anIObj);
    if(!GB->IsSubIntensityOn())
      return;
    GB->SubIntensityOff();
    Standard_Boolean UpdMain(Standard_False);
    
    for(TColStd_ListIteratorOfListOfInteger It(GB->DisplayedModes());It.More();It.Next()){
      if(GB->GraphicStatus()==AIS_DS_Displayed)
      {
        myMainPM->Unhighlight(anIObj,It.Value());
        UpdMain = Standard_True;
      }
    }
    
    Standard_Integer DM,HM,SM;
    GetDefModes(anIObj,DM,HM,SM);
    if(AIS_Selection::IsSelected(anIObj))
      myMainPM->Highlight(anIObj,HM);
    
    if(updateviewer){
      if(UpdMain)
        myMainVwr->Update();
    }
  }
  else {
    if(myObjects.IsBound(anIObj)){
      const Handle(AIS_GlobalStatus)& STAT = myObjects(anIObj);
      STAT->SubIntensityOff();
      TColStd_ListIteratorOfListOfInteger ItL;
      for (ItL.Initialize(STAT->DisplayedModes());ItL.More();ItL.Next())
        myMainPM->Unhighlight(anIObj,ItL.Value());
      if(STAT->IsHilighted())
        Hilight(anIObj);
    }
    else
      myLocalContexts(myCurLocalIndex)->SubIntensityOff(anIObj);
    if(IsSelected(anIObj))
      Hilight(anIObj);
    
    if(updateviewer) myMainVwr->Update();
  }
}

//=======================================================================
//function : SubIntensityOn
//purpose  : ALL THE DISPLAYED OBJECTS HAVE SUBINTENSITY...
//=======================================================================

void AIS_InteractiveContext::SubIntensityOn(const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext()) return;
  
  AIS_DataMapIteratorOfDataMapOfIOStatus It (myObjects);
  TColStd_ListIteratorOfListOfInteger ItM;
  for(;It.More();It.Next()){
    const Handle(AIS_GlobalStatus)& STAT = It.Value();
    if(STAT->GraphicStatus()==AIS_DS_Displayed)
      {
        STAT->SubIntensityOn();
        for(ItM.Initialize(STAT->DisplayedModes());ItM.More();ItM.Next())
          {myMainPM->Color(It.Key(),mySubIntensity,ItM.Value());}
      }
  }
  if(updateviewer) myMainVwr->Update();
}

//=======================================================================
//function : SubIntensityOff
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::SubIntensityOff(const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext()) return;

  AIS_DataMapIteratorOfDataMapOfIOStatus It (myObjects);
  TColStd_ListIteratorOfListOfInteger ItL;
  for(;It.More();It.Next()){
    const Handle(AIS_GlobalStatus)& STAT = It.Value();
    if(STAT->IsSubIntensityOn())
      STAT->SubIntensityOff();
    for(ItL.Initialize(STAT->DisplayedModes());ItL.More();ItL.Next())
      myMainPM->Unhighlight(It.Key());
  }

  if(updateviewer) myMainVwr->Update();
}

//=======================================================================
//function : AddFilter
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::AddFilter(const Handle(SelectMgr_Filter)& aFilter)
{
  if(HasOpenedContext())
    myLocalContexts(myCurLocalIndex)->AddFilter(aFilter);
  else
    myFilters->Add(aFilter);
}

//=======================================================================
//function : ActivateStandardMode
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::ActivateStandardMode(const TopAbs_ShapeEnum aStandardActivation)
{
  if(!HasOpenedContext()) return;
  myLocalContexts(myCurLocalIndex)->ActivateStandardMode (aStandardActivation);
}

//=======================================================================
//function : DeActivateStandardMode
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::DeactivateStandardMode(const TopAbs_ShapeEnum aStandardActivation)
{
  if(!HasOpenedContext()) return;
  myLocalContexts(myCurLocalIndex)->DeactivateStandardMode (aStandardActivation);
}

//=======================================================================
//function : RemoveFilter
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::RemoveFilter(const Handle(SelectMgr_Filter)& aFilter)
{
  if(HasOpenedContext())
    myLocalContexts(myCurLocalIndex)->RemoveFilter (aFilter);
  else
    myFilters->Remove(aFilter);
}

//=======================================================================
//function : RemoveFilters
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::RemoveFilters()
{
  if(!HasOpenedContext())
    myFilters->Clear();
  else
    myLocalContexts(myCurLocalIndex)->Clear(AIS_CM_Filters);
}

//=======================================================================
//function : ActivatedStandardModes
//purpose  : 
//=======================================================================
const TColStd_ListOfInteger& AIS_InteractiveContext::ActivatedStandardModes() const 
{
  return myLocalContexts(myCurLocalIndex)->StandardModes();
}

//=======================================================================
//function : Filters
//purpose  : 
//=======================================================================
const SelectMgr_ListOfFilter& AIS_InteractiveContext::Filters() const 
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->ListOfFilter();
  return myFilters->StoredFilters();
}

//=======================================================================
//function : DisplayActiveSensitive
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::DisplayActiveSensitive(const Handle(V3d_View)& aviou)
{
  if(HasOpenedContext())
    myLocalContexts(myCurLocalIndex)->DisplaySensitive(aviou);
  else
    myMainSel->DisplaySensitive(aviou);
}
//=======================================================================
//function : DisplayActiveSensitive
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::DisplayActiveSensitive(const Handle(AIS_InteractiveObject)& anIObj,
                                                    const Handle(V3d_View)& aviou)
{
  TColStd_ListIteratorOfListOfInteger It;
  Handle(StdSelect_ViewerSelector3d) VS;
  if(HasOpenedContext()){
    const Handle(AIS_LocalContext)& LC = myLocalContexts(myCurLocalIndex);
    if(!LC->IsIn(anIObj)) return;
    It.Initialize(LC->SelectionModes(anIObj));
    VS = LC->MainSelector();
  }
  else{
    if(!myObjects.IsBound(anIObj)) return;
    It.Initialize(myObjects(anIObj)->SelectionModes());
    VS = myMainSel;
  }
  
  
  for(;It.More();It.Next()){
    const Handle(SelectMgr_Selection)& Sel = anIObj->Selection(It.Value());
    VS->DisplaySensitive(Sel,anIObj->Transformation(), aviou,Standard_False);
  }  
  
}

//=======================================================================
//function : ClearActiveSensitive
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::ClearActiveSensitive(const Handle(V3d_View)& aviou)
{
  
  if(HasOpenedContext())
    myLocalContexts(myCurLocalIndex)->ClearSensitive(aviou);
  else
    myMainSel->ClearSensitive(aviou);
}

//=======================================================================
//function : SetAutomaticHilight
//purpose  : 
//=======================================================================
void  AIS_InteractiveContext::SetAutomaticHilight(const Standard_Boolean aStatus)
{

  if(HasOpenedContext())
    myLocalContexts(myCurLocalIndex)->SetAutomaticHilight(aStatus);
}

//=======================================================================
//function : AutomaticHilight
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveContext::AutomaticHilight() const 
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->AutomaticHilight();
  return Standard_True;
}

//=======================================================================
//function : UseDisplayedObjects
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UseDisplayedObjects()
{
  if(HasOpenedContext())
    myLocalContexts(myCurLocalIndex)->LoadContextObjects();
}

//=======================================================================
//function : NotUseDisplayedObjects
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::NotUseDisplayedObjects()
{
  if(HasOpenedContext())
    myLocalContexts(myCurLocalIndex)->UnloadContextObjects();
}




//=======================================================================
//function : PurgeDisplay
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveContext::PurgeDisplay()
{
  if(HasOpenedContext()) return 0;
  
  Standard_Integer NbStr = PurgeViewer(myMainVwr);
  myMainVwr->Update();
  return NbStr;

}


//=======================================================================
//function : PurgeViewer
//purpose  : 
//=======================================================================
Standard_Integer AIS_InteractiveContext::PurgeViewer(const Handle(V3d_Viewer)& Vwr)
{
  Handle(Graphic3d_StructureManager) GSM = Vwr->StructureManager();
  Standard_Integer NbCleared(0);
  Graphic3d_MapOfStructure SOS;
  GSM->DisplayedStructures(SOS);

  Handle(Graphic3d_Structure) G;
  for(Graphic3d_MapIteratorOfMapOfStructure It(SOS); It.More();It.Next()){
    G = It.Key();
    Standard_Address Add = G->Owner();
    if(Add==NULL){
      G->Erase();
      G->Clear();// it means that it is not referenced as a presentation of InterfactiveObject...
      NbCleared++;
    }
    Handle(AIS_InteractiveObject) IO = (AIS_InteractiveObject*)Add;
    if(!myObjects.IsBound(IO)){
      G->Erase();
      NbCleared++;
    }
  }
  return NbCleared;
}

//=======================================================================
//function : IsImmediateModeOn
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::IsImmediateModeOn()  const 
{
  if(!HasOpenedContext()) return Standard_False;
  return myLocalContexts(myCurLocalIndex)->IsImmediateModeOn();
}

//=======================================================================
//function : BeginImmediateDraw
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::BeginImmediateDraw()
{
  return HasOpenedContext()
      && myLocalContexts (myCurLocalIndex)->BeginImmediateDraw();
}

//=======================================================================
//function : ImmediateAdd
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::ImmediateAdd (const Handle(AIS_InteractiveObject)& theObj,
                                                       const Standard_Integer               theMode)
{
  return HasOpenedContext()
      && myLocalContexts (myCurLocalIndex)->ImmediateAdd (theObj, theMode);
}

//=======================================================================
//function : EndImmediateDraw
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::EndImmediateDraw (const Handle(V3d_View)& theView)
{
  return HasOpenedContext()
      && myLocalContexts (myCurLocalIndex)->EndImmediateDraw (theView->Viewer());
}

//=======================================================================
//function : EndImmediateDraw
//purpose  :
//=======================================================================

Standard_Boolean AIS_InteractiveContext::EndImmediateDraw()
{
  if (!HasOpenedContext())
  {
    return Standard_False;
  }

  myMainVwr->InitActiveViews();
  if (!myMainVwr->MoreActiveViews())
  {
    return Standard_False;
  }

  Handle(V3d_View) aView = myMainVwr->ActiveView();
  return myLocalContexts (myCurLocalIndex)->EndImmediateDraw (aView->Viewer());
}


//=======================================================================
//function : ResetOriginalState
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ResetOriginalState(const Standard_Boolean updateviewer)
{
  Standard_Boolean upd_main(Standard_False);
  TColStd_ListIteratorOfListOfInteger itl;
  myMainSel->ResetSelectionActivationStatus();

  for (AIS_DataMapIteratorOfDataMapOfIOStatus it(myObjects);it.More();it.Next()){
    const Handle(AIS_InteractiveObject)& iobj = it.Key();
    const Handle(AIS_GlobalStatus)& STAT = it.Value();
    switch(STAT->GraphicStatus()){
    case AIS_DS_Displayed:{
      upd_main = Standard_True;
      
      // part display...
      for(itl.Initialize(STAT->DisplayedModes());itl.More();itl.Next())
        myMainPM->Display(iobj,itl.Value());
      if(STAT->IsHilighted()){
        if(STAT->HilightColor()!=Quantity_NOC_WHITE)
          HilightWithColor(iobj,STAT->HilightColor(),Standard_False);
        else
          Hilight(iobj,Standard_False);
      }
      //part selection
      for(itl.Initialize(STAT->SelectionModes());itl.More();itl.Next()){
        if(itl.Value()!=-1)
          mgrSelector->Activate(iobj,itl.Value(),myMainSel);
      }
      break; 
    }
    case AIS_DS_Erased:{
      EraseGlobal(iobj,Standard_False);
      break;
    }
    default:
      break;
    }
  }
  if(updateviewer){
    if(upd_main) 
      myMainVwr->Update();
  }
}

//=======================================================================
//function : SetZDetection
//purpose  : 
//=======================================================================
void  AIS_InteractiveContext::SetZDetection(const Standard_Boolean aStatus)
{
  myZDetectionFlag = aStatus;
}

//=======================================================================
//function : ZDetection 
//purpose  : 
//=======================================================================
Standard_Boolean AIS_InteractiveContext::ZDetection() const 
{
  return myZDetectionFlag;
}
