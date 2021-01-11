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


#include <Interface_Check.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Macros.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <StepData_FileRecognizer.hxx>
#include <StepData_Protocol.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepReaderTool.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : StepData_StepReaderTool
//purpose  : 
//=======================================================================
StepData_StepReaderTool::StepData_StepReaderTool
  (const Handle(StepData_StepReaderData)& reader,
   const Handle(StepData_Protocol)& protocol)
:  theglib(protocol) , therlib(protocol)
{
  SetData(reader,protocol);
}


//=======================================================================
//function : Recognize
//purpose  : 
//=======================================================================

Standard_Boolean StepData_StepReaderTool::Recognize(const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    Handle(Standard_Transient)& ent)
{
//  Handle(Standard_Transient) bid;  // pas exploite
//  return thereco->Evaluate(thetypes.Value(num),bid);

//  Recognizer : C est lui qui assure la Reconnaissance (-> Liste limitative)
  if (!thereco.IsNull()) {
    DeclareAndCast(StepData_StepReaderData,stepdat,Data());
    return thereco->Evaluate(stepdat->RecordType(num),ent);
  }

//  Pas de Recognizer : Reconnaissance par la librairie
  return RecognizeByLib (num,theglib,therlib,ach,ent);
}


//  ....         Methodes de preparations propres a StepReaderTool         ....


//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================

void StepData_StepReaderTool::Prepare
  (const Handle(StepData_FileRecognizer)& reco, const Standard_Boolean optim)
{
  thereco = reco;
  Prepare(optim);
}


//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================

void StepData_StepReaderTool::Prepare (const Standard_Boolean optim)
{
//   SetEntityNumbers a ete mis du cote de ReaderData, because beaucoup acces
  Standard_Boolean erh = ErrorHandle();
  DeclareAndCast(StepData_StepReaderData,stepdat,Data());
  if (erh) {
    try {
      OCC_CATCH_SIGNALS
      stepdat->SetEntityNumbers(optim);
      SetEntities();
    }
    catch(Standard_Failure const& anException) {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << " Exception Raised during Preparation :\n";
      sout << anException.GetMessageString();
      sout << "\n Now, trying to continue, but with presomption of failure\n";
    }
  }
  else {
    stepdat->SetEntityNumbers(optim);
    SetEntities();
  }
}


// ....            Gestion du Header : Preparation, lecture            .... //


//=======================================================================
//function : PrepareHeader
//purpose  : 
//=======================================================================

void StepData_StepReaderTool::PrepareHeader
  (const Handle(StepData_FileRecognizer)& reco)
{
  Standard_Integer i = 0;

// Reconnaissance des types
  DeclareAndCast(StepData_StepReaderData,stepdat,Data());
  while ( (i = stepdat->FindNextHeaderRecord(i)) != 0) {
    Handle(Standard_Transient) ent;
//  On a donne un Recognizer : il fixe une liste limitative de types reconnus
    if (!reco.IsNull()) {
      if (!reco->Evaluate(stepdat->RecordType(i),ent)) {
	ent = Protocol()->UnknownEntity();
      }
    } else {
//  Pas de Recognizer : Reconnaissance par la librairie
      Handle(Interface_Check) ach = new Interface_Check;    // faudrait le lister ... ?
      RecognizeByLib (i,theglib,therlib,ach,ent);
    }
    if (ent.IsNull()) ent = Protocol()->UnknownEntity();
    stepdat->BindEntity(i,ent);
  }

//  Reste la Resolution des references : ne concerne que les sous-listes
//  Assuree par ReaderData
  stepdat->PrepareHeader();
}


// ....   Methodes pour la lecture du Modele (apres preparation)   .... //


//=======================================================================
//function : BeginRead
//purpose  : 
//=======================================================================

void StepData_StepReaderTool::BeginRead
  (const Handle(Interface_InterfaceModel)& amodel)
{
  Message_Messenger::StreamBuffer sout = Message::SendTrace();
  DeclareAndCast(StepData_StepModel,model,amodel);
  DeclareAndCast(StepData_StepReaderData,stepdat,Data());

  model->ClearHeader();
  model->SetGlobalCheck(stepdat->GlobalCheck());
  Standard_Integer i = 0;
  while ( (i = stepdat->FindNextHeaderRecord(i)) != 0) {
    Handle(Standard_Transient) ent = stepdat->BoundEntity(i);
    Handle(Interface_Check) ach = new Interface_Check(ent);
    AnalyseRecord(i,ent,ach);
    if (ent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity))) {
      TCollection_AsciiString mess("Header Entity not Recognized, StepType: ");
      mess.AssignCat(stepdat->RecordType(i));
      ach->AddWarning(mess.ToCString());
    }
    if (ach->HasFailed() || ach->HasWarnings()) {
      Handle(Interface_Check) mch = model->GlobalCheck();
      mch->GetMessages(ach); model->SetGlobalCheck(mch);
    }
    model->AddHeaderEntity(ent);
    if (ach->HasWarnings()) {
      Handle(Interface_Check) mch = model->GlobalCheck();
      Standard_Integer nbmess = ach->NbWarnings();
      sout<<nbmess<<" Warnings on Reading Header Entity N0."<<i<<":";
      if (!ent.IsNull()) sout << ent->DynamicType()->Name() << std::endl;
      for (Standard_Integer nf = 1; nf <= nbmess; nf++)
      {
        sout << ach->CWarning(nf) << "\n";
      }
    }
    if (ach->HasFailed()) {
      Handle(Interface_Check) mch = model->GlobalCheck();
      Standard_Integer nbmess = ach->NbFails();
      sout << " Fails on Reading Header Entity N0." << i << ":";
      if (!ent.IsNull()) sout << ent->DynamicType()->Name() << std::endl;
      for (Standard_Integer nf = 1; nf <= nbmess; nf++)
      {
        sout << ach->CFail(nf) << "\n";
      }
    }
  }
}


//=======================================================================
//function : AnalyseRecord
//purpose  : 
//=======================================================================

Standard_Boolean StepData_StepReaderTool::AnalyseRecord
  (const Standard_Integer num,
   const Handle(Standard_Transient)& anent,
   Handle(Interface_Check)& acheck)
{
  DeclareAndCast(StepData_StepReaderData,stepdat,Data());
  Handle(Interface_ReaderModule) imodule;
  Standard_Integer CN;
  if (therlib.Select(anent,imodule,CN))
  {
    Handle(StepData_ReadWriteModule) module =
      Handle(StepData_ReadWriteModule)::DownCast (imodule);
    module->ReadStep(CN,stepdat,num,acheck,anent);
  }
  else {
//  Pas trouve : tenter UndefinedEntity de StepData
    DeclareAndCast(StepData_UndefinedEntity,und,anent);
    if (und.IsNull()) acheck->AddFail
      ("# Entity neither Recognized nor set as UndefinedEntity from StepData #");
    else und->ReadRecord(stepdat,num,acheck);
  }
  return (!acheck->HasFailed());
}


//=======================================================================
//function : EndRead
//purpose  : 
//=======================================================================

void StepData_StepReaderTool::EndRead
  (const Handle(Interface_InterfaceModel)& amodel)
{
  DeclareAndCast(StepData_StepReaderData,stepdat,Data());
  DeclareAndCast(StepData_StepModel,stepmodel,amodel);
  if (stepmodel.IsNull()) return;
  Standard_Integer i = 0;
  while ( (i = stepdat->FindNextRecord(i)) != 0) {
    stepmodel->SetIdentLabel(stepdat->BoundEntity(i),stepdat->RecordIdent(i));
  }
}
