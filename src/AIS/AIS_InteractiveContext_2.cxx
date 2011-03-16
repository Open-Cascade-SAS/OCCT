// File:        AIS_InteractiveContext_2.cxx
// Created:     Wed Jan 29 10:55:05 1997
// Author:      Robert COUBLANC
//              <rob@robox.paris1.matra-dtv.fr>

#define IMP051001       //GG Adds SetZDetected() and ZDetected() methods

#include <AIS_InteractiveContext.jxx>

#include <AIS_LocalContext.hxx>

#include <AIS_GlobalStatus.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

#include <AIS_DataMapIteratorOfDataMapOfIOStatus.hxx>
#include <AIS_DataMapIteratorOfDataMapOfILC.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_MapOfStructure.hxx>
#include <Graphic3d_MapIteratorOfMapOfStructure.hxx>
#include <AIS_Selection.hxx>


static TColStd_ListIteratorOfListOfInteger ItL;
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

  // on dehilighte les eventuelles entitees detectees juste avant l'ouverture
  // du contexte...
  if(!IsCurrent(myLastPicked)){
    if(!myLastPicked.IsNull()){
      Standard_Integer HiMod = myLastPicked->HasHilightMode()?myLastPicked->HilightMode():0;
      myMainPM->Unhighlight(myLastPicked,HiMod);
    }}
  
  if(!mylastmoveview.IsNull()){
    if(myCurLocalIndex>0)
      myLocalContexts(myCurLocalIndex)->UnhilightLastDetected(mylastmoveview);
  }
  
  // on met a 0 les entites liees a la selection dynamique au point neutre.
  
  myLastinMain.Nullify();
  myLastinColl.Nullify();
  myLastPicked.Nullify();
  myWasLastMain = Standard_True;



  Standard_Integer untilnow = myCurLocalIndex;
  
  myCurLocalIndex = HighestIndex() + 1;
  
  Handle(AIS_LocalContext) NewLocal= new AIS_LocalContext(this,myCurLocalIndex,
                                                          UseDisplayedObjects,
                                                          AllowShapeDecomposition,
                                                          AcceptEraseOfTemporary);
  
  // rob 16/04/97 pour les Pbs d'ordres asynchrones
  if(myLocalContexts.Extent()>0){
    const Select3D_Projector& Prj = myLocalContexts(untilnow)->MainSelector()->Projector();
    NewLocal->MainSelector()->Set(Prj);
  }
  else{
    const Select3D_Projector& Prj = myMainSel->Projector();
    NewLocal->MainSelector()->Set(Prj);
  }
  
  NewLocal->MainSelector()->UpdateConversion();
  
  myLocalContexts.Bind(myCurLocalIndex,NewLocal);


#ifdef DEB
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
#ifdef DEB 
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
 
 // On va fermer le seul contexte local ouvert...
 if(myLocalContexts.Extent()==1 && GoodIndex == myCurLocalIndex){
   
   Standard_Boolean updateproj = !(myLocalContexts(myCurLocalIndex)->HasSameProjector(myMainSel->Projector()));
   myLocalContexts(myCurLocalIndex)->Terminate( updateviewer );
   myLocalContexts.UnBind(myCurLocalIndex);
   myCurLocalIndex = 0;

   ResetOriginalState(Standard_False);
   if(updateproj)
     myMainSel->UpdateConversion();
   else{
     myMainSel->ReactivateProjector();
     myMainSel->UpdateSort();
   }
   if(debugmode)
     cout<<"No More Opened Local Context "<<endl;
 }
 
 // Sinon on aura encore un contexte local d'ouvert apres la fermeture du courant
 else{
   Handle(StdSelect_ViewerSelector3d) VS = myLocalContexts(GoodIndex)->MainSelector();
   myLocalContexts(GoodIndex)->Terminate();
   myLocalContexts.UnBind(GoodIndex);
   // on ferme le courant...
   if(GoodIndex==myCurLocalIndex){
     myCurLocalIndex = HighestIndex();
     const Handle(AIS_LocalContext)& LocCtx = myLocalContexts(myCurLocalIndex);
     if(LocCtx->HasSameProjector(VS->Projector())){
       LocCtx->MainSelector()->ReactivateProjector();
     }
     else
       LocCtx->MainSelector()->UpdateConversion();
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

  myMainSel->UpdateSort();
  if(!myIsCollClosed && !myCollectorSel.IsNull())
    myCollectorSel->UpdateSort();
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
         const Standard_Integer aMode)
{
  if(!HasOpenedContext()){
    if(!myObjects.IsBound(anIObj)) return;
    const Handle(AIS_GlobalStatus)& STAT = myObjects(anIObj);
    if(STAT->GraphicStatus()==AIS_DS_Displayed)
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
  if(!HasOpenedContext()){
    if(myObjects.IsBound(anIObj)){
      //ItL est une variable statique... 
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
    Standard_Boolean UpdMain(Standard_False),UpdColl(Standard_False);
    
    for(TColStd_ListIteratorOfListOfInteger It(GB->DisplayedModes());It.More();It.Next()){
      if(GB->GraphicStatus()==AIS_DS_Displayed){
        myMainPM->Color(anIObj,mySubIntensity,It.Value());
        UpdMain = Standard_True;}
      else if(GB->GraphicStatus()==AIS_DS_Erased){
        myCollectorPM->Color(anIObj,mySubIntensity,It.Value());
        UpdColl=Standard_True;
      }
    }
    if(updateviewer){
      if(UpdMain)
        myMainVwr->Update();
      if(UpdColl)
        myCollectorVwr->Update();
    }
  }
  else {
    if(myObjects.IsBound(anIObj)){
      const Handle(AIS_GlobalStatus)& STAT = myObjects(anIObj);
      STAT->SubIntensityOn();
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
    Standard_Boolean UpdMain(Standard_False),UpdColl(Standard_False);
    
    for(TColStd_ListIteratorOfListOfInteger It(GB->DisplayedModes());It.More();It.Next()){
      if(GB->GraphicStatus()!=AIS_DS_Erased){
        myMainPM->Unhighlight(anIObj,It.Value());
        UpdMain = Standard_True;}
      else {
        myCollectorPM->Unhighlight(anIObj,It.Value());
        UpdColl=Standard_True;
      }
    }
    
    Standard_Integer DM,HM,SM;
    GetDefModes(anIObj,DM,HM,SM);
    if(AIS_Selection::IsSelected(anIObj))
      myMainPM->Highlight(anIObj,HM);
    
    if(updateviewer){
      if(UpdMain)
        myMainVwr->Update();
      if(UpdColl)
        myCollectorVwr->Update();
    }
  }
  else {
    if(myObjects.IsBound(anIObj)){
      const Handle(AIS_GlobalStatus)& STAT = myObjects(anIObj);
      STAT->SubIntensityOff();
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
//function : DisplayActiveAreas
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::DisplayActiveAreas(const Handle(V3d_View)& aviou)
{
  if(HasOpenedContext())
    myLocalContexts(myCurLocalIndex)->DisplayAreas(aviou);
  else
    myMainSel->DisplayAreas(aviou);
  
}

//=======================================================================
//function : ClearActiveAreas
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::ClearActiveAreas(const Handle(V3d_View)& aviou)
{
  if(HasOpenedContext())
    myLocalContexts(myCurLocalIndex)->ClearAreas(aviou);
  else
    myMainSel->ClearAreas(aviou);
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
    VS->DisplaySensitive(Sel,aviou,Standard_False);
  }  
  
}

//=======================================================================
//function : DisplayActiveAreas
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::DisplayActiveAreas(const Handle(AIS_InteractiveObject)& anIObj,
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
    VS->DisplayAreas(Sel,aviou,Standard_False);
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

Standard_Integer AIS_InteractiveContext::PurgeDisplay(const Standard_Boolean CollectorToo)
{
  if(HasOpenedContext()) return 0;
  
  Standard_Integer NbStr = PurgeViewer(myMainVwr);
  if(!myCollectorVwr.IsNull())
    if(CollectorToo){
      NbStr+=PurgeViewer(myCollectorVwr);
      if(!IsCollectorClosed())
        myCollectorVwr->Update();
    }
  myMainVwr->Update();
  return NbStr;

}


//=======================================================================
//function : PurgeViewer
//purpose  : 
//=======================================================================
Standard_Integer AIS_InteractiveContext::PurgeViewer(const Handle(V3d_Viewer)& Vwr)
{
  const Handle(Graphic3d_StructureManager)& GSM = Vwr->Viewer();
  Standard_Integer NbCleared(0);
  Graphic3d_MapOfStructure SOS;
  GSM->DisplayedStructures(SOS);

  Handle(Graphic3d_Structure) G;
  for(Graphic3d_MapIteratorOfMapOfStructure It(SOS); It.More();It.Next()){
    G = It.Key();
    Standard_Address Add = G->Owner();
    if(Add==NULL){
      G->Erase();
      G->Clear();// ca veut dire qu'elle n'est pas reference comme une presentation d'un InterfactiveObject...
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

Standard_Boolean  AIS_InteractiveContext::BeginImmediateDraw()
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->BeginImmediateDraw();
  return Standard_False;
}

//=======================================================================
//function : ImmediateAdd
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::ImmediateAdd(const Handle(AIS_InteractiveObject)& anIObj,
                                                      const Standard_Integer AMode)
{ 
  if(HasOpenedContext()){
    return myLocalContexts(myCurLocalIndex)->ImmediateAdd(anIObj,AMode);}
  return Standard_False;
}

//=======================================================================
//function : ImmediateRemove
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::ImmediateRemove(const Handle(AIS_InteractiveObject)& anIObj,
                                                         const Standard_Integer aMode)
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->ImmediateRemove(anIObj,aMode);
  return Standard_False;
}

//=======================================================================
//function : EndImmediateDraw
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::EndImmediateDraw(const Handle(V3d_View)& aView,
                                                          const Standard_Boolean DoubleBuf)
{
  if(HasOpenedContext())
    return myLocalContexts(myCurLocalIndex)->EndImmediateDraw(aView,DoubleBuf);
  return Standard_False;
  
}

//=======================================================================
//function : EndImmediateDraw
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::EndImmediateDraw(const Standard_Boolean DoubleBuf)
{
  if(HasOpenedContext()){
    Handle(V3d_View) V ;
    myMainVwr->InitActiveViews();
    if(myMainVwr->MoreActiveViews()){
      V = myMainVwr->ActiveView();
      
      return myLocalContexts(myCurLocalIndex)->EndImmediateDraw(V,DoubleBuf);
    }
  }
  return Standard_False;
  
}


//=======================================================================
//function : ResetOriginalState
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ResetOriginalState(const Standard_Boolean updateviewer)
{
  Standard_Boolean upd_main(Standard_False),upd_col(Standard_False);
  TColStd_ListIteratorOfListOfInteger itl;
  
  for (AIS_DataMapIteratorOfDataMapOfIOStatus it(myObjects);it.More();it.Next()){
    const Handle(AIS_InteractiveObject)& iobj = it.Key();
    const Handle(AIS_GlobalStatus)& STAT = it.Value();
    switch(STAT->GraphicStatus()){
    case AIS_DS_Displayed:{
      upd_main = Standard_True;
      
      // partie display...
      for(itl.Initialize(STAT->DisplayedModes());itl.More();itl.Next())
        myMainPM->Display(iobj,itl.Value());
      if(STAT->IsHilighted()){
        if(STAT->HilightColor()!=Quantity_NOC_WHITE)
          HilightWithColor(iobj,STAT->HilightColor(),Standard_False);
        else
          Hilight(iobj,Standard_False);
      }
      //partie selection
      for(itl.Initialize(STAT->SelectionModes());itl.More();itl.Next()){
        if(itl.Value()!=-1)
          mgrSelector->Activate(iobj,itl.Value(),myMainSel);
      }
      break; 
    }
    case AIS_DS_Erased:{
      upd_col = Standard_True;
      EraseGlobal(iobj,Standard_False,Standard_True);
      break;
    }
    case AIS_DS_FullErased:{
      EraseGlobal(iobj,Standard_False,Standard_False);
      break;
    }
    default:
      break;
    }
  }
  if(updateviewer){
    if(upd_main) 
      myMainVwr->Update();
    if(upd_col)
      myCollectorVwr->Update();
  }
}

#ifdef IMP051001
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
#endif
