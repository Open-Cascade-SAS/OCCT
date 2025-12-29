// Created on: 2000-05-11
// Created by: data exchange team
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

#include <Interface_EntityIterator.hxx>
#include "RWStepBasic_RWDocument.pxx"
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentType.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=================================================================================================

RWStepBasic_RWDocument::RWStepBasic_RWDocument() = default;

//=================================================================================================

void RWStepBasic_RWDocument::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                      const int                                   num,
                                      occ::handle<Interface_Check>&               ach,
                                      const occ::handle<StepBasic_Document>&      ent) const
{
  // Check number of parameters
  if (!data->CheckNbParams(num, 4, ach, "document"))
    return;

  // Own fields of Document

  occ::handle<TCollection_HAsciiString> aId;
  data->ReadString(num, 1, "id", ach, aId);

  occ::handle<TCollection_HAsciiString> aName;
  data->ReadString(num, 2, "name", ach, aName);

  occ::handle<TCollection_HAsciiString> aDescription;
  bool                                  hasDescription = true;
  if (data->IsParamDefined(num, 3))
  {
    data->ReadString(num, 3, "description", ach, aDescription);
  }
  else
  {
    hasDescription = false;
  }

  occ::handle<StepBasic_DocumentType> aKind;
  data->ReadEntity(num, 4, "kind", ach, STANDARD_TYPE(StepBasic_DocumentType), aKind);

  // Initialize entity
  ent->Init(aId, aName, hasDescription, aDescription, aKind);
}

//=================================================================================================

void RWStepBasic_RWDocument::WriteStep(StepData_StepWriter&                   SW,
                                       const occ::handle<StepBasic_Document>& ent) const
{

  // Own fields of Document

  SW.Send(ent->Id());

  SW.Send(ent->Name());

  if (ent->HasDescription())
  {
    SW.Send(ent->Description());
  }
  else
    SW.SendUndef();

  SW.Send(ent->Kind());
}

//=================================================================================================

void RWStepBasic_RWDocument::Share(const occ::handle<StepBasic_Document>& ent,
                                   Interface_EntityIterator&              iter) const
{

  // Own fields of Document

  iter.AddItem(ent->Kind());
}
