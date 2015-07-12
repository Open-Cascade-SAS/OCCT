// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWDocumentRepresentationType.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentRepresentationType.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWDocumentRepresentationType
//purpose  : 
//=======================================================================
RWStepBasic_RWDocumentRepresentationType::RWStepBasic_RWDocumentRepresentationType ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentRepresentationType::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                         const Standard_Integer num,
                                                         Handle(Interface_Check)& ach,
                                                         const Handle(StepBasic_DocumentRepresentationType) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"document_representation_type") ) return;

  // Own fields of DocumentRepresentationType

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(StepBasic_Document) aRepresentedDocument;
  data->ReadEntity (num, 2, "represented_document", ach, STANDARD_TYPE(StepBasic_Document), aRepresentedDocument);

  // Initialize entity
  ent->Init(aName,
            aRepresentedDocument);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentRepresentationType::WriteStep (StepData_StepWriter& SW,
                                                          const Handle(StepBasic_DocumentRepresentationType) &ent) const
{

  // Own fields of DocumentRepresentationType

  SW.Send (ent->Name());

  SW.Send (ent->RepresentedDocument());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentRepresentationType::Share (const Handle(StepBasic_DocumentRepresentationType) &ent,
                                                      Interface_EntityIterator& iter) const
{

  // Own fields of DocumentRepresentationType

  iter.AddItem (ent->RepresentedDocument());
}
