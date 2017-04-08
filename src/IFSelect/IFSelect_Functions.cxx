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

//#58 rln 28.12.98 Versioning

#include <IFSelect_Act.hxx>
#include <IFSelect_CheckCounter.hxx>
#include <IFSelect_Dispatch.hxx>
#include <IFSelect_DispGlobal.hxx>
#include <IFSelect_DispPerCount.hxx>
#include <IFSelect_DispPerFiles.hxx>
#include <IFSelect_DispPerOne.hxx>
#include <IFSelect_DispPerSignature.hxx>
#include <IFSelect_EditForm.hxx>
#include <IFSelect_Editor.hxx>
#include <IFSelect_Functions.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <IFSelect_GraphCounter.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_ListEditor.hxx>
#include <IFSelect_Modifier.hxx>
#include <IFSelect_ModifReorder.hxx>
#include <IFSelect_SelectDeduct.hxx>
#include <IFSelect_SelectDiff.hxx>
#include <IFSelect_SelectEntityNumber.hxx>
#include <IFSelect_SelectErrorEntities.hxx>
#include <IFSelect_SelectIncorrectEntities.hxx>
#include <IFSelect_SelectIntersection.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectRange.hxx>
#include <IFSelect_SelectRoots.hxx>
#include <IFSelect_SelectShared.hxx>
#include <IFSelect_SelectSharing.hxx>
#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_SelectSuite.hxx>
#include <IFSelect_SelectUnion.hxx>
#include <IFSelect_SelectUnknownEntities.hxx>
#include <IFSelect_SessionFile.hxx>
#include <IFSelect_SessionPilot.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_Signature.hxx>
#include <IFSelect_SignatureList.hxx>
#include <IFSelect_SignCounter.hxx>
#include <IFSelect_SignType.hxx>
#include <IFSelect_Transformer.hxx>
#include <IFSelect_WorkLibrary.hxx>
#include <IFSelect_WorkSession.hxx>
#include <Interface_Category.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_Static.hxx>
#include <Interface_Version.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <stdio.h>
//  Decomposition of a file name in its parts : prefix, root, suffix
static void SplitFileName
  (const Standard_CString filename,
   TCollection_AsciiString& prefix,
   TCollection_AsciiString& fileroot,
   TCollection_AsciiString& suffix)
{
  Standard_Integer nomdeb, nomfin, nomlon;
  TCollection_AsciiString resfile (filename);
  nomlon = resfile.Length();
  nomdeb = resfile.SearchFromEnd ("/");
  if (nomdeb <= 0) nomdeb = resfile.SearchFromEnd("\\");  // pour NT
  if (nomdeb <  0) nomdeb = 0;
  nomfin = resfile.SearchFromEnd (".");
  if (nomfin < nomdeb) nomfin = nomlon + 1;

  if (nomdeb > 0) prefix = resfile.SubString (1,nomdeb);
  fileroot = resfile.SubString(nomdeb+1,nomfin-1);
  if (nomfin <= nomlon) suffix = resfile.SubString (nomfin,nomlon);
}



//  Functions definit un certain nombre de commandes
//  enregistrees dans le Dictionnaire de Activator (par des Act unitaires)
//  Les actions elles-memes sont regroupees en fin de fichier

//  Les definitions

static IFSelect_ReturnStatus funstatus
  (const Handle(IFSelect_SessionPilot)& )
{
//        ****    Version & cie     ****
  //#58 rln
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  sout<<"Processor Version : "<<XSTEP_PROCESSOR_VERSION<<endl;
  sout<<"OL Version        : "<<XSTEP_SYSTEM_VERSION<<endl;
  sout<<"Configuration     : "<<XSTEP_Config<<endl;
  sout<<"UL Names          : "<<XSTEP_ULNames<<endl;
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun1
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    ToggleHandler     ****
  Standard_Boolean hand = !WS->ErrorHandle();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (hand) sout << " --  Mode Catch Error now Active"   <<endl;
  else      sout << " --  Mode Catch Error now Inactive" <<endl;
  WS->SetErrorHandle(hand);
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun3
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    XRead / Load         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Read/Load : give file name !"<<endl; return IFSelect_RetError; }
  if (WS->Protocol().IsNull()) { sout<<"Protocol not defined"<<endl; return IFSelect_RetError; }
  if (WS->WorkLibrary().IsNull()) { sout<<"WorkLibrary not defined"<<endl; return IFSelect_RetError; }

  IFSelect_ReturnStatus status = WS->ReadFile (arg1);
// status : 0 OK, 1 erreur lecture, 2 Fail(try/catch),
//          -1 fichier non trouve, -2 lecture faite mais resultat vide
  switch (status) {
    case IFSelect_RetVoid  : sout<<"file:"<<arg1<<" gives empty result"<<endl; break;
    case IFSelect_RetError : sout<<"file:"<<arg1<<" could not be opened"<<endl; break;
    case IFSelect_RetDone  : sout<<"file:"<<arg1<<" read"<<endl; break;
    case IFSelect_RetFail  : sout<<"file:"<<arg1<<" : error while reading"<<endl; break;
    case IFSelect_RetStop  : sout<<"file:"<<arg1<<" : EXCEPTION while reading"<<endl; break;
    default : sout<<"file:"<<arg1<<" could not be read"<<endl; break;
  }
  if (status != IFSelect_RetDone) return status;
//      sout<<" - clearing list of already written files"<<endl;
  WS->BeginSentFiles(Standard_True);
  return status;
}

static IFSelect_ReturnStatus fun4
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Write All         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Write All : give file name !"<<endl; return IFSelect_RetError; }
  return WS->SendAll (arg1);
}

static IFSelect_ReturnStatus fun5
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//  const Standard_CString arg2 = pilot->Arg(2);
//        ****    Write Selected         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Write Selected : give file name + givelist !"<<endl; return IFSelect_RetError; }
  Handle(TColStd_HSequenceOfTransient) result =
    IFSelect_Functions::GiveList (WS,pilot->CommandPart( 2));
  if (result.IsNull()) { sout<<"No entity selected"<<endl; return IFSelect_RetError; }
  else sout<<"Nb Entities selected : "<<result->Length()<<endl;
  Handle(IFSelect_SelectPointed) sp = new IFSelect_SelectPointed;
  sp->SetList (result);
  return WS->SendSelected (arg1,sp);
}

static IFSelect_ReturnStatus fun6
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Write Entite(s)         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Write Entitie(s) : give file name + n0s entitie(s)!"<<endl; return IFSelect_RetError; }
  int ko = 0;
  Handle(IFSelect_SelectPointed) sp = new IFSelect_SelectPointed;
  for (Standard_Integer ia = 2; ia < argc ; ia ++) {
    Standard_Integer id = pilot->Number(pilot->Arg(ia));
    if (id > 0) {
      Handle(Standard_Transient) item = WS->StartingEntity(id);
      if (sp->Add(item)) sout<<"Added:no."<<id<<endl;
      else { sout<<" Fail Add n0."<<id<<endl; ko ++; }
    }
    else { sout<<"Not an entity number:"<<pilot->Arg(ia)<<endl; ko ++; }
  }
  if (ko > 0) { sout<<ko<<" bad arguments, abandon"<<endl; return IFSelect_RetError; }
  return WS->SendSelected (arg1,sp);
}

static IFSelect_ReturnStatus fun7
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Entity Label       ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give entity number"<<endl; return IFSelect_RetError; }
  if (!WS->HasModel()) { sout<<"No loaded model, abandon"<<endl; return IFSelect_RetError; }
  Standard_Integer nument = WS->NumberFromLabel (arg1);
  if (nument <= 0 || nument > WS->NbStartingEntities())
    { sout<<"Not a suitable number: "<<arg1<<endl;  return IFSelect_RetError; }
  sout<<"N0."<<nument<<" ->Label in Model : ";
  WS->Model()->PrintLabel(WS->StartingEntity(nument),sout);  
  sout<<endl;
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun8
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Entity Number      ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give label to search"<<endl; return IFSelect_RetError; }
  if (!WS->HasModel()) { sout<<"No loaded model, abandon"<<endl; return IFSelect_RetError; }
  const Handle(Interface_InterfaceModel) &model = WS->Model();
  Standard_Integer i, cnt = 0;
  Standard_Boolean exact = Standard_False;
  sout<<" **  Search Entity Number for Label : "<<arg1<<endl;
  for (i = model->NextNumberForLabel (arg1, 0, exact)  ; i != 0;
       i = model->NextNumberForLabel (arg1, i, exact)) {
    cnt ++;
    sout<<" **  Found n0/id:"; model->Print (model->Value(i),sout); sout<<endl;
  }
  
  if (cnt == 0) sout<<" **  No Match"<<endl;
  else if (cnt == 1) sout<<" **  1 Match"<<endl;
  else sout<<cnt<<" Matches"<<endl;
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun9
  (const Handle(IFSelect_SessionPilot)& pilot)
{
//        ****    List Types         ****
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Handle(IFSelect_Signature) signtype = WS->SignType();
  if (signtype.IsNull()) signtype = new IFSelect_SignType;
  Handle(IFSelect_SignCounter) counter =
    new IFSelect_SignCounter(signtype,Standard_False);
  return pilot->ExecuteCounter (counter,1);
}

static IFSelect_ReturnStatus funcount
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg0 = pilot->Arg(0);
  const Standard_CString arg1 = pilot->Arg(1);
  Standard_Boolean listmode = (arg0[0] == 'l');
//        ****    List Counter         ****

  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    sout<<"Designer signature ou compteur, + facultatif selection + facultatif entite"<<endl;
    sout<<" signature/compteur seul -> tout le modele"<<endl
      <<  " sign/compteur + selection -> cette selection, evaluation normale"<<endl
	<<" sign/compteur + sel + num -> cette selection evaluee sur entite n0 num"<<endl;
    return IFSelect_RetError;
  }
  DeclareAndCast(IFSelect_SignCounter,counter,WS->NamedItem(arg1));
  if (counter.IsNull()) {
    DeclareAndCast(IFSelect_Signature,signa,WS->NamedItem(arg1));
    if (!signa.IsNull()) counter = new IFSelect_SignCounter(signa,Standard_False,listmode);
  }
//  Handle(IFSelect_Selection) sel;
//  Standard_Integer n3 = 0;  if (argc > 3) n3 = WS->NumberFromLabel(arg3);
//  if (argc > 2) sel = GetCasted(IFSelect_Selection,WS->NamedItem(arg2));
//  if (counter.IsNull() || (argc > 2 && n3 <= 0 && sel.IsNull()) ) {
//    sout<<"Nom:"<<arg1; if (argc > 2) sout<<" et/ou "<<arg2;
//    sout<<" incorrect (demande: compteur ou signature [selection])"<<endl;
//    return IFSelect_RetError;
//  }

//  Ajout : si Selection, on applique un GraphCounter
//   Et en ce cas, on peut en avoir plusieurs : la limite est le mot-cle "on"
  Standard_Integer onflag = 0;
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 2; i < argc; i ++) {
    if (!strcmp (pilot->Arg(i),"on")) { onflag = i; break; }
  }

  Handle(IFSelect_Selection) sel = WS->GiveSelection(arg1);
  DeclareAndCast(IFSelect_SelectDeduct,seld,sel);
  if (!seld.IsNull()) {
//  Si onflag, faire une SelectSuite
    if (onflag > 2) {
      Handle(IFSelect_SelectSuite) suite = new IFSelect_SelectSuite;
      for (i = 1; i < onflag; i ++) {
	sel = WS->GiveSelection(pilot->Arg(i));
	if (!suite->AddInput(sel)) {
	  sout<<"Incorrect definition for applied selection"<<endl;
	  return IFSelect_RetError;
	}
      }
      seld = suite;
    }

    Handle(IFSelect_GraphCounter) gc = new IFSelect_GraphCounter(Standard_False,listmode);
    gc->SetApplied (seld);
    counter = gc;
  }

  if (counter.IsNull()) {
    sout<<"Neither Counter nor Signature : "<<arg1<<endl;
    return IFSelect_RetError;
  }

  if (onflag == 0) onflag = 1;
  IFSelect_PrintCount pcm = IFSelect_ListByItem;
  if (arg0[0] == 'c') pcm = IFSelect_CountByItem;
  if (arg0[0] == 's') pcm = IFSelect_CountSummary;
  return pilot->ExecuteCounter (counter,onflag+1, pcm);
}

static IFSelect_ReturnStatus funsigntype
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Sign Type              ****
  Handle(IFSelect_Signature) signtype = WS->SignType();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (signtype.IsNull()) sout<<"signtype actually undefined"<<endl;
  else {
    Handle(TCollection_HAsciiString) str = WS->Name (signtype);
    Standard_Integer id = WS->ItemIdent (signtype);
    sout<<signtype->Label()<<endl;
    if (str.IsNull()) {
      if (id > 0) sout<<"signtype : item n0 "<<id<<endl;
    } else {
      sout<<"signtype : also named as "<<str->ToCString()<<endl;
    }
  }
  if (argc < 2) sout<<"signtype newitem  to change, signtype . to clear"<<endl;
  else {
    if (arg1[0] == '.' && arg1[1] == '\0') {
      signtype.Nullify();
      sout<<"signtype now cleared"<<endl;
    } else {
      signtype = GetCasted(IFSelect_Signature,WS->NamedItem(arg1));
      if (signtype.IsNull()) { sout<<"Not a Signature : "<<arg1<<endl; return IFSelect_RetError; }
      else sout<<"signtype now set to "<<arg1<<endl;
    }
    WS->SetSignType(signtype);
    return IFSelect_RetDone;
  }
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus funsigncase
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Sign Case              ****
  Handle(IFSelect_Signature) signcase = GetCasted(IFSelect_Signature,WS->NamedItem(arg1));
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (signcase.IsNull()) sout<<"Not a Signature : "<<arg1<<endl;
  else {
    Standard_Boolean hasmin,hasmax;  Standard_Integer valmin,valmax;
    if (signcase->IsIntCase(hasmin,valmin,hasmax,valmax)) {
      sout<<"Signature "<<arg1<<" : Integer Case";
      if (hasmin) sout<<" - Mini:"<<valmin;
      if (hasmax) sout<<" - Maxi:"<<valmax;
      sout<<endl;
    }
    Handle(TColStd_HSequenceOfAsciiString) caselist = signcase->CaseList();
    if (caselist.IsNull()) sout<<"Signature "<<arg1<<" : no predefined case, see command  count "<<arg1<<endl;
    else {
      Standard_Integer i, nb = caselist->Length();
      sout<<"Signature "<<arg1<<" : "<<nb<<" basic cases :"<<endl;
      for (i = 1; i <= nb; i ++) sout<<"  "<<caselist->Value(i);
      sout<<endl;
    }
  }
  return IFSelect_RetVoid;
}


static IFSelect_ReturnStatus fun10
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Entity Status          ****
  Standard_Integer i,nb;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    nb = Interface_Category::NbCategories();
    sout<<" Categories defined :"<<nb<<" i.e. :\n";
    for (i = 0; i <= nb; i ++)
      sout<<"Cat."<<i<<"  : "<<Interface_Category::Name(i)<<"\n";
    sout<<" On a given entity : give its number"<<endl;
    return IFSelect_RetVoid;
  }
  Standard_Integer num = pilot->Number(arg1);
  if (num <= 0 || num > WS->NbStartingEntities())
    { sout<<"Not a suitable entity number : "<<arg1<<endl; return IFSelect_RetError; }
  Handle(Standard_Transient) ent = WS->StartingEntity(num);
  WS->PrintEntityStatus(ent,sout);
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun11
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    DumpModel (Data)  ****
  Standard_Integer niv = 0;
//  char arg10 = arg1[0];
//  if (argc < 2) arg10 = '?';
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  switch (arg1[0]) {
    case '?' :
     sout<<"? for this help, else give a listing mode (first letter suffices) :\n"
        <<" general    General Statistics\n roots    Roots\n"
	<<" entities   All Entities\n"
	<<" listfails  CheckList (fails)    per entity\n"
	<<" messages   CheckList (complete) per entity\n"
	<<" fails      CheckList (fails)    per message (counting)\n"
	<<" check      CheckList (complete) per message (counting)\n"
	<<" totalcheck CheckList (complete) per message (listing n0 ents)\n"
	<<" FAILS      CheckList (fails)    per message (listing complete)\n"
	<<" TOTALCHECK CheckList (complete) per message (listing complete)"<<endl;
     return IFSelect_RetVoid;
    case 'g' : niv = 0; break;
    case 'r' : niv = 1; break;
    case 'e' : niv = 2; break;
    case 'l' : niv = 3; break;
    case 'm' : niv = 4; break;
    case 'c' : niv = 5; break;
    case 't' : niv = 6; break;
    case 'T' : niv = 7; break;
    case 'f' : niv = 8; break;
    case 'F' : niv =10; break;
    default  : sout<<"Unknown Mode .  data tout court pour help"<<endl; return IFSelect_RetError;
  }
  WS->TraceDumpModel(niv);
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fundumpent
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  Handle(IFSelect_WorkLibrary) WL = WS->WorkLibrary();
  Standard_Integer levdef=0,levmax=10,level;
  WL->DumpLevels (levdef,levmax);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2 || (argc == 2 && levmax < 0)) {
    sout<<"Give n0 or id of entity";
    if (levmax < 0) sout<<"  and dump level"<<endl;
    else sout<<"  + optinal, dump level in [0 - "<<levmax<<"] , default = "<<levdef<<endl;
    for (level = 0; level <= levmax; level ++) {
      Standard_CString help = WL->DumpHelp (level);
      if (help[0] != '\0') sout<<level<<" : "<<help<<endl;
    }
    return IFSelect_RetError;
  }

  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Standard_Integer num = pilot->Number(arg1);
  if (num == 0) return IFSelect_RetError;
  level = levdef;
  if (argc > 2) level = atoi(arg2);
  Handle(Standard_Transient) ent = WS->StartingEntity(num);
  if ( ent.IsNull() ) {
    sout << "No entity with given id " << arg1 << " (" << num << ") is found in the current model" << endl;
  }
  else {
    sout << "  --   DUMP  Entity n0 " << num << "  level " << level << endl;
    WL->DumpEntity (WS->Model(),WS->Protocol(),ent,sout,level);

    Interface_CheckIterator chl = WS->CheckOne (ent);
    if (!chl.IsEmpty(Standard_False)) chl.Print(sout,WS->Model(),Standard_False);
  }
//  sout << flush;

  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus funsign
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<" Give signature name + n0 or id of entity"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Signature,sign,WS->NamedItem(arg1));
  if (sign.IsNull()) { sout<<"Not a signature : "<<arg1<<endl; return IFSelect_RetError; }
  Standard_Integer num = pilot->Number(arg2);
  Handle(Standard_Transient) ent = WS->StartingEntity (num);
  if (num == 0) return IFSelect_RetError;
  sout<<"Entity n0 "<<num<<" : "<<WS->SignValue(sign,ent)<<endl;
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus funqp
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<" Give 2 numeros or labels : dad son"<<endl; return IFSelect_RetError; }
  Standard_Integer n1 = WS->NumberFromLabel(arg1);
  Standard_Integer n2 = WS->NumberFromLabel(arg2);
  sout<<"QueryParent for dad:"<<arg1<<":"<<n1<<" and son:"<<arg2<<":"<<n2<<endl;
  Standard_Integer qp = WS->QueryParent(WS->StartingEntity(n1),WS->StartingEntity(n2));
  if (qp < 0) sout<<arg1<<" is not super-entity of "<<arg2<<endl;
  else if (qp == 0) sout<<arg1<<" is same as "<<arg2<<endl;
  else sout<<arg1<<" is super-entity of "<<arg2<<" , max level found="<<qp<<endl;
//  sout<<" Trouve "<<qp<<endl;
  return IFSelect_RetVoid;
}
  

static IFSelect_ReturnStatus fun12
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    DumpShare         ****
  WS->DumpShare();  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun13
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    ListItems         ****
  WS->ListItems(pilot->Arg(1));  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun14
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    NewInt            ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 1) { sout<<"Donner la valeur entiere pour IntParam"<<endl; return IFSelect_RetError; }
  Handle(IFSelect_IntParam) intpar = new IFSelect_IntParam;
  if (argc >= 1)       intpar->SetValue(atoi(arg1));
  return pilot->RecordItem (intpar);
}

static IFSelect_ReturnStatus fun15
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SetInt            ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3)
    { sout<<"Donner 2 arguments : nom Parametre et Valeur"<<endl; return IFSelect_RetError; }
  Standard_Integer val = atoi(arg2);
  DeclareAndCast(IFSelect_IntParam,par,WS->NamedItem(arg1));
  if (!WS->SetIntValue(par,val)) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun16
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    NewText           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 1) { sout<<"Donner la valeur texte pour TextParam"<<endl; return IFSelect_RetError; }
  Handle(TCollection_HAsciiString) textpar = new TCollection_HAsciiString();
  if (argc >= 1) textpar->AssignCat(arg1);
  return pilot->RecordItem (textpar);
}

static IFSelect_ReturnStatus fun17
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SetText           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3)
    { sout<<"Donner 2 arguments : nom Parametre et Valeur"<<endl; return IFSelect_RetError; }
  DeclareAndCast(TCollection_HAsciiString,par,WS->NamedItem(arg1));
  if (!WS->SetTextValue(par,arg2)) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun19
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    DumpSel           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give 1 argument : Selection Name"<<endl; return IFSelect_RetError; }
  WS->DumpSelection (GetCasted(IFSelect_Selection,WS->NamedItem(arg1)));
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun20
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
//        ****    EvalSel           ****
//        ****    GiveList          ****
//        ****    GiveShort GivePointed  ****
//        ****    MakeList          ****
  char mode = pilot->Arg(0)[0];  // givelist/makelist
  if (mode == 'g') mode = pilot->Arg(0)[4];  // l list  s short  p pointed
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give Entity ID, or Selection Name [+ optional other selection or entity]"<<endl; return IFSelect_RetError; }

//    MakeList : sur Pointed existante ou a creer
  Handle(IFSelect_SelectPointed) pnt;
  if (mode == 'm') {
    const Standard_CString arg1 = pilot->Arg(1);
    Handle(Standard_Transient) item = WS->NamedItem (arg1);
    pnt = GetCasted(IFSelect_SelectPointed,item);
    if (!pnt.IsNull()) {
      sout<<arg1<<":Already existing Selection for List, cleared then filled"<<endl;
      pnt->Clear();
    } else if (!item.IsNull()) {
      sout<<arg1<<":Already existing Item not for a List, command ignored"<<endl;
      return IFSelect_RetFail;
    } else {
      pnt = new IFSelect_SelectPointed;
      WS->AddNamedItem (arg1,pnt);
    }
  }

  Handle(TColStd_HSequenceOfTransient) result =
    IFSelect_Functions::GiveList (WS,pilot->CommandPart( (mode == 'm' ? 2 : 1) ));
  if (result.IsNull()) return IFSelect_RetError;
  Interface_EntityIterator iter (result);
  sout<<pilot->CommandPart( (mode == 'm' ? 2 : 1) )<<" : ";
  if      (mode == 'l')   WS->ListEntities (iter,0);
  else if (mode == 's' || mode == 'm') WS->ListEntities (iter,2);
  else if (mode == 'p') {
    sout<<iter.NbEntities()<<" Entities : ";
    for (iter.Start(); iter.More(); iter.Next())
      sout<<" +"<<WS->StartingNumber (iter.Value());
    sout<<endl;
  }

  if (!pnt.IsNull()) {
    pnt->SetList (result);
    sout<<"List set to a SelectPointed : "<<pilot->Arg(1)<<endl;
    sout<<"Later editable by command setlist"<<endl;
  }

  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun20c
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
//        ****    GiveCount         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give Entity ID, or Selection Name [+ optional other selection or entity]"<<endl; return IFSelect_RetError; }
//  WS->EvaluateSelection(GetCasted(IFSelect_Selection,WS->NamedItem(arg1)));
  Handle(TColStd_HSequenceOfTransient) result =
    IFSelect_Functions::GiveList (WS,pilot->CommandPart(1));
  if (result.IsNull()) return IFSelect_RetError;
  sout<<pilot->CommandPart(1)<<" : List of "<<result->Length()<<" Entities"<<endl;
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus funselsuite
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
//        ****    SelSuite         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give Entity ID, or Selection Name [+ optional other selection or entity]"<<endl; return IFSelect_RetError; }
//  WS->EvaluateSelection(GetCasted(IFSelect_Selection,WS->NamedItem(arg1)));
  Handle(IFSelect_SelectSuite) selsuite = new IFSelect_SelectSuite;

  for (Standard_Integer i = 1; i < argc; i ++) {
    Handle(IFSelect_Selection) sel = WS->GiveSelection(pilot->Arg(i));
    if (!selsuite->AddInput(sel)) {
      sout<<pilot->Arg(i-1)<<" : not a SelectDeduct, no more can be added. Abandon"<<endl;
      return IFSelect_RetError;
    }
  }
  selsuite->SetLabel (pilot->CommandPart(1));
  return pilot->RecordItem (selsuite);
}


static IFSelect_ReturnStatus fun21
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    ClearItems           ****
  WS->ClearItems();  WS->ClearFinalModifiers();  WS->ClearShareOut(Standard_False);
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun22
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    ClearData           ****
  Standard_Integer mode = -1;
  if (argc >= 2) {
    if (arg1[0] == 'a') mode = 1;
    if (arg1[0] == 'g') mode = 2;
    if (arg1[0] == 'c') mode = 3;
    if (arg1[0] == 'p') mode = 4;
    if (arg1[0] == '?') mode = -1;
  }
  else mode = 0;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (mode <= 0) {
    if (mode < 0) sout<<"Give a suitable mode";
    sout<<"  Available Modes :\n"
      <<" a : all data    g : graph+check  c : check  p : selectpointed"<<endl;
    return (mode < 0 ? IFSelect_RetError : IFSelect_RetVoid);
  }
  WS->ClearData (mode);
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun24
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
//        ****    Item Label         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  TCollection_AsciiString label;
  if (argc < 2) { sout<<" Give  label to search"<<endl;  return IFSelect_RetError;  }
  for (int i = 1; i < argc; i ++) {
    label.AssignCat(pilot->Arg(i));
    if (i < argc-1) label.AssignCat(" ");
  }
  for (int mode = 0; mode <= 2; mode ++) {
    int nbitems = 0;  int id;
    sout<<"Searching label : "<<label<<". in mode ";
    if (mode == 0) sout <<" exact" << endl;
    if (mode == 1) sout <<" same head" << endl;
    if (mode == 2) sout <<" search if present" << endl;
    for (id = WS->NextIdentForLabel(label.ToCString(), 0,mode)  ; id != 0;
	 id = WS->NextIdentForLabel(label.ToCString(),id,mode)) {
      sout<<" "<<id;  nbitems ++;
    }
    sout<<" -- giving " << nbitems << " found" << endl;
  }
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun25
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Save (Dump)       ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner nom du Fichier"<<endl; return IFSelect_RetError; }
  IFSelect_SessionFile dumper(WS,arg1);
  if (!dumper.IsDone()) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun26
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Restore (Dump)    ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner nom du Fichier"<<endl; return IFSelect_RetError; }
  IFSelect_SessionFile dumper(WS);
  Standard_Integer readstat = dumper.Read(arg1);
  if      (readstat == 0) return IFSelect_RetDone;
  else if (readstat >  0) sout << "-- Erreur Lecture Fichier "<<arg1<<endl;
  else                    sout << "-- Pas pu ouvrir Fichier "<<arg1<<endl;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun27
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    Param(Value)         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    Handle(TColStd_HSequenceOfHAsciiString) li = Interface_Static::Items();
    Standard_Integer i,nb = li->Length();
    sout<<" List of parameters : "<<nb<<" items : "<<endl;
    for (i = 1; i <= nb; i ++) {
      sout<<li->Value(i);
      sout<<" : "<<Interface_Static::CVal(li->Value(i)->ToCString())<<endl;
    }
    return IFSelect_RetVoid;
  } else if (atoi(arg1) > 0) {
    Standard_Integer use = atoi (arg1);
    WS->TraceStatics (use);
  } else {
    if (argc > 2) sout<<"     FORMER STATUS of Static Parameter "<<arg1<<endl;
    else          sout<<"     ACTUAL STATUS of Static Parameter "<<arg1<<endl;
    if (!Interface_Static::IsPresent(arg1)) { sout<<" Parameter "<<arg1<<" undefined"<<endl; return IFSelect_RetError; }
    if (!Interface_Static::IsSet(arg1)) sout<<" Parameter "<<arg1<<" not valued"<<endl;
    else if (argc == 2) Interface_Static::Static (arg1) -> Print(sout);
    else sout<<" Value : "<<Interface_Static::CVal(arg1)<<endl;

    if (argc == 2) sout<<"To modify, param name_param new_val"<<endl;
    else {
      sout<<" New demanded value : "<<arg2;
      if (Interface_Static::SetCVal (arg1,arg2))
	{  sout<<"   OK"<<endl;  return IFSelect_RetDone;  }
      else  {  sout <<" , refused"<<endl;  return IFSelect_RetError;  }
    }
  }
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun28
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  const Standard_CString arg3 = pilot->Arg(3);
//        ****    DefParam         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    Handle(TColStd_HSequenceOfHAsciiString) li = Interface_Static::Items();
    Standard_Integer i,nb = li->Length();
    sout<<" List of parameters : "<<nb<<" items :"<<endl;
    for (i = 1; i <= nb; i ++) sout<<"  "<<li->Value(i)->ToCString();
    sout<<endl<<"  defparam name_param  to known more about one"<<endl;
    sout<<"  defparam nom_param e options  to edit a definition"<<endl;

  } else if (argc == 2) {
    sout<<" Definition of Parameter : "<<arg1<<endl;
    Handle(Interface_Static) unst = Interface_Static::Static (arg1);
    if (unst.IsNull()) sout<<"  undefined"<<endl;
    else unst->Print(sout);
    return IFSelect_RetVoid;

  } else if (arg2[0] == 'i') {
//  initialisation : arg1=nompar  a2='i'  a3=family  a4=type  [a5=val]
    if (argc < 5) { sout<<" name init family type [valinit]"<<endl; return IFSelect_RetVoid; }
    char typ = (pilot->Arg(4))[0];
    Standard_Boolean ok= (argc==5 ? Interface_Static::Init(arg3,arg1,typ) :
			  Interface_Static::Init(arg3,arg1,typ,pilot->Arg(5)));
    return (ok ? IFSelect_RetDone : IFSelect_RetFail);

  } else if (arg2[0] == 'e') {
//  edition : arg1=nompar  arg2='e' arg3=option  arg4...=parametres option
    char comm[100];
    if (argc < 4) {
      sout<<" give name and options !  Options (according type), 1 a time\n"
	  <<" imin ival / imax ival / rmin rval / rmax rval /\n"
	  <<" enum stnum / enum stnum match / eval e1 e2 e3 ... (maxi 10)\n"
	  <<endl;
      return IFSelect_RetVoid;
    }
    if (argc > 4) sout<<"Only the command and ONE more arg are considered"<<endl;
    sprintf(comm,"%s %s",pilot->Arg(3),pilot->Arg(4));
    sout<<"Editing parameter : "<<arg1<<" , by command : "<<comm<<endl;

    Handle(Interface_Static) unst = Interface_Static::Static (arg1);
    if (unst.IsNull()) { sout<<arg1<<"  undefined"<<endl; return IFSelect_RetError; }
    if (Interface_Static::Init(unst->Family(),arg1,'&',comm))
      {  sout<<"Editing done"<<endl;  return IFSelect_RetDone;  }
    else  {  sout<<"Command not processed : "<<comm<<endl; return IFSelect_RetFail;  }
  }
  sout<<"Unknown Option : "<<arg2<<endl;
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun29
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SentFiles         ****
  Handle(TColStd_HSequenceOfHAsciiString) list = WS->SentFiles();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (list.IsNull())
    { sout<<"List of Sent Files not enabled"<<endl; return IFSelect_RetVoid; }
  Standard_Integer i, nb = list->Length();
  sout<<"  Sent Files : "<<nb<<" : "<<endl;
  for (i = 1; i <= nb; i ++)
    sout<<list->Value(i)->ToCString()<<endl; 
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun30
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    FilePrefix        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    if (WS->FilePrefix().IsNull()) sout<<"Pas de prefixe defini"<<endl;
    else sout<<"Prefixe : "<<WS->FilePrefix()->ToCString()<<endl;
    sout<<"Pour changer :  filepref newprefix"<<endl;
    return IFSelect_RetVoid;
  }
  WS->SetFilePrefix(arg1);
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun31
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    FileExtension     ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    if (WS->FileExtension().IsNull()) sout<<"Pas d extension definie"<<endl;
    else sout<<"Extension : "<<WS->FileExtension()->ToCString()<<endl;
    sout<<"Pour changer :  fileext newext"<<endl;
    return IFSelect_RetVoid;
  }
  WS->SetFileExtension(arg1);
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun32
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    FileRoot          ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Dispatch et nom de Root"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Dispatch,disp,WS->NamedItem(arg1));
  if (argc < 3) {
    if (WS->FileRoot(disp).IsNull()) sout<<"Pas de racine definie pour "<<arg1<<endl;
    else sout<<"Racine pour "<<arg1<<" : "<<WS->FileRoot(disp)->ToCString()<<endl;
    sout<<"Pour changer :  fileroot nomdisp newroot"<<endl;
    return IFSelect_RetVoid;
  }
  if (!WS->SetFileRoot(disp,arg2)) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun33
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Default File Root     ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    if (WS->DefaultFileRoot().IsNull()) sout<<"Pas de racine par defaut definie"<<endl;
    else sout<<"Racine par defaut : "<<WS->DefaultFileRoot()->ToCString()<<endl;
    sout<<"Pour changer :  filedef newdef"<<endl;
    return IFSelect_RetVoid;
  }
  WS->SetDefaultFileRoot(arg1);
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun34
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    EvalFile          ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (!WS->HasModel()) 
    {  sout<<"Pas de Modele charge, abandon"<<endl;  return IFSelect_RetFail; }

  sout<<"Evaluation avec Memorisation des resultats"<<endl;
  WS->EvaluateFile();
  Standard_Integer nbf = WS->NbFiles();
  for (Standard_Integer i = 1; i <= nbf; i ++) {
    Handle(Interface_InterfaceModel) mod = WS->FileModel(i);
    if (mod.IsNull())
      {  sout<<"Modele "<<i<<" Model non genere ..."<<endl; continue;  }
    TCollection_AsciiString name = WS->FileName(i);
    sout<<"Fichier n0 "<<i<<" Nb Entites : "<<mod->NbEntities()<<"  Nom: ";
    sout<<name<<endl;
  }
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun35
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    ClearFile          ****
  WS->ClearFile();  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun36
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
//        ****    Split              ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  IFSelect_ReturnStatus stat = IFSelect_RetVoid;
  if (argc < 2) sout<<"Split : derniere liste de dispatches definie"<<endl;
  else {
    WS->ClearShareOut(Standard_True);
    for (Standard_Integer i = 1; i < argc; i ++) {
      DeclareAndCast(IFSelect_Dispatch,disp,WS->NamedItem(pilot->Arg(i)));
      if (disp.IsNull()) {
	sout<<"Pas un dispatch:"<<pilot->Arg(i)<<", Splitt abandonne"<<endl;
	stat = IFSelect_RetError;
      }
      else WS->SetActive(disp,Standard_True);
    }
  }
  if (stat ==  IFSelect_RetError) return stat;
  WS->BeginSentFiles(Standard_True);
  if (!WS->SendSplit()) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun37
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Remaining Data     ****
  char mode = '?';  IFSelect_RemainMode numod = IFSelect_RemainDisplay;
  if (argc >= 2) mode = arg1[0];
  if      (mode == 'u') numod = IFSelect_RemainUndo;
  else if (mode == 'l') numod = IFSelect_RemainDisplay;
  else if (mode == 'c') numod = IFSelect_RemainCompute;
  else if (mode == 'f') numod = IFSelect_RemainForget;
  else {
    Handle(Message_Messenger) sout = Message::DefaultMessenger();
    if (argc<2) sout<<"Donner un Mode - ";
    sout<<"Modes possibles : l  list, c compute, u undo, f forget"<<endl;
    if (mode == '?') return IFSelect_RetDone;   else return IFSelect_RetError;
  }
  if (!WS->SetRemaining(numod)) return IFSelect_RetVoid;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun38
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SetModelContent    ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner nom selection et mode (k=keep,r=remove)"<<endl;  return IFSelect_RetError; }
  Standard_Boolean keepmode;
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  if (sel.IsNull())
    { sout<<"Pas de Selection de Nom : "<<arg1<<endl; return IFSelect_RetError; }
  if      (arg2[0] == 'k') {  sout<<" -- SetContent keep ..."; keepmode = Standard_True; }
  else if (arg2[0] == 'r') {  sout<<" -- SetContent remove ..."; keepmode = Standard_False; }
  else { sout<<"Donner nom selection et mode (k=keep,r=remove)"<<endl;  return IFSelect_RetError; }
  
  if (WS->SetModelContent(sel,keepmode)) sout<<" Done"<<endl;
  else sout<<" Result empty, ignored"<<endl;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun40
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    ListModif          ****
  WS->ListFinalModifiers(Standard_True);
  WS->ListFinalModifiers(Standard_False);  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun41
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Modifier           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom du Modifier"<<endl;  return IFSelect_RetError; }
  DeclareAndCast(IFSelect_GeneralModifier,modif,WS->NamedItem(arg1));
  if (modif.IsNull())
    { sout<<"Pas de Modifier de Nom : "<<arg1<<endl; return IFSelect_RetVoid; }
  Handle(IFSelect_IntParam) low,up;

  Handle(IFSelect_Dispatch) disp = modif->Dispatch();
  sout<<"Modifier : "<<arg1<<" Label : "<<modif->Label()<<endl;
  Standard_Integer rank = WS->ModifierRank(modif);
  if (modif->IsKind(STANDARD_TYPE(IFSelect_Modifier)))
    sout<< "Model Modifier n0." << rank;
  else sout<< "File Modifier n0." << rank;
  if (disp.IsNull()) sout<<"  Applique a tous les Dispatchs" << endl;
  else {
    sout << "  Dispatch : "<<disp->Label();
    if (WS->HasName(disp)) sout << " - Nom:"<<WS->Name(disp)->ToCString();
    sout<<endl;
  }

  Handle(IFSelect_Selection) sel = modif->Selection();
  if (!sel.IsNull()) sout<<"  Selection : "<< sel->Label();
  if (WS->HasName(sel)) sout<<" - Nom:"<< WS->Name(sel)->ToCString();
  sout<<endl;
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun42
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    ModifSel           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom Modifier; + Nom Selection optionnel\n"
	              <<"Selection pour Mettre une Selection, sinon Annule"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_GeneralModifier,modif,WS->NamedItem(arg1));
  if (modif.IsNull())
    { sout<<"Pas un nom de Modifier : "<<arg1<<endl; return IFSelect_RetError;  }
  Handle(IFSelect_Selection) sel;
  if (arg2[0] != '\0') {
    sel = GetCasted(IFSelect_Selection,WS->NamedItem(arg2));
    if (sel.IsNull())
      { sout<<"Pas un nom de Selection : "<<arg2<<endl;  return IFSelect_RetError;  }
  }
  if (!WS->SetItemSelection(modif,sel)) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun43
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SetAppliedModifier           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom Modifier; + Nom Dispatch ou Transformer optionnel :\n"
		      <<" - rien : tous Dispatches\n - Dispatch : ce Dispatch seul\n"
		      <<" - Transformer : pas un Dispatch mais un Transformer"<<endl;
		  return IFSelect_RetError;  }
  DeclareAndCast(IFSelect_GeneralModifier,modif,WS->NamedItem(arg1));
  if (modif.IsNull())
    { sout<<"Pas un nom de Modifier : "<<arg1<<endl;  return IFSelect_RetError;  }
  Handle(Standard_Transient) item;
  if (arg2[0] != '\0') {
    item = WS->NamedItem(arg2);
    if (item.IsNull())
      { sout<<"Pas un nom connu : "<<arg2<<endl;  return IFSelect_RetError;  }
  }
  else item = WS->ShareOut();
  if (!WS->SetAppliedModifier(modif,item)) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun44
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    ResetApplied (modifier)    ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Designer un modifier"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_GeneralModifier,modif,WS->NamedItem(arg1));
  if (modif.IsNull())
    { sout<<"Pas un nom de Modifier : "<<arg1<<endl;  return IFSelect_RetError;  }
  if (!WS->ResetAppliedModifier(modif)) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun45
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  const Standard_CString arg3 = pilot->Arg(3);
//        ****    ModifMove         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 4) { sout<<"modifmove MF rang1 rang2, M pour Model F pour File"<<endl; return IFSelect_RetError; }
  Standard_Boolean formodel;
  if      (arg1[0] == 'm' || arg1[0] == 'M') formodel = Standard_True;
  else if (arg1[0] == 'f' || arg1[0] == 'F') formodel = Standard_False;
  else { sout<<"preciser M pour Model, F pour File"<<endl; return IFSelect_RetError; }
  Standard_Integer before = atoi(arg2);
  Standard_Integer after  = atoi(arg3);
  if (before == 0 || after == 0) { sout<<"Donner 2 Entiers Positifs"<<endl; return IFSelect_RetError; }
  if (!WS->ChangeModifierRank(formodel,before,after)) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun51
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    DispSel           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner Noms Dispatch et Selection Finale"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Dispatch,disp,WS->NamedItem(arg1));
  if (disp.IsNull())
    { sout<<"Pas un nom de Dispatch : "<<arg1<<endl;  return IFSelect_RetError;  }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg2));
  if (sel.IsNull())
    { sout<<"Pas un nom de Selection : "<<arg2<<endl;  return IFSelect_RetError;  }
  if (!WS->SetItemSelection(disp,sel)) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun_dispone
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    DispOne           ****
  Handle(IFSelect_DispPerOne) disp = new IFSelect_DispPerOne;
  return pilot->RecordItem(disp);
}

static IFSelect_ReturnStatus fun_dispglob
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    DispGlob          ****
  Handle(IFSelect_DispGlobal) disp = new IFSelect_DispGlobal;
  return pilot->RecordItem(disp);
}

static IFSelect_ReturnStatus fun_dispcount
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    DispCount         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom IntParam pour Count"<<endl;  return IFSelect_RetError; }
  DeclareAndCast(IFSelect_IntParam,par,WS->NamedItem(arg1));
  if (par.IsNull())
    { sout<<"Pas un nom de IntParam : "<<arg1<<endl;  return IFSelect_RetError;  }
  Handle(IFSelect_DispPerCount) disp = new IFSelect_DispPerCount;
  disp->SetCount (par);
  return pilot->RecordItem(disp);
}

static IFSelect_ReturnStatus fun_dispfiles
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    DispFiles         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom IntParam pour NbFiles"<<endl;  return IFSelect_RetError; }
  DeclareAndCast(IFSelect_IntParam,par,WS->NamedItem(arg1));
  if (par.IsNull())
    { sout<<"Pas un nom de IntParam : "<<arg1<<endl;  return IFSelect_RetError;  }
  Handle(IFSelect_DispPerFiles) disp = new IFSelect_DispPerFiles;
  disp->SetCount (par);
  return pilot->RecordItem(disp);
}


static IFSelect_ReturnStatus fun_dispsign
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    DispFiles         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom Signature"<<endl;  return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Signature,sig,WS->NamedItem(arg1));
  if (sig.IsNull())
    { sout<<"Pas un nom de Signature : "<<arg1<<endl;  return IFSelect_RetError;  }
  Handle(IFSelect_DispPerSignature) disp = new IFSelect_DispPerSignature;
  disp->SetSignCounter (new IFSelect_SignCounter(sig));
  return pilot->RecordItem(disp);
}


static IFSelect_ReturnStatus fun56
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Dispatch           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom du Dispatch"<<endl;  return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Dispatch,disp,WS->NamedItem(arg1));
  if (disp.IsNull()) { sout<<"Pas un dispatch : "<<arg1<<endl; return IFSelect_RetError;  }
  Standard_Integer num = WS->DispatchRank(disp);
  sout<<"Dispatch de Nom : "<<arg1<<" , en ShareOut, Numero "<<num<<" : ";
  Handle(IFSelect_Selection) sel = WS->ItemSelection(disp);
  Handle(TCollection_HAsciiString) selname = WS->Name(sel);
  if (sel.IsNull())  sout<<"Pas de Selection Finale"<<endl;
  else if (selname.IsNull()) sout<<"Selection Finale : #"<<WS->ItemIdent(sel)<<endl;
  else sout<<"Selection Finale : "<<selname->ToCString()<<endl;
  if (disp->HasRootName()) sout<<"-- Racine nom de fichier : "
    <<disp->RootName()->ToCString()<<endl;
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun57
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Remove           ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give Name to Remove !"<<endl;  return IFSelect_RetError; }
  if (!WS->RemoveNamedItem(arg1)) return IFSelect_RetFail;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun58
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    EvalDisp          ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"evaldisp mode disp [disp ...] :  Mode + Name(s) of Dispatch(es). Mode:\n"
		    <<"  0 brief  1 +forgotten ents  2 +duplicata  3 1+2"<<endl
		    <<"See also : evaladisp  writedisp  xsplit"<<endl;
		  return IFSelect_RetVoid;  }
  Standard_Boolean OK = Standard_True;
  Standard_Integer i , mode = atoi(arg1);  sout<<" Mode "<<mode<<"\n";
  for (i = 2; i < argc; i ++) {
    DeclareAndCast(IFSelect_Dispatch,disp,WS->NamedItem(pilot->Arg(i)));
    if (disp.IsNull())
      { sout<<"Not a dispatch:"<<pilot->Arg(i)<<endl; OK = Standard_False; }
  }
  if (!OK) {
    sout<<"Some of the parameters are not correct"<<endl;
    return IFSelect_RetError;
  }

  WS->ClearShareOut(Standard_True);
  for (i = 2; i < argc; i ++) {
    DeclareAndCast(IFSelect_Dispatch,disp,WS->NamedItem(pilot->Arg(i)));
    WS->SetActive(disp,Standard_True);
  }
//      WS->EvaluateDispatch(disp,mode);
  WS->EvaluateComplete(mode);
  return IFSelect_RetVoid;
}


static IFSelect_ReturnStatus fun_evaladisp
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    EvalADisp [GiveList]         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"evaladisp mode(=0-1-2-3) disp [givelist] :  Mode + Dispatch [+ GiveList]\n  If GiveList not given, computed from Selection of the Dispatch. Mode:\n"
		    <<"  0 brief  1 +forgotten ents  2 +duplicata  3 1+2"<<endl
		    <<"See also : writedisp"<<endl;
		  return IFSelect_RetVoid;  }
  if (arg1[1] != '\0') { sout<<"first parameter : mode, must be a number between 0 and 3"<<endl; return IFSelect_RetError; }
  Standard_Integer mode = atoi(arg1);  sout<<" Mode "<<mode<<"\n";
//  DeclareAndCast(IFSelect_Dispatch,disp,WS->NamedItem(pilot->Arg(2)));
  Handle(IFSelect_Dispatch) disp = IFSelect_Functions::GiveDispatch (WS,pilot->Arg(2),Standard_True);
  if (disp.IsNull())
    { sout<<"Not a dispatch:"<<pilot->Arg(2)<<endl; return IFSelect_RetError; }
  Handle(IFSelect_Selection) selsav = disp->FinalSelection();
  Handle(IFSelect_Selection) sel;
  if (argc > 3) {
    Handle(IFSelect_SelectPointed) sp = new IFSelect_SelectPointed;
    Handle(TColStd_HSequenceOfTransient) list = IFSelect_Functions::GiveList
      (pilot->Session(),pilot->CommandPart(3));
    Standard_Integer nb = (list.IsNull() ? 0 : list->Length());
    if (nb > 0)  {  sp->AddList (list);  sel = sp;  }
  }

  if (sel.IsNull() && selsav.IsNull())
    { sout<<"No Selection nor GiveList defined"<<endl; return IFSelect_RetError; }
  if (sel.IsNull() && !selsav.IsNull()) {
    if (argc > 3) sout<<"GiveList is empty, hence computed from the Selection of the Dispatch"<<endl;
    sel = selsav;
  }
  disp->SetFinalSelection(sel);
//  WS->ClearShareOut(Standard_True);
//  WS->SetActive(disp,Standard_True);
  WS->EvaluateDispatch(disp,mode);
  disp->SetFinalSelection(selsav);

  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun_writedisp
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    EvalADisp [GiveList]         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"writedisp filename disp [givelist] :  FileName + Dispatch [+ GiveList]\n  If GiveList not given, computed from Selection of the Dispatch.\n"
		    <<"FileName : rootname.ext will gives rootname_1.ext etc...\n"
		    <<"  path/rootname.ext gives  path/rootname_1.ext etc...\n"
		    <<"See also : evaladisp"<<endl;
		  return IFSelect_RetVoid;  }
  TCollection_AsciiString prefix,rootname,suffix;
  SplitFileName (arg1,prefix,rootname,suffix);
  if (rootname.Length() == 0 || suffix.Length() == 0) {
    sout<<"Empty Root Name or Extension"<<endl;
    return IFSelect_RetError;
  }

//  DeclareAndCast(IFSelect_Dispatch,disp,WS->NamedItem(pilot->Arg(2)));
  Handle(IFSelect_Dispatch) disp = IFSelect_Functions::GiveDispatch (WS,pilot->Arg(2),Standard_True);
  if (disp.IsNull())
    { sout<<"Not a dispatch:"<<pilot->Arg(2)<<endl; return IFSelect_RetError; }
  Handle(IFSelect_Selection) selsav = disp->FinalSelection();
  Handle(IFSelect_Selection) sel;
  if (argc > 3) {
    Handle(IFSelect_SelectPointed) sp = new IFSelect_SelectPointed;
    Handle(TColStd_HSequenceOfTransient) list = IFSelect_Functions::GiveList
      (pilot->Session(),pilot->CommandPart(3));
    Standard_Integer nb = (list.IsNull() ? 0 : list->Length());
    if (nb > 0)  {  sp->AddList (list);  sel = sp;  }
  }

  if (sel.IsNull() && selsav.IsNull())
    { sout<<"No Selection nor GiveList defined"<<endl; return IFSelect_RetError; }
  if (sel.IsNull() && !selsav.IsNull()) {
    if (argc > 3) sout<<"GiveList is empty, hence computed from the Selection of the Dispatch"<<endl;
    sel = selsav;
  }

  WS->ClearShareOut(Standard_True);
  disp->SetFinalSelection(sel);
  WS->SetActive(disp,Standard_True);
  WS->BeginSentFiles(Standard_True);

  WS->SetFilePrefix    (prefix.ToCString());
  WS->SetFileExtension (suffix.ToCString());
  WS->SetFileRoot(disp,rootname.ToCString());

  Standard_Boolean OK = WS->SendSplit();
  disp->SetFinalSelection(selsav);
  return (OK ? IFSelect_RetDone : IFSelect_RetFail);
}


static IFSelect_ReturnStatus fun59
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    EvalComplete      ****
  Standard_Integer mode = 0;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) sout << " -- mode par defaut 0\n";
  else { mode = atoi(arg1); sout << " -- mode : " << mode << endl;  }
  WS->EvaluateComplete(mode);  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun60
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    LastRunCheckList    ****
  Interface_CheckIterator chlist = WS->LastRunCheckList();
  Handle(IFSelect_CheckCounter) counter = new IFSelect_CheckCounter(0);
  counter->Analyse(chlist,WS->Model(),Standard_False);
  counter->PrintCount (Message::DefaultMessenger());
  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun61
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    RunTransformer    ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom de Transformer"<<endl; return IFSelect_RetError;  }
  DeclareAndCast(IFSelect_Transformer,tsf,WS->NamedItem(arg1));
  Standard_Integer effect = WS->RunTransformer(tsf);
  switch (effect) {
    case -4 : sout<<"Edition sur place, nouveau Protocole, erreur recalcul graphe"<<endl; break;
    case -3 : sout<<"Erreur, Transformation ignoree"<<endl; break;
    case -2 : sout<<"Erreur sur edition sur place, risque de corruption (verifier)"<<endl; break;
    case -1 : sout<<"Erreur sur edition locale, risque de corruption (verifier)"<<endl; break;
    case  0 :
      if   (tsf.IsNull()) sout<<"Erreur, pas un Transformer: "<<arg1<<endl;
      else sout<<"Execution non faite"<<endl;
	      break;
    case  1 : sout<<"Transformation locale (graphe non touche)"<<endl; break;
    case  2 : sout<<"Edition sur place (graphe recalcule)"<<endl;  break;
    case  3 : sout<<"Modele reconstruit"<<endl; break;
    case  4 : sout<<"Edition sur place, nouveau Protocole"<<endl;  break;
    case  5 : sout<<"Nouveau Modele avec nouveau Protocole"<<endl; break;
    default : break;
  }
  return ((effect > 0) ? IFSelect_RetDone : IFSelect_RetFail);
}

static IFSelect_ReturnStatus fun62
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    TransformStandard Copy         ****
  return pilot->RecordItem(WS->NewTransformStandard(Standard_True));
}

static IFSelect_ReturnStatus fun63
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    TransformStandard OntheSpot         ****
  return pilot->RecordItem(WS->NewTransformStandard(Standard_False));
}

static IFSelect_ReturnStatus fun6465
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    Run Modifier avec Standard Copy     ****
//        ****    Run Modifier avec OnTheSpot         ****
  Standard_Boolean runcopy = (pilot->Arg(0)[3] == 'c');
//  soit c est un nom, sinon c est une commande
  Handle(IFSelect_Modifier) modif;
  if (WS->NameIdent(arg1) > 0)
    modif = GetCasted(IFSelect_Modifier,WS->NamedItem(arg1));
  else {
    pilot->RemoveWord(0);    // c etait la commande run
    pilot->Perform();
    modif = GetCasted(IFSelect_Modifier,pilot->RecordedItem());
  }
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (modif.IsNull())
    { sout<<"Pas un nom de Modifier : "<<arg1<<endl;  return IFSelect_RetError;  }

  Handle(TColStd_HSequenceOfTransient) list;
  Handle(IFSelect_SelectPointed) sp;
  if (argc > 2) {
    list = IFSelect_Functions::GiveList (WS,pilot->CommandPart(2));
    sp = new IFSelect_SelectPointed;
    sp->SetList (list);
  }

  Standard_Integer effect = 0;
  effect = WS->RunModifierSelected (modif,sp,runcopy);
//      sout<<"Modifier applique sur TransformStandard #"<<WS->ItemIdent(tsf)<<endl;
  switch (effect) {
    case -4 : sout<<"Edition sur place, nouveau Protocole, erreur recalcul graphe"<<endl; break;
    case -3 : sout<<"Erreur, Transformation ignoree"<<endl; break;
    case -2 : sout<<"Erreur sur edition sur place, risque de corruption (verifier)"<<endl; break;
    case -1 : sout<<"Erreur sur edition locale, risque de corruption (verifier)"<<endl; break;
    case  0 :
      if   (modif.IsNull()) sout<<"Erreur, pas un Modifier: "<<arg1<<endl;
      else sout<<"Execution non faite"<<endl;
	      break;
    case  1 : sout<<"Transformation locale (graphe non touche)"<<endl; break;
    case  2 : sout<<"Edition sur place (graphe recalcule)"<<endl;  break;
    case  3 : sout<<"Modele reconstruit"<<endl; break;
    case  4 : sout<<"Edition sur place, nouveau Protocole"<<endl;  break;
    case  5 : sout<<"Nouveau Modele avec nouveau Protocole"<<endl; break;
    default : break;
  }
  return ((effect > 0) ? IFSelect_RetDone : IFSelect_RetFail);
}

static IFSelect_ReturnStatus fun66
  (const Handle(IFSelect_SessionPilot)& pilot)
{
//        ****    (xset) ModifReorder         ****
  char opt = ' ';
  Standard_Integer argc = pilot->NbWords();
  if (argc >= 2) opt = pilot->Word(1).Value(1);
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (opt != 'f' && opt != 'l')
    { sout<<"Donner option : f -> root-first  l -> root-last"<<endl; return IFSelect_RetError; }
  return pilot->RecordItem(new IFSelect_ModifReorder(opt == 'l'));
}

static IFSelect_ReturnStatus fun70
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SelToggle         ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom de Selection"<<endl; return IFSelect_RetError;  }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  if (!WS->ToggleSelectExtract(sel))
    { sout<<"Pas une SelectExtract : "<<arg1<<endl; return IFSelect_RetFail;  }
  if (WS->IsReversedSelectExtract(sel)) sout<<arg1<<" a present Reversed"<<endl;
  else sout<<arg1<<" a present Directe"<<endl;
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun71
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelInput          ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner Noms Selections cible et input"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,sou,WS->NamedItem(arg2));
  if (sel.IsNull() || sou.IsNull())
    {  sout<<"Incorrect : "<<arg1<<","<<arg2<<endl;  return IFSelect_RetError;  }
  if (!WS->SetInputSelection(sel,sou)) { 
    sout<<"Nom incorrect ou Selection "<<arg1<<" ni Extract ni Deduct"<<endl;
    return IFSelect_RetFail;
  }
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun72
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SelModelRoots     ****
  return pilot->RecordItem (new IFSelect_SelectModelRoots);
}

static IFSelect_ReturnStatus fun73
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelRange          ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc >= 2 && arg1[0] == '?') argc = 1;
  if (argc < 2) {
    sout<<"Donner la description du SelectRange"
      <<"    Formes admises :\n <n1> <n2>  : Range de <n1> a <n2>\n"
      <<" <n1> tout seul : Range n0 <n1>\n  from <n1>  : Range From <n1>\n"
      <<"  until <n2> : Range Until <n2>"<<endl;
    return IFSelect_RetVoid;
  }

  Handle(IFSelect_IntParam) low,up;
  Handle(IFSelect_SelectRange) sel;
//                                         Range From
  if (pilot->Word(1).IsEqual("from")) {
    if (argc < 3) { sout<<"Forme admise : from <i>"<<endl; return IFSelect_RetError; }
    low = GetCasted(IFSelect_IntParam,WS->NamedItem(arg2));
    sel = new IFSelect_SelectRange;
    sel->SetFrom (low);
//                                         Range Until
  } else if (pilot->Word(1).IsEqual("until")) {
    if (argc < 3) { sout<<"Forme admise : until <i>"<<endl; return IFSelect_RetError; }
    up  = GetCasted(IFSelect_IntParam,WS->NamedItem(arg2));
    sel = new IFSelect_SelectRange;
    sel->SetUntil (up);
//                                         Range One (n-th)
  } else if (argc < 3) {
    low = GetCasted(IFSelect_IntParam,WS->NamedItem(arg1));
    sel = new IFSelect_SelectRange;
    sel->SetOne (low);
//                                         Range (from-to)
  } else {
    low = GetCasted(IFSelect_IntParam,WS->NamedItem(arg1));
    up  = GetCasted(IFSelect_IntParam,WS->NamedItem(arg2));
    sel = new IFSelect_SelectRange;
    sel->SetRange (low,up);
  }
  return pilot->RecordItem (sel);
}

static IFSelect_ReturnStatus fun74
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SelRoots          ****
  return pilot->RecordItem (new IFSelect_SelectRoots);
}

static IFSelect_ReturnStatus fun75
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SelShared         ****
  return pilot->RecordItem (new IFSelect_SelectShared);
}

static IFSelect_ReturnStatus fun76
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelDiff           ****
  Handle(IFSelect_Selection) sel = new IFSelect_SelectDiff;
  if (sel.IsNull()) return IFSelect_RetFail;
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) sout<<"Diff sans input : ne pas oublier de les definir (ctlmain, ctlsec)!"<<endl;
  DeclareAndCast(IFSelect_Selection,selmain,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,selsec ,WS->NamedItem(arg2));
  if (argc >= 2)
    if (!WS->SetControl(sel,selmain,Standard_True))
      sout<<"Echec ControlMain:"<<arg1<<" , a refaire (ctlmain)"<<endl;
  if (argc >= 3)
    if (!WS->SetControl(sel,selsec,Standard_False))
      sout<<"Echec ControlSecond:"<<arg2<<" , a refaire (ctlsec)"<<endl;
  return pilot->RecordItem (sel);
}

static IFSelect_ReturnStatus fun77
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelControlMain       ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner Noms de Control et MainInput"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,selmain,WS->NamedItem(arg2));
  if (WS->SetControl(sel,selmain,Standard_True)) return IFSelect_RetDone;
  sout<<"Nom incorrect ou Selection "<<arg1<<" pas de type Control"<<endl;
  return IFSelect_RetFail;
}

static IFSelect_ReturnStatus fun78
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelControlSecond       ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner Noms de Control et SecondInput"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,seldif,WS->NamedItem(arg2));
  if (WS->SetControl(sel,seldif,Standard_False))  return IFSelect_RetDone;
  sout<<"Nom incorrect ou Selection "<<arg1<<" pas de type Control"<<endl;
  return IFSelect_RetFail;
}

static IFSelect_ReturnStatus fun79
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SelModelAll       ****
  return pilot->RecordItem (new IFSelect_SelectModelEntities);
}

static IFSelect_ReturnStatus fun80
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelCombAdd        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner n0 Combine et une Input"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,seladd,WS->NamedItem(arg2));
  if (WS->CombineAdd(sel,seladd)) return IFSelect_RetDone;
  sout<<"Nom incorrect ou Selection "<<arg1<<" pas Combine"<<endl;
  return IFSelect_RetFail;
}

static IFSelect_ReturnStatus fun81
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
//        ****    SelCombRem        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Donner n0 Combine et RANG a supprimer"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Selection,sel,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_Selection,inp,WS->NamedItem(arg2));
  if (WS->CombineRemove(sel,inp)) return IFSelect_RetDone;
  sout<<"Nom incorrect ou Selection "<<arg1<<" ni Union ni Intersection"<<endl;
  return IFSelect_RetFail;
}

static IFSelect_ReturnStatus fun82
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SelEntNumber      ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner Nom IntParam pour n0 Entite"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_IntParam,par,WS->NamedItem(arg1));
  Handle(IFSelect_SelectEntityNumber) sel = new IFSelect_SelectEntityNumber;
  sel->SetNumber(par);
  return pilot->RecordItem (sel);
}

static IFSelect_ReturnStatus fun83
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SelUnion          ****
  return pilot->RecordItem (new IFSelect_SelectUnion);
}

static IFSelect_ReturnStatus fun84
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SelIntersection   ****
  return pilot->RecordItem (new IFSelect_SelectIntersection);
}

static IFSelect_ReturnStatus fun85
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SelTextType Exact ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner le TYPE a selectionner"<<endl; return IFSelect_RetError; }
  return pilot->RecordItem (new IFSelect_SelectSignature
			    (new IFSelect_SignType,arg1,Standard_True));
}

static IFSelect_ReturnStatus fun86
  (const Handle(IFSelect_SessionPilot)& pilot)
{
//        ****    SelErrorEntities  ****
  return pilot->RecordItem (new IFSelect_SelectErrorEntities);
}
      
static IFSelect_ReturnStatus fun87
  (const Handle(IFSelect_SessionPilot)& pilot)
{
//        ****    SelUnknownEntities  **
  return pilot->RecordItem (new IFSelect_SelectUnknownEntities);
}

static IFSelect_ReturnStatus fun88
  (const Handle(IFSelect_SessionPilot)& pilot)
{
//        ****    SelSharing        ****
  return pilot->RecordItem (new IFSelect_SelectSharing);
}

static IFSelect_ReturnStatus fun89
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SelTextType Contain **
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner le TYPE a selectionner"<<endl; return IFSelect_RetError; }
  return pilot->RecordItem (new IFSelect_SelectSignature
			    (new IFSelect_SignType,arg1,Standard_False));
}

static IFSelect_ReturnStatus fun90
  (const Handle(IFSelect_SessionPilot)& pilot)
{
//        ****    SelPointed        ****
  Handle(IFSelect_SelectPointed) sp = new IFSelect_SelectPointed;
  if (pilot->NbWords() > 1) {
    Handle(TColStd_HSequenceOfTransient) list = IFSelect_Functions::GiveList
    (pilot->Session(),pilot->CommandPart(1));
    if (list.IsNull()) return IFSelect_RetFail;
    Handle(Message_Messenger) sout = Message::DefaultMessenger();
    sout<<"SelectPointed : "<<list->Length()<<" entities"<<endl;
    sp->AddList (list);
  }
  return pilot->RecordItem (sp);
}

static IFSelect_ReturnStatus fun91
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
//        ****    SetPointed (edit) / SetList (edit)    ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) {
    sout<<"Donner NOM SelectPointed + Option(s) :\n"
        <<" aucune : liste des entites pointees\n"
	<<" 0: Clear  +nn ajout entite nn  -nn enleve nn  /nn toggle nn"<<endl;
	return IFSelect_RetError;
  }
  DeclareAndCast(IFSelect_SelectPointed,sp,WS->NamedItem(arg1));
  if (sp.IsNull()) { sout<<"Pas une SelectPointed:"<<arg1<<endl; return IFSelect_RetError; }
  const Handle(Interface_InterfaceModel) &model = WS->Model();  // pour Print
  if (argc == 2) {    // listage simple
    Standard_Integer nb = sp->NbItems();
    sout<<" SelectPointed : "<<arg1<<" : "<<nb<<" Items :"<<endl;
    for (Standard_Integer i = 1; i <= nb; i ++) {
      Handle(Standard_Transient) pointed = sp->Item(i);
      Standard_Integer id = WS->StartingNumber(pointed);
      if (id == 0) sout <<" (inconnu)";
      else  {  sout<<"  "; model->Print(pointed,sout);  }
    }
    if (nb > 0) sout<<endl;
    return IFSelect_RetDone;
  }

  for (Standard_Integer ia = 2; ia < argc ; ia ++) {
    const TCollection_AsciiString argi = pilot->Word(ia);
    Standard_Integer id = pilot->Number(&(argi.ToCString())[1]);
    if (id == 0) {
      if (!argi.IsEqual("0")) sout<<"Incorrect,ignore:"<<argi<<endl;
      else {  sout<<"Clear SelectPointed"<<endl; sp->Clear(); }
    } else if (argi.Value(1) == '-') {
      Handle(Standard_Transient) item = WS->StartingEntity(id);
      if (sp->Remove(item)) sout<<"Removed:no."<<id;
      else sout<<" Echec Remove "<<id;
      sout<<": "; model->Print(item,sout);  sout<<endl;
    } else if (argi.Value(1) == '/') {
      Handle(Standard_Transient) item = WS->StartingEntity(id);
      if (sp->Remove(item)) sout<<"Toggled:n0."<<id;
      else sout<<" Echec Toggle "<<id;
      sout<<": "; model->Print(item,sout);  sout<<endl;
    } else if (argi.Value(1) == '+') {
      Handle(Standard_Transient) item = WS->StartingEntity(id);
      if (sp->Add(item)) sout<<"Added:no."<<id;
      else sout<<" Echec Add "<<id;
      sout<<": "; model->Print(item,sout);  sout<<endl;
    } else {
      sout<<"Ignore:"<<argi<<" , donner n0 PRECEDE de + ou - ou /"<<endl;
    }
  }
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun92
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SelIncorrectEntities  ****
  WS->ComputeCheck();
  return pilot->RecordItem (new IFSelect_SelectIncorrectEntities);
}

static IFSelect_ReturnStatus fun93
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SelSignature        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Give name of Signature or Counter, text + option exact(D) else contains"<<endl; return IFSelect_RetError; }
  Standard_Boolean exact = Standard_True;
  if (argc > 3) { if (pilot->Arg(3)[0] == 'c') exact = Standard_False; }

  DeclareAndCast(IFSelect_Signature,sign,WS->NamedItem(arg1));
  DeclareAndCast(IFSelect_SignCounter,cnt,WS->NamedItem(arg1));
  Handle(IFSelect_SelectSignature) sel;

  if (!sign.IsNull())     sel = new IFSelect_SelectSignature (sign,arg2,exact);
  else if (!cnt.IsNull()) sel = new IFSelect_SelectSignature (cnt,arg2,exact);
  else { sout<<arg1<<":neither Signature nor Counter"<<endl; return IFSelect_RetError; }

  return pilot->RecordItem(sel);
}

static IFSelect_ReturnStatus fun94
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    SignCounter        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner nom signature"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_Signature,sign,WS->NamedItem(arg1));
  if (sign.IsNull()) { sout<<arg1<<":pas une signature"<<endl; return IFSelect_RetError; }
  Handle(IFSelect_SignCounter) cnt = new IFSelect_SignCounter (sign,Standard_True,Standard_True);
  return pilot->RecordItem(cnt);
}

static IFSelect_ReturnStatus funbselected
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  const Standard_CString arg1 = pilot->Arg(1);
  Handle(IFSelect_WorkSession) WS = pilot->Session();
//        ****    NbSelected = GraphCounter        ****
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Donner nom selection (deduction) a appliquer"<<endl; return IFSelect_RetError; }
  DeclareAndCast(IFSelect_SelectDeduct,applied,WS->GiveSelection(arg1));
  if (applied.IsNull()) { sout<<arg1<<":pas une SelectDeduct"<<endl; return IFSelect_RetError; }
  Handle(IFSelect_GraphCounter) cnt = new IFSelect_GraphCounter (Standard_True,Standard_True);
  cnt->SetApplied (applied);
  return pilot->RecordItem(cnt);
}

//  #########################################
//  ####    EDITOR  -  EDITFORM          ####
//  #########################################

static IFSelect_ReturnStatus fun_editlist
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give the name of an EditForm or an Editor"<<endl;
		  return IFSelect_RetError;  }
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(IFSelect_WorkSession) WS = pilot->Session();

//  EditForm

  DeclareAndCast(IFSelect_EditForm,edf,WS->NamedItem(arg1));
  Handle(IFSelect_Editor) edt;
  if (!edf.IsNull()) {
    sout<<"Print EditForm "<<arg1<<endl;
    edt = edf->Editor();
    if (argc < 3) {

//       DEFINITIONS : Editor (direct ou via EditForm)

      if (edt.IsNull()) edt = GetCasted(IFSelect_Editor,WS->NamedItem(arg1));
      if (edt.IsNull()) return IFSelect_RetVoid;

      sout<<"Editor, Label : "<<edt->Label()<<endl;
      sout<<endl<<" --  Names (short - complete) + Labels of Values"<<endl;
      edt->PrintNames(sout);
      sout<<endl<<" --  Definitions  --"<<endl;
      edt->PrintDefs (sout);
      if (!edf.IsNull()) {
	edf->PrintDefs(sout);
	sout<<endl<<"To display values, add an option : o original  f final  m modified"<<endl;
      }

      return IFSelect_RetVoid;

    } else {
      char opt = arg2[0];
      Standard_Integer what = 0;
      if (opt == 'o') what = -1;
      else if (opt == 'f') what = 1;

      edf->PrintValues (sout,what,Standard_False);
    }
  }

  return IFSelect_RetVoid;
}

static IFSelect_ReturnStatus fun_editvalue
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 3) { sout<<"Give the name of an EditForm + name of Value [+ newvalue or . to nullify]"<<endl;
		  return IFSelect_RetError;  }
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  DeclareAndCast(IFSelect_EditForm,edf,WS->NamedItem(arg1));
  if (edf.IsNull())
    {  sout<<"Not an EditForm : "<<arg1<<endl; return IFSelect_RetError;  }
  Standard_Integer num = edf->NameNumber (arg2);
  if (num == 0) sout<<"Unknown Value Name : "<<arg2<<endl;
  if (num <  0) sout<<"Not Extracted Value Name : "<<arg2<<endl;
  if (num <= 0) return IFSelect_RetError;

  Standard_Boolean islist = edf->Editor()->IsList(num);
  Standard_CString name = edf->Editor()->Name(num,Standard_True); // vrai nom
  Handle(TColStd_HSequenceOfHAsciiString) listr;
  Handle(TCollection_HAsciiString) str;
  sout<<"Value Name : "<<name<<(edf->IsModified(num) ? "(already edited) : " : " : ");

  if (islist) {
    listr = edf->EditedList(num);
    if (listr.IsNull()) sout<<"(NULL LIST)"<<endl;
    else {
      Standard_Integer ilist,nblist = listr->Length();
      sout<<"(List : "<<nblist<<" Items)"<<endl;
      for (ilist = 1; ilist <= nblist; ilist ++) {
	str = listr->Value(ilist);
	sout<<"  ["<<ilist<<"]	"<< (str.IsNull() ? "(NULL)" : str->ToCString())<<endl;
      }
    }
    if (argc < 4) sout<<"To Edit, options by editval edit-form value-name ?"<<endl;
  } else {
    str = edf->EditedValue (num);
    sout<<(str.IsNull() ? "(NULL)" : str->ToCString())<<endl;
  }
  if (argc < 4) return IFSelect_RetVoid;

//  Valeur simple ou liste ?
  Standard_Integer numarg = 3;
  str.Nullify();

  const Standard_CString argval = pilot->Arg(numarg);
  if (islist) {
    if (argval[0] == '?') {
      sout<<"To Edit, options"<<endl<<" + val : add value at end (blanks allowed)"
	<<endl<<" +nn text : insert val before item nn"<<endl
	<<" nn text : replace item nn with a new value"<<endl
	<<" -nn : remove item nn"<<endl<<" . : clear the list"<<endl;
      return IFSelect_RetVoid;
    }
    Standard_Boolean stated = Standard_False;
    Handle(IFSelect_ListEditor) listed = edf->ListEditor (num);
    if (listed.IsNull()) return IFSelect_RetError;
    if (argval[0] == '.') { listr.Nullify();  stated = listed->LoadEdited(listr); }
    else if (argval[0] == '+') {
      Standard_Integer numadd = 0;
      if (argval[1] != '\0') numadd = atoi(argval);
      stated = listed->AddValue (new TCollection_HAsciiString(pilot->CommandPart(numarg+1)),numadd);
    }
    else if (argval[0] == '-') {
      Standard_Integer numrem = atoi(argval);
      stated = listed->Remove(numrem);
    }
    else {
      Standard_Integer numset = atoi(argval);
      if (numset > 0) stated = listed->AddValue
	(new TCollection_HAsciiString(pilot->CommandPart(numarg+1)),numset);
    }
    if (stated) stated = edf->ModifyList (num,listed,Standard_True);
    if (stated) sout<<"List Edition done"<<endl;
    else sout<<"List Edition not done, option"<<argval<<endl;
  } else {
    if (argval[0] == '.' && argval[1] == '\0') str.Nullify();
    else str = new TCollection_HAsciiString (pilot->CommandPart(numarg));
    if (edf->Modify (num,str,Standard_True)) {
      sout<<"Now set to "<<(str.IsNull() ? "(NULL)" : str->ToCString())<<endl;
    } else {
      sout<<"Modify not done"<<endl;  return IFSelect_RetFail;
    }
  }
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun_editclear
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give the name of an EditForm [+ name of Value  else all]"<<endl;
		  return IFSelect_RetError;  }
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  DeclareAndCast(IFSelect_EditForm,edf,WS->NamedItem(arg1));
  if (edf.IsNull())
    {  sout<<"Not an EditForm : "<<arg1<<endl; return IFSelect_RetError;  }
  if (argc < 3) { edf->ClearEdit(); sout<<"All Modifications Cleared"<<endl; }
  else {
    Standard_Integer num = edf->NameNumber (arg2);
    if (num == 0) sout<<"Unknown Value Name : "<<arg2<<endl;
    if (num <  0) sout<<"Not Extracted Value Name : "<<arg2<<endl;
    if (num <= 0) return IFSelect_RetError;
    if (!edf->IsModified(num))
      { sout<<"Value "<<arg2<<" was not modified"<<endl; return IFSelect_RetVoid; }
    edf->ClearEdit (num);
    sout<<"Modification on Value "<<arg2<<" Cleared"<<endl;
  }
  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun_editapply
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give the name of an EditForm [+ option keep to re-apply edited values]"<<endl;
		  return IFSelect_RetError;  }
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  DeclareAndCast(IFSelect_EditForm,edf,WS->NamedItem(arg1));
  if (edf.IsNull())
    {  sout<<"Not an EditForm : "<<arg1<<endl; return IFSelect_RetError;  }

  Handle(Standard_Transient) ent = edf->Entity();
  Handle(Interface_InterfaceModel) model = edf->Model();
  if (!model.IsNull()) {
    if (ent.IsNull()) sout<<"Applying modifications on loaded model"<<endl;
    else {
      sout<<"Applying modifications on loaded entity : ";
      model->PrintLabel (ent,sout);
    }
  }
  else sout<<"Applying modifications"<<endl;

  if (!edf->ApplyData (edf->Entity(),edf->Model())) {
    sout<<"Modifications could not be applied"<<endl;
    return IFSelect_RetFail;
  }
  sout<<"Modifications have been applied"<<endl;

  Standard_Boolean stat = Standard_True;
  if (argc > 2 && arg2[0] == 'k') stat = Standard_False;
  if (stat) {
    edf->ClearEdit();
    sout<<"Edited values are cleared"<<endl;
  }
  else sout<<"Edited values are kept for another loading/applying"<<endl;

  return IFSelect_RetDone;
}

static IFSelect_ReturnStatus fun_editload
  (const Handle(IFSelect_SessionPilot)& pilot)
{
  Standard_Integer argc = pilot->NbWords();
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  if (argc < 2) { sout<<"Give the name of an EditForm [+ Entity-Ident]"<<endl;
		  return IFSelect_RetError;  }
  const Standard_CString arg1 = pilot->Arg(1);
  const Standard_CString arg2 = pilot->Arg(2);
  Handle(IFSelect_WorkSession) WS = pilot->Session();
  DeclareAndCast(IFSelect_EditForm,edf,WS->NamedItem(arg1));
  if (edf.IsNull())
    {  sout<<"Not an EditForm : "<<arg1<<endl; return IFSelect_RetError;  }

  Standard_Integer num = (argc < 3 ? 0 : pilot->Number (arg2));
  Standard_Boolean stat = Standard_False;
  if (argc < 3) {
    sout<<"EditForm "<<arg1<<" : Loading Model"<<endl;
    stat = edf->LoadModel(WS->Model());
  } else if (num <= 0) {
    sout<<"Not an entity ident : "<<arg2<<endl;
    return IFSelect_RetError;
  } else {
    sout<<"EditForm "<<arg1<<" : Loading Entity "<<arg2<<endl;
    stat = edf->LoadData (WS->StartingEntity(num),WS->Model());
  }

  if (!stat) {
    sout<<"Loading not done"<<endl;
    return IFSelect_RetFail;
  }
  sout<<"Loading done"<<endl;
  return IFSelect_RetDone;
}

//  #########################################
//  ####    FONCTIONS COMPLEMENTAIRES    ####
//  #########################################

    Handle(Standard_Transient)  IFSelect_Functions::GiveEntity
  (const Handle(IFSelect_WorkSession)& WS,
   const Standard_CString name)
{
  Handle(Standard_Transient) ent;  // demarre a Null
  Standard_Integer num = GiveEntityNumber(WS,name);
  if (num > 0) ent = WS->StartingEntity(num);
  return ent;
}

    Standard_Integer  IFSelect_Functions::GiveEntityNumber
  (const Handle(IFSelect_WorkSession)& WS,
   const Standard_CString name)
{
  Standard_Integer num = 0;
  if (!name || name[0] == '\0') {
    char ligne[80];  ligne[0] = '\0';
    cin >> ligne;
//    cin.clear();  cin.getline (ligne,79);
    if (ligne[0] == '\0') return 0;
    num    = WS->NumberFromLabel (ligne);
  }
  else num = WS->NumberFromLabel (name);
  return num;
}

    Handle(TColStd_HSequenceOfTransient)  IFSelect_Functions::GiveList
  (const Handle(IFSelect_WorkSession)& WS,
   const Standard_CString first, const Standard_CString second)
{
  return WS->GiveList (first,second);
}


//  Function which returns an EVALUATED DISPATCH
//   (could be added in WorkSession.cdl ...)
//  Two modes : returns dispatch as it is, or return with edition
//  Dispatch Name can be : an immediate name of already recorded Dispatch
//  Or a name of dispatch + a parameter :  dispatch-name(param-value)
//  According to type of Dispatch : integer , signature name

Handle(IFSelect_Dispatch)  IFSelect_Functions::GiveDispatch
  (const Handle(IFSelect_WorkSession)& WS,
   const Standard_CString name, const Standard_Boolean mode)
{
  DeclareAndCast(IFSelect_Dispatch,disp,WS->NamedItem(name));
  if (!disp.IsNull()) return disp;    // OK as it is given

//   Else, let s try special cases
  TCollection_AsciiString nam(name);
  Standard_Integer paro = nam.Location(1,'(',1,nam.Length());
  Standard_Integer parf = nam.Location(1,')',1,nam.Length());
  nam.SetValue(paro,'\0'); nam.SetValue(parf,'\0');
  if (paro <= 0 &&parf <= 0) return disp;
  disp = GetCasted(IFSelect_Dispatch,WS->NamedItem(nam.ToCString()));
  if (disp.IsNull()) return disp;     // KO anyway

//  According to the type of dispatch :
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
  DeclareAndCast(IFSelect_DispPerCount,dc,disp);
  if (!dc.IsNull()) {
    Standard_Integer nb = atoi( &(nam.ToCString())[paro]);
    if (nb <= 0) {
      sout<<" DispPerCount, count is not positive"<<endl;
      disp.Nullify();
      return disp;
    }
    if (mode) {
      Handle(IFSelect_IntParam) val = new IFSelect_IntParam;
      val->SetValue(nb);
      dc->SetCount (val);
    }
    return dc;
  }
  DeclareAndCast(IFSelect_DispPerFiles,dp,disp);
  if (!dp.IsNull()) {
    Standard_Integer nb = atoi( &(nam.ToCString())[paro]);
    if (nb <= 0) {
      sout<<" DispPerFiles, count is not positive"<<endl;
      disp.Nullify();
      return disp;
    }
    if (mode) {
      Handle(IFSelect_IntParam) val = new IFSelect_IntParam;
      val->SetValue(nb);
      dp->SetCount (val);
    }
    return dp;
  }
  DeclareAndCast(IFSelect_DispPerSignature,ds,disp);
  if (!ds.IsNull()) {
    DeclareAndCast(IFSelect_Signature,sg,WS->NamedItem( &(nam.ToCString())[paro]));
    if (sg.IsNull()) {
      sout<<"DispPerSignature "<<nam<<" , Signature not valid : "<<&(nam.ToCString())[paro]<<endl;
      disp.Nullify();
      return disp;
    }
    if (mode) ds->SetSignCounter (new IFSelect_SignCounter(sg));
    return ds;
  }
  sout<<"Dispatch : "<<name<<" , Parameter : "<<&(nam.ToCString())[paro]<<endl;
  return disp;
}


//  #########################################
//  ####    INITIALISATIONS              ####
//  #########################################

static int THE_IFSelect_Functions_initactor = 0;

void IFSelect_Functions::Init()
{
  if (THE_IFSelect_Functions_initactor)
  {
    return;
  }

  THE_IFSelect_Functions_initactor = 1;
  IFSelect_Act::SetGroup("DE: General");
  IFSelect_Act::AddFunc("xstatus","Lists XSTEP Status : Version, System Name ...",funstatus);
  IFSelect_Act::AddFunc("handler","Toggle status catch Handler Error of the session",fun1);
  IFSelect_Act::AddFunc("xload","file:string  : Read File -> Load Model",fun3);
  IFSelect_Act::AddFunc("xread","file:string  : Read File -> Load Model",fun3);
  IFSelect_Act::AddFunc("writeall","file:string  : Write all model (no split)",fun4);
  IFSelect_Act::AddFunc("writesel","file:string sel:Selection : Write Selected (no split)",fun5);
  IFSelect_Act::AddFunc("writeent","file:string  n1ent n2ent...:integer : Write Entite(s) (no split)",fun6);
  IFSelect_Act::AddFunc("writent", "file:string  n1ent n2ent...:integer : Write Entite(s) (no split)",fun6);
  IFSelect_Act::AddFunc("elabel","nument:integer   : Displays Label Model of an entity",fun7);
  IFSelect_Act::AddFunc("enum","label:string  : Displays entities n0.s of which Label Model ends by..",fun8);

  IFSelect_Act::AddFunc("listtypes","List nb entities per type. Optional selection name  else all model",fun9);
  IFSelect_Act::AddFunc("count","Count : counter [selection]",funcount);
  IFSelect_Act::AddFunc("listcount","List Counted : counter [selection [nument]]",funcount);
  IFSelect_Act::AddFunc("sumcount","Summary Counted : counter [selection [nument]]",funcount);
  IFSelect_Act::AddFunc("signtype","Sign Type [newone]",funsigntype);
  IFSelect_Act::AddFunc("signcase","signature : displays possible cases",funsigncase);

  IFSelect_Act::AddFunc("estatus","ent/nument : displays status of an entity",fun10);
  IFSelect_Act::AddFunc("data","Data (DumpModel); whole help : data tout court",fun11);
  IFSelect_Act::AddFunc("entity","give n0 ou id of entity [+ level]",fundumpent);
  IFSelect_Act::AddFunc("signature","signature name + n0/ident entity",funsign);
  IFSelect_Act::AddFunc("queryparent"," give 2 n0s/labels of entities : dad son",funqp);

  IFSelect_Act::AddFunc("dumpshare","Dump Share (dispatches, IntParams)",fun12);
  IFSelect_Act::AddFunc("listitems","List Items [label else all]  ->Type,Label[,Name]",fun13);
  IFSelect_Act::AddFSet("integer","value:integer : cree un IntParam",fun14);
  IFSelect_Act::AddFunc("setint","name:IntParam   newValue:integer  : Change valeur IntParam",fun15);
  IFSelect_Act::AddFSet("text","value:string  : cree un TextParam",fun16);
  IFSelect_Act::AddFunc("settext","Name:TextParam  newValue:string   : Change valeur TextParam",fun17);
  IFSelect_Act::AddFunc("dumpsel","Dump Selection suivi du Nom de la Selection a dumper",fun19);
  IFSelect_Act::AddFunc("evalsel","name:Selection [num/sel]  : Evalue une Selection",fun20);
  IFSelect_Act::AddFunc("givelist","num/sel [num/sel ...]  : Evaluates GiveList",fun20);
  IFSelect_Act::AddFunc("giveshort","num/sel [num/sel ...]  : GiveList in short form",fun20);
  IFSelect_Act::AddFunc("givepointed","num/sel [num/sel ...]  : GiveList to fill a SelectPointed",fun20);
  IFSelect_Act::AddFunc("makelist","listname [givelist] : Makes a List(SelectPointed) from GiveList",fun20);
  IFSelect_Act::AddFunc("givecount","num/sel [num/sel ...]  : Counts GiveList",fun20c);
  IFSelect_Act::AddFSet("selsuite","sel sel ...  : Creates a SelectSuite",funselsuite);
  IFSelect_Act::AddFunc("clearitems","Clears all items (selections, dispatches, etc)",fun21);
  IFSelect_Act::AddFunc("cleardata","mode:a-g-c-p  : Clears all or some data (model, check...)",fun22);

  IFSelect_Act::AddFunc("itemlabel","xxx xxx : liste items having this label",fun24);
  IFSelect_Act::AddFunc("xsave","filename:string  : sauve items-session",fun25);
  IFSelect_Act::AddFunc("xrestore","filename:string  : restaure items-session",fun26);
  IFSelect_Act::AddFunc("param","nompar:string : displays parameter value; + nompar val : changes it",fun27);
  IFSelect_Act::AddFunc("defparam","nompar:string : display def. param; also : nompar edit, nompar init",fun28);

  IFSelect_Act::AddFunc("sentfiles","Lists files sent from last Load",fun29);
  IFSelect_Act::AddFunc("fileprefix","prefix:string    : definit File Prefix",fun30);
  IFSelect_Act::AddFunc("fileext","extent:string    : definit File Extension",fun31);
  IFSelect_Act::AddFunc("fileroot","disp:Dispatch  root:string  : definit File Root sur un Dispatch",fun32);
  IFSelect_Act::AddFunc("filedef","defroot:string   : definit File DefaultRoot",fun33);
  IFSelect_Act::AddFunc("evalfile","Evaluation du FileNaming et memorisation",fun34);
  IFSelect_Act::AddFunc("clearfile","Efface la liste d'EvalFile",fun35);
  IFSelect_Act::AddFunc("xsplit","[disp:Dispatch  sinon tout]  : Split, la grande affaire !",fun36);
  IFSelect_Act::AddFunc("remaining","options... : Remaining Entities, help complet par  remaining ?",fun37);
  IFSelect_Act::AddFunc("setcontent","sel:Selection mode:k ou r  : Restreint contenu du modele",fun38);

  IFSelect_Act::AddFunc("listmodif","List Final Modifiers",fun40);
  IFSelect_Act::AddFunc("dumpmodif","modif:Modifier  : Affiche le Statut d'un Modifier",fun41);
  IFSelect_Act::AddFunc("modifsel","modif:Modifier [sel:Selection]  : Change/Annule Selection de Modifier",fun42);
  IFSelect_Act::AddFunc("setapplied","modif:Modifier [name:un item sinon sortie fichier]  : Applique un Modifier",fun43);
  IFSelect_Act::AddFunc("resetapplied","modif:Modifier  : Enleve un Modifier de la sortie fichier",fun44);
  IFSelect_Act::AddFunc("modifmove","modif:Modifier M(model)/F(file) avant,apres:integer  : Deplace un Modifier (sortie fichier)",fun45);

  IFSelect_Act::AddFunc("dispsel","disp:Dispatch sel:Selection  -> Selection Finale de Dispatch",fun51);
  IFSelect_Act::AddFSet("dispone","cree DispPerOne",fun_dispone);
  IFSelect_Act::AddFSet("dispglob","cree DispGlobal",fun_dispglob);
  IFSelect_Act::AddFSet("dispcount","count:IntParam  : cree DispPerCount",fun_dispcount);
  IFSelect_Act::AddFSet("dispfile","files:IntParam  : cree DispPerFiles",fun_dispfiles);
  IFSelect_Act::AddFSet("dispsign","sign:Signature  : cree DispPerSignature",fun_dispsign);
  IFSelect_Act::AddFunc("dumpdisp","disp:Dispatch   : Affiche le Statut d'un Dispatch",fun56);

  IFSelect_Act::AddFunc("xremove","nom  : Remove a Control Item de la Session",fun57);
  IFSelect_Act::AddFunc("evaldisp","mode=[0-3]  disp:Dispatch  : Evaluates one or more Dispatch(es)",fun58);
  IFSelect_Act::AddFunc("evaladisp","mode=[0-3]  disp:Dispatch [givelist]  : Evaluates a Dispatch (on a GiveList)",fun_evaladisp);
  IFSelect_Act::AddFunc("writedisp","filepattern  disp:Dispatch [givelist]  : Writes Entities by Splitting by a Dispatch",fun_writedisp);
  IFSelect_Act::AddFunc("evalcomplete","Evaluation Complete de la Repartition",fun59);

  IFSelect_Act::AddFunc("runcheck","affiche LastRunCheckList (write,modif)",fun60);
  IFSelect_Act::AddFunc("runtranformer","transf:Transformer  : Applique un Transformer",fun61);
  IFSelect_Act::AddFSet("copy","cree TransformStandard, option Copy, vide",fun62);
  IFSelect_Act::AddFSet("onthespot","cree TransformStandard, option OntheSpot, vide",fun63);
  IFSelect_Act::AddFunc("runcopy","modif:ModelModifier [givelist] : Run <modif> via TransformStandard option Copy",fun6465);
  IFSelect_Act::AddFunc("runonthespot","modif:ModelModifier [givelist] : Run <modif> via TransformStandard option OnTheSpot",fun6465);
  IFSelect_Act::AddFSet("reorder","[f ou t] reordonne le modele",fun66);

  IFSelect_Act::AddFunc("toggle","sel:Selection genre Extract  : Toggle Direct/Reverse",fun70);
  IFSelect_Act::AddFunc("input","sel:Selection genre Deduct ou Extract  input:Selection  : Set Input",fun71);
  IFSelect_Act::AddFSet("modelroots","cree SelectModelRoots",fun72);
  IFSelect_Act::AddFSet("range","options... : cree SelectRange ...; tout court pour help",fun73);
  IFSelect_Act::AddFSet("roots","cree SelectRoots (local roots)",fun74);
  IFSelect_Act::AddFSet("shared","cree SelectShared",fun75);
  IFSelect_Act::AddFSet("diff","[main:Selection diff:Selection]  : cree SelectDiff",fun76);
  IFSelect_Act::AddFunc("selmain","sel:Selection genre Control  main:Selection  : Set Main Input",fun77);
  IFSelect_Act::AddFunc("selsecond","sel:Selection genre Control  sec:Selection   : Set Second Input",fun78);
  IFSelect_Act::AddFSet("modelall","cree SelectModelAll",fun79);
  IFSelect_Act::AddFunc("seladd","sel:Selection genre Combine  input:Selection  : Add Selection",fun80);
  IFSelect_Act::AddFunc("selrem","sel:Selection genre Combine  input:Selection  : Remove Selection",fun81);
  IFSelect_Act::AddFSet("number","num:IntParam  : Cree SelectEntityNumber",fun82);

  IFSelect_Act::AddFSet("union","cree SelectUnion (vide), cf aussi combadd, combrem",fun83);
  IFSelect_Act::AddFSet("intersect","cree SelectIntersection (vide), cf aussi combadd, combrem",fun84);
  IFSelect_Act::AddFSet("typexact","type:string  : cree SelectTextType Exact",fun85);
  IFSelect_Act::AddFSet("errors","cree SelectErrorEntities (from file)",fun86);
  IFSelect_Act::AddFSet("unknown","cree SelectUnknownEntities",fun87);
  IFSelect_Act::AddFSet("sharing","cree SelectSharing",fun88);
  IFSelect_Act::AddFSet("typecontain","type:string  : cree SelectTextType Contains",fun89);
  IFSelect_Act::AddFSet("pointed","cree SelectPointed [num/sel num/sel]",fun90);
  IFSelect_Act::AddFunc("setpointed","sel:SelectPointed  : edition SelectPointed. tout court pour help",fun91);
  IFSelect_Act::AddFunc("setlist","sel:SelectPointed  : edition SelectPointed. tout court pour help",fun91);
  IFSelect_Act::AddFSet("incorrect","cree SelectIncorrectEntities (computed)",fun92);

  IFSelect_Act::AddFSet("signsel","sign:Signature|cnt:Counter text:string [e(D)|c] : cree SelectSignature",fun93);
  IFSelect_Act::AddFSet("signcounter","sign:Signature : cree SignCounter",fun94);
  IFSelect_Act::AddFSet("nbselected","applied:Selection : cree GraphCounter(=NbSelected)",funbselected);

  IFSelect_Act::AddFunc("editlist","editor or editform : lists defs + values",fun_editlist);
  IFSelect_Act::AddFunc("editvalue","editform paramname [newval or .] : lists-changes a value",fun_editvalue);
  IFSelect_Act::AddFunc("editclear","editform [paramname] : clears edition on all or one param",fun_editclear);
  IFSelect_Act::AddFunc("editload","editform [entity-id] : loads from model or an entity",fun_editload);
  IFSelect_Act::AddFunc("editapply","editform [keep] : applies on loaded data",fun_editapply);
}
