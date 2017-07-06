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


#include <IFSelect_AppliedModifiers.hxx>
#include <IFSelect_ContextModif.hxx>
#include <IFSelect_ContextWrite.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <IFSelect_ModelCopier.hxx>
#include <IFSelect_Modifier.hxx>
#include <IFSelect_Selection.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_ShareOutResult.hxx>
#include <IFSelect_WorkLibrary.hxx>
#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HSequenceOfInteger.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_ModelCopier,Standard_Transient)

//#define MISOPOINT
IFSelect_ModelCopier::IFSelect_ModelCopier ()    {  }

    void  IFSelect_ModelCopier::SetShareOut
  (const Handle(IFSelect_ShareOut)& sho)
      {  theshareout = sho;  }


//  ########################################################################
//  ########    OPERATIONS DE TRANSFERT GLOBAL (memorise ou non)    ########


    void  IFSelect_ModelCopier::ClearResult ()
      {  thefilemodels.Clear();  thefilenames.Clear();  theapplieds.Clear();
         theremain.Nullify();  }


    Standard_Boolean  IFSelect_ModelCopier::AddFile
  (const TCollection_AsciiString& filename,
   const Handle(Interface_InterfaceModel)& content)
{
  Standard_Integer nb = thefilenames.Length();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (filename.IsEmpty()) continue;
    if (thefilenames(i).IsEqual(filename)) return Standard_False;
  }
  Handle(IFSelect_AppliedModifiers) nulapplied;
  thefilenames.Append  (filename);
  thefilemodels.Append (content);
  theapplieds.Append   (nulapplied);
  return Standard_True;
}

    Standard_Boolean  IFSelect_ModelCopier::NameFile
  (const Standard_Integer num,
   const TCollection_AsciiString& filename)
{
  Standard_Integer nb = thefilenames.Length();
  if (num <= 0 || num > nb) return Standard_False;
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (filename.IsEmpty()) continue;
    if (thefilenames(i).IsEqual(filename)) return Standard_False;
  }
  thefilenames.SetValue(num,filename);
  return Standard_True;
}

    Standard_Boolean  IFSelect_ModelCopier::ClearFile
  (const Standard_Integer num)
{
  Standard_Integer nb = thefilenames.Length();
  if (num <= 0 || num > nb) return Standard_False;
  thefilenames.ChangeValue(num).Clear();
  return Standard_True;
}

    Standard_Boolean  IFSelect_ModelCopier::SetAppliedModifiers
  (const Standard_Integer num, const Handle(IFSelect_AppliedModifiers)& applied)
{
  Standard_Integer nb = theapplieds.Length();
  if (num <= 0 || num > nb) return Standard_False;
  theapplieds.SetValue(num,applied);
  return Standard_True;
}

    Standard_Boolean  IFSelect_ModelCopier::ClearAppliedModifiers
  (const Standard_Integer num)
{
  Standard_Integer nb = theapplieds.Length();
  if (num <= 0 || num > nb) return Standard_False;
  theapplieds.ChangeValue(num).Nullify();
  return Standard_True;
}

//  ....    Copy : Opere les Transferts, les Memorise (pas d envoi fichier ici)

      Interface_CheckIterator IFSelect_ModelCopier::Copy
  (IFSelect_ShareOutResult& eval,
   const Handle(IFSelect_WorkLibrary)& WL,
   const Handle(Interface_Protocol)& protocol) 
{
  Interface_CopyTool TC (eval.Graph().Model(), protocol);
  return Copying (eval,WL,protocol,TC);
}

//  Copy Interne

    Interface_CheckIterator  IFSelect_ModelCopier::Copying
  (IFSelect_ShareOutResult& eval,
   const Handle(IFSelect_WorkLibrary)& WL,
   const Handle(Interface_Protocol)& protocol,
   Interface_CopyTool& TC)
{
  Message::DefaultMessenger() <<
    "** WorkSession : Copying split data before sending"<<endl;
  const Interface_Graph& G = eval.Graph();
  Interface_CheckIterator checks;
  theshareout = eval.ShareOut();
  theremain = new TColStd_HArray1OfInteger(0,G.Size()); theremain->Init(0);
  for (eval.Evaluate(); eval.More(); eval.Next()) {
    Handle(Interface_InterfaceModel) model;
    TCollection_AsciiString filename = eval.FileName();
    Standard_Integer dispnum = eval.DispatchRank();
    Standard_Integer numod, nbmod;
    eval.PacketsInDispatch (numod,nbmod);
    Handle(IFSelect_AppliedModifiers) curapp;
    CopiedModel (G, WL,protocol, eval.PacketRoot(), filename,dispnum,numod, TC,
		 model, curapp,checks);

    AddFile (filename, model);
    theapplieds.SetValue (theapplieds.Length(), curapp);
  }
  theshareout->SetLastRun (theshareout->NbDispatches());
  checks.SetName ("X-STEP WorkSession : Split Copy (no Write)");
  return checks;
}

//  Send a deux arguments : Envoi Fichier du Resultat deja memorise

    Interface_CheckIterator  IFSelect_ModelCopier::SendCopied
  (const Handle(IFSelect_WorkLibrary)& WL,
   const Handle(Interface_Protocol)& protocol)
{
  Message::DefaultMessenger() <<
    "** WorkSession : Sending split data already copied"<<endl;
  Standard_Integer nb = NbFiles();
  Interface_CheckIterator checks;
  if (nb > 0) {
    for (Standard_Integer i = 1; i <= nb; i ++) {
      if (FileName(i).Length() == 0) continue;
      Handle(IFSelect_AppliedModifiers) curapp = theapplieds.Value(i);
      IFSelect_ContextWrite ctx (FileModel(i),protocol,curapp,FileName(i).ToCString());
      Standard_Boolean res = WL->WriteFile (ctx);
      Interface_CheckIterator checklst = ctx.CheckList();
      checks.Merge(checklst);
//	(FileName(i).ToCString(), FileModel(i),protocol,curapp,checks);
//      if (!checks.IsEmpty(Standard_False)) {
//	sout<<"  **  On Sending File n0."<<i<<", Check Messages :  **"<<endl;
//	checks.Print (sout,Standard_False);
//      }
      if (!res) {
	char mess[100];  sprintf(mess,"Split Send (WriteFile) abandon on file n0.%d",i);
	checks.CCheck(0)->AddFail (mess);
	Message::DefaultMessenger() << 
	  "  **  Sending File n0."<<i<<" has failed, abandon  **"<<endl;
	return checks;
      }
      AddSentFile (FileName(i).ToCString());
    }
    ClearResult();
  }
  checks.SetName ("X-STEP WorkSession : Split Send (Copy+Write)");
  return checks;
}


//  .... Send a 4 arguments : Calcul du Transfert et Envoi sur Fichier

    Interface_CheckIterator  IFSelect_ModelCopier::Send
  (IFSelect_ShareOutResult& eval,
   const Handle(IFSelect_WorkLibrary)& WL,
   const Handle(Interface_Protocol)& protocol)
{
  Interface_CopyTool TC (eval.Graph().Model(), protocol);
  return Sending (eval,WL,protocol,TC);
}

    Interface_CheckIterator  IFSelect_ModelCopier::Sending
  (IFSelect_ShareOutResult& eval,
   const Handle(IFSelect_WorkLibrary)& WL,
   const Handle(Interface_Protocol)& protocol,
   Interface_CopyTool& TC)
{
  const Interface_Graph& G = eval.Graph();
  Interface_CheckIterator checks;
  Standard_Integer i = 0;
  Message::DefaultMessenger() <<
    "** WorkSession : Copying then sending split data"<<endl;
  theshareout = eval.ShareOut();
  theremain = new TColStd_HArray1OfInteger(0,G.Size()); theremain->Init(0);
  for (eval.Evaluate(); eval.More(); eval.Next()) {
    i ++;
    Handle(Interface_InterfaceModel) model;
    TCollection_AsciiString filename = eval.FileName();
    Standard_Integer dispnum = eval.DispatchRank();
    Standard_Integer numod, nbmod;
    eval.PacketsInDispatch (numod,nbmod);
    Handle(IFSelect_AppliedModifiers) curapp;
    CopiedModel (G, WL,protocol, eval.PacketRoot(), filename,dispnum,numod, TC,
		 model, curapp, checks);
    IFSelect_ContextWrite ctx (model,protocol,curapp,filename.ToCString());
    Standard_Boolean res = WL->WriteFile (ctx);
    Interface_CheckIterator checklst = ctx.CheckList();
    checks.Merge(checklst);
//      (filename.ToCString(), model, protocol, curapp, checks);
//    if (!checks.IsEmpty(Standard_False)) {
//      sout<<"  **  On Sending File "<<filename<<", Check Messages :  **"<<endl;
//      checks.Print (sout,model,Standard_False);
//    }
    if (!res) {
      char mess[100];  sprintf(mess,"Split Send (WriteFile) abandon on file n0.%d",i);
      checks.CCheck(0)->AddFail (mess);
      Message::DefaultMessenger() <<
	"  **  Sending File "<<filename<<" has failed, abandon  **"<<endl;
      checks.SetName ("X-STEP WorkSession : Split Send (only Write)");
      return checks;
    }
    AddSentFile (filename.ToCString());
  }
  theshareout->SetLastRun (theshareout->NbDispatches());
  checks.SetName ("X-STEP WorkSession : Split Send (only Write)");
  return checks;
}


//  .... SendAll : Donnees a tranferer dans G, aucun split, envoi sur fichier

    Interface_CheckIterator  IFSelect_ModelCopier::SendAll
  (const Standard_CString filename,   const Interface_Graph& G,
   const Handle(IFSelect_WorkLibrary)& WL,
   const Handle(Interface_Protocol)& protocol)
{
  Interface_CheckIterator checks;
  checks.SetName ("X-STEP WorkSession : Send All");
  Message::DefaultMessenger() <<
    "** WorkSession : Sending all data"<<endl;
  Handle(Interface_InterfaceModel)  model = G.Model();
  if (model.IsNull() || protocol.IsNull() || WL.IsNull()) return checks;

  Interface_CopyTool TC (model, protocol);
  Standard_Integer i, nb = model->NbEntities();
  for (i = 1; i <= nb; i ++)  TC.Bind (model->Value(i),model->Value(i));

  Interface_EntityIterator pipo;
  Handle(Interface_InterfaceModel)  newmod;
  Handle(IFSelect_AppliedModifiers) applied;
  CopiedModel (G, WL,protocol,pipo,TCollection_AsciiString(filename),
	       0,0,TC,newmod, applied,checks);

  IFSelect_ContextWrite ctx (model,protocol,applied,filename);
  Standard_Boolean res = WL->WriteFile (ctx);
  Interface_CheckIterator checklst = ctx.CheckList();
  checks.Merge(checklst);
  if (!res) checks.CCheck(0)->AddFail ("SendAll (WriteFile) has failed");
//  if (!checks.IsEmpty(Standard_False)) {
//    Message::DefaultMessenger() <<
//      "  **    SendAll has produced Check Messages :    **"<<endl;
//    checks.Print (sout,model,Standard_False);
//  }
  return checks;
}


//  .... SendSelected : Donnees a tranferer dans G, filtrees par iter,
//       aucun split, envoi sur fichier

    Interface_CheckIterator  IFSelect_ModelCopier::SendSelected
  (const Standard_CString filename,   const Interface_Graph& G,
   const Handle(IFSelect_WorkLibrary)& WL,
   const Handle(Interface_Protocol)& protocol,
   const Interface_EntityIterator& list)
{
  Interface_CheckIterator checks;
  checks.SetName ("X-STEP WorkSession : Send Selected");
  Message::DefaultMessenger() <<
    "** WorkSession : Sending selected data"<<endl;
  Handle(Interface_InterfaceModel)  original = G.Model();
  if (original.IsNull() || protocol.IsNull() || WL.IsNull()) return checks;
  Handle(Interface_InterfaceModel) newmod  = original->NewEmptyModel();
  Interface_CopyTool TC (original, protocol);
  TC.FillModel(newmod);    // pour Header ...

//  Pas de copie : AddWithRefs plus declaration de Bind
  Interface_GeneralLib lib(protocol);
  for (list.Start(); list.More(); list.Next()) {
    newmod->AddWithRefs (list.Value(),lib);
  }
  Standard_Integer i, nb = newmod->NbEntities();
  for (i = 1; i <= nb; i ++)  TC.Bind (newmod->Value(i),newmod->Value(i));
  if (theremain.IsNull())
    { theremain = new TColStd_HArray1OfInteger(0,G.Size()); theremain->Init(0); }

  Interface_EntityIterator pipo;
  Handle(IFSelect_AppliedModifiers) applied;
  CopiedModel (G, WL,protocol, pipo,TCollection_AsciiString(filename),
	       0,0,TC,newmod, applied,checks);
//  Alimenter Remaining : les entites copiees sont a noter
  Handle(Standard_Transient) ent1,ent2;
  for (Standard_Integer ic = TC.LastCopiedAfter (0,ent1,ent2); ic > 0;
       ic = TC.LastCopiedAfter (ic,ent1,ent2) ) {
    if (ic <= theremain->Upper())
      theremain->SetValue(ic,theremain->Value(ic)+1);
  }
  IFSelect_ContextWrite ctx (newmod,protocol,applied,filename);
  Standard_Boolean res = WL->WriteFile (ctx);
  Interface_CheckIterator checklst = ctx.CheckList();
  checks.Merge(checklst);
  if (!res) checks.CCheck(0)->AddFail ("SendSelected (WriteFile) has failed");
//  if (!checks.IsEmpty(Standard_False)) {
//    Message::DefaultMessenger() <<
//      "  **    SendSelected has produced Check Messages :    **"<<endl;
//    checks.Print (sout,original,Standard_False);
//  }
  return checks;
}


//  ##########################################################################
//  ########        UN TRANSFERT UNITAIRE (avec Modifications)        ########

    void  IFSelect_ModelCopier::CopiedModel
  (const Interface_Graph& G,
   const Handle(IFSelect_WorkLibrary)& WL,
   const Handle(Interface_Protocol)& protocol,
   const Interface_EntityIterator& tocopy,
   const TCollection_AsciiString& filename,
   const Standard_Integer dispnum, const Standard_Integer /* numod */,
   Interface_CopyTool& TC,
   Handle(Interface_InterfaceModel)& newmod,
   Handle(IFSelect_AppliedModifiers)& applied,
   Interface_CheckIterator& checks) const
{
//  ...  Premiere partie "standard" : remplissage du modele  ...
//  On cree le Modele, on le remplit avec les Entites, et avec le Header depart

//  ATTENTION : dispnum = 0  signifie prendre modele original, ne rien copier
//                             et aussi : pas de Dispatch (envoi en bloc)

  applied.Nullify();
  Handle(Interface_InterfaceModel) original = G.Model();
  if (dispnum > 0) {
    newmod  = original->NewEmptyModel();
    TC.Clear();
    WL->CopyModel (original,newmod,tocopy,TC);

    Handle(Standard_Transient) ent1,ent2;
//  Alimenter Remaining : les entites copiees sont a noter
    for (Standard_Integer ic = TC.LastCopiedAfter (0,ent1,ent2); ic > 0;
	 ic = TC.LastCopiedAfter (ic,ent1,ent2) ) {
      if (ic <= theremain->Upper())
	theremain->SetValue(ic,theremain->Value(ic)+1);
    }
  }
  else if (newmod.IsNull()) newmod = original;

//  ...  Ensuite : On prend en compte les Model Modifiers  ...
  Standard_Integer nbmod = 0;
  if (!theshareout.IsNull()) nbmod = theshareout->NbModifiers(Standard_True);
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nbmod; i ++) {
    Handle(IFSelect_Modifier) unmod = theshareout->ModelModifier(i);

//    D abord,  critere Dispatch/Packet
    if (dispnum > 0)
      if (!unmod->Applies (theshareout->Dispatch(dispnum))) continue;
    IFSelect_ContextModif ctx (G,TC,filename.ToCString());
//    Ensuite, la Selection
    Handle(IFSelect_Selection) sel = unmod->Selection();
    if (!sel.IsNull()) {
      Interface_EntityIterator entiter = sel->UniqueResult(G);
      ctx.Select (entiter);
    }
    if (ctx.IsForNone()) continue;
    unmod->Perform (ctx,newmod,protocol,TC);
    Interface_CheckIterator checklst = ctx.CheckList();
    checks.Merge (checklst);

//    Faut-il enregistrer les erreurs dans newmod ? bonne question
//    if (!checks.IsEmpty(Standard_False)) {
//      Message::DefaultMessenger() <<
//        " Messages on Copied Model n0 "<<numod<<", Dispatch Rank "<<dispnum<<endl;
//      checks.Print(sout,newmod,Standard_False);
//    }
  }

//  ...  Puis les File Modifiers : en fait, on les enregistre  ...
  nbmod = 0;
  if (!theshareout.IsNull()) nbmod = theshareout->NbModifiers(Standard_False);
  if (nbmod == 0) return;
  applied = new IFSelect_AppliedModifiers (nbmod,newmod->NbEntities());
  for (i = 1; i <= nbmod; i ++) {
    Handle(IFSelect_GeneralModifier) unmod = theshareout->GeneralModifier(Standard_False,i);

//    D abord,  critere Dispatch/Packet
    if (dispnum > 0)
      if (!unmod->Applies (theshareout->Dispatch(dispnum))) continue;
//    Ensuite, la Selection
    Handle(IFSelect_Selection) sel = unmod->Selection();
    if (sel.IsNull()) applied->AddModif (unmod);    // vide -> on prend tout
    else {
      Interface_EntityIterator list = sel->UniqueResult(G);
      Handle(Standard_Transient) newent;

//    Entites designees par la Selection et Copiees ?
//    -> s ilyena au moins une, le Modifier s applique, sinon il est rejete
//    -> et cette liste est exploitable par le Modifier ...
      for (list.Start(); list.More(); list.Next()) {
	if (TC.Search (list.Value(),newent))
	  applied->AddNum (newmod->Number(newent));
      }
    }
  }
}


    void  IFSelect_ModelCopier::CopiedRemaining
  (const Interface_Graph& G, const Handle(IFSelect_WorkLibrary)& WL,
   Interface_CopyTool& TC,   Handle(Interface_InterfaceModel)& newmod)
{
  Handle(Interface_InterfaceModel) original = G.Model();
//  Interface_CopyTool TC(original,protocol);
  newmod  = original->NewEmptyModel();
  TC.Clear();
  Interface_EntityIterator tocopy;
  Standard_Integer nb = G.Size();
  theremain = new TColStd_HArray1OfInteger(0,nb+1); theremain->Init(0);
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (G.Status(i) == 0) tocopy.AddItem (original->Value(i));
    else theremain->SetValue(i,-1);  //  ?? -1
  }
  WL->CopyModel (original,newmod,tocopy,TC);

  if (newmod->NbEntities() == 0) newmod.Nullify();
  else {
//  CE QUI SUIT NE DOIT PAS ETRE SUPPRIME ! cf theremain
    Handle(Standard_Transient) ent1,ent2;
    for (Standard_Integer ic = TC.LastCopiedAfter (0,ent1,ent2); ic > 0;
	 ic = TC.LastCopiedAfter (ic,ent1,ent2) ) {
      if (ic <= theremain->Upper())
	theremain->SetValue(ic,1);
    }
//  qq impressions de mise au point
#ifdef MISOPOINT
    cout << " Remaining Model : " << newmod->NbEntities() << " Entities"<<endl;
    Standard_Integer ne = 0;
    for (i = 1; i <= nb; i ++) {
      if (theremain->Value(i) == 0) {
	if (ne == 0)     cout << " Refractaires : ";
	ne ++;  cout << " " << i;
      }
    }
    if (ne > 0) cout << "  -- " << ne << " Entities" << endl;
    else cout<<"  -- Remaining data complete"<<endl;
#endif
  }
}

    Standard_Boolean  IFSelect_ModelCopier::SetRemaining
  (Interface_Graph& CG) const
{
  Standard_Integer nb = CG.Size();
  if (theremain.IsNull()) return (nb == 0);
  if (nb != theremain->Upper()) return Standard_False;
  for (Standard_Integer i = 1; i <= nb; i ++) {
    if (CG.Status(i) >= 0) CG.SetStatus(i,CG.Status(i)+theremain->Value(i));
  }
  theremain->Init(0);
  return Standard_True;
}

//  ##########################################################################
//  ########        RESULTAT de la Memorisation des Transferts        ########

    Standard_Integer  IFSelect_ModelCopier::NbFiles () const 
      {  return thefilemodels.Length();  }

    TCollection_AsciiString  IFSelect_ModelCopier::FileName
  (const Standard_Integer num) const 
      {  return thefilenames.Value(num);  }

    Handle(Interface_InterfaceModel)  IFSelect_ModelCopier::FileModel
  (const Standard_Integer num) const
      {  return thefilemodels.Value(num);  }

    Handle(IFSelect_AppliedModifiers)  IFSelect_ModelCopier::AppliedModifiers
  (const Standard_Integer num) const
      {  return theapplieds.Value(num);  }


    void  IFSelect_ModelCopier::BeginSentFiles
  (const Handle(IFSelect_ShareOut)& sho, const Standard_Boolean record)
{
  thesentfiles.Nullify();
  if (record) thesentfiles = new TColStd_HSequenceOfHAsciiString();
//  et numerotation des fichiers par defaut : detenue par ShareOut
  if (sho.IsNull()) return;
  Standard_Integer lastrun = sho->LastRun();
  sho->ClearResult (Standard_True);
  sho->SetLastRun (lastrun);        // on ne s interesse quaux numeros
}

    void  IFSelect_ModelCopier::AddSentFile (const Standard_CString filename)
      {  if (!thesentfiles.IsNull())
	   thesentfiles->Append(new TCollection_HAsciiString(filename));  }

    Handle(TColStd_HSequenceOfHAsciiString)  IFSelect_ModelCopier::SentFiles () const
      {  return thesentfiles;  }
