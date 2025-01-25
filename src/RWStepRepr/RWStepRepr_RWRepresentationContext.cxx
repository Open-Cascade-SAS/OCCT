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

#include "RWStepRepr_RWRepresentationContext.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationContext.hxx>

RWStepRepr_RWRepresentationContext::RWStepRepr_RWRepresentationContext() {}

void RWStepRepr_RWRepresentationContext::ReadStep(
  const Handle(StepData_StepReaderData)&        data,
  const Standard_Integer                        num,
  Handle(Interface_Check)&                      ach,
  const Handle(StepRepr_RepresentationContext)& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "representation_context"))
    return;

  // --- own field : contextIdentifier ---

  Handle(TCollection_HAsciiString) aContextIdentifier;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadString(num, 1, "context_identifier", ach, aContextIdentifier);

  // --- own field : contextType ---

  Handle(TCollection_HAsciiString) aContextType;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadString(num, 2, "context_type", ach, aContextType);

  //--- Initialisation of the read entity ---

  ent->Init(aContextIdentifier, aContextType);
}

void RWStepRepr_RWRepresentationContext::WriteStep(
  StepData_StepWriter&                          SW,
  const Handle(StepRepr_RepresentationContext)& ent) const
{

  // --- own field : contextIdentifier ---

  SW.Send(ent->ContextIdentifier());

  // --- own field : contextType ---

  SW.Send(ent->ContextType());
}
