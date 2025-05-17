// Created on : Sat May 02 12:41:14 2020
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#include "RWStepRepr_RWRepresentationReference.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationReference.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationContextReference.hxx>

//=================================================================================================

RWStepRepr_RWRepresentationReference::RWStepRepr_RWRepresentationReference() {}

//=================================================================================================

void RWStepRepr_RWRepresentationReference::ReadStep(
  const Handle(StepData_StepReaderData)&          theData,
  const Standard_Integer                          theNum,
  Handle(Interface_Check)&                        theAch,
  const Handle(StepRepr_RepresentationReference)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 2, theAch, "representation_reference"))
    return;

  // Own fields of RepresentationReference

  Handle(TCollection_HAsciiString) aId;
  theData->ReadString(theNum, 1, "id", theAch, aId);

  Handle(StepRepr_RepresentationContextReference) aContextOfItems;
  theData->ReadEntity(theNum,
                      2,
                      "context_of_items",
                      theAch,
                      STANDARD_TYPE(StepRepr_RepresentationContextReference),
                      aContextOfItems);

  // Initialize entity
  theEnt->Init(aId, aContextOfItems);
}

//=================================================================================================

void RWStepRepr_RWRepresentationReference::WriteStep(
  StepData_StepWriter&                            SW,
  const Handle(StepRepr_RepresentationReference)& theEnt) const
{

  // Own fields of RepresentationReference

  SW.Send(theEnt->Id());

  SW.Send(theEnt->ContextOfItems());
}

//=================================================================================================

void RWStepRepr_RWRepresentationReference::Share(
  const Handle(StepRepr_RepresentationReference)& theEnt,
  Interface_EntityIterator&                       iter) const
{

  // Own fields of RepresentationReference

  iter.AddItem(theEnt->ContextOfItems());
}
