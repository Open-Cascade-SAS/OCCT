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

// dce 21/01/99 S3767 : Suppression of general messages

#include <Interface_Check.hxx>
#include <Interface_CheckFailure.hxx>
#include <Interface_FileReaderData.hxx>
#include <Interface_FileReaderTool.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_InterfaceMismatch.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ReaderLib.hxx>
#include <Interface_ReaderModule.hxx>
#include <Interface_ReportEntity.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_Msg.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Transient.hxx>
#include <Standard_TypeMismatch.hxx>

#ifdef _WIN32
#include <OSD_Exception.hxx>
#else
#include <OSD_Signal.hxx>
#endif
#include <stdio.h>

// MGE 16/06/98
// To use Msg class
#include <Message_Msg.hxx>
// To use TCollectionHAsciiString
#include <TCollection_HAsciiString.hxx>

// Failure pour recuperer erreur en lecture fichier,
// TypeMismatch pour message d erreur circonstancie (cas particulier important)


//  Gere le chargement d un Fichier, prealablement transforme en FileReaderData
//  (de la bonne norme), dans un Modele


//=======================================================================
//function : Interface_FileReaderTool
//purpose  : 
//=======================================================================

Interface_FileReaderTool::Interface_FileReaderTool ()
{
  themessenger = Message::DefaultMessenger();
  theerrhand = Standard_True;
  thetrace = 1;
  thenbrep0 = thenbreps = 0;
}

//=======================================================================
//function : SetData
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::SetData(const Handle(Interface_FileReaderData)& reader,
                                       const Handle(Interface_Protocol)& protocol)
{
  thereader = reader;
  theproto = protocol;
}


//=======================================================================
//function : Protocol
//purpose  : 
//=======================================================================

Handle(Interface_Protocol) Interface_FileReaderTool::Protocol () const
{
  return theproto;
}


//=======================================================================
//function : Data
//purpose  : 
//=======================================================================

Handle(Interface_FileReaderData) Interface_FileReaderTool::Data () const
{
  return thereader;
}


//=======================================================================
//function : SetModel
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::SetModel
  (const Handle(Interface_InterfaceModel)& amodel)
{
  themodel = amodel;
}


//=======================================================================
//function : Model
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) Interface_FileReaderTool::Model () const
{
  return themodel;
}

//=======================================================================
//function : SetMessenger
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::SetMessenger (const Handle(Message_Messenger)& messenger)
{
  if ( messenger.IsNull() )
    themessenger = Message::DefaultMessenger();
  else   
    themessenger = messenger;
}

//=======================================================================
//function : Messenger
//purpose  : 
//=======================================================================

Handle(Message_Messenger) Interface_FileReaderTool::Messenger () const
{
  return themessenger;
}

//=======================================================================
//function : SetTraceLevel
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::SetTraceLevel (const Standard_Integer tracelev)
{
  thetrace = tracelev;
}

//=======================================================================
//function : TraceLevel
//purpose  : 
//=======================================================================

Standard_Integer Interface_FileReaderTool::TraceLevel () const
{
  return thetrace;
}

//=======================================================================
//function : SetErrorHandle
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::SetErrorHandle(const Standard_Boolean err)
{
  theerrhand = err;
}


//=======================================================================
//function : ErrorHandle
//purpose  : 
//=======================================================================

Standard_Boolean  Interface_FileReaderTool::ErrorHandle() const
{
  return theerrhand;
}

//  ....            Actions Connexes au CHARGEMENT DU MODELE            ....

// SetEntities fait appel a des methodes a fournir :
// s appuyant sur un Recognizer adapte a l interface :
// - Recognize fait reco->Evaluate(... : selon record no num)
//   et recupere le resultat
// ainsi que la definition de l entite inconnue de l interface


//=======================================================================
//function : SetEntities
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::SetEntities ()
{
  Standard_Integer num;
  thenbreps = 0;  thenbrep0 = 0;

  for (num = thereader->FindNextRecord(0);  num > 0;
       num = thereader->FindNextRecord(num)) {
    Handle(Standard_Transient) newent;
    Handle(Interface_Check) ach = new Interface_Check;
    if (!Recognize (num,ach,newent)) {
      newent = UnknownEntity();
      if (thereports.IsNull()) thereports =
	new TColStd_HArray1OfTransient (1,thereader->NbRecords());
      thenbreps ++;  thenbrep0 ++;
      thereports->SetValue (num,new Interface_ReportEntity(ach,newent));
    }
    else if ((ach->NbFails() + ach->NbWarnings() > 0) && !newent.IsNull()) {
      if (thereports.IsNull()) thereports =
	new TColStd_HArray1OfTransient (1,thereader->NbRecords());
      thenbreps ++;  thenbrep0 ++;
      thereports->SetValue (num,new Interface_ReportEntity(ach,newent));
    }
    thereader->BindEntity (num,newent);
  }
}


//=======================================================================
//function : RecognizeByLib
//purpose  : 
//=======================================================================

Standard_Boolean Interface_FileReaderTool::RecognizeByLib(const Standard_Integer num,
                                                          Interface_GeneralLib& glib,
                                                          Interface_ReaderLib& rlib,
                                                          Handle(Interface_Check)& ach,
                                                          Handle(Standard_Transient)& ent) const
{
  Handle(Interface_GeneralModule) gmod;
  Handle(Interface_ReaderModule)  rmod;
  Handle(Interface_Protocol) proto;
  Standard_Integer CN = 0;
//   Chercher dans ReaderLib : Reconnaissance de cas -> CN , proto
  for (rlib.Start(); rlib.More(); rlib.Next()) {
    rmod = rlib.Module();
    if (rmod.IsNull()) continue;
    CN = rmod->CaseNum(thereader,num);
    if (CN > 0)  {  proto = rlib.Protocol();  break;  }
  }
  if (CN <= 0 || proto.IsNull()) return Standard_False;
//   Se recaler dans GeneralLib : Creation de l entite vide
  Handle(Standard_Type) typrot = proto->DynamicType();
  for (glib.Start(); glib.More(); glib.Next()) {
    proto = glib.Protocol();
    if (proto.IsNull()) continue;
    if (proto->DynamicType() != typrot) continue;
    Standard_Boolean res = glib.Module()->NewVoid(CN,ent);
    if (res) return res;
    if (!rmod.IsNull()) return rmod->NewRead (CN,thereader,num,ach,ent);
//    return res;
  }
  return Standard_False;
}


//=======================================================================
//function : UnknownEntity
//purpose  : 
//=======================================================================

Handle(Standard_Transient) Interface_FileReaderTool::UnknownEntity() const
{
  return theproto->UnknownEntity();
}


//=======================================================================
//function : NewModel
//purpose  : 
//=======================================================================

Handle(Interface_InterfaceModel) Interface_FileReaderTool::NewModel() const
{
  return theproto->NewModel();
}


//=======================================================================
//function : EndRead
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::EndRead(const Handle(Interface_InterfaceModel)& )
{
}    // par defaut, ne fait rien; redefinissable selon besoin


//  ....               (Sa Majeste le) CHARGEMENT DU MODELE               ....


//=======================================================================
//function : LoadModel
//purpose  : 
//=======================================================================

void Interface_FileReaderTool::LoadModel
  (const Handle(Interface_InterfaceModel)& amodel)
//
//   Methode generale de lecture d un fichier : il est lu via un FileReaderData
//   qui doit y donner acces de la facon la plus performante possible
//   chaque interface definit son FileHeader avec ses methodes, appelees ici
{
  // MGE 16/06/98
  // Building of Messages
  //====================================
  Handle(Message_Messenger) TF = Messenger();
  //====================================
  Handle(Interface_Check) ach = new Interface_Check;

  SetModel(amodel);

//  ..            Demarrage : Lecture du Header            ..
  if (theerrhand) {
    try {
      OCC_CATCH_SIGNALS
      BeginRead(amodel);  // selon la norme
    }
    catch (Standard_Failure) {
      // Sendinf of message : Internal error during the header reading
      Message_Msg Msg11("XSTEP_11");
      TF->Send (Msg11, Message_Info); 
    }
  }
  else
    BeginRead(amodel);  // selon la norme

  //  ..            Lecture des Entites            ..

  amodel->Reservate (thereader->NbEntities());

  Standard_Integer num, num0 = thereader->FindNextRecord(0);
  num = num0;

  while (num > 0) {
    Standard_Integer ierr = 0;  // erreur sur analyse d une entite
    Handle(Standard_Transient) anent;
    try {
      OCC_CATCH_SIGNALS
      for (num = num0;  num > 0; num = thereader->FindNextRecord(num)) {
	num0 = num;

	//    Lecture sous protection contre crash
	//    (fait aussi AddEntity mais pas SetReportEntity)
	anent = LoadedEntity(num);

	//     Lecture non protegee : utile pour travailler avec dbx
////    else
////      anent = LoadedEntity(num);

	//   ..        Fin Lecture        ..
	if (anent.IsNull())  {
          // Sending of message : Number of ignored Null Entities  
	  Message_Msg Msg21("XSTEP_21");
          Msg21.Arg(amodel->NbEntities());
          TF->Send (Msg21, Message_Info);
	  continue;
	}
	//      LoadedEntity fait AddEntity MAIS PAS SetReport (en bloc a la fin)

      }    // ---- fin boucle sur entites
      num0 = 0;    // plus rien
    }      // ---- fin du try, le catch suit

    //   En cas d erreur NON PREVUE par l analyse, recuperation par defaut
    //   Attention : la recuperation peut elle-meme planter ... (cf ierr)
    catch (Standard_Failure const& anException) {
      //      Au passage suivant, on attaquera le record suivant
      num0 = thereader->FindNextRecord(num); //:g9 abv 28 May 98: tr8_as2_ug.stp - infinite cycle: (0);

#ifdef _WIN32
      if (anException.IsKind(STANDARD_TYPE(OSD_Exception))) ierr = 2;
#else
      if (anException.IsKind(STANDARD_TYPE(OSD_Signal))) ierr = 2;
#endif
//:abv 03Apr00: anent is actually a previous one:      if (anent.IsNull()) 
      anent = thereader->BoundEntity(num);
      if (anent.IsNull()) {
	if (thetrace > 0) 
	{
	  // Sending of message : Number of ignored Null Entities  
	  Message_Msg Msg21("XSTEP_21");
	  Msg21.Arg(amodel->NbEntities()+1);
	  TF->Send (Msg21, Message_Info);
	  continue;
	}
      }
      /*Handle(Interface_Check)*/ ach = new Interface_Check(anent);
      //: abv 03 Apr 00: trj3_s1-tc-214.stp: generate a message on exception
      Message_Msg Msg278("XSTEP_278");
      Msg278.Arg(amodel->StringLabel(anent));
      ach->SendFail (Msg278); 
      
      if (ierr == 2) {
       // Sending of message : reading of entity failed  
	Message_Msg Msg22("XSTEP_22");
        Msg22.Arg(amodel->StringLabel(anent));
        TF->Send (Msg22, Message_Info); 
	return;
      }

      if (!ierr) {
	//char mess[100]; svv #2
	ierr = 1;
// ce qui serait bien ici serait de recuperer le texte de l erreur pour ach ...
	if (thetrace > 0) {
	  // Sending of message : recovered entity  
	  Message_Msg Msg23("XSTEP_23");
	  Msg23.Arg(num);
	  TF->Send (Msg23, Message_Info); 
	}

//  Finalement, on charge une Entite Inconnue
	thenbreps ++;
	Handle(Interface_ReportEntity) rep =
	  new Interface_ReportEntity(ach,anent);
	Handle(Standard_Transient) undef = UnknownEntity();
	AnalyseRecord(num,undef,ach);
	rep->SetContent(undef);

	if (thereports.IsNull()) thereports =
	  new TColStd_HArray1OfTransient (1,thereader->NbRecords());
	thenbreps ++;
	thereports->SetValue (num,rep);
        //if(isValid)
          amodel->AddEntity (anent);    // pas fait par LoadedEntity ...
      }
      else {
	if (thetrace > 0) {
	  // Sending of message : reading of entity failed  
	  Message_Msg Msg22("XSTEP_22");
	  Msg22.Arg(amodel->StringLabel(anent));
	  TF->Send (Msg22, Message_Info);
	}
//  On garde <rep> telle quelle : pas d analyse fichier supplementaire,
//  Mais la phase preliminaire eventuelle est conservee
//  (en particulier, on garde trace du Type lu du fichier, etc...)
      }
    }    // -----  fin complete du try/catch
  }      // -----  fin du while

//  ..        Ajout des Reports, silya
  if (!thereports.IsNull()) {
    if (thetrace > 0) 
    {
      // Sending of message : report   
      Message_Msg Msg24("XSTEP_24");
      Msg24.Arg(thenbreps);
      TF->Send (Msg24, Message_Info); 
    }
    amodel->Reservate (-thenbreps-10);
    thenbreps = thereports->Upper();
    for (Standard_Integer nr = 1; nr <= thenbreps; nr ++) {
      if (thereports->Value(nr).IsNull()) continue;
      Handle(Standard_Transient) anent = thereader->BoundEntity (nr);
      Handle(Interface_ReportEntity) rep =
	Handle(Interface_ReportEntity)::DownCast(thereports->Value(nr));
      amodel->SetReportEntity (-amodel->Number(anent),rep);
    }
  }

//   Conclusion : peut ne rien faire : selon necessite
  if (theerrhand) {
    try {
      OCC_CATCH_SIGNALS
      EndRead(amodel);  // selon la norme
    }
    catch (Standard_Failure) {
      // Sendinf of message : Internal error during the header reading
      Message_Msg Msg11("XSTEP_11");
      TF->Send (Msg11, Message_Info); 
    }
  }
  else
    EndRead(amodel);  // selon la norme
}


//=======================================================================
//function : LoadedEntity
//purpose  : 
//=======================================================================

Handle(Standard_Transient) Interface_FileReaderTool::LoadedEntity
       (const Standard_Integer num)
{
  Handle(Standard_Transient) anent = thereader->BoundEntity(num);
  Handle(Interface_Check) ach = new Interface_Check(anent);
  Handle(Interface_ReportEntity) rep;    // entite Report, s il y a lieu
  Standard_Integer irep = 0;
  //Standard_Integer nbe  = 0; svv #2
  if (thenbrep0 > 0) {
    rep = Handle(Interface_ReportEntity)::DownCast(thereports->Value(num));
    if (!rep.IsNull()) { irep = num;  ach = rep->Check(); }
  }

//    Trace Entite Inconnue
  if (thetrace >= 2 && theproto->IsUnknownEntity(anent)) {
    Handle(Message_Messenger) TF = Messenger();
    Message_Msg Msg22("XSTEP_22");
    // Sending of message : reading of entity failed
    Msg22.Arg(themodel->StringLabel(anent)->String());
    TF->Send (Msg22, Message_Info); 
  }
//  ..        Chargement proprement dit : Specifique de la Norme        ..
  AnalyseRecord(num,anent,ach);

//  ..        Ajout dans le modele de l entite telle quelle        ..
//            ATTENTION, ReportEntity traitee en bloc apres les Load
    themodel->AddEntity(anent);

//   Erreur ou Correction : On cree une ReportEntity qui memorise le Check,
//   l Entite, et en cas d Erreur une UndefinedEntity pour les Parametres

//   On exploite ici le flag IsLoadError : s il a ete defini (a vrai ou faux)
//   il a priorite sur les fails du check. Sinon, ce sont les fails qui parlent

  Standard_Integer nbf = ach->NbFails();
  Standard_Integer nbw = ach->NbWarnings();
  if (nbf + nbw > 0) {
    //Standard_Integer n0; svv #2
    themodel->NbEntities();
    rep = new Interface_ReportEntity(ach,anent);
    if (irep == 0) {
      if (thereports.IsNull()) thereports =
	new TColStd_HArray1OfTransient (1,thereader->NbRecords());
      irep = num;
      thenbreps ++;
    }
    thereports->SetValue(irep,rep);

    if ( thetrace >= 2)
      ach->Print (Messenger(),2);
  }
  
//    Rechargement ? si oui, dans une UnknownEntity fournie par le protocole
  if (thereader->IsErrorLoad())  nbf = (thereader->ResetErrorLoad() ? 1 : 0);
  if (nbf > 0)  {
    Handle(Standard_Transient) undef = UnknownEntity();
    AnalyseRecord(num,undef,ach);
    rep->SetContent(undef);
  }

//    Conclusion  (Unknown : traite en externe because traitement Raise)
////  if (irep > 0) themodel->SetReportEntity (nbe,rep);  en bloc a la fin

  return anent;
}


//=======================================================================
//function : ~Interface_FileReaderTool
//purpose  : 
//=======================================================================

Interface_FileReaderTool::~Interface_FileReaderTool()
{}
     
void Interface_FileReaderTool::Clear()
{
  theproto.Nullify();
  thereader.Nullify();
  themodel.Nullify();
  thereports.Nullify();
}
