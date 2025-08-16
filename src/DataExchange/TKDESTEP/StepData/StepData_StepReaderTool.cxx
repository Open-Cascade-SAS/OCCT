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

//=================================================================================================

// Constructor: Initialize the STEP reader tool with reader data and protocol
StepData_StepReaderTool::StepData_StepReaderTool(const Handle(StepData_StepReaderData)& reader,
                                                 const Handle(StepData_Protocol)&       protocol)
    : theglib(protocol), // General library from protocol
      therlib(protocol)  // Reader library from protocol
{
  SetData(reader, protocol);
}

//=================================================================================================

// Recognize and create an entity from a STEP record number
// Returns true if recognition was successful
Standard_Boolean StepData_StepReaderTool::Recognize(const Standard_Integer      num,
                                                    Handle(Interface_Check)&    ach,
                                                    Handle(Standard_Transient)& ent)
{
  //  Handle(Standard_Transient) bid;  // not used
  //  return thereco->Evaluate(thetypes.Value(num),bid);

  // Recognizer: It ensures the Recognition (-> Restrictive list of types)
  // If a specific recognizer is available, use it for type recognition
  if (!thereco.IsNull())
  {
    DeclareAndCast(StepData_StepReaderData, stepdat, Data());
    return thereco->Evaluate(stepdat->RecordType(num), ent);
  }

  // No Recognizer: Recognition by the library
  // Fall back to standard library recognition when no specific recognizer is set
  return RecognizeByLib(num, theglib, therlib, ach, ent);
}

// ....         Preparation methods specific to StepReaderTool         ....

//=================================================================================================

// Prepare the reader tool with a file recognizer and optimization flag
void StepData_StepReaderTool::Prepare(const Handle(StepData_FileRecognizer)& reco,
                                      const Standard_Boolean                 optim)
{
  thereco = reco; // Store the recognizer for later use
  Prepare(optim); // Continue with standard preparation
}

//=================================================================================================

void StepData_StepReaderTool::Prepare(const Standard_Boolean optim)
{
  // SetEntityNumbers has been moved to ReaderData side, because of many accesses
  // Check if error handling is enabled to decide on exception handling strategy
  Standard_Boolean erh = ErrorHandle();
  DeclareAndCast(StepData_StepReaderData, stepdat, Data());
  if (erh)
  {
    try
    {
      OCC_CATCH_SIGNALS
      stepdat->SetEntityNumbers(optim);
      SetEntities();
    }
    catch (Standard_Failure const& anException)
    {
      Message_Messenger::StreamBuffer sout = Message::SendInfo();
      sout << " Exception Raised during Preparation :\n";
      sout << anException.GetMessageString();
      sout << "\n Now, trying to continue, but with presomption of failure\n";
    }
  }
  else
  {
    stepdat->SetEntityNumbers(optim);
    SetEntities();
  }
}

// ....            Header Management: Preparation, reading            .... //

//=================================================================================================

// Prepare header entities by recognizing their types and binding them
void StepData_StepReaderTool::PrepareHeader(const Handle(StepData_FileRecognizer)& reco)
{
  Standard_Integer i = 0; // Index for iterating through header records

  // Type recognition
  DeclareAndCast(StepData_StepReaderData, stepdat, Data());
  while ((i = stepdat->FindNextHeaderRecord(i)) != 0)
  {
    Handle(Standard_Transient) ent;
    // A Recognizer was provided: it sets a restrictive list of recognized types
    if (!reco.IsNull())
    {
      // Try to recognize the entity type using the provided recognizer
      if (!reco->Evaluate(stepdat->RecordType(i), ent))
      {
        // If recognition fails, create an unknown entity
        ent = Protocol()->UnknownEntity();
      }
    }
    else
    {
      // No Recognizer: Recognition by the library
      Handle(Interface_Check) ach = new Interface_Check; // should this be listed...?
      RecognizeByLib(i, theglib, therlib, ach, ent);
    }
    if (ent.IsNull())
      ent = Protocol()->UnknownEntity();
    stepdat->BindEntity(i, ent);
  }

  // Remaining reference resolution: only concerns sub-lists
  // Handled by ReaderData
  stepdat->PrepareHeader();
}

// ....   Methods for reading the Model (after preparation)   .... //

//=================================================================================================

// Begin reading process: clear model header and process header entities
void StepData_StepReaderTool::BeginRead(const Handle(Interface_InterfaceModel)& amodel)
{
  Message_Messenger::StreamBuffer sout = Message::SendTrace();
  DeclareAndCast(StepData_StepModel, model, amodel);
  DeclareAndCast(StepData_StepReaderData, stepdat, Data());

  model->ClearHeader();
  model->SetGlobalCheck(stepdat->GlobalCheck());
  Standard_Integer i = 0;
  while ((i = stepdat->FindNextHeaderRecord(i)) != 0)
  {
    Handle(Standard_Transient) ent = stepdat->BoundEntity(i);
    Handle(Interface_Check)    ach = new Interface_Check(ent);
    AnalyseRecord(i, ent, ach);
    if (ent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity)))
    {
      TCollection_AsciiString mess("Header Entity not Recognized, StepType: ");
      mess.AssignCat(stepdat->RecordType(i));
      ach->AddWarning(mess.ToCString());
    }
    if (ach->HasFailed() || ach->HasWarnings())
    {
      Handle(Interface_Check) mch = model->GlobalCheck();
      mch->GetMessages(ach);
      model->SetGlobalCheck(mch);
    }
    model->AddHeaderEntity(ent);
    if (ach->HasWarnings())
    {
      Handle(Interface_Check) mch    = model->GlobalCheck();
      Standard_Integer        nbmess = ach->NbWarnings();
      sout << nbmess << " Warnings on Reading Header Entity N0." << i << ":";
      if (!ent.IsNull())
        sout << ent->DynamicType()->Name() << std::endl;
      for (Standard_Integer nf = 1; nf <= nbmess; nf++)
      {
        sout << ach->CWarning(nf) << "\n";
      }
    }
    if (ach->HasFailed())
    {
      Handle(Interface_Check) mch    = model->GlobalCheck();
      Standard_Integer        nbmess = ach->NbFails();
      sout << " Fails on Reading Header Entity N0." << i << ":";
      if (!ent.IsNull())
        sout << ent->DynamicType()->Name() << std::endl;
      for (Standard_Integer nf = 1; nf <= nbmess; nf++)
      {
        sout << ach->CFail(nf) << "\n";
      }
    }
  }
}

//=================================================================================================

// Analyze a STEP record and populate the corresponding entity
// Returns true if analysis was successful (no failures)
Standard_Boolean StepData_StepReaderTool::AnalyseRecord(const Standard_Integer            num,
                                                        const Handle(Standard_Transient)& anent,
                                                        Handle(Interface_Check)&          acheck)
{
  DeclareAndCast(StepData_StepReaderData, stepdat, Data());
  Handle(Interface_ReaderModule) imodule;
  Standard_Integer               CN;
  // Try to find appropriate reader module for this entity type
  if (therlib.Select(anent, imodule, CN))
  {
    // Cast to STEP-specific module and read the entity data
    Handle(StepData_ReadWriteModule) module = Handle(StepData_ReadWriteModule)::DownCast(imodule);
    module->ReadStep(CN, stepdat, num, acheck, anent);
  }
  else
  {
    // Not found: try UndefinedEntity from StepData
    DeclareAndCast(StepData_UndefinedEntity, und, anent);
    if (und.IsNull())
      acheck->AddFail("# Entity neither Recognized nor set as UndefinedEntity from StepData #");
    else
      und->ReadRecord(stepdat, num, acheck);
  }
  return (!acheck->HasFailed());
}

//=================================================================================================

// Finish reading process: set entity labels from record identifiers
void StepData_StepReaderTool::EndRead(const Handle(Interface_InterfaceModel)& amodel)
{
  DeclareAndCast(StepData_StepReaderData, stepdat, Data());
  DeclareAndCast(StepData_StepModel, stepmodel, amodel);
  if (stepmodel.IsNull())
    return;
  Standard_Integer i = 0;
  while ((i = stepdat->FindNextRecord(i)) != 0)
  {
    stepmodel->SetIdentLabel(stepdat->BoundEntity(i), stepdat->RecordIdent(i));
  }
}
