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


#include <Dico_DictionaryOfTransient.hxx>
#include <Dico_IteratorOfDictionaryOfInteger.hxx>
#include <Dico_IteratorOfDictionaryOfTransient.hxx>
#include <IFSelect_DispPerCount.hxx>
#include <IFSelect_DispPerFiles.hxx>
#include <IFSelect_DispPerOne.hxx>
#include <IFSelect_DispPerSignature.hxx>
#include <IFSelect_EditForm.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <IFSelect_GraphCounter.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_ParamEditor.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectShared.hxx>
#include <IFSelect_SelectSharing.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_SignAncestor.hxx>
#include <IFSelect_Signature.hxx>
#include <IFSelect_SignCategory.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IFSelect_SignType.hxx>
#include <IFSelect_SignValidity.hxx>
#include <IFSelect_WorkLibrary.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_Static.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <MoniTool_Option.hxx>
#include <MoniTool_Profile.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_ActorOfFinderProcess.hxx>
#include <Transfer_ActorOfTransientProcess.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_FinderProcess.hxx>
#include <Transfer_SimpleBinderOfTransient.hxx>
#include <Transfer_TransientMapper.hxx>
#include <TransferBRep_ShapeMapper.hxx>
#include <XSControl_ConnectedShapes.hxx>
#include <XSControl_Controller.hxx>
#include <XSControl_SelectForTransfer.hxx>
#include <XSControl_SignTransferStatus.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

//  ParamEditor
//  Transferts
static Handle(Dico_DictionaryOfTransient)& listadapt()
{
  static Handle(Dico_DictionaryOfTransient) listad;
  if (listad.IsNull()) listad = new Dico_DictionaryOfTransient;
  return listad;
}

static TColStd_IndexedMapOfTransient&      mapadapt()
{
  static TColStd_IndexedMapOfTransient     mapad;
  return mapad;
}


    XSControl_Controller::XSControl_Controller
  (const Standard_CString longname, const Standard_CString shortname)
    : theShortName (shortname) , theLongName (longname) 
{
  theAdaptorApplied.Clear();
  theAdaptorHooks = new TColStd_HSequenceOfHAsciiString();

  //  Parametres Standard

  Interface_Static::Standards();
//  TraceStatic ("read.stdsameparameter.mode",5);
  TraceStatic ("read.precision.mode" , 5);
  TraceStatic ("read.precision.val"  , 5);
  TraceStatic ("write.precision.mode" , 6);
  TraceStatic ("write.precision.val"  , 6);

  //  Initialisation du Profile

  theProfile = new MoniTool_Profile;

/*  essai option sur parametre
  Handle(MoniTool_Option) optrdprec = new MoniTool_Option
    (Interface_Static::Static ("read.precision.mode"),"readprecision.mode");
  optrdprec->AddBasic("default","File");
  optrdprec->AddBasic("Session");
  optrdprec->Switch ("default");
  theProfile->AddOption (optrdprec);
*/

//  Handle(MoniTool_Option) optproto = new MoniTool_Option
//    (STANDARD_TYPE(Interface_Protocol),"protocol");
//  theProfile->AddOption (optproto);

  Handle(MoniTool_Option) optsign  = new MoniTool_Option
    (STANDARD_TYPE(IFSelect_Signature),"sign-type");
  optsign->Add ("default",theSignType);
  theProfile->AddOption (optsign);

//  Handle(MoniTool_Option) optwlib  = new MoniTool_Option
//    (STANDARD_TYPE(IFSelect_WorkLibrary),"access");
//  theProfile->AddOption (optwlib);

  Handle(MoniTool_Option) optactrd = new MoniTool_Option
    (STANDARD_TYPE(Transfer_ActorOfTransientProcess),"tr-read");
  theProfile->AddOption (optactrd);

  Handle(MoniTool_Option) optactwr = new MoniTool_Option
    (STANDARD_TYPE(Transfer_ActorOfFinderProcess),"tr-write");
  theProfile->AddOption (optactwr);

//    Definition de la config de base : suite a la customisation
}

    void  XSControl_Controller::SetNames
  (const Standard_CString longname, const Standard_CString shortname)
{
  if (longname && longname[0] != '\0') {
    theLongName.Clear();   theLongName.AssignCat  (longname);
  }
  if (shortname && shortname[0] != '\0') {
    theShortName.Clear();  theShortName.AssignCat (shortname);
  }
}

    void  XSControl_Controller::AutoRecord () const
{
  Record (Name(Standard_True));
  Record (Name(Standard_False));
}

    void  XSControl_Controller::Record (const Standard_CString name) const
{
  Standard_Boolean deja;
  Handle(Standard_Transient)& newadapt = listadapt()->NewItem (name,deja);
  if (deja) {
    Handle(Standard_Transient) thisadapt (this);
    if (newadapt->IsKind(thisadapt->DynamicType())) 
      {
      }
    else if  (thisadapt->IsKind(newadapt->DynamicType()))
      {
	newadapt = this;
	if (mapadapt().FindIndex(newadapt) == 0) mapadapt().Add(newadapt);
      }
    else 
      if (thisadapt != newadapt) Standard_DomainError::Raise
      ("XSControl_Controller : Record");
  }
  else {
    newadapt = this;
    if (mapadapt().FindIndex(newadapt) == 0) mapadapt().Add(newadapt);
  }
}

    Handle(XSControl_Controller)  XSControl_Controller::Recorded
  (const Standard_CString name)
{
  Handle(Standard_Transient) recorded;
  if (!listadapt()->GetItem (name,recorded)) recorded.Nullify();
  return Handle(XSControl_Controller)::DownCast (recorded);
}

    Handle(TColStd_HSequenceOfHAsciiString)  XSControl_Controller::ListRecorded
  (const Standard_Integer mode)
{
  Handle(TColStd_HSequenceOfHAsciiString) list = new TColStd_HSequenceOfHAsciiString();
  if (mode == 0) {
    Dico_IteratorOfDictionaryOfTransient iter (listadapt());
    for (; iter.More(); iter.Next()) {
      Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString
	(iter.Name());
      list->Append(name);
    }
  } else {
    Standard_Integer i, nb = mapadapt().Extent();
    for (i = 1; i <= nb; i ++) {
      DeclareAndCast(XSControl_Controller,ctl,mapadapt().FindKey(i));
      if (ctl.IsNull()) continue;
      Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString
	(ctl->Name( (mode < 0) ));
      list->Append(name);
    }
  }
  return list;
}

    Standard_CString  XSControl_Controller::Name (const Standard_Boolean rsc) const
      {  return (rsc ? theShortName.ToCString() : theLongName.ToCString());  }

//    ####    PROFILE    ####

    Handle(MoniTool_Profile)  XSControl_Controller::Profile () const
      {  return theProfile;  }

    void  XSControl_Controller::DefineProfile
  (const Standard_CString confname)
{
  if (!theProfile->HasConf(confname)) theProfile->AddConf (confname);
  theProfile->SetFromCurrent (confname);
}

    Standard_Boolean  XSControl_Controller::SetProfile
  (const Standard_CString confname)
{
  if (!theProfile->SetCurrent (confname)) return Standard_False;

  Handle(Standard_Transient) anItem;
//  theProfile->Value("protocol",theAdaptorProtocol);
  if (theProfile->Value("sign-type",anItem))
    theSignType = Handle(IFSelect_Signature)::DownCast (anItem);
//  theProfile->Value("access",theAdaptorLibrary);
  if (theProfile->Value("tr-read",anItem))
    theAdaptorRead = Handle(Transfer_ActorOfTransientProcess)::DownCast (anItem);
  if (theProfile->Value("tr-write",anItem))
    theAdaptorWrite = Handle(Transfer_ActorOfFinderProcess)::DownCast (anItem);

  return SettingProfile (confname);
}

    Standard_Boolean  XSControl_Controller::SettingProfile
  (const Standard_CString )
      {  return Standard_True;  }

    Standard_Boolean  XSControl_Controller::ApplyProfile
  (const Handle(XSControl_WorkSession)& WS, const Standard_CString confname)
{
  if (!SetProfile (confname)) return Standard_False;

//   Typed Values : toutes

  theProfile->SetTypedValues();

//   SignType
  Handle(Standard_Transient) signtype;
  theProfile->Value ("sign-type",signtype);
  WS->SetSignType (Handle(IFSelect_Signature)::DownCast (signtype));

//   ActorRead

  Handle(Standard_Transient) actrd;
  theProfile->Value ("tr-read",actrd);
  WS->TransferReader()->SetActor (Handle(Transfer_ActorOfTransientProcess)::DownCast (actrd));

//   ActorWrite : dans le Controller meme

  Handle(Standard_Transient) actwr;
  theProfile->Value ("tr-write",actwr);
  theAdaptorWrite = Handle (Transfer_ActorOfFinderProcess)::DownCast (actwr);

  return ApplyingProfile (WS,confname);
}

    Standard_Boolean  XSControl_Controller::ApplyingProfile
  (const Handle(XSControl_WorkSession)& , const Standard_CString )
    {  return Standard_True;  }

//    ####    DEFINITION    ####

    Handle(Interface_Protocol)  XSControl_Controller::Protocol () const
      {  return theAdaptorProtocol;  }

    Handle(IFSelect_Signature)  XSControl_Controller::SignType () const
      {  return theSignType;  }

    Handle(IFSelect_WorkLibrary)  XSControl_Controller::WorkLibrary () const
      {  return theAdaptorLibrary;  }

    Handle(Transfer_ActorOfFinderProcess)  XSControl_Controller::ActorWrite () const
      {  return theAdaptorWrite;  }

    void  XSControl_Controller::UpdateStatics
  (const Standard_Integer, const Standard_CString ) const
      {  }    // a redefinir si besoin

// ###########################
//  Help du Transfer : controle de valeur + help

    void  XSControl_Controller::SetModeWrite
  (const Standard_Integer modemin, const Standard_Integer modemax,
   const Standard_Boolean )
{
  if (modemin > modemax)  {  theModeWriteShapeN.Nullify(); return;  }
  theModeWriteShapeN = new Interface_HArray1OfHAsciiString (modemin,modemax);
}

    void  XSControl_Controller::SetModeWriteHelp
  (const Standard_Integer modetrans, const Standard_CString help,
   const Standard_Boolean )
{
  if (theModeWriteShapeN.IsNull()) return;
  if (modetrans < theModeWriteShapeN->Lower() ||
      modetrans > theModeWriteShapeN->Upper()) return;
  Handle(TCollection_HAsciiString) hl = new TCollection_HAsciiString (help);
  theModeWriteShapeN->SetValue (modetrans,hl);
}

    Standard_Boolean  XSControl_Controller::ModeWriteBounds
  (Standard_Integer& modemin, Standard_Integer& modemax,
   const Standard_Boolean ) const
{
  modemin = modemax = 0;
  if (theModeWriteShapeN.IsNull()) return Standard_False;
  modemin = theModeWriteShapeN->Lower();
  modemax = theModeWriteShapeN->Upper();
  return Standard_True;
}

    Standard_Boolean  XSControl_Controller::IsModeWrite
  (const Standard_Integer modetrans, const Standard_Boolean ) const
{
  if (theModeWriteShapeN.IsNull()) return Standard_True;
  if (modetrans < theModeWriteShapeN->Lower()) return Standard_False;
  if (modetrans > theModeWriteShapeN->Upper()) return Standard_False;
  return Standard_True;
}

    Standard_CString  XSControl_Controller::ModeWriteHelp
  (const Standard_Integer modetrans, const Standard_Boolean ) const
{
  if (theModeWriteShapeN.IsNull()) return "";
  if (modetrans < theModeWriteShapeN->Lower()) return "";
  if (modetrans > theModeWriteShapeN->Upper()) return "";
  Handle(TCollection_HAsciiString) str = theModeWriteShapeN->Value(modetrans);
  if (str.IsNull()) return "";
  return str->ToCString();
}


// ###########################
//  Transfer : on fait ce qu il faut par defaut (avec ActorWrite)
//    peut etre redefini ...

    Standard_Boolean  XSControl_Controller::RecognizeWriteTransient
  (const Handle(Standard_Transient)& obj,
   const Standard_Integer modetrans) const
{
  if (theAdaptorWrite.IsNull()) return Standard_False;
  theAdaptorWrite->ModeTrans() = modetrans;
  return theAdaptorWrite->Recognize (new Transfer_TransientMapper(obj));
}

//    Fonction interne

static IFSelect_ReturnStatus TransferFinder
  (const Handle(Transfer_ActorOfFinderProcess)& actor,
   const Handle(Transfer_Finder)& mapper,
   const Handle(Transfer_FinderProcess)& FP,
   const Handle(Interface_InterfaceModel)& model,
   const Standard_Integer modetrans)
{
  if (actor.IsNull()) return IFSelect_RetError;
  if (model.IsNull()) return IFSelect_RetError;
  actor->ModeTrans() = modetrans;
  FP->SetModel (model);
  FP->SetActor (actor);
  FP->Transfer (mapper);

  IFSelect_ReturnStatus stat = IFSelect_RetFail;
  Handle(Transfer_Binder) binder = FP->Find (mapper);
  Handle(Transfer_SimpleBinderOfTransient) bindtr;
  while (!binder.IsNull()) {
    bindtr = Handle(Transfer_SimpleBinderOfTransient)::DownCast (binder);
    if (!bindtr.IsNull()) {
      Handle(Standard_Transient) ent = bindtr->Result();
      if (!ent.IsNull()) {
	stat = IFSelect_RetDone;
	model->AddWithRefs (ent);
      }
    }
    binder = binder->NextResult();
  }
  return stat;
}


    IFSelect_ReturnStatus  XSControl_Controller::TransferWriteTransient
  (const Handle(Standard_Transient)& obj,
   const Handle(Transfer_FinderProcess)& FP,
   const Handle(Interface_InterfaceModel)& model,
   const Standard_Integer modetrans) const
{
  if (obj.IsNull()) return IFSelect_RetVoid;
  return TransferFinder
    (theAdaptorWrite,new Transfer_TransientMapper(obj), FP,model,modetrans);
}

    Standard_Boolean  XSControl_Controller::RecognizeWriteShape
  (const TopoDS_Shape& shape,
   const Standard_Integer modetrans) const
{
  if (theAdaptorWrite.IsNull()) return Standard_False;
  theAdaptorWrite->ModeTrans() = modetrans;
  return theAdaptorWrite->Recognize (new TransferBRep_ShapeMapper(shape));
}

    IFSelect_ReturnStatus  XSControl_Controller::TransferWriteShape
  (const TopoDS_Shape& shape,
   const Handle(Transfer_FinderProcess)& FP,
   const Handle(Interface_InterfaceModel)& model,
   const Standard_Integer modetrans) const
{
  if (shape.IsNull()) return IFSelect_RetVoid;

  IFSelect_ReturnStatus theReturnStat = TransferFinder
    (theAdaptorWrite,new TransferBRep_ShapeMapper(shape), FP,model,modetrans);
  return theReturnStat;
}

// ###########################
//  File Cluster : quand un ensemble de donnees n est pas envoye d un coup mais
//    en plusieurs petits paquets ...
//  D abord, on detecte la chose et on prepare un contexte de resolution
//    specifique du cas a traiter. Null Handle si rien a faire (par defaut)
//  Ensuite on resoud
//    Les formules par defaut ne font rien (redefinissables)

    Handle(Standard_Transient)  XSControl_Controller::ClusterContext
  (const Handle(XSControl_WorkSession)& ) const
      {  Handle(Standard_Transient) nulctx;  return nulctx;  }

    Interface_CheckIterator  XSControl_Controller::ResolveCluster
  (const Handle(XSControl_WorkSession)& , const Handle(Standard_Transient)& ) const
      {  Interface_CheckIterator nulist;  return nulist;  }


// ###########################
//  ControlItems : si ActorWrite etc... ne suffisent meme plus, on peut en
//    rajouter, Controller ne fait alors que les accueillir

    void  XSControl_Controller::AddControlItem
  (const Handle(Standard_Transient)& item, const Standard_CString name)
{
  if (item.IsNull() || name[0] == '\0') return;
  if (theItems.IsNull()) theItems = new Dico_DictionaryOfTransient;
  theItems->SetItem (name,item);
}

    Handle(Standard_Transient)  XSControl_Controller::ControlItem
  (const Standard_CString name) const
{
  Handle(Standard_Transient) item;
  if (theItems.IsNull()) return item;
  theItems->GetItem (name,item);
  return item;
}

// ###########################
//  Cutomisation ! On enregistre des Items pour une WorkSession
//     (annule et remplace)
//     Ensuite, on les remet en place a la demande

    void  XSControl_Controller::TraceStatic
  (const Standard_CString name, const Standard_Integer use)
{
  Handle(Interface_Static) val = Interface_Static::Static(name);
  if (val.IsNull()) return;
  theParams.Append (val);
  theParamUses.Append(use);
}

    void  XSControl_Controller::AddSessionItem
  (const Handle(Standard_Transient)& item, const Standard_CString name,
   const Standard_CString setapplied)
{
  if (item.IsNull() || name[0] == '\0') return;
  if (theAdaptorSession.IsNull()) theAdaptorSession =
    new Dico_DictionaryOfTransient;
  theAdaptorSession->SetItem (name,item);
  if (!setapplied || setapplied[0] == '\0') return;
  if (item->IsKind(STANDARD_TYPE(IFSelect_GeneralModifier))) {
//    cout<<" -- Xstep Controller : SetApplied n0."<<theAdaptorApplied.Length()+1
//      <<" Name:"<<name<<endl;
    theAdaptorApplied.Append(item);
    Handle(TCollection_HAsciiString) hook = new TCollection_HAsciiString(setapplied);
    theAdaptorHooks->Append (hook);
  }
}

    Handle(Standard_Transient)  XSControl_Controller::SessionItem
  (const Standard_CString name) const
{
  Handle(Standard_Transient) item;
  if (theAdaptorSession.IsNull()) return item;
  theAdaptorSession->GetItem (name,item);
  return item;
}

    Standard_Boolean  XSControl_Controller::IsApplied
  (const Handle(Standard_Transient)& item) const
{
  if (item.IsNull()) return Standard_False;
  for (Standard_Integer i = theAdaptorApplied.Length(); i >= 1; i --)
    if (item == theAdaptorApplied.Value(i)) return Standard_True;
  return Standard_False;
}

    void  XSControl_Controller::Customise
  ( Handle(XSControl_WorkSession)& WS) 
{
  WS->SetParams (theParams,theParamUses);


//  General
  if(!theAdaptorSession.IsNull()) {
    Dico_IteratorOfDictionaryOfTransient iter(theAdaptorSession);
    for (iter.Start(); iter.More(); iter.Next()) {
      WS->AddNamedItem (iter.Name().ToCString() , iter.Value());
    }
  }
  Customising(WS);
//  Applied Modifiers
  Standard_Integer i, nb = theAdaptorApplied.Length();
  for (i = 1; i <= nb; i ++) {
    Handle(Standard_Transient) anitem = theAdaptorApplied.Value(i);
    Handle(TCollection_HAsciiString) name = WS->Name(anitem);
//    Handle(Message_Messenger) sout = Message::DefaultMessenger();
//    sout<<" --  Customise applied n0."<<i<<" type:"<<anitem->DynamicType()->Name();
//    if (name.IsNull()) sout<<" no name"<<endl;
//    else sout<<" name:"<<name->ToCString()<<endl;
    WS->SetAppliedModifier
      (GetCasted(IFSelect_GeneralModifier,theAdaptorApplied.Value(i)),
       WS->ShareOut() );
  }

//  Editeurs de Parametres
//  Ici car les constructeurs specifiques des controlleurs ont pu creer des
//  Parametres : attendre donc ici

  Handle(TColStd_HSequenceOfHAsciiString) listat = Interface_Static::Items();
  Handle(IFSelect_ParamEditor) paramed =
    IFSelect_ParamEditor::StaticEditor (listat,"All Static Parameters");
  WS->AddNamedItem ("xst-static-params-edit",paramed);
  Handle(IFSelect_EditForm) paramform = paramed->Form(Standard_False);
  WS->AddNamedItem ("xst-static-params",paramform);
  
//   Norm Specific
  //Customising (WS);

//   Loading Options of the Profile

//   Available Signatures
  Handle(MoniTool_Option) optsign  = theProfile->Option ("sign-type");
//  Handle(TColStd_HSequenceOfHAsciiString) signs =
//    WS->ItemNames (STANDARD_TYPE(IFSelect_Signature));
//  Standard_Integer isign, nbsign = (signs.IsNull() ? 0 : signs->Length());
//  for (isign = 1; isign <= nbsign; isign ++) {
//    Handle(TCollection_HAsciiString) signame = signs->Value(isign);
//    Handle(Standard_Transient) asign = WS->NamedItem (signame);
//    optsign->Add (signame->ToCString(),asign);
//  }
  optsign->Add ("default",theSignType);   // defaut specifique
  optsign->Switch ("default");  // garder courante la definition par defaut !

//   Actor Read
  Handle(MoniTool_Option) optacrd  = theProfile->Option ("tr-read");
  optacrd->Add ("default",theAdaptorRead);
  optacrd->Switch ("default");

//   Actor Write
  Handle(MoniTool_Option) optacwr  = theProfile->Option ("tr-write");
  optacwr->Add ("default",theAdaptorWrite);
  optacwr->Switch ("default");

//   Basic configuration

  theProfile->AddConf ("Base");
  theProfile->AddSwitch ("Base","sign-type","default");
  theProfile->AddSwitch ("Base","tr-read","default");
  theProfile->AddSwitch ("Base","tr-write","default");
  theProfile->SetCurrent ("Base");
}

    void  XSControl_Controller::Customising
  ( Handle(XSControl_WorkSession)& WS)    
{
   //ndle(IFSelect_SelectModelRoots)    slr = new IFSelect_SelectModelRoots;
 ///WS->AddNamedItem ("xst-model-roots",slr);
  if(!WS->NamedItem("xst-model-all").IsNull()) return;
  Handle(IFSelect_SelectModelEntities) sle = new IFSelect_SelectModelEntities;
  WS->AddNamedItem ("xst-model-all",sle);
  Handle(IFSelect_SelectModelRoots)    slr;
  slr = new IFSelect_SelectModelRoots;
  WS->AddNamedItem ("xst-model-roots",slr);
  if(strcasecmp(WS->SelectedNorm(),"STEP")) {
    Handle(XSControl_SelectForTransfer) st1 = new XSControl_SelectForTransfer;
    st1->SetInput (slr);
    st1->SetReader (WS->TransferReader());
    WS->AddNamedItem ("xst-transferrable-roots",st1);
    
  }
  //else slr = Handle(IFSelect_SelectModelRoots)::DownCast(WS->NamedItem("xst-model-roots"));
  Handle(XSControl_SelectForTransfer) st2 = new XSControl_SelectForTransfer;
  st2->SetInput (sle);
  st2->SetReader (WS->TransferReader());
  WS->AddNamedItem ("xst-transferrable-all",st2);
   
  Handle(XSControl_SignTransferStatus) strs = new XSControl_SignTransferStatus;
   strs->SetReader (WS->TransferReader());
  WS->AddNamedItem ("xst-transfer-status",strs);
  
  Handle(XSControl_ConnectedShapes) scs = new XSControl_ConnectedShapes;
  scs->SetReader (WS->TransferReader());
  WS->AddNamedItem ("xst-connected-faces",scs);

  Handle(IFSelect_SignType) stp = new IFSelect_SignType (Standard_False);
  WS->AddNamedItem ("xst-long-type",stp);
  Handle(IFSelect_SignType) stc = new IFSelect_SignType (Standard_True);
  WS->AddNamedItem ("xst-type",stc);
  Handle(IFSelect_SignAncestor) sta = new IFSelect_SignAncestor;
  WS->AddNamedItem ("xst-ancestor-type",sta);
  Handle(IFSelect_SignCounter) tc1 =
    new IFSelect_SignCounter(stp,Standard_False,Standard_True);
  WS->AddNamedItem ("xst-types",tc1);
  Handle(IFSelect_SignCategory) sca = new IFSelect_SignCategory;
  WS->AddNamedItem ("xst-category",sca);
  Handle(IFSelect_SignValidity) sva = new IFSelect_SignValidity;
  WS->AddNamedItem ("xst-validity",sva);

  Handle(IFSelect_DispPerOne) dispone = new IFSelect_DispPerOne;
  dispone->SetFinalSelection(slr);
  WS->AddNamedItem ("xst-disp-one",dispone);
  Handle(IFSelect_DispPerCount) dispcount = new IFSelect_DispPerCount;
  Handle(IFSelect_IntParam) intcount = new IFSelect_IntParam;
  intcount->SetValue(5);
  dispcount->SetCount(intcount);
  dispcount->SetFinalSelection(slr);
  WS->AddNamedItem ("xst-disp-count",dispcount);
  Handle(IFSelect_DispPerFiles) dispfiles = new IFSelect_DispPerFiles;
  Handle(IFSelect_IntParam) intfiles = new IFSelect_IntParam;
  intfiles->SetValue(10);
  dispfiles->SetCount(intfiles);
  dispfiles->SetFinalSelection(slr);
  WS->AddNamedItem ("xst-disp-files",dispfiles);
  Handle(IFSelect_DispPerSignature) dispsign = new IFSelect_DispPerSignature;
  dispsign->SetSignCounter(new IFSelect_SignCounter(stc));
  dispsign->SetFinalSelection(slr);
  WS->AddNamedItem ("xst-disp-sign",dispsign);

//   Pas utilisables directement mais bien utiles quand meme
  WS->AddNamedItem ("xst-pointed",new IFSelect_SelectPointed);
  WS->AddNamedItem ("xst-sharing",new IFSelect_SelectSharing);
  WS->AddNamedItem ("xst-shared",new IFSelect_SelectShared);
  WS->AddNamedItem ("xst-nb-selected",new IFSelect_GraphCounter);
  theSignType      = stp;
    // au moins cela
}
 Handle(Dico_DictionaryOfTransient) XSControl_Controller::AdaptorSession() const
{
  return theAdaptorSession;
}
