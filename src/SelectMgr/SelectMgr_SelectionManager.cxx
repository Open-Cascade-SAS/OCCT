// Created on: 1995-02-13
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <SelectMgr_SelectionManager.ixx>
#include <SelectMgr_ViewerSelector.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SequenceOfSelector.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TColStd_MapIteratorOfMapOfTransient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <SelectMgr_DataMapIteratorOfDataMapOfObjectSelectors.hxx>
#include <OSD_Environment.hxx>


static Standard_Boolean SelectDebugModeOnSM()
{
  static Standard_Integer isDebugMode( -1 );
  if ( isDebugMode < 0 ) {
    isDebugMode = 1;
    OSD_Environment selectdb("SELECTIONDEBUG");
    if ( selectdb.Value().IsEmpty() )
      isDebugMode = 0;
  }                       
  return ( isDebugMode != 0 );
}

static Standard_Integer SMSearch(const SelectMgr_SequenceOfSelector& seq,
                                 const Handle(SelectMgr_ViewerSelector)& theSel)
{
  Standard_Integer ifound=0;
  for (Standard_Integer i=1;i<=seq.Length()&& ifound==0;i++)
  {if(theSel==seq.Value(i)) ifound=i;}
  return ifound;

} 

//==================================================
// Function: Create
// Purpose :
//==================================================

SelectMgr_SelectionManager::SelectMgr_SelectionManager()
{}


//==================================================
// Function: Add
// Purpose :
//==================================================
void SelectMgr_SelectionManager::
Add (const Handle(SelectMgr_ViewerSelector)& aViewSel)
{
  myselectors.Add(aViewSel);
}



//==================================================
// Function: Remove
// Purpose :
//==================================================
void SelectMgr_SelectionManager::
Remove (const Handle(SelectMgr_ViewerSelector)& aViewSel)
{
  SelectMgr_DataMapIteratorOfDataMapOfObjectSelectors It(mylocal);
  for(;It.More();It.Next())
  {
    SelectMgr_SequenceOfSelector& theviews =mylocal.ChangeFind(It.Key());
    Standard_Integer rank = SMSearch(theviews,aViewSel);
    if(rank!=0 && rank<=theviews.Length()) theviews.Remove(rank);
  }
  if(myselectors.Contains(aViewSel)) myselectors.Remove(aViewSel);
}

//==================================================
// Function: Contains
// Purpose :
//==================================================
Standard_Boolean SelectMgr_SelectionManager::
Contains (const Handle(SelectMgr_ViewerSelector)& aViewSel) const
{return myselectors.Contains(aViewSel);}

//==================================================
// Function: Contains
// Purpose :
//==================================================
Standard_Boolean SelectMgr_SelectionManager::
Contains (const Handle(SelectMgr_SelectableObject)& aSelObj) const
{if (myglobal.Contains(aSelObj)) return Standard_True;
if (mylocal.IsBound(aSelObj)) return Standard_True;
return Standard_False;
}



//==================================================
// Function: Load
// Purpose :
//==================================================

void SelectMgr_SelectionManager::
Load (const Handle(SelectMgr_SelectableObject)& anObject,
      const Standard_Integer amode)
{
  if(!myglobal.Contains(anObject))
    myglobal.Add(anObject);
  if(amode!=-1) 
    LoadMode (anObject,amode);
}


//==================================================
// Function: Load
// Purpose :
//==================================================
void SelectMgr_SelectionManager::
Load (const Handle(SelectMgr_SelectableObject)& anObject,
      const Handle(SelectMgr_ViewerSelector)& aview,      
      const Standard_Integer amode)
{
  if(!myselectors.Contains(aview)) myselectors.Add(aview);
  if(amode!=-1)
    LoadMode (anObject,amode);


  if (mylocal.IsBound(anObject)){
    SelectMgr_SequenceOfSelector& theviews = mylocal.ChangeFind(anObject);
    if (SMSearch(theviews,aview)==0) theviews.Append(aview);
  }
  else {
    if(!myglobal.Contains(anObject)){
      SelectMgr_SequenceOfSelector newviews;
      newviews.Append(aview);
      mylocal.Bind(anObject,newviews);
    }
  }
}


//==================================================
// Function: Remove
// Purpose :
//==================================================

void SelectMgr_SelectionManager::
Remove(const Handle(SelectMgr_SelectableObject)& anObject)
{

  if(myglobal.Contains(anObject)) {
    TColStd_MapIteratorOfMapOfTransient It(myselectors);
    for(;It.More();It.Next())
    {
      Handle(SelectMgr_ViewerSelector) curview = 
        Handle(SelectMgr_ViewerSelector)::DownCast(It.Key());
      if(curview->Contains(anObject)){
        for(anObject->Init();anObject->More();anObject->Next())
        {
          curview->Remove(anObject->CurrentSelection());
        }

      }
    }
    myglobal.Remove(anObject);
  }

  else if(mylocal.IsBound(anObject)) {
    SelectMgr_SequenceOfSelector& seq = mylocal.ChangeFind (anObject);
    for (Standard_Integer i=1;i<=seq.Length();i++) {
      Handle(SelectMgr_ViewerSelector) curview =
        Handle(SelectMgr_ViewerSelector)::DownCast(seq(i));
      if(curview->Contains(anObject)){
        for(anObject->Init();anObject->More();anObject->Next())
        {
          curview->Remove(anObject->CurrentSelection());
        }
      }

    }
    mylocal.UnBind(anObject);
  }
}

//==================================================
// Function: Remove
// Purpose :
//==================================================

void SelectMgr_SelectionManager::
Remove(const Handle(SelectMgr_SelectableObject)& anObject,
       const Handle(SelectMgr_ViewerSelector)& aVS)
{
  if(aVS->Contains(anObject)) {
    for(anObject->Init();anObject->More();anObject->Next()){
      aVS->Remove(anObject->CurrentSelection());
    }


    if(mylocal.IsBound(anObject)) {
      SelectMgr_SequenceOfSelector& seq = mylocal.ChangeFind (anObject);
      Standard_Boolean NotFound (Standard_True);
      for (Standard_Integer i=1;i<=seq.Length()&&NotFound;i++) {
        if(seq(i)== aVS){
          seq.Remove(i);
          NotFound =Standard_False;
        }
      }
      if(seq.IsEmpty())
        mylocal.UnBind(anObject);
    }
  }
}

//==================================================
// Function: Activate
// Purpose :
//==================================================

void SelectMgr_SelectionManager::
Activate(const Handle(SelectMgr_SelectableObject)& anObject,
         const Standard_Integer aMode,
         const Standard_Boolean AutomaticProj)
{
  if(aMode==-1) return;
  //  Standard_Boolean global = Standard_False;
  if(!anObject->HasSelection(aMode)) LoadMode(anObject,aMode);


  if(myglobal.Contains(anObject)) {
    TColStd_MapIteratorOfMapOfTransient It(myselectors);

    for(;It.More();It.Next()){
      Handle(SelectMgr_ViewerSelector) curview = 
        Handle(SelectMgr_ViewerSelector)::DownCast(It.Key());
      Activate(anObject,aMode,curview,AutomaticProj);
    }
  }

  else if(mylocal.IsBound(anObject)) {
    SelectMgr_SequenceOfSelector& seq = mylocal.ChangeFind (anObject);
    for (Standard_Integer i=1;i<=seq.Length();i++) {
      Handle(SelectMgr_ViewerSelector) curview =
        Handle(SelectMgr_ViewerSelector)::DownCast(seq(i));
      // ATTENTION : si la selection est a remettre a jour, on le fait la ....      
      const Handle(SelectMgr_Selection)& Sel = anObject->Selection(aMode);

      switch(Sel->UpdateStatus()){
      case SelectMgr_TOU_Full:
        anObject->UpdateSelection(aMode); // pas de break expres...
      case SelectMgr_TOU_Partial:
        {
          if(anObject->HasLocation())
            anObject->UpdateLocation(Sel);
          Sel->UpdateStatus(SelectMgr_TOU_None);
          break;
        }
      default:
        break;
      }

      curview->Activate(Sel,AutomaticProj);
    }
  }
}


//==================================================
// Function: Activate
// Purpose :
//==================================================

void SelectMgr_SelectionManager::
Activate(const Handle(SelectMgr_SelectableObject)& anObject,
         const Standard_Integer aMode,
         const Handle(SelectMgr_ViewerSelector)& aViewSel,
         const Standard_Boolean AutomaticProj)
{
  if(aMode==-1) return;

  if(!myselectors.Contains(aViewSel)) return;

  if (!anObject->HasSelection(aMode)) LoadMode(anObject,aMode);

  const Handle(SelectMgr_Selection)& Sel = anObject->Selection(aMode);

  switch(Sel->UpdateStatus()){
  case SelectMgr_TOU_Full:
    anObject->UpdateSelection(aMode); 
  case SelectMgr_TOU_Partial:
    {
      if(anObject->HasLocation())
        anObject->UpdateLocation(Sel);
      break;
    }
  default:
    break;
  }
  Sel->UpdateStatus(SelectMgr_TOU_None);

  if  (myglobal.Contains(anObject)) 
    aViewSel->Activate (anObject->Selection(aMode));

  else {
    if (mylocal.IsBound(anObject)) {
      if (SMSearch(mylocal.Find(anObject),aViewSel)==0)
        (mylocal.ChangeFind (anObject)).Append(aViewSel);
      aViewSel->Activate (anObject->Selection(aMode),AutomaticProj);
    }
  }
}

//==================================================
// Function: Deactivate
// Purpose :
//==================================================

void SelectMgr_SelectionManager::
Deactivate(const Handle(SelectMgr_SelectableObject)& anObject)
{
  Standard_Boolean global = Standard_False;
  if(myglobal.Contains(anObject)) global = Standard_True;
  TColStd_MapIteratorOfMapOfTransient It(myselectors);
  Handle(SelectMgr_ViewerSelector) curview; 
  for(;It.More();It.Next()){
    curview = Handle(SelectMgr_ViewerSelector)::DownCast(It.Key());
    if (global || mylocal.IsBound (anObject)) {
      for (anObject->Init();anObject->More();anObject->Next())
      {curview->Deactivate(anObject->CurrentSelection());}  

    }

  }
}

//==================================================
// Function: Deactivate
// Purpose :
//==================================================

void SelectMgr_SelectionManager::
Deactivate(const Handle(SelectMgr_SelectableObject)& anObject,
           const Standard_Integer amode)

{
  Standard_Boolean global = Standard_False;
  if(myglobal.Contains(anObject)) global = Standard_True;
  TColStd_MapIteratorOfMapOfTransient It(myselectors);
  Handle(SelectMgr_ViewerSelector) curview;
  for(;It.More();It.Next()){
    curview = Handle(SelectMgr_ViewerSelector)::DownCast(It.Key());
    if (global || mylocal.IsBound(anObject)) {
      if(anObject->HasSelection(amode))
        curview->Deactivate(anObject->Selection(amode));

    }
  }
}

//==================================================
// Function: Deactivate
// Purpose :
//==================================================

void SelectMgr_SelectionManager::
Deactivate(const Handle(SelectMgr_SelectableObject)& anObject,
           const Standard_Integer aMode,
           const Handle(SelectMgr_ViewerSelector)& aViewSel)
{
  if(myselectors.Contains(aViewSel))
  {
    if(myglobal.Contains(anObject)|| mylocal.IsBound(anObject)) 
      if(anObject->HasSelection(aMode))
        aViewSel->Deactivate (anObject->Selection(aMode));
  }  

}
//==================================================
// Function: Deactivate
// Purpose :
//==================================================

void SelectMgr_SelectionManager::
Deactivate(const Handle(SelectMgr_SelectableObject)& anObject,
           const Handle(SelectMgr_ViewerSelector)& aViewSel)

{
  if(myselectors.Contains(aViewSel))
  {
    if(myglobal.Contains(anObject)|| mylocal.IsBound(anObject)) {
      for (anObject->Init();anObject->More();anObject->Next())
      {aViewSel->Deactivate(anObject->CurrentSelection());}}  

  }  

}


//==================================================
// Function: Sleep
// Purpose :
//==================================================
void SelectMgr_SelectionManager::
Sleep (const Handle(SelectMgr_ViewerSelector)& aViewSel)
{
  if (myselectors.Contains(aViewSel))
    aViewSel->Sleep();
}

void SelectMgr_SelectionManager::
Sleep (const Handle(SelectMgr_SelectableObject)& anObject)
{

  if(myglobal.Contains(anObject)){
    for( TColStd_MapIteratorOfMapOfTransient It(myselectors);
      It.More();It.Next())
      Handle(SelectMgr_ViewerSelector)::DownCast(It.Key())->Sleep(anObject);
  }
  else if(mylocal.IsBound(anObject)){
    const SelectMgr_SequenceOfSelector & VSeq = mylocal(anObject);
    for (Standard_Integer I=1;I<=VSeq.Length();I++)
      VSeq(I)->Sleep(anObject);
  }


}

//=======================================================================
//function : Sleep
//purpose  : 
//=======================================================================
void SelectMgr_SelectionManager::
Sleep(const Handle(SelectMgr_SelectableObject)& anObject,
      const Handle(SelectMgr_ViewerSelector)& aViewSel)
{
  if(!myselectors.Contains(aViewSel)) return;

  if(!myglobal.Contains(anObject)){
    if(!mylocal.IsBound(anObject))
      return;
    if(SMSearch(mylocal(anObject),aViewSel)==0)
      return;
  }
  aViewSel->Sleep(anObject);
}



//==================================================
// Function: Awake
// Purpose :
//==================================================
void SelectMgr_SelectionManager::
Awake (const Handle(SelectMgr_ViewerSelector)& aViewSel,
       const Standard_Boolean AutomaticProj)
{
  if (myselectors.Contains(aViewSel))
    aViewSel->Awake(AutomaticProj);
}


//=======================================================================
//function : Awake
//purpose  : 
//=======================================================================
void SelectMgr_SelectionManager::Awake (const Handle(SelectMgr_SelectableObject)& anObject,
                                        const Standard_Boolean AutomaticProj)
{
  if(myglobal.Contains(anObject)){
    for( TColStd_MapIteratorOfMapOfTransient It(myselectors);
      It.More();It.Next())
      Handle(SelectMgr_ViewerSelector)::DownCast( It.Key())->Awake(anObject,AutomaticProj);
  }
  else if(mylocal.IsBound(anObject)){
    const SelectMgr_SequenceOfSelector & VSeq = mylocal(anObject);
    for (Standard_Integer I=1;I<=VSeq.Length();I++)
      VSeq(I)->Awake(anObject,AutomaticProj);
  }
}

//=======================================================================
//function : Awake
//purpose  : 
//=======================================================================
void SelectMgr_SelectionManager::Awake (const Handle(SelectMgr_SelectableObject)& anObject,
                                        const Handle(SelectMgr_ViewerSelector)& aViewSel,
                                        const Standard_Boolean AutomaticProj)
{
  if(!myselectors.Contains(aViewSel)) return;

  if(!myglobal.Contains(anObject)){
    if(!mylocal.IsBound(anObject))
      return;
    if(SMSearch(mylocal(anObject),aViewSel)==0)
      return;
  }
  aViewSel->Awake(anObject,AutomaticProj);

}


//=======================================================================
//function : IsActivated
//purpose  : 
//=======================================================================
Standard_Boolean SelectMgr_SelectionManager::IsActivated(const Handle(SelectMgr_SelectableObject)& anObject) const
{
  for(anObject->Init();anObject->More();anObject->Next()){
    if(IsActivated(anObject,anObject->CurrentSelection()->Mode()))
      return Standard_True;
  }
  return Standard_False;
}
//=======================================================================
//function : IsActivated
//purpose  : 
//=======================================================================
Standard_Boolean SelectMgr_SelectionManager::IsActivated(const Handle(SelectMgr_SelectableObject)& anObject,
                                                         const Standard_Integer aMode) const
{
  if(!anObject->HasSelection(aMode)) return Standard_False;
  if (!(myglobal.Contains(anObject) || mylocal.IsBound(anObject))) 
    return Standard_False;

  Handle(Standard_Transient) Tr;
  const Handle(SelectMgr_Selection)& Sel = anObject->Selection(aMode);
  for(TColStd_MapIteratorOfMapOfTransient It(myselectors);It.More();It.Next()){
    Tr = It.Key();
    Handle(SelectMgr_ViewerSelector) VS = *((Handle(SelectMgr_ViewerSelector)*)&Tr);
    if(VS->Status(Sel)==SelectMgr_SOS_Activated)
      return Standard_True;
  }
  return Standard_False;

}

//=======================================================================
//function : IsActivated
//purpose  : 
//=======================================================================
Standard_Boolean SelectMgr_SelectionManager::IsActivated(const Handle(SelectMgr_SelectableObject)& anObject,
                                                         const Handle(SelectMgr_ViewerSelector)& VS,
                                                         const Standard_Integer aMode) const
{
  if(!anObject->HasSelection(aMode))                               
    return Standard_False;
  if(!myselectors.Contains(VS))                                   
    return Standard_False;
  if (!(myglobal.Contains(anObject) || mylocal.IsBound(anObject))) 
    return Standard_False;
  const Handle(SelectMgr_Selection)& Sel = anObject->Selection(aMode);
  return (VS->Status(Sel)==SelectMgr_SOS_Activated);
}

//==================================================
// Function: Update
// Purpose :
//==================================================
void SelectMgr_SelectionManager::
RecomputeSelection (const Handle(SelectMgr_SelectableObject)& anObject,
                    const Standard_Boolean ForceUpdate,
                    const Standard_Integer aMode)
{
  if( SelectDebugModeOnSM() ) cout<<"===>SelectMgr_SelectionManager::Update"<<endl;

  if(ForceUpdate){
    if( SelectDebugModeOnSM() ) cout<<"\t Global Recalculation of selections"<<endl;
    if(aMode==-1){
      anObject->UpdateSelection();
      anObject->UpdateLocation();
    }
    else if(anObject->HasSelection(aMode)){
      anObject->UpdateSelection(aMode);
      anObject->UpdateLocation();
    }
    return;
  }
  // objet is not known to SMgr.
  if (!(myglobal.Contains(anObject) || mylocal.IsBound(anObject))){
    if( SelectDebugModeOnSM() ) {cout<<"\t Object not loaded in the SelectionManager"<<endl;
    cout<<"\t eventual selections are flagged"<<endl;}
    if( aMode == -1 ){
      for(anObject->Init();anObject->More();anObject->Next()){
        if( SelectDebugModeOnSM() ) cout<<"\t\t Mode "<<anObject->CurrentSelection()->Mode()<<"  ";
        anObject->CurrentSelection()->UpdateStatus(SelectMgr_TOU_Full);
      }
      if( SelectDebugModeOnSM() )  
        cout << endl;
    }
    else if (anObject->HasSelection(aMode))
      anObject->Selection(aMode)->UpdateStatus(SelectMgr_TOU_Full);
  }

  // recalculate whatever is required
  // and set flag on top...
  else{
    TColStd_MapIteratorOfMapOfTransient It;
    Handle(Standard_Transient) Tr;
    Standard_Boolean Found;
    // object selections are parsed

    for(anObject->Init();anObject->More();anObject->Next()){
      const Handle(SelectMgr_Selection)& Sel = anObject->CurrentSelection();
      Sel->UpdateStatus(SelectMgr_TOU_Full);
      Standard_Integer curmode = Sel->Mode();
      Found = Standard_False;

      // parsing of selections ...
      for(It.Initialize(myselectors);It.More();It.Next()){
        Tr = It.Key();
        Handle(SelectMgr_ViewerSelector) VS = *((Handle(SelectMgr_ViewerSelector)*)&Tr);
        if(VS->Status(Sel)==SelectMgr_SOS_Activated){
          Found  = Standard_True;
          switch(Sel->UpdateStatus()){
    case SelectMgr_TOU_Full:
      anObject->UpdateSelection(curmode); // no break on purpose...
    case SelectMgr_TOU_Partial:
      anObject->UpdateLocation(Sel);
      break;
    default:
      break;
          }
          if(Found){
            VS->Convert(Sel);
            Sel->UpdateStatus(SelectMgr_TOU_None);
          }
        }
      }
    }
  }
}

//=======================================================================
//function : Update
//purpose  : Selections are recalculated if they are flagged
//           "TO RECALCULATE" and activated in one of selectors.
//           If ForceUpdate = True, and they are "TO RECALCULATE"
//           This is done without caring for the state of activation.
//=======================================================================
void SelectMgr_SelectionManager::Update(const Handle(SelectMgr_SelectableObject)& anObject,
                                        const Standard_Boolean ForceUpdate)
{
  Standard_Boolean wasrecomputed;

  for(anObject->Init();anObject->More();anObject->Next()){
    const Handle(SelectMgr_Selection)& Sel = anObject->CurrentSelection();
    wasrecomputed = Standard_False;
    if(ForceUpdate){
      switch(Sel->UpdateStatus()){
      case SelectMgr_TOU_Full:
        anObject->UpdateSelection(Sel->Mode()); // no break on purpose...
      case SelectMgr_TOU_Partial:
        anObject->UpdateLocation(Sel);
        wasrecomputed = Standard_True;
        break;
      default:
        break;
      }
      Sel->UpdateStatus(SelectMgr_TOU_None);
    }

    // it is checked which selectors are concerned by the selection
    // to redo projections if necessary.
    Handle(Standard_Transient) Tr;
    for(TColStd_MapIteratorOfMapOfTransient It(myselectors);It.More();It.Next()){
      Tr = It.Key();
      Handle(SelectMgr_ViewerSelector) VS = *((Handle(SelectMgr_ViewerSelector)*)&Tr);
      if(VS->Status(Sel)==SelectMgr_SOS_Activated)
        switch(Sel->UpdateStatus()){
  case SelectMgr_TOU_Full:
    anObject->UpdateSelection(Sel->Mode()); // no break on purpose...
  case SelectMgr_TOU_Partial:
    anObject->UpdateLocation(Sel);
    wasrecomputed = Standard_True;
    break;
  default:
    break;
      }
      if(wasrecomputed)
        VS->Convert(Sel);
      Sel->UpdateStatus(SelectMgr_TOU_None);
    }
  }
}


//==================================================
// Function: Update
// Purpose : Attention, it is required to know what is done...
//==================================================
void SelectMgr_SelectionManager::
Update(const Handle(SelectMgr_SelectableObject)& anObject,
       const Handle(SelectMgr_ViewerSelector)& aViewSel,
       const Standard_Boolean ForceUpdate)
{ 
  if( SelectDebugModeOnSM() ) cout<<"==>SelectMgr_SelectionManager::Update(obj,VS)"<<endl;
  if(!myselectors.Contains(aViewSel)) return;

  Standard_Boolean okay = myglobal.Contains(anObject);
  if(!okay)
    okay = (mylocal.IsBound(anObject) && (SMSearch(mylocal.Find(anObject),aViewSel)!=0)) ;
  if(!okay) return;


  // 
  Standard_Boolean wasrecomputed;
  for(anObject->Init();anObject->More();anObject->Next()){
    const Handle(SelectMgr_Selection)& Sel = anObject->CurrentSelection();
    wasrecomputed = Standard_False;
    if(ForceUpdate){
      switch(Sel->UpdateStatus()){
      case SelectMgr_TOU_Full:
        anObject->UpdateSelection(Sel->Mode()); //  no break on purpose...
      case SelectMgr_TOU_Partial:
        anObject->UpdateLocation(Sel);
        wasrecomputed = Standard_True;
        break;
      default:
        break;
      }
      Sel->UpdateStatus(SelectMgr_TOU_None);
    }

    if(aViewSel->Status(Sel) == SelectMgr_SOS_Activated){
      switch(Sel->UpdateStatus()){
      case SelectMgr_TOU_Full:
        anObject->UpdateSelection(Sel->Mode());
      case SelectMgr_TOU_Partial:
        if(anObject->HasLocation())
          anObject->UpdateLocation(Sel);
        wasrecomputed = Standard_True;
        break;
      default:
        break;
      }
      if(wasrecomputed)
        aViewSel->Convert(Sel);
      Sel->UpdateStatus(SelectMgr_TOU_None);
    }
  }
}

//==================================================
// Function: Status
// Purpose :
//==================================================
TCollection_AsciiString SelectMgr_SelectionManager::
Status() const{
  TCollection_AsciiString theMgrStatus("\t\t\tStatus of the SelectManager :;\n\t\t\t============================\n");

  TCollection_AsciiString nbview (myselectors.Extent()),nbglobal(myglobal.Extent());

  theMgrStatus +=             "\t Number of ViewerSelectors: ";
  theMgrStatus += nbview +  "\n\t Number of global objects : " + nbglobal+"\n";
  theMgrStatus = theMgrStatus+"\t Number of local objects  : " + TCollection_AsciiString (mylocal.Extent())+"  \n";

  return theMgrStatus;
}

//==================================================
// Function: Status
// Purpose :
//==================================================


TCollection_AsciiString SelectMgr_SelectionManager::
Status(const Handle(SelectMgr_SelectableObject)& anObject) const
{
  TCollection_AsciiString TheStatus("\t\tStatus of object:");

  if(myglobal.Contains(anObject))
  {TheStatus += "GLOBAL (available for all viewers in the SelectionManager)\n";}
  else if (mylocal.IsBound(anObject))TheStatus +="LOCAL:\n\t\t"; 
  TColStd_MapIteratorOfMapOfTransient It(myselectors);
  Standard_Integer iv = 0;
  //  Standard_Boolean FirstTime=Standard_True;
  for(;It.More();It.Next()){
    const Handle(SelectMgr_ViewerSelector)& curview = 
      Handle(SelectMgr_ViewerSelector)::DownCast(It.Key());  
    iv++;
    TheStatus = TheStatus + "status in the ViewerSelector :"+TCollection_AsciiString(iv)+"\n\t\t";
    TheStatus+=curview->Status(anObject);
    TheStatus+="\n\t\t----------------------\n\t\t";
  }

  return TheStatus;

}

//==================================================
// Function: LoadMode
// Purpose : Private Method
//==================================================


void SelectMgr_SelectionManager
::LoadMode (const Handle(SelectMgr_SelectableObject)& anObject,
            const Standard_Integer amode)
{
  if(amode==-1) return;
  if(!anObject->HasSelection(amode))
  {
    Handle(SelectMgr_Selection) NewSel = new SelectMgr_Selection(amode); 
    anObject->AddSelection (NewSel,amode);
  }
}


//=======================================================================
//function : SetUpdateMode
//purpose  : 
//=======================================================================

void SelectMgr_SelectionManager::
SetUpdateMode(const Handle(SelectMgr_SelectableObject)& anObject,
              const SelectMgr_TypeOfUpdate aType)
{
  for(anObject->Init();anObject->More();anObject->Next())
    anObject->CurrentSelection()->UpdateStatus(aType);

}

void SelectMgr_SelectionManager::
SetUpdateMode(const Handle(SelectMgr_SelectableObject)& anObject,
              const Standard_Integer aMode,
              const SelectMgr_TypeOfUpdate aType)
{
  if(anObject->HasSelection(aMode))
    anObject->Selection(aMode)->UpdateStatus(aType);
}

