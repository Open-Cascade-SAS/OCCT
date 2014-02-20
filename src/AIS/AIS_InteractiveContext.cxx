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

#define BUC60577        //GG_101099     Enable to compute correctly
//                      transparency with more than one object in the view.

#define BUC60632        //GG 15/03/00 Add protection on SetDisplayMode()
//                      method, compute only authorized presentation.

#define BUC60688        //GG 25/05/00 Add SetSensitivity() methods.

#define BUC60722  //GG 04/09/00 Always enable viewer update when erasing something

#define IMP051001       //GG Adds SetZDetected() and ZDetected() methods

#define OCC172          //SAV clear static map before destroying context.

#define OCC204          //SAV 26/02/02 : pass <updateviewer> flag to 
// AddOrRemoveCurrentObject method from ClearGlobal.

#define OCC4373         //SAN 10/11/03 : improve display mode management in
// Display( IO, updateviewer ) and 
// SetDisplayMode( IO, mode, updateviewer ) methods

#include <AIS_InteractiveContext.ixx>

//#include <AIS_DataMapIteratorOfDataMapOfInteractiveInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <AIS_LocalContext.hxx>
#include <AIS_LocalStatus.hxx>
#include <Precision.hxx>
#include <AIS_Selection.hxx>
#include <AIS_DataMapIteratorOfDataMapOfIOStatus.hxx>
#include <AIS_ConnectedShape.hxx>
#include <AIS_MultipleConnectedShape.hxx>
#include <AIS_DataMapIteratorOfDataMapOfILC.hxx>
#include <AIS_GlobalStatus.hxx>
#include <AIS_MapIteratorOfMapOfInteractive.hxx>
#include <PrsMgr_ModedPresentation.hxx>
#include <Visual3d_ViewManager.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <AIS_Shape.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <HLRBRep.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <Standard_Atomic.hxx>
#include <UnitsAPI.hxx>

#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>
#include <OSD_Environment.hxx>

#include <AIS_ListIteratorOfListOfInteractive.hxx>

// In the method GetDefModes() the returned value of the selection mode 
// is always equal to 0 if it is -1.
#define BUC61051

// The local context is closed the method ::ResetOriginalState() sets the selection mode equal to 0
// in spite of the selection mode of the interactive object in Natural Point.
#define OCC166

static Standard_Boolean AISDebugModeOn()
{
//  static OSD_Environment aisdb("AISDEBUGMODE");
//  return !aisdb.Value().IsEmpty();
  static Standard_Integer isDebugMode(-1);
  if (isDebugMode < 0) {
    isDebugMode = 1;
    OSD_Environment aisdb("AISDEBUGMODE");
    if (aisdb.Value().IsEmpty())
      isDebugMode = 0;
  }
  return (isDebugMode != 0);
}

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
};

//=======================================================================
//function : AIS_InteractiveContext
//purpose  : 
//=======================================================================

AIS_InteractiveContext::AIS_InteractiveContext(const Handle(V3d_Viewer)& MainViewer):
mgrSelector(new SelectMgr_SelectionManager()),
myMainPM(new PrsMgr_PresentationManager3d(MainViewer->Viewer())),
myMainVwr(MainViewer),
myMainSel(new StdSelect_ViewerSelector3d()),
myToHilightSelected( Standard_False ),
myFilters(new SelectMgr_OrFilter()),
myDefaultDrawer(new Prs3d_Drawer()),
myDefaultColor(Quantity_NOC_GOLDENROD),
myHilightColor(Quantity_NOC_CYAN1),
mySelectionColor(Quantity_NOC_GRAY80),
myPreselectionColor(Quantity_NOC_GREEN),
mySubIntensity(Quantity_NOC_GRAY40),
myDisplayMode(0),
myCurLocalIndex(0),
#ifdef IMP051001
myZDetectionFlag(0),
#endif
myIsAutoActivateSelMode( Standard_True )
{ 
  InitAttributes();
}

void AIS_InteractiveContext::Delete() const
{
  //Clear the static current selection. Else the memory
  //is not release
  AIS_Selection::ClearCurrentSelection();
#ifdef OCC172
  // to avoid an exception
  if ( AIS_Selection::Find( mySelectionName.ToCString() ) )
    AIS_Selection::Remove( mySelectionName.ToCString() );

  // to avoid an exception
  if ( AIS_Selection::Find( myCurrentName.ToCString() ) )
    AIS_Selection::Remove( myCurrentName.ToCString() );

  // let's remove one reference explicitly. this operation's supposed to
  // be performed when mgrSelector will be destroyed but anyway...
  mgrSelector->Remove( myMainSel );
#endif
  AIS_ListOfInteractive aList;
  
  AIS_DataMapIteratorOfDataMapOfIOStatus anIt(myObjects);
  Handle(AIS_InteractiveContext) aNullContext;
  for(; anIt.More() ; anIt.Next())
  {
    Handle(AIS_InteractiveObject) anObj = anIt.Key();
    anObj->SetContext(aNullContext);
    
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

void AIS_InteractiveContext::DisplayedObjects(AIS_ListOfInteractive& aListOfIO,
                                              const Standard_Boolean OnlyFromNeutral) const 
{
#ifdef DEBUG
  cout<<"AIS_IC::DisplayedObjects"<<endl;
#endif

  AIS_DataMapIteratorOfDataMapOfIOStatus It(myObjects);
  if(!HasOpenedContext() || OnlyFromNeutral){
    for(;It.More();It.Next()){
      if(It.Value()->GraphicStatus()==AIS_DS_Displayed)
        aListOfIO.Append(It.Key());
    }
  }
  else{
    TColStd_MapOfTransient theMap;
    // neutral point
    for(;It.More();It.Next()){
      if(It.Value()->GraphicStatus()==AIS_DS_Displayed)
        theMap.Add(It.Key());
    }
#ifdef DEBUG
    cout<<"\tFrom Neutral Point : "<<theMap.Extent()<<endl;
#endif

    //parse all local contexts...
#ifdef DEBUG
    Standard_Integer NbDisp;
    for(AIS_DataMapIteratorOfDataMapOfILC it1(myLocalContexts);it1.More();it1.Next()){
      const Handle(AIS_LocalContext)& LC = it1.Value();
      NbDisp =  LC->DisplayedObjects(theMap);
      cout<<"\tIn Local Context "<<it1.Key()<<" : "<<NbDisp<<endl;
    }
#endif
    Handle(AIS_InteractiveObject) curIO;
    Handle(Standard_Transient) Tr;
      for(TColStd_MapIteratorOfMapOfTransient it2(theMap);it2.More();it2.Next()){
        Tr = it2.Key();
        curIO = *((Handle(AIS_InteractiveObject)*) &Tr);
        aListOfIO.Append(curIO);
      }
    }
}
//=======================================================================
//function : DisplayedObjects
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::DisplayedObjects(const AIS_KindOfInteractive TheKind,
                                              const Standard_Integer TheSign,
                                              AIS_ListOfInteractive& aListOfIO,
                                              const Standard_Boolean /*OnlyFromNeutral*/) const 
{
  ObjectsByDisplayStatus( TheKind, TheSign, AIS_DS_Displayed, aListOfIO );
}

//=======================================================================
//function : ErasedObjects
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ErasedObjects(AIS_ListOfInteractive& theListOfIO) const 
{
  ObjectsByDisplayStatus( AIS_DS_Erased, theListOfIO );
}

//=======================================================================
//function : ErasedObjects
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ErasedObjects(const AIS_KindOfInteractive TheKind,
                                           const Standard_Integer TheSign,
                                           AIS_ListOfInteractive& theListOfIO) const 
{
  ObjectsByDisplayStatus( TheKind, TheSign, AIS_DS_Erased, theListOfIO );
}

//=======================================================================
//function : ObjectsByDisplayStatus
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ObjectsByDisplayStatus(const AIS_DisplayStatus theStatus,
                                                    AIS_ListOfInteractive&  theListOfIO) const 
{
  AIS_DataMapIteratorOfDataMapOfIOStatus It(myObjects);
  for(;It.More();It.Next()){
    if(It.Value()->GraphicStatus() == theStatus)
      theListOfIO.Append(It.Key());
  }
}

//=======================================================================
//function : ObjectsByDisplayStatus
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ObjectsByDisplayStatus(const AIS_KindOfInteractive TheKind,
                                                    const Standard_Integer TheSign,
                                                    const AIS_DisplayStatus theStatus,
                                                    AIS_ListOfInteractive& theListOfIO) const 
{
  AIS_DataMapIteratorOfDataMapOfIOStatus It(myObjects);
  for(;It.More();It.Next()){
    if(It.Value()->GraphicStatus()==theStatus){
      if(It.Key()->Type()==TheKind){
        if(TheSign ==-1)
          theListOfIO.Append(It.Key());
        else{
          if(It.Key()->Signature()==TheSign)
            theListOfIO.Append(It.Key());
        }
      }
    }
  }
}

//=======================================================================
//function : ObjectsInside
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ObjectsInside(AIS_ListOfInteractive& aListOfIO,
                                           const AIS_KindOfInteractive TheKind,
                                           const Standard_Integer TheSign) const 
{
  AIS_DataMapIteratorOfDataMapOfIOStatus It(myObjects);
  if(TheKind==AIS_KOI_None && TheSign ==-1){
    for(;It.More();It.Next()){
        aListOfIO.Append(It.Key());
    }
  }
  else{
    for(;It.More();It.Next()){
      if(It.Key()->Type()==TheKind){
        if(TheSign ==-1)
          aListOfIO.Append(It.Key());
        else{
          if(It.Key()->Signature()==TheSign)
            aListOfIO.Append(It.Key());
        }
      }
    }
  }
}

//=======================================================================
//function : Display
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Display(const Handle(AIS_InteractiveObject)& anIObj,
                                     const Standard_Boolean updateviewer )
{
  if(anIObj.IsNull()) return;
  
  Standard_Integer DispMode,HiMod,SelMode;
  
  GetDefModes(anIObj,DispMode,HiMod,SelMode);
  
  Handle (AIS_InteractiveContext) aThis = this;
  if(!anIObj->HasInteractiveContext()) 
    anIObj->SetContext(aThis);
  
  //NO LOCAL CONTEXT OPEN
  if(!HasOpenedContext()) {
#ifndef OCC4373
    // SAN : Do not return here. Perform advanced display mode analysis a bit later...
    if(IsDisplayed(anIObj)) return;
#endif

    // it did not yet exist
    if(!myObjects.IsBound(anIObj)){
      
      Handle(AIS_GlobalStatus) STATUS= 
#ifdef OCC166
        new AIS_GlobalStatus(AIS_DS_Displayed,DispMode,SelMode);
#else
      new AIS_GlobalStatus(AIS_DS_Displayed,DispMode,0);
#endif
      myObjects.Bind (anIObj,STATUS);
      myMainPM->Display(anIObj,DispMode);
      if( myIsAutoActivateSelMode )
      {
        if(!mgrSelector->Contains(anIObj))
           mgrSelector->Load(anIObj);
        mgrSelector->Activate(anIObj,SelMode,myMainSel);
      }
        
     if(updateviewer) myMainVwr->Update();
    }
    // it is somewhere else...
    else {
      // CLE
      // const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
      Handle(AIS_GlobalStatus) STATUS = myObjects(anIObj);
      // ENDCLE
      if (STATUS->GraphicStatus() == AIS_DS_Displayed || STATUS->GraphicStatus() == AIS_DS_Erased)
      {
        // SAN : erase presentations for all display modes different from <DispMode>;
        //       then make sure <DispMode> is displayed and maybe highlighted;
        //       Finally, activate selection mode <SelMode> if not yet activated.
        TColStd_ListOfInteger aModesToRemove;
        TColStd_ListIteratorOfListOfInteger ItL (STATUS->DisplayedModes());
        for(;ItL.More();ItL.Next()){

          Standard_Integer OldMode = ItL.Value();

          if(OldMode!=DispMode){
            aModesToRemove.Append(OldMode);
            if(myMainPM->IsHighlighted(anIObj,OldMode))
              myMainPM->Unhighlight(anIObj,OldMode);
            myMainPM->Erase(anIObj,OldMode);
          }
        }

        for(ItL.Initialize(aModesToRemove);ItL.More();ItL.Next())
          STATUS->RemoveDisplayMode(ItL.Value());

        if(!STATUS->IsDModeIn(DispMode))
          STATUS->AddDisplayMode(DispMode);

        myMainPM->Display(anIObj,DispMode);
        if (STATUS->GraphicStatus() == AIS_DS_Erased)
        {
          STATUS->SetGraphicStatus(AIS_DS_Displayed);
        }
        if(STATUS->IsHilighted()){
          myMainPM->Highlight(anIObj,HiMod);
        }
        if( myIsAutoActivateSelMode )
        {
           if(!mgrSelector->Contains(anIObj))
              mgrSelector->Load(anIObj);
           if(!mgrSelector->IsActivated(anIObj,SelMode))
              mgrSelector->Activate(anIObj,SelMode,myMainSel);
        }
      }
    } 
    
    if(anIObj->IsTransparent() && !myMainVwr->Viewer()->Transparency())
      myMainVwr->Viewer()->SetTransparency(Standard_True);
    if(updateviewer) myMainVwr->Update();
  }
  
  //  LOCAL CONTEXT OPEN
  else
  {
    myLocalContexts(myCurLocalIndex)->Display(anIObj,DispMode,anIObj->AcceptShapeDecomposition(),SelMode);
    
    if(anIObj->IsTransparent() && !myMainVwr->Viewer()->Transparency())
      myMainVwr->Viewer()->SetTransparency(Standard_True);
    
    
    if(updateviewer) myMainVwr->Update();
  }
}

//=======================================================================
//function : Display
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Display(const Handle(AIS_InteractiveObject)& anIObj,
                                    const Standard_Integer aDisplayMode, 
                                    const Standard_Integer aSelectionMode, 
                                    const Standard_Boolean updateviewer, 
                                    const Standard_Boolean allowdecomposition)
{
  if(anIObj.IsNull()) return;

  if(!anIObj->HasInteractiveContext()) anIObj->SetContext(this);
  
  // if no local context...
  if(!HasOpenedContext()) {
    //    if(!anIObj->HasDisplayMode())
    //      anIObj->SetDisplayMode(aDisplayMode);
  
    if(!myObjects.IsBound(anIObj)){
      Handle(AIS_GlobalStatus) STATUS= 
        new AIS_GlobalStatus(AIS_DS_Displayed,aDisplayMode,aSelectionMode);
      myObjects.Bind (anIObj,STATUS);
      myMainPM->Display(anIObj,aDisplayMode);
      if(aSelectionMode!=-1){
        if(!mgrSelector->Contains(anIObj))
          mgrSelector->Load(anIObj,aSelectionMode);
        mgrSelector->Activate(anIObj,aSelectionMode,myMainSel);
      }
      if(updateviewer) myMainVwr->Update();
    }
    //    anIObj->SelectionMode(aSelectionMode);
    else{
      // CLE
      // const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
      Handle(AIS_GlobalStatus) STATUS = myObjects(anIObj);
      // ENDCLE
      if(!STATUS->IsDModeIn(aDisplayMode)){
        myMainPM->Display(anIObj,aDisplayMode);
        STATUS->AddDisplayMode(aDisplayMode);
        if(STATUS->IsSubIntensityOn())
          myMainPM->Color(anIObj,mySubIntensity,aDisplayMode);
      }
      if(aSelectionMode!=-1){
        STATUS->AddSelectionMode(aSelectionMode);
        mgrSelector->Activate(anIObj,aSelectionMode,myMainSel);
      }
      if(updateviewer) myMainVwr->Update();
      
    }
  }
  else
    {
      myLocalContexts(myCurLocalIndex)->Display(anIObj,aDisplayMode,allowdecomposition,aSelectionMode);
      if(updateviewer) myMainVwr->Update();
    }  
}


//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Load(const Handle(AIS_InteractiveObject)& anIObj,
                                  const Standard_Integer SelMode,
                                  const Standard_Boolean AllowDecomposition)
{
  if(anIObj.IsNull()) return;
  if(!anIObj->HasInteractiveContext()) anIObj->SetContext(this);
  

  if(!HasOpenedContext()) {
    if(SelMode==-1 && !AllowDecomposition){
      Standard_Integer DispMode,HiMod,SelModeDef;
      
      GetDefModes(anIObj,DispMode,HiMod,SelModeDef);
      Handle(AIS_GlobalStatus) STATUS= 
        new AIS_GlobalStatus(AIS_DS_Erased,DispMode,SelModeDef);
      myObjects.Bind (anIObj,STATUS);      
      return;
    }
  }
  myLocalContexts(myCurLocalIndex)->Load(anIObj,AllowDecomposition,SelMode);
}


//=======================================================================
//function : Erase
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Erase(const Handle(AIS_InteractiveObject)& anIObj, 
                                   const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return;
  
  if ( !anIObj->IsAutoHilight() )
    anIObj->ClearSelected();
  
  if(!HasOpenedContext()){
    EraseGlobal(anIObj,updateviewer);
  }
  else
    {
      // First it is checked if it is possible to remove in the current local context
      // then one tries to remove in other local contexts, if they allow it...
      
      Standard_Boolean WasInCtx = myLocalContexts(myCurLocalIndex)->Erase(anIObj);
//      if(!WasInCtx) {
      AIS_DataMapIteratorOfDataMapOfILC It(myLocalContexts);
      for (;It.More();It.Next()){
        if(It.Value()->AcceptErase())
#ifdef BUC60722
          WasInCtx |= It.Value()->Erase(anIObj);
#else
          WasInCtx = It.Value()->Erase(anIObj);
#endif
      }
      
      if(!WasInCtx)
        EraseGlobal(anIObj,updateviewer);
      else
        if(updateviewer) myMainVwr->Update();
    }
}

//=======================================================================
//function : EraseAll
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::EraseAll(const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext())
  {
    for (AIS_DataMapIteratorOfDataMapOfIOStatus ItM(myObjects); ItM.More(); ItM.Next())
    {
      if(ItM.Value()->GraphicStatus() == AIS_DS_Displayed)
        Erase(ItM.Key(),Standard_False);
    }
    if(updateviewer)
    {
      myMainVwr->Update();
    }
  }
}

//=======================================================================
//function : DisplayAll
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::DisplayAll(const Standard_Boolean updateviewer)
{
  if(!HasOpenedContext()){

    AIS_DisplayStatus aStatus;

    if (!HasOpenedContext())
    {
      for (AIS_DataMapIteratorOfDataMapOfIOStatus ItM (myObjects); ItM.More(); ItM.Next())
      {
        aStatus = ItM.Value()->GraphicStatus();
        if (aStatus == AIS_DS_Erased)
          Display(ItM.Key(),Standard_False);
      }
      if(updateviewer)
      {
        myMainVwr->Update();
      }
    }
  }
  
}

//=======================================================================
//function : DisplaySelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::DisplaySelected(const Standard_Boolean updateviewer)
{
  
  if (!HasOpenedContext())
  {
    Standard_Boolean found = Standard_False;
    Handle(AIS_Selection) sel = AIS_Selection::Selection(myCurrentName.ToCString());
    Handle(AIS_InteractiveObject) iObj;
    for (sel->Init(); sel->More(); sel->Next())
    {
      iObj = Handle(AIS_InteractiveObject)::DownCast(sel->Value());
      Display(iObj,Standard_False);
      found = Standard_True;
    }
    if (found && updateviewer)
    {
      myMainVwr->Update();
    }
  }
}


//=======================================================================
//function : EraseSelected
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::EraseSelected(const Standard_Boolean updateviewer)
{
  if (!HasOpenedContext())
  {
    Standard_Boolean found = Standard_False;
    Handle(AIS_Selection) sel = AIS_Selection::Selection(myCurrentName.ToCString());
    Handle(AIS_InteractiveObject) iObj;
    for (sel->Init();sel->More();sel->Next())
    {
      iObj = Handle(AIS_InteractiveObject)::DownCast(sel->Value());
      Erase(iObj,Standard_False);
      found = Standard_True;
    }
    if(found && updateviewer)
    {
      myMainVwr->Update();
    }
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
//purpose  : 
//=======================================================================

AIS_DisplayStatus AIS_InteractiveContext::DisplayStatus(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  if(anIObj.IsNull()) return AIS_DS_None;

  if(myObjects.IsBound(anIObj))
    return myObjects(anIObj)->GraphicStatus();

  AIS_DataMapIteratorOfDataMapOfILC ItM(myLocalContexts);
  for(;ItM.More();ItM.Next()){
    if(ItM.Value()->IsIn(anIObj))
      return AIS_DS_Temporary;
  }
  
  return AIS_DS_None;
  
}




//=======================================================================
//function : DisplayedModes
//purpose  : 
//=======================================================================

const TColStd_ListOfInteger& AIS_InteractiveContext::
DisplayedModes(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  return myObjects(anIObj)->DisplayedModes();
}


//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::Remove(const Handle(AIS_InteractiveObject)& anIObj,
                                    const Standard_Boolean updateviewer)
{

  if(anIObj.IsNull()) return;

  if(!HasOpenedContext()){ 
    ClearGlobal(anIObj,updateviewer);
  }
  else
    {
      myLocalContexts(myCurLocalIndex)->Remove(anIObj);
      AIS_DataMapIteratorOfDataMapOfILC It(myLocalContexts);
      for (;It.More() ;It.Next()){
        if(It.Value()->AcceptErase())
          It.Value()->Remove(anIObj);
        
      }
      //      if(!WasInCtx)
      ClearGlobal(anIObj,updateviewer);
//      else
//      if(updateviewer) myMainVwr->Update();
    }
}

//=======================================================================
//function : RemoveAll
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::RemoveAll(const Standard_Boolean updateviewer)
{
  AIS_ListOfInteractive aList;
  ObjectsInside(aList);
  AIS_ListIteratorOfListOfInteractive aListIterator;
  for(aListIterator.Initialize(aList);aListIterator.More();aListIterator.Next()){
    Remove(aListIterator.Value(), Standard_False);
  }
	
  if(updateviewer)
    myMainVwr->Update();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Clear(const Handle(AIS_InteractiveObject)& anIObj,
                                   const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return;

  if(!HasOpenedContext()){ 
    ClearGlobal(anIObj,updateviewer);
  }
  else
    {
      
      Standard_Boolean  WasInCtx = myLocalContexts(myCurLocalIndex)->Remove(anIObj);
      AIS_DataMapIteratorOfDataMapOfILC It(myLocalContexts);
      for (;It.More() ;It.Next()){
        if(It.Value()->AcceptErase())
          WasInCtx = It.Value()->Remove(anIObj);
        
      }
      if(!WasInCtx)
        ClearGlobal(anIObj,updateviewer);
      else
        if(updateviewer) myMainVwr->Update();
    }
}




//=======================================================================
//function : ClearPrs
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ClearPrs(const Handle(AIS_InteractiveObject)& anIObj,
                                      const Standard_Integer aMode,
                                      const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return;

  if(!HasOpenedContext()){
    ClearGlobalPrs(anIObj,aMode,updateviewer);
  }
  else
    {
      Standard_Boolean  WasInCtx = myLocalContexts(myCurLocalIndex)->ClearPrs(anIObj,aMode);
      AIS_DataMapIteratorOfDataMapOfILC It(myLocalContexts);
      for (;It.More() ;It.Next()){
        if(It.Value()->AcceptErase())
          WasInCtx = It.Value()->ClearPrs(anIObj,aMode);
      }
      if(!WasInCtx)
        ClearGlobalPrs(anIObj,aMode,updateviewer);
      else
        if(updateviewer) myMainVwr->Update();
      
    }
}

//=======================================================================
//function : Hilight
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Hilight(const Handle(AIS_InteractiveObject)& anIObj,
                                     const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return;

  if(!anIObj->HasInteractiveContext()) anIObj->SetContext(this);
  if (!HasOpenedContext())
  {
    if(!myObjects.IsBound(anIObj)) return;

    // CLE
    // const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
    // const TColStd_ListOfInteger& LL = STATUS->DisplayedModes();
    Handle(AIS_GlobalStatus) STATUS = myObjects(anIObj);

    // ENDCLE
    STATUS->SetHilightStatus (Standard_True);

    if (STATUS->GraphicStatus() == AIS_DS_Displayed)
    {
      Standard_Integer aHilightMode = anIObj->HasHilightMode() ? anIObj->HilightMode() : 0;
      myMainPM->Highlight (anIObj, aHilightMode);
    }
  }
  else
  {
    myLocalContexts(myCurLocalIndex)->Hilight(anIObj);
  }

  if(updateviewer) myMainVwr->Update();
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

    const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
    STATUS->SetHilightStatus (Standard_True);

    if (STATUS->GraphicStatus() == AIS_DS_Displayed)
    {
      Standard_Integer aHilightMode = anIObj->HasHilightMode() ? anIObj->HilightMode() : 0;
      myMainPM->Color (anIObj, aCol, aHilightMode);
      STATUS->SetHilightColor (aCol);
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

    const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
    STATUS->SetHilightStatus (Standard_False);
    STATUS->SetHilightColor(Quantity_NOC_WHITE);

    if (STATUS->GraphicStatus() == AIS_DS_Displayed)
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

Standard_Boolean AIS_InteractiveContext::
IsDisplayed(const Handle(AIS_InteractiveObject)& anIObj,
            const Standard_Integer aMode) const 
{
  if(anIObj.IsNull()) return Standard_False;
  if(myObjects.IsBound(anIObj)) {
    if(myObjects(anIObj)->GraphicStatus()==AIS_DS_Displayed &&
       myObjects(anIObj)->IsDModeIn(aMode)) 
      return Standard_True;
  }
  
  AIS_DataMapIteratorOfDataMapOfILC ItM(myLocalContexts);
  for(;ItM.More();ItM.Next()){
    if(ItM.Value()->IsDisplayed(anIObj,aMode))
      return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : DisplayPriority
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveContext::
DisplayPriority(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  if(anIObj.IsNull()) return -1;

  if (myObjects.IsBound(anIObj))
  {
    Handle(AIS_GlobalStatus) STATUS = myObjects(anIObj);
    if (STATUS->GraphicStatus() == AIS_DS_Displayed || STATUS->GraphicStatus() == AIS_DS_Erased)
    {
      Standard_Integer aDispMode = anIObj->HasDisplayMode() ? anIObj->DisplayMode() : 
                                   (anIObj->AcceptDisplayMode(myDisplayMode)? myDisplayMode : 0);
      return myMainPM->DisplayPriority (anIObj, aDispMode);
    }
  }
  return 0;
}
//=======================================================================
//function : SetDisplayPriority
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetDisplayPriority(const Handle(AIS_InteractiveObject)& anIObj,
                                               const Standard_Integer aPriority)
{
  if(anIObj.IsNull())
    return;
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  if(myObjects.IsBound(anIObj))
  {
    Handle(AIS_GlobalStatus) STATUS = myObjects(anIObj);

    if (STATUS->GraphicStatus() == AIS_DS_Displayed || STATUS->GraphicStatus() == AIS_DS_Erased)
    {
      Standard_Integer aDisplayMode = anIObj->HasDisplayMode() ? anIObj->DisplayMode() : 
                                      (anIObj->AcceptDisplayMode(myDisplayMode)? myDisplayMode : 0);
      myMainPM->SetDisplayPriority (anIObj, aDisplayMode, aPriority);
    }
  }
  else if (HasOpenedContext())
  {
    myLocalContexts(myCurLocalIndex)->SetDisplayPriority(anIObj,aPriority);
  }
}

//=======================================================================
//function : Redisplay
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Redisplay(const Handle(AIS_InteractiveObject)& anIObj,
                                      const Standard_Boolean updateviewer,
                                      const Standard_Boolean allmodes)
{
  if(AISDebugModeOn()){
    cout<<"===>AIS_InteractiveContext::Redisplay de :";
    cout<<anIObj->DynamicType()->Name()<<endl;
  }

  RecomputePrsOnly(anIObj,updateviewer,allmodes);
  RecomputeSelectionOnly(anIObj);
}

//=======================================================================
//function : Redisplay
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Redisplay(const AIS_KindOfInteractive KOI,
                                       const Standard_Integer /*Sign*/,
                                      const Standard_Boolean updateviewer)
{
  Standard_Boolean found_viewer(Standard_False);
  // update
  for(AIS_DataMapIteratorOfDataMapOfIOStatus It(myObjects);It.More();It.Next()){
    // CLE
    // const Handle(AIS_InteractiveObject)& IO = It.Key();
    Handle(AIS_InteractiveObject) IO = It.Key();
    // ENDCLE
    if(IO->Type()== KOI){ 
#ifdef DEB
//      Standard_Boolean good = (Sign==-1)? Standard_True : 
//        ((IO->Signature()==Sign)? Standard_True:Standard_False);
#endif
      Redisplay(IO,Standard_False);
      if (It.Value()->GraphicStatus() == AIS_DS_Displayed)
      {
        found_viewer = Standard_True;
      }
    }
  }
  // update viewer...
  if(updateviewer && found_viewer)
  {
    myMainVwr->Update();
  }
}


//=======================================================================
//function : RecomputePrsOnly
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::RecomputePrsOnly(const Handle(AIS_InteractiveObject)& anIObj,
                                              const Standard_Boolean updateviewer,
                                              const Standard_Boolean allmodes)
{
  if(anIObj.IsNull()) return;
  anIObj->Update(allmodes);

  if (!updateviewer)
  {
    return;
  }

  if (HasOpenedContext() ||
     (myObjects.IsBound(anIObj) && myObjects(anIObj)->GraphicStatus() == AIS_DS_Displayed))
  {
    myMainVwr->Update();
  }
}
//=======================================================================
//function : RecomputeSelectionOnly
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::RecomputeSelectionOnly(const Handle(AIS_InteractiveObject)& anIObj)
{
  if(anIObj.IsNull()) return;
  mgrSelector->RecomputeSelection(anIObj);



  TColStd_ListOfInteger LI;
  TColStd_ListIteratorOfListOfInteger Lit;
  ActivatedModes(anIObj,LI);
  if(!HasOpenedContext()){
    if(!myObjects.IsBound(anIObj)) return;

    if (myObjects(anIObj)->GraphicStatus() == AIS_DS_Displayed)
    {
      for(Lit.Initialize(LI);Lit.More();Lit.Next())
      {
        mgrSelector->Activate(anIObj,Lit.Value(),myMainSel);
      }
    }
  }
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::Update (const Handle(AIS_InteractiveObject)& theIObj,
                                     const Standard_Boolean theUpdateViewer)
{
  if (theIObj.IsNull())
  {
    return;
  }

  TColStd_ListOfInteger aListOfFlaggedPrsModes;
  theIObj->ToBeUpdated (aListOfFlaggedPrsModes);

  TColStd_ListIteratorOfListOfInteger aPrsModesIt (aListOfFlaggedPrsModes);
  for ( ; aPrsModesIt.More(); aPrsModesIt.Next())
  {
    theIObj->Update (aPrsModesIt.Value(), Standard_False);
  }

  mgrSelector->Update(theIObj);

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

void AIS_InteractiveContext::SetLocation(const Handle(AIS_InteractiveObject)& anIObj,
                                         const TopLoc_Location& aLoc)
{
  if(anIObj.IsNull()) return;


  if(anIObj->HasLocation() && aLoc.IsIdentity()){
    anIObj->ResetLocation();
    mgrSelector->Update(anIObj,Standard_False);
    return;
  }
  if(aLoc.IsIdentity()) return ;

  // first reset the previous location to properly clean everything...
  if(anIObj->HasLocation())
    anIObj->ResetLocation();


  anIObj->SetLocation(aLoc);
  
  if(!HasOpenedContext())
    mgrSelector->Update(anIObj,Standard_False);
  else
    {
      // CLE
      // const Handle(StdSelect_ViewerSelector3d)& tempSel = myLocalContexts(myCurLocalIndex)->MainSelector();
      Handle(StdSelect_ViewerSelector3d) tempSel = myLocalContexts(myCurLocalIndex)->MainSelector();
      // ENDCLE
      mgrSelector->Update(anIObj,tempSel,Standard_False);
    }
}
//=======================================================================
//function : ResetLocation
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::ResetLocation(const Handle(AIS_InteractiveObject)& anIObj)
{
  if(anIObj.IsNull()) return;

  anIObj->ResetLocation();
  mgrSelector->Update(anIObj,Standard_False);
}

//=======================================================================
//function : HasLocation
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::
HasLocation(const Handle(AIS_InteractiveObject)& anIObj) const
{
  if(anIObj.IsNull()) return Standard_False;

  return anIObj->HasLocation();
}

const TopLoc_Location& AIS_InteractiveContext::
Location(const Handle(AIS_InteractiveObject)& anIObj) const
{
  return anIObj->Location();
}

//=======================================================================
//function : SetDeviationCoefficient
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetDeviationCoefficient(const Standard_Real aCoefficient)
{
  myDefaultDrawer->SetDeviationCoefficient(aCoefficient);
}
//=======================================================================
//function : SetDeviationAngle
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetDeviationAngle(const Standard_Real anAngle)
{
  myDefaultDrawer->SetDeviationCoefficient(anAngle);
}

//=======================================================================
//function : DeviationAngle
//purpose  : Gets  deviationAngle
//=======================================================================

Standard_Real AIS_InteractiveContext::DeviationAngle() const
{
  //return M_PI/180.0e0 ;
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

void AIS_InteractiveContext::SetHLRDeviationCoefficient(const Standard_Real aCoefficient)
{
  myDefaultDrawer->SetHLRDeviationCoefficient(aCoefficient);
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

void AIS_InteractiveContext::SetHLRAngle(const Standard_Real anAngle)
{
  myDefaultDrawer->SetHLRAngle(anAngle);
}

//=======================================================================
//function : SetHLRAngleAndDeviation
//purpose  : compute with anangle a HLRAngle and a HLRDeviationCoefficient 
//           and set them in myHLRAngle and in myHLRDeviationCoefficient
//           of myDefaultDrawer 
//=======================================================================

void AIS_InteractiveContext::SetHLRAngleAndDeviation(const Standard_Real anAngle)
{

  Standard_Real OutAngl,OutDefl;
  HLRBRep::PolyHLRAngleAndDeflection(anAngle,OutAngl,OutDefl);

  myDefaultDrawer->SetHLRAngle(OutAngl);
  myDefaultDrawer->SetHLRDeviationCoefficient(OutDefl);

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

void AIS_InteractiveContext::SetDisplayMode(const AIS_DisplayMode aMode,
                                           const Standard_Boolean updateviewer)
{
  if(aMode==myDisplayMode) return;
  AIS_DataMapIteratorOfDataMapOfIOStatus It(myObjects);

  for(;It.More();It.Next()){
    // CLE
    // const Handle(AIS_InteractiveObject)& anObj = It.Key();
    Handle(AIS_InteractiveObject) anObj = It.Key();
    // ENDCLE
    Standard_Boolean Processed = (anObj->IsKind(STANDARD_TYPE(AIS_Shape)) ||
                                  anObj->IsKind(STANDARD_TYPE(AIS_ConnectedShape)) ||
                                  anObj->IsKind(STANDARD_TYPE(AIS_MultipleConnectedShape)) );
    
    if ((!anObj->HasDisplayMode()) && Processed) 
      {
        if(anObj->AcceptDisplayMode(aMode)){
          // CLE
          // const Handle(AIS_GlobalStatus)& STATUS = It.Value();
          Handle(AIS_GlobalStatus) STATUS = It.Value();
          // ENDCLE
          if(STATUS->IsDModeIn(myDisplayMode))
            STATUS->RemoveDisplayMode(myDisplayMode);
          
          STATUS->AddDisplayMode(aMode);
          
          if(STATUS->GraphicStatus()== AIS_DS_Displayed){
            myMainPM->SetVisibility (anObj, myDisplayMode, Standard_False);
            myMainPM->Display(anObj, aMode);
            if(STATUS->IsSubIntensityOn())
              myMainPM->Color(anObj,mySubIntensity,aMode);
          }
          //  myDisplayMode = aMode;
        }
      }
    
  } 
  myDisplayMode = aMode;
  if(updateviewer) myMainVwr->Update();
}

//=======================================================================
//function : SetDisplayMode
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetDisplayMode(const Handle(AIS_InteractiveObject)& anIObj, 
                                           const Standard_Integer aMode,
                                           const Standard_Boolean updateviewer)
{
  
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  if(!HasOpenedContext()){
    
    if(!myObjects.IsBound(anIObj)) 
      anIObj->SetDisplayMode(aMode);
    else if( anIObj->AcceptDisplayMode(aMode) ) 
    {
      // CLE
      // const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
      Handle(AIS_GlobalStatus) STATUS = myObjects(anIObj);
      // ENDCLE
      // SAN : erase presentations for all display modes different from <aMode>
      if(STATUS->GraphicStatus()==AIS_DS_Displayed){
        TColStd_ListOfInteger aModesToRemove;
        TColStd_ListIteratorOfListOfInteger ItL (STATUS->DisplayedModes());
        for(;ItL.More();ItL.Next()){

          Standard_Integer OldMode = ItL.Value();

          if(OldMode!=aMode){
            aModesToRemove.Append(OldMode);
            if(myMainPM->IsHighlighted(anIObj,OldMode))
              myMainPM->Unhighlight(anIObj,OldMode);
            myMainPM->SetVisibility (anIObj, OldMode, Standard_False);
          }
        }

        for(ItL.Initialize(aModesToRemove);ItL.More();ItL.Next())
          STATUS->RemoveDisplayMode(ItL.Value());

        if(!STATUS->IsDModeIn(aMode))
          STATUS->AddDisplayMode(aMode);

        myMainPM->Display(anIObj,aMode);
        Standard_Integer DM,HM,SM;
        GetDefModes(anIObj,DM,HM,SM);
        if(STATUS->IsHilighted()){
          myMainPM->Highlight(anIObj,HM);
        }
        if(STATUS->IsSubIntensityOn()){
          myMainPM->Color(anIObj,mySubIntensity,aMode);
        }
        if(anIObj->IsTransparent() && !myMainVwr->Viewer()->Transparency())
          myMainVwr->Viewer()->SetTransparency(Standard_True);
        
        if(updateviewer) myMainVwr->Update();
      }
      anIObj->SetDisplayMode(aMode);
    }
  }
}

//=======================================================================
//function : UnsetDisplayMode
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::
UnsetDisplayMode(const Handle(AIS_InteractiveObject)& anIObj,
                 const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
  if(!anIObj->HasDisplayMode()) return;
  
  if(!myObjects.IsBound(anIObj))
    anIObj->UnsetDisplayMode();
  else
    {
      Standard_Integer OldMode = anIObj->DisplayMode();
      if(myDisplayMode==OldMode) return;
      const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
      STATUS->RemoveDisplayMode(OldMode);
      if(!STATUS->IsDModeIn(myDisplayMode))
        STATUS->AddDisplayMode(myDisplayMode);
      if(STATUS->GraphicStatus()==AIS_DS_Displayed){
        if(myMainPM->IsHighlighted(anIObj,OldMode))
          myMainPM->Unhighlight(anIObj,OldMode);
        myMainPM->SetVisibility (anIObj, OldMode, Standard_False);
        myMainPM->Display(anIObj,myDisplayMode);
        Standard_Integer DM,HM,SM;
        GetDefModes(anIObj,DM,HM,SM);
        if(STATUS->IsHilighted()){
          myMainPM->Highlight(anIObj,HM);
        }
        if(STATUS->IsSubIntensityOn()){
          myMainPM->Color(anIObj,mySubIntensity,myDisplayMode);
        }
        if(updateviewer) myMainVwr->Update();
      }
      anIObj->UnsetDisplayMode();
      
    }
}

//=======================================================================
//function : SetCurrentFacingModel
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetCurrentFacingModel(
                        const Handle(AIS_InteractiveObject)& anIObj,
                        const Aspect_TypeOfFacingModel aModel)
{
  if (  !anIObj.IsNull ()  )
    anIObj->SetCurrentFacingModel(aModel);
}

//=======================================================================
//function : SetColor
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetColor(const Handle(AIS_InteractiveObject)& anIObj, 
                                     const Quantity_NameOfColor aColor,
                                     const Standard_Boolean updateviewer)
{
  SetColor(anIObj,Quantity_Color(aColor),updateviewer);
}

void AIS_InteractiveContext::SetColor(const Handle(AIS_InteractiveObject)& anIObj, 
                                     const Quantity_Color &aColor,
                                     const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
 
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  anIObj->SetColor(aColor);
  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      Standard_Integer NbDisp=0;
      TColStd_ListIteratorOfListOfInteger ITI(anIObj->ListOfRecomputeModes());
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
          NbDisp++;
        }
      anIObj->SetRecomputeOk();
#ifdef DEB
      cout<<"nb of modes to recalculate : "<<NbDisp<<endl;
#endif
    }


#ifdef DEB
//   // pour isg
//   if(anIObj->Type()==AIS_KOI_Datum && anIObj->Signature()==3){
//     Handle(AIS_Trihedron) Tr = *((Handle(AIS_Trihedron)*)&anIObj);
//     Standard_Real Lx = myDefaultDrawer->DatumAspect()->FirstAxisLength();
//     gp_Trsf T; 
//     gp_Dir D =Tr->Component()->XDirection();
//     gp_Pnt O = Tr->Component()->Location();
//     gp_Vec V(D);V*=Lx/5.;
//     T.SetRotation(gp_Ax1(O,D),M_PI/6.);
//     T.SetTranslationPart(V);
//     TopLoc_Location L,IncLoc(T);
   
//     for(Standard_Integer I=1;I<90;I++){
//       if(anIObj->HasLocation())
//      L = anIObj->Location() * IncLoc;
//       else
//      L = IncLoc;
//       SetLocation(anIObj,L);
//       UpdateCurrentViewer();
//     }
//      for(I=1;I<90;I++){
//        L = anIObj->Location() /IncLoc;
//       SetLocation(anIObj,L);
//       UpdateCurrentViewer();
//     }
//    ResetLocation(anIObj);
//   }
 #endif
  if(updateviewer) UpdateCurrentViewer();
}

//=======================================================================
//function : SetDeviationCoefficient
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetDeviationCoefficient(
                                     const Handle(AIS_InteractiveObject)& anIObj, 
                                     const Standard_Real   aCoefficient,
                                     const Standard_Boolean updateviewer)
{
//  cout<<" Coefficient:"<< aCoefficient <<endl;
  if(anIObj.IsNull()) return ;
 
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if(anIObj->Type()!=AIS_KOI_Object && anIObj->Type()!=AIS_KOI_Shape) return;
  if(anIObj->Signature()!=0) return;
  (*((Handle(AIS_Shape)*)&anIObj))->SetOwnDeviationCoefficient(aCoefficient);



  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      Standard_Integer NbDisp=0;
      TColStd_ListOfInteger LL;
      anIObj->ToBeUpdated(LL);
      TColStd_ListIteratorOfListOfInteger ITI(LL);
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
          NbDisp++;
        }
      anIObj->SetRecomputeOk();
#ifdef DEB
      cout<<"nb of modes to recalculate : "<<NbDisp<<endl;
#endif
    }
  if(updateviewer) UpdateCurrentViewer();
}

//=======================================================================
//function : SetHLRDeviationCoefficient
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetHLRDeviationCoefficient(
                                     const Handle(AIS_InteractiveObject)& anIObj, 
                                     const Standard_Real    aCoefficient,
                                     const Standard_Boolean updateviewer)
{
//  cout<<" HLRCoefficient:"<< aCoefficient <<endl;
  if(anIObj.IsNull()) return ;
 
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if(anIObj->Type()!=AIS_KOI_Object && anIObj->Type()!=AIS_KOI_Shape) return;
  if(anIObj->Signature()!=0) return;
  (*((Handle(AIS_Shape)*)&anIObj))->SetOwnHLRDeviationCoefficient(aCoefficient);



  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      Standard_Integer NbDisp=0;

      TColStd_ListOfInteger LL;
      anIObj->ToBeUpdated(LL);
      TColStd_ListIteratorOfListOfInteger ITI(LL);
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
          NbDisp++;
        }
      anIObj->SetRecomputeOk();
#ifdef DEB
      cout<<"nb of modes to recalculate : "<<NbDisp<<endl;
#endif
    }
  if(updateviewer) UpdateCurrentViewer();
}


//=======================================================================
//function : SetDeviationAngle
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetDeviationAngle(
                                     const Handle(AIS_InteractiveObject)& anIObj, 
                                     const Standard_Real   anAngle,
                                     const Standard_Boolean updateviewer)
{
//  cout<<" Angle:"<< anAngle <<endl;
  if(anIObj.IsNull()) return ;
 
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if(anIObj->Type()!=AIS_KOI_Shape) return;
  if(anIObj->Signature()!=0) return;
  (*((Handle(AIS_Shape)*)&anIObj))->SetOwnDeviationAngle(anAngle);


  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      Standard_Integer NbDisp=0;
      TColStd_ListOfInteger LL;
      anIObj->ToBeUpdated(LL);
      TColStd_ListIteratorOfListOfInteger ITI(LL);
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
          NbDisp++;
        }
      anIObj->SetRecomputeOk();
#ifdef DEB
      cout<<"nb of modes to recalculate : "<<NbDisp<<endl;
#endif
    }
  if(updateviewer) UpdateCurrentViewer();
}
//=======================================================================
//function : SetDeviationAngle
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetAngleAndDeviation(
                                     const Handle(AIS_InteractiveObject)& anIObj, 
                                     const Standard_Real   anAngle,
                                     const Standard_Boolean updateviewer)
{
//  cout<<" Angle:"<< anAngle <<endl;
  if(anIObj.IsNull()) return ;
//   Standard_Real anAngleRad = M_PI*anAngle/180; test rob...
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if(anIObj->Type()!=AIS_KOI_Shape) return;
  if(anIObj->Signature()!=0) return;
  (*((Handle(AIS_Shape)*)&anIObj))->SetAngleAndDeviation(anAngle);


  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    Update(anIObj,updateviewer);
}

//=======================================================================
//function : SetDeviationAngle
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetHLRAngleAndDeviation(
                                     const Handle(AIS_InteractiveObject)& anIObj, 
                                     const Standard_Real   anAngle,
                                     const Standard_Boolean updateviewer)
{
//  cout<<" Angle:"<< anAngle <<endl;

  if(anIObj.IsNull()) return ;
   if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);

  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if(anIObj->Type()!=AIS_KOI_Shape) return;
  if(anIObj->Signature()!=0) return;
  (*((Handle(AIS_Shape)*)&anIObj))->SetHLRAngleAndDeviation(anAngle);


  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      Standard_Integer NbDisp=0;

      TColStd_ListOfInteger LL;
      anIObj->ToBeUpdated(LL);
      TColStd_ListIteratorOfListOfInteger ITI(LL);
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
          NbDisp++;
        }
      anIObj->SetRecomputeOk();
#ifdef DEB
      cout<<"nb of modes to recalculate : "<<NbDisp<<endl;
#endif
    }
  if(updateviewer) UpdateCurrentViewer();
}

//=======================================================================
//function : SetHLRDeviationAngle
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetHLRDeviationAngle(
                                     const Handle(AIS_InteractiveObject)& anIObj, 
                                     const Standard_Real   anAngle,
                                     const Standard_Boolean updateviewer)
{
//  cout<<" HLRAngle:"<< anAngle <<endl;
  if(anIObj.IsNull()) return ;
 
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  // To be modified after the related methods of AIS_Shape are passed to InteractiveObject
  if( anIObj->Type()!=AIS_KOI_Shape) return;
  if(anIObj->Signature()!=0) return;
  (*((Handle(AIS_Shape)*)&anIObj))->SetOwnHLRDeviationAngle(anAngle);


  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      Standard_Integer NbDisp=0;
      TColStd_ListOfInteger LL;
      anIObj->ToBeUpdated(LL);
      TColStd_ListIteratorOfListOfInteger ITI(LL);
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
          NbDisp++;
        }
      anIObj->SetRecomputeOk();
#ifdef DEB
      cout<<"nb of modes to recalculate : "<<NbDisp<<endl;
#endif
    }
  if(updateviewer) UpdateCurrentViewer();
}

//=======================================================================
//function : UnsetColor
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UnsetColor(const Handle(AIS_InteractiveObject)& anIObj,
                                     const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
   anIObj->UnsetColor();
  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      Standard_Integer NbDisp =0;
      TColStd_ListIteratorOfListOfInteger ITI(anIObj->ListOfRecomputeModes());
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
          NbDisp++;
        }
#ifdef DEB
      cout<<"nb of modes to recalculate : "<<NbDisp<<endl;
#endif
      anIObj->SetRecomputeOk();
    }
  if(updateviewer) 
    UpdateCurrentViewer();
}

//=======================================================================
//function : HasColor
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::HasColor(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  return anIObj->HasColor();

}

//=======================================================================
//function : Color
//purpose  : 
//=======================================================================

Quantity_NameOfColor AIS_InteractiveContext::Color(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  return anIObj->Color();
}

void AIS_InteractiveContext::Color(const Handle(AIS_InteractiveObject)& anIObj,
                                   Quantity_Color &aColor) const
{
  anIObj->Color(aColor);
}

//=======================================================================
//function : Width
//purpose  : 
//=======================================================================

Standard_Real AIS_InteractiveContext::Width(const Handle(AIS_InteractiveObject)& anIObj) const 
{
  return anIObj->Width();
}

//=======================================================================
//function : SetWidth
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetWidth(const Handle(AIS_InteractiveObject)& anIObj,
                                     const Standard_Real aValue,
                                     const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
 
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  anIObj->SetWidth(aValue);
  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      Standard_Integer NbDisp=0;
      TColStd_ListIteratorOfListOfInteger ITI(anIObj->ListOfRecomputeModes());
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
          NbDisp++;
        }
#ifdef DEB
      cout<<"nb of modes to recalculate : "<<NbDisp<<endl;
#endif
      anIObj->SetRecomputeOk();
    }
   if(updateviewer) 
    UpdateCurrentViewer();
}

//=======================================================================
//function : UnsetWidth
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UnsetWidth(const Handle(AIS_InteractiveObject)& anIObj,
                                       const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
   anIObj->UnsetWidth();
  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      TColStd_ListIteratorOfListOfInteger ITI(anIObj->ListOfRecomputeModes());
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
        }
      anIObj->SetRecomputeOk();
    }
   if(updateviewer) 
    UpdateCurrentViewer();
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetMaterial(const Handle(AIS_InteractiveObject)& anIObj,
//POP pour K4L
                                        const Graphic3d_NameOfMaterial aName,
//                                      const Graphic3d_NameOfPhysicalMaterial aName,
                                        const Standard_Boolean updateviewer)
{
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  anIObj->SetMaterial(aName);
  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      TColStd_ListIteratorOfListOfInteger ITI(anIObj->ListOfRecomputeModes());
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
        }
      anIObj->SetRecomputeOk();
    }
   if(updateviewer)
    UpdateCurrentViewer();

}

//=======================================================================
//function : UnsetMaterial
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UnsetMaterial(const Handle(AIS_InteractiveObject)& anIObj,
                                          const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
  anIObj->UnsetMaterial();
  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      TColStd_ListIteratorOfListOfInteger ITI(anIObj->ListOfRecomputeModes());
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
        }
      anIObj->SetRecomputeOk();
    }
  if(updateviewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : SetTransparency
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetTransparency(const Handle(AIS_InteractiveObject)& anIObj,
                                            const Standard_Real aValue,
                                            const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
   if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);

#ifdef BUC60577 //right optimization
  if(!anIObj->IsTransparent() && aValue<=0.05) return;
#else  
//  if(!anIObj->IsTransparent() && aValue<=0.05) return;
#endif
  if(aValue<=0.05){
    UnsetTransparency(anIObj,updateviewer);
    return;
  }
  
  if(!myMainVwr->Viewer()->Transparency())
    myMainVwr->Viewer()->SetTransparency(Standard_True);
  anIObj->SetTransparency(aValue);
  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      TColStd_ListIteratorOfListOfInteger ITI(anIObj->ListOfRecomputeModes());
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
        }
      anIObj->SetRecomputeOk();
    }
   if(updateviewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : UnsetTransparency
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UnsetTransparency(const Handle(AIS_InteractiveObject)& anIObj,
                                              const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
   anIObj->UnsetTransparency();
  if(anIObj->RecomputeEveryPrs())
    anIObj->Redisplay();
  else
    {
      TColStd_ListIteratorOfListOfInteger ITI(anIObj->ListOfRecomputeModes());
      for (;ITI.More();ITI.Next())
        {
          anIObj->Update(ITI.Value(),Standard_False);
        }
      anIObj->SetRecomputeOk();
    }

  // To Unset transparency in the viewer, if no other object is transparent ...(Speed)
  AIS_DataMapIteratorOfDataMapOfIOStatus It(myObjects);
  Standard_Boolean FoundTransp(Standard_False);
  for(;It.More() && !FoundTransp ;It.Next()){
    if(It.Key()->IsTransparent())
      FoundTransp = Standard_True;
  }
  if(!FoundTransp)
    myMainVwr->Viewer()->SetTransparency(Standard_False);
  
  
  if(updateviewer)
    UpdateCurrentViewer();
}

//=======================================================================
//function : SetSelectedAspect
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::SetSelectedAspect(
                                const Handle(Prs3d_BasicAspect)& anAspect,
                                const Standard_Boolean globalChange,
                                const Standard_Boolean updateViewer)
{
  if( !HasOpenedContext() ) {
    Standard_Boolean found = Standard_False;
    Handle(AIS_Selection) sel = 
                AIS_Selection::Selection(myCurrentName.ToCString());
    Handle(AIS_InteractiveObject) object;
    for( sel->Init() ; sel->More() ; sel->Next()) {
      found = Standard_True;
      object = Handle(AIS_InteractiveObject)::DownCast(sel->Value());
      object->SetAspect(anAspect,globalChange);
    }
    if( found && updateViewer) {
      myMainVwr->Update();
    }
  }
}

//=======================================================================
//function : SetLocalAttributes
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetLocalAttributes(const Handle(AIS_InteractiveObject)& anIObj, 
                                                const Handle(AIS_Drawer)& aDrawer,
                                                const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  anIObj->SetAttributes(aDrawer);
  Update(anIObj,updateviewer);
  
}

//=======================================================================
//function : UnsetLocalAttributes
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::UnsetLocalAttributes(const Handle(AIS_InteractiveObject)& anIObj,
                                                  const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
  if(!anIObj->HasInteractiveContext())
    anIObj->SetContext(this);
  anIObj->UnsetAttributes();
  Update(anIObj,updateviewer);
}


//=======================================================================
//function : Status
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::Status(const Handle(AIS_InteractiveObject)& anIObj, TCollection_ExtendedString& astatus) const 
{
  astatus = "";

  if(anIObj.IsNull()) return ;
  if(myObjects.IsBound(anIObj)){
    astatus += "\t ____________________________________________";
    astatus += "\t| Known at Neutral Point:\n\tDisplayStatus:";
    const Handle(AIS_GlobalStatus)& ST = myObjects(anIObj);
    switch(ST->GraphicStatus()){
    case AIS_DS_Displayed:
      {
        astatus +="\t| -->Displayed\n";
        break;
      }
    case AIS_DS_Erased:
      {
        astatus +="\t| -->Erased\n";
        break;
      }
    default:
      break;
    }
    astatus += "\t| Active Display Modes in the MainViewer :\n";
    TColStd_ListIteratorOfListOfInteger ItL (ST->DisplayedModes());
    for(;ItL.More();ItL.Next()){
      astatus += "\t|\t Mode ";
      astatus += TCollection_AsciiString(ItL.Value());
      astatus+="\n";
    }   
    if(IsCurrent(anIObj)) astatus +="\t| Current\n";
    if(IsSelected(anIObj)) astatus +="\t| Selected\n";

    astatus += "\t| Active Selection Modes in the MainViewer :\n";
    for(ItL.Initialize(ST->SelectionModes());ItL.More();ItL.Next()){
      astatus += "\t\t Mode ";
      astatus += TCollection_AsciiString(ItL.Value());
      astatus+="\n";
    }   
    astatus += "\t ____________________________________________";
      
    }
}


//=======================================================================
//function : GetDefModes
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::GetDefModes(const Handle(AIS_InteractiveObject)& anIObj,
                                     Standard_Integer& DispMode,
                                     Standard_Integer& HiMode,
                                     Standard_Integer& SelMode) const 
{
  if(anIObj.IsNull()) return ;
  DispMode = anIObj->HasDisplayMode() ? anIObj->DisplayMode() : 
  (anIObj->AcceptDisplayMode(myDisplayMode)? myDisplayMode : 0);
  
  HiMode = anIObj->HasHilightMode()? anIObj->HilightMode():DispMode;
#ifdef BUC61051
  SelMode = anIObj->HasSelectionMode()? anIObj->SelectionMode() : -1;
#else
  SelMode = anIObj->HasSelectionMode()? anIObj->SelectionMode() : 0;
#endif
}


//=======================================================================
//function : EraseGlobal
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::EraseGlobal(const Handle(AIS_InteractiveObject)& anIObj, 
                                         const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
  if(!myObjects.IsBound(anIObj)) return;

  // CLE
  // const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
  Handle(AIS_GlobalStatus) STATUS = myObjects(anIObj);
  // ENDCLE
  Standard_Integer Dmode = anIObj->HasHilightMode() ? anIObj->HilightMode() : 0;
  if(STATUS->GraphicStatus()==AIS_DS_Displayed){
    
    TColStd_ListIteratorOfListOfInteger ItL (STATUS->DisplayedModes());
    for(;ItL.More();ItL.Next()){
      if(myMainPM->IsHighlighted(anIObj,ItL.Value()))
        myMainPM->Unhighlight(anIObj,ItL.Value());
      myMainPM->SetVisibility (anIObj, ItL.Value(), Standard_False);
    }
    if(IsCurrent(anIObj) && !STATUS->IsDModeIn(Dmode))
      myMainPM->SetVisibility (anIObj, Dmode, Standard_False);
    
    for(ItL.Initialize(STATUS->SelectionModes());ItL.More();ItL.Next())
      mgrSelector->Deactivate(anIObj,ItL.Value(),myMainSel);
    if(updateviewer) myMainVwr->Update();
  }
  STATUS->SetGraphicStatus(AIS_DS_Erased);
  
}

//=======================================================================
//function : ClearGlobal
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ClearGlobal(const Handle(AIS_InteractiveObject)& anIObj, 
                                         const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
  if(!myObjects.IsBound(anIObj)) return;
  // CLE
  // const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
  Handle(AIS_GlobalStatus) STATUS = myObjects(anIObj);
  // ENDCLE
   TColStd_ListIteratorOfListOfInteger ItL (STATUS->DisplayedModes());
   for(;ItL.More();ItL.Next()){
     if(STATUS->IsHilighted()){
       if(IsCurrent(anIObj))
#ifdef OCC204
         AddOrRemoveCurrentObject( anIObj, updateviewer );
#else
         AddOrRemoveCurrentObject(anIObj);
#endif
       else if(myMainPM->IsHighlighted(anIObj,ItL.Value()))
         myMainPM->Unhighlight(anIObj,ItL.Value());
     }
     myMainPM->Erase(anIObj,ItL.Value());
     myMainPM->Clear(anIObj,ItL.Value());
     if(anIObj->HasHilightMode()){
       Standard_Integer im = anIObj->HilightMode();
       myMainPM->Unhighlight(anIObj,im);
       myMainPM->Erase(anIObj,im);
  
     }
   }

  //Object removes from Detected sequence
  Standard_Integer i = 1;
  for(i = 1; i < myAISDetectedSeq.Length(); i++)
  {
    Handle(AIS_InteractiveObject) anObj = DetectedCurrentObject();
    if( !anObj.IsNull() && anObj != anIObj )
      myAISDetectedSeq.Remove( i );
  }

  if(myLastinMain == anIObj)
    myLastinMain.Nullify();

  if(myLastPicked == anIObj)
    myLastPicked.Nullify();

  // OCC21671: Remove IO from the selection manager in any case
  // to avoid memory leaks
  mgrSelector->Remove(anIObj);

  if (updateviewer && (STATUS->GraphicStatus() == AIS_DS_Displayed))
  {
    myMainVwr->Update();
  }

  myObjects.UnBind(anIObj);
  
}

//=======================================================================
//function : ClearGlobalPrs
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::ClearGlobalPrs(const Handle(AIS_InteractiveObject)& anIObj, 
                                            const Standard_Integer aMode,
                                            const Standard_Boolean updateviewer)
{
  if(anIObj.IsNull()) return ;
  if(!myObjects.IsBound(anIObj)) return;
  const Handle(AIS_GlobalStatus)& STATUS = myObjects(anIObj);
  
  Standard_Integer DM = anIObj->HasHilightMode() ? anIObj->HilightMode() : 0; 
  if(STATUS->IsDModeIn(aMode)){
    if(DM==aMode && myMainPM->IsHighlighted(anIObj,aMode))
      myMainPM->Unhighlight(anIObj,aMode);
    
    myMainPM->Erase(anIObj,aMode);
    myMainPM->Clear(anIObj,aMode);
  }
  

  if(STATUS->GraphicStatus()==AIS_DS_Displayed && updateviewer)
    myMainVwr->Update();
}

//=======================================================================
//function : DrawHiddenLine
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::DrawHiddenLine () const {

  return myDefaultDrawer->DrawHiddenLine();
}

//=======================================================================
//function : EnableDrawHiddenLine
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::EnableDrawHiddenLine () const {
  myDefaultDrawer->EnableDrawHiddenLine();
}

//=======================================================================
//function : DisableDrawHiddenLine 
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::DisableDrawHiddenLine () const {
  myDefaultDrawer->DisableDrawHiddenLine();
}

//=======================================================================
//function : HiddenLineAspect
//purpose  : 
//=======================================================================

Handle (Prs3d_LineAspect) AIS_InteractiveContext::HiddenLineAspect () const  {
  return myDefaultDrawer->HiddenLineAspect();
}

//=======================================================================
//function : SetHiddenLineAspect
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetHiddenLineAspect ( const Handle(Prs3d_LineAspect)& anAspect) const {
  myDefaultDrawer->SetHiddenLineAspect(anAspect);
}

//=======================================================================
//function : SetIsoNumber
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetIsoNumber(const Standard_Integer Nb,const AIS_TypeOfIso Type)
{
  switch(Type){
  case AIS_TOI_IsoU:
    myDefaultDrawer->UIsoAspect()->SetNumber(Nb);
    break;
  case AIS_TOI_IsoV:
    myDefaultDrawer->VIsoAspect()->SetNumber(Nb);
    break;
  case AIS_TOI_Both:
    myDefaultDrawer->UIsoAspect()->SetNumber(Nb);
    myDefaultDrawer->VIsoAspect()->SetNumber(Nb);
    break;
 
  }
}
//=======================================================================
//function : IsoNumber
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveContext::IsoNumber(const AIS_TypeOfIso Type)
{
  
  switch(Type){
  case AIS_TOI_IsoU:
    return myDefaultDrawer->UIsoAspect()->Number();
  case AIS_TOI_IsoV:
    return myDefaultDrawer->VIsoAspect()->Number();
  case AIS_TOI_Both:
    return (myDefaultDrawer->UIsoAspect()->Number()==
            myDefaultDrawer->VIsoAspect()->Number()) ? 
              myDefaultDrawer->UIsoAspect()->Number(): -1;
  }
  return 0;
}

//=======================================================================
//function : IsoOnPlane
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::IsoOnPlane(const Standard_Boolean SwitchOn)
{
  myDefaultDrawer->SetIsoOnPlane(SwitchOn);
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
//function : SetSelectionMode
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetSelectionMode(const Handle_AIS_InteractiveObject&, const Standard_Integer )
{
}

//=======================================================================
//function : UnsetSelectionMode
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::UnsetSelectionMode(const Handle_AIS_InteractiveObject&)
{
}

//=======================================================================
//function : SetSensitivityMode
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetSensitivityMode(const StdSelect_SensitivityMode aMode) {

  if( HasOpenedContext() )
        myLocalContexts(myCurLocalIndex)->SetSensitivityMode(aMode);
  else {
    myMainSel->SetSensitivityMode(aMode);
  }
}

//=======================================================================
//function : SensitivityMode
//purpose  : 
//=======================================================================

StdSelect_SensitivityMode AIS_InteractiveContext::SensitivityMode() const {

  if( HasOpenedContext() )
        return myLocalContexts(myCurLocalIndex)->SensitivityMode();
  return myMainSel->SensitivityMode();
}

//=======================================================================
//function : SetSensitivity
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetSensitivity(const Standard_Real aPrecision) {

  if( HasOpenedContext() )
        myLocalContexts(myCurLocalIndex)->SetSensitivity(aPrecision);
  else {
    myMainSel->SetSensitivity(aPrecision);
  }
}

//=======================================================================
//function : Sensitivity
//purpose  : 
//=======================================================================

Standard_Real AIS_InteractiveContext::Sensitivity() const {

  if( HasOpenedContext() )
        return myLocalContexts(myCurLocalIndex)->Sensitivity();
  return myMainSel->Sensitivity();
}

//=======================================================================
//function : SetPixelTolerance
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetPixelTolerance(const Standard_Integer aPrecision) {

  if( HasOpenedContext() )
        myLocalContexts(myCurLocalIndex)->SetPixelTolerance(aPrecision);
  else {
    myMainSel->SetPixelTolerance(aPrecision);
  }
}

//=======================================================================
//function : PixelTolerance
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveContext::PixelTolerance() const {

  if( HasOpenedContext() )
        return myLocalContexts(myCurLocalIndex)->PixelTolerance();
  return myMainSel->PixelTolerance();
}

//=======================================================================
//function : IsInLocal
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::IsInLocal(const Handle_AIS_InteractiveObject& anIObj,
                                                   Standard_Integer& TheIndex) const 
{
  if(anIObj.IsNull()) return Standard_False;
  // if it exists at neutral point 0 index is returned
  if(myObjects.IsBound(anIObj)) {
    TheIndex = 0;
    return Standard_False;
  }
  for(Standard_Integer I=1;I<=myLocalContexts.Extent();I++){
    if(myLocalContexts.IsBound(I)){
      if(myLocalContexts(I)->IsIn(anIObj)){
        TheIndex = I;
        return Standard_True;
        
      }
    }
  }
  TheIndex =-1;
  return Standard_False;
}  
  
//=======================================================================
//function : InitAttributes
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::InitAttributes()
{

  mgrSelector->Add(myMainSel);
  myCurrentName   = AIS_Context_NewCurName();
  mySelectionName = AIS_Context_NewSelName();

  AIS_Selection::CreateSelection(mySelectionName.ToCString());
  AIS_Selection::CreateSelection(myCurrentName.ToCString());

  
  myDefaultDrawer->SetShadingAspectGlobal(Standard_False);
  Graphic3d_MaterialAspect aMat(Graphic3d_NOM_BRASS);
  myDefaultDrawer->ShadingAspect()->SetMaterial(aMat);

//  myDefaultDrawer->ShadingAspect()->SetColor(Quantity_NOC_GRAY70);
  Handle (Prs3d_LineAspect) HLA = myDefaultDrawer->HiddenLineAspect();
  HLA->SetColor(Quantity_NOC_GRAY20);
  HLA->SetWidth(1);
  HLA->SetTypeOfLine(Aspect_TOL_DASH);

  // tolerance to 4 pixels...
  SetPixelTolerance();

  // Customizing the drawer for trihedrons and planes...

  Handle (Prs3d_DatumAspect) DA = myDefaultDrawer->DatumAspect();
  Standard_Real aLength(100.);
  DA->SetAxisLength(aLength,aLength,aLength);
  Quantity_NameOfColor col = Quantity_NOC_LIGHTSTEELBLUE4;
  DA->FirstAxisAspect()->SetColor(col);
  DA->SecondAxisAspect()->SetColor(col);
  DA->ThirdAxisAspect()->SetColor(col);

  Handle(Prs3d_PlaneAspect)PA = myDefaultDrawer->PlaneAspect();
  aLength =200.;
  PA->SetPlaneLength(aLength,aLength);
  PA->EdgesAspect()->SetColor(Quantity_NOC_SKYBLUE);
  

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
void AIS_InteractiveContext::SetTrihedronSize(const Standard_Real aVal,const Standard_Boolean /*updateviewer*/)
{
  myDefaultDrawer->DatumAspect()->SetAxisLength(aVal,aVal,aVal);
  Redisplay(AIS_KOI_Datum,3,Standard_False);
  Redisplay(AIS_KOI_Datum,4,Standard_True);
}



//=======================================================================
//function : SetPlaneSize
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::SetPlaneSize(const Standard_Real aValX,
                                          const Standard_Real aValY,
                                          const Standard_Boolean /*updateviewer*/)
{
  myDefaultDrawer->PlaneAspect()->SetPlaneLength(aValX,aValY);
  Redisplay(AIS_KOI_Datum,7);
}

//=======================================================================
//function : SetPlaneSize
//purpose  : 
//=======================================================================

void AIS_InteractiveContext::SetPlaneSize(const Standard_Real aVal,
                                          const Standard_Boolean updateviewer)
{
  SetPlaneSize(aVal,aVal,updateviewer);
}

//=======================================================================
//function : PlaneSize
//purpose  : 
//=======================================================================

Standard_Boolean AIS_InteractiveContext::PlaneSize(Standard_Real& LX,Standard_Real& LY) const
{
  LX = myDefaultDrawer->PlaneAspect()->PlaneXLength();
  LY =  myDefaultDrawer->PlaneAspect()->PlaneYLength();
  return (Abs(LX-LY)<=Precision::Confusion());
}



//=======================================================================
//function :
//purpose  : 
//=======================================================================
void AIS_InteractiveContext::SetAutoActivateSelection( const Standard_Boolean Auto )
{
  myIsAutoActivateSelMode = Auto;
}

//=======================================================================
//function :
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
  if (theIObj.IsNull ())
    return;

  if (myObjects.IsBound (theIObj))
  {
    if (myObjects (theIObj)->GraphicStatus() == AIS_DS_Displayed || myObjects (theIObj)->GraphicStatus() == AIS_DS_Erased)
    {
      theIObj->SetZLayer (myMainPM, theLayerId);
    }
  }
  else if (HasOpenedContext ())
  {
    myLocalContexts (myCurLocalIndex)->SetZLayer (theIObj, theLayerId);
  }
}

//=======================================================================
//function : GetZLayer
//purpose  : 
//=======================================================================

Standard_Integer AIS_InteractiveContext::GetZLayer (const Handle(AIS_InteractiveObject)& theIObj) const
{
  if (theIObj.IsNull ())
    return -1;

  if (myObjects.IsBound (theIObj))
  {
    return theIObj->GetZLayer (myMainPM);
  }
  else if (HasOpenedContext ())
  {
    return myLocalContexts (myCurLocalIndex)->GetZLayer (theIObj);
  }

  return -1;
}
