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

//:i1 pdn 03.04.99  BUC60301  

#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <Geom2d_Point.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_HGraph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_IntVal.hxx>
#include <Interface_Macros.hxx>
#include <Message_Messenger.hxx>
#include <MoniTool_Profile.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_Finder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_ResultFromModel.hxx>
#include <Transfer_ResultFromTransient.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_TransferWriter.hxx>
#include <XSControl_Vars.hxx>
#include <XSControl_WorkSession.hxx>

// tpent
//=======================================================================
//function : XSControl_WorkSession
//purpose  : 
//=======================================================================
XSControl_WorkSession::XSControl_WorkSession ()
{
  theModeWriteShape = 0;
  theTransferRead  = new XSControl_TransferReader;
  theTransferWrite = new XSControl_TransferWriter;
  theVars = new XSControl_Vars;
}


//=======================================================================
//function : ClearData
//purpose  : 
//=======================================================================

void  XSControl_WorkSession::ClearData (const Standard_Integer mode)
{
  // 1-2-3-4 : standard IFSelect
  if (mode >= 1 && mode <= 4) IFSelect_WorkSession::ClearData (mode);

  // 5 : Transferts seuls
  // 6 : Resultats forces seuls
  // 7 : Management, y compris tous transferts (forces/calcules), views

  if (mode == 5 || mode == 7) {
    theTransferRead->Clear(-1);
    theTransferWrite->Clear(-1);
  }
  if (mode == 6 && !theTransferRead.IsNull()) theTransferRead->Clear(1);
  theTransferRead->SetGraph (HGraph());
}


//=======================================================================
//function : SelectNorm
//purpose  : 
//=======================================================================

Standard_Boolean  XSControl_WorkSession::SelectNorm(const Standard_CString normname,
                                                    const Standard_CString profile)
{
  //   RAZ ancienne norme  et resultats
  theTransferRead->Clear(-1);
  //  ????  En toute rigueur, menage a faire dans XWS : virer les items
  //        ( a la limite, pourquoi pas, refaire XWS en entier)

  Handle(XSControl_Controller) newadapt =
    XSControl_Controller::Recorded (normname);
  if (newadapt.IsNull()) return Standard_False;
  if (newadapt == theController) return Standard_True;
  SetController (newadapt);
  if (profile && profile[0] != '\0') newadapt->Profile()->SetCurrent(profile);

//  cout<<"##########################\n"
//      <<"##  Select Norm : "<<normname<<"\n"
//      <<"##########################"<<endl;
  return Standard_True;
}


//=======================================================================
//function : SelectProfile
//purpose  : 
//=======================================================================

Standard_Boolean  XSControl_WorkSession::SelectProfile(const Standard_CString profile)
{
  return theController->Profile()->SetCurrent(profile);
}


//=======================================================================
//function : SetController
//purpose  : 
//=======================================================================

void  XSControl_WorkSession::SetController(const Handle(XSControl_Controller)& ctl)
{
  theController = ctl;

  SetLibrary   ( theController->WorkLibrary() );
  SetProtocol  ( theController->Protocol() );
  ClearItems();  ClearFinalModifiers();
  ClearShareOut(Standard_False);  ClearFile();
  Handle(XSControl_WorkSession) aWorkSession = this;
  theController->Customise (aWorkSession);
  SetSignType  ( theController->SignType() );
  theTransferRead->SetController (theController);
  theTransferWrite->SetController (theController);

  AdaptNorm ();
}


//=======================================================================
//function : AdaptNorm
//purpose  : 
//=======================================================================

void XSControl_WorkSession::AdaptNorm ()
{
}


//=======================================================================
//function : SelectedNorm
//purpose  : 
//=======================================================================

Standard_CString XSControl_WorkSession::SelectedNorm(const Standard_Boolean rsc) const
{
  //JR/Hp :
  Standard_CString astr = (Standard_CString ) (theController.IsNull() ? "" : theController->Name(rsc));
  return astr ;
}
//      {  return (theController.IsNull() ? "" : theController->Name(rsc));  }


//=======================================================================
//function : NormAdaptor
//purpose  : 
//=======================================================================

Handle(XSControl_Controller) XSControl_WorkSession::NormAdaptor () const
{
  return theController;
}


//              ##########################################
//              ############  Contexte de Transfert ######
//              ##########################################


//=======================================================================
//function : Context
//purpose  : 
//=======================================================================

Handle(Dico_DictionaryOfTransient) XSControl_WorkSession::Context () const
{
  return theContext;
}


//=======================================================================
//function : SetAllContext
//purpose  : 
//=======================================================================

void XSControl_WorkSession::SetAllContext(const Handle(Dico_DictionaryOfTransient)& context)
{
  theContext = context;
  theTransferRead->Context() = context;
}


//=======================================================================
//function : ClearContext
//purpose  : 
//=======================================================================

void XSControl_WorkSession::ClearContext ()
{
  Handle(Dico_DictionaryOfTransient) nulctx;
  SetAllContext(nulctx);
}


//              ##########################################
//              ############    RESULTATS FORCES    ######
//              ##########################################


//=======================================================================
//function : PrintTransferStatus
//purpose  : 
//=======================================================================

Standard_Boolean  XSControl_WorkSession::PrintTransferStatus(const Standard_Integer num,
                                                             const Standard_Boolean wri,
                                                             const Handle(Message_Messenger)& S) const
{
  Handle(Transfer_FinderProcess)    FP = MapWriter();
  Handle(Transfer_TransientProcess) TP = MapReader();

  Handle(Transfer_Binder) binder;
  Handle(Transfer_Finder) finder;
  Handle(Standard_Transient) ent;

  //   ***   WRITE  ***
  if (wri) {
    if (FP.IsNull()) return Standard_False;
    if (num == 0 ) return Standard_False;

    Standard_Integer ne=0, nr=0, max = FP->NbMapped() ,maxr = FP->NbRoots();
    if (num > 0) {
      if (num > max) return Standard_False;
      ne = num;
      finder = FP->Mapped(ne);
      nr = FP->RootIndex(finder);
    } else if (num < 0) {
      nr = -num;
      if (nr > maxr) return Standard_False;
      finder = FP->Root(nr);
      ne  = FP->MapIndex(finder);
    }

    S<<"Transfer Write item n0."<<ne<<" of "<<max;
    if (nr > 0) S<<"  ** Transfer Root n0."<<ne;  S<<endl;
    ent = FP->FindTransient(finder);
    S<<" -> Type "<<finder->DynamicType()->Name()<<endl;
    FP->StartTrace (binder,finder,0,0);  // pb sout/S
    if (!ent.IsNull()) {
      S<<" ** Resultat Transient, type "<<ent->DynamicType()->Name();
      Handle(Interface_InterfaceModel) model = Model();
      if (!model.IsNull())
	{  S<<" In output Model, Entity ";  model->Print(ent,S);  }
      S<<endl;
    }
  }

  //    ***   READ   ***
  else {
    if (TP.IsNull()) return Standard_False;
    Handle(Interface_InterfaceModel) model = TP->Model();
    if (model.IsNull()) cout<<"No Model"<<endl;
    else if (model != Model()) cout<<"Model different from the session"<<endl;
    if (num == 0) return Standard_False;

    Standard_Integer  ne=0, nr=0, max = TP->NbMapped() ,maxr = TP->NbRoots();
    if (num > 0) {
      if (num > max) return Standard_False;
      ne = num;
      ent = TP->Mapped(ne);
      nr = TP->RootIndex(finder);
    } else if (num < 0) {
      nr = -num;
      if (nr > maxr) return Standard_False;
      ent = TP->Root(nr);
      ne  = TP->MapIndex(ent);
    }

    S<<"Transfer Read item n0."<<ne<<" of "<<max;
    if (nr > 0) S<<"  ** Transfer Root n0."<<ne;  S<<endl;
    if (!model.IsNull())  {  S<<" In Model, Entity ";  model->Print(ent,S); }
    binder = TP->MapItem (ne);
    S<<endl;
    TP->StartTrace (binder,ent,0,0);

  }

//   ***   CHECK (commun READ+WRITE)   ***
  if (!binder.IsNull()) {
    const Handle(Interface_Check) ch = binder->Check();
    Standard_Integer i,nbw = ch->NbWarnings(), nbf = ch->NbFails();
    if (nbw > 0) {
      S<<" - Warnings : "<<nbw<<" :\n";
      for (i = 1; i <= nbw; i ++) S<<ch->CWarning(i)<<endl;
    }
    if (nbf > 0) {
      S<<" - Fails : "<<nbf<<" :\n";
      for (i = 1; i <= nbf; i ++) S<<ch->CFail(i)<<endl;
    }
  }
  return Standard_True;
}


//=======================================================================
//function : InitTransferReader
//purpose  : 
//=======================================================================

void  XSControl_WorkSession::InitTransferReader(const Standard_Integer mode)
{
  if (mode == 0 || mode == 5)  theTransferRead->Clear(-1);  // full clear
  if (theTransferRead.IsNull()) SetTransferReader (new XSControl_TransferReader);
  else SetTransferReader (theTransferRead);

  // mode = 0 fait par SetTransferReader suite a Nullify
  if (mode == 1) {
    if (!theTransferRead.IsNull()) theTransferRead->Clear(-1);
    else SetTransferReader (new XSControl_TransferReader);
  }
  if (mode == 2) {
    Handle(Transfer_TransientProcess) TP = theTransferRead->TransientProcess();
    if (TP.IsNull()) {
      TP = new Transfer_TransientProcess;
      theTransferRead->SetTransientProcess(TP);
      TP->SetGraph (HGraph());
    }
    Handle(TColStd_HSequenceOfTransient) lis = theTransferRead->RecordedList();
    Standard_Integer i, nb = lis->Length();
    for (i = 1; i <= nb; i ++) TP->SetRoot(lis->Value(i));
  }
  if (mode == 3) {
    Handle(Transfer_TransientProcess) TP = theTransferRead->TransientProcess();
    if (TP.IsNull()) return;
    Standard_Integer i, nb = TP->NbRoots();
    for (i = 1; i <= nb; i ++) theTransferRead->RecordResult(TP->Root(i));
  }
  if (mode == 4 || mode == 5) theTransferRead->BeginTransfer();
}


//=======================================================================
//function : SetTransferReader
//purpose  : 
//=======================================================================

void XSControl_WorkSession::SetTransferReader(const Handle(XSControl_TransferReader)& TR)
{
  if (theTransferRead != TR) //i1 pdn 03.04.99 BUC60301
    theTransferRead = TR;
  if (TR.IsNull()) return;
  TR->SetController (theController);
  TR->SetGraph (HGraph());
  if (!TR->TransientProcess().IsNull()) return;
  Handle(Transfer_TransientProcess) TP = new Transfer_TransientProcess
    (Model().IsNull() ? 100 : Model()->NbEntities() + 100);
  TP->SetGraph (HGraph());
  TP->SetErrorHandle(Standard_True);
  TR->SetTransientProcess(TP);
}


//=======================================================================
//function : TransferReader
//purpose  : 
//=======================================================================

Handle(XSControl_TransferReader) XSControl_WorkSession::TransferReader () const
{
  return theTransferRead;
}


//=======================================================================
//function : MapReader
//purpose  : 
//=======================================================================

Handle(Transfer_TransientProcess) XSControl_WorkSession::MapReader () const
{
  return theTransferRead->TransientProcess();
}


//=======================================================================
//function : SetMapReader
//purpose  : 
//=======================================================================

Standard_Boolean  XSControl_WorkSession::SetMapReader
  (const Handle(Transfer_TransientProcess)& TP)
{
  if ( TP.IsNull())  return Standard_False;
  if (TP->Model().IsNull()) TP->SetModel (Model());
  TP->SetGraph (HGraph());
  if (TP->Model() != Model()) return Standard_False;
//  TR ne doit pas bouger, c est un "crochet" pour signatures, selections ...
//  En revanche, mieux vaut le RAZ
//  Handle(XSControl_TransferReader) TR = new XSControl_TransferReader;
  Handle(XSControl_TransferReader) TR = theTransferRead;
  TR->Clear(-1);

  SetTransferReader (TR);        // avec le meme mais le reinitialise
  TR->SetTransientProcess (TP);  // et prend le nouveau TP
  return Standard_True;
}


//=======================================================================
//function : Result
//purpose  : 
//=======================================================================

Handle(Standard_Transient)  XSControl_WorkSession::Result
  (const Handle(Standard_Transient)& ent, const Standard_Integer mode) const
{
  Standard_Integer ouca = (mode % 10);
  Standard_Integer kica = (mode / 10);

  Handle(Transfer_Binder) binder;
  Handle(Transfer_ResultFromModel) resu;

  if (ouca !=  1) resu = theTransferRead->FinalResult(ent);
  if (mode == 20) return resu;

  if (!resu.IsNull()) binder = resu->MainResult()->Binder();
  if (binder.IsNull() && ouca > 0)
    binder = theTransferRead->TransientProcess()->Find(ent);

  if (kica == 1) return binder;
  DeclareAndCast(Transfer_SimpleBinderOfTransient,trb,binder);
  if (!trb.IsNull()) return trb->Result();
  return binder;
}

//              ##########################################
//              ############    TRANSFERT    #############
//              ##########################################


//=======================================================================
//function : TransferReadOne
//purpose  : 
//=======================================================================

Standard_Integer  XSControl_WorkSession::TransferReadOne
  (const Handle(Standard_Transient)& ent)
{
  Handle(Interface_InterfaceModel) model = Model();
  if (ent == model) return TransferReadRoots();

  Handle(TColStd_HSequenceOfTransient) list = GiveList(ent);
  if (list->Length() == 1) return theTransferRead->TransferOne(list->Value(1));
  else return theTransferRead->TransferList (list);
}


//=======================================================================
//function : TransferReadRoots
//purpose  : 
//=======================================================================

Standard_Integer  XSControl_WorkSession::TransferReadRoots ()
{
  return theTransferRead->TransferRoots(Graph());
}


//              ##########################################
//              ############    TRANSFERT  WRITE
//              ##########################################

//=======================================================================
//function : NewModel
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) XSControl_WorkSession::NewModel ()
{
  Handle(Interface_InterfaceModel) newmod;
  if (theController.IsNull()) return newmod;
  newmod = theController->NewModel();
  
  SetModel(newmod);
  if(!MapReader().IsNull())
    MapReader()->Clear();
  //clear all contains of WS
  theTransferRead->Clear(3);
  theTransferWrite->Clear(-1);

  return newmod;
}


//=======================================================================
//function : TransferWriter
//purpose  : 
//=======================================================================

Handle(XSControl_TransferWriter) XSControl_WorkSession::TransferWriter () const
{
  return theTransferWrite;
}


//=======================================================================
//function : MapWriter
//purpose  : 
//=======================================================================

Handle(Transfer_FinderProcess) XSControl_WorkSession::MapWriter () const
{
  return theTransferWrite->FinderProcess();
}


//=======================================================================
//function : SetMapWriter
//purpose  : 
//=======================================================================

Standard_Boolean XSControl_WorkSession::SetMapWriter
  (const Handle(Transfer_FinderProcess)& FP)
{
  if ( FP.IsNull())  return Standard_False;
  theTransferWrite->SetFinderProcess (FP);
  return Standard_True;
}


//=======================================================================
//function : SetModeWriteShape
//purpose  : 
//=======================================================================

void XSControl_WorkSession::SetModeWriteShape(const Standard_Integer mode)
{
  theTransferWrite->SetTransferMode(mode);
}


//=======================================================================
//function : ModeWriteShape
//purpose  : 
//=======================================================================

Standard_Integer XSControl_WorkSession::ModeWriteShape () const
{
  return theTransferWrite->TransferMode();
}


//=======================================================================
//function : TransferWriteShape
//purpose  : 
//=======================================================================

IFSelect_ReturnStatus XSControl_WorkSession::TransferWriteShape
  (const TopoDS_Shape& shape, const Standard_Boolean compgraph)
{
  IFSelect_ReturnStatus  status;
  if (theController.IsNull()) return IFSelect_RetError;
  Handle(Interface_InterfaceModel) model = Model();
  if (model.IsNull()) return IFSelect_RetVoid;

  status = theTransferWrite->TransferWriteShape (model,shape);
  //  qui s occupe de tout, try/catch inclus

  //skl insert param compgraph for XDE writing 10.12.2003
  if(compgraph) ComputeGraph(Standard_True);

  return status;
}


//=======================================================================
//function : TransferWriteCheckList
//purpose  : 
//=======================================================================

Interface_CheckIterator XSControl_WorkSession::TransferWriteCheckList () const
{
  return theTransferWrite->ResultCheckList (Model());
}


//=======================================================================
//function : Vars
//purpose  : 
//=======================================================================

Handle(XSControl_Vars) XSControl_WorkSession::Vars () const
{
  return theVars;
}


//=======================================================================
//function : SetVars
//purpose  : 
//=======================================================================

void XSControl_WorkSession::SetVars (const Handle(XSControl_Vars)& newvars)
{
  theVars = newvars;
}


//=======================================================================
//function : ClearBinders
//purpose  : 
//=======================================================================

void XSControl_WorkSession::ClearBinders()
{
  Handle(Transfer_FinderProcess) FP = theTransferWrite->FinderProcess();
  //Due to big number of chains of binders it is necessary to 
  //collect head binders of each chain in the sequence
  TColStd_SequenceOfTransient aSeqBnd;
  TColStd_SequenceOfTransient aSeqShapes;
  Standard_Integer i =1;
  for( ; i <= FP->NbMapped();i++) {
    Handle(Transfer_Binder) bnd = FP->MapItem ( i );
    if(!bnd.IsNull())
      aSeqBnd.Append(bnd);
    Handle(Standard_Transient) ash (FP->Mapped(i));
    aSeqShapes.Append(ash);
  }
  //removing finder process containing result of translation.
  FP->Clear();
  ClearData(1);
  ClearData(5);
  
  //removing each chain of binders
  while(aSeqBnd.Length() >0) {
    Handle(Transfer_Binder) aBnd = Handle(Transfer_Binder)::DownCast(aSeqBnd.Value(1));
    Handle(Standard_Transient) ash =aSeqShapes.Value(1);
    aSeqBnd.Remove(1);
    aSeqShapes.Remove(1);
    ash.Nullify();
    while(!aBnd.IsNull()) {
      Handle(Transfer_Binder) aBndNext = aBnd->NextResult();
      aBnd.Nullify();
      aBnd = aBndNext;
    }
    
  }

}


//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void XSControl_WorkSession::Destroy()
{
  ClearBinders();
}


