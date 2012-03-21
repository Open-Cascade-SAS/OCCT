// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <StepData_StepReaderTool.ixx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StepData_StepModel.hxx>
#include <Standard_Transient.hxx>
#include <StepData_ReadWriteModule.hxx>
#include <StepData_UndefinedEntity.hxx>

#include <Message_Messenger.hxx>
#include <Message.hxx>
#include <Interface_Macros.hxx>
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
    catch(Standard_Failure) {
      Handle(Message_Messenger) sout = Message::DefaultMessenger();
      sout << " Exception Raised during Preparation :\n";
      sout << Standard_Failure::Caught()->GetMessageString();
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
  Handle(Message_Messenger) sout = Message::DefaultMessenger();
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
      if (!ent.IsNull()) sout << ent->DynamicType()->Name() << endl;
      for (Standard_Integer nf = 1; nf <= nbmess; nf ++)
	sout << ach->CWarning(nf) << "\n";
    }
    if (ach->HasFailed()) {
      Handle(Interface_Check) mch = model->GlobalCheck();
      Standard_Integer nbmess = ach->NbFails();
      sout << " Errors on Reading Header Entity N0."<<i<<":";
      if (!ent.IsNull()) sout << ent->DynamicType()->Name() << endl;
      for (Standard_Integer nf = 1; nf <= nbmess; nf ++)
	sout << ach->CFail(nf) << "\n";
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
  Handle(StepData_ReadWriteModule) module;  Standard_Integer CN;
  if (therlib.Select(anent,module,CN))
    module->ReadStep(CN,stepdat,num,acheck,anent);
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
